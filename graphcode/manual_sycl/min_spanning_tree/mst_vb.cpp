#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#define DEBUG 0
#include "../graph.h"

using namespace sycl;

int main(int argc, char **argv)
{

    std::chrono::steady_clock::time_point tic_0 = std::chrono::steady_clock::now();
    std::ofstream logfile;

    std::string name = argv[1];
    int NUM_THREADS = atoi(argv[2]);
    std::string NUM_THREADS_STR = std::to_string(NUM_THREADS);

    logfile.open("min_spanning_tree/output/" + name + "_mst_vb_time_" + NUM_THREADS_STR + ".txt");

    logfile << "Processing " << name << std::endl;
    default_selector d_selector;
    queue Q(d_selector);
    logfile << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
    logfile << "Number of parallel work items: " << NUM_THREADS << std::endl;

    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();
    graph *g = malloc_shared<graph>(1, Q);
    g->load_graph(name, Q);
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    logfile << "Time to load data from files: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    int n_vertices = g->get_num_nodes();
    int n_edges = g->get_num_edges();
    int stride = NUM_THREADS;

    int *d_parent = malloc_device<int>(n_vertices, Q);
    copy(d_parent, g->V, NUM_THREADS, n_vertices, Q);
    int *d_local_min_edge = malloc_device<int>(n_vertices, Q);
    int *d_comp_min_edge = malloc_device<int>(n_vertices, Q);
    int *d_edge_in_mst = malloc_device<int>(n_edges, Q);
    initialize(d_edge_in_mst, 0, NUM_THREADS, n_edges, Q);

    bool *rem_comp = malloc_shared<bool>(1, Q);
    int *counter = malloc_shared<int>(1, Q);
    bool *found_min_edge = malloc_shared<bool>(1, Q);
    bool *parents_updated = malloc_shared<bool>(1, Q);

    tic = std::chrono::steady_clock::now();
    logfile << "Starting minimum spanning tree calculation..." << std::endl;

    *rem_comp = false;
    *counter = 0;

    while (!*rem_comp)
    {
        *rem_comp = true;

        initialize(d_local_min_edge, -1, NUM_THREADS, n_vertices, Q);
        initialize(d_comp_min_edge, -1, NUM_THREADS, n_vertices, Q);

        // find minimum edge to different component from each node
        forall(n_vertices, NUM_THREADS)
        {
            int edge;
            for_neighbours(u, edge)
            {
                int v = get_neighbour(edge);
                // if u and v in different components
                if (d_parent[u] != d_parent[v])
                {
                    int curr_min_edge = d_local_min_edge[u];
                    if (curr_min_edge == -1)
                    {
                        d_local_min_edge[u] = edge;
                    }
                    else
                    {
                        int curr_neigh = get_neighbour(curr_min_edge);
                        int curr_edge_weight = get_weight(edge), curr_min_edge_weight = get_weight(curr_min_edge);
                        if (curr_edge_weight < curr_min_edge_weight || (curr_edge_weight == curr_min_edge_weight && d_parent[v] < d_parent[curr_neigh]))
                        {
                            d_local_min_edge[u] = edge;
                        }
                    }
                }
            }
        }
        end;

        // find the minimum edge from the component
        *found_min_edge = false;
        while (!*found_min_edge)
        {
            *found_min_edge = true;
            forall(n_vertices, NUM_THREADS)
            {
                int my_comp = d_parent[u];

                int comp_min_edge = d_comp_min_edge[my_comp];

                int my_min_edge = d_local_min_edge[u];

                if (my_min_edge != -1)
                {
                    int v = get_neighbour(my_min_edge);

                    if (comp_min_edge == -1)
                    {
                        d_comp_min_edge[my_comp] = my_min_edge;
                        *found_min_edge = false;
                    }
                    else
                    {
                        int curr_min_neigh = get_neighbour(comp_min_edge);
                        int my_min_edge_weight = get_weight(my_min_edge), comp_min_edge_weight = get_weight(comp_min_edge);
                        if (my_min_edge_weight < comp_min_edge_weight || (my_min_edge_weight == comp_min_edge_weight && d_parent[v] < d_parent[curr_min_neigh]))
                        {
                            d_comp_min_edge[my_comp] = my_min_edge;
                            *found_min_edge = false;
                        }
                    }
                }
            }
            end;
        }

        // remove cycles of 2 nodes
        forall(n_vertices, NUM_THREADS)
        {
            // if u is the representative of its component
            if (d_parent[u] == get_node(u))
            {
                int comp_min_edge = d_comp_min_edge[u];
                if (comp_min_edge != -1)
                {
                    int v = get_neighbour(comp_min_edge);
                    int parent_v = d_parent[v];

                    int v_comp_min_edge = d_comp_min_edge[parent_v];
                    if (v_comp_min_edge != -1)
                    {
                        // v is comp(u)'s neighbour
                        // w is comp(v)'s neighbour
                        int w = get_neighbour(v_comp_min_edge);
                        if (d_parent[u] == d_parent[w] && d_parent[u] < d_parent[v])
                        {
                            d_comp_min_edge[d_parent[v]] = -1;
                        }
                    }
                }
            }
        }
        end;

        // update the MST edges
        forall(n_vertices, NUM_THREADS)
        {
            // if u is the representative of its component
            if (d_parent[u] == get_node(u))
            {
                int curr_comp_min_edge = d_comp_min_edge[u];
                if (curr_comp_min_edge != -1)
                {
                    d_edge_in_mst[curr_comp_min_edge] = 1;
                    ATOMIC_INT atomic_data(*counter);
                    atomic_data += 1;
                }
            }
        }
        end;

        logfile << "Num Edges: " << *counter << std::endl;

        // update parents
        forall(n_vertices, NUM_THREADS)
        {
            // if u is the representative of its component
            if (d_parent[u] == get_node(u))
            {
                int curr_comp_min_edge = d_comp_min_edge[u];
                if (curr_comp_min_edge != -1)
                {
                    *rem_comp = false;
                    int v = get_neighbour(curr_comp_min_edge);
                    d_parent[u] = d_parent[v];
                }
            }
        }
        end;

        // flatten parents
        *parents_updated = false;
        while (!*parents_updated)
        {
            *parents_updated = true;
            forall(n_vertices, NUM_THREADS)
            {
                int parent_u = d_parent[u];
                int parent_parent_u = d_parent[parent_u];

                if (parent_u != parent_parent_u)
                {
                    *parents_updated = false;
                    d_parent[u] = parent_parent_u;
                }
            }
            end;
        }
    }
    toc = std::chrono::steady_clock::now();
    logfile << "Time to run minimum spanning tree: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;

    resultfile.open("min_spanning_tree/output/" + name + "_mst_vb_result_" + NUM_THREADS_STR + ".txt");

    std::vector<int> op(n_edges);
    memcpy(&op[0], d_edge_in_mst, n_edges, Q);

    int count = 0;
    for (int i = 0; i < n_edges; i++)
    {
        if (op[i] == 1)
            count++;
        resultfile << i << " " << op[i] << std::endl;
    }
    resultfile << "Num edges included in MST: " << count << " Total nodes in graph: " << n_vertices << std::endl;
    resultfile.close();
    toc = std::chrono::steady_clock::now();
    logfile << "Time to write data to file: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    std::chrono::steady_clock::time_point toc_0 = std::chrono::steady_clock::now();
    logfile << "Total time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(toc_0 - tic_0).count() << "[µs]" << std::endl;
    return 0;
}

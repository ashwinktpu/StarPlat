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
    int source = atoi(argv[2]);
    int NUM_THREADS = atoi(argv[3]);
    std::string NUM_THREADS_STR = std::to_string(NUM_THREADS);

    default_selector d_selector;
    queue Q(d_selector);

    logfile.open("single_source_shortest_path/output/" + name + "_sssp_top_time_" + NUM_THREADS_STR + ".txt");

    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

    graph *g = malloc_shared<graph>(1, Q);
    g->load_graph(name, Q);

    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    logfile << "Processing " << g->get_graph_name() << std::endl;
    logfile << "Num nodes: " << g->get_num_nodes() << "\t"
            << "Num edges: " << g->get_num_edges() << std::endl;
    logfile << "Time to load data from files: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;
    logfile << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
    logfile << "Number of parallel work items: " << NUM_THREADS << std::endl;

    int N = g->get_num_nodes();
    int stride = NUM_THREADS;
    std::vector<int> dist(N, INT_MAX);

    int *dev_flag = malloc_device<int>(N, Q);
    initialize(dev_flag, 0, NUM_THREADS, N, Q, source, 1);

    int *dev_dist = malloc_device<int>(N, Q);
    initialize(dev_dist, INT_MAX, NUM_THREADS, N, Q, source, 0);

    int *dev_dist_i = malloc_device<int>(N, Q);
    initialize(dev_dist_i, INT_MAX, NUM_THREADS, N, Q, source, 0);

    tic = std::chrono::steady_clock::now();
    logfile << "Starting SSSP..." << std::endl;
    int *early_stop = malloc_shared<int>(1, Q);

    for (int round = 1; round < N; round++)
    {

        if (*early_stop == 1)
        {
            logfile << "Number of rounds required for convergence: " << round << std::endl;
            break;
        }
        *early_stop = 1;

        forall(N, NUM_THREADS)
        {
            if (dev_flag[u])
            {
                dev_flag[u] = 0;
                int u_itr;
                for_neighbours(u, u_itr)
                {
                    int v = get_neighbour(u_itr);
                    int w = get_weight(u_itr);

                    int du = dev_dist[u];
                    int dv = dev_dist[v];
                    int new_dist = du + w;

                    if (du == INT_MAX)
                    {
                        continue;
                    }
                    ATOMIC_INT atomic_data(dev_dist_i[v]);
                    atomic_data.fetch_min(new_dist);
                }
            }
        }
        end;

        forall(N, NUM_THREADS)
        {
            if (dev_dist[u] > dev_dist_i[u])
            {
                dev_dist[u] = dev_dist_i[u];
                dev_flag[u] = 1;
                *early_stop = 0;
            }
            dev_dist_i[u] = dev_dist[u];
        }
        end;
    }

    toc = std::chrono::steady_clock::now();
    logfile << "Time to run SSSP: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;
    int num_covered = 0;

    memcpy(&dist[0], dev_dist, N, Q);

    resultfile.open("single_source_shortest_path/output/" + name + "_sssp_top_result_" + NUM_THREADS_STR + ".txt");

    for (int i = 0; i < N; i++)
    {
        if (dist[i] != INT_MAX)
        {
            num_covered += 1;
        }
        resultfile << i << " " << dist[i] << std::endl;
    }
    resultfile.close();
    toc = std::chrono::steady_clock::now();
    logfile << "Time to write data to file: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    std::chrono::steady_clock::time_point toc_0 = std::chrono::steady_clock::now();
    logfile << "Total time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(toc_0 - tic_0).count() << "[µs]" << std::endl;
    logfile << "Percentage coverage from given source: " << 100 * (1.0 * num_covered) / N << std::endl
            << "Number of nodes covered: " << num_covered << std::endl;
    g->free_memory(Q);
    return 0;
}

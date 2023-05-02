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

    logfile.open("triangle_count/output/" + name + "_tc_bs_time_" + NUM_THREADS_STR + ".txt");

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

    int N = g->get_num_nodes();
    int stride = NUM_THREADS;

    tic = std::chrono::steady_clock::now();
    logfile << "Starting triangle count..." << std::endl;

    int triangle_count = 0;
    int *dev_triangle_count = malloc_device<int>(1, Q);

    memcpy(dev_triangle_count, &triangle_count, 1, Q);

    forall(N, NUM_THREADS)
    {
        int u_itr1, u_itr2;
        for_neighbours(u, u_itr1)
        {
            int v = get_neighbour(u_itr1);
            if (v < u)
            {
                for_neighbours(u, u_itr2)
                {
                    int w = get_neighbour(u_itr2);
                    int nbrs_connected = 0;
                    if (w > u)
                    {
                        nbrs_connected = neighbours(v, w, g);
                    }
                    if (nbrs_connected)
                    {
                        ATOMIC_INT atomic_data(dev_triangle_count[0]);
                        atomic_data += 1;
                    }
                }
            }
        }
    }
    end;

    toc = std::chrono::steady_clock::now();
    logfile << "Time to run triangle count: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    memcpy(&triangle_count, dev_triangle_count, 1, Q);

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;

    resultfile.open("triangle_count/output/" + name + "_tc_bs_result_" + NUM_THREADS_STR + ".txt");

    resultfile << "Number of triangles in graph =  " << triangle_count << std::endl;

    resultfile.close();
    toc = std::chrono::steady_clock::now();
    logfile << "Time to write data to file: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    std::chrono::steady_clock::time_point toc_0 = std::chrono::steady_clock::now();
    logfile << "Total time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(toc_0 - tic_0).count() << "[µs]" << std::endl;

    return 0;
}

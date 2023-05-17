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

    logfile.open("pagerank/output/" + name + "_pr_pull_time_" + NUM_THREADS_STR + ".txt");

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

    float beta = 0.001;
    int maxIter = 100;
    float delta = 0.85;

    int N = g->get_num_nodes();
    int stride = NUM_THREADS;

    std::vector<float> pagerank(N);
    float *dev_pagerank = malloc_device<float>(N, Q);
    initialize(dev_pagerank, float(1 / N), NUM_THREADS, N, Q);

    float *dev_pagerank_i = malloc_device<float>(N, Q);
    initialize(dev_pagerank_i, float(1 / N), NUM_THREADS, N, Q);

    tic = std::chrono::steady_clock::now();
    logfile << "Starting Pagerank..." << std::endl;
    float *diff = malloc_shared<float>(1, Q);
    int iterCount = 0;
    do
    {
        forall(N, NUM_THREADS)
        {
            float sum = 0;
            // pull
            int u_itr;
            for_parents(u, u_itr)
            {
                int nbr = get_parent(u_itr);
                sum = sum + dev_pagerank[nbr] / (get_num_neighbours(nbr));
            }
            float val = (1 - delta) / N + delta * sum;
            ATOMIC_FLOAT atomic_data(*diff);
            atomic_data += (float)val - dev_pagerank[u];
            dev_pagerank_i[u] = val;
        }
        end;

        forall(N, NUM_THREADS)
        {
            dev_pagerank[u] = dev_pagerank_i[u];
        }
        end;

        iterCount += 1;
    } while ((*diff > beta) && (iterCount < maxIter));

    toc = std::chrono::steady_clock::now();
    logfile << "Time to run Pagerank: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;
    int num_covered = 0;

    memcpy(&pagerank[0], dev_pagerank, N, Q);

    resultfile.open("pagerank/output/" + name + "_pr_pull_result_" + NUM_THREADS_STR + ".txt");

    for (int i = 0; i < N; i++)
    {
        resultfile << i << " " << pagerank[i] << std::endl;
    }
    resultfile.close();
    toc = std::chrono::steady_clock::now();
    logfile << "Time to write data to file: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    std::chrono::steady_clock::time_point toc_0 = std::chrono::steady_clock::now();
    logfile << "Total time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(toc_0 - tic_0).count() << "[µs]" << std::endl;

    return 0;
}

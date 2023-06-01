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
    int sSize = atoi(argv[3]);
    std::string str_sSize = std::to_string(sSize);
    std::vector<int> sourceSet;

    std::ifstream input("betweenness_centrality/sources/" + name + "/" + str_sSize);
    int num;
    while ((input >> num))
    {
        sourceSet.push_back(num);
    }
    input.close();

    std::string NUM_THREADS_STR = std::to_string(NUM_THREADS);

    logfile.open("betweenness_centrality/output/" + name + "_" + str_sSize + "_bc_vp_f_time_" + NUM_THREADS_STR + ".txt");

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

    std::vector<float> bc(N);

    float *dev_bc = malloc_device<float>(bc.size(), Q);
    Q.submit([&](handler &h)
             { h.parallel_for(
                   NUM_THREADS, [=](id<1> v)
                   {
                           for (; v < N; v += stride)
                           {
                               dev_bc[v] = 0;
                           } }); })
        .wait();
    float *dev_delta = malloc_device<float>(N, Q);

    int *dev_d = malloc_device<int>(N, Q);
    int *dev_sigma = malloc_device<int>(N, Q);

    tic = std::chrono::steady_clock::now();
    logfile << "Starting betweenness centrality calculation..." << std::endl;

    int *S = malloc_shared<int>(N, Q);
    int *ends = malloc_shared<int>(N, Q);
    int *position = malloc_shared<int>(1, Q);
    int *s = malloc_shared<int>(1, Q);
    *s = 0;
    int *finish_limit_position = malloc_shared<int>(1, Q);
    int *done = malloc_shared<int>(1, Q);
    int *current_depth = malloc_shared<int>(1, Q);

    for (auto x : sourceSet)
    {
        *s = x;
        initialize(dev_d, INT_MAX, NUM_THREADS, N, Q, *s, 0);
        initialize(dev_sigma, 0, NUM_THREADS, N, Q, *s, 1);
        initialize(dev_delta, float(0), NUM_THREADS, N, Q);

        *current_depth = 0;
        *done = 0;
        *position = 0;
        *finish_limit_position = 1;
        ends[0] = 0;

        while (!*done)
        {
            *done = 1;
            forall(N, NUM_THREADS)
            {
                if (dev_d[u] == *current_depth)
                {
                    ATOMIC_INT atomic_data(*position);
                    int t = atomic_data++;
                    S[t] = u;
                    int r;
                    for_neighbours(u, r)
                    {
                        int w = get_neighbour(r);
                        if (dev_d[w] == INT_MAX)
                        {
                            dev_d[w] = dev_d[u] + 1;
                            *done = 0;
                        }

                        if (dev_d[w] == (dev_d[u] + 1))
                        {
                            ATOMIC_INT atomic_data(dev_sigma[w]);
                            atomic_data += dev_sigma[u];
                        }
                    }
                }
            }
            end;
            *current_depth += 1;
            ends[*finish_limit_position] = *position;
            ++*finish_limit_position;
        }

        for (int itr1 = *finish_limit_position - 1; itr1 >= 0; itr1--)
        {
            Q.submit([&](handler &h)
                     { h.parallel_for(
                           NUM_THREADS, [=](id<1> i)
                           {
                                    for(int itr2 = ends[itr1] + i; itr2 < ends[itr1 + 1]; itr2 += stride){
                                        for(int itr3 = g->I[S[itr2]]; itr3 < g->I[S[itr2] + 1]; itr3++){
                                            int consider = g->E[itr3];
                                            if(dev_d[consider] == dev_d[S[itr2]] + 1){
                                                dev_delta[S[itr2]] += (((float)dev_sigma[S[itr2]] / dev_sigma[consider]) * ((float)1 + dev_delta[consider]));

                                            }
                                        }

                                        if(S[itr2] != *s){
                                            dev_bc[S[itr2]] += dev_delta[S[itr2]];
                                        }
                                    } }); })
                .wait();
        }
    }

    toc = std::chrono::steady_clock::now();
    logfile << "Time to run betweenness centrality: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    memcpy(&bc[0], dev_bc, N, Q);

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;

    resultfile.open("betweenness_centrality/output/" + name + "_" + str_sSize + "_bc_vp_f_result_" + NUM_THREADS_STR + ".txt");

    for (int i = 0; i < N; i++)
    {
        resultfile << i << " " << bc[i] / 2.0 << std::endl;
    }
    resultfile.close();
    toc = std::chrono::steady_clock::now();
    logfile << "Time to write data to file: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    std::chrono::steady_clock::time_point toc_0 = std::chrono::steady_clock::now();
    logfile << "Total time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(toc_0 - tic_0).count() << "[µs]" << std::endl;
    return 0;
}

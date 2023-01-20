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

    logfile.open("betweenness_centrality/output/" + name + "_" + str_sSize + "_bc_we_time_" + NUM_THREADS_STR + ".txt");

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

    int *dev_d = malloc_device<int>(N, Q);
    int *dev_sigma = malloc_device<int>(N, Q);
    float *dev_bc = malloc_device<float>(bc.size(), Q);
    initialize(dev_bc, float(0), NUM_THREADS, N, Q);
    float *dev_delta = malloc_device<float>(N, Q);

    tic = std::chrono::steady_clock::now();
    logfile << "Starting betweenness centrality calculation..." << std::endl;

    int *s = malloc_shared<int>(1, Q);
    *s = 0;

    int *q_curr = malloc_shared<int>(N, Q);
    int *q_curr_len = malloc_shared<int>(1, Q);

    int *q_next = malloc_shared<int>(N, Q);
    int *q_next_len = malloc_shared<int>(1, Q);

    int *S = malloc_shared<int>(N, Q);
    int *S_len = malloc_shared<int>(1, Q);

    int *ends = malloc_shared<int>(N, Q);
    int *ends_len = malloc_shared<int>(1, Q);
    int *depth = malloc_shared<int>(1, Q);

    for (auto x : sourceSet)
    {
        *s = x;

        initialize(dev_d, INT_MAX, NUM_THREADS, N, Q, *s, 0);
        initialize(dev_sigma, 0, NUM_THREADS, N, Q, *s, 1);
        initialize(dev_delta, float(0), NUM_THREADS, N, Q);

        q_curr[0] = *s;
        *q_curr_len = 1;
        *q_next_len = 0;
        S[0] = *s;
        *S_len = 1;
        ends[0] = 0;
        ends[1] = 1;
        *ends_len = 2;
        *depth = 0;

        while (1)
        {
            forall(*q_curr_len, NUM_THREADS)
            {
                int v = q_curr[u], v_itr;
                for_neighbours(v, v_itr)
                {
                    int w = get_neighbour(v_itr);

                    ATOMIC_INT atomic_data(dev_d[w]);
                    int old = INT_MAX;
                    old = atomic_data.compare_exchange_strong(old, dev_d[v] + 1);
                    if (old)
                    {
                        ATOMIC_INT atomic_data(*q_next_len);
                        int t = atomic_data++;
                        q_next[t] = w;
                    }

                    if (dev_d[w] == dev_d[v] + 1)
                    {
                        ATOMIC_INT atomic_data(dev_sigma[w]);
                        atomic_data += dev_sigma[v];
                    }
                }
            }
            end;

            if (*q_next_len == 0)
            {
                break;
            }
            else
            {
                forall(*q_next_len, NUM_THREADS)
                {
                    q_curr[u] = q_next[u];
                    S[u + *S_len] = q_next[u];
                }
                end;

                ends[*ends_len] = ends[*ends_len - 1] + *q_next_len;
                *ends_len += 1;
                int new_curr_len = *q_next_len;
                *q_curr_len = new_curr_len;
                *S_len += new_curr_len;
                *q_next_len = 0;
                *depth += 1;
            }
        }
        while (*depth >= 0)
        {
            forall(ends[*depth + 1] - ends[*depth], NUM_THREADS)
            {
                int tid = u + ends[*depth];
                int w = S[tid];
                int dsw = 0, r;
                for_neighbours(w, r)
                {
                    int v = get_neighbour(r);
                    if (dev_d[v] == dev_d[w] + 1)
                    {
                        dev_delta[w] += (((float)dev_sigma[w] / dev_sigma[v]) * ((float)1 + dev_delta[v]));
                    }
                }
                if (w != *s)
                {
                    dev_bc[w] += dev_delta[w];
                }
            }
            end;

            *depth -= 1;
        }
    }

    logfile << std::endl;
    toc = std::chrono::steady_clock::now();
    logfile << "Time to run betweenness centrality: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    memcpy(&bc[0], dev_bc, N, Q);

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;

    resultfile.open("betweenness_centrality/output/" + name + "_" + str_sSize + "_bc_we_result_" + NUM_THREADS_STR + ".txt");

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

#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#define DEBUG 0

using namespace sycl;

int main(int argc, char **argv)
{

    std::chrono::steady_clock::time_point tic_0 = std::chrono::steady_clock::now();
    std::ofstream logfile;

    std::string name = argv[1];
    int NUM_THREADS = atoi(argv[2]);
    std::string NUM_THREADS_STR = std::to_string(NUM_THREADS);

    logfile.open("triangle_count/output/" + name + "_tc_lin_time_" + NUM_THREADS_STR + ".txt");

    logfile << "Processing " << name << std::endl;

    default_selector d_selector;
    queue Q(d_selector);
    logfile << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
    logfile << "Number of parallel work items: " << NUM_THREADS << std::endl;

    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();
    std::vector<int> V, I, E, RE, RI;
    load_from_file("csr_graphs/" + name + "/V", V);
    load_from_file("csr_graphs/" + name + "/I", I);
    load_from_file("csr_graphs/" + name + "/E", E);

    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    logfile << "Time to load data from files: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    int *dev_V = malloc_device<int>(V.size(), Q);
    int *dev_I = malloc_device<int>(I.size(), Q);
    int *dev_E = malloc_device<int>(E.size(), Q);

    Q.submit([&](handler &h)
             { h.memcpy(dev_V, &V[0], V.size() * sizeof(int)); });

    Q.submit([&](handler &h)
             { h.memcpy(dev_I, &I[0], I.size() * sizeof(int)); });

    Q.submit([&](handler &h)
             { h.memcpy(dev_E, &E[0], E.size() * sizeof(int)); });

    Q.wait();

    int N = V.size();
    int stride = NUM_THREADS;

    tic = std::chrono::steady_clock::now();
    logfile << "Starting triangle count..." << std::endl;
    int triangle_count = 0;
    logfile << "H1" << std::endl;
    int *dev_triangle_count = malloc_device<int>(1, Q);
    logfile << "H2" << std::endl;
    Q.submit([&](handler &h)
             { h.memcpy(dev_triangle_count, &triangle_count, sizeof(int)); })
        .wait();
    logfile << "H3" << std::endl;

    Q.submit([&](handler &h)
             { h.parallel_for(
                   NUM_THREADS, [=](id<1> i)
                   {
                            for (; i < N; i += stride)
                            {
                                for (int edge1 = dev_I[i]; edge1 < dev_I[i + 1]; edge1++)
                                {
                                    int u = dev_E[edge1];
                                    if(u < i){
                                        for (int edge2 = dev_I[i]; edge2 < dev_I[i + 1]; edge2++){
                                            int w = dev_E[edge2];
                                            if(w > i){
                                                int nbrs_connected = 0;
                                                for (int edge = dev_I[u]; edge < dev_I[u + 1]; edge++){
                                                    int nbr = dev_E[edge];
                                                    if(nbr == w){
                                                         nbrs_connected = 1;
                                                         break;
                                                    }
                                                }
                                                 if(nbrs_connected){
                                                     atomic_ref<int, memory_order::seq_cst, memory_scope::device, access::address_space::global_space> atomic_data(*dev_triangle_count);
                                                     atomic_data+=1;
                                                }
                                            }
                                        }
                                    }
                                }
} }); })
        .wait();

    toc = std::chrono::steady_clock::now();
    logfile << "Time to run triangle count: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    Q.submit([&](handler &h)
             { h.memcpy(&triangle_count, dev_triangle_count, sizeof(int)); })
        .wait();

    tic = std::chrono::steady_clock::now();
    std::ofstream resultfile;

    resultfile.open("triangle_count/output/" + name + "_tc_lin_result_" + NUM_THREADS_STR + ".txt");

    resultfile << "Number of triangles in graph =  " << triangle_count << std::endl;

    resultfile.close();
    toc = std::chrono::steady_clock::now();
    logfile << "Time to write data to file: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    std::chrono::steady_clock::time_point toc_0 = std::chrono::steady_clock::now();
    logfile << "Total time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(toc_0 - tic_0).count() << "[µs]" << std::endl;

    return 0;
}

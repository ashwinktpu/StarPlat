// metis_integration.cpp
#include <iostream>
#include <metis.h>
#include <chrono>
#include <filesystem>
#include "metis_integration.h"

std::vector<int> partitionGraph(std::vector<int>& xadj, std::vector<int>& adjncy, std::vector<int>& weights, int numPartitions) {
    // Placeholder for METIS integration
    int V = xadj.size () ;
    V-- ;
    int num_cons = 1 ;
    idx_t *nvtxs=&V;
    idx_t *ncon=&num_cons;
    idx_t *vwgt=nullptr;
    idx_t *vsize=nullptr;
    idx_t *nparts=&numPartitions;
    real_t *ubvec=nullptr;
    real_t *tpwgts=nullptr;
    idx_t objval;
    int* part=new int[V];
    idx_t options[METIS_NOPTIONS];

     METIS_SetDefaultOptions(options);
    // options[METIS_OPTION_MINCONN]=1;
    options[METIS_OPTION_OBJTYPE]=METIS_OBJTYPE_CUT;
    // options[METIS_OPTION_IPTYPE]=METIS_IPTYPE_EDGE;

    std::cout << "num vertices " << V << std::endl ;

    std::cout<<"starting graph partitioning\n";
    auto t1 = std::chrono::high_resolution_clock::now();

    int ret = METIS_PartGraphKway(&V, &num_cons, xadj.data (), adjncy.data(),
                        NULL, NULL, NULL, nparts, NULL,
                        NULL, options, &objval, part);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration <double, std::milli> ms_double = t2-t1;
    std::cout << ms_double.count()<<"ms\n";
    std:: cout << ret << std::endl;
    std::vector<int> partitionResult (part, part+V); // Replace with actual METIS result

    return partitionResult;
}

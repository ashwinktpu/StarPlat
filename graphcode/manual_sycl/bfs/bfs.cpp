#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"

using namespace sycl;
using namespace std;


void BFS(graph &g, int *h_level, int src)
{
    queue Q(gpu_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;
    
    // CSR BEGIN
    int V = g.num_nodes();
    int E = g.num_edges();

    printf("Number of nodes:%d\n", V);
    printf("Number of edges:%d\n", E);
    int *edgeLen = g.getEdgeLen();
    
    //Host Memory Allocation
    int *h_rowPtr;
    int *h_colIndices;
   

    h_rowPtr = (int *)malloc((V + 1) * sizeof(int));
    h_colIndices = (int *)malloc((E) * sizeof(int));
  

    for (int i = 0; i <= V; i++)
    {   
        h_rowPtr[i] = g.indexofNodes[i];
    }
    
    for (int i = 0; i < E; i++)
    {
        h_colIndices[i] = g.edgeList[i];
    }
    
    //Device Memory ALlocation
    int *d_rowPtr;
    int *d_colIndices;
    int *d_level;
    bool *d_modified_next;
    

    d_rowPtr = malloc_device<int>((V+1), Q);
    d_colIndices = malloc_device<int>((E), Q);
    d_level = malloc_device<int>(V, Q);
    d_modified_next = malloc_device<bool>(V, Q);
    

    //<--  direction of copying data
    Q.submit([&](handler &h)
             { h.memcpy(d_rowPtr, h_rowPtr, sizeof(int) * (V + 1)); })     
        .wait();
    Q.submit([&](handler &h)
             { h.memcpy(d_colIndices, h_colIndices, sizeof(int) * (E)); })
        .wait();
  
    
    // CSR END
    // LAUNCH CONFIG
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;

    // TIMER START
    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_level[i] = (int)INT_MAX; }); })
        .wait();
    
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_level[src] = (int)0; }); })
        .wait(); 

    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_modified_next[i] = false; }); })
        .wait();
    
    
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_modified_next[src] = (bool)true; }); })
        .wait(); 

   
    bool finished = false;
    int levelnumber=1;
    while (!finished)
    {

        finished = true;
        bool *d_finished = malloc_device<bool>(1, Q);
        int *d_levelNumber = malloc_device<int>(1, Q);
        Q.submit([&](handler &h)
                 { h.memcpy(d_finished, &finished, 1 * sizeof(bool)); })
            .wait();
        Q.submit([&](handler &h)
                 { h.memcpy(d_levelNumber, &levelnumber, 1 * sizeof(int)); })
            .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                  {
                                      for (; v < V; v += NUM_THREADS)
                                      { // BEGIN KER FUN via ADDKERNEL
                                        
                                          if (d_modified_next[v] == true)
                                         {   
                                             d_modified_next[v] =false;
                                             for (int edge = d_rowPtr[v]; edge < d_rowPtr[v+1]; edge++) 
                                             {  
                                                 
                                                int nbr = d_colIndices[edge];
                                                if(d_level[nbr]==INT_MAX)
                                                {   
                                                    d_level[nbr]=*d_levelNumber;
                                                    d_modified_next[nbr] = true;;
                                                   *d_finished = false ;
                                                }

                                              } 

                                         } 
                                      }
                               });      
                 
                 })
            .wait(); 

        Q.submit([&](handler &h)
                 { h.memcpy(&finished, d_finished, 1 * sizeof(bool)); })
            .wait();
      
        levelnumber=levelnumber+1;
     } 
   

    // TIMER STOP
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
   
    Q.submit([&](handler &h)
             { h.memcpy(h_level,d_level, sizeof(int) * (V)); })
        .wait();
    
    int count=0;
    for (int i = 0; i < V; i++)
    { //std::cout << i << " " << h_level[i] << std::endl;
      if(h_level[i]!=INT_MAX)
          count++;
    }
    
     std::cout<<"Count of Nodes reachable from source "<<count<<std::endl;
     std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;

} 


int main(int argc, char** argv)
{   
    char* inp = argv[1];
	std::cout << "Taking input from: " << inp << std::endl;
	graph G1(inp);
	G1.parseGraph();
    
    int src = 0;
    int *h_level = (int *)malloc(G1.num_nodes() * sizeof(int));
    BFS(G1,h_level, src);
    return 0;
}






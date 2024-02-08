#include <iostream>
#include <vector>
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"

using namespace sycl;
using namespace std;

int main(int argc, char** argv) 
{
    
    queue Q(gpu_selector_v);
    
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;
    
    char* inp = argv[1];
	std::cout << "Taking input from: " << inp << std::endl;
	graph g(inp);
	g.parseGraph();
    int V = g.num_nodes();
    int E = g.num_edges();

    printf("Number of nodes:%d\n", V);
    printf("Number of edges:%d\n", E);

    //Host Memory Allocation
    int *h_active= (int *)malloc((V) * sizeof(int));
    int *h_rowPtr=(int *)malloc((V + 1) * sizeof(int));;
    int *h_colIndices=(int *)malloc((E) * sizeof(int));;
    int *h_cdegree=(int *)malloc((V) * sizeof(int));
    int *h_ndegree=(int *)malloc((V) * sizeof(int));
    
    // TIMER START
  
    for(int i=0;i<V;i++)
    {
        h_active[i]=1;
        h_rowPtr[i] = g.indexofNodes[i];
        h_cdegree[i]=0;
        h_ndegree[i]=0;
    }
     h_rowPtr[V] = g.indexofNodes[V];
         
    for (int i = 0; i < E; i++)
    {  
        int temp=g.edgeList[i];
        h_colIndices[i] = temp;
        h_cdegree[temp]=h_cdegree[temp]+1;
        h_ndegree[temp]=h_ndegree[temp]+1;
    }
    
    
    int k=3;
    
     int mindegree=INT_MAX;
    for(int i=0;i<V;i++)
        if(mindegree>h_cdegree[i])
            mindegree=h_cdegree[i];
    std::cout<<mindegree<<std::endl;
    
    
    
    int  *d_rowPtr;
    int  *d_colIndices;
    int  *d_active;
    int *d_cdegree;
    int  *d_ndegree;

    d_rowPtr = malloc_device<int>((V+1), Q);
    d_colIndices = malloc_device<int>((E), Q);
    d_active = malloc_device<int>((V), Q);
    d_cdegree = malloc_device<int>((V), Q);
    d_ndegree = malloc_device<int>((V), Q);
    
    Q.submit([&](handler &h)
             { h.memcpy(d_rowPtr, h_rowPtr, sizeof(int) * (V + 1)); })     
        .wait();
    Q.submit([&](handler &h)
             { h.memcpy(d_colIndices, h_colIndices, sizeof(int) * (E)); })     
        .wait();
    Q.submit([&](handler &h)
             { h.memcpy(d_active, h_active, sizeof(int) * (V)); })     
        .wait();
    Q.submit([&](handler &h)
             { h.memcpy(d_cdegree, h_cdegree, sizeof(int) * (V)); })     
        .wait();
    Q.submit([&](handler &h)
             { h.memcpy(d_ndegree, h_ndegree, sizeof(int) * (V)); })     
        .wait();
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;
    
    int decrementValue = 1;
    int change=1;
    bool finished=false;
    
    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();
    while(!finished)
    {       finished = true;
            bool *d_finished = malloc_device<bool>(1, Q);
            Q.submit([&](handler &h)
                 { h.memcpy(d_finished, &finished, 1 * sizeof(bool)); })
            .wait();
              
        
              Q.submit([&](handler &h)
                             { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                  {
                                      for (; v < V; v += NUM_THREADS)
                                      { 
                                          
                                         if(d_cdegree[v]<k && d_active[v]==1)
                                         {   d_active[v]=0;
                                             for(int edge=d_rowPtr[v];edge<d_rowPtr[v+1];edge++)
                                             {   
                                    atomic_ref<int,sycl::memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_ndegree[d_colIndices[edge]]);
                                                atomic_data.fetch_sub(1);
                                             
                                                 if(d_ndegree[d_colIndices[edge]]<k)
                                                 {
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
                
             Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                {
                   for (; i < V; i += stride) 
                   {
                       d_cdegree[i] = d_ndegree[i];
                       //std::cout<<d_cdegree[i]<<" ";
                   }
                
                }); 
             })
            .wait();
           
    }
    
    Q.submit([&](handler &h)
             { h.memcpy(h_active, d_active, sizeof(int) * (V)); })
        .wait();
    
    Q.submit([&](handler &h)
             { h.memcpy(h_cdegree,d_cdegree, sizeof(int) * (V)); })
        .wait();
    
    int count=0;
    for(int i=0;i<V;i++)
        if(h_active[i]==1)
        {
            //std::cout<<i<<" "<<std::endl;
            count=count+1;
        }
    
    
     //std::cout<<count1<<std::endl;
     std::cout<<count<<std::endl;
     std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
     std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;
    
    return 0;
}




    
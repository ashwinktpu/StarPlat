#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include <atomic>
#include <map>
#include "graph.hpp"

class PushRebel;

using namespace sycl;

void bfs(queue &Q, int src, int *d_rowPtr, int *d_colIndices, int *d_removed_vertex, int V, int E){
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride){ 
        if(d_removed_vertex[i] == -1)
            d_removed_vertex[i] = (int)2;
    }
    }); })
        .wait();
    
    int *d_level;
    bool *d_modified_next;
    
    d_level = malloc_device<int>(V, Q);
    d_modified_next = malloc_device<bool>(V, Q);

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
    bool *d_finished = malloc_device<bool>(1, Q);

    while (!finished)
    {

        finished = true;
        Q.submit([&](handler &h)
                 { h.memcpy(d_finished, &finished, 1 * sizeof(bool)); })
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
                                                if(d_level[nbr]==INT_MAX && d_removed_vertex[nbr]!=1)
                                                {   
                                                    d_level[nbr]=(int)1;
                                                    d_removed_vertex[nbr] = (int)-1;
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
     }
    
    Q.submit([&](handler &h)
        { h.parallel_for(NUM_THREADS, [=](id<1> i) {
    for (; i < V; i += stride){ 
        if(d_removed_vertex[i] == (int)2)
            d_removed_vertex[i] = (int)1;
    }
    }); })
        .wait();
    Q.submit([&](handler &h)
                 { h.single_task([=]()
                                 { d_removed_vertex[src] = (int)-1; }); })
            .wait(); 


    free(d_level, Q);
    free(d_modified_next, Q);
    free(d_finished, Q);
}

void pushRebeal(queue &Q, int src, int sink, int *d_rowPtr, int *d_colIndices, int *d_removed_vertex, int *d_height, int *d_flow, int *d_weight, int *d_rev_edge_id, int V, int E){
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;
    
    bool *d_modified_next = malloc_device<bool>(V, Q);
    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_height[i] = (int)INT_MAX; }); })
        .wait();
    
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { 
                            d_height[sink] = (int)0; }); })
        .wait(); 
    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_modified_next[i] = false; }); })
        .wait();
    
    
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_modified_next[sink] = (bool)true; }); })
        .wait(); 
    
    bool *d_finished = malloc_device<bool>(1, Q);
    int *d_levelNumber = malloc_device<int>(1, Q);
    
    bool finished = false;
    int levelNumber = 0;
    
    while (!finished)
    {

        finished = true;
        bool *d_finished = malloc_device<bool>(1, Q);
        int *d_levelNumber = malloc_device<int>(1, Q);
        Q.submit([&](handler &h)
                 { h.memcpy(d_finished, &finished, 1 * sizeof(bool)); })
            .wait();
        Q.submit([&](handler &h)
                 { h.memcpy(d_levelNumber, &levelNumber, 1 * sizeof(int)); })
            .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                  {
                                      for (; v < V; v += NUM_THREADS)
                                      { // BEGIN KER FUN via ADDKERNEL
                                        
                                          if (d_modified_next[v] == true)
                                         {   
                                             d_modified_next[v] =false;
                                             for (int rev_edge = d_rowPtr[v]; rev_edge < d_rowPtr[v+1]; rev_edge++) 
                                             {  
                                                int nbr = d_colIndices[rev_edge];
                                                int edge = d_rev_edge_id[rev_edge];
                                                if(d_height[nbr]==INT_MAX && d_removed_vertex[nbr]!=1 && ((d_weight[edge]-d_flow[edge])>0) )
                                                {
                                                    
                                                    d_height[nbr]=d_height[v]+1;
                                                    d_modified_next[nbr] = true;
                                                    *d_finished = false ;
                                                }

                                              } 

                                         } 
                                      }
                               });      
                 
                 })
            .wait(); 
        
//         int *h_modified_next = (int *)malloc((V + 1) * sizeof(int));
//         Q.submit([&](handler &h)
//                  { h.memcpy(h_modified_next, d_modified_next, 1 * sizeof(bool)); })
//             .wait();
//         int s = 0;
//         for(int i=0; i<V; i++){
//             if(h_modified_next[i])
//                 s++;
//         }
//         std::cout << "size--> " << s << std::endl;

        Q.submit([&](handler &h)
                 { h.memcpy(&finished, d_finished, 1 * sizeof(bool)); })
            .wait();
      
        levelNumber=levelNumber+1;
     } 

     std::cout << "Level ---> " <<  levelNumber << std::endl;

   

     Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_height[src] = levelNumber+1; }); })
        .wait(); 
    
    free(d_modified_next, Q);
    free(d_finished, Q);
    free(d_levelNumber, Q);
}

void Compute_maxflow(graph &g, int src, int sink, long long *f){
    queue Q(default_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    
    int V = g.num_nodes();
    int E = g.num_edges();
    
    std::cout << V << " " << E << std::endl;
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;

    int *edgeLen = g.getEdgeLen();
    
    // Declaring all the host variables
    int *h_vertex; // storing the offset list
    int *h_edges; // storing the csr list
    int *h_weight; // storing the weights of edges

    int *h_rev_vertex; // storing the reverse csr list
    int *h_rev_edges; // storing the reverse offset list
    int *h_rev_edge_id; // storing rev_csr to csr map
    
    int *h_flow; // storing the flow
    int *h_height; // storing the flow
    long long *h_excessflow; // storing the reverse flow
    
    int *h_inDegree;
    int *h_outDegree;
    int *h_removed_vertex;
    
    bool flag = true;
    int *h_tmp;

    // Initializing all host variables
    h_vertex = (int *)malloc((V + 1) * sizeof(int));
    h_edges = (int *)malloc(E * sizeof(int));
    h_weight = (int *)malloc(E * sizeof(int));

    h_rev_vertex = (int *)malloc((V + 1) * sizeof(int));
    h_rev_edges = (int *)malloc(E * sizeof(int));
    h_rev_edge_id = (int *)malloc(E * sizeof(int));

    h_flow = (int *)malloc(E * sizeof(int));
    h_height = (int *)malloc((V + 1) * sizeof(int));
    h_excessflow = (long long *)malloc((V + 1) * sizeof(long long));

    h_removed_vertex = (int *)malloc((V+1) * sizeof(int));
    
    h_tmp = (int *)malloc(E * sizeof(int));
    
    // Storing the graph values in the respective host variables
    for (int i = 0; i <= V; i++)
    {
        int temp = g.indexofNodes[i];
        h_vertex[i] = temp;

        temp = g.rev_indexofNodes[i];
        h_rev_vertex[i] = temp;

        h_height[i] = 0;
        h_excessflow[i] = 0;
        h_removed_vertex[i] = 0;
    }
    h_height[src] = V;
    
    int onesum = 0;
    for (int i = 0; i < E; i++)
    {
        int temp = g.edgeList[i];
        h_edges[i] = temp;
        
        temp = g.srcList[i];
        h_rev_edges[i] = temp;
        
        temp = g.edgeMap[i];
        h_rev_edge_id[i] = temp;

        temp = edgeLen[i];
        h_weight[i] = temp;
        
        h_flow[i] = 0;
        h_tmp[i] = 0;
    }
    
    std::cout << h_rev_vertex[sink] << " " << h_rev_vertex[sink+1] << " " << h_rev_vertex[sink]-h_rev_vertex[sink+1] << std::endl;

    // Declaring device variables for the respective host variables
    int *d_vertex;
    int *d_edges;
    int *d_weight;

    int *d_rev_vertex;
    int *d_rev_edges;
    int *d_rev_edge_id;

    int *d_flow;
    int *d_height;
    long long *d_excessflow;
    
    int *d_inDegree;
    int *d_outDegree;
    int *d_removed_vertex;
    
    bool *d_flag;
    int *d_tmp;

    int *d_queue;

    // Initializing device variables
    d_vertex = malloc_device<int>((V + 1), Q);
    d_edges = malloc_device<int>((E), Q);
    d_weight = malloc_device<int>((E), Q);

    d_rev_vertex = malloc_device<int>((V + 1), Q);
    d_rev_edges = malloc_device<int>(E, Q);
    d_rev_edge_id = malloc_device<int>(E, Q);
        
    d_flow = malloc_device<int>(E, Q);
    d_height = malloc_device<int>((V + 1), Q);
    d_excessflow = malloc_device<long long>((V + 1), Q);

    d_removed_vertex = malloc_device<int>((V + 1), Q);
    d_queue = malloc_device<int>((2*V), Q);
    
    d_flag = malloc_device<bool>(1, Q);
    d_tmp = malloc_device<int>(E, Q);
    
    // Trasferring values from CPU to GPU memory
    Q.submit([&](handler &h)
            { h.memcpy(d_vertex, h_vertex, sizeof(int) * (V + 1));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_edges, h_edges, sizeof(int) * (E));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_weight, h_weight, sizeof(int) * (E));})
        .wait();
    
    Q.submit([&](handler &h)
            { h.memcpy(d_rev_vertex, h_rev_vertex, sizeof(int) * (V + 1));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_rev_edges, h_rev_edges, sizeof(int) * (E));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_rev_edge_id, h_rev_edge_id, sizeof(int) * (E));})
        .wait();

    
    Q.submit([&](handler &h)
            { h.memcpy(d_flow, h_flow, sizeof(int) * (E));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_height, h_height, sizeof(int) * (V+1));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_excessflow, h_excessflow, sizeof(long long) * (V+1));})
        .wait();
    
    Q.submit([&](handler &h)
            { h.memcpy(d_tmp, h_tmp, sizeof(int) * (E));})
        .wait();
    
    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_removed_vertex[i] = -1; }); })
        .wait();
    
//     Start recording the timings
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    
    //Trim
    bfs(Q, src, d_vertex, d_edges, d_removed_vertex, V, E);
    bfs(Q, sink, d_rev_vertex, d_rev_edges, d_removed_vertex, V, E);
    
    Q.submit([&](handler &h)
            { h.memcpy(h_removed_vertex, d_removed_vertex, sizeof(int) * (V));})
        .wait();
    
    int s = 0;
    for(int i=0; i<V; i++){
        if(h_removed_vertex[i]==-1)
            s++;
    }
    std::cout << "Start " << s << std::endl;
    
//     initialize

    int *d_curr_size = malloc_device<int>(1, Q);
    int *d_size = malloc_device<int>(1, Q);
    
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { *d_size = (int)0; }); })
        .wait(); 

    int t = (h_vertex[src+1] - h_vertex[src]);
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> id){
        for (; id < t; id += stride){
            int i = id + d_vertex[src];
            int nbr = d_edges[i];
            if(d_removed_vertex[nbr]==-1){

                atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(*d_size);
                int index = atomic_data.fetch_add(1);
                d_queue[index] = nbr;
                
                d_excessflow[nbr] = d_weight[i];
                d_flow[i] = d_weight[i];
            }
        }
    }); })
        .wait();

    pushRebeal(Q, src, sink, d_rev_vertex, d_rev_edges, d_removed_vertex, d_height, d_flow, d_weight, d_rev_edge_id, V, E);
    // Q.submit([&](handler &h)
    //         { h.memcpy(h_height, d_height, sizeof(int) * (V));})
    //     .wait();
    
    // for(int i=0; i<V; i++)
    //     std::cout << h_height[i] << " ";
    // std::cout << std::endl;
    
    // Q.submit([&](handler &h)
    //          { h.single_task([=]()
    //                          { d_height[src] = s; }); })
    //     .wait(); 


    int *d_taken = malloc_device<int>(V, Q);
    
    int it = 0;
    while(flag){
        it++;
        Q.submit([&](handler &h)
            { h.memcpy(h_excessflow, d_excessflow, sizeof(long long) * (V+1));})
        .wait();
        
        std::cout << "ExcessFlow--> ";
        int s = 0;
        for(int i=0; i<V; i++){
            if(h_excessflow[i]>0 && i!=src && i!=sink){
//                 std::cout << i << " ";
                s++;
            }
        }
        std::cout << s << std::endl;
        
        // std::cout << "ExcessFlow--> ";
        // for(int i=0; i<V; i++)
        //     std::cout << h_excessflow[i] << " ";
        // std::cout << std::endl;
        
        
        // int size = 0;
        // Q.submit([&](handler &h)
        //     { h.memcpy(&size, d_size, sizeof(int));})
        // .wait();
        // int *h_queue = (int *)malloc((V+1) * sizeof(int));
        // Q.submit([&](handler &h)
        //     { h.memcpy(h_queue, d_queue, sizeof(int) * V);})
        // .wait();
        
        // for(int i=0; i<size; i++)
        //     std::cout << h_queue[i] << "  ";
        // std::cout << std::endl;
        
//         Q.submit([&](handler &h)
//             { h.memcpy(h_height, d_height, sizeof(int) * (V+1));})
//         .wait();
        
//         std::cout << "height--> ";
//         for(int i=0; i<V; i++)
//             std::cout << h_height[i] << " ";
//         std::cout << std::endl;
        
//         Q.submit([&](handler &h)
//             { h.memcpy(h_flow, d_flow, sizeof(int) * (E));})
//         .wait();
        
//         std::cout << "Flow--> ";
//         for(int i=0; i<E; i++)
//             std::cout << h_flow[i] << " ";
//         std::cout << std::endl;
        
//         std::cout << it << std::endl;        
        
        t = (h_rev_vertex[sink+1]-h_rev_vertex[sink]);
        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> id){
            for (; id < t; id += stride){
                int i = d_rev_edge_id[id + d_rev_vertex[sink]];
                if(d_flow[i] != d_weight[i]){
                    d_flag[0] = false;
                }
            }
        }); })
            .wait();
        
        Q.submit([&](handler &h)
            { h.memcpy(&flag, d_flag, sizeof(bool));})
        .wait();
        
//         std::cout << std::endl;
        
        if(flag)
            break;
        
        Q.submit([&](handler &h)
             { h.single_task([=]()
                             { *d_curr_size = (int)0; }); })
        .wait(); 

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> id){
            for (; id < V; id += stride){
                d_taken[id] = (int)0;
            }
        }); })
            .wait();
        
//         flag = false;
//         Q.submit([&](handler &h)
//             { h.memcpy(d_flag, &flag, sizeof(bool));})
//         .wait();

        Q.submit([&](handler &h)
             { h.single_task([=]()
                             { *d_curr_size = (int)0; }); })
        .wait(); 
        
        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> index){
                for (; index < *d_size; index += stride){
                    int id = d_queue[index];
                    if(d_excessflow[id] > 0 && d_removed_vertex[id]==-1 && id!=sink && id!=src){
                        d_flag[0] = true;

                        //forward edges
                        int start = d_vertex[id];
                        int end = d_vertex[id+1];

                        int mn = INT_MAX;
                        int mnNode = -1;
                        int flow = -1;
                        int mnId = -1;

                        for(int i=start; i<end; i++){
                            int node = d_edges[i];
                            if(d_removed_vertex[node]==-1 && (d_weight[i]-d_flow[i])>0 && mn>d_height[node]){
                                mn = d_height[node];
                                mnNode = node;
                                flow = (d_weight[i]-d_flow[i]);
                                mnId = i;
                            }
                        }

                        if(mn < d_height[id]){                        
                            int tmp;
                            if(d_excessflow[id] < flow)
                                tmp = d_excessflow[id];
                            else
                                tmp = flow;

                            atomic_ref<long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data1(d_excessflow[id]);
                            atomic_data1.fetch_add(-1*tmp);
                            atomic_ref<long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data2(d_excessflow[mnNode]);
                            atomic_data2.fetch_add(tmp);
                            
                            if(d_excessflow[id]!=0){
                                atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_test1(d_taken[id]);
                                bool add_flag = atomic_test1.fetch_add(1);
                                if(add_flag==0){
                                    atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_size(*d_curr_size);
                                    int add_index = atomic_size.fetch_add(1);
                                    d_queue[V + add_index] = id;
                                }
                            }

                            if(d_excessflow[mnNode]!=0){
                                atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_test2(d_taken[mnNode]);
                                bool add_flag = atomic_test2.fetch_add(1);
                                if(add_flag==0){
                                    atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_size(*d_curr_size);
                                    int add_index = atomic_size.fetch_add(1);
                                    d_queue[V + add_index] = mnNode;
                                }
                            }

                            d_flow[mnId] += tmp;
                            return ;
                        }

    //                     d_tmp[id] = 1;

    //                     residual edges
                        start = d_rev_vertex[id];
                        end = d_rev_vertex[id+1];

                        for(int i=start; i<end; i++){
                            int node = d_rev_edges[i];
                            int edgeId = d_rev_edge_id[i];
                            if(d_removed_vertex[node]==-1 && mn > d_height[node] && d_flow[edgeId]>0){
                                mn = d_height[node];
                                mnNode = node;
                                flow = d_flow[edgeId];
                                mnId = edgeId;
                            }
                        }

                        if(mn < d_height[id]){
                            int tmp;
                            if(d_excessflow[id] < flow)
                                tmp = d_excessflow[id];
                            else
                                tmp = flow;

                            atomic_ref<long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data1(d_excessflow[id]);
                            atomic_data1.fetch_add(-1*tmp);
                            atomic_ref<long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data2(d_excessflow[mnNode]);
                            atomic_data2.fetch_add(tmp);

                            if(d_excessflow[id]!=0){
                                atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_test1(d_taken[id]);
                                bool add_flag = atomic_test1.fetch_add(1);
                                if(add_flag==0){
                                    atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_size(*d_curr_size);
                                    int add_index = atomic_size.fetch_add(1);
                                    d_queue[V + add_index] = id;
                                }
                            }

                            if(d_excessflow[mnNode]!=0){
                                atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_test2(d_taken[mnNode]);
                                bool add_flag = atomic_test2.fetch_add(1);
                                if(add_flag==0){
                                    atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_size(*d_curr_size);
                                    int add_index = atomic_size.fetch_add(1);
                                    d_queue[V + add_index] = mnNode;
                                }
                            }

                            d_flow[mnId] -= tmp;
                            return ;
                        }

                        if(mn==INT_MAX)
                            d_height[id]++;
                        else
                            d_height[id] = mn+1;
                    }
                }
            });
        });
        Q.wait_and_throw();

        Q.submit([&](handler &h)
            { h.memcpy(d_size, d_curr_size, sizeof(bool));})
        .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> id){
            for (; id < *d_curr_size; id += stride){
                d_queue[id] = d_queue[V + id];
            }
        }); })
            .wait();
        
        Q.submit([&](handler &h)
            { h.memcpy(&flag, d_flag, sizeof(bool));})
        .wait();
        
        if(it%10==0)
            pushRebeal(Q, src, sink, d_rev_vertex, d_rev_edges, d_removed_vertex, d_height, d_flow, d_weight, d_rev_edge_id, V, E);
    }
    
    std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
    std::cout << "GPU Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " milliseconds." << std::endl;
    
    std::cout << "iteration--> " << it << std::endl;
    Q.submit([&](handler &h)
        { h.memcpy(h_excessflow, d_excessflow, sizeof(long long) * (V+1));})
    .wait();
    
    *f = h_excessflow[sink];
}

int main(int argc, char **argv){
    graph G1(argv[1]);
    G1.parseGraph();
    
    int src = atoi(argv[2]);
    int sink = atoi(argv[3]);
    
    long long *flow;
    flow = (long long *)malloc(sizeof(long long));
    *flow = 0;
//     std::cout << "done" << std::endl;
    Compute_maxflow(G1, src, sink, flow);
    std::cout << "FLow---> " << *flow << std::endl;
    return 0;
}
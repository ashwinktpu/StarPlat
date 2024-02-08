#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"

using namespace sycl;

void compute_degree(queue &Q, int *d_vertex, int *d_edges, int *d_parent, int *d_vertexCover, int *d_degree, int *h_degree, int *h_vertexCover, int V, int E){
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride){ 
        d_degree[i] = 0;
    }
    }); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < E; i += stride){ 
        if(d_vertexCover[d_parent[i]]==0 && d_vertexCover[d_edges[i]]==0){
            atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_degree[d_parent[i]]);
            atomic_data.fetch_add(1);
        }
    }
    }); })
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(h_degree, d_degree, sizeof(int) * (V));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(h_vertexCover, d_vertexCover, sizeof(int) * (V));})
        .wait();
    
    int s = 0;
    for(int i=0; i<V; i++){
        if(h_vertexCover[i]==0 && h_degree[i]>2)
            s++;
    }
    std::cout << s << std::endl;
}

void trim1(queue &Q, int *d_vertex, int *d_edges, int *d_parent, int *d_vertexCover, int *d_degree, int V, int E){
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;

    bool flag = true;
    bool *d_flag = malloc_device<bool>(1, Q);

    while(flag){

      Q.submit([&](handler &h)
             { h.single_task([=]()
                             { *d_flag = (bool)false; }); })
        .wait(); 

      Q.submit([&](handler &h)
              { h.parallel_for(NUM_THREADS, [=](id<1> i)
                                {
      for (; i < E; i += stride){ 
          if(d_vertexCover[d_parent[i]]==0 && d_vertexCover[d_edges[i]]==0){
              bool f = true;

              if(d_degree[d_parent[i]] == 1 || (d_degree[d_parent[i]]==2 && d_degree[d_edges[i]]!=1)){
                  int v = d_edges[i];
                  f = false;
                  d_vertexCover[v] = (int)1;

                  for (int edge = d_vertex[v]; edge < d_vertex[v+1]; edge++) 
                  {
                      int nbr = d_edges[edge];
                      if(d_vertexCover[nbr]==0)
                      {   
                          atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_degree[nbr]);
                          atomic_data.fetch_add(-1);
                      }

                  }
              }

              if(f && (d_degree[d_edges[i]] == 1 || d_degree[d_edges[i]] == 2)){
                  f = false;
                  int v = d_parent[i];
                  d_vertexCover[v] = (int)1;

                  for (int edge = d_vertex[v]; edge < d_vertex[v+1]; edge++) 
                  {
                      int nbr = d_edges[edge];
                      if(d_vertexCover[nbr]==0)
                      {   
                          atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_degree[nbr]);
                          atomic_data.fetch_add(-1);
                      }

                  }
              }

              *d_flag = !f;
          }
      }
      }); })
          .wait();

      Q.submit([&](handler &h)
            { h.memcpy(&flag, d_flag, sizeof(bool));})
        .wait();
    }
}

void trim2(queue &Q, int *d_vertex, int *d_edges, int *d_parent, int *d_vertexCover, int *d_degree, int *h_edges, int *h_parent, int *h_degree, int *h_vertexCover, int V, int E){
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;
    

    Q.submit([&](handler &h)
            { h.memcpy(h_vertexCover, d_vertexCover, sizeof(int) * (V));})
        .wait();
    
    int avg=0;
    for(int i=0; i<E; i++){
        if(h_vertexCover[h_parent[i]]==0 && h_vertexCover[h_edges[i]]==0){
            avg++;
        }
    }
    int rem = 0;
    for(int i=0; i<V; i++){
        if(h_vertexCover[i]==0)
            rem++;
    }
    avg = (avg*2)/rem;

    Q.submit([&](handler &h)
         { h.parallel_for(NUM_THREADS, [=](id<1> id){
    for (; id < V; id += stride){
            if(d_degree[id]>avg){
                d_vertexCover[id] = (int)1;
            }
    }
        });
    });
    Q.wait_and_throw();    
}

void bfs(queue &Q, int src, int *d_rowPtr, int *d_colIndices, int *d_parent, int *d_vertexCover, int *d_level, bool *d_modified_next, bool *d_curr_taken, int *d_degree, int V, int E){
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;
    
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

    bool *d_finished = malloc_device<bool>(1, Q);
    int *d_levelNumber = malloc_device<int>(1, Q);

    while (!finished)
    {

        finished = true;
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
                                                if(d_level[nbr]==INT_MAX && d_vertexCover[nbr]==0)
                                                {   
                                                    d_level[nbr]=*d_levelNumber;
                                                    d_modified_next[nbr] = true;
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

     std::cout << levelnumber << std::endl;
    
//     int *h_level = (int *)malloc(V * sizeof(int));
//     Q.submit([&](handler &h)
//             { h.memcpy(h_level, d_level, sizeof(int) * (V));})
//         .wait(); 
//     for(int i=0; i<V; i++){
//         std::cout << h_level[i] << " ";
//     }
//     std::cout << std::endl;
    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride){ 
        if(d_level[i]%2==0)
            d_curr_taken[i] = (bool)true;
        else
            d_curr_taken[i] = (bool)false;
    }
    }); })
        .wait();
    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < E; i += stride){ 
        if((d_level[d_parent[i]]%2)==0 && (d_level[d_colIndices[i]]%2)==0){
            if(d_degree[d_parent[i]] < d_degree[d_colIndices[i]])
                d_curr_taken[d_parent[i]] = (bool)false;
            else
                d_curr_taken[d_colIndices[i]] = (bool)false;
        }
    }
    }); })
        .wait();
    
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride){ 
        if(d_curr_taken[i])
            d_vertexCover[i] = (int)1;
    }
    }); })
        .wait();
}

int Compute_vertexCover(graph &g, int *vc){
    queue Q(default_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;
    
    int V = g.num_nodes();
    int E = g.num_edges();
    std::cout << V << " " << E << std::endl;
    
    // Declaring all the host variables
    int *h_vertex; // storing the offset list
    int *h_edges; // storing the csr list
    int *h_vertexCover;
    int *h_degree;
    int *h_parent;
    
    // Initializing all host variables
    h_vertex = (int *)malloc((V + 1) * sizeof(int));
    h_edges = (int *)malloc(E * sizeof(int));
    h_vertexCover = (int *)malloc((V + 1) * sizeof(int));
    h_degree = (int *)malloc((V + 1) * sizeof(int));
    h_parent = (int *)malloc(E * sizeof(int));

    bool flag = true;
    
    // Storing the graph values in the respective host variables
    int avgDegree = (2*E)/V;
    for (int i = 0; i <= V; i++)
    {
        int temp = g.indexofNodes[i];
        h_vertex[i] = temp;
        
        temp = g.inDeg[i];
        h_degree[i] = temp;
        
        h_vertexCover[i] = 0;
    }
     
    int p = 0;
    for (int i = 0; i < E; i++)
    {
        while(h_vertex[p+1]==i)
            p++;
        h_parent[i] = p;
        
        int temp = g.edgeList[i];
        h_edges[i] = temp;
    }
    
    
    // Declaring device variables for the respective host variables
    int *d_vertex;
    int *d_edges;
    int *d_vertexCover;
    int *d_degree;
    int *d_parent;
    bool *d_flag;

    int *d_level;
    bool *d_modified_next;
    bool *d_curr_taken;
    
    // Initializing device variables
    d_vertex = malloc_device<int>((V + 1), Q);
    d_edges = malloc_device<int>((E), Q);
    d_vertexCover = malloc_device<int>((V + 1), Q);
    d_degree = malloc_device<int>((V + 1), Q);
    d_parent = malloc_device<int>((E), Q); 
    d_flag = malloc_device<bool>(1, Q);
    
    d_level = malloc_device<int>(V, Q);
    d_modified_next = malloc_device<bool>(V, Q);
    d_curr_taken = malloc_device<bool>(V, Q);

    // Trasferring values from CPU to GPU memory
    Q.submit([&](handler &h)
            { h.memcpy(d_vertex, h_vertex, sizeof(int) * (V + 1));})
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(d_edges, h_edges, sizeof(int) * (E));})
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(d_vertexCover, h_vertexCover, sizeof(int) * (V + 1));})
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(d_degree, h_degree, sizeof(int) * (V + 1));})
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(d_parent, h_parent, sizeof(int) * (E));})
        .wait();
    
    //     Start recording the timings
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    
    Q.submit([&](handler &h)
         { h.parallel_for(NUM_THREADS, [=](id<1> id){
    for (; id < V; id += stride){
        if(d_degree[id]>avgDegree || d_degree[id]==0){
//         if(d_degree[id]==0){
                d_vertexCover[id] = (int)1;
            }
    }
        });
    });
    Q.wait_and_throw();

    Q.submit([&](handler &h)
        { h.memcpy(h_vertexCover, d_vertexCover, sizeof(int) * (V + 1));})
    .wait();
    
    int s = 0;
    for(int i=0; i<V; i++){
        s += h_vertexCover[i];
    }
    std::cout << s << std::endl;
    
    int it = 0;
    while(flag){
//     for(int i=0; i<2;i++){
        int src;
        int *d_src = malloc_device<int>(1, Q);
        
        Q.submit([&](handler &h)
             { h.single_task([=]()
                             { *d_flag = (bool)true; }); })
        .wait();

        Q.submit([&](handler &h)
            { h.parallel_for(NUM_THREADS, [=](id<1> i) {
        for (; i < E; i += stride){ 
            if(d_vertexCover[d_parent[i]]==0 && d_vertexCover[d_edges[i]]==0){
                *d_src = d_parent[i];
                *d_flag = false;
            }
        }
        }); })
            .wait();
        
        Q.submit([&](handler &h)
            { h.memcpy(&flag, d_flag, sizeof(bool));})
        .wait();
        
        if(flag){
            
            Q.submit([&](handler &h)
                { h.memcpy(h_vertexCover, d_vertexCover, sizeof(int) * (V + 1));})
            .wait();
            
            for(int i=0; i<V; i++){
                if(h_vertexCover[i] == 1)
                    (*vc)++;
            }
            
//             free(h_vertex);
//             free(h_edges);
//             free(h_parent);
//             free(h_degree);
//             free(h_vertexCover);
            
//             free(d_vertex);
//             free(d_edges);
//             free(d_parent);
//             free(d_degree);
//             free(d_vertexCover);
            
//             free(d_level);
//             free(d_modified_next);
//             free(d_curr_taken);
            
            std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
            std::cout << "GPU Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " milliseconds." << std::endl;
            return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        }
        
        Q.submit([&](handler &h)
            { h.memcpy(&src, d_src, sizeof(int));})
        .wait();
        
        std::cout << "Src: " << src << std::endl;
        
        bfs(Q, src, d_vertex, d_edges, d_parent, d_vertexCover, d_level, d_modified_next, d_curr_taken, d_degree, V, E);
        
        compute_degree(Q, d_vertex, d_edges, d_parent, d_vertexCover, d_degree, h_degree, h_vertexCover, V, E);
        trim1(Q, d_vertex, d_edges, d_parent, d_vertexCover, d_degree, V, E);
        trim2(Q, d_vertex, d_edges, d_parent, d_vertexCover, d_degree, h_edges, h_parent, h_degree, h_vertexCover, V, E);
    
        Q.submit([&](handler &h)
            { h.memcpy(h_vertexCover, d_vertexCover, sizeof(int) * (V + 1));})
        .wait();
        
        int s = 0;
        for(int i=0; i<V; i++){
            s += h_vertexCover[i];
        }
        std::cout << s << std::endl;

        flag = true;
    }
}

int main(int argc, char **argv){
    graph G1(argv[1]);
    G1.parseGraph();
    
    long long ans = 0;
    long long avgTime = 0;
    
    for(int i=0; i<5; i++){
        int *vc;
        vc = (int *)malloc(sizeof(int));
        *vc = 0;
        int time = Compute_vertexCover(G1, vc);
        std::cout << "vertex Cover for iteration " << i << " : -> " << *vc << std::endl;
        ans += *vc;
        avgTime += time;
        std::cout << std::endl;
    }
    std::cout << "Avg vertex cover --> " << (ans/5) << std::endl;
    std::cout << "Avg Time --> " << (avgTime/5) << std::endl;
    return 0;
}
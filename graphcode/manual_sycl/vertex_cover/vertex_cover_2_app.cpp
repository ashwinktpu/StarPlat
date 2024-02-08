#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"

using namespace sycl;

int Compute_vertexCover(graph &g, int *vc){
    queue Q(default_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    
    int NUM_THREADS = 1048576;
    int stride = NUM_THREADS;
    
    int V = g.num_nodes();
    int E = g.num_edges();
    std::cout << V << " " << E << std::endl;
    
    // Declaring all the host variables
    int *h_vertex; // storing the offset list
    int *h_edges; // storing the csr list
    int *h_parent;
    
    int *h_selected;
    
    // Initializing all host variables
    h_vertex = (int *)malloc((V + 1) * sizeof(int));
    h_edges = (int *)malloc(E * sizeof(int));
    h_parent = (int *)malloc(E * sizeof(int));
    
    h_selected = (int *)malloc((V+1) * sizeof(int));
    
    // Storing the graph values in the respective host variables
    for (int i = 0; i <= V; i++)
    {
        int temp = g.indexofNodes[i];
        h_vertex[i] = temp;
        
        h_selected[i] = 0;
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
    int *d_parent;
    int *d_selected;
    
    // Initializing device variables
    d_vertex = malloc_device<int>((V + 1), Q);
    d_edges = malloc_device<int>((E), Q);
    d_parent = malloc_device<int>((E), Q); 
    
    d_selected = malloc_device<int>((V+1), Q);
    
    // Trasferring values from CPU to GPU memory
    Q.submit([&](handler &h)
            { h.memcpy(d_vertex, h_vertex, sizeof(int) * (V + 1));})
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(d_edges, h_edges, sizeof(int) * (E));})
        .wait();

    Q.submit([&](handler &h)
            { h.memcpy(d_selected, h_selected, sizeof(int) * (V + 1));})
        .wait();
    Q.submit([&](handler &h)
            { h.memcpy(d_parent, h_parent, sizeof(int) * (E));})
        .wait();
    
    // Start recording the timing
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    
//     Q.submit([&](handler &h)
//          { h.parallel_for(NUM_THREADS, [=](id<1> v){
//     for (; v < V; v += stride){
//         if (d_selected[v] == false){ // if filter begin 
//         for (int edge = d_vertex[v]; edge < d_vertex[v+1]; edge++) { // FOR NBR ITR 
//           int nbr = d_edges[edge];
//           if (d_selected[nbr] == false){ // if filter begin 
//             d_selected[nbr] = true;
//             d_selected[v] = true;

//           } // if filter end

//         } //  end FOR NBR ITR. TMP FIX!

//       } // if filter end
//     }
//         });
//     });
//     Q.wait_and_throw();


//     //reduce
//     bool *d_stict_select = malloc_device<bool>((V + 1), Q);
//     Q.submit([&](handler &h)
//              { h.parallel_for(NUM_THREADS, [=](id<1> i)
//                               {
//     for (; i < V; i += stride) d_stict_select[i] = (bool)false; }); })
//         .wait();

//     Q.submit([&](handler &h)
//          { h.parallel_for(NUM_THREADS, [=](id<1> e){
//     for (; e < E; e += stride){
//         if(d_selected[d_parent[e]] && !d_selected[d_edges[e]])
//             d_stict_select[d_parent[e]] = (bool)true;
//         if(!d_selected[d_parent[e]] && d_selected[d_edges[e]])
//             d_stict_select[d_edges[e]] = (bool)true;
//     }
//         });
//     });
//     Q.wait_and_throw();

//     Q.submit([&](handler &h)
//          { h.parallel_for(NUM_THREADS, [=](id<1> v){
//     for (; v < V; v += stride){
//         if(!d_stict_select[v])
//             d_selected[v] = false;
//     }
//         });
//     });
//     Q.wait_and_throw();

    Q.submit([&](handler &h)
         { h.parallel_for(NUM_THREADS, [=](id<1> e){
    for (; e < E; e += stride){
        if(!d_selected[d_parent[e]] && !d_selected[d_edges[e]]){
            d_selected[d_parent[e]] = true;
            d_selected[d_edges[e]] = true;
        }
    }
        });
    });
    Q.wait_and_throw();
    
    
    Q.submit([&](handler &h)
        { h.memcpy(h_selected, d_selected, sizeof(int) * (V+1));})
    .wait();

//     int s = 0;
    for(int i=0; i<V; i++){
        *vc += h_selected[i];
    }
    std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
    std::cout << "GPU Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " milliseconds." << std::endl;
    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
//     std::cout << s << std::endl;
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
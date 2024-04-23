// #ifndef GENCPP_K_CORE.TXT_H
// #define GENCPP_K_CORE.TXT_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace std;
using namespace sycl;

void k_core(graph& g,int k)
{
  queue Q(default_selector_v);
  std::cout << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
  int *d_k;
  d_k=malloc_device<int>(1, Q);
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;

  d_meta=malloc_device<int>((1+V), Q);
  d_data=malloc_device<int>((E), Q);
  d_src=malloc_device<int>((E), Q);
  d_weight=malloc_device<int>((E), Q);
  d_rev_meta=malloc_device<int>((V+1), Q);

  Q.submit([&](handler &h)
    { h.memcpy(  d_meta,   h_meta, sizeof(int)*(V+1)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(  d_data,   h_data, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(   d_src,    h_src, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_weight, h_weight, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1))); })
  .wait();

  // CSR END
  //LAUNCH CONFIG
  int NUM_THREADS = 1048576;
  int stride = NUM_THREADS;

  // TIMER START
  std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int *d_kcore_count;
  d_kcore_count=malloc_device<int>(1, Q);
  int kcore_count = 0; // asst in main

  int *d_update_count;
  d_update_count=malloc_device<int>(1, Q);
  int update_count = 1; // asst in main

  int* d_indeg;
  d_indeg=malloc_device<int>(V, Q);

  int* d_indeg_prev;
  d_indeg_prev=malloc_device<int>(V, Q);

  bool* d_modified;
  d_modified=malloc_device<bool>(V, Q);

  bool* d_modified_nxt;
  d_modified_nxt=malloc_device<bool>(V, Q);

  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
    for (; i < V; i += stride) d_modified[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
  for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
d_indeg[v] = (int)(d_meta[v+1]-d_meta[v]); //InitIndex
d_indeg_prev[v] = (int)d_indeg[v]; //InitIndex
}
}); }).wait(); // end KER FUNC

bool *d_finished;
d_finished=malloc_device<bool>(1, Q);
bool finished = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!finished) {

finished = true;
Q.submit([&](handler &h)
{ h.memcpy(d_finished, &finished, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h)
{ h.memcpy(d_k, &k, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_indeg[v] < *d_k && d_indeg[v] != 0){ // if filter begin 
d_indeg[v] = 0;
// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
  int nbr = d_data[edge];
  if (d_indeg[nbr] >= *d_k){ // if filter begin 
    // atomic update
    atomic_ref<int,sycl::memory_order::relaxed,memory_scope::device, access::address_space::global_space> atomic_data_0(d_indeg[nbr]);
    atomic_data_0 -= 1;
    d_modified_nxt[nbr] = true;

  } // if filter end

} //  end FOR NBR ITR. TMP FIX!

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h)
{ h.memcpy(&k, d_k, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_finished = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&finished, d_finished, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT
    
// int *d_k = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
{ h.memcpy(d_k, &k, 1 * sizeof(int)); })
.wait();


Q.submit([&](handler &h)
{ h.memcpy(d_kcore_count, &kcore_count, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_indeg[v] >= k){ // if filter begin
 atomic_ref<int,sycl::memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_kcore_count[0]);   
 atomic_data.fetch_add(1);;

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h)
{ h.memcpy(&k, d_k, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h)
{ h.memcpy(&kcore_count, d_kcore_count, 1 * sizeof(int)); })
.wait();
    

//free up!! all propVars in this BLOCK!
free(d_modified_nxt, Q);
free(d_modified, Q);
free(d_indeg_prev, Q);
free(d_indeg, Q);

//TIMER STOP

std::cout<<kcore_count<<std::endl;
std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
std::cout<<"Time required: "<<std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count()<<"[µs]"<<std::endl;

} //end FUN

// #endif

int main(int argc, char** argv)
{
	char* inp = argv[1];
	std::cout << "Taking input from: " << inp << std::endl;
	graph g(inp);
	g.parseGraph();
	k_core(g, 3);
	return 0;
}
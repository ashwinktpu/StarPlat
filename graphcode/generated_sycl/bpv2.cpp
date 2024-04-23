// #ifndef GENCPP_BP.TXT_H
// #define GENCPP_BP.TXT_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void bp(graph& g,int k)
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
    h_weight[i] = 1;
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
  int* d_cur_prob;
  d_cur_prob=malloc_device<int>(V, Q);

  int* d_prior_prob;
  d_prior_prob=malloc_device<int>(V, Q);

  int *d_iter;
  d_iter=malloc_device<int>(1, Q);
  int iter = 0; // asst in main

  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
    for (; i < V; i += stride) d_cur_prob[i] = (int)1;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
  for (; i < V; i += stride) d_prior_prob[i] = (int)1;});
}).wait();

do{
iter++;
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
  int new_prob = 0; // asst in main

  // for all else part
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    int e = edge;
    new_prob = new_prob + d_weight[e] * d_prior_prob[v];

  } //  end FOR NBR ITR. TMP FIX!
  d_cur_prob[v] = new_prob;
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
d_prior_prob[v] = d_cur_prob[v];
}
}); }).wait(); // end KER FUNC


}while(iter < k);

//free up!! all propVars in this BLOCK!
int *h_cur_prob;
h_cur_prob = (int *)malloc((V)*sizeof(int));
Q.submit([&](handler &h)
			{ h.memcpy(h_cur_prob, d_cur_prob, sizeof(int) * (V)); })
		.wait(); 

int minibelief=h_cur_prob[0],maxibelief=h_cur_prob[0];
      //std::cout<<"MINIMUM_BELIEF: "<< minibelief << "  MAXIMUM_BELIEF: "<<maxibelief<<std::endl;
      for(int i=1;i<V;i++)
      {   
          //std::cout<<h_cur_prob[i];
          if(minibelief>h_cur_prob[i])
            minibelief=h_cur_prob[i];
          
          if(maxibelief<h_cur_prob[i])
            maxibelief=h_cur_prob[i];
      }
int count=0;
int midvalue=(maxibelief+minibelief)/2+1;    
std::cout<<"MIDVALUE: "<<midvalue<<std::endl; 
      for(int i=0;i<V;i++)
      { 
          if(midvalue>=h_cur_prob[i])
           count++;
      
      }
      std::cout<<"No of nodes having belief value less than midvalue "<<count<<std::endl;   
free(d_prior_prob, Q);
free(d_cur_prob, Q);

//TIMER STOP
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
    bp(g,2);
	return 0;
}
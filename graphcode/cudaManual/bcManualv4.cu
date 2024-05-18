//~ nvcc bcManualv4.cu -o bcManualv4-sm60.out -arch=sm_60 -I.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"

#include <random>
#include <sstream>
#include <fstream>
#include <string>
#include <string>

#define cudaCheckError() {                                             \
 cudaError_t e=cudaGetLastError();                                     \
 if(e!=cudaSuccess) {                                                  \
   printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e)); \
   exit(0);                                                            \
 }                                                                     \
}


template <typename T>
__global__ void initKernel(unsigned nSize,T* dArray,T initVal){
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if(id < nSize){ 
    dArray[id]=initVal;
  }
}

__global__ void incHop(int* d_hops_from_source) {
    *d_hops_from_source = *d_hops_from_source + 1;
  }

  __global__ void bc_forward_pass(int* d_offset,int* d_edgeList,int* d_edgeLen, double* d_sigma, int* d_level, int* d_hops_from_source, unsigned n, bool* d_finished) {
    unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
    if(v >= n) return;

    // only processing the nodes at level '*d_hops_from_source' -- a level synchronous processing, though not work efficient
    if(d_level[v] == *d_hops_from_source) {
       unsigned end = d_offset[v+1];
       for(unsigned i = d_offset[v]; i < end; ++i) { // going over the neighbors of u
          unsigned w = d_edgeList[i];
          if(d_level[w] == -1) {  // v is seen for the first time
            d_level[w] = *d_hops_from_source + 1; // no atomics required since this is benign data race due to level synchronous implementation
            *d_finished = false;
          }
          if(d_level[w] == *d_hops_from_source + 1) { // 'v' is indeed the neighbor of u
            atomicAdd(&d_sigma[w], d_sigma[v]);
          }
       }
    }
  }


  __global__ void bc_backward_pass(int* d_offset,int* d_edgeList,int* d_edgeLen, double* d_sigma, double* d_delta, double* d_nodeBC, int* d_level, int* d_hops_from_source, unsigned n) {
    // TO replace - DONE
    // u --> v
    // v --> w
    unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
    if(v >= n) return;

    if(d_level[v] == *d_hops_from_source - 1) { // backward traversal of DAG, one level at a time

       unsigned end = d_offset[v+1];
       double sum = 0.0;
       for(unsigned i = d_offset[v]; i < end; ++i) { // going over the neighbors of u for which it is the predecessor in the DAG
          unsigned w = d_edgeList[i];
          if(d_level[w] == *d_hops_from_source) {
            //~ if(std::isnan(d_sigma[w])
            //~ assert(d_sigma[w]!=0);
            sum += (1.0 * d_sigma[v]) / d_sigma[w] * (1.0 + d_delta[w]);
          }
       }

       d_delta[v] += sum;
    }

  }

  __global__ void accumulate_bc(double * d_delta, double* d_nodeBC, int* d_level, unsigned s, unsigned n) {

    unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid >= n || tid == s || d_level[tid] == -1) return;


    d_nodeBC[tid] += d_delta[tid]/2.0;

  }


__global__ void initialize(double* d_sigma, double* d_delta, int* d_level, int* d_hops_from_source, unsigned s, unsigned n) {
    unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid < n) {
      d_level[tid] = -1;
      d_delta[tid] = 0.0;
      d_sigma[tid] = 0;

      if(tid == s) { // for the source
        d_level[tid] = 0;
        d_sigma[tid] = 1;
        *d_hops_from_source = 0;
      }
    }
  }


void BC(int * OA , int * edgeList , int* edgeLength, int V, int E, std::set<unsigned> sourceSet, bool printAns=false) {

  int* d_offset;
  int* d_edgeList;
  int* d_edgeLen;  //why this for unweighted?

  // VAR for BC
  // G variables
  cudaMalloc(&d_offset,sizeof(int) *(1+V));
  cudaMalloc(&d_edgeList,sizeof(int) *(E));
  cudaMalloc(&d_edgeLen,sizeof(int) *(E));
  // G's Memcpy
  cudaMemcpy (d_offset, OA, sizeof(int) *(1+V) ,cudaMemcpyHostToDevice);
  cudaMemcpy (d_edgeList, edgeList, sizeof(int) *(E) ,cudaMemcpyHostToDevice);
  cudaMemcpy (d_edgeLen, edgeLength , sizeof(int) *(E) ,cudaMemcpyHostToDevice);


  // Vars from DSL body
  double* d_sigma; cudaMalloc(&d_sigma,  sizeof(double) * V);
  double* d_delta ;  cudaMalloc(&d_delta,  sizeof(double)   * V);

  // Vars from DSL func param and output var
  double* d_nodeBC;  cudaMalloc(&d_nodeBC, sizeof(double)   * V);

  // extra D vars
  bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));
  int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

  // extra H vars
  bool finished = false;
  int hops_from_source;

  //DEBUG ONLY
  std::cout<< "srcSet Size:"<< sourceSet.size() << '\n';

  cudaEvent_t start, stop; ///TIMER START
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);

  // FOR LAUNCH CONFIG - V
  unsigned int block_size = V;
  unsigned int num_blocks = 1;
  if(V > 1024){
    block_size = 1024;                        // at some point when sh-mem comes in, it should be 512
    num_blocks = (V+block_size-1)/block_size; // modified without ceil fun call
  }

  for(auto src:sourceSet) {

    hops_from_source = 0; // keeps track of the number of hops from source in the current iteration.
    initialize<<<num_blocks, block_size>>>(d_sigma, d_delta, d_level, d_hops_from_source, src, V);
    //DEBUG
    long k=0;
    //FORWARD PASS
    do{

      finished=true;
      cudaMemcpy(d_finished,&finished, sizeof(bool) ,cudaMemcpyHostToDevice);

      //~ cudaMemset(d_finished,true,sizeof(bool)); DONOT use MEMSET --rupesh

      bc_forward_pass<<<num_blocks, block_size>>>(d_offset, d_edgeList,d_edgeLen, d_sigma, d_level, d_hops_from_source, V, d_finished);

      cudaDeviceSynchronize(); //MUST - rupesh

      ++hops_from_source; // updating the level to process in the next iteration

      incHop<<<1,1>>>(d_hops_from_source);
      cudaDeviceSynchronize(); //MUST -rupesh //this 2nd CDS may be removed because of following D2H
      //DEBUG
      k++;
      cudaMemcpy(&finished,d_finished, sizeof(bool) ,cudaMemcpyDeviceToHost);
      //~ std::cout<< "SRC:"<< src <<" Fin? "<< (finished?"True":"False") << '\n';
    }while(!finished);


    //DEBUG
    /*
    cudaMemcpy(level,d_level , sizeof(int) * (V), cudaMemcpyDeviceToHost);
    std::cout<< "SRC:"<< src << " iters:" << k << " Hops:"<<hops_from_source<< '\n';
    for (int i = 0; i <V; i++)
      printf("%d %d\n", i, level[i]);
    return;
    */

    hops_from_source--;
    cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(hops_from_source), cudaMemcpyHostToDevice);

    //BACKWARD PASS
    while(hops_from_source > 1) {
      bc_backward_pass<<<num_blocks, block_size>>>(d_offset, d_edgeList,d_edgeLen, d_sigma, d_delta, d_nodeBC, d_level, d_hops_from_source, V);
      --hops_from_source;
      cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(hops_from_source), cudaMemcpyHostToDevice);
    }

    accumulate_bc<<<num_blocks, block_size>>>(d_delta, d_nodeBC, d_level, src, V);
    cudaDeviceSynchronize();
  }// END FOR



  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
  //~ printf("GPU Time: %.6f ms \nIterations:%d\n", milliseconds);

  if(printAns){

  //For printing result variable
  double* nodeBC;
  nodeBC = (double *)malloc( (V)*sizeof(double));
  cudaMemcpy(nodeBC,d_nodeBC , sizeof(double) * (V), cudaMemcpyDeviceToHost);

   for (int i = 0; i <9; i++)
     printf("%d %lf\n", i, nodeBC[i]);
  }

  //cudaCheckError();


  //~ char *outputfilename = "output_generated.txt";
  //~ FILE *outputfilepointer;
  //~ outputfilepointer = fopen(outputfilename, "w");
  //~ for (int i = 0; i <V; i++)
  //~ {
    //~ fprintf(outputfilepointer, "%d  %d\n", i, dist[i]);
  //~ }
  //~ Let's add fclose!

}


// driver program to test above function
int main(int argc , char ** argv)
{
  graph G(argv[1]);
  G.parseGraph();
  bool printAns = false;

  std::set<unsigned> src;

  if(argc>3) { // ./a.out inputfile srcFile -p
      printAns = true;
  }

  // Check and READ Src file =================

  std::string line;
  std::ifstream srcfile(argv[2]);
  if (!srcfile.is_open()) {
    std::cout << "Unable to open src file :" << argv[1] << std::endl;
    exit(1);
  }
  

  int nodeVal;
  while ( std::getline (srcfile,line) ) {
   std::stringstream ss(line);
   ss>> nodeVal;
   //~ std::cout << "src " << nodeVal << '\n';
   src.insert(nodeVal);
  }

  srcfile.close();
  //==========================================

  //---------------------------------------//
  int V = G.num_nodes();
  int E = G.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  //-------------------------------------//

  int* edgeLen = G.getEdgeLen();

  int *OA;
  int *edgeList;
  int *edgeLength;

   OA = (int *)malloc( (V+1)*sizeof(int));
   edgeList = (int *)malloc( (E)*sizeof(int));
   edgeLength = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = G.indexofNodes[i];
    OA[i] = temp;
  }
  //~ for(int i=1; i<=V;++i){
    //~ std::cout<< i << " " << OA[i]-OA[i-1] << '\n';
  //~ }
  for(int i=0; i< E; i++) {
    int temp = G.edgeList[i];
    edgeList[i] = temp;
    temp = edgeLen[i];
    edgeLength[i] = temp;
  }


    //~ cudaEvent_t start, stop; // should not be here!
    //~ cudaEventCreate(&start);
    //~ cudaEventCreate(&stop);
    //~ float milliseconds = 0;
    //~ cudaEventRecord(start,0);

    BC(OA,edgeList, edgeLength, V,E,src, printAns);
    cudaDeviceSynchronize();

    //~ cudaEventRecord(stop,0);
    //~ cudaEventSynchronize(stop);
    //~ cudaEventElapsedTime(&milliseconds, start, stop);
    //~ printf("Time taken by function to execute is: %.6f ms\n", milliseconds);
    cudaCheckError();

  return 0;

}

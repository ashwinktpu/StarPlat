#include "bc_dsl_v3.h"
///usr/local/cuda-9.2/bin/nvcc -o "bc_dsl_v3-fixed".out "bc_dsl_v3-fixed.cu"  -std=c++14 -rdc=true -arch=sm_70
void Compute_BC(graph& g,double* BC,std::set<int>& sourceSet)
{
  // CSR BEGIN
  unsigned V = g.num_nodes();
  unsigned E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_weight;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_weight;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);

  // CSR END
  double * d_BC;cudaMalloc(&d_BC, sizeof(int)*(V));
  //LAUNCH CONFIG

  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? V: 512;
  unsigned numBlocks    = (numThreads+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);

  //END CSR

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING
  initKernel<double> <<<numBlocks,numThreads>>>(V,d_BC,0);

  double* d_sigma;
  cudaMalloc(&d_sigma, sizeof(double)*(V));

  double* d_delta;
  cudaMalloc(&d_delta, sizeof(double)*(V));

  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++)
  {
    unsigned src = *itr;
    initKernel<double> <<<numBlocks,numThreads>>>(V,d_delta,0);

    initKernel<double> <<<numBlocks,numThreads>>>(V,d_sigma,0);

    initIndex<double><<<1,1>>>(V,d_sigma,src, 1.0);


    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    int hops_from_source=0;

    bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
    int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));
    int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

    initKernel<int> <<<numBlocks,numThreads>>>(V,d_level,-1);
    initIndex<int><<<1,1>>>(V,d_level,src, 0);

    long k =0 ;// For DEBUG
    do {
      finished = true;
      cudaMemcpy(d_finished, &finished, sizeof(bool)*(1), cudaMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass<<<numBlocks,numThreads>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished, d_BC);

      incrementDeviceVar<<<1,1>>>(d_hops_from_source);
      cudaDeviceSynchronize(); //MUST - rupesh
      ++hops_from_source; // updating the level to process in the next iteration
      k++;

      cudaMemcpy(&finished, d_finished, sizeof(bool)*(1), cudaMemcpyDeviceToHost);
    }while(!finished);

    hops_from_source--;
    cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);

    //BACKWARD PASS
    while(hops_from_source > 1) {

      //KERNEL Launch
      back_pass<<<numBlocks,numThreads>>>(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished, d_BC);

      hops_from_source--;
      cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);
    }
  }

  //ADD TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(BC,d_BC , sizeof(double) * (V), cudaMemcpyDeviceToHost);

}

// driver program to test above function
int main(int argc , char ** argv)
{
  graph G(argv[1]);
  G.parseGraph();
  bool printAns = false;

  std::set<int> src;

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
  printf("#srces:%d\n",src.size());
  //==========================================




    //~ cudaEvent_t start, stop; // should not be here!
    //~ cudaEventCreate(&start);
    //~ cudaEventCreate(&stop);
    //~ float milliseconds = 0;
    //~ cudaEventRecord(start,0);
    unsigned V = G.num_nodes();
    unsigned E = G.num_nodes();
    double* BC = (double *)malloc(sizeof(double)*V);
    Compute_BC(G,BC,src);

    int LIMIT = 9;
    if(printAns)
     LIMIT=V;

    for (int i = 0; i < LIMIT; i++){
      printf("%lf\n", BC[i]);
    }

    //~ cudaDeviceSynchronize();

    //~ cudaEventRecord(stop,0);
    //~ cudaEventSynchronize(stop);
    //~ cudaEventElapsedTime(&milliseconds, start, stop);
    //~ printf("Time taken by function to execute is: %.6f ms\n", milliseconds);
    cudaCheckError();

  return 0;

}

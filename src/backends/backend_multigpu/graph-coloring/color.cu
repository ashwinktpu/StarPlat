#include "color.h"


void colorGraph(graph& g){
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

    cudaMalloc(&d_meta, sizeof(int)*(1+V));
    cudaMalloc(&d_data, sizeof(int)*(E));
    cudaMalloc(&d_src, sizeof(int)*(E));
    cudaMalloc(&d_weight, sizeof(int)*(E));
    cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));
 
    cudaMemcpy(d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_src,    h_src, sizeof(int)*(E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1)), cudaMemcpyHostToDevice);

    // CSR END
    //LAUNCH CONFIG
    const unsigned threadsPerBlock = 512;
    unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
    unsigned numThreads   = (V < threadsPerBlock)? V: 512;

    // TIMER START
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    float milliseconds = 0;
    cudaEventRecord(start,0);

    unsigned int* d_color;unsigned int* h_color;
    cudaMalloc(&d_color,sizeof(unsigned int)*(V+1));
    h_color = (unsigned int*)malloc(sizeof(unsigned int)*(V+1));
    bool* d_modified;
    cudaMalloc(&d_modified,sizeof(bool)*(V+1));
    bool* d_modified_next;
    cudaMalloc(&d_modified_next,sizeof(bool)*(V+1));

    initKernel<unsigned int> <<<numBlocks,threadsPerBlock>>>(V,d_color,(unsigned int)0);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int fpoint1 = 0;
    int iter = 0;
    while(fpoint1<V){
        cudaMemcpyToSymbol(::fpoint1,&fpoint1,sizeof(int),0,cudaMemcpyHostToDevice);
        curandGenerator_t gen;
        curandCreateGenerator(&gen,CURAND_RNG_PSEUDO_PHILOX4_32_10);
        curandSetPseudoRandomGeneratorSeed(gen,rand());       
        curandGenerate(gen,d_color,(V+1));
        compute_colors<<<numBlocks,numThreads>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_color,d_modified,d_modified_next);
        cudaDeviceSynchronize();
        cudaMemcpyFromSymbol(&fpoint1,::fpoint1,sizeof(int),0,cudaMemcpyDeviceToHost);
        cudaMemcpy(d_modified,d_modified_next,(V+1)*sizeof(bool),cudaMemcpyDeviceToDevice);
        iter+=1;
        // fpoint1+=1;
        printf("%d\n",fpoint1);
        curandDestroyGenerator(gen);
    }

    printf("num colors %d\n",iter);

    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("GPU Time: %.6f ms\n", milliseconds);


}



int main(int argc,char* argv[])
{
  char *file_name = argv[1];
  graph g(file_name);
  g.parseGraph();
  colorGraph(g);
  return 0;
}

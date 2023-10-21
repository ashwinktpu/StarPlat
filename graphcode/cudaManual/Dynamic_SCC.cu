// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
// This code is incomplete. Work is going on in this.
#include "DynamicSCC_cuda.h"

void staticSCC(graph &g, int* d_meta, int* d_data, int* d_rev_meta, int* d_src, bool* h_isPivot, int* h_range, int* scc, bool* d_isPivot, int* d_range, int* d_scc)
{
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  bool* d_modified_next;
  cudaMalloc(&d_modified_next, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  int* d_outDeg;
  cudaMalloc(&d_outDeg, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_outDeg,(int)0);

  int* d_inDeg;
  cudaMalloc(&d_inDeg, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_inDeg,(int)0);

  bool* d_visitFw;
  cudaMalloc(&d_visitFw, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitFw,(bool)false);

  bool* d_visitBw;
  cudaMalloc(&d_visitBw, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitBw,(bool)false);

  bool* d_propFw;
  cudaMalloc(&d_propFw, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propFw,(bool)false);

  bool* d_propBw;
  cudaMalloc(&d_propBw, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propBw,(bool)false);

  int* d_pivotField;
  cudaMalloc(&d_pivotField, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_pivotField,(int)-1);

  vHong_kernel1<<<numBlocks, threadsPerBlock>>>(V,E,d_outDeg,d_inDeg,d_meta,d_rev_meta);
  cudaDeviceSynchronize();



  ; // asst in .cu

  bool fpoint1 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  int k=0; // #fixpt-Iterations
  while(!fpoint1) {

    fpoint1 = true;
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_range,d_scc,d_isPivot);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT

  vHong_kernel3<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_outDeg,d_inDeg,d_pivotField,d_range);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  vHong_kernel4<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_pivotField,d_range,d_isPivot,d_visitBw,d_visitFw);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  bool fpoint2 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  k=0; // #fixpt-Iterations
  while(!fpoint2) {

    fpoint2 = true;
    cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_propBw,d_visitBw,d_propFw,d_visitFw,d_scc,d_range);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT

  vHong_kernel6<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_visitFw,d_visitBw,d_range);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  vHong_kernel7<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_visitFw,d_visitBw,d_propBw,d_propFw);
  cudaDeviceSynchronize();



  fpoint1 = false;
  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  k=0; // #fixpt-Iterations
  while(!fpoint1) {

    fpoint1 = true;
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel8<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_range,d_scc,d_isPivot);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_range,(int)0);

  vHong_kernel9<<<numBlocks, threadsPerBlock>>>(V,E,d_range);
  cudaDeviceSynchronize();



  bool fpoint4 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  k=0; // #fixpt-Iterations
  while(!fpoint4) {

    fpoint4 = true;
    cudaMemcpyToSymbol(::fpoint4, &fpoint4, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint4, &fpoint4, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel10<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_range,d_scc);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint4, ::fpoint4, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    cudaMemcpyToSymbol(::fpoint4, &fpoint4, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel11<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_range);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint4, ::fpoint4, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint4, ::fpoint4, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT

  bool fpoint5 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  k=0; // #fixpt-Iterations
  while(!fpoint5) {

    fpoint5 = true;
    cudaMemcpyToSymbol(::fpoint5, &fpoint5, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel12<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_outDeg,d_inDeg,d_pivotField,d_range);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    vHong_kernel13<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_pivotField,d_range,d_isPivot,d_visitBw,d_visitFw);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    fpoint2 = false;
    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k=0; // #fixpt-Iterations
    while(!fpoint2) {

      fpoint2 = true;
      cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
      vHong_kernel14<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_propBw,d_visitBw,d_propFw,d_visitFw,d_scc,d_range);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu


      cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    vHong_kernel15<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_visitFw,d_visitBw,d_range);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    cudaMemcpyToSymbol(::fpoint5, &fpoint5, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel16<<<numBlocks, threadsPerBlock>>>(V,E,d_scc,d_visitFw,d_visitBw,d_propBw,d_propFw);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint5, ::fpoint5, sizeof(bool), 0, cudaMemcpyDeviceToHost);




    cudaMemcpyFromSymbol(&fpoint5, ::fpoint5, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT

  cudaMemcpy(h_isPivot, d_isPivot, sizeof(bool)*V, cudaMemcpyDeviceToHost);
  cudaMemcpy(h_range, d_range, sizeof(int)*V, cudaMemcpyDeviceToHost);
  cudaMemcpy(scc, d_scc, sizeof(int)*V, cudaMemcpyDeviceToHost);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_propBw);
  cudaFree(d_propFw);
  cudaFree(d_visitBw);
  cudaFree(d_visitFw);
  cudaFree(d_inDeg);
  cudaFree(d_outDeg);
  cudaFree(d_pivotField);
  cudaFree(d_modified);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
void DynSCC(graph &g, std::vector<update> updateBatch, int batchSize) {
	int V = g.num_nodes();
	int E = g.num_edges();
	
	int *h_meta;
	int *h_data;
	int *h_src;
	int *h_rev_meta;

	h_meta = (int *)malloc( (V+1)*sizeof(int));
	h_data = (int *)malloc( (E)*sizeof(int));
	h_src = (int *)malloc( (E)*sizeof(int));
	h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

	for(int i=0; i<= V; i++) {
		int temp;
		temp = g.indexofNodes[i];
		h_meta[i] = temp;
		temp = g.rev_indexofNodes[i];
		h_rev_meta[i] = temp;
	}

	for(int i=0; i< E; i++) {
		int temp;
		temp = g.edgeList[i];
		h_data[i] = temp;
		temp = g.srcList[i];
		h_src[i] = temp;
	}

	int* d_meta;
	int* d_data;
	int* d_src;
	int* d_rev_meta;

	cudaMalloc(&d_meta, sizeof(int)*(1+V));
	cudaMalloc(&d_data, sizeof(int)*(E));
	cudaMalloc(&d_src, sizeof(int)*(E));
	cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));

	cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
	cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
	cudaMemcpy(   d_src,    h_src, sizeof(int)*(E), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1)), cudaMemcpyHostToDevice);
	
	const unsigned threadsPerBlock = 512;
	unsigned numThreads   = (V < threadsPerBlock)? 512: V;
	unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
	
	bool* isPivot = (bool*)malloc(V*sizeof(bool));
	int* range = (int*)malloc(V*sizeof(int));
	int* scc = (int*)malloc(V*sizeof(int));

	bool* d_isPivot;
	int* d_range;
	int* d_scc;

	cudaMalloc(&d_isPivot, V*sizeof(bool));
	cudaMalloc(&d_range, V*sizeof(int));
	cudaMalloc(&d_scc, V*sizeof(int));

	staticSCC(g, d_meta, d_data, d_rev_meta, d_src, isPivot, range, scc, d_isPivot, d_range, d_scc);

	int vertices = 0;
	for (int i = 0; i < V; i++){
		if (isPivot[i]){
			vertices++;
		}
	}
	
	int *condense_meta = (int *)malloc(vertices * sizeof(int));
	int *condense_edge = (int *)malloc(E * sizeof(int));
	int *mapper = (int *)malloc(2 * vertices * sizeof(int));
	int k = 0;
	for (int i = 0; i < V; i++) {
		if (isPivot[i]) {
			mapper[k] = i;
			k += 2;
		}
	}
	k = 0;
	for (int i = 1; i < 2 * vertices; i += 2){
		mapper[i] = k;
		k++;
	}
	
	int* d_source;
	int* d_destination;
	int* d_mapper;
	int* d_pivot;
	int* d_edges;
	int* d_mark;
	int* locks;
	int* parents;
	int* d_k;
	int* d_mapper_vertices;
	
	cudaMalloc(&d_source, E * sizeof(int));
	cudaMalloc(&d_destination, E * sizeof(int));
	cudaMalloc(&d_mapper, 2 * vertices * sizeof(int));
	cudaMalloc(&d_pivot, V*sizeof(int));
	cudaMalloc(&d_edges, E*sizeof(int));
	cudaMalloc(&locks, E*sizeof(int));
	cudaMalloc(&parents, vertices*sizeof(int));
	cudaMalloc(&d_k, sizeof(int));
	cudaMalloc(&d_mapper_vertices, E*sizeof(int));
	
	initKernel<int> <<<numBlocks,numThreads>>>(V,d_range,(int)0);
	initKernel<int> <<<numBlocks,numThreads>>>(V,d_scc,(int)-1);
	initKernel<bool> <<<numBlocks,numThreads>>>(V,d_isPivot,(bool)false);
	initKernel<int> <<<numBlocks,numThreads>>>(E,d_edges,(int)-1);
	initKernel<int> <<<numBlocks,numThreads>>>(E,d_source,(int)-1);
	initKernel<int> <<<numBlocks,numThreads>>>(E,d_destination,(int)-1);
	
	numThreads   = (V < threadsPerBlock)? 512: V;
	numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
	
	cudaMemcpy(range, d_range, V*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(d_mapper, mapper, 2*vertices*sizeof(int), cudaMemcpyHostToDevice);
	
	int batchElements = 0;
	int* mark = (int*)malloc(2 * batchSize * sizeof(int));
	cudaMalloc(&d_mark, 2 * batchSize * sizeof(int));
	for (int updateIndex = 0; updateIndex < updateBatch.size(); updateIndex += batchSize) {
		if ((updateIndex + batchSize) > updateBatch.size()) {
				batchElements = updateBatch.size() - updateIndex;
		}
		else
			batchElements = batchSize;
		
		cudaMemset(d_k, 0, sizeof(int));
		
		int index = 0;
		set_pivot<<<numBlocks, numThreads>>>(V, d_pivot, d_isPivot, d_range);
		create_graph<<<numBlocks, numThreads>>>(V, E, vertices, d_meta, d_data, d_source, d_destination, d_mapper, d_edges, d_isPivot, d_range, d_k, d_pivot);
		
		bool fpoint = false;
		while (!fpoint)
		{
			cudaMemcpyToSymbol(::fpoint, &fpoint, sizeof(bool), 0, cudaMemcpyHostToDevice);
			clean_graph<<<numBlocks, numThreads>>>(E, d_source, d_destination);
			cudaMemcpyFromSymbol(&fpoint, ::fpoint, sizeof(bool), 0, cudaMemcpyDeviceToHost);
		}
		
		index = 0;
		for (int batchIndex = updateIndex; batchIndex < (updateIndex + batchSize) && batchIndex < updateBatch.size(); batchIndex++) {
			if (updateBatch[batchIndex].type == 'a') {
				update u = updateBatch[batchIndex];
				int src = u.source;
				int dst = u.destination;
				mark[index++] = src;
				mark[index++] = dst;
			}
		}
		g.updateCSRAdd(updateBatch, updateIndex, batchElements);
		
		numThreads   = (index / 2 < threadsPerBlock)? 512: index / 2;
		numBlocks    = (index / 2 + threadsPerBlock-1)/threadsPerBlock;
		
		initKernel<int> <<<numBlocks,numThreads>>>(E,locks,(int)0);
		initKernel<int> <<<numBlocks,numThreads>>>(vertices,parents,(int)-1);
		
		numThreads   = (E < threadsPerBlock)? 512: E;
		numBlocks    = (E+threadsPerBlock-1)/threadsPerBlock;
		
		initKernel<int> <<<numBlocks,threadsPerBlock>>>(E,d_edges,(int)-1);
		
		cudaMemcpy(d_mark, mark, 2 * batchSize * sizeof(int), cudaMemcpyHostToDevice);
		update_condense_graph<<<numBlocks, numThreads>>>(vertices, E, d_source, d_destination, d_k, d_mark, d_pivot, d_mapper);
		check_cycle<<<numBlocks,numThreads>>>(vertices, E, d_source, d_destination, d_k, d_mark, d_mapper, d_pivot, d_edges, locks, parents, d_mapper_vertices);
		check_cycle_repeat<<<numBlocks,numThreads>>>(vertices, E, d_source, d_destination, d_k, d_mark, d_mapper, d_pivot, d_edges, locks, parents, d_mapper_vertices);
		
		fpoint = false;
		while (!fpoint)
		{
			cudaMemcpyToSymbol(::fpoint, &fpoint, sizeof(bool), 0, cudaMemcpyHostToDevice);
			clean_graph<<<numBlocks, numThreads>>>(E, d_source, d_destination);
			cudaMemcpyFromSymbol(&fpoint, ::fpoint, sizeof(bool), 0, cudaMemcpyDeviceToHost);
		}
		
		numThreads   = (V < threadsPerBlock)? 512: V;
		numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
		
		graph_recolour<<<numBlocks, numThreads>>>(V, d_range, d_pivot);
	}
	
	cudaFree(d_meta);
	cudaFree(d_data);
	cudaFree(d_rev_meta);
	cudaFree(d_src);
	cudaFree(d_mapper);
	cudaFree(d_pivot);
	cudaFree(d_isPivot);
	cudaFree(d_range);
	cudaFree(d_scc);
	cudaFree(locks);
	cudaFree(parents);
	cudaFree(d_source);
	cudaFree(d_destination);
	cudaFree(d_edges);
	cudaFree(d_mark);
	cudaFree(d_k);
	
	int count_scc = 0;
	for (int i = 0; i < V; i++) {
		if (d_isPivot[i]) {
			count_scc++;
		}
	}
}

/*int main(int argc, char*argv[])
{
	std::string s(argv[1]);
	double start, end;
	if (argc > 1)
		;
	else
	{
		std::cout << "Give the path to the input graph text file" << std::endl;
		std::cin >> s;
	}
	char *c = new char[s.length() + 1];
	std::copy(s.begin(), s.end(), c);
	graph g(c);
	g.parseGraph();
	DynSCC(g);
	return 0;
}*/

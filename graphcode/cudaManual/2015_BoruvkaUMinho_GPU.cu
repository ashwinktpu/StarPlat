#include "BoruvkaUMinho_GPU.cuh"


texture<unsigned int, 1, cudaReadModeElementType> tex_psrc;
texture<unsigned int, 1, cudaReadModeElementType> tex_outdegree;
texture<unsigned int, 1, cudaReadModeElementType> tex_edgessrcdst;
texture<unsigned int, 1, cudaReadModeElementType> tex_edgessrcwt;

__global__
void find_min_per_vertex(CSR_Graph g, unsigned int *vertex_minedge){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x; // Vertex ID
	if(id >= g.nnodes) return;

	unsigned min_edge = 0;
	unsigned min_weight = UINT_MAX;
	unsigned min_dst = g.nnodes;

	unsigned edge = tex1Dfetch(tex_psrc, id);
	unsigned last_edge = edge + tex1Dfetch(tex_outdegree, id);
	for(; edge < last_edge; ++edge)
	{
		unsigned wt = tex1Dfetch(tex_edgessrcwt, edge);
		unsigned dst = tex1Dfetch(tex_edgessrcdst, edge);
		//if(dst != g.nnodes)
		{
			if(wt < min_weight || (wt == min_weight && dst < min_dst))
			{
				min_weight = wt;
				min_edge = edge;
				min_dst = dst;
			}
		}
		//else if (dst == min_dst && wt >= min_weight)
		//{
		//	g.edgessrcdst[edge] = g.nnodes;
		//}
	}

	vertex_minedge[id] = min_edge;
}

__global__
void initialize_color(unsigned int nnodes, unsigned int *color, unsigned int *vertex_minedge){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= nnodes) return;
	
	unsigned edge = vertex_minedge[id];
	if(edge == 0) color[id] = id;
	else color[id] = tex1Dfetch(tex_edgessrcdst, edge);
}

__global__
void propagate_color(unsigned int nnodes, unsigned int *color, unsigned int *changed){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= nnodes) return;

	unsigned int my_color = color[id];
	unsigned int other_color = color[my_color];

	if(my_color != other_color)
	{
		color[id] = other_color;
		*changed = true;
	}
}

__global__
void remove_duplicates(CSR_Graph g, unsigned int *vertex_minedge){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= g.nnodes) return;

	unsigned int edge = vertex_minedge[id];
	if(edge == 0) return;
	unsigned int dst = tex1Dfetch(tex_edgessrcdst, edge);

	unsigned int other_edge = vertex_minedge[dst];
	if(other_edge == 0) return;
	unsigned int other_dst = tex1Dfetch(tex_edgessrcdst, other_edge);

	if(id == other_dst && id > dst) // found loop and maintain edge by smaller vertex id
	{
		vertex_minedge[dst] = 0;
	}
}

__global__
void mark_mst_edges(unsigned int nnodes, unsigned int *selected_edges, unsigned int *vertex_minedge, unsigned int *map_edges){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= nnodes) return;

	unsigned int edge = vertex_minedge[id];
	selected_edges[map_edges[edge]] = 1;
}



__global__
void create_new_vertex_id(CSR_Graph g, unsigned int *color, unsigned int *new_vertex, unsigned int *next_nnodes){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= g.nnodes) return;

	if(id == color[id] && tex1Dfetch(tex_outdegree, id) > 0) // representative thread
	{	
		new_vertex[id] = 1;
	}
	else new_vertex[id] = 0;
}

__global__
void count_new_edges(CSR_Graph g, CSR_Graph next, unsigned int *color, unsigned int *new_vertex){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= g.nnodes) return;

	unsigned my_color = color[id];
	// count how many edges I will be adding to supervertex
	unsigned new_edges = 0;
	unsigned edge = tex1Dfetch(tex_psrc, id);
	unsigned last_edge = edge + tex1Dfetch(tex_outdegree, id);

	for(; edge < last_edge; ++edge)
	{
		unsigned dst = tex1Dfetch(tex_edgessrcdst, edge);
		if(/*dst != g.nnodes &&*/ my_color != color[dst]) ++new_edges;
	}

	unsigned supervertex_id = new_vertex[my_color];
	atomicAdd(&(next.outdegree[supervertex_id]), new_edges);
}

__global__
void setup_psrc(CSR_Graph next, unsigned int *next_nedges){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= next.nnodes) return;

	++next.psrc[id];
}

__global__
void insert_new_edges(CSR_Graph g, unsigned int *next_edgessrcdst, unsigned int *next_edgessrcwt, unsigned int *color, unsigned int *new_vertex, unsigned int *topedge_per_vertex, unsigned int *old_map_edges, unsigned int *new_map_edges){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= g.nnodes) return;

	unsigned my_color = color[id];
	unsigned supervertex_id = new_vertex[my_color];
	unsigned edge = tex1Dfetch(tex_psrc, id);
	unsigned last_edge = edge + tex1Dfetch(tex_outdegree, id);

	for(; edge < last_edge; ++edge)
	{
		unsigned dst = tex1Dfetch(tex_edgessrcdst, edge);
		if(dst != g.nnodes)
		{
			unsigned other_color = color[dst];
			if(my_color != other_color)
			{
				unsigned top_edge = atomicInc(&(topedge_per_vertex[supervertex_id]), UINT_MAX);
				//next_edgessrcdst[top_edge] = other_supervertex;
				next_edgessrcdst[top_edge] = new_vertex[other_color];
				next_edgessrcwt[top_edge] = tex1Dfetch(tex_edgessrcwt, edge);
				new_map_edges[top_edge] = old_map_edges[edge];
			}
		}

	}
}

__global__
void load_weights(CSR_Graph g, unsigned int *selected_edges, unsigned int *vertex_minweight){
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id >= g.nedges + 1) return;

	if(selected_edges[id] == 1)
	{
		vertex_minweight[id] = g.edgessrcwt[id];
	}
}





MGPU_MEM(unsigned int) BoruvkaUMinho_GPU(CSR_Graph *h_graph, unsigned block_size){
	mgpu::ContextPtr context = mgpu::CreateCudaDevice(0, NULL, true);
	detect_devices();
	cudaDeviceSetCacheConfig(cudaFuncCachePreferL1);

	//CSR_Graph *h_graph = new CSR_Graph(argv[1]);

	std::vector<CSR_Graph*> d_graph;
	d_graph.push_back(new CSR_Graph(h_graph->nnodes, h_graph->nedges, DEVICE));
	d_graph[0]->d_allocate();

	unsigned problem_size = h_graph->nnodes;
	unsigned edges_size = h_graph->nedges;
	//unsigned block_size = 1024;

	unsigned int *next_nnodes, *next_nedges;
	cudaMalloc((void **)&next_nnodes, sizeof(unsigned int));
	cudaMalloc((void **)&next_nedges, sizeof(unsigned int));

    unsigned int *d_changed, h_changed;
	if(cudaMalloc((void **)&d_changed, sizeof(unsigned int)) != cudaSuccess)
	{
		CudaTest(const_cast<char*>("allocating changed failed"));
	}

	MGPU_MEM(unsigned int) vertex_minedge = context->Malloc<unsigned int>(problem_size);
	MGPU_MEM(unsigned int) vertex_minweight = context->Fill<unsigned int>(edges_size+1, 0);
	MGPU_MEM(unsigned int) color = context->Malloc<unsigned int>(problem_size);	
	MGPU_MEM(unsigned int) new_vertex = context->Malloc<unsigned int>(problem_size);
	MGPU_MEM(unsigned int) supervertex_flag = context->Malloc<unsigned int>(problem_size);
	MGPU_MEM(unsigned int) topedge_per_vertex = context->Malloc<unsigned int>(problem_size);	
	MGPU_MEM(unsigned int) map_edges = context->FillAscending<unsigned int>(edges_size + 1, 0, 1);	
	MGPU_MEM(unsigned int) selected_edges = context->Fill<unsigned int>(edges_size + 1, 0);
	MGPU_MEM(unsigned int) new_map_edges = context->FillAscending<unsigned int>(edges_size + 1, 0, 1);	

	double starttime, endtime;
	float time;
	float timings[19];

	for(unsigned j = 0; j < 19; ++j) 
	{
		timings[j] = 0.0f;
	}

  	cudaEvent_t start, stop;
	cudaEventCreate(&start);  cudaEventCreate(&stop);

	unsigned int iteration = 0;
	long unsigned int total_weight = 0;

	starttime = rtclock();

	cudaEventRecord(start, 0);
	h_graph->copyHostToDevice(d_graph[0]); 
 	cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
 	timings[18] += time;
 	CudaTest(const_cast<char*>("copy CSR_Graph host to device failed"));

	do{
		//toString<<<1,1>>>(*d_graph[iteration]);
		unsigned n_blocks = compute_n_blocks(problem_size, block_size);
		printf("Graph has %u nodes and %u edges\n", problem_size, edges_size);

		//SegSortPairsFromIndices(d_graph[iteration]->edgessrcwt, d_graph[iteration]->edgessrcdst, edges_size+1, d_graph[iteration]->psrc, problem_size+1, *context);

		cudaBindTexture(0, tex_psrc, d_graph[iteration]->psrc, sizeof(unsigned int) * problem_size);
		CudaTest(const_cast<char*>("bind tex_psrc failed"));
		cudaBindTexture(0, tex_outdegree, d_graph[iteration]->outdegree, sizeof(unsigned int) * problem_size);
		CudaTest(const_cast<char*>("bind tex_outdegree failed"));
		cudaBindTexture(0, tex_edgessrcdst, d_graph[iteration]->edgessrcdst, sizeof(unsigned int) * (edges_size + 1));
		CudaTest(const_cast<char*>("bind tex_edgessrcdst failed"));
		cudaBindTexture(0, tex_edgessrcwt, d_graph[iteration]->edgessrcwt, sizeof(unsigned int) * (edges_size + 1));
		CudaTest(const_cast<char*>("bind tex_edgessrcwt failed"));


		cudaEventRecord(start, 0);
		find_min_per_vertex<<<n_blocks, block_size>>>(*d_graph[iteration], vertex_minedge->get());
	 	cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
	 	timings[0] += time;
		CudaTest(const_cast<char*>("find_min_per_vertex failed"));


		// depends on find_min_per_vertex
		cudaEventRecord(start, 0);
		remove_duplicates<<<n_blocks, block_size>>>(*d_graph[iteration], vertex_minedge->get());
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[16] += time;
		CudaTest(const_cast<char*>("remove_duplicates failed"));

		cudaEventRecord(start, 0);
	 	initialize_color<<<n_blocks, block_size>>>(d_graph[iteration]->nnodes, color->get(), vertex_minedge->get());
	 	cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
	 	timings[1] += time;
	 	CudaTest(const_cast<char*>("initialize_color color failed"));

		do{
			cudaEventRecord(start, 0);
			cudaMemset(d_changed, 0, sizeof(unsigned int)); 
			cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
			timings[2] += time;
			CudaTest(const_cast<char*>("memset d_changed failed"));

			// depends on initialize color
			// depends on find_min_per_vertex
			cudaEventRecord(start, 0);
			propagate_color<<<n_blocks, block_size>>>(d_graph[iteration]->nnodes, color->get(), d_changed);
			cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
			timings[3] += time;
			CudaTest(const_cast<char*>("propagate_color failed"));

			cudaEventRecord(start, 0);	
			cudaMemcpy(&h_changed, d_changed, sizeof(h_changed), cudaMemcpyDeviceToHost);
			cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
			timings[4] += time;
			CudaTest(const_cast<char*>("copy d_changed failed"));
		} while(h_changed);

		///////////////////////
		// saving selected edges and weights
		///////////////////////


		cudaEventRecord(start, 0);
		mark_mst_edges<<<n_blocks, block_size>>>(d_graph[iteration]->nnodes, selected_edges->get(), vertex_minedge->get(), map_edges->get());

		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[17] += time;
		CudaTest(const_cast<char*>("mark_mst_edges failed"));

		///////////////////////
		// allocate new device graph
		///////////////////////
		cudaEventRecord(start, 0);
		d_graph.push_back(new CSR_Graph(0, 0, DEVICE));
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[7] += time;
		CudaTest(const_cast<char*>("push_back failed"));

		///////////////////////
		// creating supervertices
		///////////////////////

		//depends on propagate colors
		cudaEventRecord(start, 0);
		cudaMemset(next_nnodes, 0, sizeof(unsigned int));
		create_new_vertex_id<<<n_blocks, block_size>>>(*d_graph[iteration], color->get(), supervertex_flag->get(), next_nnodes);
		CudaTest(const_cast<char*>("create_new_vertex_id failed"));
		mgpu::Scan<mgpu::MgpuScanTypeExc>(supervertex_flag->get(), problem_size, (unsigned int)0, mgpu::plus<unsigned int>(), (unsigned int*)0, &(d_graph[iteration+1]->nnodes), new_vertex->get(), *context);
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		//cudaMemcpy(&(d_graph[iteration+1]->nnodes), next_nnodes, sizeof(unsigned int), cudaMemcpyDeviceToHost);
		timings[10] += time;
		CudaTest(const_cast<char*>("mgpu::scan failed"));
		

		unsigned new_nnodes;
		new_nnodes = d_graph[iteration+1]->nnodes;
		
		if(unlikely(new_nnodes <= 1))
		{
			cudaUnbindTexture(tex_psrc);
			cudaUnbindTexture(tex_outdegree);
			cudaUnbindTexture(tex_edgessrcdst);
			cudaUnbindTexture(tex_edgessrcwt);
				//if(iteration > 0) d_graph[iteration]->d_deallocate();
			d_graph[iteration]->d_deallocate();
			break;
		}

		d_graph[iteration+1]->d_allocate_nodes();
		cudaDeviceSynchronize();
		///////////////////////
		// inserting new contracted edges
		///////////////////////

		// depends on propagate colors
		// depends on create_new_vertex_id
		cudaEventRecord(start, 0);
		count_new_edges<<<n_blocks, block_size>>>(*d_graph[iteration], *d_graph[iteration+1], color->get(), new_vertex->get());
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[12] += time;
		CudaTest(const_cast<char*>("count_new_edges failed"));

		// depends on count_new_edges
		cudaEventRecord(start, 0);
		cudaMemset(next_nedges, 0, sizeof(unsigned int));
		mgpu::Scan<mgpu::MgpuScanTypeExc>(d_graph[iteration+1]->outdegree, new_nnodes, (unsigned int)0, mgpu::plus<unsigned int>(), (unsigned int*)0, &(d_graph[iteration+1]->nedges), d_graph[iteration+1]->psrc, *context);
		CudaTest(const_cast<char*>("mgpu::Scan failed"));
		setup_psrc<<<compute_n_blocks(new_nnodes, block_size), block_size>>>(*d_graph[iteration+1], next_nedges);
		CudaTest(const_cast<char*>("setup_psrc failed"));
		//cudaMemcpy(&(d_graph[iteration+1]->nedges), next_nedges, sizeof(unsigned int), cudaMemcpyDeviceToHost);
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[13] += time;

		d_graph[iteration+1]->d_allocate_edges();
		cudaDeviceSynchronize();

		cudaEventRecord(start, 0);
		cudaMemcpy(topedge_per_vertex->get(), d_graph[iteration+1]->psrc, sizeof(unsigned int) * new_nnodes, cudaMemcpyDeviceToDevice); 
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[14] += time;
		CudaTest(const_cast<char*>("copy topedge_per_vertex failed"));

		// depends on topedge_per_vertex memcpy
		// depends on setup_psrc
		cudaEventRecord(start, 0);
		insert_new_edges<<<n_blocks, block_size>>>(*d_graph[iteration], d_graph[iteration+1]->edgessrcdst, d_graph[iteration+1]->edgessrcwt, color->get(), new_vertex->get(), topedge_per_vertex->get(), map_edges->get(), new_map_edges->get());
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[15] += time;
		CudaTest(const_cast<char*>("insert_new_edges failed"));
	
		edges_size = d_graph[iteration+1]->nedges;

		cudaEventRecord(start, 0);		
		cudaMemcpy(map_edges->get(), new_map_edges->get(), sizeof(unsigned int) * (edges_size + 1), cudaMemcpyDeviceToDevice); 
		cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
		timings[6] += time;
		CudaTest(const_cast<char*>("copy map_edges failed"));

		problem_size = new_nnodes;

		cudaUnbindTexture(tex_psrc);
		cudaUnbindTexture(tex_outdegree);
		cudaUnbindTexture(tex_edgessrcdst);
		cudaUnbindTexture(tex_edgessrcwt);

		if(iteration > 0)
		{
			//cudaDeviceSynchronize();
			d_graph[iteration]->d_deallocate();
			//cudaDeviceSynchronize();
		}

		++iteration;

	} while(true);
	return selected_edges;
}
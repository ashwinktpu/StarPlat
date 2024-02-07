// d_src -> Used to number the different strongly connected components of the graph.
// d_range -> Used for numbering/colouring the different subgrahs.
// d_pivotField -> Used to collect all the potential pivots for subgraph id.
// d_isPivot -> Indicator used to set the pivot vertex.
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"
using namespace sycl;

void scc_hong(graph& g)
{
	queue Q(gpu_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;
    
	
	int V = g.num_nodes();
	int E = g.num_edges();
	int *edgeLen = g.getEdgeLen();
	
	// Declaring all the host variables
	int *h_meta; // storing the offset list
	int *h_data; // storing the csr list
	int *h_src; // storing the reverse csr list
	int *h_weight; // storing the weights of edges
	int *h_rev_meta; // storing the reverse offset list
	
	// Initializing all host variables
	h_meta = (int *)malloc((V + 1) * sizeof(int));
	h_data = (int *)malloc(E * sizeof(int));
	h_src = (int *)malloc(E * sizeof(int));
	h_weight = (int *)malloc(E * sizeof(int));
	h_rev_meta = (int *)malloc((V + 1) * sizeof(int));
	
	// Storing the graph values in the respective host variables
	for (int i = 0; i <= V; i++)
	{
		int temp = g.indexofNodes[i];
		h_meta[i] = temp;
		temp = g.rev_indexofNodes[i];
		h_rev_meta[i] = temp;
	}
	
	for (int i = 0; i < E; i++)
	{
		int temp = g.edgeList[i];
		h_data[i] = temp;
		temp = g.srcList[i];
		h_src[i] = temp;
		temp = edgeLen[i];
		h_weight[i] = temp;
	}
	
	// Declaring device variables for the respective host variables
	int *d_meta;
	int *d_data;
	int *d_src;
	int *d_weight;
	int *d_rev_meta;
	
	// Initializing device variables
	d_meta = malloc_device<int>((V + 1), Q);
	d_data = malloc_device<int>((E), Q);
	d_src = malloc_device<int>((E), Q);
	d_weight = malloc_device<int>((E), Q);
	d_rev_meta = malloc_device<int>((V + 1), Q);
	
	// Trasferring values from CPU to GPU memory
	Q.submit([&](handler &h)
			{ h.memcpy(d_meta, h_meta, sizeof(int) * (V + 1)); })
		.wait();
		
	Q.submit([&](handler &h)
			{ h.memcpy(d_data, h_data, sizeof(int) * (E)); })
		.wait();
		
	Q.submit([&](handler &h)
			{ h.memcpy(d_src, h_src, sizeof(int) * (E)); })
		.wait();
		
	Q.submit([&](handler &h)
			{ h.memcpy(d_weight, h_weight, sizeof(int) * (E)); })
		.wait();
		
	Q.submit([&](handler &h)
			{ h.memcpy(d_rev_meta, h_rev_meta, sizeof(int) * (V + 1)); })
		.wait();
		
	// Initializing the number of threads with which the number of threads the kernel will launch
	int NUM_THREADS = 1048576;
	int stride = NUM_THREADS;
	
	// Start recording the timing
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	// Declaring device variables which are required specific to our computation
	int *d_outDeg;
	int *d_inDeg;
	bool *d_visitFw;
	bool *d_visitBw;
	bool *d_propFw;
	bool *d_propBw;
	bool *d_isPivot;
	int *d_scc;
	int *d_range;
	int *d_pivotField;
	
	// Initializing the device variables
	d_outDeg = malloc_device<int>((V), Q);
	d_inDeg = malloc_device<int>((V), Q);
	d_visitFw = malloc_device<bool>((V), Q);
	d_visitBw = malloc_device<bool>((V), Q);
	d_propFw = malloc_device<bool>((V), Q);
	d_propBw = malloc_device<bool>((V), Q);
	d_isPivot = malloc_device<bool>((V), Q);
	d_scc = malloc_device<int>((V), Q);
	d_range = malloc_device<int>((V), Q);
	d_pivotField = malloc_device<int>((V), Q);
	
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_outDeg[i] = 0; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_inDeg[i] = 0; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_visitFw[i] = false; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_visitBw[i] = false; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_propFw[i] = false; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_propBw[i] = false; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_isPivot[i] = false; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_scc[i] = -1; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_range[i] = 0; }); });
				
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> i)
				{
			for ( ;i < V; i += stride) d_pivotField[i] = -1; }); });
				
	// Calculating the in=degree and out-degree of all vertices of the graph.
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> src)
				{
			for ( ; src < V; src += stride) {
				int nxtSrc = src + 1;
				
				d_inDeg[src] = d_inDeg[src] + d_rev_meta[nxtSrc] - d_rev_meta[src];
				d_outDeg[src] = d_outDeg[src] + d_meta[nxtSrc] - d_meta[src];
			 }
			 }); });
			
	// Fixed point variable for implementing the fixed point construct.
	bool *fpoint1 = malloc_shared<bool>(1, Q);
	*fpoint1 = false;
	
    
    // removing nodes which are scc1 ( if the outdegree or indegree of that node is 0 it means that vertice is scc)
	// Trimming	
	while (!(*fpoint1))
	{
		*fpoint1 = true;
		Q.submit([&](handler &h)
				{ h.parallel_for(NUM_THREADS, [=](id<1> src)
					{
				for ( ; src < V; src += stride){
					if (d_scc[src] == -1){
						int havePar = 0;
						int haveChild = 0;
						// Iterating through the parents of each node of the same subgraph
						for (int edge = d_rev_meta[src]; edge < d_rev_meta[src + 1]; edge++)
						{
							int par = d_src[edge];
							if (d_scc[par] == -1){
								if (d_range[par] == d_range[src]){
									havePar = 1;
								}
							}
						}
						// Iterating through the children of each node of the same subgraph
						for (int edge = d_meta[src]; edge < d_meta[src + 1]; edge++)
						{
							int dst = d_data[edge];
							if (d_scc[dst] == -1)
							{
								if (d_range[dst] == d_range[src])
								{
									haveChild = 1;
								}
							}
						}
						// Checking for existence of parents and children of each node of same subgraph
						if ((havePar == 0) || (haveChild == 0))
						{
							d_scc[src] = src;
							d_isPivot[src] = true;
							*fpoint1 = false;
						}
					}
				}
				}); }).wait();
	}
	
	// Updating the degree of vertices for pivot selection
        bool *fpoint4 = malloc_shared<bool>(1, Q);
		*fpoint4 = false;
		while (!(*fpoint4))
		{
			        *fpoint4 = true;
                     Q.submit([&](handler &h)
                   { h.parallel_for(NUM_THREADS, [=](id<1> src)
                    {
                for ( ; src < V; src += stride){
                    if (d_scc[src] == -1){
                        int color = d_range[src];
                        int index = color - (color / V) * V;
                        int oldSrcValue = -1;
                        int oldSrc = d_pivotField[index];
                        if (oldSrc >= 0){
                            oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];
                        }
                        if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){
                            *fpoint4 = false;
                            d_pivotField[index] = src;
                        }
                    }
                }
                }); }).wait();
		}
		
		free(fpoint4, Q);
	

	
	// Poll the first pivot to intiate the forward-backward pass
    
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> src)
				{
			for ( ; src < V; src += stride){
				if (d_scc[src] == -1){
					int color = d_range[src];
					int index = color - (color / V) * V;
					if (d_pivotField[index] == src){
						d_visitFw[src] = true;
						d_visitBw[src] = true;
						d_isPivot[src] = true;
					}
				}
			}
			}); }).wait();
			
	// Fixed point contruct for the forward-backward pass
	*fpoint1 = false;
	while (!(*fpoint1))
	{
		*fpoint1 = true;
		Q.submit([&](handler &h)
				{ h.parallel_for(NUM_THREADS, [=](id<1> src)
					{
				for ( ; src < V; src += stride){
					if (d_scc[src] == -1){
						int myrange = d_range[src];
						// Forward pass
						if (d_propFw[src] == false && d_visitFw[src] == true){
							for (int edge = d_meta[src]; edge < d_meta[src + 1]; edge++){
								int dst = d_data[edge];
								if (d_scc[dst] == -1){
									if (d_visitFw[dst] == false && d_range[dst] == myrange){
										d_visitFw[dst] = true;
										*fpoint1 = false;
									}
								}
							}
							d_propFw[src] = true;
						}
						// Backward pass
						if (d_propBw[src] == false && d_visitBw[src] == true){
							for (int edge = d_rev_meta[src]; edge < d_rev_meta[src + 1]; edge++){
								int par = d_src[edge];
								if (d_scc[par] == -1){
									if (d_visitBw[par] == false && d_range[par] == myrange){
										d_visitBw[par] = true;
										*fpoint1 = false;
									}
								}
							}
							d_propBw[src] = true;
						}
					}
				}
				}); }).wait();
	}
	
	// Assigning colours to different vertices of the three subgraphs discovered after the forward-backward pass.
	// Also updating the d_scc field of the SCC component newly identified.
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> src)
				{
			for ( ; src < V; src += stride){
				if (d_scc[src] == -1){
					if (d_visitFw[src] == false || d_visitBw[src] == false){
						int ext = 0;
                        
                        
						if (d_visitFw[src] == false && d_visitBw[src] == true){
							ext = 1;
						}			
						else if (d_visitBw[src] == false && d_visitFw[src] == true){
							ext = 2;
						}
                        else
                            ext=3;
						
						// Creating a new color to assign new subgraphs created.
						d_range[src] = 3 * d_range[src] + ext;
						d_visitFw[src] = false;
						d_visitBw[src] = false;
						d_propFw[src] = false;
						d_propBw[src] = false;
					}
					
					// Setting the d_scc of the vertex identified in the newly identified SCC.
					if (d_visitFw[src] == true && d_visitBw[src] == true){
						d_scc[src] = src;
					}
				}
			}
			}); }).wait();

	// Trimming after the identifying the one large SCC of O(n)
	*fpoint1 = false;
	while (!(*fpoint1))
	{
		*fpoint1 = true;
		Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> src)
				{
			for ( ; src < V; src += stride){
				if (d_scc[src] == -1){
					int havePar = 0;
					int haveChild = 0;
					
					// Iterating through all the parents of a node
					for (int edge = d_rev_meta[src]; edge < d_rev_meta[src + 1]; edge++)
					{
						int par = d_src[edge];
						if (d_scc[par] == -1){
							if (d_range[par] == d_range[src]){
								havePar = 1;
							}
						}
					}
					
					// Iterating through all the children of a node 
					for (int edge = d_meta[src]; edge < d_meta[src + 1]; edge++){
						int dst = d_data[edge];
						if (d_scc[dst] == -1){
							if (d_range[dst] == d_range[src]){
								haveChild = 1;
							}
						}
					}
					
					// Checking and isolating the nodes having no parent or no children 
					if ((havePar == 0) || (haveChild == 0)){
						d_scc[src] = src;
						d_isPivot[src] = true;
						*fpoint1 = false;
					}
				}
			}
			}); }).wait();
	}
	
	
	Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> src)
				{
			for ( ; src < V; src += stride){
					d_range[src] =  src;
			}
			}); }).wait();
	
	// Reassigning the colouring of all the vertices to again initiate the forward-backward pass
	// WCC decomposition.
	*fpoint1 = false;
	while (!(*fpoint1)){
		*fpoint1 = true;
		Q.submit([&](handler &h)
				{ h.parallel_for(NUM_THREADS, [=](id<1> src)
					{
				for ( ; src < V; src += stride){
					if (d_scc[src] == -1){
						// Iterating through the children of the vertices whose SCC has not been identified.
						for (int edge = d_meta[src]; edge < d_meta[src + 1]; edge++){
							int dst = d_data[edge];
							if (d_scc[dst] == -1){
								if (d_range[dst] > d_range[src]){
									d_range[src] = d_range[dst];
									*fpoint1 = false;
								}
							}
						}
					}
				}
				}); }).wait();
		
    }
        
        
    
    // Main procedure iteration
	*fpoint1 = false;
	while (!(*fpoint1))
	{
		*fpoint1 = true;
		// Recomputing the max degree vertex of each sub-graph for the unidentified SCCs.
        
        bool *fpoint3 = malloc_shared<bool>(1, Q);
		*fpoint3 = false;
		while (!(*fpoint3))
		{
			*fpoint3 = true;
			Q.submit([&](handler &h)
				{ h.parallel_for(NUM_THREADS, [=](id<1> src)
					{
				for ( ; src < V; src += stride){
					if (d_scc[src] == -1){
						int color = d_range[src];
						int index = color - (color / V) * V;
						int oldSrcValue = -1;
						int oldSrc = d_pivotField[index];
						if (oldSrc >= 0){
							oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];
						}
						if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){
							d_pivotField[index] = src;
                            *fpoint3 = false;
						}
					}
				}
				}); }).wait();
		}
		
		free(fpoint3, Q);
	
		Q.submit([&](handler &h)
				{ h.parallel_for(NUM_THREADS, [=](id<1> src)
					{
				for ( ; src < V; src += stride){
					if (d_scc[src] == -1){
						int color = d_range[src];
						int index = color - (color / V) * V;
						if (d_pivotField[index] == src){
							d_visitFw[src] = true;
							d_visitBw[src] = true;
							d_isPivot[src] = true;
						}
					}
				}
				}); }).wait();
        
       
		
		bool *fpoint2 = malloc_shared<bool>(1, Q);
		// Detecting SCC in the graph.
		*fpoint2 = false;
		while (!(*fpoint2))
		{
			*fpoint2 = true;
			Q.submit([&](handler &h)
					{ h.parallel_for(NUM_THREADS, [=](id<1> src)
						{
					for ( ; src < V; src += stride){
						if (d_scc[src] == -1){
							int myrange = d_range[src];
							// Forward pass
							if (d_propFw[src] == false && d_visitFw[src] == true){
								for (int edge = d_meta[src]; edge < d_meta[src + 1]; edge++){
									int dst = d_data[edge];
									if (d_scc[dst] == -1){
										if (d_visitFw[dst] == false && d_range[dst] == myrange){
											d_visitFw[dst] = true;
											*fpoint2 = false;
										}
									}
								}
								d_propFw[src] = true;	
							}
							// Backward pass
							if (d_propBw[src] == false && d_visitBw[src] == true){
								for (int edge = d_rev_meta[src]; edge < d_rev_meta[src + 1]; edge++){
									int par = d_src[edge];
									if (d_scc[par] == -1){
										if (d_visitBw[par] == false && d_range[par] == myrange){
											d_visitBw[par] = true;
											*fpoint2 = false;
										}
									}
								}
								d_propBw[src] = true;
							}
						}
					}
					}); }).wait();
		}
		
		free(fpoint2, Q);
		
		// Final processing and colour assignment to the vertices.
		Q.submit([&](handler &h)
			{ h.parallel_for(NUM_THREADS, [=](id<1> src)
				{
			for ( ; src < V; src += stride){
				if (d_scc[src] == -1){
					// New colour initialization to vertices of subgraphs discovered.
					if (d_visitFw[src] == false || d_visitBw[src] == false){
						*fpoint1 = false;
						int ext = 0;
						if (d_visitFw[src] == false && d_visitBw[src] == true){
							ext = 1;
						}			
						else if (d_visitBw[src] == false && d_visitFw[src] == true){
							ext = 2;
						}
                        else
                            ext=3;
						
						// Creating a new color to assign new subgraphs created.
						d_range[src] = 3 * d_range[src] + ext;
						d_visitFw[src] = false;
						d_visitBw[src] = false;
						d_propFw[src] = false;
						d_propBw[src] = false;
					}
					// Fixing the vertices of the newly discovered vertices.
					if (d_visitFw[src] == true && d_visitBw[src] == true){
						d_scc[src] = src;
					}
				}
			}
			}); }).wait();
	}
	// Transferring from device to hast memory for final counting of SCCs.
	bool *h_scc = (bool *)malloc((V) * sizeof(bool));
	Q.submit([&](handler &h)
			{ h.memcpy(h_scc, d_isPivot, sizeof(bool) * (V)); }).wait();
	// SCC counting in host side.
	int count_scc = 0;
	for (int i = 0; i < V; i++){
		if (h_scc[i]){
			count_scc++;
		}
	}
	std::cout << std::endl;
	std::cout << "No.of strongly connected components in graph: " << count_scc << std::endl;
	
	free(d_range, Q);
	free(d_propBw, Q);
	free(d_visitFw, Q);
	free(d_propFw, Q);
	free(d_inDeg, Q);
	free(d_pivotField, Q);
	free(d_isPivot, Q);
	free(d_outDeg, Q);
	free(d_visitBw, Q);
	free(d_scc, Q);
	free(fpoint1, Q);
	
	std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
	std::cout << "GPU Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " milliseconds." << std::endl;
}

int main(int argc, char** argv)
{
	std::chrono::steady_clock::time_point start_prg = std::chrono::steady_clock::now();
	char* inp = argv[1];
	std::cout << "Taking input from: " << inp << std::endl;
	graph g(inp);
	g.parseGraph();
	scc_hong(g);
	std::chrono::steady_clock::time_point stop_prg = std::chrono::steady_clock::now();
	std::cout << "Time taken by program is: " <<std::chrono::duration_cast<std::chrono::milliseconds>(stop_prg - start_prg).count();
	std::cout << " milliseconds." << std::endl;
	return 0;
}
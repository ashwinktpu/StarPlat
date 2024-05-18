/**
 * Manual implementation for Betweenness Centrality
 * Brandes Algorithm 
 * Parallel implementation using AMD HIP Code. 
 * 
 * @author: cs22m056
*/

#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include <iostream>

#include "../graph.hpp"

int count = 0;
__device__ int countd = 0;

#define NL '\n'
// ! EXTREMELY DANGEROUS MACROS BELOW
#define debug_flag false
#define DEBUG if(debug_flag) 
#define hds hipDeviceSynchronize();

using std::cin;
using std::cout;
using std::vector;

#define FLAG cout << "FLAG\n";

__global__
void PrintArray(double* array, long size) {

	for(int i = 0; i < size; i++)
		printf("%1.0f ", array[i]);
	printf("\n");
}

template <typename T>
__global__
void InitializeArray(T* array, T value, long size) {

	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;

	if(id >= size)
		return;

	array[id] = value;
}

template <typename T>
__global__
void InitializeArrayLocation(T* array, T value, long location, long size) {

	if(location >= size)
		return;

	array[location] = value;
}

__global__
void BreadthFirstSearch(
	int nv, int* d_meta, int* d_data, double* d_sigma,
	int* d_d, int* level, bool* finished
) {

	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;

	if(v >= nv)
		return;

	DEBUG printf("Current Node: %d\n", v); 

	if(d_d[v] == *level) {

		for(int e = d_meta[v]; e < d_meta[v+1]; e++) { // ? Dynamic Parelelism

			int w = d_data[e];

			if(d_d[w] < 0) {

				// atomicCAS(&d_d[w], -1, *level + 1);
				d_d[w] = *level + 1; //? Atomics required?
				*finished = false;
			}

			if(d_d[w] == *level + 1)
				atomicAdd(&d_sigma[w], d_sigma[v]);
		}
	}
}

__global__
void ReversePass(
	int nv, int* d_meta, int* d_data, double* d_delta, double* d_sigma,
	int* d_d, int* level, bool* finished, double* d_bc
) {

	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;

	if(v >= nv)
		return;

	// auto grid = cooperative_groups::this_grid(); // TODO

	// !
	// ! POSSIBLE PROBLEM
	// !
	// ! Check the levels things. 
	// !

	if(d_d[v] == *level - 1) {

		for(int e = d_meta[v]; e < d_meta[v+1]; e++) {

			int w = d_data[e];

			if(d_d[w] == *level)
				atomicAdd(&d_delta[v], (d_sigma[v] / d_sigma[w]) * (1 + d_delta[w]));
		}

		// ? Moved to another kernel
		// printf("v = %d dd = %d level = %d\n", v, d_d[v], *level);
		// grid.sync();
		// d_bc[v] += d_delta[v];
	}
}

__global__
void AddElements(int nv, double* a, double* b, int* d_d, int* level, int src) {

	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if(v >= nv)
		return;

	if(d_d[v] == *level - 1 && v != src)
		a[v] += b[v];
}

void ComputeBetweennessCentrality(graph& g, vector<int>& sources, double *h_bc) { // TODO: sources should be a set

	unsigned V = g.num_nodes();
	unsigned E = g.num_edges();

	printf("Nodes : %d\n", V);
	printf("Edges : %d\n", E);

	int *h_meta = (int*) malloc(V * sizeof(int));
	int *h_data = (int*) malloc(E * sizeof(int));

	for(int i=0; i< V; i++)
		h_meta[i] = g.indexofNodes[i];

	for(int i=0; i< E; i++)
		h_data[i] = g.edgeList[i];

	int* d_meta;
	int* d_data;
	double* d_bc;	
	
	hipMalloc(&d_meta, sizeof(int) * V);
	hipMalloc(&d_data, sizeof(int) * E);
	hipMalloc(&d_bc, sizeof(double) * V);

	hipMemcpy(d_meta, h_meta, sizeof(int) * V, hipMemcpyHostToDevice);
	hipMemcpy(d_data, h_data, sizeof(int) * E, hipMemcpyHostToDevice);

	// ! TODO: Temp launch config

	int threads = 1024; //! TODO: try other config
	int blocks = ceil(V / 1024.0);

	hipEvent_t start, stop;
	hipEventCreate(&start);
	hipEventCreate(&stop);
	float milliseconds = 0;
	hipEventRecord(start,0);

	InitializeArray<double><<<blocks, threads>>>(d_bc, 0.0, V);

	double* d_sigma;
	double* d_delta;
	int* d_d;

	hipMalloc(&d_sigma, sizeof(double) * V);
	hipMalloc(&d_delta, sizeof(double) * V);
	hipMalloc(&d_d, sizeof(int) * V);

	bool* isAllNodesTraversed;
	hipHostMalloc(&isAllNodesTraversed, sizeof(bool), 0);

	int* level;
	hipHostMalloc(&level, sizeof(int), 0);

	for(int s: sources) {

		InitializeArray<double><<<blocks, threads>>>(d_sigma, 0.0, V);
		InitializeArrayLocation<double><<<1, 1>>>(d_sigma, 1.0, s, V);
		InitializeArray<int><<<blocks, threads>>>(d_d, -1, V); // ? Streams?
		InitializeArrayLocation<int><<<1, 1>>>(d_d, 0, s, V);

		*isAllNodesTraversed = false;
		*level = 0;

		hipDeviceSynchronize(); // ? Is it not reqd wo streams?

		while(!*isAllNodesTraversed) {

			*isAllNodesTraversed = true;
			BreadthFirstSearch<<<blocks, threads>>>(
				V, d_meta, d_data, d_sigma,
				d_d, level, isAllNodesTraversed
			);

			hipDeviceSynchronize();
			(*level)++;
		}

		(*level)--;

		InitializeArray<double><<<blocks, threads>>>(d_delta, 0.0, V);

		while(*level > 0) {
			
			ReversePass<<<blocks, threads>>>(
				V, d_meta, d_data, d_delta, d_sigma,
				d_d, level, isAllNodesTraversed, d_bc
			); 
			
			//? Problem with grid.sync()

			AddElements<<<blocks, threads>>>( 
				V, d_bc, d_delta, d_d, level, s
			);

			hipDeviceSynchronize(); //? Problem with grid.sync()
			
			(*level)--;
		}
		hipDeviceSynchronize();
	}

	hipEventRecord(stop,0);
	hipEventSynchronize(stop);
	hipEventElapsedTime(&milliseconds, start, stop);
	printf("GPU Time: %.6f ms\n", milliseconds);

	hipMemcpy(h_bc, d_bc, sizeof(double) * V, hipMemcpyDeviceToHost);

	//! TODO: Free up memory
}

int main(int argc, char* argv[]) {

	graph G(argv[1]);
	G.parseGraph();

	vector<int> src;

	std::string line;
	std::ifstream srcfile(argv[2]);
	int nodeVal;

	while ( std::getline (srcfile,line) ) {

		std::stringstream ss(line);
		ss>> nodeVal;
		src.push_back(nodeVal);
	}

	srcfile.close();

	double *bc = (double*) malloc(sizeof(double) * G.num_nodes());
	ComputeBetweennessCentrality(G, src, bc);

	// Process bc data
	cout << i << " " << bc[1] << " " << bc[2] << NL;

	for(int i: src)
		cout << i << " " << bc[i] << NL;

	free(bc);

	return 0;
}


// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
// This code is incomplete and work is still going on in this respect.

#ifndef GENCPP_SCC_V4_H
#define GENCPP_SCC_V4_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void vHong(graph& g);



__global__ void vHong_kernel1(int V, int E,int* d_outDeg,int* d_inDeg, int* d_meta, int* d_rev_meta){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int nxtSrc = v + 1; // DEVICE ASSTMENT in .h

  initIndex<int><<<1,1>>>(V,d_inDeg,v,(int)d_inDeg[v] + d_rev_meta[nxtSrc] - d_rev_meta[v]); //InitIndexDevice
  initIndex<int><<<1,1>>>(V,d_outDeg,v,(int)d_outDeg[v] + d_meta[nxtSrc] - d_meta[v]); //InitIndexDevice
} // end KER FUNC
__device__ bool fpoint1 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel2(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_range,int* d_scc,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int havePar = 0; // DEVICE ASSTMENT in .h

    int haveChild = 0; // DEVICE ASSTMENT in .h

    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int par = d_src[edge] ;
      if (d_scc[par] == -1){ // if filter begin 
        if (d_range[par] == d_range[v]){ // if filter begin 
          havePar = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      if (d_scc[dst] == -1){ // if filter begin 
        if (d_range[dst] == d_range[v]){ // if filter begin 
          haveChild = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
      d_scc[v] = v;
      d_isPivot[v] = true;
      fpoint1 = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel3(int V, int E,int* d_scc,int* d_outDeg,int* d_inDeg,int* d_pivotField,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int color = d_range[v]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    int oldSrcValue = -1; // DEVICE ASSTMENT in .h

    int oldSrc = d_pivotField[index]; // DEVICE ASSTMENT in .h

    if (oldSrc >= 0){ // if filter begin 
      oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];

    } // if filter end
    if (oldSrcValue < (d_inDeg[v] + d_outDeg[v])){ // if filter begin 
      d_pivotField[index] = v;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel4(int V, int E,int* d_scc,int* d_pivotField,int* d_range,bool* d_isPivot,bool* d_visitBw,bool* d_visitFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int color = d_range[v]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    if (d_pivotField[index] == v){ // if filter begin 
      d_visitFw[v] = true;
      d_visitBw[v] = true;
      d_isPivot[v] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ bool fpoint2 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel5(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,bool* d_propBw,bool* d_visitBw,bool* d_propFw,bool* d_visitFw,int* d_scc,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int myrange = d_range[v]; // DEVICE ASSTMENT in .h

    if (d_propFw[v] == false && d_visitFw[v] == true){ // if filter begin 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
            d_visitFw[dst] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propFw[v] = true;

    } // if filter end
    if (d_propBw[v] == false && d_visitBw[v] == true){ // if filter begin 
      for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
      {
        int par = d_src[edge] ;
        if (d_scc[par] == -1){ // if filter begin 
          if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
            d_visitBw[par] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propBw[v] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel6(int V, int E,int* d_scc,bool* d_visitFw,bool* d_visitBw,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    if (d_visitFw[v] == false || d_visitBw[v] == false){ // if filter begin 
      int ext = 0; // DEVICE ASSTMENT in .h

      if (d_visitFw[v] == false){ // if filter begin 
        ext = 1;

      } // if filter end
      if (d_visitBw[v] == false){ // if filter begin 
        ext = 2;

      } // if filter end
      int newRange = 3 * d_range[v]; // DEVICE ASSTMENT in .h

      newRange = newRange - newRange / V * V;
      d_range[v] = ext + newRange;
      d_visitFw[v] = false;
      d_visitBw[v] = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel7(int V, int E,int* d_scc,bool* d_visitFw,bool* d_visitBw,bool* d_propBw,bool* d_propFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    if (d_visitFw[v] == false || d_visitBw[v] == false){ // if filter begin 
      d_propFw[v] = false;
      d_propBw[v] = false;

    } // if filter end
    if (d_visitFw[v] == true && d_visitBw[v] == true){ // if filter begin 
      d_scc[v] = v;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel8(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_range,int* d_scc,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int havePar = 0; // DEVICE ASSTMENT in .h

    int haveChild = 0; // DEVICE ASSTMENT in .h

    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int par = d_src[edge] ;
      if (d_scc[par] == -1){ // if filter begin 
        if (d_range[par] == d_range[v]){ // if filter begin 
          havePar = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      if (d_scc[dst] == -1){ // if filter begin 
        if (d_range[dst] == d_range[v]){ // if filter begin 
          haveChild = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
      d_scc[v] = v;
      d_isPivot[v] = true;
      fpoint1 = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel9(int V, int E,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  initIndex<int><<<1,1>>>(V,d_range,v,(int)d_range[v] + v); //InitIndexDevice
} // end KER FUNC
__device__ bool fpoint4 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel10(int V, int E, int* d_meta, int* d_data,int* d_range,int* d_scc){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      if (d_scc[dst] == -1){ // if filter begin 
        if (d_range[dst] < d_range[v]){ // if filter begin 
          d_range[v] = d_range[dst];
          fpoint4 = false;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel11(int V, int E,int* d_scc,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int myrange = d_range[v]; // DEVICE ASSTMENT in .h

    if ((myrange != v) && (myrange != d_range[myrange])){ // if filter begin 
      d_range[v] = d_range[myrange];
      fpoint4 = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ bool fpoint5 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel12(int V, int E,int* d_scc,int* d_outDeg,int* d_inDeg,int* d_pivotField,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int color = d_range[v]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    int oldSrcValue = -1; // DEVICE ASSTMENT in .h

    int oldSrc = d_pivotField[index]; // DEVICE ASSTMENT in .h

    if (oldSrc >= 0){ // if filter begin 
      oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];

    } // if filter end
    if (oldSrcValue < (d_inDeg[v] + d_outDeg[v])){ // if filter begin 
      d_pivotField[index] = v;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel13(int V, int E,int* d_scc,int* d_pivotField,int* d_range,bool* d_isPivot,bool* d_visitBw,bool* d_visitFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int color = d_range[v]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    if (d_pivotField[index] == v){ // if filter begin 
      d_visitFw[v] = true;
      d_visitBw[v] = true;
      d_isPivot[v] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel14(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,bool* d_propBw,bool* d_visitBw,bool* d_propFw,bool* d_visitFw,int* d_scc,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    int myrange = d_range[v]; // DEVICE ASSTMENT in .h

    if (d_propFw[v] == false && d_visitFw[v] == true){ // if filter begin 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
            d_visitFw[dst] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propFw[v] = true;

    } // if filter end
    if (d_propBw[v] == false && d_visitBw[v] == true){ // if filter begin 
      for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
      {
        int par = d_src[edge] ;
        if (d_scc[par] == -1){ // if filter begin 
          if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
            d_visitBw[par] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propBw[v] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel15(int V, int E,int* d_scc,bool* d_visitFw,bool* d_visitBw,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    if (d_visitFw[v] == false || d_visitBw[v] == false){ // if filter begin 
      int ext = 0; // DEVICE ASSTMENT in .h

      if (d_visitFw[v] == false){ // if filter begin 
        ext = 1;

      } // if filter end
      if (d_visitBw[v] == false){ // if filter begin 
        ext = 2;

      } // if filter end
      int newRange = 3 * d_range[v]; // DEVICE ASSTMENT in .h

      newRange = newRange - newRange / V * V;
      d_range[v] = ext + newRange;
      d_visitFw[v] = false;
      d_visitBw[v] = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel16(int V, int E,int* d_scc,bool* d_visitFw,bool* d_visitBw,bool* d_propBw,bool* d_propFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scc[v] == -1){ // if filter begin 
    if (d_visitFw[v] == false || d_visitBw[v] == false){ // if filter begin 
      fpoint5 = false;
      d_propFw[v] = false;
      d_propBw[v] = false;

    } // if filter end
    if (d_visitFw[v] == true && d_visitBw[v] == true){ // if filter begin 
      d_scc[v] = v;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void set_pivot(int V, int* d_pivot, bool* d_isPivot, int* d_range){
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= V) return;
	for (int i = 0; i < V; i++) {
		if (d_isPivot[i] && d_range[i] == d_range[v]) {
			d_pivot[v] = i;
			return;
		}
	}
}
__device__ int get_index(int key, int* d_mapper, int vertices){
	for (int i = 0; i < vertices; i++)
	{
		if (d_mapper[i] == key)
			return i;
	}
	return -1;
}
__device__ int get_index_edge(int source, int destination, int* d_source, int* d_destination, int E)
{
	for (int i = 0; i < E; i++)
	{
		if (d_source[i] == source && d_destination[i] == destination)
			return i;
	}
	return -1;
}

__device__ bool fpoint;

__global__ void create_graph(int V, int E, int vertices, int* d_meta, int* d_data, int* d_source, int* d_destination, int* d_mapper, int* d_edges, bool* d_isPivot, int* d_range, int* d_k, int* d_pivot)
{
	float num_nodes = V;
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= V) return;
	for (int edge = d_meta[v]; edge < d_meta[v + 1]; edge++) {
		int dst = d_data[edge];
		if (d_range[v] != d_range[dst]) {
			int src = get_index(d_pivot[v], d_mapper, vertices);
			int target = get_index(d_pivot[dst], d_mapper, vertices);
			int index = get_index_edge(src, target, d_source, d_destination, E); //This is problematic
			int old = atomicCAS(&d_edges[index], -1, v);
			if (old == -1) {
				int indices = atomicAdd(d_k, 1);
				d_source[indices] = v;
				d_destination[indices] = dst;
			}
		}
	}
}
__global__ void update_condense_graph(int vertices, int E, int* d_source, int* d_destination, int* d_k, int* d_mark, int* d_pivot, int* d_mapper)
{
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= E) return;
	int src = get_index(d_pivot[d_mark[2 * v]], d_mapper, vertices);
	int dst = get_index(d_pivot[d_mark[2 * v + 1]], d_mapper, vertices);
	int flag = 0;
	if (src != -1 && dst != -1)
	{
		for (int i = 0; i < E; i++)
		{
			if (d_source[i] == src && d_destination[i] == dst)
			{
				flag = 1;
			}
		}
		if (flag == 0)
		{
			int indices = atomicAdd(d_k, 1);
			d_source[indices] = src;
			d_destination[indices] = dst;
		}
	}
}
__global__ void check_cycle(int vertices, int E, int* d_source, int* d_destination, int* d_k, int* d_mark, int* d_mapper, int* d_pivot, int* d_edges, int* locks, int* path, int* d_vertices_mapper)
{
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= E) return;
	bool* visited = (bool *)malloc(vertices * sizeof(bool));
	int* queue = (int *)malloc(vertices * sizeof(int));
	int source = get_index(d_pivot[d_mark[2 * v]], d_mapper, vertices);
	int target = get_index(d_pivot[d_mark[2 * v + 1]], d_mapper, vertices);
	int index_path = 0;
	int front = 0;
	int rear = 0;
	queue[rear++] = source;
	while (front != (rear + 1))
	{
		int s = queue[rear - 1];
		path[index_path++] = s;
		front++;
		if (!visited[s])
		{
			visited[s] = true;
		}
		else
		{
			int i = s;
			int representation = i;
			int index = atomicAdd(d_k, 1);
			if (d_edges[index] == -1 || d_edges[index] > v)
			{
				d_edges[index] = v;
				d_vertices_mapper[index] = s;
			}
			while (path[i] != s)
			{
				int dst = i;
				int src = path[i];
				i = path[i];
				index = atomicAdd(d_k, 1);
				if (d_edges[index] == -1 || d_edges[index] > v)
				{
					d_edges[index] = v;
					d_vertices_mapper[index] = src;
				}
			}
		}
		for (int i = 0; i < E; i++)
		{
			if (d_source[i] == s)
			{
				queue[rear++] = d_destination[i];
				path[d_destination[i]] = d_source[i];
			}
		}
	}
	free(visited);
	free(path);
	free(queue);
}
__global__ void check_cycle_repeat(int vertices, int E, int* d_source, int* d_destination, int* d_k, int* d_mark, int* d_mapper, int* d_pivot, int* d_edges, int* path, int* d_isPivot, int* d_mapper_vertices)
{
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= E) return;
	int source = get_index(d_pivot[d_mark[2 * v]], d_mapper, vertices);
	int destination = get_index(d_pivot[d_mark[2 * v + 1]], d_mapper, vertices);
	bool* visited = (bool *)malloc(vertices * sizeof(bool));
	int* queue = (int *)malloc(vertices * sizeof(int));
	int index_path = 0;
	int front = 0;
	int rear = 0;
	queue[rear++] = source;
	int s;
	while (front != (rear + 1))
	{
		s = queue[rear - 1];
		front++;
		if (!visited[s])
		{
			visited[s] = true;
		}
		else
		{
			int i = s;
			int representation = i;
			rear--;
			while (queue[--rear] != s)
			{
				int dst = i;
				int src = queue[rear];
				int index = get_index_edge(source, destination, d_source, d_destination, E); // This is problematic
				d_isPivot[d_pivot[src]] = false;
				d_pivot[src] = d_pivot[s];
				d_mapper[src] = -1;
				for (int j = 0; d_source[j] != -1; j++)
				{
					if (d_source[j] == src)
					{
						d_source[j] = representation;
					}
					if (d_destination[j] == dst)
					{
						d_destination[j] = representation;
					}
				}
			}
		}
		for (int i = 0; i < E; i++)
		{
			if (d_source[i] == s)
			{
				queue[rear++] = d_destination[i];
				path[d_destination[i]] = d_source[i];
			}
		}
	}
	free(visited);
	free(path);
	free(queue);
}
__global__ void clean_graph(int E, int* d_source, int* d_destination)
{
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= E) return;
	if (d_source[v] == d_destination[v])
	{
		d_source[v] = d_source[v + 1];
		d_destination[v] = d_destination[v + 1];
		fpoint = true;
	}
	if (d_source[v] == d_source[v + 1] && d_destination[v] == d_destination[v + 1])
	{
		d_source[v] = -1;
		d_destination[v] = -1;
		fpoint = true;
	}
}			
__global__ void graph_recolour(int V, int* d_range, int* d_pivot)
{
	unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
	if (v >= V) return;
	int i = v;
	while (i != d_pivot[i])
	{
		i = d_pivot[i];
	}
	d_range[v] = d_range[i];
}
#endif

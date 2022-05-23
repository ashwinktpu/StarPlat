#include <vector>
#include <chrono>
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../graph.hpp"
#include "../libcuda.cuh"

// kernel for computing SSSP of static graph
__global__ void sssp_kernel(int *d_meta, int *d_data, int *d_weight, int *d_dist,
                            int *d_parent, int *d_locks, int V, bool *d_modified,
                            bool *d_modified_next, bool *d_finished)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V && d_modified[id] == true && d_dist[id] != INT_MAX)
  {
    int distToCurNode = d_dist[id];
    int v, newDist, lock;
    bool gotLock;
    for (int e = d_meta[id]; e < d_meta[id + 1]; e++)
    {
      gotLock = false;
      v = d_data[e];
      newDist = distToCurNode + d_weight[e];
      do
      {
        if (gotLock == false)
          lock = atomicCAS(&d_locks[v], 0, 1);
        if (lock == 0 && newDist < d_dist[v])
        {
          d_dist[v] = newDist;
          d_parent[v] = id;
          d_modified_next[v] = true;
          *d_finished = false;
        }
        if (lock == 0)
        {
          gotLock = true;
          lock = 1;
          atomicExch(&d_locks[v], 0);
        }
      } while (gotLock == false);
    }
  }
}

// kernel for marking all the descendants of modified nodes in SPT
__global__ void mark_descendants(int *d_dist, int *d_parent, bool *d_modified, int V,
                                 bool *d_finished)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V && d_modified[id] == false)
  {
    int parent = d_parent[id];
    if (parent >= 0 && d_modified[parent] == true)
    {
      d_modified[id] = true;
      *d_finished = false;
    }
  }
}

// kernel for setting distance and parent of marked nodes
__global__ void mark_not_reachable(int *d_dist, int *d_parent, int V, bool *d_modified)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V && d_modified[id] == true)
  {
    d_dist[id] = INT_MAX;
    d_parent[id] = -1;
  }
}

// kernel for updating the distance and parent of marked nodes (as a result of deletions)
__global__ void fetch_and_update(int *d_metaR, int *d_dataR, int *d_weightR,
                                 int *d_diffMetaR, int *d_diffDataR, int *d_diffWeightR,
                                 int *d_dist, int *d_parent, int V,
                                 bool *d_modified, bool *d_finished)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V && d_modified[id] == true)
  {
    int u;
    for (int e = d_metaR[id]; e < d_metaR[id + 1]; e++)
    {
      u = d_dataR[e];
      if (u != INT_MAX && d_dist[u] != INT_MAX)
      {
        if (d_dist[id] > d_dist[u] + d_weightR[e])
        {
          d_dist[id] = d_dist[u] + d_weightR[e];
          d_parent[id] = u;
          *d_finished = false;
        }
      }
    }
    for (int e = d_diffMetaR[id]; e < d_diffMetaR[id + 1]; e++)
    {
      u = d_diffDataR[e];
      if (u != INT_MAX && d_dist[u] != INT_MAX)
      {
        if (d_dist[id] > d_dist[u] + d_diffWeightR[e])
        {
          d_dist[id] = d_dist[u] + d_diffWeightR[e];
          d_parent[id] = u;
          *d_finished = false;
        }
      }
    }
  }
}

// kernel for updating the distance and parent of marked nodes (as a result of additions)
__global__ void push_and_update(int *d_meta, int *d_data, int *d_weight, int *d_dist, int *d_parent,
                                int *d_diffMeta, int *d_diffData, int *d_diffWeight,
                                int *d_locks, int V, bool *d_modified, bool *d_finished)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V && d_modified[id] == true && d_dist[id] != INT_MAX)
  {
    int distToCurNode = d_dist[id];
    int v, newDist, lock;
    bool gotLock;
    for (int e = d_meta[id]; e < d_meta[id + 1]; e++)
    {
      if (d_data[e] != INT_MAX)
      {
        gotLock = false;
        v = d_data[e];
        newDist = distToCurNode + d_weight[e];
        do
        {
          if (gotLock == false)
            lock = atomicCAS(&d_locks[v], 0, 1);
          if (lock == 0 && newDist < d_dist[v])
          {
            d_dist[v] = newDist;
            d_parent[v] = id;
            d_modified[v] = true;
            *d_finished = false;
          }
          if (lock == 0)
          {
            gotLock = true;
            lock = 1;
            atomicExch(&d_locks[v], 0);
          }
        } while (gotLock == false);
      }
    }
    for (int e = d_diffMeta[id]; e < d_diffMeta[id + 1]; e++)
    {
      if (d_diffData[e] != INT_MAX)
      {
        gotLock = false;
        v = d_diffData[e];
        newDist = distToCurNode + d_diffWeight[e];
        do
        {
          if (gotLock == false)
            lock = atomicCAS(&d_locks[v], 0, 1);
          if (lock == 0 && newDist < d_dist[v])
          {
            d_dist[v] = newDist;
            d_parent[v] = id;
            d_modified[v] = true;
            *d_finished = false;
          }
          if (lock == 0)
          {
            gotLock = true;
            lock = 1;
            atomicExch(&d_locks[v], 0);
          }
        } while (gotLock == false);
      }
    }
  }
}

// check for cudaError
void checkCudaError()
{
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess)
  {
    printf("CUDA error: %s\n", cudaGetErrorString(error));
  }
}

void Compute_SSSP(graph &g, int *dist, int *parent, int *weight, int src)
{
  // data on host
  int V = g.num_nodes();
  int E = g.num_edges();
  bool finished;
  int *h_meta = g.indexofNodes;
  int *h_data = g.edgeList;
  bool *h_modified = new bool[V];

  // data on device
  int *d_meta, *d_data, *d_weight;
  int *d_dist, *d_parent, *d_locks;
  bool *d_finished, *d_modified, *d_modified_next;

  // for recording the total time taken
  float milliseconds = 0;
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start, 0);

  // initialize host data
  for (int i = 0; i < V; i++)
  {
    dist[i] = INT_MAX;
    parent[i] = -1;
    h_modified[i] = false;
  }
  dist[src] = 0;
  h_modified[src] = true;
  finished = false;

  // allocate memory on device
  cudaMalloc(&d_meta, sizeof(int) * (V + 1));
  cudaMalloc(&d_data, sizeof(int) * (E));
  cudaMalloc(&d_weight, sizeof(int) * (E));
  cudaMalloc(&d_dist, sizeof(int) * V);
  cudaMalloc(&d_parent, sizeof(int) * V);
  cudaMalloc(&d_locks, sizeof(int) * V);
  cudaMalloc(&d_finished, sizeof(bool));
  cudaMalloc(&d_modified, sizeof(bool) * V);
  cudaMalloc(&d_modified_next, sizeof(bool) * V);

  // copy to device
  cudaMemcpy(d_meta, h_meta, sizeof(int) * (V + 1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_data, h_data, sizeof(int) * (E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, weight, sizeof(int) * (E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_dist, dist, sizeof(int) * (V), cudaMemcpyHostToDevice);
  cudaMemcpy(d_parent, parent, sizeof(int) * (V), cudaMemcpyHostToDevice);
  cudaMemcpy(d_modified, h_modified, sizeof(bool) * (V), cudaMemcpyHostToDevice);

  // launch config
  const int numThreads = 1024;
  const int numBlocks = (V + numThreads - 1) / numThreads;

  // call kernel to compute the SSSP
  int iter = 0;
  init_kernel<bool><<<numBlocks, numThreads>>>(d_modified_next, false, V);
  init_kernel<int><<<numBlocks, numThreads>>>(d_locks, 0, V);
  while (finished != true)
  {
    init_kernel<bool><<<1, 1>>>(d_finished, true, 1);
    sssp_kernel<<<numBlocks, numThreads>>>(d_meta, d_data, d_weight, d_dist, d_parent,
                                           d_locks, V, d_modified, d_modified_next, d_finished);
    init_kernel<bool><<<numBlocks, numThreads>>>(d_modified, false, V);
    cudaMemcpy(&finished, d_finished, sizeof(bool), cudaMemcpyDeviceToHost);

    bool *tempPtr = d_modified_next;
    d_modified_next = d_modified;
    d_modified = tempPtr;

    if (++iter >= V - 1)
      break;
  }
  cudaDeviceSynchronize();
  checkCudaError();

  // copy distances and parents back to host
  cudaMemcpy(dist, d_dist, sizeof(int) * (V), cudaMemcpyDeviceToHost);
  cudaMemcpy(parent, d_parent, sizeof(int) * (V), cudaMemcpyDeviceToHost);

  // print time taken
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Initial graph, SSSP using GPU - time taken: %.3f ms \n", milliseconds);

  // free up the memory
  free(h_modified);
  cudaFree(d_meta);
  cudaFree(d_data);
  cudaFree(d_weight);
  cudaFree(d_dist);
  cudaFree(d_parent);
  cudaFree(d_locks);
  cudaFree(d_finished);
  cudaFree(d_modified);
  cudaFree(d_modified_next);

  unsigned long long sum = 0;
  for (int i = 0; i < V; i++)
  {
    if (dist[i] != INT_MAX)
      sum += dist[i];
  }
  printf("Initial graph, total path sum = %llu\n", sum);
}

void Compute_dynamicSSSPDel(graph &g, int *dist, int *parent, bool *modified)
{
  // data on host
  int V = g.num_nodes();
  int E = g.num_edges_CSR();
  int diffE1 = g.num_edges_diffCSR();
  int diffE2 = g.num_edges_diffCSC();
  bool finished;
  int *h_metaR = g.rev_indexofNodes;
  int *h_dataR = g.srcList;
  int *h_weightR = g.getRev_edgeLen();
  int *h_diffMetaR = g.diff_rev_indexofNodes;
  int *h_diffDataR = g.diff_rev_edgeList;
  int *h_diffWeightR = g.getDiff_rev_edgeLen();

  // data on device
  int *d_metaR, *d_dataR, *d_weightR;
  int *d_diffMetaR, *d_diffDataR, *d_diffWeightR;
  int *d_dist, *d_parent;
  bool *d_finished, *d_modified;

  // for recording the total time taken
  float milliseconds = 0;
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start, 0);

  // initialize host data
  finished = false;

  // allocate memory on device
  cudaMalloc(&d_metaR, sizeof(int) * (V + 1));
  cudaMalloc(&d_dataR, sizeof(int) * (E));
  cudaMalloc(&d_weightR, sizeof(int) * (E));
  cudaMalloc(&d_diffMetaR, sizeof(int) * (V + 1));
  cudaMalloc(&d_diffDataR, sizeof(int) * (diffE2));
  cudaMalloc(&d_diffWeightR, sizeof(int) * (diffE2));
  cudaMalloc(&d_dist, sizeof(int) * V);
  cudaMalloc(&d_parent, sizeof(int) * V);
  cudaMalloc(&d_finished, sizeof(bool));
  cudaMalloc(&d_modified, sizeof(bool) * V);

  // copy to device
  cudaMemcpy(d_metaR, h_metaR, sizeof(int) * (V + 1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_dataR, h_dataR, sizeof(int) * (E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weightR, h_weightR, sizeof(int) * (E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_diffMetaR, h_diffMetaR, sizeof(int) * (V + 1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_diffDataR, h_diffDataR, sizeof(int) * (diffE2), cudaMemcpyHostToDevice);
  cudaMemcpy(d_diffWeightR, h_diffWeightR, sizeof(int) * (diffE2), cudaMemcpyHostToDevice);
  cudaMemcpy(d_dist, dist, sizeof(int) * (V), cudaMemcpyHostToDevice);
  cudaMemcpy(d_parent, parent, sizeof(int) * (V), cudaMemcpyHostToDevice);
  cudaMemcpy(d_modified, modified, sizeof(bool) * (V), cudaMemcpyHostToDevice);

  // launch config
  const int numThreads = 1024;
  const int numBlocks = (V + numThreads - 1) / numThreads;

  // mark the descendants of modified nodes as modified
  int iter = 0;
  finished = false;
  while (finished != true)
  {
    init_kernel<bool><<<1, 1>>>(d_finished, true, 1);
    mark_descendants<<<numBlocks, numThreads>>>(d_dist, d_parent, d_modified,
                                                V, d_finished);
    cudaMemcpy(&finished, d_finished, sizeof(bool), cudaMemcpyDeviceToHost);
    if (++iter >= V - 1)
      break;
  }
  cudaDeviceSynchronize();
  checkCudaError();

  // set the distance and parent of marked nodes (INT_MAX, -1)
  mark_not_reachable<<<numBlocks, numThreads>>>(d_dist, d_parent, V, d_modified);
  cudaDeviceSynchronize();
  checkCudaError();

  // update the distances and parents (pull based approach)
  iter = 0;
  finished = false;
  while (finished != true)
  {
    init_kernel<bool><<<1, 1>>>(d_finished, true, 1);
    fetch_and_update<<<numBlocks, numThreads>>>(d_metaR, d_dataR, d_weightR,
                                                d_diffMetaR, d_diffDataR, d_diffWeightR,
                                                d_dist, d_parent, V, d_modified, d_finished);
    cudaMemcpy(&finished, d_finished, sizeof(bool), cudaMemcpyDeviceToHost);
    if (++iter >= V - 1)
      break;
  }
  cudaDeviceSynchronize();
  checkCudaError();

  // copy distances and parents back to host
  cudaMemcpy(dist, d_dist, sizeof(int) * (V), cudaMemcpyDeviceToHost);
  cudaMemcpy(parent, d_parent, sizeof(int) * (V), cudaMemcpyDeviceToHost);

  // print time taken
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Compute DynSSSP-Del - time taken: %.3f ms \n", milliseconds);

  // free up the memory
  cudaFree(d_metaR);
  cudaFree(d_dataR);
  cudaFree(d_weightR);
  cudaFree(d_diffMetaR);
  cudaFree(d_diffDataR);
  cudaFree(d_diffWeightR);
  cudaFree(d_dist);
  cudaFree(d_parent);
  cudaFree(d_finished);
  cudaFree(d_modified);
}

void Compute_dynamicSSSPAdd(graph &g, int *dist, int *parent, bool *modified)
{
  // data on host
  int V = g.num_nodes();
  int E = g.num_edges_CSR();
  int diffE1 = g.num_edges_diffCSR();
  int diffE2 = g.num_edges_diffCSC();
  bool finished;
  int *h_meta = g.indexofNodes;
  int *h_data = g.edgeList;
  int *h_weight = g.getEdgeLen();
  int *h_diffMeta = g.diff_indexofNodes;
  int *h_diffData = g.diff_edgeList;
  int *h_diffWeight = g.getDiff_edgeLen();

  // data on device
  int *d_meta, *d_data, *d_weight;
  int *d_diffMeta, *d_diffData, *d_diffWeight;
  int *d_dist, *d_parent, *d_locks;
  bool *d_finished, *d_modified;

  // for recording the total time taken
  float milliseconds = 0;
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start, 0);

  // initialize host data
  finished = false;

  // allocate memory on device
  cudaMalloc(&d_meta, sizeof(int) * (V + 1));
  cudaMalloc(&d_data, sizeof(int) * (E));
  cudaMalloc(&d_weight, sizeof(int) * (E));
  cudaMalloc(&d_diffMeta, sizeof(int) * (V + 1));
  cudaMalloc(&d_diffData, sizeof(int) * (diffE1));
  cudaMalloc(&d_diffWeight, sizeof(int) * (diffE1));
  cudaMalloc(&d_dist, sizeof(int) * V);
  cudaMalloc(&d_parent, sizeof(int) * V);
  cudaMalloc(&d_locks, sizeof(int) * V);
  cudaMalloc(&d_finished, sizeof(bool));
  cudaMalloc(&d_modified, sizeof(bool) * V);

  // copy to device
  cudaMemcpy(d_meta, h_meta, sizeof(int) * (V + 1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_data, h_data, sizeof(int) * (E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int) * (E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_diffMeta, h_diffMeta, sizeof(int) * (V + 1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_diffData, h_diffData, sizeof(int) * (diffE1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_diffWeight, h_diffWeight, sizeof(int) * (diffE1), cudaMemcpyHostToDevice);
  cudaMemcpy(d_dist, dist, sizeof(int) * (V), cudaMemcpyHostToDevice);
  cudaMemcpy(d_parent, parent, sizeof(int) * (V), cudaMemcpyHostToDevice);
  cudaMemcpy(d_modified, modified, sizeof(bool) * (V), cudaMemcpyHostToDevice);

  // launch config
  const int numThreads = 1024;
  const int numBlocks = (V + numThreads - 1) / numThreads;

  // update the distances and parents (push based approach)
  int iter = 0;
  init_kernel<int><<<numBlocks, numThreads>>>(d_locks, 0, V);
  while (finished != true)
  {
    init_kernel<bool><<<1, 1>>>(d_finished, true, 1);
    push_and_update<<<numBlocks, numThreads>>>(d_meta, d_data, d_weight, d_dist, d_parent,
                                               d_diffMeta, d_diffData, d_diffWeight,
                                               d_locks, V, d_modified, d_finished);
    cudaMemcpy(&finished, d_finished, sizeof(bool), cudaMemcpyDeviceToHost);

    if (++iter >= V - 1)
      break;
  }
  cudaDeviceSynchronize();
  checkCudaError();

  // copy distances and parents back to host
  cudaMemcpy(dist, d_dist, sizeof(int) * (V), cudaMemcpyDeviceToHost);
  cudaMemcpy(parent, d_parent, sizeof(int) * (V), cudaMemcpyDeviceToHost);

  // print time taken
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Compute DynSSSP-Add - time taken: %.3f ms \n", milliseconds);

  // free up the memory
  cudaFree(d_meta);
  cudaFree(d_data);
  cudaFree(d_weight);
  cudaFree(d_diffMeta);
  cudaFree(d_diffData);
  cudaFree(d_diffWeight);
  cudaFree(d_dist);
  cudaFree(d_parent);
  cudaFree(d_locks);
  cudaFree(d_finished);
  cudaFree(d_modified);
}

int main(int argc, char **argv)
{
  graph G("../inputGraphs/inputs/pokec.txt", "cuda", true);
  G.parseGraph();

  int *dist = new int[G.num_nodes()];
  int *parent = new int[G.num_nodes()];
  int *edgeLen = G.getEdgeLen();
  Compute_SSSP(G, dist, parent, edgeLen, 0);

  bool *modified_add = new bool[G.num_nodes()];
  bool *modified_del = new bool[G.num_nodes()];
  int elements = 0;

  std::vector<update> updateEdges = G.parseUpdates("../inputGraphs/updates/update_pokec.txt");

  int percent = atoi(argv[1]);
  int updateSize = percent * 0.01 * G.num_edges();
  if (!G.isGraphDirected() && updateSize & 1)
    updateSize++;
  int batchSize = updateSize;
  // int batchSize = 1000000;

  std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
  std::chrono::duration<double, std::milli> timeTaken;

  startTime = std::chrono::high_resolution_clock::now();
  for (int k = 0; k < updateSize; k += batchSize)
  {
    if ((k + batchSize) > updateSize)
      elements = updateSize - k;
    else
      elements = batchSize;

    for (int i = 0; i < G.num_nodes(); i++)
    {
      modified_add[i] = false;
      modified_del[i] = false;
    }

    for (int i = k; i < (k + batchSize) && i < updateSize; i++)
    {
      if (updateEdges[i].type == 'd')
      {
        update u = updateEdges[i];
        int src = u.source;
        int dest = u.destination;

        if (parent[dest] >= 0 && parent[dest] == src)
        {
          dist[dest] = INT_MAX;
          parent[dest] = -1;
          modified_del[dest] = true;
        }
      }
    }

    G.updateCSR_Del(updateEdges, k, elements);
    Compute_dynamicSSSPDel(G, dist, parent, modified_del);

    for (int i = k; i < (k + batchSize) && i < updateSize; i++)
    {
      if (updateEdges[i].type == 'a')
      {
        update u = updateEdges[i];
        int src = u.source;
        int dest = u.destination;
        int weight = u.weight;
        if (dist[src] != INT_MAX && dist[src] + weight < dist[dest])
          modified_add[src] = true;
      }
    }

    G.updateCSR_Add(updateEdges, k, elements);
    Compute_dynamicSSSPAdd(G, dist, parent, modified_add);
  }
  endTime = std::chrono::high_resolution_clock::now();
  timeTaken = endTime - startTime;
  printf("Total time taken for %d percent updates: %.3f ms\n", percent, timeTaken.count());

  unsigned long long sum = 0;
  for (int i = 0; i < G.num_nodes(); i++)
  {
    if (dist[i] != INT_MAX)
      sum += dist[i];
  }
  printf("Final graph, total path sum = %llu\n\n", sum);

  free(dist);
  free(parent);
  free(modified_add);
  free(modified_del);

  return 0;
}
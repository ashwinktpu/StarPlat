#include <cuda.h>

// kernel to delete edges from CSR
__global__ void del_edges_CSR(int size, update *d_batch,
                              int32_t *d_indexofNodes, int32_t *d_edgeList,
                              int32_t *d_rev_indexofNodes, int32_t *d_srcList)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < size && d_batch[id].type == 'd')
  {
    int u = d_batch[id].source;
    int v = d_batch[id].destination;
    for (int i = d_indexofNodes[u]; i < d_indexofNodes[u + 1]; i++)
    {
      if (d_edgeList[i] == v)
      {
        d_batch[id].type = 'x';
        d_edgeList[i] = INT_MAX;
        break;
      }
    }
    for (int i = d_rev_indexofNodes[v]; i < d_rev_indexofNodes[v + 1]; i++)
    {
      if (d_srcList[i] == u)
      {
        if (d_batch[id].type == 'x')
          d_batch[id].type = 'z';
        else
          d_batch[id].type = 'y';
        d_srcList[i] = INT_MAX;
        break;
      }
    }
  }
}

// kernel to delete edges from diffCSR
__global__ void del_edges_diffCSR(int size, update *d_batch,
                                  int32_t *d_diff_indexofNodes, int32_t *d_diff_edgeList,
                                  int32_t *d_diff_rev_indexofNodes, int32_t *d_diff_rev_edgeList)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < size)
  {
    int u = d_batch[id].source;
    int v = d_batch[id].destination;
    if (d_batch[id].type == 'd' || d_batch[id].type == 'y')
    {
      for (int i = d_diff_indexofNodes[u]; i < d_diff_indexofNodes[u + 1]; i++)
      {
        if (d_diff_edgeList[i] == v)
        {
          if (d_batch[id].type == 'y')
            d_batch[id].type = 'z';
          else
            d_batch[id].type = 'x';
          d_diff_edgeList[i] = INT_MAX;
          break;
        }
      }
    }
    if (d_batch[id].type == 'd' || d_batch[id].type == 'x')
    {
      for (int i = d_diff_rev_indexofNodes[v]; i < d_diff_rev_indexofNodes[v + 1]; i++)
      {
        if (d_diff_rev_edgeList[i] == u)
        {
          d_batch[id].type = 'z';
          d_diff_rev_edgeList[i] = INT_MAX;
          break;
        }
      }
    }
  }
}

// kernel for initializing an array
template <typename T>
__global__ void init_kernel(T *array, T val, int arraySize)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < arraySize)
    array[id] = val;
}

// kernel for adding edges in to the CSR and marking modified nodes
__global__ void add_edges_CSR(int size, update *d_batch,
                              int *d_indexofNodes, int *d_edgeList, int *d_edgeLen,
                              int *d_rev_indexofNodes, int *d_srcList, int *d_rev_edgeLen,
                              int *d_diffCsrOffsets_next, int *d_diffCsrOffsetsR_next)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < size && d_batch[id].type == 'a')
  {
    int u = d_batch[id].source;
    int v = d_batch[id].destination;
    int w = d_batch[id].weight;
    int flag = 0, flagR = 0;
    for (int i = d_indexofNodes[u]; i < d_indexofNodes[u + 1]; i++)
    {
      if (d_edgeList[i] == INT_MAX)
      {
        flag = atomicCAS(&d_edgeList[i], INT_MAX, v);
        if (flag == INT_MAX)
        {
          d_batch[id].type = 'x';
          d_edgeLen[i] = w;
          break;
        }
      }
    }
    for (int i = d_rev_indexofNodes[v]; i < d_rev_indexofNodes[v + 1]; i++)
    {
      if (d_srcList[i] == INT_MAX)
      {
        flagR = atomicCAS(&d_srcList[i], INT_MAX, u);
        if (flagR == INT_MAX)
        {
          if (d_batch[id].type == 'a')
            d_batch[id].type = 'y';
          else
            d_batch[id].type = 'z';
          d_rev_edgeLen[i] = w;
          break;
        }
      }
    }

    if (flag != INT_MAX)
      atomicAdd(&d_diffCsrOffsets_next[u + 1], 1);
    if (flagR != INT_MAX)
      atomicAdd(&d_diffCsrOffsetsR_next[v + 1], 1);
  }
}

// kernel for counting valid edges in diffCSR (for updating offset values)
__global__ void add_diff_edge_counts(int V, int *d_diff_indexofNodes, int *d_diff_edgeList,
                                     int *d_diff_edgeLen, int *d_diff_rev_indexofNodes, int *d_diff_rev_edgeList,
                                     int *d_diff_rev_edgeLen, int *d_diffCsrOffsets_next,
                                     int *d_diffCsrOffsetsR_next)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V)
  {
    for (int i = d_diff_indexofNodes[id]; i < d_diff_indexofNodes[id + 1]; i++)
    {
      if (d_diff_edgeList[i] != INT_MAX)
        d_diffCsrOffsets_next[id + 1] += 1;
    }
    for (int i = d_diff_rev_indexofNodes[id]; i < d_diff_rev_indexofNodes[id + 1]; i++)
    {
      if (d_diff_rev_edgeList[i] != INT_MAX)
        d_diffCsrOffsetsR_next[id + 1] += 1;
    }
  }
}

// kernel for copying values from one array to another array
template <typename T>
__global__ void copy(T *destArr, T *srcArr, int arraySize)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < arraySize)
    destArr[id] = srcArr[id];
}

// kernel for prefix sum
__global__ void prefix_sum(int V, int off, int *d_diffCsrOffsets_next,
                           int *d_diffCsrOffsetsR_next, int *d_tempArr, int *d_tempArrR)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if ((id <= V) && (id >= off))
  {
    d_diffCsrOffsets_next[id] += d_tempArr[id - off];
    d_diffCsrOffsetsR_next[id] += d_tempArrR[id - off];
  }
}

// kernel for copying the edges from old diffCSR to new diffCSR arrays
__global__ void copy_edges_diffcsr(int V, int *d_diff_indexofNodes, int *d_diff_edgeList, int *d_diff_edgeLen,
                                   int *d_diff_rev_indexofNodes, int *d_diff_rev_edgeList, int *d_diff_rev_edgeLen,
                                   int *d_diffCsrOffsets_next, int *d_diffCsrCords_next, int *d_diffCsrWeights_next,
                                   int *d_diffCsrOffsetsR_next, int *d_diffCsrCordsR_next, int *d_diffCsrWeightsR_next)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < V)
  {
    int j = d_diffCsrOffsets_next[id];
    for (int i = d_diff_indexofNodes[id]; i < d_diff_indexofNodes[id + 1]; i++)
    {
      if (d_diff_edgeList[i] != INT_MAX)
      {
        d_diffCsrCords_next[j] = d_diff_edgeList[i];
        d_diffCsrWeights_next[j] = d_diff_edgeLen[i];
        j++;
      }
    }
    j = d_diffCsrOffsetsR_next[id];
    for (int i = d_diff_rev_indexofNodes[id]; i < d_diff_rev_indexofNodes[id + 1]; i++)
    {
      if (d_diff_rev_edgeList[i] != INT_MAX)
      {
        d_diffCsrCordsR_next[j] = d_diff_rev_edgeList[i];
        d_diffCsrWeightsR_next[j] = d_diff_rev_edgeLen[i];
        j++;
      }
    }
  }
}

// kernel for adding remaining edges (from udpates) to diffCSR
__global__ void add_edges_diffcsr(int size, update *d_batch,
                                  int *d_diff_indexofNodes, int *d_diff_edgeList, int *d_diff_edgeLen,
                                  int *d_diff_rev_indexofNodes, int *d_diff_rev_edgeList, int *d_diff_rev_edgeLen)
{
  unsigned int id = blockDim.x * blockIdx.x + threadIdx.x;
  if (id < size && (d_batch[id].type == 'a' || d_batch[id].type == 'x' || d_batch[id].type == 'y'))
  {
    int u = d_batch[id].source;
    int v = d_batch[id].destination;
    int w = d_batch[id].weight;
    if (d_batch[id].type == 'a' || d_batch[id].type == 'y')
    {
      for (int i = d_diff_indexofNodes[u]; i < d_diff_indexofNodes[u + 1]; i++)
      {
        if (d_diff_edgeList[i] == INT_MAX)
        {
          if (atomicCAS(&d_diff_edgeList[i], INT_MAX, v) == INT_MAX)
          {
            d_diff_edgeLen[i] = w;
            break;
          }
        }
      }
    }
    if (d_batch[id].type == 'a' || d_batch[id].type == 'x')
    {
      for (int i = d_diff_rev_indexofNodes[v]; i < d_diff_rev_indexofNodes[v + 1]; i++)
      {
        if (d_diff_rev_edgeList[i] == INT_MAX)
        {
          if (atomicCAS(&d_diff_rev_edgeList[i], INT_MAX, u) == INT_MAX)
          {
            d_diff_rev_edgeLen[i] = w;
            break;
          }
        }
      }
    }
  }
}
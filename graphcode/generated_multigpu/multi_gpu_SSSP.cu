#include <iostream>
#include <iomanip>
#include <cuda_runtime.h>
#include <vector>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <ratio>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include <climits>
#include <algorithm>
#include <assert.h> 

using namespace std;

const int INF = 1000 * 1000 * 1000;

#define cudaCheckError()                                                                     \
    {                                                                                        \
        cudaError_t e = cudaGetLastError();                                                  \
        if (e != cudaSuccess)                                                                \
        {                                                                                    \
            printf("Cuda failure %s:%d: '%s'\n", __FILE__, __LINE__, cudaGetErrorString(e)); \
            exit(0);                                                                         \
        }                                                                                    \
    }

struct edge
{
    int from, to, wt = 0;
};


template <typename T>
__global__ void initKernel(int V, T *init_array, T init_value)
{
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V)
    {
        init_array[id] = init_value;
    }
}

template <typename T>
__global__ void initKernel0(T *init_array, T id, T init_value)
{ 
    init_array[id] = init_value;
}

__global__ void SSSPKernel(int vertex_partition_start, int vertex_partition_end, int *gpu_dist, int *gpu_modified, struct edge *adj, int *offset)
{
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    int num_vertices = vertex_partition_end - vertex_partition_start;
    if (id < num_vertices)
    {
        int vertex = vertex_partition_start + id;
        //printf("%d\n",vertex);
        for (int i = offset[vertex]; i < offset[vertex + 1]; i++)
        {
            int u = adj[i].to, v = adj[i].from, w = adj[i].wt;
            //assert(v == vertex);
            int new_dist = gpu_dist[u] + w;
           // printf("vertex %d %d %d %d\n", v,u,w,new_dist);
            if (gpu_dist[v] > new_dist)
            {
                gpu_dist[v] = new_dist;
                gpu_modified[0] = 1;
            }
        }
    }
}

void sssp(int n, int m, struct edge *adj, int *offset)
{
    int devices = 1;
    //cudaGetDeviceCount(&devices);
    cout << "Num devices = " << devices << endl;

    int *vertex_partition_start = (int *)malloc((devices + 1) * sizeof(int));
    int vertex_per_gpu = n / devices;
    int curr = 0;
    vertex_partition_start[0] = 0;
    vertex_partition_start[devices] = n;
    for (int i = 1; i < devices; i++)
    {
        if (i <= (n % devices))
        {
            curr += vertex_per_gpu + 1;
        }
        else
            curr += vertex_per_gpu;
        vertex_partition_start[i] = curr;
    }

    int block_size = n;
    int num_blocks = 1;
    if (n > 1024)
    {
        block_size = 1024;
        num_blocks = (n + block_size - 1) / block_size; // avoid ceil fun call
    }

    cudaCheckError(); 

    cudaStream_t *streams;
    streams = (cudaStream_t *)malloc(devices * sizeof(cudaStream_t));
    for(int i=0 ;i< devices;i++){
        cudaSetDevice(i);
        cudaStreamCreate(&streams[i]);
    }

    int *host_dist, **device_dist;
    cudaMallocHost(&host_dist, n * sizeof(int));
    cudaMallocHost(&device_dist, devices * sizeof(int *));
    for (int i = 0; i < devices; i++)
    {
        cudaSetDevice(i);
        cudaMalloc(&device_dist[i], n * sizeof(int));
        initKernel<<<num_blocks, block_size,0,streams[i]>>>(n, device_dist[i], INF);
        initKernel0<<<1, 1,0,streams[i]>>>(device_dist[i], 0, 0);
    }
    

    cudaCheckError();

    int **host_modified, **device_modified;
    cudaMallocHost(&host_modified, devices * sizeof(int *));
    cudaMallocHost(&device_modified, devices * sizeof(int *));
    for (int i = 0; i < devices; i++)
    {
        cudaMallocHost(&host_modified[i], sizeof(int));
    }
    for (int i = 0; i < devices; i++)
    {
        cudaSetDevice(i);
        cudaMalloc(&device_modified[i], sizeof(int));
        initKernel0<<<1, 1,0,streams[i]>>>(device_modified[i], 0, 0);
    }
    
    cudaCheckError();

    struct edge **device_edges;
    cudaMallocHost(&device_edges, devices * sizeof(struct edge *));
    for (int i = 0; i < devices; i++)
    {
        cudaSetDevice(i);
        cudaMalloc(&device_edges[i], m * sizeof(struct edge));
        cudaMemcpyAsync(device_edges[i], adj, m * sizeof(struct edge), cudaMemcpyHostToDevice,streams[i]);
    }

    cudaCheckError();

    int **device_offset;
    cudaMallocHost(&device_offset, devices * sizeof(int *));
    for (int i = 0; i < devices; i++)
    {
        cudaSetDevice(i);
        cudaMalloc(&device_offset[i], (n+1) * sizeof(int));
        cudaMemcpyAsync(device_offset[i], offset, (n+1) * sizeof(int), cudaMemcpyHostToDevice,streams[i]);
    }

    for(int i=0;i<devices;i++){
        cudaSetDevice(i);
             cudaStreamSynchronize(streams[i]);
    }


    cudaSetDevice(0);
    cudaEvent_t start_event, stop_event; /// TIMER START
    cudaEventCreate(&start_event);
    cudaEventCreate(&stop_event);
    float milliseconds = 0;
    cudaEventRecord(start_event, 0);
    cudaCheckError();

    // for(int i=0;i<=devices;i++){
    //     cout<<i<<" "<<vertex_partition_start[i]<<endl;
    // }


    int iterations = 0;

    while (iterations < n)
    {

        for (int i = 0; i < devices; i++)
        {
            cudaSetDevice(i);
            SSSPKernel<<<num_blocks, block_size,0,streams[i]>>>(vertex_partition_start[i], vertex_partition_start[i + 1], device_dist[i], device_modified[i], device_edges[i], device_offset[i]);
            cudaMemcpyAsync(host_dist + vertex_partition_start[i], device_dist[i] + vertex_partition_start[i], (vertex_partition_start[i + 1] - vertex_partition_start[i])*sizeof(int), cudaMemcpyDeviceToHost,streams[i]);
            cudaMemcpyAsync(host_modified[i], device_modified[i], sizeof(int), cudaMemcpyDeviceToHost,streams[i]);
        }

        for(int i=0;i<devices;i++){
            cudaSetDevice(i);
             cudaStreamSynchronize(streams[i]);
        }

        // for(int i=0;i<n;i++){
        //     cout<<iterations<<" "<<i<<" "<<host_dist[i]<<endl;
        // }
        
        int modified = 0;
        for (int i = 0; i < devices; i++)
        {
            modified |= host_modified[i][0];
            host_modified[i][0] = 0;
        }

        if (modified == 0)
        {
            cout << "Completed" << endl;
            break;
        }

        for (int i = 0; i < devices; i++)
        {
            cudaSetDevice(i);
            cudaMemcpyAsync(device_dist[i], host_dist, n * sizeof(int), cudaMemcpyHostToDevice,streams[i]);
            cudaMemcpyAsync(device_modified[i], host_modified[i], sizeof(int), cudaMemcpyHostToDevice,streams[i]);
        }

        for(int i=0;i<devices;i++){
            cudaSetDevice(i);
            cudaStreamSynchronize(streams[i]);
        }

        iterations++;
        if (iterations > n + 1 || iterations > 10000)
        {
            cout << "Aborting" << endl;
            break;
        }

        //cudaCheckError();
    }


    cudaSetDevice(0);
    cudaEventRecord(stop_event, 0);
    cudaEventSynchronize(stop_event);
    cudaEventElapsedTime(&milliseconds, start_event, stop_event);
    printf("GPU Time: %.6f ms \nIterations:%d\n", milliseconds, iterations);

    cudaCheckError();

    int debug = 0;
    if (debug)
    {
        for (int i = 0; i < n; i++)
        {
            cout << i << " " << host_dist[i] << endl;
        }
    }
    cout<<"Distance from vertex 0 to vertex 6 is ";
    cout<<host_dist[6]<<endl;
}

int main(int argc, char **argv)
{
    cout << "Reading input" << endl;
    ifstream input(argv[1]);
    int n, m;
    input >> n >> m;
    m *= 2;
    struct edge *adj;
    adj = (struct edge *)malloc(m * sizeof(struct edge));
    for (int i = 0; i < m; i += 2)
    {
        int u, v, w;
        input >> u >> v >> w;
        adj[i].from = u;
        adj[i].to = v;
        adj[i].wt = w;
        adj[i + 1].to = u;
        adj[i + 1].from = v;
        adj[i + 1].wt = w;
    }
    cout << "Reading input successfull" << endl;

    sort(adj, adj + m, [&](struct edge a, struct edge b)
         {
        if(a.from != b.from){
            return a.from<b.from;
        }
        else return a.to<b.to; });

    int *offset = (int *)malloc((n + 1) * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        offset[i] = -1;
    }
    offset[n] = m;
    int curr = -1;
    for (int i = 0; i < m; i++)
    {
        if (adj[i].from > curr)
        {
            curr = adj[i].from;
            offset[curr] = i;
        }
    }
    for (int i = n - 1; i >= 0; i--)
    {
        if (offset[i] == -1)
        {
            offset[i] = offset[i + 1];
        }
    }

    // for (int i = 0; i < m; i++)
    // {
    //     cout << adj[i].from << " " << adj[i].to << " " << adj[i].wt << endl;
    // }

    // for (int i = 0; i <= n; i++)
    // {
    //     cout << offset[i] << endl;
    // }

    sssp(n, m, adj, offset);

    // cout << "*******************************" << endl;
    // cout << "Operation succuessful" << endl;

    // cout << "*******************************" << endl;
}

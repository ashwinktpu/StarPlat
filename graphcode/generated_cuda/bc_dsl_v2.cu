#include"bc_dsl_v2.h"

void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet)

{
  unsigned V = g.num_nodes();
  unsigned E = g.num_edges();

  int MAX_VAL = 2147483647 ;
  int * gpu_edgeList;
   int * gpu_edgeLen;
  int * gpu_dist;
   int * gpu_OA;
  bool * gpu_modified_prev;
  bool * gpu_finished;
  int *gpu_rev_OA;
  int *gpu_srcList;
  float  *gpu_node_pr;

  cudaMalloc(&gpu_OA, sizeof(int)*(1+V));
  cudaMalloc(&gpu_edgeList, sizeof(int)*(E));
  cudaMalloc(&gpu_edgeLen, sizeof(int)*(E));
  cudaMalloc(&gpu_dist, sizeof(int)*(V));
  cudaMalloc(&gpu_modified_prev, sizeof(bool)*(V));
  cudaMalloc(&gpu_modified_next, sizeof(bool)*(V));
  cudaMalloc(&gpu_finished, sizeof(bool)*(1));
  cudaMalloc(&gpu_srcList, sizeof(int)*(E));
  cudaMalloc(&gpu_node_pr, sizeof(flaot)*(V));

  unsigned int block_size;
  unsigned int num_blocks;
  if( V <= 1024)
  {
    block_size = V;
    num_blocks = 1;
  }
  else
  {
    block_size = 1024;
    num_blocks = ceil(((float)V) / block_size);
  }
  cudaMemcpy(&d_gpu_OA,OA, sizeof(int)*(1+V), cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeLen,cpu_edgeLen , sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_dist,modified , sizeof(bool)*V, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_finished,finished , sizeof(bool)*1, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_srcList,cpu_srcList, sizeof(int)*(E), cudaMemcpyHostToDevice);
  Compute_BC_kernel<<<num_blocks, block_size>>>(gpu_OA, gpu_edgeList, V, E ;
    cudaDeviceSynchronize();


    for (int t = 0; t < V; g ++) 
    {
      BC[t] = 0;
    }
    {
      int src = *itr;
      double* sigma=new double[g.num_nodes()];
      int* bfsDist=new int[g.num_nodes()];
      float* delta=new float[g.num_nodes()];
      for (int t = 0; t < V; g ++) 
      {
        delta[t] = 0;
        bfsDist[t] = -1;
      }
      for (int t = 0; t < V; g ++) 
      {
        sigma[t] = 0;
      }
      bfsDist[src] = 0;
      sigma[src] = 1;
      std::vector<std::vector<int>> levelNodes(g.num_nodes()) ;
      std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
      std::vector<int>  levelCount(g.num_nodes()) ;
      int phase = 0 ;
      levelNodes[phase].push_back(src) ;
      int bfsCount = 1 ;
      levelCount[phase] = bfsCount;
      while ( bfsCount > 0 )
      {
         int prev_count = bfsCount ;
        bfsCount = 0 ;
        #pragma omp parallel for
        for( int l = 0; l < prev_count ; l++)
        {
          int v = levelNodes[phase][l] ;
          for(int edge = g.indexofNodes[v] ; edge < g.indexofNodes[v+1] ; edge++) {
            int nbr = g.edgeList[edge] ;
            int dnbr ;
            if(bfsDist[nbr]<0)
            {
              dnbr = __sync_val_compare_and_swap(&bfsDist[nbr],-1,bfsDist[v]+1);
              if (dnbr < 0)
              {
                int num_thread = omp_get_thread_num();
                 levelNodes_later[num_thread].push_back(nbr) ;
              }
            }
          }
          if(bfsDist[w]==bfsDist[v]+1)
          {
          }
        }
      }
      phase = phase + 1 ;
      for(int i = 0;i < omp_get_max_threads();i++)
      {
         levelNodes[phase].insert(levelNodes[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
         bfsCount=bfsCount+levelNodes_later[i].size();
         levelNodes_later[i].clear();
      }
       levelCount[phase] = bfsCount ;
    }
    phase = phase -1 ;
    while (phase > 0)
    {
      #pragma omp parallel for
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int v = levelNodes[phase][l] ;
        if(bfsDist[w]==bfsDist[v]+1)
        {
        }
      }
      BC[v] = BC[v] + delta[v];
    }
    phase = phase - 1 ;
  }
}

}
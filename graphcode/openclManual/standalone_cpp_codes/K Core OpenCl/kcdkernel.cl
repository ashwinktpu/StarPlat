#include "libOpenCL.h"

__kernel void kcd_kernel(int V,int E,int k,__global int* gpu_index,__global int* gpu_edgeList,__global int* d_active,__global int* dc_degree,__global int* dn_degree,__global int* dflag)
{
    
    unsigned int u = get_global_id(0);
    if(u<V && d_active[u]==1 && dc_degree[u]<k){
        d_active[u]=0;
        for(int i=gpu_index[u];i<gpu_index[u+1];i++){
            int v=gpu_edgeList[i];
            atomic_add(&dn_degree[v],-1);
            if(dn_degree[v]<k && d_active[v]==1){
                dflag[0]=1;
            }
            
        }

    }
}
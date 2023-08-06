__kernel void select_minimum_weight(__global int *edge_v1,
                                    __global int *edge_v2,
                                    __global int *edge_weight,
                                    __global int *min_weight,
                                    __global int *parent,
                                    __global int *no_more_edges)
{
    int i=get_global_id(0);

    if(i>0)
    {
        int v1=edge_v1[i];
        int v2=edge_v2[i];

        int p1=parent[v1];
        int p2=parent[v2];

        if(p1!=p2)
        {
            int weight=edge_weight[i];
            atomic_min(&min_weight[p1],weight);

            atomic_min(&min_weight[p2],weight);

            *no_more_edges=0;
        }
    }
}

__kernel void select_edge_with_min_opp_vertex(__global int *edge_v1,
                                            __global int *edge_v2,
                                            __global int *edge_weight,
                                            __global int *min_weight,
                                            __global int *min_opp_vertex,
                                            __global int *parent
                                            
                                    )
{
    

    int i=get_global_id(0);

    if(i>0)
    {
        int v1=edge_v1[i];

        int v2=edge_v2[i];

        int p1=parent[v1];

        int p2=parent[v2];

        if(p1!=p2)
        {
            int weight=edge_weight[i];

            if(min_weight[p1]==weight)
            {
                atomic_min(&min_opp_vertex[p1],p2);
            }
            if(min_weight[p2]==weight)
            {
                atomic_min(&min_opp_vertex[p2],p1);
            }
            
        }
    }
}

__kernel void select_edge_id(__global int *edge_v1,
                            __global int *edge_v2,
                            __global int *edge_weight,
                            __global int *min_weight,
                            __global int *min_opp_vertex,
                            __global int *min_edge_id,
                            __global int *parent
                                    )
{
    

    int i=get_global_id(0);

    if(i>0)
    {
        int v1=edge_v1[i];

        int v2=edge_v2[i];

        int p1=parent[v1];

        int p2=parent[v2];

        if(p1!=p2)
        {
            int weight=edge_weight[i];

            if(min_weight[p1]==weight)
            {
                if(min_opp_vertex[p1]==p2)
                {
                    min_edge_id[p1]=i;
                }
            }
            if(min_weight[p2]==weight)
            {
            
                if(min_opp_vertex[p2]==p1)
                {
                    min_edge_id[p2]=i;
                }
                
            }
        }
    }
}

__kernel void remove_mirror_edges(__global int *min_edge_id,
                                  __global int *min_opp_vertex,
                                  __global int *parent)
{
    int i=get_global_id(0);

    if(i>0)
    {
        if(parent[i]==i)
        {
            int opp_vertex=min_opp_vertex[i];

            if(opp_vertex!=-1 && opp_vertex!=INT_MAX && min_opp_vertex[opp_vertex]==i && i<opp_vertex)
            {
                min_edge_id[i]=-1;
                min_opp_vertex[i]=-1;
            }
        }
    }
}

__kernel void select_mst_edges(__global int *min_edge_id,
                               __global int *parent,
                               __global int *mst_edges)
{
    int i=get_global_id(0);

    if(i>0)
    {
        if(parent[i]==i)
        {
            if(min_edge_id[i]!=-1 && min_edge_id[i]!=INT_MAX)
            {
                mst_edges[min_edge_id[i]]=1;
            }
        }
    }
}

__kernel void init_colors(__global int *min_edge_id,
                          __global int *min_opp_vertex,
                          __global int *parent)
{
    int i=get_global_id(0);

    if(i>0)
    {
        if(parent[i]==i)
        {
            if(min_opp_vertex[i]==-1)
            {
                parent[i]=i;
            }
            else if (min_opp_vertex[i]!=INT_MAX)
            {
                parent[i]=min_opp_vertex[i];
            }
        }
    }
}

__kernel void prop_colors(__global int *parent)
{
    int i=get_global_id(0);

    if(i>0)
    {
        while(parent[i]!=parent[parent[i]])
        {
            parent[i]=parent[parent[i]];
        }
    }
}

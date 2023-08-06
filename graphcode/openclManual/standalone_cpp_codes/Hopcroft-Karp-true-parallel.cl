struct node
{
    int v1,v2,edge_id,weight;
};

__kernel void initialize_arrays(__global int *visited,
                                __global int *lookahead,
                                __global int *parent_edge_id,
                                __global int *starting_index
                                )
{
    int i=get_global_id(0);

    visited[i]=0;
    parent_edge_id[i]=-1;
    lookahead[i]=starting_index[i];

}

__kernel void match_and_update( __global int *Q,             //0
                                __global struct node *edges, //1
                                __global int* outdegree,     //2
                                __global int* starting_index,  //3
                                __global int* ending_vertex_of_edge, //4
                                __global int* matched_edges,  //5
                                __global int* lookahead,      //6
                                __global int* visited,        //7
                                __global int* parent_edge_id,  //8
                                __global int* outdegree_copy,  //9
                                __global int* matched)         //10

{
    int id=get_global_id(0);
    int u=Q[id];
    int stack[]={-1};

    stack[0]=u;
    if(atomic_add(&visited[u],1)==0)
    {
        while(stack[0]!=-1)
        {
            int current_vertex=stack[0];
            stack[0]=-1;
            //printf("thread=%d,vertex=%d\n",id,current_vertex);
            
                int end_index_for_current_vertex=starting_index[current_vertex]
                                                +outdegree[current_vertex]-1;


                for(int i=lookahead[current_vertex];i!=-1 && i<=end_index_for_current_vertex;i++)
                {
                    
                    //lookahead[current_vertex]=(lookahead[current_vertex]+1)>end_index_for_current_vertex?-1:lookahead[current_vertex]+1;
                    int dest_vertex=ending_vertex_of_edge[i];

                    if(atomic_add(&visited[dest_vertex],1)==0)
                    {
                        int eid=i;
                        int actual_eid=edges[eid].edge_id;
                        //printf("%d %d\n",edges[eid].v1,edges[eid].v2);
                        matched_edges[actual_eid]=1;
                        parent_edge_id[dest_vertex]=i;
                        matched[current_vertex]=eid;
                        matched[dest_vertex]=eid;

                        int end_index_for_dest_vertex=starting_index[dest_vertex]+outdegree[dest_vertex]-1;

                        for(int j=lookahead[dest_vertex];j!=-1 && j<=end_index_for_dest_vertex;j++)
                        {
                            //lookahead[dest_vertex]=(lookahead[dest_vertex]+1)>end_index_for_current_vertex?-1:lookahead[dest_vertex]+1;
                            int next_vertex=ending_vertex_of_edge[j];

                            if(atomic_add(&outdegree_copy[next_vertex],-1)==2)
                            {
                                if(atomic_add(&visited[next_vertex],1)==0)
                                {
                                    stack[0]=next_vertex;
                                    parent_edge_id[next_vertex]=j;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            
            if(stack[0]==-1)
            {

                // int eid=parent_edge_id[current_vertex];
                // if(eid==-1)
                // stack[0]=-1;
                // else
                // {
                //     int parent_vertex=edges[eid].v1+edges[eid].v2-current_vertex;
                //     int grandparent_eid=parent_edge_id[parent_vertex];
                //     int grandparent_vertex=edges[grandparent_eid].v1+
                //                             edges[grandparent_eid].v2-
                //                             parent_vertex;
                    
                //     stack[0]=grandparent_vertex;
                    
                // }  
                return;
            }
        }
    }
    
    
}

int dfs_for_hopcroft_karp(int u,
                         __global struct node *edges,
                         __global int* outdegree,
                         __global int* starting_index,
                         __global int* ending_vertex_of_edge,
                         __global int* matched,
                         __global int* lookahead,
                         __global int* visited,
                         __global int* parent_edge_id)
{
    int path_end_vertex=-1;

    int stack[]={-1};
    stack[0]=u;

    while(stack[0]!=-1)
    {
        int current_vertex=stack[0];
        stack[0]=-1;
        atomic_xchg(&visited[current_vertex],1);

        int end_index_for_current_vertex=starting_index[current_vertex]
                                          +outdegree[current_vertex]-1;
        
        for(int i=lookahead[current_vertex];i!=-1 && i<=end_index_for_current_vertex;i++)
        {
            
           lookahead[current_vertex]=(lookahead[current_vertex]+1)>end_index_for_current_vertex?-1:lookahead[current_vertex]+1;
            int dest_vertex=ending_vertex_of_edge[i];

            if(atomic_add(&visited[dest_vertex],1)==0)
            {
                if(matched[dest_vertex]==-1)
                {
                    parent_edge_id[dest_vertex]=i;
                    path_end_vertex=dest_vertex;
                    return path_end_vertex;
                }
                else
                {
                    int matching_edge_id=matched[dest_vertex];
                    stack[0]=edges[matching_edge_id].v1+edges[matching_edge_id].v2-dest_vertex;
                     
                    parent_edge_id[dest_vertex]=i;
                    parent_edge_id[stack[0]]=matching_edge_id;
                    // if(i==matching_edge_id)
                    // {
                    //     std::cout<<"matching edge id:"<<i<<"\n";
                    //     std::cout<<"concerned vertexes:";
                    //     std::cout<<current_vertex<<" "<<dest_vertex<<" "<<stack[0]<<"\n";
                    // }
                     break;
                }
            }
        }

        if(stack[0]==-1)
        {
            int eid=parent_edge_id[current_vertex];
            if(eid==-1)
            stack[0]=-1;
            else
            {
                int parent_vertex=edges[eid].v1+edges[eid].v2-current_vertex;
                int grandparent_eid=parent_edge_id[parent_vertex];
                int grandparent_vertex=edges[grandparent_eid].v1+
                                        edges[grandparent_eid].v2-
                                        parent_vertex;
                
                stack[0]=grandparent_vertex;
                
            }         
        }
        

    }

    return path_end_vertex;
}

void recalculate_matching(__global struct node *edges,
                          __global int *parent_edge_id,
                          int path_end_vertex,
                          int u,
                          __global int *matched_edges,
                          __global int *matched)
{
    int current_vertex=path_end_vertex;
    int parent_vertex;
    int flag=0;

     while(parent_edge_id[current_vertex]!=-1)
    {
        int eid=parent_edge_id[current_vertex];
        int actual_eid=edges[eid].edge_id;
        parent_vertex=edges[eid].v1+edges[eid].v2-current_vertex;
        if(!flag)
        {
            matched_edges[actual_eid]=1;
            matched[current_vertex]=eid;
            matched[parent_vertex]=eid;  
        }
        else
        {
            matched_edges[actual_eid]=0;
        }
        current_vertex=parent_vertex;
        // std::cout<<"current_vertex:"<<current_vertex<<"\n";
        // std::cout<<eid<<"\n";
        flag^=1;
    }
}
__kernel void find_augmenting_paths(__global struct node* edges,
                                    __global int* outdegree,
                                    __global int* starting_index,
                                    __global int* ending_vertex_of_edge,
                                    __global int* vertices_X,
                                    __global int* matched_edges,
                                     __global int* parent_edge_id,
                                    __global int* lookahead,
                                    __global int* visited,
                                   
                                    
                                    __global int* path_found,
                                    __global int* matched)
{
   
    int i=get_global_id(0);
    int u=vertices_X[i];

    if(matched[u]==-1)
    {
        int path_end_vertex=dfs_for_hopcroft_karp(u,
                                                  edges,
                                                  outdegree,
                                                  starting_index,
                                                  ending_vertex_of_edge,
                                                  matched,
                                                  lookahead,
                                                  visited,
                                                  parent_edge_id);

        //printf("%d %d\n",u,path_end_vertex);
        if(path_end_vertex!=-1)
        {
            atomic_xchg(path_found,1);
            

            recalculate_matching(edges,
                                 parent_edge_id,
                                 path_end_vertex,
                                 u,
                                 matched_edges,
                                 matched
                                 );
        }
    }
    
}

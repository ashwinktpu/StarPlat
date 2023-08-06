#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable

bool isRangeSet(int tag){return (tag&16);}
void setTrim1(__global int tags[],int index) { atomic_or( &tags[index] ,32); };
bool isTrim1(int tags) { return ( tags & 32); }
void setTrim2(__global int tags[],int index) { atomic_or(&tags[index] ,64); };
bool isTrim2(int tags) { return ( tags & 64); };
bool isForwardVisited(int tags){ return (tags & 1);}
bool isForwardPropagate(int tags){ return (tags & 4);}
void setForwardPropagateBit(__global int tags[],int index)
{ atomic_or(&tags[index],4); };
void clearForwardPropagateBit(__global int tags[],int index){tags[index] = ( tags[index] ^ 4); };
void rangeSet(__global int tags[],int index) { tags[index] = ( tags[index] | 16); };
bool isBackwardVisited(int tags) { return (tags & 2); }
bool isBackwardPropagate(int tags) { return ( tags & 8); }
void setBackwardPropagateBit(__global int tags[],int index) { atomic_or(&tags[index] , 8); };
void clearBackwardPropagateBit(__global int tags[],int index){ tags[index] = ( tags[index] ^ 8);};
bool isForwardProcessed(int tags){return (tags&256);};
void setForwardProcessed(__global int tags[],int index){atomic_or(&tags[index],256);}
void clearForwardProcessed(__global int tags[],int index){ tags[index] = (tags[index] ^ 256);};
bool isBackwardProcessed(int tags){return (tags&512);};
void setBackwardProcessed(__global int tags[],int index){atomic_or(&tags[index],512);}
void clearBackwardProcessed(__global int tags[],int index){ tags[index] = (tags[index] ^ 512);};
bool isPropagate(int tags,const int direction)
{
    return (direction==0 ? isForwardPropagate(tags):isBackwardPropagate(tags));
}

bool isProcessed(int tags,const int direction)
{
    return (direction==0 ? isForwardProcessed(tags):isBackwardProcessed(tags));
}
void setProcessed(__global int tags[],int index,int direction)
{
    (direction==0 ? setForwardProcessed(tags,index):setBackwardProcessed(tags,index));
}

void setPropagate(__global int tags[],int index,int direction)
{
    direction==0 ? setForwardPropagateBit(tags,index) : setBackwardPropagateBit(tags,index);

}
void setPivot(__global int tags[],int index) { tags[index] = ( tags[index] | 128); };
bool isPivot(int tags) { return ( tags & 128); }

struct node
{
    int v1,v2,weight,id;
};

struct node2
{
    int v1,v2,id;
};
__kernel void trim1(__global int *tags,
                    __global int *Fc,
                    __global int *Fr,
                    __global int *Bc,
                    __global int *Br,
                    __global unsigned *colors,
                    __global bool *terminate,
                    __global int *trim1_cnt )
{

   
    int vertex=get_global_id(0);

    //  for(;vertex<=21297772;vertex+=1){
        
        if(vertex>0 )
        {
            if(isTrim1(tags[vertex]) || isTrim2(tags[vertex]))
            return;//continue;

        //  printf("Hello,vertex=%d\n",vertex);
            
            bool eliminate=true;
            int starting_index=Br[vertex];
            int ending_index=Br[vertex+1];

            
        //  printf("vertex=%d,starting_index=%d,end_index=%d\n",
            //                       vertex,starting_index,ending_index);
                

            for(int i=starting_index;i<ending_index;i++)
            {
                int neighbour_vertex=Bc[i];
                
                if(!isTrim1(tags[neighbour_vertex]) && (!isTrim2(tags[neighbour_vertex])) && colors[vertex]==colors[neighbour_vertex])
                {
                    eliminate=false;
                    break;
                }
            }
        
            if(!eliminate)
            {
                eliminate=true;
                starting_index=Fr[vertex];
                ending_index=Fr[vertex+1];

             
                for(int i=starting_index;i<ending_index;i++)
                {
                    int neighbour_vertex=Fc[i];
                   
                    // if(neighbour_vertex<0)
                    // {
                    //     printf("This is the culprit1,vertex=%d,i=%d\n",vertex,i);
                    // }
                    // else if(neighbour_vertex>3774768)
                    // {
                    //     printf("This is the culprit2,vertex=%d,i=%d\n",vertex,i);
                    // }
                    if(!isTrim1(tags[neighbour_vertex]) && (!isTrim2(tags[neighbour_vertex])) &&colors[vertex]==colors[neighbour_vertex])
                    {
                        eliminate=false;
                        break;
                    }
                }
            }
            
            if(eliminate)
            {
               // setTrim1(tags,vertex);
                setTrim1(tags,vertex);
                *terminate=false;
                atomic_add(trim1_cnt,1);
              //  printf("%d\n",vertex);
            // printf("tags:%d,vertex:%d\n",tags[vertex],vertex);
            }
            
        }
  
   //  }

}



__kernel void pollforpivots(__global unsigned *colors,
                            __global int *tags,
                            __global int *Fr,
                            __global int *Br,
                            __global unsigned int *max_criteria,
                            const int vertex_count)
{
    int vertex=get_global_id(0);
    if(vertex>0)
    {
        if(!isTrim1(tags[vertex]) && (!isTrim2(tags[vertex]))){
            unsigned c=colors[vertex];
            
            long int newdegree=((long int)(Fr[vertex+1]-Fr[vertex]))
                                *((long int)(Br[vertex+1]-Br[vertex]));
            atomic_max(&max_criteria[c%(vertex_count)],newdegree);
            
        }
    }
}




__kernel void selectpivots(__global unsigned *colors,
                            __global int *tags,
                            __global int *Fr,
                            __global int *Br,
                            __global unsigned int *max_criteria,
                            __global int *pivot_fields,
                            const int vertex_count
                            )

{
    int vertex=get_global_id(0);
    if(vertex>0)
    {
        if(!isTrim1(tags[vertex]) && (!isTrim2(tags[vertex]))){
            unsigned c=colors[vertex];
            long int newdegree=((long int)(Fr[vertex+1]-Fr[vertex]))
                                *((long int)(Br[vertex+1]-Br[vertex]));
            if(newdegree==max_criteria[c%(vertex_count)])
            {
                //pivot_fields[c%(*vertex_count)]=vertex;
               if(atomic_cmpxchg(&pivot_fields[c%(vertex_count)],0,vertex)==0)
               {
                      //setBackwardPropagateBit(tags,vertex);
                      //setForwardPropagateBit(tags,vertex);
                    

                      setPropagate(tags,vertex,0);
                   

                      setPropagate(tags,vertex,1);
                    

                      setPivot(tags,vertex);
                      
                     // atomic_add(pivot_count,1);
                      

               }
            }
        }
    }
}

//direction=0 --> fwd propagation
//direction=1 --> bwd propagation

__kernel void bfs(  __global int *Fr,
                    __global int *Fc,
                    __global int *tags,
                    __global unsigned *colors,
                    __global bool *terminatef,
                    __global int *visited,
                    const int direction
                          )
{
    int i=get_global_id(0);
    // __global static int visited_arr[1632804]={0};
    //for(int vertex=i;vertex<=21297772;vertex+=4096)
    //__global static int flag=0;

    
    int vertex=i;
      {
        if(vertex>0)
        {

            //printf("vertex=%d,color=%d\n",vertex,colors[vertex]);
            if(vertex>1632803)
            {
                printf("culprit 3 is here\n");
            }
            
            if(!isTrim1(tags[vertex]) && (!isTrim2(tags[vertex])))
            {
                if(isPropagate(tags[vertex],direction))
                {
                    if(!isProcessed(tags[vertex],direction))
                    {
                        // if(visited[vertex]>1)
                        // {
                        //     printf("culprit vertex=%d\n",vertex);
                        // }
                        
                          atomic_add(visited,1);
                        // printf("culprit vertex %d\n",vertex);
                        // setForwardProcessed(tags,vertex);
                       // atomic_or(&tags[vertex],256);
                        setProcessed(tags,vertex,direction);
                        int starting_index=Fr[vertex];
                        int ending_index=Fr[vertex+1];

                        for(int i=starting_index;i<ending_index;i++)
                        {
                            int neighbour_vertex=Fc[i];
                            
                            if(i>30622564)
                            {
                                printf("Culprit 1 here");
                            }
                            
                            if(neighbour_vertex>1632803)
                            {
                                printf("Culprit 2 here\n");
                            }
                            if(colors[vertex]==colors[neighbour_vertex] && !isTrim1(tags[neighbour_vertex]))
                            {

                                // if(visited[neighbour_vertex]==1)
                                //     printf("neighbour vertex=%d\n",neighbour_vertex);

                                //     visited[neighbour_vertex]=1;

                                    //setForwardPropagateBit(tags,neighbour_vertex);
                                    setPropagate(tags,neighbour_vertex,direction);
                                    *terminatef=false;
                                
                            }                 
                        }
                    }
                }
            }
        }
    }
}

__kernel void update(__global unsigned *colors,
                     __global int *tags,
                     __global bool *terminate
                     )
{
    int vertex=get_global_id(0);
    if(vertex>0)
    {
        
        if(!isTrim1(tags[vertex]) && (!isTrim2(tags[vertex])))
        {
            if(isPropagate(tags[vertex],0) && isPropagate(tags[vertex],1))
            {
                setTrim1(tags,vertex);
            }
            else
            {
                *terminate=false;
                if(isForwardPropagate(tags[vertex])&&(!isBackwardPropagate(tags[vertex])))
                {
                    colors[vertex]=3*colors[vertex];
                    clearForwardProcessed(tags,vertex);
                    clearForwardPropagateBit(tags,vertex);
                }
                else if(isBackwardPropagate(tags[vertex])&& (!isForwardPropagate(tags[vertex])))
                {
                    colors[vertex]=3*colors[vertex]+1;
                    clearBackwardProcessed(tags,vertex);
                    clearBackwardPropagateBit(tags,vertex);
                }
                else if((!isForwardPropagate(tags[vertex])&&(!isBackwardPropagate(tags[vertex]))))
                {
                    colors[vertex]=3*colors[vertex]+2;
                }
            }
         }
     }
}


__kernel void trim2(
                    __global int *tags,
                    __global int *Fc,
                    __global int *Fr,
                    __global int *Bc,
                    __global int *Br,
                    __global unsigned *colors
                  )
{
   
    int vertex=get_global_id(0);
    bool eliminate=false;
    if(vertex>0)
    {
        if(!isTrim1(vertex) && (!isTrim2(tags[vertex])))
        {
            int outdegree=0;
            int sole_neighbour=-1;

            int starting_index=Fr[vertex];
            int end_index=Fr[vertex+1];

            for(int i=starting_index;i<end_index;i++)
            {
                int neighbour=Fc[i];
                if(!isTrim1(tags[neighbour]) &&  (!isTrim2(tags[neighbour])) && colors[neighbour]==colors[vertex])
                {
                    outdegree++;
                    sole_neighbour=neighbour;
              
                }       

            }
            if(outdegree==1)
            {
                outdegree=0;
                starting_index=Fr[sole_neighbour];
                end_index=Fr[sole_neighbour+1];
                int back_neighbour=-1;

                for(int i=starting_index;i<end_index;i++)
                {
                    int neighbour=Fc[i];
                    if(!isTrim1(tags[neighbour]) && (!isTrim2(tags[neighbour])) &&colors[neighbour]==colors[vertex])
                    {
                        outdegree++;
                        back_neighbour=neighbour;
                
                    }  


                }
                if(outdegree==1 && back_neighbour==vertex)
                {
                    setTrim1(tags,vertex);
                    setTrim2(tags,vertex);
                    setTrim1(tags,sole_neighbour);
                    setTrim2(tags,sole_neighbour);
                    eliminate=true;

                }
            }

            if(!eliminate)
            {
                int indegree=0;
                starting_index=Br[vertex];
                end_index=Br[vertex+1];
                sole_neighbour=-1;

                for(int i=starting_index;i<end_index;i++)
                {
                    int neighbour=Bc[i];
                    if(!isTrim1(tags[neighbour]) && (!isTrim2(tags[neighbour])) &&colors[neighbour]==colors[vertex])
                    {
                        indegree++;
                        sole_neighbour=neighbour;
                    }

                }
                if(indegree==1)
                {
                    indegree=0;
                    starting_index=Br[sole_neighbour];
                    end_index=Br[sole_neighbour+1];
                    int back_neighbour=-1;
                    for(int i=starting_index;i<end_index;i++)
                    {
                        int neighbour=Bc[i];
                        if(!isTrim1(tags[neighbour]) && (!isTrim2(tags[neighbour])) &&colors[neighbour]==colors[vertex])
                        {
                            indegree++;
                            back_neighbour=neighbour;
                        }

                    }
                    if(indegree==1 && back_neighbour==vertex)
                    {
                       
                        setTrim2(tags,vertex);
                        
                        setTrim2(tags,sole_neighbour);
                        eliminate=true;
                    }

                }
                

            }

        }
    }
    
    
}

__kernel void assign_self_root(__global int *WCC,
                              __global int *tags)
{
    int vertex=get_global_id(0);
    if(vertex>0)
    {
        if(!isTrim1(tags[vertex]) && !isTrim2(tags[vertex]))
        {
            WCC[vertex]=vertex;
        }
    }
}

__kernel void assign_WCC_roots(__global int *WCC,
                              __global int *tags,
                              __global unsigned *colors,
                              __global int *Fr,
                              __global int *Fc,
                              __global bool *terminate)
{
    int vertex=get_global_id(0);
    if(vertex>0)
    {
        if(!isTrim1(tags[vertex]) && !isTrim2(tags[vertex]))
        {
            int starting_index=Fr[vertex];
            int ending_index=Fr[vertex+1];

            for(int i=starting_index;i<ending_index;i++)
            {
                int neighbour_vertex=Fc[i];
                if(!isTrim1(tags[neighbour_vertex]) && !isTrim2(tags[neighbour_vertex]))
                {
                    if(colors[neighbour_vertex]==colors[vertex])
                    {
                        if(WCC[neighbour_vertex]<WCC[vertex])
                        {
                            WCC[vertex]=WCC[neighbour_vertex];
                            *terminate=false;
                        }
                    }
                }
            } 
        }
    }
}


__kernel void shorten_paths(__global int  *WCC,
                            __global int  *tags,
                            __global bool *terminate
                            )
{

    int vertex=get_global_id(0);
    if(vertex>0)
    {
        if(!isTrim1(tags[vertex]) && !isTrim2(tags[vertex]))
        {
            int k=WCC[vertex];

            if(k!=vertex && k!=WCC[k])
            {
                WCC[vertex]=WCC[k];
                *terminate=false;
            }
             
        }
    }
}

__kernel void color_WCC(__global int *WCC,
                        __global int *tags,
                        __global unsigned *colors)
{
    int vertex=get_global_id(0);
    if(vertex>0)
    {
        
        if(!isTrim1(tags[vertex]) && !isTrim2(tags[vertex]))
        {
            // atomic_add(&in_vertex,1);
            // printf("in vertex=%d\n",in_vertex);
            int k=WCC[vertex];

            colors[vertex]=WCC[vertex];
            // atomic_add(&out_vertex,1);
            // printf("out vertex=%d\n",out_vertex);

        }
    }
}

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
                                __global int *actual_edge_id, //1
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
                        int actual_eid=actual_edge_id[eid];
                        //printf("%d %d\n",edges[eid].v1,edges[eid].v2);
                        matched_edges[actual_eid]=1;
                        parent_edge_id[dest_vertex]=i;
                        matched[current_vertex]=eid;
                        matched[dest_vertex]=eid;

                        int end_index_for_dest_vertex=starting_index[dest_vertex]+outdegree[dest_vertex]-1;

                        // for(int j=lookahead[dest_vertex];j!=-1 && j<=end_index_for_dest_vertex;j++)
                        // {
                        //     //lookahead[dest_vertex]=(lookahead[dest_vertex]+1)>end_index_for_current_vertex?-1:lookahead[dest_vertex]+1;
                        //     int next_vertex=ending_vertex_of_edge[j];

                        //     if(atomic_add(&outdegree_copy[next_vertex],-1)==2)
                        //     {
                        //         if(atomic_add(&visited[next_vertex],1)==0)
                        //         {
                        //             stack[0]=next_vertex;
                        //             parent_edge_id[next_vertex]=j;
                        //             break;
                        //         }
                        //     }
                        // }
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
                         __global struct node2 *edges,
                         __global int* outdegree,
                         __global int* starting_index,
                         __global int* ending_vertex_of_edge,
                         __global int* matched,
                         __global int* lookahead,
                         __global int* visited,
                         __global int* parent_edge_id,
                         __global int* actual_edge_id)
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
                    int actual_eid=actual_edge_id[matching_edge_id];
                    stack[0]=edges[actual_eid].v1+edges[actual_eid].v2-dest_vertex;
                     
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
                int actual_eid=actual_edge_id[eid];
                int parent_vertex=edges[actual_eid].v1+edges[actual_eid].v2-current_vertex;
                int grandparent_eid=parent_edge_id[parent_vertex];
                actual_eid=actual_edge_id[grandparent_eid];

                int grandparent_vertex=edges[actual_eid].v1+
                                        edges[actual_eid].v2-
                                        parent_vertex;
                
                stack[0]=grandparent_vertex;
                
            }         
        }
        

    }

    return path_end_vertex;
}

void recalculate_matching(__global struct node2 *edges,
                          __global int *parent_edge_id,
                          int path_end_vertex,
                          int u,
                          __global int *matched_edges,
                          __global int *matched,
                          __global int *actual_edge_id)
{
    int current_vertex=path_end_vertex;
    int parent_vertex;
    int flag=0;

     while(parent_edge_id[current_vertex]!=-1)
    {
        int eid=parent_edge_id[current_vertex];
        int actual_eid=actual_edge_id[eid];
        parent_vertex=edges[actual_eid].v1+edges[actual_eid].v2-current_vertex;
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


__kernel void find_augmenting_paths(__global struct node2* edges,
                                    __global int* outdegree,
                                    __global int* starting_index,
                                    __global int* ending_vertex_of_edge,
                                    __global int* vertices_X,
                                    __global int* matched_edges,
                                     __global int* parent_edge_id,
                                    __global int* lookahead,
                                    __global int* visited,
                                   
                                    
                                    __global int* path_found,
                                    __global int* matched,
                                    __global int* actual_edge_id)
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
                                                  parent_edge_id,
                                                  actual_edge_id);

        //printf("%d %d\n",u,path_end_vertex);
        if(path_end_vertex!=-1)
        {
            atomic_xchg(path_found,1);
            

            recalculate_matching(edges,
                                 parent_edge_id,
                                 path_end_vertex,
                                 u,
                                 matched_edges,
                                 matched,
                                 actual_edge_id
                                 );
        }
    }
    
}
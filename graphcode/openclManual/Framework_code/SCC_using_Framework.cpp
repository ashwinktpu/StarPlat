#include<opencl_framework.h>

void dummy_bfs(std::vector<int>&Fr,
                std::vector<int>&Fc,
                int starting_vertex,
                std::vector<int>&tags,
                int vertex_count)
{
    std::vector<int>visited(vertex_count+1,0);
    std::queue<int>q;
    q.push(starting_vertex);
    visited[starting_vertex]=1;
    int total_visited=0;
    while(!q.empty())
    {
        int current_vertex=q.front();
        total_visited++;
        int starting_index=Fr[current_vertex];
        int ending_index=Fr[current_vertex+1];

        for(int i=starting_index;i<ending_index;i++)
        {
            if(!visited[Fc[i]] && ((tags[Fc[i]]&32)==0))
            {
                    q.push(Fc[i]);
                    visited[Fc[i]]=1;
                    
            }
            
        }
        q.pop();
    }
    std::cout<<"total visited"<<total_visited<<"\n";
}




void read_graph_input(std::string filename,
                      
                      int &edge_count,
                      int &vertex_count,
                      std::vector<std::pair<int,int>>&edges
                     )
{
    std::ifstream infile(filename);

    int v1,v2,add=0;

    int min_vertex=-1,max_vertex=-1;

    //int opt_weight;
    while(infile>>v1>>v2)
    {
        if(v1!=v2)
        {
            // edge_map[{v1,v2}]=1;
            // edge_map[{v2,v1}]=1;
            edges.push_back({v1,v2});
          //  edges.push_back({v2,v1});
         //   edges.push_back({v2,v1});
            min_vertex=min_vertex==-1?std::min(v1,v2):std::min(min_vertex,std::min(v1,v2));
            max_vertex=max_vertex==-1?std::max(v1,v2):std::max(max_vertex,std::max(v1,v2));
        }
    }

    edge_count=edges.size();
   
    std::cout<<"reading complete\n";

    if(min_vertex==0)
    {
        add=1;
    }    
    
    vertex_count=max_vertex+add;
    std::cout<<"vertex count:"<<vertex_count<<",edge count:"<<edge_count<<"\n";
    

    for(auto &x:edges)
    {
      x.first=x.first+add;
      x.second=x.second+add;
    }
    



}

void calculate_outdegree(std::vector<std::pair<int,int>>&edges,
                        std::vector<int>&outdegree)
{
    for(auto u:edges)
    {
        outdegree[u.first]++;
        
    }
}




void generate_CSR_format(std::vector<std::pair<int,int>>&edges,
                         std::vector<int>&outdegree,
                        std::vector<int>&Fc,
                        std::vector<int>&Fr
                        
                        )
{
    int v_cnt=Fr.size();
    int e_cnt=edges.size();
    std::vector<int>tmp_indegree(v_cnt+1);
    std::vector<int>curr_ptr(v_cnt+1);
    Fr[1]=1;
    curr_ptr[1]=1;
    for(int i=2;i<v_cnt;i++)
    {
        Fr[i]=Fr[i-1]+outdegree[i-1];
        curr_ptr[i]=Fr[i];
    }

    for(auto u:edges)
    {
        Fc[curr_ptr[u.first]]=u.second;
        // if(Fc[curr_ptr[u.first]])
        // {
        //     printf("Out of bounds \n");
        // }
        curr_ptr[u.first]++;
    }
    // for(int i=1;i<e_cnt;i++)
    // {
        
    //     Fc[i]=edges[i].second;
    //     if(Fc[i]>11548845)
    //    {

    //     printf("First fix this\n");
    //     break;
    //    }
       
    // }



    
    
}



int main(){

  Graph g;

  std::string filename="soc-pokec-relationships.txt";//"orkut.txt";// "graph_input_test.txt";
    int CSize;
    int RSize;

    

    std::vector<std::pair<int,int>>edges;
    
    int edge_count=0,vertex_count=0;

    read_graph_input(filename,
                     edge_count,
                     vertex_count,
                     edges);

    std::cout<<"graph read complete\n";
    std::cout<<"vertex count:"<<vertex_count<<",edge count:"<<edge_count<<"\n";

    
    std::vector<int>outdegree(vertex_count+2,0);

    calculate_outdegree(edges,
                        outdegree);

    edges.insert(edges.begin(),{0,0});
    std::vector<int>Fr(vertex_count+2),Fc(edge_count+1),
    Bc(edge_count+1),Br(vertex_count+2);

    generate_CSR_format(edges,
                        outdegree,
                        Fc,
                        Fr
                        );
    
    std::cout<<"CSR for forward reachability done\n";

    for(int i=1;i<=edge_count;i++)
    {
        std::swap(edges[i].first,edges[i].second);
    }
    std::cout<<"edges reversed\n";
    outdegree.assign(outdegree.size(),0);

    calculate_outdegree(edges,
                        outdegree);

    // std::sort(edges.begin(),edges.end());

    // std::cout<<"sorting the vertexes again\n";

    generate_CSR_format(edges,
                        outdegree,
                        Bc,
                        Br);
    
    std::cout<<"CSR for backward reachability\n";

    
    std::vector<unsigned>colors(vertex_count+1,1);

    std::vector<int>tags(vertex_count+1,0);

    int trim_count=0;

    std::cout<<"Fc size:"<<Fc.size()<<"\n";
    std::cout<<"Fr size:"<<Fr.size()<<"\n";
    std::cout<<"Br size:"<<Br.size()<<"\n";
    std::cout<<"Bc size:"<<Bc.size()<<"\n";

    try{
    g.trim1(tags,
             Fc,
             Fr,
             Bc,
             Br,
             colors,
             trim_count,
             vertex_count);

    printf("%d\n",tags[5935]);

    g.select_pivots(tags,
                    colors,
                    Fr,
                    Br,
                    vertex_count);

    
    
    g.bfs(Fr,Fc,tags,colors,vertex_count,0);

    g.bfs(Br,Bc,tags,colors,vertex_count,1);

    bool terminate=true;
   
    g.update_colors(colors,tags,vertex_count,terminate);

    g.trim1(tags,
             Fc,
             Fr,
             Bc,
             Br,
             colors,
             trim_count,
             vertex_count);

    int trim2_count=0;

    g.trim2(tags,
             Fc,
             Fr,
             Bc,
             Br,
             colors,
             trim2_count,
             vertex_count);

     g.trim1(tags,
             Fc,
             Fr,
             Bc,
             Br,
             colors,
             trim_count,
             vertex_count);

    std::vector<int>WCC(vertex_count+1);
    g.WCC_find(WCC,
                tags,
                colors,
                Fr,
                Fc,
                vertex_count);

    
    FILE *fp=fopen("output1.txt","w");

    if(fp!=NULL){

        for(int i=1;i<=vertex_count;i++)
        {
            fprintf(fp,"%d,%d\n",tags[i],colors[i]);
        }
        fclose(fp);
        printf("file writing complete");

    }
    int iter=0;
    
    do
    {
        terminate=true;
          iter++;
        printf("current iteration index=%d\n",iter);

        g.select_pivots(tags,
                        colors,
                        Fr,
                        Br,
                        vertex_count);

        g.bfs(Fr,Fc,
                tags,
                colors,
                vertex_count,
                0);  

        
        std::cout<<"Forward Prop done\n";

        g.bfs(Br,Bc,tags,
                colors,
                vertex_count,
                1);
        
        std::cout<<"Backward Prop done\n";

        g.update_colors(colors,
                        tags,
                        vertex_count,
                        terminate);

        std::cout<<"colors updated\n";
    }while(!terminate);

    int pivot_cnt=0;
    for(int i=1;i<=vertex_count;i++)
    {
        if((tags[i]&128))
        {
            pivot_cnt++;
        }
    }

    std::cout<<" pivot cnt="<<pivot_cnt<<"\n";
    std::cout<<"SCC cnt="<<pivot_cnt+trim2_count/2+trim_count<<"\n";

    

    }
     catch(cl::Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
        exit(0);
        }

    
}
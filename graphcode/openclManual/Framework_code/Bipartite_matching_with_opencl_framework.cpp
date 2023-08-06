#include<opencl_framework.h>
struct node
{
    int v1,v2,edge_id;
};
bool sortbykey(const Graph::node2 &a,const Graph::node2 &b)
{
    if(a.v1!=b.v1)
    return a.v1<b.v1;
    if(a.v2!=b.v2)
    return a.v2<b.v2;
}

void read_graph(std::string filename,
                std::vector<Graph::node2>&edges,
                int &edge_count,
                int &vertex_count,
                std::vector<int>&colors,
                 std::map<std::pair<int,int>,int>&edge_map)
{
    std::ifstream infile(filename);

    std::vector<Graph::node2>temp_edges;
    int u,v;

    int cnt=0;
    int X_max_vertex=0,Y_max_vertex=0;
    while(infile>>u>>v)
    {
       
            cnt++;
            temp_edges.push_back({u,v,cnt});
        
       
    }
     infile.close();
    std::cout<<"reading done\n";
    
    edge_count=cnt;

    for(auto edge:temp_edges)
    {
        X_max_vertex=std::max(X_max_vertex,edge.v1);
        Y_max_vertex=std::max(Y_max_vertex,edge.v2);
    }
    std::vector<int>X_vertex_map(X_max_vertex+1,-1);
    std::vector<int>Y_vertex_map(Y_max_vertex+1,-1);

    int mapped_value_X=0,mapped_value_Y=0;

    
    for(auto edge:temp_edges)
    {
        
        int v1=edge.v1;
        int v2=edge.v2;
        if(X_vertex_map[v1]==-1)
        {
            mapped_value_X++;
            X_vertex_map[v1]=mapped_value_X;
        }
    
        if(Y_vertex_map[v2]==-1)
        {
            mapped_value_Y++;
            Y_vertex_map[v2]=mapped_value_Y;
        }
        

    }
    
    std::cout<<"set1 "<<mapped_value_X<<" set2 "<<mapped_value_Y<<"\n";
     cnt=0;
    for(auto edge:temp_edges)
    {
        cnt++;
        int v1=edge.v1;
        int v2=edge.v2;

        edges.push_back({X_vertex_map[v1],Y_vertex_map[v2]+mapped_value_X,cnt});
       // edges.push_back({Y_vertex_map[v2]+mapped_value_X,X_vertex_map[v1],cnt});
    }
    temp_edges.clear();
    

    
    colors.reserve(mapped_value_X+mapped_value_Y+1);
    vertex_count=mapped_value_X+mapped_value_Y;
    colors.assign(colors.size(),0);

    

   

    for(auto edge:edges)
    {

        colors[std::min(edge.v1,edge.v2)]=1;
        colors[std::max(edge.v1,edge.v2)]=0;
       
    }
    

    std::cout<<"vertex count:"<<vertex_count<<"\n";
    //std::sort(edges.begin(),edges.end(),sortbykey);
}

void calculate_outdegree(std::vector<int>&outdegree,
                         std::vector<Graph::node2>&edges
                         )
{
    for(int i=1;i<edges.size();i++)
    {
        outdegree[edges[i].v1]++;
        outdegree[edges[i].v2]++;   
    }
    
}

void generate_csr_format(std::vector<Graph::node2>edges,
                        std::vector<int>outdegree,
                        std::vector<int>&starting_index,
                        std::vector<int>&ending_vertex_of_edge,
                        std::vector<int>&actual_edge_id)
{
    starting_index[1]=1;
    std::vector<int>curr_ptr(outdegree.size(),0);
    curr_ptr[1] = 1;
    for(int i=2;i<outdegree.size();i++)
    {
        starting_index[i]=starting_index[i-1]+outdegree[i-1];
        curr_ptr[i]=starting_index[i];
    }

    // for(int i=1;i<edges.size();i++)
    // {
    //     std::cout<<edges[i].v1 <<" "<<edges[i].v2<<" "<<edges[i].edge_id<<"\n";
    // }
    for(int i=1;i<edges.size();i++)
    {
        //Graph::node2 u=edges[i];
        int v1=edges[i].v1;
        int v2=edges[i].v2;
        int id=edges[i].edge_id;
        ending_vertex_of_edge[curr_ptr[v1]]=v2;
        ending_vertex_of_edge[curr_ptr[v2]]=v1;
        actual_edge_id[curr_ptr[v1]]=id;
        actual_edge_id[curr_ptr[v2]]=id;
        curr_ptr[v1] ++;
        curr_ptr[v2] ++;
    }
    
    // for(int i=1;i<outdegree.size();i++)
    // std::cout<<starting_index[i]<<",";

    // std::cout<<"\n";

    // for(int i=1;i<ending_vertex_of_edge.size();i++)
    // std::cout<<ending_vertex_of_edge[i]<<",";

    // std::cout<<"\n";

    // for(int i=1;i<actual_edge_id.size();i++)
    // std::cout<<actual_edge_id[i]<<",";

    // std::cout<<"\n";
    

}
int main()
{
    
     std::string filename;
     std::cout<<"Please enter the filename:";
     std::cin>>filename;
    int vertex_count=0,edge_count=0;

    std::vector<Graph::node2>edges(1);
    std::map<std::pair<int,int>,int>edge_map;
    edges[0].v1=0,edges[0].v2=0;
    std::vector<int>colors(1);
    read_graph(filename,
                edges,
                edge_count,
                vertex_count,
                colors,
                edge_map);

    std::cout<<"graph read complete\n";

    // for(int i=1;i<=edges.size();i++)
    // {
    //     std::cout<<edges[i].v1<<" "<<edges[i].v2<<" "<<edges[i].edge_id<<"\n";
    // }
    
    std::vector<int>outdegree(vertex_count+1,0);

    

    std::vector<int>starting_index(vertex_count+1);

    std::vector<int>ending_vertex_of_edge(2*edge_count+1);

    std::vector<int>actual_edge_id(2*edge_count+1);

    

    std::vector<int>degree_one_vertex,multidegree_vertex;

    calculate_outdegree(outdegree,
                        edges);
    
    std::cout<<"outdegree calculated\n";
    
    generate_csr_format(edges,
                        outdegree,
                        starting_index,
                        ending_vertex_of_edge,
                        actual_edge_id);

    std::cout<<"csr format generated\n";
    Graph g;
    std::vector<int>matched_edges(edge_count+1,0);
    g.set_vertex_and_edge_count(vertex_count,edge_count);
    g.max_bipartite_matching(matched_edges,
                              edges,
                              outdegree,
                              starting_index,
                              ending_vertex_of_edge,
                              actual_edge_id,
                              colors);

    int matching_count=0;
    for(int i=1;i<=edge_count;i++)
        {
            if(matched_edges[i])
            {
                matching_count++;
                
            }
        }
     std::cout<<"matching count:"<<matching_count<<"\n";
    

}
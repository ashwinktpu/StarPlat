#include<opencl_framework.h>
struct node
{
   int v1,v2,weight,id;
};
struct edge_info
{
    int v1,v2,weight,id;
};
void take_graph_input_weighted(std::string filename,
                               std::vector<struct node>&edges,
                               int &vertex_count,
                               int &edge_count,
                               int &starting_vertex )
{
    std::ifstream infile(filename);
    int v1,v2,w;
    std::map<std::pair<int,int>,int>edge_inputs;
    std::set<int>vertex_set,unique_edge_weights;

    
    while(infile>>v1>>v2>>w)
    {
        if(v1!=v2)
        {
            // int first_vertex=std::min(v1,v2);
            // int second_vertex=std::max(v1,v2);



            // if(edge_inputs[{first_vertex,second_vertex}]==0)
            // {
            //     edge_inputs[{first_vertex,second_vertex}]=w;
            //     global_adj[first_vertex].push_back(second_vertex);
            //     global_adj[second_vertex].push_back(first_vertex);
            // }
            // else
            // {
            //     //std::cout<<first_vertex<<" "<<second_vertex<<" "<<edge_inputs[{first_vertex,second_vertex}]<<" "<<w<<"\n";
            //     edge_inputs[{first_vertex,second_vertex}]=std::min(edge_inputs[{first_vertex,second_vertex}],w);
            // }
            edges.push_back({v1,v2,w,0});
            edge_count++;
            vertex_set.insert(v1);
            vertex_set.insert(v2);
        }
       
        
    }
   
    // for(auto u:edge_inputs)
    // {
    //     edges.push_back({u.first.first,u.first.second,u.second,0});
    //     vertex_set.insert(u.first.first);
    //     vertex_set.insert(u.first.second);
        
        
    // }   
    
    auto itr=vertex_set.end();
    itr--;
    vertex_count=*itr;
    starting_vertex=*(vertex_set.begin());
    int add=0;
    if(starting_vertex==0)
    add=1;

    for(auto &x:edges)
    {
        x.v1=x.v1+add,x.v2=x.v2+add;
    }
    vertex_count+=add;
    std::cout<<"vertex count:"<<vertex_count<<"edge count:"<<edge_count<<"\n";
    // edges[0].v1=0,edges[1].v2=0,edges[2].weight=0;
    // std::sort(edges.begin(),edges.end(),&compare);

    //exit(0);

    infile.close();
}
void take_graph_input_unweighted(std::string filename,
                               std::vector<struct node>&edges,
                               int &vertex_count,
                               int &edge_count,
                               int &starting_vertex )
{
    std::ifstream infile(filename);
    int v1,v2,w;
    
    
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist_rand(1,100);
    

    
    std::map<std::pair<int,int>,int>edge_inputs;
    std::set<int>vertex_set,unique_edge_weights;

    
    while(infile>>v1>>v2)
    {
        if(v1!=v2)
        {
            // int first_vertex=std::min(v1,v2);
            // int second_vertex=std::max(v1,v2);



            // if(edge_inputs[{first_vertex,second_vertex}]==0)
            // {
            //     edge_inputs[{first_vertex,second_vertex}]=INT_MAX;
            //     // global_adj[first_vertex].push_back(second_vertex);
            //     // global_adj[second_vertex].push_back(first_vertex);
            // }
            int current_wt=dist_rand(rng);
            edges.push_back({v1,v2,current_wt,0});
            edge_count++;
            vertex_set.insert(v1);
            vertex_set.insert(v2);
          
        }
       
        
    }
    infile.close();
    // for(auto u:edge_inputs)
    // {
    //     int current_wt=dist_rand(rng);
    //     edges.push_back({u.first.first,u.first.second,current_wt,0});
    //     vertex_set.insert(u.first.first);
    //     vertex_set.insert(u.first.second);
        
    //     edge_count++;
    // }   
    
    auto itr=vertex_set.end();
    itr--;
    vertex_count=*itr;
    starting_vertex=*(vertex_set.begin());
    int add=0;
    if(starting_vertex==0)
    add=1;

    for(auto &x:edges)
    {
        x.v1=x.v1+add,x.v2=x.v2+add;
    }
    vertex_count+=add;
    std::cout<<"vertex count:"<<vertex_count<<"edge count:"<<edge_count<<"\n";
    // edges[0].v1=0,edges[1].v2=0,edges[2].weight=0;
    // std::sort(edges.begin(),edges.end(),&compare);

    //exit(0);

    

}

void calculate_degree(std::vector<struct node>&edges,
                      std::vector<int>&degree)
{
    for(int i=1;i<edges.size();i++)
    {
        degree[edges[i].v1]++;
        degree[edges[i].v2]++;
    }
    //std::cout<<"degree of 264346 "<<degree[264346]<<"\n";
}

void create_edge_arrays(std::vector<struct node>&edges,
                        std::vector<int>&edge_v1,
                        std::vector<int>&edge_v2,
                        std::vector<int>&edge_weights,
                        int edge_count)
{
     for(int i=1;i<=edge_count;i++)
     {
        int v1=edges[i].v1;
        int v2=edges[i].v2;
        int weight=edges[i].weight;

        edge_weights[i]=1,edge_v1[i]=v1,edge_v2[i]=v2;
     }
}

int main()
{
    std::string filename="USA-road-d-for-Boruvka.txt";

    std::cout<<"The filename is:"<<filename<<"\n";

    int vertex_count=0,edge_count=0,starting_vertex=-1,
    no_more_edges=0;
    std::vector<struct node>edges(1);

    char is_weighted;
    std::cout<<"Is the graph weighted? press Y for yes and N for no:";

    std::cin>>is_weighted;

    if(is_weighted=='Y' || is_weighted=='y')
    {
    
        take_graph_input_weighted(filename,
                                edges,
                                vertex_count,
                                edge_count,
                                starting_vertex
                                );
    }
    else
    {
        take_graph_input_unweighted(filename,
                                edges,
                                vertex_count,
                                edge_count,
                                starting_vertex
                                );
    }

    if(starting_vertex==-1)
    {
        std::cout<<"graph not read properly from file\n";
        exit(0);
    }
    else
    {

        std::cout<<"graph read complete\n";

       std::vector<int>edge_v1(edge_count+1);
       std::vector<int>edge_v2(edge_count+1);

       std::vector<int>edge_weight(edge_count+1);

       std::vector<int>parent(vertex_count+1);


        for(int i=1;i<=vertex_count;i++)
        {
            parent[i]=i;
        }

        create_edge_arrays(edges,
                            edge_v1,
                            edge_v2,
                            edge_weight,
                            edge_count);

        try{

            for(int i=1;i<=vertex_count;i++)
            {
                parent[i]=i;
            }
            Graph g;
            std::vector<int>mst_edges(edge_count+1,0);
            g.set_vertex_and_edge_count(vertex_count,edge_count);
            g.find_MST(edge_v1,
                       edge_v2,
                       edge_weight,
                       parent,
                       mst_edges);

             int mst_edge_count=0;
             for(int i=1;i<=edge_count;i++)
            {
                if(mst_edges[i])
                {
                    mst_edge_count++;
                }
            }
    
            std::cout<<"mst edge count:"<<mst_edge_count<<"\n";
        }
        catch(cl::Error error)
        {
            std::cout << error.what() << "(" << error.err() << ")" << std::endl;
            exit(0);
        }

        
    }
}
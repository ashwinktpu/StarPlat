#include<bits/stdc++.h>
#define INF INT_MAX
#define __CL_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220 
#define CL_DEVICE_TYPE_DEFAULT CL_DEVICE_TYPE_GPU 
#include<CL/cl2.hpp>
#include<stdint.h>

#define INF INT_MAX
std::map<int,std::vector<int>>global_adj;
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
 //   while(EOF!=fscanf(fp,"%d,%d",&v1,&v2))
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
   // fclose(fp);
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


void device_setup_OpenCL(std::string &kernel_filename,
                         std::vector<std::string>&funcname,
                         std::vector<cl::Platform>&platforms,
                         cl::Context &context,
                         std::vector<cl::Device>&devices,
                         cl::CommandQueue &queue,
                         cl::Program &program,
                         std::vector<cl::Kernel>&kernel_arr)
{
    //std::cout<<"Entering device setup\n";
    cl::Platform::get(&platforms);
    
  //  std::cout<<"Platforms queried\n";
    cl_context_properties cps[3]={ 
                CL_CONTEXT_PLATFORM, 
                (cl_context_properties)(platforms[0])(), 
                0 
            };
    
    std::cout<<"context properties obtained\n";
    context=cl::Context(CL_DEVICE_TYPE_GPU,cps);

  //  std::cout<<"Success2\n";
    
    devices=context.getInfo<CL_CONTEXT_DEVICES>();

    

  //  std::cout<<"context queried\n";
    queue=cl::CommandQueue(context,devices[0],CL_QUEUE_PROFILING_ENABLE);

    std::ifstream sourceFile(kernel_filename);
            std::string sourceCode(
                std::istreambuf_iterator<char>(sourceFile),
                (std::istreambuf_iterator<char>()));
            cl::Program::Sources sources(1,sourceCode.c_str());

               

   std::cout<<"Code read complete\n";

    // Make program of the source code in the context
    program = cl::Program(context, sources);
   
    // Build program for these specific devices
    program.build(devices,"-cl-fast-relaxed-math");
    
    std::cout<<"program built\n";
    // kernel_find_min_edge_weight=cl::Kernel(program,kernel_funcname1.c_str());

    // kernel_find_min_edge_index=cl::Kernel(program,kernel_funcname2.c_str());

    // kernel_combine_components=cl::Kernel(program,kernel_funcname3.c_str());

    // kernel_set_max_edge_weight=cl::Kernel(program,kernel_funcname4.c_str());

    for(int i=0;i<funcname.size();i++)
    {
        //std::cout<<"Hi\n";
        kernel_arr[i]=cl::Kernel(program,funcname[i].c_str());
    }


    //std::cout<<"kernel set\n";
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

        edge_weights[i]=weight,edge_v1[i]=v1,edge_v2[i]=v2;
     }
}

void print_execution_time(cl::Event start,
                           cl::Event stop )
{
    cl_ulong time_start, time_end;
    double total_time;
    start.getProfilingInfo(CL_PROFILING_COMMAND_START,
                        &time_start);
    
    stop.getProfilingInfo(CL_PROFILING_COMMAND_END,
                        &time_end);
    total_time = time_end - time_start;

    std::cout << "Execution time in milliseconds " 
    << total_time / (float)10e6<< "\n";
}

void runBoruvkaMSTparallel(cl::Context &context,
                           cl::CommandQueue &queue,
                           std::vector<cl::Kernel>&kernel_arr,
                           int vertex_count,
                           int edge_count,
                           std::vector<int>&edge_v1,
                            std::vector<int>&edge_v2,
                            std::vector<int>&edge_weight,
                            std::vector<int>&parent,
                            std::vector<int>&mst_edges
                           )
{
    cl::Buffer d_edge_v1,
               d_edge_v2,
               d_edge_weight,
               d_parent,
               d_min_weight,
               d_min_opp_vertex,
               d_min_edge_id,
               d_no_more_edges,
               d_mst_edges;

    d_edge_v1=cl::Buffer(context,CL_MEM_READ_ONLY,
                         sizeof(int)*(edge_count+1));

    d_edge_v2=cl::Buffer(context,CL_MEM_READ_ONLY,
                         sizeof(int)*(edge_count+1));

    d_edge_weight = cl::Buffer(context,CL_MEM_READ_ONLY,
                         sizeof(int)*(edge_count+1));

     d_mst_edges = cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(edge_count+1));

    d_parent= cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(vertex_count+1));


    d_min_weight=cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(vertex_count+1));

    d_min_opp_vertex = cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(vertex_count+1));

    d_min_edge_id = cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(vertex_count+1));


    d_no_more_edges = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(int));


    queue.enqueueWriteBuffer(d_edge_v1,CL_TRUE,0,sizeof(int)*(edge_count+1),
                               (void*)edge_v1.data());

    queue.enqueueWriteBuffer(d_edge_v2,CL_TRUE,0,sizeof(int)*(edge_count+1),
                                 (void*)edge_v2.data());

    
    queue.enqueueWriteBuffer(d_edge_weight,CL_TRUE,0,sizeof(int)*(edge_count+1),
                                 (void*)edge_weight.data());

    queue.enqueueWriteBuffer(d_parent,CL_TRUE,0,sizeof(int)*(vertex_count+1),
                                   (void*)parent.data());

    
    queue.enqueueFillBuffer(d_no_more_edges,1,0,sizeof(int));

    queue.enqueueFillBuffer(d_mst_edges,0,0,sizeof(int)*(edge_count+1));

    while(1)
    {
        queue.enqueueFillBuffer(d_min_opp_vertex,INT_MAX,0,sizeof(int)*(vertex_count+1));

        queue.enqueueFillBuffer(d_min_edge_id,INT_MAX,0,sizeof(int)*(vertex_count+1));

        queue.enqueueFillBuffer(d_min_weight,INT_MAX,0,sizeof(int)*(vertex_count+1));


        queue.enqueueFillBuffer(d_no_more_edges,1,0,sizeof(int));

        kernel_arr[0].setArg(0,d_edge_v1);
        kernel_arr[0].setArg(1,d_edge_v2);
        kernel_arr[0].setArg(2,d_edge_weight);
        kernel_arr[0].setArg(3,d_min_weight);
        kernel_arr[0].setArg(4,d_parent);
        kernel_arr[0].setArg(5,d_no_more_edges);

        cl::NDRange global = cl::NDRange(edge_count+1);
        cl::NDRange local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[0],cl::NullRange,global,local);

        
        int no_more_edges=1;

        queue.enqueueReadBuffer(d_no_more_edges,CL_TRUE,0,sizeof(int),
                                   (void*)&no_more_edges);

        if(no_more_edges)
        {
            break;
        }

        kernel_arr[1].setArg(0,d_edge_v1);
        kernel_arr[1].setArg(1,d_edge_v2);
        kernel_arr[1].setArg(2,d_edge_weight);
        kernel_arr[1].setArg(3,d_min_weight);
        kernel_arr[1].setArg(4,d_min_opp_vertex);
        kernel_arr[1].setArg(5,d_parent);

        global = cl::NDRange(edge_count+1);
        local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[1],cl::NullRange,global,local);

        kernel_arr[2].setArg(0,d_edge_v1);
        kernel_arr[2].setArg(1,d_edge_v2);
        kernel_arr[2].setArg(2,d_edge_weight);
        kernel_arr[2].setArg(3,d_min_weight);
        kernel_arr[2].setArg(4,d_min_opp_vertex);
        kernel_arr[2].setArg(5,d_min_edge_id);
        kernel_arr[2].setArg(6,d_parent);

        global = cl::NDRange(edge_count+1);
        local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[2],cl::NullRange,global,local);




        kernel_arr[3].setArg(0,d_min_edge_id);
        kernel_arr[3].setArg(1,d_min_opp_vertex);
        kernel_arr[3].setArg(2,d_parent);

        global = cl::NDRange(vertex_count+1);
        local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[3],cl::NullRange,global,local);

        kernel_arr[6].setArg(0,d_min_edge_id);
        kernel_arr[6].setArg(1,d_parent);
        kernel_arr[6].setArg(2,d_mst_edges);

        global = cl::NDRange(vertex_count+1);
        local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[6],cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_mst_edges,CL_TRUE,0,sizeof(int)*(edge_count+1),
                               (void*)mst_edges.data());


        int mst_edge_count=0;
             for(int i=1;i<=edge_count;i++)
            {
                if(mst_edges[i])
                {
                    mst_edge_count++;
                }
            }
        std::cout<<"mst edge count:"<<mst_edge_count<<"\n";
        kernel_arr[4].setArg(0,d_min_edge_id);
        kernel_arr[4].setArg(1,d_min_opp_vertex);
        kernel_arr[4].setArg(2,d_parent);

        global = cl::NDRange(vertex_count+1);
        local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[4],cl::NullRange,global,local);

        kernel_arr[5].setArg(0,d_parent);

        global = cl::NDRange(vertex_count+1);
        local = cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[5],cl::NullRange,global,local);








    }
    
    queue.enqueueReadBuffer(d_mst_edges,CL_TRUE,0,sizeof(int)*(edge_count+1),
                               (void*)mst_edges.data());
    

    
}

int check_CSR(int vertex_count,
              std::vector<int>&starting_index,
              std::vector<struct node>&ending_vertex)
{
       for(int i=1;i<=vertex_count;i++)
       {
         if(starting_index[i+1]-starting_index[i]==global_adj[i].size())
         {
            int limit=starting_index[i+1];
            int cnt=0;
            std::cout<<i<<"-->";
            for(int j=starting_index[i];j<limit;j++)
            {
                std::cout<<ending_vertex[j].v1<<",";
                // if(ending_vertex[j].v1==global_adj[i][cnt])
                // {
                //     std::cout<<ending_vertex[j].v1<<",";
                //     continue;
                // }
                // else
                // {
                //     std::cout<<i<<" nbr mismatched\n"<<ending_vertex[j].v1<<" "<<
                //                               global_adj[i][cnt]  ;
                //     return 0;
                // }
            }
            std::cout<<"\n";
            for(;cnt<global_adj[i].size();cnt++)
            {
                std::cout<<global_adj[i][cnt]<<",";
            }
            std::cout<<"\n";
         }
         else
         {
            std::cout<<i<<"degree mismatched\n"<<global_adj[i].size();
            return 0;
         }
       }
       return 1;
}


int main()
{
    std::string filename="wikipedia.txt";

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


        



        // int matched=check_CSR(vertex_count,
        //                      starting_index,
        //                      end_vertex);

        // if(matched==1)
        // {
        //     std::cout<<"All good\n";
        // }
        // else
        // {
        //     std::cout<<"Not all good \n";
        
        // }
        // exit(0);

        try
        {
            /* code */
             std::vector<cl::Platform> platforms;
            
            cl::Context context;
            std::vector<cl::Device> devices;
            cl::CommandQueue queue;
            cl::Program program;

            cl::Kernel kernel_select_minimum_weight,
                       kernel_select_edge_with_min_opp_vertex,
                       kernel_select_edge_id,
                       kernel_remove_mirror_edges,
                       kernel_init_colors,
                       kernel_prop_colors,
                       kernel_select_mst_edges;
            std::vector<std::string>func_name={"select_minimum_weight",  //0
                                                "select_edge_with_min_opp_vertex",//1
                                                "select_edge_id",//2
                                                "remove_mirror_edges",//3
                                                "init_colors",//4
                                                "prop_colors",//5
                                                "select_mst_edges" //6

                                                };            


            std::vector<cl::Kernel>kernel_arr={ kernel_select_minimum_weight,
                                                kernel_select_edge_with_min_opp_vertex,
                                                kernel_select_edge_id,
                                                kernel_remove_mirror_edges,
                                                kernel_init_colors,
                                                kernel_prop_colors,
                                                kernel_select_mst_edges
                                               };
            
             std::string kernel_filename="Boruvka-true-parallel-edge-version.cl";

            device_setup_OpenCL(kernel_filename,
                                func_name,
                                platforms,
                                context,
                                devices,
                                queue,
                                program,
                                kernel_arr);

            std::cout<<"device setup done\n";

            

            for(int i=1;i<=vertex_count;i++)
            {
                parent[i]=i;
            }
           cl::Event start,stop;
           // auto start =std::chrono::high_resolution_clock::now();

        //     std::cout<<"starting index of each vertex\n";
        //    for(int i=1;i<=vertex_count;i++)
        //    {
        //         std::cout<<i<<" "<<starting_index[i]<<"\n";
        //    }

        //    std::cout<<"neighbours\n";
        //    for(int i=1;i<=vertex_count;i++)
        //    {
        //         std::cout<<i<<"-->";
        //         for(int j=starting_index[i];j<starting_index[i]+degree[i];j++)
        //         {
        //             std::cout<<end_vertex[j].v1<<","<<end_vertex[j].weight<<","<<end_vertex[j].id<<" ";

        //         }
        //         std::cout<<"\n";
        //    }
            std::vector<int>mst_edges(edge_count+1,0);

           queue.enqueueMarkerWithWaitList(NULL,&start);
            runBoruvkaMSTparallel(context,
                                   queue,
                                   kernel_arr,
                                   vertex_count,
                                   edge_count,
                                   edge_v1,
                                   edge_v2,
                                   edge_weight,
                                   parent,
                                   mst_edges
                                   );
            queue.enqueueMarkerWithWaitList(NULL,&stop);
            stop.wait();
            queue.finish();
            print_execution_time(start,stop); 

            int mst_edge_count=0;
             for(int i=1;i<=edge_count;i++)
            {
                if(mst_edges[i])
                {
                    mst_edge_count++;
                }
            }

            long long total_weight=0;
            std::cout<<"mst edge count:"<<mst_edge_count<<"\n";

            std::vector<int>edge_marked(edge_count+1,0);

            // for(int i=1;i<=vertex_count;i++)
            // {
            //     for(int j=starting_index[i];j<starting_index[i]+degree[i];j++)
            //     {
                    
            //         if(!edge_marked[end_vertex[j].id] && mst_edges[end_vertex[j].id])
            //         {
            //             total_weight+=end_vertex[j].weight;
            //             edge_marked[end_vertex[j].id]=1;
            //         }
            //     }
            // }

            std::cout<<"Total weight of mst ="<<total_weight<<"\n";

          //  auto stop = std::chrono::high_resolution_clock::now();

            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            // std::cout << "Elapsed time : " << duration.count() << " milli seconds " << "\n";


    


            
        }
        catch(cl::Error error)
        {
            std::cout << error.what() << "(" << error.err() << ")" << std::endl;
            exit(0);
        }
        

    }
}

#include<bits/stdc++.h>
#define INF INT_MAX
#define __CL_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220 
#define CL_DEVICE_TYPE_DEFAULT CL_DEVICE_TYPE_GPU 
#include<CL/cl2.hpp>
#include<stdint.h>
#define INF INT_MAX

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

void device_setup_OpenCL(std::string kernel_filename,
                         std::vector<std::string>&funcname,
                         std::vector<cl::Platform>&platforms,
                         cl::Context &context,
                         std::vector<cl::Device>&devices,
                         cl::CommandQueue &queue,
                         cl::Program &program,
                         std::vector<cl::Kernel>&kernel_arr

)
{
    //std::cout<<"Entering device setup\n";
    cl::Platform::get(&platforms);
    
  //  std::cout<<"Platforms queried\n";
    cl_context_properties cps[3]={ 
                CL_CONTEXT_PLATFORM, 
                (cl_context_properties)(platforms[0])(), 
                0 
            };
    
  //  std::cout<<"context properties obtained\n";
    context=cl::Context(CL_DEVICE_TYPE_GPU,cps);

  //  std::cout<<"Success2\n";
    
    devices=context.getInfo<CL_CONTEXT_DEVICES>();

  //  std::cout<<"context queried\n";
    queue=cl::CommandQueue(context,devices[0],CL_QUEUE_PROFILING_ENABLE);

    std::ifstream sourceFile(kernel_filename);
            std::string sourceCode(
                std::istreambuf_iterator<char>(sourceFile),
                (std::istreambuf_iterator<char>()));
            cl::Program::Sources sources{sourceCode};

   std::cout<<"Code read complete\n";

    // Make program of the source code in the context
    program = cl::Program(context, sources);
   // std::cout<<"Hello\n";
    // Build program for these specific devices
    program.build(devices,"-cl-std=CL2.0");

    std::cout<<"program built\n";
    // kernel_find_min_edge_weight=cl::Kernel(program,kernel_funcname1.c_str());

    // kernel_find_min_edge_index=cl::Kernel(program,kernel_funcname2.c_str());

    // kernel_combine_components=cl::Kernel(program,kernel_funcname3.c_str());

    // kernel_set_max_edge_weight=cl::Kernel(program,kernel_funcname4.c_str());

    for(int i=0;i<kernel_arr.size();i++)
    {
        kernel_arr[i]=cl::Kernel(program,funcname[i].c_str());
    }


    //std::cout<<"kernel set\n";
}

void run_parallel_SCC(cl::Context context,
                      cl::CommandQueue &queue,
                      std::vector<cl::Kernel>&kernel_arr,
                      int &vertex_count,
                      int &edge_count,
                      std::vector<int>&Fc,
                      std::vector<int>&Fr,
                      std::vector<int>&Bc,
                      std::vector<int>&Br)
{

    cl::Buffer d_Fr,\
                d_Br,\
                d_Fc,\
                d_Bc,\
                d_pivots,\
                d_range,\
                d_tags,\
                d_terminatef,\
                d_terminateb,\
                d_pivot_criteria,\
                d_colors,\
                d_trim1_cnt,\
                d_vertex_count,\
                d_WCC,\
                d_global_color,\
                d_indegree,\
                d_outdegree;

    std::vector<int>tags(vertex_count+1);
    bool terminatef=false,terminateb=false;

     int FWD_iterations = 0;
    int BWD_iterations = 0;
    uint32_t iterations = 0;
    int Trimm_iterations = 0;

    int max_pivot_size=(vertex_count+1);
    long int pivot_criteria=-1;

    std::vector<int>indegree(vertex_count+1),outdegree(vertex_count+1);

    for(int i=1;i<=vertex_count;i++)
    {
        outdegree[i]=Fr[i+1]-Fr[i];
        indegree[i]=Br[i+1]-Br[i];
    }

    d_indegree = cl::Buffer(context,CL_MEM_READ_WRITE,
                              sizeof(int)*(vertex_count+1));

    d_outdegree = cl::Buffer(context,CL_MEM_READ_WRITE,
                              sizeof(int)*(vertex_count+1));

    d_Fc=cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fc.size()));
    
    d_Fr=cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fr.size()));

    d_Bc=cl::Buffer(context,CL_MEM_READ_WRITE,\
                    sizeof(int)*(Bc.size()));

    d_Br=cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Br.size()));

    d_range=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(vertex_count+1));
    
    d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(vertex_count+1));

    d_pivots=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*max_pivot_size);

    d_terminatef=cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(bool));

    d_terminateb=cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(bool));
    
    d_pivot_criteria=cl::Buffer(context,CL_MEM_READ_WRITE,
                                sizeof(long int));

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (vertex_count+1)*sizeof(unsigned));
    
    d_trim1_cnt=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int));
    
    d_vertex_count=cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int));

    d_WCC=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (vertex_count+1)*sizeof(int));
    
    d_global_color=cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int));
  

    queue.enqueueWriteBuffer(d_indegree,
    CL_TRUE,0,sizeof(int)*(vertex_count+1),
    (void*)indegree.data());

    queue.enqueueWriteBuffer(d_outdegree,
    CL_TRUE,0,sizeof(int)*(vertex_count+1),
    (void*)outdegree.data());

    
    queue.enqueueWriteBuffer(d_Fc,
    CL_TRUE,0,sizeof(int)*(Fc.size()),
    (void*)Fc.data());

    queue.enqueueWriteBuffer(d_Fr,
    CL_TRUE,0,sizeof(int)*(Fr.size()),
    (void*)Fr.data());

    queue.enqueueWriteBuffer(d_Bc,
    CL_TRUE,0,sizeof(int)*(Bc.size()),
    (void*)Bc.data());

    queue.enqueueWriteBuffer(d_Br,
    CL_TRUE,0,sizeof(int)*(Br.size()),
    (void*)Br.data());

    queue.enqueueWriteBuffer(d_pivot_criteria,
    CL_TRUE,0,sizeof(long int),(void*)&pivot_criteria);

    queue.enqueueWriteBuffer(d_vertex_count,
    CL_TRUE,0,sizeof(int),(void*)&vertex_count);

    std::cout<<"Buffers written\n";

    queue.enqueueFillBuffer(d_range,0,0,
    sizeof(int)*(vertex_count+1) );
    
    queue.enqueueFillBuffer(d_tags,0,0,
    sizeof(int)*(vertex_count+1) );

    queue.enqueueFillBuffer(d_colors,1,0,
    sizeof(unsigned)*(vertex_count+1));

    queue.enqueueFillBuffer(d_trim1_cnt,0,0,
    sizeof(int));

    std::cout<<"buffers filled\n";
    
     cl::NDRange global(vertex_count+1);
    cl::NDRange local(1);



    
    do
    {
       
        terminatef=true;
        queue.enqueueWriteBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);
        kernel_arr[0].setArg(0,d_range);
        kernel_arr[0].setArg(1,d_tags);
        kernel_arr[0].setArg(2,d_Fc);
        kernel_arr[0].setArg(3,d_Fr);
        kernel_arr[0].setArg(4,d_Bc);
        kernel_arr[0].setArg(5,d_Br);
        kernel_arr[0].setArg(6,d_colors);
        kernel_arr[0].setArg(7,d_terminatef);
        kernel_arr[0].setArg(8,d_trim1_cnt);

        // kernel_arr[0].setArg(9,d_indegree);
        // kernel_arr[0].setArg(10,d_outdegree);
        //global=cl::NDRange(vertex_count+1+(vertex_count+1)%2);
        global = cl::NDRange(vertex_count+1);
        local=cl::NDRange(1);
        queue.enqueueNDRangeKernel
        (kernel_arr[0],cl::NullRange,global,local);

       // std::cout<<"Hello\n";

        queue.enqueueReadBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        int temp_cnt;

        queue.enqueueReadBuffer(d_trim1_cnt,
        CL_TRUE,0,sizeof(int),(void*)&temp_cnt);

        std::cout<<"trim1 cnt:"<<temp_cnt<<"\n";

        
    } while (!terminatef);

    std::cout<<"loop completed\n";

    
    
    global =cl::NDRange(vertex_count+1);
    local=cl::NDRange(1);
    std::vector<int>temp_tags(vertex_count+1);
    std::vector<int>visited(vertex_count+1);

    queue.enqueueReadBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(temp_tags.size()),
    (void*)temp_tags.data());

    int cnt=0;
    
    #if 1
   
    //std::cout<<"1-SCC count:"<<cnt<<"\n";

    kernel_arr[1].setArg(0,d_tags);
    kernel_arr[1].setArg(1,d_pivot_criteria);
    kernel_arr[1].setArg(2,d_Fr);
    kernel_arr[1].setArg(3,d_Br);

    global=cl::NDRange(vertex_count+1);
    local=cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_arr[1],cl::NullRange,global,local);

    kernel_arr[2].setArg(0,d_tags);
    kernel_arr[2].setArg(1,d_pivots);
    kernel_arr[2].setArg(2,d_pivot_criteria);
    kernel_arr[2].setArg(3,d_Fr);
    kernel_arr[2].setArg(4,d_Br);

    global=cl::NDRange(vertex_count+1);
    local=cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_arr[2],cl::NullRange,global,local);

    std::vector<int>pivot_vertex(vertex_count);

    queue.enqueueReadBuffer(d_pivots,
    CL_TRUE,0,sizeof(int)*pivot_vertex.size(), 
    (void*)pivot_vertex.data()
    );

    

    // std::cout<<"pivot vertex:"<<pivot_vertex[0]<<"\n";

    // queue.enqueueReadBuffer(d_tags,
    // CL_TRUE,0,sizeof(int)*(temp_tags.size()),
    // (void*)temp_tags.data());

   // temp_tags[pivot_vertex[0]]|=4;
    
    // std::cout<<"pivot vertex="<<pivot_vertex[0]<<"\n";
    // std::cout<<"Fw criteria:"<<(Fr[pivot_vertex[0]+1]-Fr[pivot_vertex[0]])<<",Bw criteria:"
    //             <<(Br[pivot_vertex[0]+1]-Br[pivot_vertex[0]])<<"\n";

    // queue.enqueueWriteBuffer(d_tags,
    // CL_TRUE,0,sizeof(int)*(temp_tags.size()),
    // (void*)temp_tags.data());

    cl::Buffer d_temp_visited=cl::Buffer(context,CL_MEM_READ_WRITE,
                                       (sizeof(int)));
    queue.enqueueFillBuffer(d_temp_visited,0,0,(sizeof(int)));
    std::cout<<"Entering the loop\n";

    do
    {
        /* code */
        terminatef=true;
        queue.enqueueWriteBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);



        kernel_arr[3].setArg(0,d_Fr);
        kernel_arr[3].setArg(1,d_Fc);
        kernel_arr[3].setArg(2,d_tags);
        kernel_arr[3].setArg(3,d_colors);
        kernel_arr[3].setArg(4,d_terminatef);
        kernel_arr[3].setArg(5,d_temp_visited);
      
        

        global=cl::NDRange(vertex_count+1);
        local=cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_arr[3],cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        int v_cnt;
        queue.enqueueReadBuffer(d_temp_visited,
        CL_TRUE,0,sizeof(int),(void*)&v_cnt);

        std::cout<<"vertex count :"<<v_cnt<<"\n";

        

    } while (!terminatef);
    
    queue.enqueueReadBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(vertex_count+1),
    (void*)temp_tags.data());

    std::cout<<"Forward Propagation\n";

    for(int i=1;i<=vertex_count;i++)
    {
        if(temp_tags[i]&1)
        {
            visited[i]=1;
        }
    }

   // temp_tags[pivot_vertex[0]]|=8;

    queue.enqueueWriteBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(temp_tags.size()),
    (void*)temp_tags.data());

    queue.enqueueFillBuffer(d_temp_visited,0,0,sizeof(int));
    int temp_itr_count=0;
    
    do
    {
        /* code */
        terminateb=true;
        queue.enqueueWriteBuffer(d_terminateb,
        CL_TRUE,0,sizeof(bool),(void*)&terminateb);

        kernel_arr[4].setArg(0,d_Br);
        kernel_arr[4].setArg(1,d_Bc);
        kernel_arr[4].setArg(2,d_tags);
        kernel_arr[4].setArg(3,d_colors);
        kernel_arr[4].setArg(4,d_terminateb);
       
         global=cl::NDRange(vertex_count+1);
        local=cl::NDRange(1);

         queue.enqueueNDRangeKernel
        (kernel_arr[4],cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminateb,
        CL_TRUE,0,sizeof(bool),(void*)&terminateb);

        

        queue.enqueueFillBuffer(d_temp_visited,0,0,sizeof(int));


    } while (!terminateb);
    
     
    // queue.enqueueReadBuffer(d_tags,
    // CL_TRUE,0,sizeof(int)*(vertex_count+1),
    // (void*)temp_tags.data());

    std::cout<<"Backward Propagation\n";

    // for(int i=1;i<=vertex_count;i++)
    // {
    //     if(temp_tags[i]&2)
    //     {
    //         visited[i]=1;
    //     }
    // }

    global=cl::NDRange(vertex_count+1);
    local=cl::NDRange(1);

    
    kernel_arr[5].setArg(0,d_colors);
    kernel_arr[5].setArg(1,d_tags);
    kernel_arr[5].setArg(2,d_terminatef);

    queue.enqueueNDRangeKernel
        (kernel_arr[5],cl::NullRange,global,local);

    std::cout<<"update done\n";

    // std::vector<int>colors(vertex_count+1);

    // queue.enqueueReadBuffer(d_colors,
    // CL_TRUE,0,sizeof(int)*(vertex_count+1),
    // (void*)colors.data());

    //std::cout<<"colors"<<"\n";

    // // for(int i=1;i<=vertex_count;i++)
    // // {
    // //     std::cout<<"vertex:"<<i<<",color:"<<colors[i]<<"\n";
    // // }
    
     do
    {
        
        terminatef=true;
        queue.enqueueWriteBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);
        kernel_arr[0].setArg(0,d_range);
        kernel_arr[0].setArg(1,d_tags);
        kernel_arr[0].setArg(2,d_Fc);
        kernel_arr[0].setArg(3,d_Fr);
        kernel_arr[0].setArg(4,d_Bc);
        kernel_arr[0].setArg(5,d_Br);
        kernel_arr[0].setArg(6,d_colors);
        kernel_arr[0].setArg(7,d_terminatef);
    
       
        global=cl::NDRange(vertex_count+1);
        local=cl::NDRange(1);
        queue.enqueueNDRangeKernel
        (kernel_arr[0],cl::NullRange,global,local);

        ///std::cout<<"Hello\n";

        queue.enqueueReadBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        int temp_cnt;

        queue.enqueueReadBuffer(d_trim1_cnt,
        CL_TRUE,0,sizeof(int),(void*)&temp_cnt);

        std::cout<<"trim1 cnt:"<<temp_cnt<<"\n";

    } while (!terminatef);

    global=cl::NDRange(vertex_count+1);
    local=cl::NDRange(1);
     
    queue.enqueueReadBuffer(d_trim1_cnt,
    CL_TRUE,0,sizeof(int),
    (void*)&cnt);
    
     std::cout<<"1-SCC count:"<<cnt<<"\n";
    
    terminatef=true;
    queue.enqueueWriteBuffer(d_terminatef,
    CL_TRUE,0,sizeof(bool),(void*)&terminatef);
    kernel_arr[6].setArg(0,d_range);
    kernel_arr[6].setArg(1,d_tags);
    kernel_arr[6].setArg(2,d_Fc);
    kernel_arr[6].setArg(3,d_Fr);
    kernel_arr[6].setArg(4,d_Bc);
    kernel_arr[6].setArg(5,d_Br);
    kernel_arr[6].setArg(6,d_colors);
    kernel_arr[6].setArg(7,d_terminatef);


    queue.enqueueNDRangeKernel
    (kernel_arr[6],cl::NullRange,global,local);

        //  queue.enqueueReadBuffer(d_terminatef,
        // CL_TRUE,0,sizeof(bool),(void*)&terminatef);
   
    
    // global=cl::NDRange(1);
    // local=cl::NDRange(1);


    queue.enqueueReadBuffer(d_tags,
    CL_TRUE,0,temp_tags.size()*(sizeof(int)),
    (void*)temp_tags.data());

    int trim2_cnt=0;
    for(int i=1;i<=vertex_count;i++)
    {
        if((temp_tags[i]&64))
        {
            trim2_cnt++;
            visited[i]=1;
        }
    }
    std::cout<<"trim2 count"<<trim2_cnt<<"\n";

   

    kernel_arr[7].setArg(0,d_WCC);
    kernel_arr[7].setArg(1,d_tags);

    queue.enqueueNDRangeKernel
    (kernel_arr[7],cl::NullRange,global,local);

    //queue.finish();

    std::cout<<"self roots assigned\n";

    
    do
    {
        queue.enqueueFillBuffer(d_terminatef,true,0,sizeof(bool));
        kernel_arr[8].setArg(0,d_WCC);
        kernel_arr[8].setArg(1,d_tags);
        kernel_arr[8].setArg(2,d_colors);
        kernel_arr[8].setArg(3,d_Fr);
        kernel_arr[8].setArg(4,d_Fc);
        kernel_arr[8].setArg(5,d_terminatef);

        queue.enqueueNDRangeKernel
        (kernel_arr[8],cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        queue.enqueueFillBuffer(d_terminateb,true,0,sizeof(bool));

        kernel_arr[9].setArg(0,d_colors);
        kernel_arr[9].setArg(1,d_tags);
        kernel_arr[9].setArg(2,d_terminateb);

        queue.enqueueNDRangeKernel
        (kernel_arr[9],cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminateb,
        CL_TRUE,0,sizeof(bool),(void*)&terminateb);




    } while (!terminatef || !terminateb);
    

    std::cout<<"WCC decomposition done\n";

    

   queue.enqueueFillBuffer(d_colors,0,0,sizeof(unsigned)*(vertex_count+1));

    kernel_arr[13].setArg(0,d_WCC);
    kernel_arr[13].setArg(1,d_tags);
    kernel_arr[13].setArg(2,d_colors);

    queue.enqueueNDRangeKernel
    (kernel_arr[13],cl::NullRange,global,local);
    // // for(int i=1;i<=vertex_count;i++)
    // // {
    // //     std::cout<<"vertex:"<<i<<",tag:"<<temp_tags[i]<<"\n";
    // // }
    queue.finish();

    std::cout<<"kernel finished\n";
    queue.enqueueWriteBuffer(d_terminatef,
            CL_TRUE,0,sizeof(bool),(void*)&terminatef);
    std::cout<<"WCC colors assigned\n";


    cl::Buffer d_max_criteria,d_count_rem_vertex,d_pivot_count;

    d_max_criteria=cl::Buffer(context,CL_MEM_READ_WRITE,
                              sizeof(unsigned int)*(vertex_count+1) );

    d_count_rem_vertex=cl::Buffer(context,CL_MEM_READ_WRITE,
                              sizeof(int) );

    d_pivot_count=cl::Buffer(context,CL_MEM_READ_WRITE,
                              sizeof(int) );


    queue.enqueueFillBuffer(d_pivot_count,0,0,sizeof(int));
   
    int iter=0;
    int count_rem_vertex=0;

   
    do
    {
        iter++;
        printf("current iteration index=%d\n",iter);
        queue.enqueueFillBuffer(d_max_criteria,0,0,sizeof(unsigned int)*(vertex_count+1));

        kernel_arr[10].setArg(0,d_colors);
        kernel_arr[10].setArg(1,d_tags);
        kernel_arr[10].setArg(2,d_Fr);
        kernel_arr[10].setArg(3,d_Br);
        kernel_arr[10].setArg(4,d_max_criteria);
        kernel_arr[10].setArg(5,d_vertex_count);

        queue.enqueueNDRangeKernel
        (kernel_arr[10],cl::NullRange,global,local);

        queue.enqueueFillBuffer(d_pivots,0,0,sizeof(int)*(vertex_count+1));

        kernel_arr[11].setArg(0, d_colors);
        kernel_arr[11].setArg(1,d_tags);
        kernel_arr[11].setArg(2,d_Fr);
        kernel_arr[11].setArg(3,d_Br);
        kernel_arr[11].setArg(4,d_max_criteria);
        kernel_arr[11].setArg(5,d_pivots);
        kernel_arr[11].setArg(6,d_vertex_count);
        

        queue.enqueueNDRangeKernel
        (kernel_arr[11],cl::NullRange,global,local);

        std::cout<<"pivot selected\n";

        do
        {
            /* code */

            terminatef=true;
            queue.enqueueWriteBuffer(d_terminatef,
            CL_TRUE,0,sizeof(bool),(void*)&terminatef);

            std::cout<<"starting fwd prop\n";

            kernel_arr[3].setArg(0,d_Fr);
            kernel_arr[3].setArg(1,d_Fc);
            kernel_arr[3].setArg(2,d_tags);
            kernel_arr[3].setArg(3,d_colors);
            kernel_arr[3].setArg(4,d_terminatef);
            kernel_arr[3].setArg(5,d_temp_visited);

             global=cl::NDRange(vertex_count+1);
            local=cl::NDRange(1);

            queue.enqueueNDRangeKernel
            (kernel_arr[3],cl::NullRange,global,local);

            queue.enqueueReadBuffer(d_terminatef,
            CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        } while (!terminatef);

        std::cout<<"Forward Prop done\n";
       
        do
        {
            /* code */
            terminateb=true;
            queue.enqueueWriteBuffer(d_terminateb,
            CL_TRUE,0,sizeof(bool),(void*)&terminateb);

            kernel_arr[4].setArg(0,d_Br);
            kernel_arr[4].setArg(1,d_Bc);
            kernel_arr[4].setArg(2,d_tags);
            kernel_arr[4].setArg(3,d_colors);
            kernel_arr[4].setArg(4,d_terminateb);
            
            global=cl::NDRange(vertex_count+1);
            local=cl::NDRange(1);

            queue.enqueueNDRangeKernel
            (kernel_arr[4],cl::NullRange,global,local);

            queue.enqueueReadBuffer(d_terminateb,
            CL_TRUE,0,sizeof(bool),(void*)&terminateb);


        } while (!terminateb);
        
        std::cout<<"Backward Prop done\n";

        global=cl::NDRange(vertex_count+1);
        local=cl::NDRange(1);

        terminatef=true;
        queue.enqueueWriteBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);



        kernel_arr[5].setArg(0,d_colors);
        kernel_arr[5].setArg(1,d_tags);
        kernel_arr[5].setArg(2,d_terminatef);

        queue.enqueueNDRangeKernel
            (kernel_arr[5],cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminatef,
            CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        std::cout<<"colors updated\n";

          queue.enqueueFillBuffer(d_count_rem_vertex,0,0,sizeof(int));

        kernel_arr[12].setArg(0,d_tags);
        kernel_arr[12].setArg(1,d_count_rem_vertex);

        queue.enqueueNDRangeKernel
        (kernel_arr[12],cl::NullRange,global,local);

        

        queue.enqueueReadBuffer(d_count_rem_vertex,
        CL_TRUE,0,sizeof(int),(void*)&count_rem_vertex);

        printf("SCC count=%d\n",count_rem_vertex+cnt+trim2_cnt/2);

        
    }while(!terminatef);
   
    #endif
   
    queue.enqueueReadBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(vertex_count+1),
    (void*)temp_tags.data());
    int pivot_cnt=0;

    for(int i=1;i<=vertex_count;i++)
    {
        if((temp_tags[i]&128))
        {
            pivot_cnt++;
        }
    }

    std::cout<<"1-SCC count:"<<cnt<<" pivot cnt="<<pivot_cnt<<"\n";
    
   





    









    

    
    



}   

void single_threaded_trim(std::vector<int>&Fc,
                          std::vector<int>&Fr,
                          std::vector<int>&Bc,
                          std::vector<int>&Br,
                          std::vector<int>&tags,
                          bool &terminate,
                          int vertex,
                          int &count)
{
     bool eliminate=true;
     if(!(tags[vertex]&32))
     {
     int starting_index=Fr[vertex];
     int ending_index=Fr[vertex+1];

     


     for(int i=starting_index;i<ending_index;i++)
     {
        int neighbour=Fc[i];
        if(!(tags[neighbour]&32))
        {
            eliminate=false;
            break;
        }
     }
     if(!eliminate)
     {
        eliminate=true;
        starting_index=Br[vertex];
        ending_index=Br[vertex+1];

        for(int i=starting_index;i<ending_index;i++)
        {
            int neighbour=Bc[i];
            if(!(tags[neighbour]&32))
            {
                eliminate=false;
                break;
            }
        }
     }

     if(eliminate)
     {
        tags[vertex]|=32;
        terminate=false;
        count++;
     }
     }
} 

void print_execution_time(cl::Event start,
                           cl::Event stop )
{
    cl_ulong time_start, time_end;
    double total_time;
    start.getProfilingInfo(CL_PROFILING_COMMAND_END,
                        &time_start);
    
    stop.getProfilingInfo(CL_PROFILING_COMMAND_START,
                        &time_end);
    total_time = time_end - time_start;

    std::cout << "Execution time in milliseconds " 
    << total_time / (float)10e6<< "\n";
}




int main()
{
    std::string filename="GermanyRoadud.txt";//"orkut.txt";// "graph_input_test.txt";
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

    
    
    

    
    // for(int i=1;i<=vertex_count+1;i++)
    // {
    //     std::cout<<Fr[i]<<" ";
    // }

    // std::cout<<"\n";
    // for(int i=1;i<=edge_count;i++)
    // {
    //     std::cout<<Fc[i]<<" ";
    // }

    // std::cout<<"\n";
    // for(int i=1;i<=vertex_count+1;i++)
    // {
    //     std::cout<<Br[i]<<" ";
    // }

    // std::cout<<"\n";
    // for(int i=1;i<=edge_count;i++)
    // {
    //     std::cout<<Bc[i]<<" ";
    // }

    std::cout<<"\n";
    std::vector<cl::Platform>platforms;

    cl::Context context;
    std::vector<cl::Device> devices;
    cl::CommandQueue queue;
    cl::Program program;

    cl::Kernel kernel_trim1,kernel_pollforfirstpivot,
    kernel_selectfirstpivot,kernel_fwd_kernel,kernel_bwd_kernel,
    kernel_update,kernel_trim2,kernel_assign_self_root,kernel_assign_WCC_roots,
    kernel_shorten_paths,kernel_pollforpivots,kernel_selectpivots,
    kernel_remaining_vertices,kernel_color_WCC;

    std::vector<cl::Kernel>kernel_arr={
                                        kernel_trim1,
                                        kernel_pollforfirstpivot,
                                        kernel_selectfirstpivot,
                                        kernel_fwd_kernel,
                                        kernel_bwd_kernel,
                                        kernel_update,
                                        kernel_trim2,
                                        kernel_assign_self_root,
                                        kernel_assign_WCC_roots,
                                        kernel_shorten_paths,
                                        kernel_pollforpivots,
                                        kernel_selectpivots,
                                        kernel_remaining_vertices,
                                        kernel_color_WCC

                                      };

    std::string kernel_filename="SCC_true_parallel.cl";
    
    std::vector<std::string>funcname={
                                       "trim1",      //0
                                       "pollforfirstpivot",  //1
                                       "selectfirstpivot",  //2
                                       "fwd_kernel",    //3
                                       "bwd_kernel",    //4
                                       "update",        //5
                                       "trim2",         //6
                                       "assign_self_root",  //7
                                       "assign_WCC_roots",  //8
                                       "shorten_paths",     //9
                                       "pollforpivots",     //10
                                       "selectpivots",      //11
                                       "remaining_vertices", //12
                                       "color_WCC"     //13
                                      };
    
    device_setup_OpenCL(kernel_filename,
                        funcname,
                        platforms,
                        context,
                        devices,
                        queue,
                        program,
                        kernel_arr);

     std::cout<<"Device setup complete\n";

     

    
     try
     {
        cl::Event start,stop;

        queue.enqueueMarkerWithWaitList(NULL,&start);
        run_parallel_SCC(context,
                    queue,
                    kernel_arr,
                    vertex_count,
                    edge_count,
                    Fc,
                    Fr,
                    Bc,
                    Br
                    );
        queue.enqueueMarkerWithWaitList(NULL,&stop);
         stop.wait();

        print_execution_time(start,stop); 

     }
     catch(cl::Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
        exit(0);
        }
/*
    std::cout<<"Beginning single threaded trim\n";
    std::vector<int>temp_tags(vertex_count+1,0);
    std::vector<int>visited(vertex_count+1,0);
    bool terminate=true,actual_terminate=true;
    int trim_cnt=0;
    do
    {
        actual_terminate=true;
        for(int i=1;i<=vertex_count;i++)
        {
            terminate=true;
            single_threaded_trim(Fc,
                                Fr,
                                Bc,
                                Br,
                                temp_tags,
                                terminate,
                                i,
                                trim_cnt);
            if((temp_tags[i]&32)&&(!visited[i]))
            {
                
                visited[i]=1;
            }
            actual_terminate&=terminate;
        }
        std::cout<<"trim count:"<<trim_cnt<<"\n";
        
    } while (!actual_terminate);*/

    // int trim_cnt=0;

    // for(int i=1;i<=vertex_count;i++)
    // {
        
    // }

    
    
    



    
    
   


     





    

    


}

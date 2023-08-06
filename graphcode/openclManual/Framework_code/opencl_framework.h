#include<bits/stdc++.h>
#define INF INT_MAX
#define __CL_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220 
#define CL_DEVICE_TYPE_DEFAULT CL_DEVICE_TYPE_GPU 
#include<CL/cl2.hpp>
#include<stdint.h>
class Graph
{
    
    
    std::string kernel_filename;
    std::vector<cl::Platform>platforms;
    cl::Context context;
    std::vector<cl::Device>devices;
    cl::CommandQueue queue;
    cl::Program program;
    int vertex_count,edge_count;
    
    
    void initialize_kernel(std::string &funcname,
                               cl::Kernel &kernel );

    bool is_bipartite(int x,
                  
                  std::vector<int>outdegree,
                  std::vector<int>starting_index,
                  std::vector<int>ending_vertex_of_edge,
                  std::vector<int>&colors);
    public :
    struct node
    {
    int v1,v2,weight,id;
    };

    struct node2
    {
        int v1,v2,edge_id;
    };

     Graph();

    void set_vertex_and_edge_count(int vertex_count,int edge_count)
    {
        this->vertex_count=vertex_count;
        this->edge_count= edge_count;
    }
    
    void trim1( std::vector<int>&tags,
                std::vector<int>&Fc,
                std::vector<int>&Fr,
                std::vector<int>&Bc,
                std::vector<int>&Br,
                std::vector<unsigned>&colors,
                int &trim_count,
                int vertex_count);

    
    void select_pivots(std::vector<int>&tags,
                        std::vector<unsigned>&colors,
                        std::vector<int>&Fr,
                        std::vector<int>&Br,
                        int vertex_count);

    

    void trim2(std::vector<int>&tags,
                std::vector<int>&Fc,
                std::vector<int>&Fr,
                std::vector<int>&Bc,
                std::vector<int>&Br,
                std::vector<unsigned>&colors,
                int &trim_count,
                int vertex_count);

    void bfs(std::vector<int>&Fr,
             std::vector<int>&Fc,
             std::vector<int>&tags,
             std::vector<unsigned>&colors,
             int vertex_count,
             int direction);

    void WCC_find(std::vector<int>&WCC,
                  std::vector<int>&tags,
                  std::vector<unsigned>&colors,
                  std::vector<int>&Fr,
                  std::vector<int>&Fc,
                  int vertex_count);

    void update_colors(std::vector<unsigned>&colors,
                     std::vector<int>&tags,
                     int vertex_count,
                     bool &terminate);

    void find_MST(std::vector<int>&edge_v1,
                    std::vector<int>&edge_v2,
                    std::vector<int>&edge_weight,
                    std::vector<int>&parent,
                    std::vector<int>&mst_edges
                                         
                                         );

    void find_min_edge_per_vertex(cl::Buffer &d_edge_v1,
                                   cl::Buffer &d_edge_v2,
                                   cl::Buffer &d_edge_weight,
                                   cl::Buffer &d_parent,
                                   cl::Buffer &d_min_edge_id,
                                   cl::Buffer &d_min_opp_vertex,
                                   cl::Buffer &d_no_more_edges);

    void remove_mirror_edges(cl::Buffer &d_min_edge_id,
                             cl::Buffer &d_min_opp_vertex,
                             cl::Buffer &d_parent
                             );

    
    void select_mst_edges(cl::Buffer &d_min_edge_id,
                           cl::Buffer &d_parent,
                           cl::Buffer &d_mst_edges);

    void init_colors_and_propagate(cl::Buffer &d_min_edge_id,
                     cl::Buffer &d_min_opp_vertex,
                     cl::Buffer &d_parent);

    void max_bipartite_matching(std::vector<int>&matched_edges,
                                 std::vector<struct node2>&edges,
                                 std::vector<int>&outdegree,
                                 std::vector<int>&starting_index,
                                 std::vector<int>&ending_vertex_of_edge,
                                 std::vector<int>&actual_edge_id,
                                 std::vector<int>&colors
                                 
                                 );

    void initialize_matching(std::vector<int>&multidegree_vertex_count,
                             
                             cl::Buffer &d_edges,
                             cl::Buffer &d_outdegree,
                             cl::Buffer &d_starting_index,
                             cl::Buffer &d_ending_vertex_of_edge,
                             
                             cl::Buffer &d_matched_edges,
                             cl::Buffer &d_parent_edge_id,
                             cl::Buffer &d_lookahead,
                             cl::Buffer &d_visited,
                             cl::Buffer &d_matched,
                             cl::Buffer &d_outdegree_copy,
                             cl::Buffer &d_actual_edge_id);

    void run_parallel_pothen_fan(cl::Buffer &d_edges,
                                 cl::Buffer &d_outdegree,
                                 cl::Buffer &d_starting_index,
                                 cl::Buffer &d_ending_vertex_of_edge,
                                 cl::Buffer &d_vertices_X,
                                 cl::Buffer &d_matched_edges,
                                 cl::Buffer &d_parent_edge_id,
                                 cl::Buffer &d_lookahead,
                                 cl::Buffer &d_visited,
                                 cl::Buffer &d_matched,
                                 cl::Buffer &d_outdegree_copy,
                                 cl::Buffer &d_actual_edge_id,
                                 int vertex_count_X
                                 );


    
    
    
    
    
};

Graph::Graph()
{

    kernel_filename = "cl_functions.cl";
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

    // for(int i=0;i<kernel_arr.size();i++)
    // {
    //     kernel_arr[i]=cl::Kernel(program,funcname[i].c_str());
    // }

};

void Graph::initialize_kernel(std::string &funcname,
                               cl::Kernel &kernel )
{
   
    kernel = cl::Kernel(program,funcname.c_str());
    
}
void Graph::trim1(std::vector<int>&tags,
                std::vector<int>&Fc,
                std::vector<int>&Fr,
                std::vector<int>&Bc,
                std::vector<int>&Br,
                std::vector<unsigned>&colors,
                int &trim_count,
                int vertex_count)
{
    cl::Kernel kernel_trim1;
    std::string funcname = "trim1";

    initialize_kernel(funcname,kernel_trim1);

    cl::Buffer d_tags,
               d_Fc,
               d_Fr,
               d_Bc,
               d_Br,
               d_colors,
               d_terminate,
               d_trim_count;

    d_Fc = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fc.size()));
    
    d_Fr = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fr.size()));

    d_Bc = cl::Buffer(context,CL_MEM_READ_WRITE,\
                    sizeof(int)*(Bc.size()));

    d_Br=cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Br.size()));

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (colors.size())*sizeof(unsigned));

    d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(tags.size()));

    d_terminate=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(bool));

    d_trim_count = cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int));
  


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

     queue.enqueueWriteBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());


    queue.enqueueWriteBuffer(d_colors,
    CL_TRUE,0,sizeof(int)*(colors.size()),
    (void*)colors.data());

    queue.enqueueWriteBuffer(d_trim_count,
    CL_TRUE,0,sizeof(int),(void*)&trim_count);
    bool terminate=true;

    std::cout<<"Buffers complete\n";
    do
    {
        terminate=true;
        queue.enqueueFillBuffer(d_terminate,true,0,sizeof(bool));
        
        kernel_trim1.setArg(0,d_tags);
        kernel_trim1.setArg(1,d_Fc);
        kernel_trim1.setArg(2,d_Fr);
        kernel_trim1.setArg(3,d_Bc);
        kernel_trim1.setArg(4,d_Br);
        kernel_trim1.setArg(5,d_colors);
        kernel_trim1.setArg(6,d_terminate);
        kernel_trim1.setArg(7,d_trim_count);

        // kernel_trim1.setArg(9,d_indegree);
        // kernel_trim1.setArg(10,d_outdegree);
        //global=cl::NDRange(vertex_count+1+(vertex_count+1)%2);
        cl::NDRange global(vertex_count+1);
        cl::NDRange local(1);
        queue.enqueueNDRangeKernel
        (kernel_trim1,cl::NullRange,global,local);

       // std::cout<<"Hello\n";

        queue.enqueueReadBuffer(d_terminate,
        CL_TRUE,0,sizeof(bool),(void*)&terminate);

        // int temp_cnt;

        // queue.enqueueReadBuffer(d_trim1_cnt,
        // CL_TRUE,0,sizeof(int),(void*)&temp_cnt);

        //std::cout<<"trim1 cnt:"<<temp_cnt<<"\n";

        
    } while (!terminate);

    std::cout<<"trim1 complete\n";
    
    queue.enqueueReadBuffer(d_trim_count,
    CL_TRUE,0,sizeof(int),(void*)&trim_count);

    queue.enqueueReadBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),(void*)tags.data());

             
}


void Graph::trim2(std::vector<int>&tags,
                std::vector<int>&Fc,
                std::vector<int>&Fr,
                std::vector<int>&Bc,
                std::vector<int>&Br,
                std::vector<unsigned>&colors,
                int &trim_count,
                int vertex_count)
{
     cl::Kernel kernel_trim2;
    std::string funcname = "trim2";

    initialize_kernel(funcname,kernel_trim2);
    cl::Buffer d_tags,
               d_Fc,
               d_Fr,
               d_Bc,
               d_Br,
               d_colors,
               d_trim_count;

    d_Fc = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fc.size()));
    
    d_Fr = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fr.size()));

    d_Bc = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Bc.size()));

    d_Br=cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Br.size()));

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (colors.size())*sizeof(unsigned));

    d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(tags.size()));

    d_trim_count = cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int));


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

     queue.enqueueWriteBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());

    queue.enqueueWriteBuffer(d_colors,
    CL_TRUE,0,sizeof(int)*(colors.size()),
    (void*)colors.data());

    queue.enqueueWriteBuffer(d_trim_count,
    CL_TRUE,0,sizeof(int),(void*)&trim_count);



    kernel_trim2.setArg(0,d_tags);
    kernel_trim2.setArg(1,d_Fc);
    kernel_trim2.setArg(2,d_Fr);
    kernel_trim2.setArg(3,d_Bc);
    kernel_trim2.setArg(4,d_Br);
    kernel_trim2.setArg(5,d_colors);

    cl::NDRange global(vertex_count+1);
    cl::NDRange local(1);
    
    queue.enqueueNDRangeKernel
    (kernel_trim2,cl::NullRange,global,local);

    queue.enqueueReadBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),(void*)tags.data());

    for(int i=1;i<=vertex_count;i++)
    {
        if((tags[i]&64))
        {
            trim_count++;
        }
    }

    std::cout<<"trim2 count="<<trim_count<<"\n";
    

    
}



void Graph::select_pivots(std::vector<int>&tags,
                        std::vector<unsigned>&colors,
                        std::vector<int>&Fr,
                        std::vector<int>&Br,
                        int vertex_count)
{
    cl::Kernel kernel_poll_for_pivots,kernel_select_pivots;
    std::string funcname = "pollforpivots";

    initialize_kernel(funcname,kernel_poll_for_pivots);

    funcname = "selectpivots";

    initialize_kernel(funcname,kernel_select_pivots);

    cl::Buffer d_tags,
               d_colors,/*fill this with 1 when searching for first pivot*/
               d_Fr,
               d_Br,
               d_pivots,
               d_heuristic_arr;


    d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(vertex_count+1));

    d_Fr = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fr.size()));

    d_Br=cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Br.size()));

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (vertex_count+1)*sizeof(unsigned));

    d_heuristic_arr = cl::Buffer(context,CL_MEM_READ_WRITE,
                        (vertex_count+1)*sizeof(unsigned));

    d_pivots = cl::Buffer(context,CL_MEM_READ_WRITE,
                        (vertex_count+1)*sizeof(int));



    queue.enqueueFillBuffer(d_heuristic_arr,0,0,sizeof(unsigned int)*(vertex_count+1));

    queue.enqueueFillBuffer(d_pivots,0,0,sizeof(int)*(vertex_count+1));



    queue.enqueueWriteBuffer(d_tags,CL_TRUE,0,(vertex_count+1)*sizeof(unsigned),(void*)tags.data());

    queue.enqueueWriteBuffer(d_Fr,CL_TRUE,0,sizeof(int)*(Fr.size()),
    (void*)Fr.data());

    queue.enqueueWriteBuffer(d_Br,CL_TRUE,0,
    sizeof(int)*(Br.size()),(void*)Br.data());

    queue.enqueueWriteBuffer(d_colors,CL_TRUE,0,sizeof(unsigned)*(vertex_count+1),
    (void*)colors.data());
    
    kernel_poll_for_pivots.setArg(0,d_colors);
    kernel_poll_for_pivots.setArg(1,d_tags);
    kernel_poll_for_pivots.setArg(2,d_Fr);
    kernel_poll_for_pivots.setArg(3,d_Br);
    kernel_poll_for_pivots.setArg(4,d_heuristic_arr);
    kernel_poll_for_pivots.setArg(5,vertex_count);

    cl::NDRange global(vertex_count+1);
    cl::NDRange local (1);

    queue.enqueueNDRangeKernel
    (kernel_poll_for_pivots,cl::NullRange,global,local);


    kernel_select_pivots.setArg(0,d_colors);
    kernel_select_pivots.setArg(1,d_tags);
    kernel_select_pivots.setArg(2,d_Fr);
    kernel_select_pivots.setArg(3,d_Br);
    kernel_select_pivots.setArg(4,d_heuristic_arr);
    kernel_select_pivots.setArg(5,d_pivots);
    kernel_select_pivots.setArg(6,vertex_count);

    

    queue.enqueueNDRangeKernel
    (kernel_select_pivots,cl::NullRange,global,local);

    int current_pivot;

    unsigned int max_criteria;

    queue.enqueueReadBuffer(d_pivots,CL_TRUE,sizeof(int),sizeof(int),
                               (void*)&current_pivot);

    
    queue.enqueueReadBuffer(d_heuristic_arr,CL_TRUE,sizeof(unsigned),sizeof(unsigned),
                                  (void*)&max_criteria);

    
    queue.enqueueReadBuffer(d_tags,CL_TRUE,0,
                             sizeof(int)*(tags.size()),(void*)tags.data());

    std::cout<<"pivot="<<current_pivot<<" heuristic="<<max_criteria<<"\n";




}

void Graph::bfs(std::vector<int>&Fr,
                std::vector<int>&Fc,
                std::vector<int>&tags,
                std::vector<unsigned>&colors,
                int vertex_count,
                int direction)
{
    cl::Kernel kernel_bfs;
    std::string funcname = "bfs";

    initialize_kernel(funcname,kernel_bfs);
    cl::Buffer d_Fr,
               d_Fc,
               d_tags,
               d_colors,
               d_terminate,
               d_visited;

   
    d_Fc = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fc.size()));
    
    d_Fr = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fr.size()));

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (colors.size())*sizeof(unsigned));

    d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(tags.size()));

    d_terminate = cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(bool));

    d_visited= cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int));
    queue.enqueueWriteBuffer(d_Fc,
    CL_TRUE,0,sizeof(int)*(Fc.size()),
    (void*)Fc.data());

    queue.enqueueWriteBuffer(d_Fr,
    CL_TRUE,0,sizeof(int)*(Fr.size()),
    (void*)Fr.data());

    queue.enqueueWriteBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());

    queue.enqueueWriteBuffer(d_colors,
    CL_TRUE,0,sizeof(int)*(colors.size()),
    (void*)colors.data());

     queue.enqueueFillBuffer(d_visited,0,0,sizeof(int));

    bool terminate=true;

    do
    {
        /* code */
        terminate=true;
        queue.enqueueFillBuffer(d_terminate,true,0,sizeof(bool));

        kernel_bfs.setArg(0,d_Fr);
        kernel_bfs.setArg(1,d_Fc);
        kernel_bfs.setArg(2,d_tags);
        kernel_bfs.setArg(3,d_colors);
        kernel_bfs.setArg(4,d_terminate);
        kernel_bfs.setArg(5,d_visited);
        kernel_bfs.setArg(6,direction);

        cl::NDRange global(vertex_count+1);
        cl::NDRange local(1);

        queue.enqueueNDRangeKernel
        (kernel_bfs,cl::NullRange,global,local);

        
        queue.enqueueReadBuffer(d_terminate,CL_TRUE,0,sizeof(bool),
                                 (void*)&terminate);

        

    } while (!terminate);
    
    

    int visited_count=0;

    queue.enqueueReadBuffer(d_visited,CL_TRUE,0,
                            sizeof(int),(void*)&visited_count);

    queue.enqueueReadBuffer(d_tags,CL_TRUE,0,
                             sizeof(int)*(tags.size()),(void*)tags.data());

    //printf("the total vertices visited is %d\n",visited_count);



}

void Graph::update_colors(std::vector<unsigned>&colors,
                     std::vector<int>&tags,
                     int vertex_count,
                     bool &terminate)
{
    cl::Kernel kernel_update;
    std::string funcname = "update";

    initialize_kernel(funcname,kernel_update);

    cl::Buffer d_colors,
               d_tags,
               d_terminate;

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                    (colors.size())*sizeof(unsigned));

    d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(tags.size()));

    d_terminate=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(bool));

    queue.enqueueWriteBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());

    queue.enqueueWriteBuffer(d_colors,
    CL_TRUE,0,sizeof(unsigned)*(colors.size()),
    (void*)colors.data());

    queue.enqueueFillBuffer(d_terminate,true,0,sizeof(bool));

    kernel_update.setArg(0,d_colors);
    kernel_update.setArg(1,d_tags);
    kernel_update.setArg(2,d_terminate);

    cl::NDRange global(vertex_count+1);
    cl::NDRange local(1);

    printf("update complete\n");    
    queue.enqueueNDRangeKernel
    (kernel_update,cl::NullRange,global,local);

     printf("update complete\n");   
    queue.enqueueReadBuffer(d_tags,CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());

    queue.enqueueReadBuffer(d_colors,CL_TRUE,0,sizeof(unsigned)*(colors.size()),
    (void*)colors.data());

    queue.enqueueReadBuffer(d_terminate,CL_TRUE,0,sizeof(bool),
    (void*)&terminate);


   

}


void Graph::WCC_find(std::vector<int>&WCC,
                  std::vector<int>&tags,
                  std::vector<unsigned>&colors,
                  std::vector<int>&Fr,
                  std::vector<int>&Fc,
                  int vertex_count)
{
     cl::Kernel kernel_assign_self_root,
                kernel_assign_WCC_roots,
                kernel_shorten_paths,
                kernel_color_WCC;

    std::vector<std::string>funcname ={"assign_self_root",
                                       "assign_WCC_roots",
                                       "shorten_paths",
                                       "color_WCC"};

    initialize_kernel(funcname[0],kernel_assign_self_root);

    initialize_kernel(funcname[1],kernel_assign_WCC_roots);

    initialize_kernel(funcname[2],kernel_shorten_paths);

    initialize_kernel(funcname[3],kernel_color_WCC);


    cl::Buffer d_tags,
               d_WCC,
               d_colors,
               d_Fc,
               d_Fr,
               d_terminatef,
               d_terminateb;

    d_WCC=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (vertex_count+1)*sizeof(int));

    d_colors=cl::Buffer(context,CL_MEM_READ_WRITE,
                        (colors.size())*sizeof(unsigned));

     d_Fc = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fc.size()));
    
    d_Fr = cl::Buffer(context,CL_MEM_READ_WRITE,
                    sizeof(int)*(Fr.size()));

     d_tags=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(int)*(tags.size()));

     d_terminatef=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(bool));

      d_terminateb=cl::Buffer(context,CL_MEM_READ_WRITE,
                        sizeof(bool));

     queue.enqueueWriteBuffer(d_Fc,
    CL_TRUE,0,sizeof(int)*(Fc.size()),
    (void*)Fc.data());

    queue.enqueueWriteBuffer(d_Fr,
    CL_TRUE,0,sizeof(int)*(Fr.size()),
    (void*)Fr.data());

     queue.enqueueWriteBuffer(d_tags,
    CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());


    queue.enqueueWriteBuffer(d_colors,
    CL_TRUE,0,sizeof(int)*(colors.size()),
    (void*)colors.data());


    kernel_assign_self_root.setArg(0,d_WCC);
    kernel_assign_self_root.setArg(1,d_tags);

    cl::NDRange global(vertex_count+1);
    cl::NDRange local(1);

    queue.enqueueNDRangeKernel
    (kernel_assign_self_root,cl::NullRange,global,local);

    bool terminatef=true,terminateb=true;

    do
    {
        queue.enqueueFillBuffer(d_terminatef,true,0,sizeof(bool));
        kernel_assign_WCC_roots.setArg(0,d_WCC);
        kernel_assign_WCC_roots.setArg(1,d_tags);
        kernel_assign_WCC_roots.setArg(2,d_colors);
        kernel_assign_WCC_roots.setArg(3,d_Fr);
        kernel_assign_WCC_roots.setArg(4,d_Fc);
        kernel_assign_WCC_roots.setArg(5,d_terminatef);

        queue.enqueueNDRangeKernel
        (kernel_assign_WCC_roots,cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminatef,
        CL_TRUE,0,sizeof(bool),(void*)&terminatef);

        queue.enqueueFillBuffer(d_terminateb,true,0,sizeof(bool));

        kernel_shorten_paths.setArg(0,d_WCC);
        kernel_shorten_paths.setArg(1,d_tags);
        kernel_shorten_paths.setArg(2,d_terminateb);

        queue.enqueueNDRangeKernel
        (kernel_shorten_paths,cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_terminateb,
        CL_TRUE,0,sizeof(bool),(void*)&terminateb);




    } while (!terminatef || !terminateb);

    queue.enqueueFillBuffer(d_colors,0,0,sizeof(unsigned)*(vertex_count+1));

    kernel_color_WCC.setArg(0,d_WCC);
    kernel_color_WCC.setArg(1,d_tags);
    kernel_color_WCC.setArg(2,d_colors);

    queue.enqueueNDRangeKernel
    (kernel_color_WCC,cl::NullRange,global,local);

    queue.enqueueReadBuffer(d_colors,CL_TRUE,0,sizeof(unsigned)*(colors.size()),
    (void*)colors.data());

    
    queue.enqueueReadBuffer(d_tags,CL_TRUE,0,sizeof(int)*(tags.size()),
    (void*)tags.data());


    std::cout<<"WCC colors assigned\n";



}

void Graph::find_MST(std::vector<int>&edge_v1,
                    std::vector<int>&edge_v2,
                    std::vector<int>&edge_weight,
                    std::vector<int>&parent,
                    std::vector<int>&mst_edges)
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


     d_edge_v1=cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(edge_count+1));

    d_edge_v2=cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(edge_count+1));

    d_edge_weight = cl::Buffer(context,CL_MEM_READ_WRITE,
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
        
        queue.enqueueFillBuffer(d_no_more_edges,1,0,sizeof(int));
        find_min_edge_per_vertex(d_edge_v1,
                                 d_edge_v2,
                                 d_edge_weight,
                                 d_parent,
                                 d_min_edge_id,
                                 d_min_opp_vertex,
                                 d_no_more_edges);
        
        int no_more_edges=1;
        queue.enqueueReadBuffer(d_no_more_edges,CL_TRUE,0,sizeof(int),
                                   (void*)&no_more_edges);

        if(no_more_edges)
        {
            break;
        }
        remove_mirror_edges(d_min_edge_id,
                             d_min_opp_vertex,
                             d_parent);

        select_mst_edges(d_min_edge_id,
                            d_parent,
                            d_mst_edges);

        init_colors_and_propagate(d_min_edge_id,
                                    d_min_opp_vertex,
                                    d_parent);
    }

    queue.enqueueReadBuffer(d_mst_edges,CL_TRUE,0,sizeof(int)*(edge_count+1),(void*)mst_edges.data());

    
}

void Graph::find_min_edge_per_vertex(cl::Buffer &d_edge_v1,
                                   cl::Buffer &d_edge_v2,
                                   cl::Buffer &d_edge_weight,
                                   cl::Buffer &d_parent,
                                   cl::Buffer &d_min_edge_id,
                                   cl::Buffer &d_min_opp_vertex,
                                   cl::Buffer &d_no_more_edges)
{
    cl::Kernel kernel_select_minimum_weight,
               kernel_select_edge_with_min_opp_vertex,
               kernel_select_edge_id;
    
    std::vector<std::string>funcnames={"select_minimum_weight",
                                       "select_edge_with_min_opp_vertex",
                                       "select_edge_id"};

    initialize_kernel(funcnames[0],kernel_select_minimum_weight);
    initialize_kernel(funcnames[1],kernel_select_edge_with_min_opp_vertex);
    initialize_kernel(funcnames[2],kernel_select_edge_id);
  
    cl::Buffer d_min_weight;
    d_min_weight=cl::Buffer(context,CL_MEM_READ_WRITE,
                         sizeof(int)*(vertex_count+1));
    queue.enqueueFillBuffer(d_min_weight,INT_MAX,0,sizeof(int)*(vertex_count+1));

    kernel_select_minimum_weight.setArg(0,d_edge_v1);
    kernel_select_minimum_weight.setArg(1,d_edge_v2);
    kernel_select_minimum_weight.setArg(2,d_edge_weight);
    kernel_select_minimum_weight.setArg(3,d_min_weight);
    kernel_select_minimum_weight.setArg(4,d_parent);
    kernel_select_minimum_weight.setArg(5,d_no_more_edges);

    cl::NDRange global = cl::NDRange(edge_count+1);
    cl::NDRange local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_select_minimum_weight,cl::NullRange,global,local);

    int no_more_edges=1;

     queue.enqueueReadBuffer(d_no_more_edges,CL_TRUE,0,sizeof(int),
                                   (void*)&no_more_edges);

        if(no_more_edges)
        {
           return;
        }
    
    kernel_select_edge_with_min_opp_vertex.setArg(0,d_edge_v1);
    kernel_select_edge_with_min_opp_vertex.setArg(1,d_edge_v2);
    kernel_select_edge_with_min_opp_vertex.setArg(2,d_edge_weight);
    kernel_select_edge_with_min_opp_vertex.setArg(3,d_min_weight);
    kernel_select_edge_with_min_opp_vertex.setArg(4,d_min_opp_vertex);
    kernel_select_edge_with_min_opp_vertex.setArg(5,d_parent);

    global = cl::NDRange(edge_count+1);
    local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_select_edge_with_min_opp_vertex,cl::NullRange,global,local);


    kernel_select_edge_id.setArg(0,d_edge_v1);
    kernel_select_edge_id.setArg(1,d_edge_v2);
    kernel_select_edge_id.setArg(2,d_edge_weight);
    kernel_select_edge_id.setArg(3,d_min_weight);
    kernel_select_edge_id.setArg(4,d_min_opp_vertex);
    kernel_select_edge_id.setArg(5,d_min_edge_id);
    kernel_select_edge_id.setArg(6,d_parent);

    global = cl::NDRange(edge_count+1);
    local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_select_edge_id,cl::NullRange,global,local);



}

void Graph::remove_mirror_edges(cl::Buffer &d_min_edge_id,
                             cl::Buffer &d_min_opp_vertex,
                             cl::Buffer &d_parent
                             )
{
    cl::Kernel kernel_remove_mirror_edges;
    std::string funcname = "remove_mirror_edges";

    initialize_kernel(funcname,kernel_remove_mirror_edges);

    kernel_remove_mirror_edges.setArg(0,d_min_edge_id);
    kernel_remove_mirror_edges.setArg(1,d_min_opp_vertex);
    kernel_remove_mirror_edges.setArg(2,d_parent);

    cl::NDRange    global = cl::NDRange(vertex_count+1);
    cl::NDRange    local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_remove_mirror_edges,cl::NullRange,global,local);

}


void Graph::select_mst_edges(cl::Buffer &d_min_edge_id,
                           cl::Buffer &d_parent,
                           cl::Buffer &d_mst_edges)
{
    cl::Kernel kernel_select_mst_edges;

    std::string funcname = "select_mst_edges";

    initialize_kernel(funcname,kernel_select_mst_edges);

    kernel_select_mst_edges.setArg(0,d_min_edge_id);
    kernel_select_mst_edges.setArg(1,d_parent);
    kernel_select_mst_edges.setArg(2,d_mst_edges);

    cl::NDRange global = cl::NDRange(vertex_count+1);
    cl::NDRange local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_select_mst_edges,cl::NullRange,global,local);

    std::vector<int>mst_edges(edge_count+1);

     queue.enqueueReadBuffer(d_mst_edges,CL_TRUE,0,sizeof(int)*(edge_count+1),
                               (void*)mst_edges.data());


       
}

void Graph::init_colors_and_propagate(cl::Buffer &d_min_edge_id,
                     cl::Buffer &d_min_opp_vertex,
                     cl::Buffer &d_parent)
{
    cl::Kernel kernel_init_colors,kernel_propagate_colors;

    

    std::vector<std::string> funcname ={"init_colors",
                                      "prop_colors"};

    initialize_kernel(funcname[0],kernel_init_colors);
    initialize_kernel(funcname[1],kernel_propagate_colors);

    kernel_init_colors.setArg(0,d_min_edge_id);
    kernel_init_colors.setArg(1,d_min_opp_vertex);
    kernel_init_colors.setArg(2,d_parent);

    cl::NDRange    global = cl::NDRange(vertex_count+1);
    cl::NDRange    local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    (kernel_init_colors,cl::NullRange,global,local);

    kernel_propagate_colors.setArg(0,d_parent);

    global = cl::NDRange(vertex_count+1);
    local = cl::NDRange(1);

    queue.enqueueNDRangeKernel
    ( kernel_propagate_colors,cl::NullRange,global,local);


}
    
void Graph::max_bipartite_matching(std::vector<int>&matched_edges,
                                 std::vector<struct node2>&edges,
                                 std::vector<int>&outdegree,
                                 std::vector<int>&starting_index,
                                 std::vector<int>&ending_vertex_of_edge,
                                 std::vector<int>&actual_edge_id,
                                 std::vector<int>&colors
                                
                                
                                 )
{
    std::vector<int>vertices_X,vertices_Y,degree,
     degree_one_vertex, multidegree_vertex ;
     try{
    if(is_bipartite(1,
                   
                    outdegree,
                    starting_index,
                    ending_vertex_of_edge,
                    colors))
    {
         std::cout<<"bipartite check done\n";
        for(int i=1;i<=vertex_count;i++)
        {
            if(colors[i]==1)
            {
                vertices_X.push_back(i);
                if(outdegree[i]==1)
                {
                    degree_one_vertex.push_back(i);
                    
                }
                else if(outdegree[i]>1)
                {
                    multidegree_vertex.push_back(i);
                }
            }
           
        }
        cl::Buffer d_edges,
                   d_outdegree,
                   d_starting_index,
                   d_ending_vertex_of_edge,
                   d_vertices_X,
                   d_matched_edges,
                   d_parent_edge_id,
                   d_lookahead,
                   d_matched,
                   d_visited,
                   d_degree_one_vertex,
                   d_outdegree_copy,
                   d_actual_edge_id;

        
        d_edges = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(struct node)*(edges.size()));
        
        d_outdegree = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int)*(outdegree.size()));

        d_starting_index = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int)*(starting_index.size()));

        d_ending_vertex_of_edge = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int)*(ending_vertex_of_edge.size()));

        d_actual_edge_id = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int)*(ending_vertex_of_edge.size()));


        d_vertices_X = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int)*(vertices_X.size()));
        
        d_matched_edges = cl::Buffer(context,CL_MEM_READ_WRITE,
                             sizeof(int)*(matched_edges.size()));

        d_parent_edge_id = cl::Buffer(context,CL_MEM_READ_WRITE,
                             sizeof(int)*(vertex_count+1));

        d_lookahead = cl::Buffer(context,CL_MEM_READ_WRITE,
                                sizeof(int)*(vertex_count+1));

        d_visited = cl::Buffer(context,CL_MEM_READ_WRITE,
                                sizeof(int)*(vertex_count+1));

        d_matched = cl::Buffer(context,CL_MEM_READ_WRITE,
                                sizeof(int)*(vertex_count+1));

        

        d_outdegree_copy = cl::Buffer(context,CL_MEM_READ_WRITE,
                            sizeof(int)*(outdegree.size()));

        
        queue.enqueueWriteBuffer(d_edges,CL_TRUE,0,sizeof(struct node2)*(edges.size()),
                                   (void*)edges.data());

         queue.enqueueWriteBuffer(d_outdegree,CL_TRUE,0,sizeof(int)*(outdegree.size()),
                                   (void*)outdegree.data());

        
         queue.enqueueWriteBuffer(d_outdegree_copy,CL_TRUE,0,sizeof(int)*(outdegree.size()),
                                   (void*)outdegree.data());

        queue.enqueueWriteBuffer(d_starting_index,CL_TRUE,0,sizeof(int)*(starting_index.size()),
                                     (void*)starting_index.data());
        
        queue.enqueueWriteBuffer(d_ending_vertex_of_edge,CL_TRUE,0, sizeof(int)*(ending_vertex_of_edge.size()),
                                    (void*)ending_vertex_of_edge.data());

        queue.enqueueWriteBuffer(d_actual_edge_id,CL_TRUE,0, sizeof(int)*(actual_edge_id.size()),
                                    (void*)actual_edge_id.data());
        queue.enqueueWriteBuffer(d_vertices_X,CL_TRUE,0, sizeof(int)*(vertices_X.size()),
                                    (void*)vertices_X.data());
        
        queue.enqueueFillBuffer(d_matched_edges,0,0,sizeof(int)*(matched_edges.size()));

        queue.enqueueFillBuffer(d_parent_edge_id,-1,0,sizeof(int)*(vertex_count+1));

       // std::vector<int>parent_edge_id(vertex_count+1,0);

        

        queue.enqueueFillBuffer(d_lookahead,-1,0, sizeof(int)*(vertex_count+1));

        queue.enqueueFillBuffer(d_visited,0,0,sizeof(int)*(vertex_count+1));

        queue.enqueueFillBuffer(d_matched,-1,0,sizeof(int)*(vertex_count+1));
                                    
        initialize_matching(multidegree_vertex,
                            d_edges,
                            d_outdegree,
                            d_starting_index,
                            d_ending_vertex_of_edge,
                            d_matched_edges,
                            d_parent_edge_id,
                            d_lookahead,
                            d_visited,
                            d_matched,
                            d_outdegree_copy,
                            d_actual_edge_id
                            );

        queue.enqueueReadBuffer(d_matched_edges,CL_TRUE,0,sizeof(int)*(matched_edges.size()),
                                    (void*)matched_edges.data());

        int matched_count=0;

        std::set<int>s;

        for(int i=1;i<=edge_count;i++)
        {
            if(matched_edges[i])
            {
                matched_count++;
                s.insert(edges[i].v1);
                s.insert(edges[i].v2);
                if(edges[i].v1 >vertex_count || edges[i].v2 > vertex_count)
                std::cout<<"Culprit is here";
            }
        }

        std::cout<<"matched vertices:"<<s.size()<<"\n";

        std::cout<<"after initialize matching,matching count:"<<matched_count<<"\n";

        run_parallel_pothen_fan(d_edges,
                                 d_outdegree,
                                 d_starting_index,
                                 d_ending_vertex_of_edge,
                                 d_vertices_X,
                                 d_matched_edges,
                                 d_parent_edge_id,
                                 d_lookahead,
                                 d_visited,
                                 d_matched,
                                 d_outdegree_copy,
                                 d_actual_edge_id,
                                 vertices_X.size());
        

        queue.enqueueReadBuffer(d_matched_edges,CL_TRUE,0,sizeof(int)*(matched_edges.size()),
                                    (void*)matched_edges.data());
    }else{
        printf("Please provide a bipartite graph");
    }
     }catch(cl::Error error)
        {
            std::cout << error.what() << "(" << error.err() << ")" << std::endl;
            exit(0);
        }
}

bool Graph::is_bipartite(int x,
                  
                  std::vector<int>outdegree,
                  std::vector<int>starting_index,
                  std::vector<int>ending_vertex_of_edge,
                  std::vector<int>&colors)
{
    
    for(int i=1;i<=vertex_count;i++)
    {
        int current_end_index=starting_index[i]+outdegree[i]-1;
        for(int j=starting_index[i];j<=current_end_index;j++)
        {
            int edge_end_vertex=ending_vertex_of_edge[j];
            if(colors[i]==colors[edge_end_vertex])
            {
                //std::cout<<i<<" "<<edge_end_vertex<<"\n";
                return false;
            }
        }


    }
    return true;
    
}

 void Graph::initialize_matching(std::vector<int>&multidegree_vertex,
                                
                             cl::Buffer &d_edges,
                             cl::Buffer &d_outdegree,
                             cl::Buffer &d_starting_index,
                             cl::Buffer &d_ending_vertex_of_edge,
                             
                             cl::Buffer &d_matched_edges,
                             cl::Buffer &d_parent_edge_id,
                             cl::Buffer &d_lookahead,
                             cl::Buffer &d_visited,
                             cl::Buffer &d_matched,
                             cl::Buffer &d_outdegree_copy,
                             cl::Buffer &d_actual_edge_id)
{
    cl::Kernel kernel_initialize_arrays,kernel_match_and_update;
    
    std::vector<std::string>funcname={"initialize_arrays",
                                        "match_and_update"};

    initialize_kernel(funcname[0],kernel_initialize_arrays);
    initialize_kernel(funcname[1],kernel_match_and_update);

    kernel_initialize_arrays.setArg(0,d_visited);
    kernel_initialize_arrays.setArg(1,d_lookahead);
    kernel_initialize_arrays.setArg(2,d_parent_edge_id);
    kernel_initialize_arrays.setArg(3,d_starting_index);

    cl::NDRange global(vertex_count+1);
    cl::NDRange local(1);

    queue.enqueueNDRangeKernel
    (kernel_initialize_arrays,cl::NullRange,global,local);

    kernel_match_and_update.setArg(1,d_actual_edge_id);
    kernel_match_and_update.setArg(2,d_outdegree);
    kernel_match_and_update.setArg(3,d_starting_index);
    kernel_match_and_update.setArg(4,d_ending_vertex_of_edge);
    kernel_match_and_update.setArg(5,d_matched_edges);
    kernel_match_and_update.setArg(6,d_lookahead);
    kernel_match_and_update.setArg(7,d_visited);
    kernel_match_and_update.setArg(8,d_parent_edge_id);
    kernel_match_and_update.setArg(9,d_outdegree_copy);
    kernel_match_and_update.setArg(10,d_matched);

    if(multidegree_vertex.size()>0)
    {
        cl::Buffer d_Q=cl::Buffer(context,CL_MEM_READ_WRITE,
                                    sizeof(int)*(multidegree_vertex.size()));

        queue.enqueueWriteBuffer(d_Q,
        CL_TRUE,0,sizeof(int)*(multidegree_vertex.size()),
        (void*)multidegree_vertex.data());

        std::cout<<"multidegree vertices\n";

       kernel_match_and_update.setArg(0,d_Q);

        global=cl::NDRange(multidegree_vertex.size());

        local=cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_match_and_update,cl::NullRange,global,local);
    }






}

void Graph::run_parallel_pothen_fan(cl::Buffer &d_edges,
                                 cl::Buffer &d_outdegree,
                                 cl::Buffer &d_starting_index,
                                 cl::Buffer &d_ending_vertex_of_edge,
                                 cl::Buffer &d_vertices_X,
                                 cl::Buffer &d_matched_edges,
                                 cl::Buffer &d_parent_edge_id,
                                 cl::Buffer &d_lookahead,
                                 cl::Buffer &d_visited,
                                 cl::Buffer &d_matched,
                                 cl::Buffer &d_outdegree_copy,
                                 cl::Buffer &d_actual_edge_id,
                                 int vertex_count_X
                                 )
{
    cl::Kernel kernel_initialize_arrays, kernel_find_augmenting_paths;

    std::vector<std::string>funcnames={"initialize_arrays",
                                       "find_augmenting_paths"};

    initialize_kernel(funcnames[0],kernel_initialize_arrays);
    initialize_kernel(funcnames[1],kernel_find_augmenting_paths);

    int path_found=0;

    cl::Buffer d_path_found=cl::Buffer(context,CL_MEM_READ_WRITE,
                                sizeof(int));
    do
    {
        path_found=0;
        queue.enqueueFillBuffer(d_path_found,0,0,sizeof(int));

        
        kernel_initialize_arrays.setArg(0, d_visited);   // __global int *visited,
        
        
        kernel_initialize_arrays.setArg(1, d_lookahead);//                         __global int *lookahead,
        kernel_initialize_arrays.setArg(2, d_parent_edge_id);         //                         __global int *parent_edge_id,
        kernel_initialize_arrays.setArg(3, d_starting_index); //                         __global int *starting_index

        cl::NDRange global(vertex_count+1);
        cl::NDRange local(1);

        queue.enqueueNDRangeKernel
        (kernel_initialize_arrays,cl::NullRange,global,local);

        kernel_find_augmenting_paths.setArg(0, d_edges);       // __global struct node* edges,
        kernel_find_augmenting_paths.setArg(1, d_outdegree);       // __global int* outdegree,
        kernel_find_augmenting_paths.setArg(2, d_starting_index);       // __global int* starting_index,
        kernel_find_augmenting_paths.setArg(3, d_ending_vertex_of_edge);      // __global int* ending_vertex_of_edge,
        kernel_find_augmenting_paths.setArg(4, d_vertices_X);      // __global int* vertices_X,
        kernel_find_augmenting_paths.setArg(5, d_matched_edges);      // __global int* matched_edges,
        kernel_find_augmenting_paths.setArg(6, d_parent_edge_id);      // __global int* parent_edge_id,
        kernel_find_augmenting_paths.setArg(7, d_lookahead);      // __global int* lookahead,
        kernel_find_augmenting_paths.setArg(8, d_visited);      // __global int* visited,


        kernel_find_augmenting_paths.setArg(9, d_path_found);                        // __global int* path_found,
        kernel_find_augmenting_paths.setArg(10, d_matched);                        // __global int* matched

        kernel_find_augmenting_paths.setArg(11, d_actual_edge_id);
        global=cl::NDRange(vertex_count_X);
        local=cl::NDRange(1);

        queue.enqueueNDRangeKernel
        (kernel_find_augmenting_paths,cl::NullRange,global,local);

        queue.enqueueReadBuffer(d_path_found,
        CL_TRUE,0,sizeof(int),
        (void*)&path_found);
        

    } while (path_found!=0);
}

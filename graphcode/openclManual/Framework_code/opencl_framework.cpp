#include<bits/stdc++.h>
#define INF INT_MAX
#define __CL_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220 
#define CL_DEVICE_TYPE_DEFAULT CL_DEVICE_TYPE_GPU 
#include<CL/cl2.hpp>
#include<stdint.h>


class Graph
{
    int vertex_count;
    int edge_count;
    std::string kernel_filename;
    std::vector<cl::Platform>platforms;
    cl::Context context;
    std::vector<cl::Device>devices;
    cl::CommandQueue queue;
    cl::Program program;

    void initialize_kernel(std::string &funcname,
                               cl::Kernel &kernel );
    public :
     Graph();
    
    void trim1(std::vector<int>&tags,
                std::vector<int>&Fc,
                std::vector<int>&Fr,
                std::vector<int>&Bc,
                std::vector<int>&Br,
                std::vector<unsigned>&colors,
                int &trim_count,
                int vertex_count);

    void trim2(std::vector<int>&tags,
                std::vector<int>&Fc,
                std::vector<int>&Fr,
                std::vector<int>&Bc,
                std::vector<int>&Br,
                std::vector<unsigned>&colors,
                int vertex_count);

    void bfs(std::vector<int>&Fr,
             std::vector<int>&Fc,
             std::vector<int>&tags,
             std::vector<unsigned>&colors);

    void WCC_find(std::vector<int>&WCC,
                  std::vector<int>&tags,
                  std::vector<unsigned>&colors,
                  std::vector<int>&Fr,
                  std::vector<int>&Fc);
    
    
    
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

    
    std::vector<unsigned>colors(vertex_count+1);

    std::vector<int>tags(vertex_count+1);

    int trim_count=0;

    try{
    g.trim1(tags,
             Fc,
             Fr,
             Bc,
             Br,
             colors,
             trim_count,
             vertex_count);
    }
     catch(cl::Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
        exit(0);
        }

    std::cout<<"trim1 count:"<<trim_count<<"\n";
}
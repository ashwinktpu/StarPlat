#include "myheader.h"
using namespace std;
#include<queue>
#include<chrono>

/*
In this algorithm I have implemented preflow push relabel in opencl backend
1.I have applied some heuristics vertex removal that is remove the verticeswhich don't have any contribution in the maxflow
2.I have called kernel which does push relabel operation on each vertices concurrently
3.After each kernel call I have called my global relabel heuristic to update the heights of the vertices 
4.graph.hpp is used to parse the graph I have added two more function in this for creating the residual graph CSR
*/

void bfs(int source,int V,int E,int * index,int *edge_List,int *khush1,int ss,int tt){
    queue<int> q;
    khush1[source]=1;
    
    int *visited=(int*)calloc(V+1,sizeof(int));
    visited[source]=1;
    q.push(source);
    while(!q.empty()){
        int u=q.front();
       // cout<<u<<"   is reachable  from  "<<source<<endl;
        q.pop();
        if(source ==tt && u==ss) continue;
        if(source==ss && u==tt) continue;
        int start=index[u];
        int end=E-1;
        if(u<V-1) end=index[u+1]-1;
        for(int i=start;i<=end;i++){
            int v=edge_List[i];
            if(visited[v]==0){
                visited[v]=1;
                if(v==3){
                    cout<<"3 is reachable form "<<source<<" as there is an edge from "<<u<<"  to   "<<v<<endl;
                }
                khush1[v]=1;
                q.push(v);
            }
        }

    }
}



int main (int argc, char **argv) {

    int source=0;
    int sink=1;



    /*********************************************************Parsing Started**************************************************************************/
    int *cpu_index,*cpu_edgeList,*cpu_edgeLength,*cpu_Rindex,*cpu_RedgeList,*cpu_RedgeLength;// cpu network and cpu residual graph
    int *rev_cpu_index,*rev_cpu_edgeList,*simple_revIndex,*simple_revList;
    

    char* filepath=argv[1];
    graph g(filepath),rg(filepath);



    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();
    g.parseGraph();
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    std::cout << "Time required: to parse Original graphs is " << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;


    tic=std::chrono::steady_clock::now();
    rg.parseGraphResidual();
    toc=std::chrono::steady_clock::now();
    std::cout << "Time required: to parse Residual graphs is " << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;
    
    int V=g.num_nodes();
    int E=g.num_edges();

    int Vr=rg.num_nodes();
    int Er=rg.num_edges();

    cout<<"in original graphs  nodes are "<<V<<"  edges are  "<<E<<endl;
    cout<<"in residual graphs  nodes are "<<Vr<<"  edges are  "<<Er<<endl;
    

    cpu_index=(int*)malloc((V+1) * sizeof(int));
    cpu_edgeList=(int*)malloc((E) * sizeof(int));
    cpu_edgeLength=(int*)malloc((E) *sizeof(int));

    cpu_Rindex=(int*)malloc((V+1) * sizeof(int));
    cpu_RedgeList=(int*)malloc((Er) *sizeof(int));
    cpu_RedgeLength=(int*)malloc((Er) *sizeof(int));


    simple_revIndex=(int*)malloc((V) * sizeof(int));
    simple_revList=(int*)malloc((E) *sizeof(int));


    rev_cpu_index=(int*)malloc((V) * sizeof(int)); 
    rev_cpu_edgeList=(int*)malloc((Er) *sizeof(int));
    

    for(int i=0;i<V;i++){
        cpu_index[i]=g.indexofNodes[i];
        simple_revIndex[i]=g.rev_indexofNodes[i];
        //cout<<g.rev_indexofNodes[i]<<"   ";
    }
    //cout<<endl;

    int *p=g.getEdgeLen();
    for(int i=0;i<E;i++){
        cpu_edgeList[i]=g.edgeList[i];
        cpu_edgeLength[i]=p[i];
        simple_revList[i]=g.srcList[i];
        //cout<<g.srcList[i]<<"  ";
        
    }


    for(int i=0;i<V;i++) {
        cpu_Rindex[i]=rg.indexofNodes[i];
        rev_cpu_index[i]=rg.rev_indexofNodes[i];
    }


    int *q=rg.getEdgeLen();
    for(int i=0;i<Er;i++){
        cpu_RedgeList[i]=rg.edgeList[i];
        cpu_RedgeLength[i]=q[i];
        rev_cpu_edgeList[i]=rg.srcList[i];
        
    }

    // declaring variables to store graph data on host as well as on CUDA device global memory 
    int *cpu_height;
    //int *gpu_height;
    int *cpu_excess_flow;
    //int *gpu_excess_flow;
    int *Excess_total;

    cpu_height = (int*)malloc(V*sizeof(int));
    cpu_excess_flow = (int*)malloc(V*sizeof(int));
    Excess_total = (int*)malloc(sizeof(int));


//     /*********************************************************Parsing Completed**************************************************************************/


    int *khush1;
    khush1=(int*)calloc((V+1),sizeof(int));
    bfs(source,V,E,cpu_index,cpu_edgeList,khush1,source,sink);

    int *khush2;
    khush2=(int*)calloc((V+1),sizeof(int));
    bfs(sink,V,E,simple_revIndex,simple_revList,khush2,source,sink);

    int *khush;
    khush=(int*)calloc((V+1),sizeof(int));
    for(int i=0;i<V;i++){
        if(khush1[i]==1 && khush2[i]==1){
            //cout<<i<<"   common\n";
            khush[i]=1;
        }
    }
    // cout<<"khush[3] "<<khush1[3]<<khush2[3]<<khush[3]<<endl;
    // cout<<"khush[6] "<<khush[6]<<endl<<endl;



    //deleting all the edges from the sink in the original graph
    int akss=cpu_index[sink];
    int akse=E;
    if(sink<V-1) akse= cpu_index[sink+1]-1;
    for(int i=akss;i<=akse;i++){
        cpu_edgeLength[i]=0;
    }
    //deleting all the edges from the sink in the residual graph

    akss=cpu_Rindex[sink];
    akse=Er;
    if(sink<V-1) akse= cpu_Rindex[sink+1]-1;
    for(int i=akss;i<=akse;i++){
        cpu_RedgeLength[i]=0;
    }

    //deleting all the edges to the the sink in the original graph
    akss=simple_revIndex[source];
    akse=E;
    if(source<V-1) akse= simple_revIndex[source+1]-1;
    for(int i=akss;i<=akse;i++){
        int v=simple_revList[i];
        change(V,E,v,source,cpu_index,cpu_edgeList,cpu_edgeLength,0);
    }


    //deleting all the edges to the the sink in the residual graph
    akss=rev_cpu_index[source];
    akse=Er;
    if(source<V-1) akse= rev_cpu_index[source+1]-1;
    for(int i=akss;i<=akse;i++){
        int v=rev_cpu_edgeList[i];
        change(V,E,v,source,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,0);
    }


    cout<<"Calling preflow"<<endl;
    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  SET THE PREFLOW    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
    preflow(V,E ,Er,source, sink,
            cpu_index,cpu_edgeList,cpu_edgeLength,
            cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,
            cpu_height,cpu_excess_flow,
            Excess_total ,khush);


    cout<<"Excess Total after preflow is "<<*Excess_total<<endl;
    cout<<cpu_excess_flow[source]<<"     "<<cpu_excess_flow[sink]<<endl;



    /*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^OPENCL STARTA^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

        //Getting platforms
        cl_int status;
        cl_platform_id *platforms = NULL;
        cl_uint number_of_platforms;
        status = clGetPlatformIDs(0, NULL, &number_of_platforms);
        platforms = (cl_platform_id *)malloc(number_of_platforms*sizeof(cl_platform_id));
        status = clGetPlatformIDs(number_of_platforms, platforms, NULL);
        printf("Got Platform, status = %d \n", status);

        //Getting Devices present on platform
        cl_device_id *devices= NULL;
        cl_uint number_of_devices;
        status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU, 0, NULL, &number_of_devices);
        devices = (cl_device_id *)malloc(number_of_devices*sizeof(cl_device_id));
        status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, number_of_devices, devices, NULL);
        printf("Got Devices, status = %d \n", status);

        //Creating context
        cl_context context;
        context = clCreateContext(NULL, number_of_devices, devices, NULL, NULL, &status);

        //Creating command queue
        cl_command_queue command_queue ;
        command_queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE , &status);
        printf("command queue created, status = %d\n", status);
        cout<<" atleast opencl startted"<<endl<<endl;






    //Allocating memory for 8 arrays 
    cl_mem gpu_index = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
    //printf("ccreating success, status = %d\n", status);
    cl_mem gpu_edgeList = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_edgeLength= clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
     //printf("ccreating success, status = %d\n", status);



    cl_mem gpu_Rindex = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_RedgeList = clCreateBuffer(context, CL_MEM_READ_WRITE, (Er)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_RedgeLength= clCreateBuffer(context, CL_MEM_READ_WRITE, (Er)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);



    cl_mem gpu_height= clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_excess_flow= clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
    //  printf("ccreating success, status = %d\n", status);


    cl_mem gpu_khush= clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);

    //allocating memory completed

    //&&&&&&&&&&&&&&&&&&&Transfer of 8 arrays statted&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //status = clEnqueueWriteBuffer(command_queue,   gpu_index , CL_TRUE, 0, sizeof(int)*(V+1),   cpu_index, 0, NULL, NULL );
    
     status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
   // printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
        printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }

    status = clEnqueueWriteBuffer(command_queue,   gpu_edgeList , CL_TRUE, 0, sizeof(int)*E,   cpu_edgeList, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
        //printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }

    status = clEnqueueWriteBuffer(command_queue,    gpu_edgeLength , CL_TRUE, 0, sizeof(int)*E,    cpu_edgeLength, 0, NULL, NULL );
   // printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
    // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }


    status = clEnqueueWriteBuffer(command_queue, gpu_Rindex , CL_TRUE, 0, sizeof(int)*(V+1), cpu_Rindex, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
    // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }

    status = clEnqueueWriteBuffer(command_queue, gpu_RedgeList , CL_TRUE, 0, sizeof(int)*(Er), cpu_RedgeList, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
        printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }

    status = clEnqueueWriteBuffer(command_queue, gpu_RedgeLength , CL_TRUE, 0, sizeof(int)*(Er), cpu_RedgeLength, 0, NULL, NULL );
   // printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
    // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }


    status = clEnqueueWriteBuffer(command_queue,   gpu_height , CL_TRUE, 0, sizeof(int)*(V),   cpu_height, 0, NULL, NULL );
   // printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
    // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }

    status = clEnqueueWriteBuffer(command_queue,   gpu_excess_flow , CL_TRUE, 0, sizeof(int)*(V),   cpu_excess_flow, 0, NULL, NULL );
   // printf("copying success, status = %d\n", status);
    if (status != CL_SUCCESS) {
    // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
        // Handle the error, possibly return or exit the program
    }

    status = clEnqueueWriteBuffer(command_queue,   gpu_khush , CL_TRUE, 0, sizeof(int)*(V),   khush, 0, NULL, NULL );


    //cout<<"CSR transfer to OpenCl memory completed\n"<<endl;
    // /**********************************************CSR TRANSFER END**********************************************************************/


    /* Instead of checking for overflowing vertices(as in the sequential push relabel),
     * sum of excess flow values of sink and source are compared against Excess_total 
     * If the sum is lesser than Excess_total, 
     * it means that there is atleast one more vertex with excess flow > 0, apart from source and sink
     */

    /* declaring the mark and scan boolean arrays used in the global_relabel routine outside the while loop 
     * This is not to lose the mark values if it goes out of scope and gets redeclared in the next iteration 
     */

    bool *mark,*scanned;
    mark = (bool*)malloc(V*sizeof(bool));
    scanned = (bool*)malloc(V*sizeof(bool));

    // initialising mark values to false for all nodes
    for(int i = 0; i < V; i++)
    {
        mark[i] = false;
    }

    int sss=0;
    tic=std::chrono::steady_clock::now();
    while(   (  (cpu_excess_flow[source] + cpu_excess_flow[sink]) < *Excess_total) && ++sss )
    //while(   (  (cpu_excess_flow[source] + cpu_excess_flow[sink]) < *Excess_total))
    {   
    cout<<"*************************************again inside the main loop for  "<< sss<<"    time **********************************\n ";
        cout<<"cpu_excess_flow[source]   "<<cpu_excess_flow[source]<<endl;
        cout<<"cpu_excess_flow[sink]    "<<cpu_excess_flow[sink]<<endl;
        cout<<"*Excess_total   "<<*Excess_total<<endl;
    toc=std::chrono::steady_clock::now();
    std::cout << "Time" << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;


    if(sss>200) break;
       




        // copying height values to OPENCl device global memory
        //cudaMemcpy(gpu_height,cpu_height,V*sizeof(int),cudaMemcpyHostToDevice);

        status = clEnqueueWriteBuffer(command_queue,   gpu_height , CL_TRUE, 0, sizeof(int)*(V),   cpu_height, 0, NULL, NULL );
        status = clEnqueueWriteBuffer(command_queue,   gpu_excess_flow , CL_TRUE, 0, sizeof(int)*(V),   cpu_excess_flow, 0, NULL, NULL );
        status = clEnqueueWriteBuffer(command_queue, gpu_RedgeLength , CL_TRUE, 0, sizeof(int)*(Er), cpu_RedgeLength, 0, NULL, NULL );

        
        
//####################################################launching the kernel#####################################################################

        // push_relabel_kernel<<<number_of_blocks_nodes,threads_per_block>>>
        // (V,E,source,sink ,gpu_height,gpu_excess_flow,gpu_index,gpu_edgeList,gpu_edgeLength,gpu_Rindex,gpu_RedgeList,gpu_RedgeLength);
        // clFinish(command_queue);
          //Reading kernel file

        cl_event event;



        FILE* kernelfp = fopen("mypush_relabel_kernel.cl", "rb"); 
        size_t program_size;
        fseek(kernelfp, 0, SEEK_END);
        program_size = ftell(kernelfp);
        rewind(kernelfp);
        char* kernelSource;
        kernelSource = (char *)malloc((program_size+1)* sizeof(char));
        fread(kernelSource, sizeof(char), program_size, kernelfp);
        kernelSource[program_size] = (char)NULL ;
        fclose(kernelfp);

          //Creating program from source(Create and build Program)
        cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &status);
        //printf("Progran created from source, status = %d \n", status);
        status = clBuildProgram(program, number_of_devices, devices, " -I ./", NULL, NULL);
       // printf(" Program building completed, status = %d \n ",status);

          //Variable for launch configuration
        size_t global_size;
        size_t local_size;
        cl_kernel pr_kernel = clCreateKernel(program, "push_relabel_kernel", &status);
        if(status != CL_SUCCESS){
                char build_log[4096];
                clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
                printf("Build log:\n%s\n", build_log);
                printf("Program building failed, status = %d \n", status);
                return 0 ;
        }
        else{
           // printf("AKS     successful to create push_relabel_kernel.\n");
        }

//V,E,source,sink ,gpu_height,gpu_excess_flow,gpu_index,gpu_edgeList,gpu_edgeLength,gpu_Rindex,gpu_RedgeList,gpu_RedgeLength
        status = clSetKernelArg(pr_kernel,0, sizeof(int),(void *)&V);
        status = clSetKernelArg(pr_kernel,1, sizeof(int),(void *)&E);
        status = clSetKernelArg(pr_kernel,2, sizeof(int),(void *)&Er);
        status = clSetKernelArg(pr_kernel,3, sizeof(int),(void *)&source);
        status = clSetKernelArg(pr_kernel,4, sizeof(int),(void *)&sink);

        status = clSetKernelArg(pr_kernel,5, sizeof(cl_mem),(void *)&gpu_height);
        status = clSetKernelArg(pr_kernel,6, sizeof(cl_mem),(void *)&gpu_excess_flow);

        status = clSetKernelArg(pr_kernel,7, sizeof(cl_mem),(void *)&gpu_index);
        status = clSetKernelArg(pr_kernel,8, sizeof(cl_mem),(void *)&gpu_edgeList);
        status = clSetKernelArg(pr_kernel,9, sizeof(cl_mem),(void *)&gpu_edgeLength);

        status = clSetKernelArg(pr_kernel,10, sizeof(cl_mem),(void *)&gpu_Rindex);
        status = clSetKernelArg(pr_kernel,11, sizeof(cl_mem),(void *)&gpu_RedgeList);
        status = clSetKernelArg(pr_kernel,12, sizeof(cl_mem),(void *)&gpu_RedgeLength);

        status = clSetKernelArg(pr_kernel,13, sizeof(cl_mem),(void *)&gpu_khush);

        local_size = 1024;
        global_size = (V + local_size -1)/ local_size * local_size;
        cout<<"local size is "<<local_size<<"   and global size is  "<<global_size<<endl;
        //cout<<"Calling kernel\n";

        status = clEnqueueNDRangeKernel(command_queue,pr_kernel, 1,NULL, &global_size, &local_size , 0,NULL,&event);
        clWaitForEvents(1,&event);
        //cout<<"end of the kernel\n";


////###########################################################################################################################################



       // ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL);
        status = clEnqueueReadBuffer(command_queue, gpu_height, CL_TRUE, 0, sizeof(int) * V, cpu_height, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
        status = clEnqueueReadBuffer(command_queue, gpu_excess_flow, CL_TRUE, 0, sizeof(int) * V, cpu_excess_flow, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
       // status = clEnqueueReadBuffer(command_queue, gpu_Rindex, CL_TRUE, 0, sizeof(int) * (V+1), cpu_Rindex, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
       // status = clEnqueueReadBuffer(command_queue, gpu_RedgeList, CL_TRUE, 0, sizeof(int) * Er, cpu_RedgeList, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
        status = clEnqueueReadBuffer(command_queue, gpu_RedgeLength, CL_TRUE, 0, sizeof(int)* Er, cpu_RedgeLength, 0, NULL, NULL);

        Akglobal_relabel(V,E,Er,source,sink,
                        cpu_height,cpu_excess_flow,
                        Excess_total,mark,scanned,
                        cpu_index,cpu_edgeList,cpu_edgeLength,
                        cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,
                        rev_cpu_index,rev_cpu_edgeList,khush);



        printf("\nAfter global relabel\n");
        cout<<"cpu_excess_flow[source]   "<<cpu_excess_flow[source]<<endl;
        cout<<"cpu_excess_flow[sink]    "<<cpu_excess_flow[sink]<<endl;
        cout<<"*Excess_total   "<<*Excess_total<<endl;


    }

    toc=std::chrono::steady_clock::now();
    std::cout << "Time required: PP is  " << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;
    cout<<sss<<" so breaking\n";


    cout<<endl<<endl;
    cout<<"Anup Kumar Singh your answer is  "<<cpu_excess_flow[sink]<<endl;

//closing for main
}

#include "myheader.h"
using namespace std;

int main(int argc, char **argv){
    char *filepath=argv[1];
    graph g(filepath);
    g.parseGraph();
    int V=g.num_nodes();
    int E=g.num_edges();
    cout<<"no of nodes are "<<V<<endl;
    cout<<"no of edges are "<<E<<endl;
    int *cpu_index,*cpu_edgeList;
    cpu_index=(int*)malloc((V+1) * sizeof(int));
    cpu_edgeList=(int*)malloc((E) * sizeof(int));


    int *h_active,*hc_degree,*hn_degree,*hflag;
    h_active=(int*)malloc(V * sizeof(int));
    hc_degree=(int*)malloc((V) * sizeof(int));
    hn_degree=(int*)malloc((V) * sizeof(int));
    hflag=(int*)malloc((1) * sizeof(int));
    hflag[0]=1;
    // cout<<"no of nodes are2 "<<V<<endl;
    // cout<<"no of edges are2 "<<E<<endl;


    for(int i=0;i<V;i++){
        cpu_index[i]=g.indexofNodes[i];
        h_active[i]=1;
        hc_degree[i]=0;
        hn_degree[i]=0;
    }
    cpu_index[V]=g.indexofNodes[V];
    // for(int i=0;i<=V;i++) cout<<cpu_index[i]<<"  ";
    // cout<<endl;

    for(int i=0;i<E;i++){
        int t=g.edgeList[i];
        cpu_edgeList[i]=t;
        hc_degree[t]=hc_degree[t]+1;
        hn_degree[t]=hn_degree[t]+1;
    }
    // for(int i=0;i<E;i++) cout<<cpu_edgeList[i]<<" ";
    // cout<<endl;
    // for(int i=0;i<V;i++){
    //     cout<<i<<" indegree   "<<hc_degree[i]<<endl;
    // }



    
    // //Getting platforms
    cout<<"ANUp\n";
    cl_int status;
    // cout<<"Singh\n";
    cl_platform_id *platforms = NULL;
    cout<<"Kumar\n";
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
    




    cl_mem gpu_index = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
    cl_mem gpu_edgeList = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
    cl_mem d_active = clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
    cl_mem dc_degree = clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
    cl_mem dn_degree= clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
    cl_mem dflag = clCreateBuffer(context, CL_MEM_READ_WRITE, (1)*sizeof(int), NULL, &status);



    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(command_queue, gpu_edgeList, CL_TRUE, 0, sizeof(int)*(E), cpu_edgeList, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(command_queue, d_active, CL_TRUE, 0, sizeof(int)*(V), h_active, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(command_queue, dc_degree, CL_TRUE, 0, sizeof(int)*(V),hc_degree, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(command_queue, dn_degree, CL_TRUE, 0, sizeof(int)*(V), hn_degree, 0, NULL, NULL);
    cout<<"at the start of teh main loop\n";

    int *tempzero=(int*)malloc((1) * sizeof(int));
    tempzero[0]=0;
    int k=2;

    while(hflag[0]==1){

        //setting the dflag to zero so that if there is any vertex it will again be set by the next kernel
        status = clEnqueueWriteBuffer(command_queue, dflag, CL_TRUE, 0, sizeof(int)*(1), tempzero, 0, NULL, NULL);

        /**************************************************************************************start of my   my kernel**************************************************************************/
            cl_event event;
            FILE* kernelfp = fopen("kcdkernel.cl", "rb"); 
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
            cl_kernel pr_kernel = clCreateKernel(program, "kcd_kernel", &status);
            if(status != CL_SUCCESS){
                    char build_log[4096];
                    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
                    printf("Build log:\n%s\n", build_log);
                    printf("Program building failed, status = %d \n", status);
                    return 0 ;
            }
            else{
                printf("AKS     successful to create kcd_kernel.\n");
            }

            status = clSetKernelArg(pr_kernel,0, sizeof(int),(void *)&V);
            status = clSetKernelArg(pr_kernel,1, sizeof(int),(void *)&E);
            status = clSetKernelArg(pr_kernel,2, sizeof(int),(void *)&k);
            status = clSetKernelArg(pr_kernel,3, sizeof(cl_mem),(void *)&gpu_index);
            status = clSetKernelArg(pr_kernel,4, sizeof(cl_mem),(void *)&gpu_edgeList);
            status = clSetKernelArg(pr_kernel,5, sizeof(cl_mem),(void *)&d_active);
            status = clSetKernelArg(pr_kernel,6, sizeof(cl_mem),(void *)&dc_degree);
            status = clSetKernelArg(pr_kernel,7, sizeof(cl_mem),(void *)&dn_degree);
            status = clSetKernelArg(pr_kernel,8, sizeof(cl_mem),(void *)&dflag);


            local_size = 1024;
            global_size = (V + local_size -1)/ local_size * local_size;
            // local_size=1;
            // global_size=1;
            //cout<<"global size is "<<global_size<<endl;
            status = clEnqueueNDRangeKernel(command_queue,pr_kernel, 1,NULL, &global_size, &local_size , 0,NULL,&event);
            clWaitForEvents(1,&event);
        /****************************************************************************End of my kernel************************************************************************/
        status = clEnqueueReadBuffer(command_queue, dn_degree, CL_TRUE, 0, sizeof(int) * V, hc_degree, 0, NULL, NULL);
        status = clEnqueueWriteBuffer(command_queue, dc_degree, CL_TRUE, 0, sizeof(int)*(V), hc_degree, 0, NULL, NULL);

        
        status = clEnqueueReadBuffer(command_queue, dflag, CL_TRUE, 0, sizeof(int) * 1, hflag, 0, NULL, NULL);
        cout<<"vaue of the hflag is now"<<hflag[0]<<endl;

    }
    status = clEnqueueReadBuffer(command_queue, d_active, CL_TRUE, 0, sizeof(int) * V, h_active, 0, NULL, NULL);
    cout<<"out of the main loop\n";
    int count=0;
    cout<<"No. of vertices that survived kCD are\n";
    for(int i=0;i<V;i++){
        if(h_active[i]){
            cout<<i<<" ";
            count++;
        }
    }
    cout<<endl<<"total count is "<<count<<endl;

}

/*

{
            cl_event event;
            FILE* kernelfp = fopen("setvar.cl", "rb"); 
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
            cl_kernel pr_kernel = clCreateKernel(program, "set_var_kernel", &status);
            if(status != CL_SUCCESS){
                    char build_log[4096];
                    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
                    printf("Build log:\n%s\n", build_log);
                    printf("Program building failed, status = %d \n", status);
                    return 0 ;
            }
            else{
                printf("AKS     successful to create push_relabel_kernel.\n");
            }
            status = clSetKernelArg(pr_kernel,0, sizeof(cl_mem),(void *)&dflag);
            local_size=1;
            global_size=1;
            status = clEnqueueNDRangeKernel(command_queue,pr_kernel, 1,NULL, &global_size, &local_size , 0,NULL,&event);
            clWaitForEvents(1,&event);

        }
*/
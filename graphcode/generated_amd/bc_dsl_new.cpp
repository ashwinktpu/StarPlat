#include "bc_dsl_new.h"

void Compute_BC_new(graph& g,double * BC,std::set<int>& sourceSet)
{
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

  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  cl_mem d_meta = clCreateBuffer(context, CL_MEM_READ_WRITE, (1+V)*sizeof(int), NULL, &status);
  cl_mem d_data = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
  cl_mem d_src = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
  cl_mem d_weight = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
  cl_mem d_rev_meta = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
  cl_mem d_modified_next = clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);

  status = clEnqueueWriteBuffer(command_queue,   d_meta , CL_TRUE, 0, sizeof(int)*(V+1),   h_meta, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue,   d_data , CL_TRUE, 0, sizeof(int)*E,   h_data, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue,    d_src , CL_TRUE, 0, sizeof(int)*E,    h_src, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue, d_weight , CL_TRUE, 0, sizeof(int)*E, h_weight, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue, d_rev_meta , CL_TRUE, 0, sizeof(int)*(V+1), h_rev_meta, 0, NULL, NULL );

  // CSR END

  // TIMER START
  cl_event event;
  double kernelTime ;
  double totalTime = 0.0;
  cl_ulong start, end;
  cl_ulong convertToMS = 1e6;

  //DECLAR DEVICE AND HOST vars in params
  cl_mem d_BC = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(double),NULL, &status);


  //BEGIN DSL PARSING 

  //Reading kernel file
  FILE* kernelfp = fopen("bc_dsl_new.cl", "rb"); 
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
  printf("Progran created from source, status = %d \n", status);
  status = clBuildProgram(program, number_of_devices, devices, " -I ./", NULL, NULL);
  printf(" Program building completed, status = %d \n ",status);

  //Variable for launch configuration
  size_t global_size;
  size_t local_size;
  // Creating initBC_kernel  Kernel
  cl_kernel initBC_kernel = clCreateKernel(program, "initBC_kernel", &status);

  // Initialization for BC variable
  double BCValue = (double)0; 
  status = clSetKernelArg(initBC_kernel, 0 , sizeof(cl_mem), (void *)& d_BC);
  status = clSetKernelArg(initBC_kernel, 1, sizeof(double) , (void*)& BCValue);
  status = clSetKernelArg(initBC_kernel, 2, sizeof(int), (void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initBC_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initBC_kernel);

  double *h_sigma = (double *)malloc(V*sizeof(double));
  cl_mem d_sigma = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(double),NULL, &status);

  double *h_delta = (double *)malloc(V*sizeof(double));
  cl_mem d_delta = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(double),NULL, &status);


  //ForAll started here
  //FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++) 
  {
    int src = *itr;
    // Creating initdelta_kernel  Kernel
    cl_kernel initdelta_kernel = clCreateKernel(program, "initdelta_kernel", &status);

    // Initialization for delta variable
    double deltaValue = (double)0; 
    status = clSetKernelArg(initdelta_kernel, 0 , sizeof(cl_mem), (void *)& d_delta);
    status = clSetKernelArg(initdelta_kernel, 1, sizeof(double) , (void*)& deltaValue);
    status = clSetKernelArg(initdelta_kernel, 2, sizeof(int), (void*)&V);
    local_size = 128;
    global_size = (V + local_size -1)/ local_size * local_size;
    status = clEnqueueNDRangeKernel(command_queue, initdelta_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;
    status = clReleaseKernel(initdelta_kernel);

    // Creating initsigma_kernel  Kernel
    cl_kernel initsigma_kernel = clCreateKernel(program, "initsigma_kernel", &status);

    // Initialization for sigma variable
    double sigmaValue = (double)0; 
    status = clSetKernelArg(initsigma_kernel, 0 , sizeof(cl_mem), (void *)& d_sigma);
    status = clSetKernelArg(initsigma_kernel, 1, sizeof(double) , (void*)& sigmaValue);
    status = clSetKernelArg(initsigma_kernel, 2, sizeof(int), (void*)&V);
    local_size = 128;
    global_size = (V + local_size -1)/ local_size * local_size;
    status = clEnqueueNDRangeKernel(command_queue, initsigma_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;
    status = clReleaseKernel(initsigma_kernel);

    cl_kernel initIndexsigma_kernel = clCreateKernel(program, "initIndexsigma_kernel", &status);
    //Indexsigma src initialization
    double initsigmavalsrc = 1;
    status = clSetKernelArg(initIndexsigma_kernel , 0,sizeof(cl_mem) ,(void *)&d_sigma);
    status = clSetKernelArg(initIndexsigma_kernel, 1,sizeof(int), (void*)&src);
    status = clSetKernelArg(initIndexsigma_kernel, 2, sizeof(double),(void *)&initsigmavalsrc);
    global_size = local_size = 1;
    status  = clEnqueueNDRangeKernel(command_queue, initIndexsigma_kernel,1,NULL, &global_size, &local_size, 0, NULL, &event);
    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;

    status = clReleaseKernel(initIndexsigma_kernel);

    //Iterate in BFS to be implemented.
    int finished;
    int hops_from_source;
    cl_mem d_finished = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR,sizeof(int), NULL, &status);
    cl_mem d_hops_from_source = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR, sizeof(int), NULL, &status);
    cl_mem d_level = clCreateBuffer(context, CL_MEM_READ_WRITE, V*sizeof(int), NULL, &status);
    //initialize d_level with -1 and for src 0
    cl_kernel initd_level_kernel = clCreateKernel(program, "initd_level_kernel", &status);
    if(status!=CL_SUCCESS)
    {
      cout<<"Failed to create initd_level_kernel"<<endl;
      exit(0);
    }
    status = clSetKernelArg(initd_level_kernel , 0, sizeof(cl_mem) ,(void *)&d_level);
    status = clSetKernelArg(initd_level_kernel , 1, sizeof(int) ,(void *)&V);
    local_size = 128;
    global_size = (V + local_size -1)/ local_size * local_size;
    status  = clEnqueueNDRangeKernel(command_queue, initd_level_kernel,1,NULL, &global_size, &local_size, 0, NULL, &event);
    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;
    status = clReleaseKernel(initd_level_kernel);
    cl_kernel initIndexd_level_kernel = clCreateKernel(program,"initIndexd_level_kernel",&status);
    status = clSetKernelArg(initIndexd_level_kernel, 0, sizeof(cl_mem), (void *)&d_level);
    status = clSetKernelArg(initIndexd_level_kernel, 1, sizeof(int), (void*)&src);
    global_size = 1;
    local_size=1;
    status = clEnqueueNDRangeKernel(command_queue, initIndexd_level_kernel, 1,NULL, &global_size, &local_size, 0, NULL, &event);
    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;
    status = clReleaseKernel(initIndexd_level_kernel);
    finished = 1;
    hops_from_source = 0;
    while(finished)
    {
      finished = 0;

      status  = clEnqueueWriteBuffer(command_queue,d_finished, CL_TRUE, 0,sizeof(int), &finished,0,0,NULL);
      status  = clEnqueueWriteBuffer(command_queue,d_hops_from_source, CL_TRUE, 0,sizeof(int), &hops_from_source,0,0,NULL);
      //Forward Pass
      cl_kernel fwd_pass_kernel = clCreateKernel(program, "fwd_pass_kernel", &status);
      if(status != CL_SUCCESS)
      {
        cout<<"Failed to create fwd_pass_kernel"<<endl;
      }
      status = clSetKernelArg(fwd_pass_kernel, 0, sizeof(int), (void *)&V);
      status = clSetKernelArg(fwd_pass_kernel, 1, sizeof(cl_mem), (void *)&d_meta);
      status = clSetKernelArg(fwd_pass_kernel, 2, sizeof(cl_mem), (void *)&d_data);
      status = clSetKernelArg(fwd_pass_kernel, 3, sizeof(cl_mem), (void *)&d_weight);
      status = clSetKernelArg(fwd_pass_kernel, 4, sizeof(cl_mem), (void *)&d_level);
      status = clSetKernelArg(fwd_pass_kernel, 5, sizeof(cl_mem), (void *)&d_hops_from_source);
      status = clSetKernelArg(fwd_pass_kernel, 6, sizeof(cl_mem), (void *)&d_finished);
      status = clSetKernelArg(fwd_pass_kernel, 7, sizeof(cl_mem), (void*)&d_sigma);
      status = clSetKernelArg(fwd_pass_kernel, 8, sizeof(cl_mem), (void*)&d_delta);
      status = clSetKernelArg(fwd_pass_kernel, 9, sizeof(cl_mem), (void*)&d_BC);
      local_size = 128;
      global_size = (V + local_size -1)/ local_size * local_size;
      status = clEnqueueNDRangeKernel(command_queue, fwd_pass_kernel, 1, NULL, &global_size, &local_size, 0, NULL, &event);
      clWaitForEvents(1,&event);
      status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
      status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
      kernelTime = (double)(end-start)/convertToMS;
      totalTime = totalTime+ kernelTime;
      status = clReleaseKernel(fwd_pass_kernel);
      status=clEnqueueReadBuffer(command_queue, d_finished , CL_TRUE, 0, sizeof(int), &finished, 0, NULL, NULL );

      hops_from_source = hops_from_source + 1;
    }

    hops_from_source = hops_from_source-1;

    //Backward Pass
    while(hops_from_source > 1)
    {

      status  = clEnqueueWriteBuffer(command_queue,d_hops_from_source, CL_TRUE, 0,sizeof(int), &hops_from_source,0,0,NULL);
      //KERNEL Launch
      cl_kernel back_pass_kernel = clCreateKernel(program,"back_pass_kernel", &status);
      if(status != CL_SUCCESS)
      {
        cout<<"Failed to create back_pass_kernel"<<endl;
      }
      status = clSetKernelArg(back_pass_kernel, 0, sizeof(int), (void *)&V);
      status = clSetKernelArg(back_pass_kernel, 1, sizeof(cl_mem), (void *)&d_meta);
      status = clSetKernelArg(back_pass_kernel, 2, sizeof(cl_mem), (void *)&d_data);
      status = clSetKernelArg(back_pass_kernel, 3, sizeof(cl_mem), (void *)&d_weight);
      status = clSetKernelArg(back_pass_kernel, 4, sizeof(cl_mem), (void *)&d_level);
      status = clSetKernelArg(back_pass_kernel, 5, sizeof(cl_mem), (void *)&d_hops_from_source);
      status = clSetKernelArg(back_pass_kernel, 6, sizeof(cl_mem), (void *)&d_finished);
      status = clSetKernelArg(back_pass_kernel, 7, sizeof(cl_mem), (void*)&d_sigma);
      status = clSetKernelArg(back_pass_kernel, 8, sizeof(cl_mem), (void*)&d_delta);
      status = clSetKernelArg(back_pass_kernel, 9, sizeof(cl_mem), (void*)&d_BC);
      local_size = 128;
      global_size = (V + local_size -1)/ local_size * local_size;
      status = clEnqueueNDRangeKernel(command_queue, back_pass_kernel, 1, NULL, &global_size, &local_size, 0, NULL, &event);
      clWaitForEvents(1,&event);
      status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
      status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
      kernelTime = (double)(end-start)/convertToMS;
      totalTime = totalTime+ kernelTime;
      status = clReleaseKernel(back_pass_kernel);

      hops_from_source--;

    }
    status = clReleaseMemObject(d_level);
    status = clReleaseMemObject(d_finished);
    status = clReleaseMemObject(d_hops_from_source);

  }

  //Free up!! all propVars in this BLOCK!
  status = clReleaseMemObject(d_delta);
  free(h_delta);
  status = clReleaseMemObject(d_sigma);
  free(h_sigma);

  //TIMER STOP
  printf("Total Kernel time = %0.2lf ms.\n ", totalTime);

  clEnqueueReadBuffer(command_queue, d_BC , CL_TRUE, 0, sizeof(double)*V, BC, 0, NULL, NULL );
  //Release openCL objects
  printf("Started releasing Objects\n");
  status = clReleaseMemObject(d_meta);
  status = clReleaseMemObject(d_data);
  status = clReleaseMemObject(d_src);
  status = clReleaseMemObject(d_weight);
  status = clReleaseMemObject(d_rev_meta);
  status = clReleaseMemObject(d_modified_next);
  status = clReleaseProgram(program);
  status = clFlush(command_queue);
  status = clFinish(command_queue);
  status = clReleaseCommandQueue(command_queue);
  status = clReleaseContext(context);
  free(h_meta);
  free(h_data);
  free(h_src);
  free(h_weight);
  free(h_rev_meta);
} //end FUN

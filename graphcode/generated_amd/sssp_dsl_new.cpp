#include "sssp_dsl_new.h"

void Compute_SSSP_new(graph& g,int * dist,int * weight,int src
)
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
  cl_mem d_dist = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);

  cl_mem d_weight = clCreateBuffer(context,CL_MEM_READ_WRITE,(E)*sizeof(int),NULL, &status);


  //BEGIN DSL PARSING 
  int *h_modified = (int *)malloc(V*sizeof(int));
  cl_mem d_modified = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);


  //Reading kernel file
  FILE* kernelfp = fopen("sssp_dsl_new.cl", "rb"); 
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
  // Creating initdist_kernel  Kernel
  cl_kernel initdist_kernel = clCreateKernel(program, "initdist_kernel", &status);

  // Initialization for dist variable
  int distValue = (int)INT_MAX; 
  status = clSetKernelArg(initdist_kernel, 0 , sizeof(cl_mem), (void *)& d_dist);
  status = clSetKernelArg(initdist_kernel, 1, sizeof(int) , (void*)& distValue);
  status = clSetKernelArg(initdist_kernel, 2, sizeof(int), (void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initdist_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initdist_kernel);

  // Creating initmodified_kernel  Kernel
  cl_kernel initmodified_kernel = clCreateKernel(program, "initmodified_kernel", &status);

  // Initialization for modified variable
  int modifiedValue = (int)false; 
  status = clSetKernelArg(initmodified_kernel, 0 , sizeof(cl_mem), (void *)& d_modified);
  status = clSetKernelArg(initmodified_kernel, 1, sizeof(int) , (void*)& modifiedValue);
  status = clSetKernelArg(initmodified_kernel, 2, sizeof(int), (void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initmodified_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initmodified_kernel);

  cl_kernel initIndexmodified_kernel = clCreateKernel(program, "initIndexmodified_kernel", &status);
  //Indexmodified src initialization
  int initmodifiedvalsrc = true;
  status = clSetKernelArg(initIndexmodified_kernel , 0,sizeof(cl_mem) ,(void *)&d_modified);
  status = clSetKernelArg(initIndexmodified_kernel, 1,sizeof(int), (void*)&src);
  status = clSetKernelArg(initIndexmodified_kernel, 2, sizeof(int),(void *)&initmodifiedvalsrc);
  global_size = local_size = 1;
  status  = clEnqueueNDRangeKernel(command_queue, initIndexmodified_kernel,1,NULL, &global_size, &local_size, 0, NULL, &event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(initIndexmodified_kernel);

  cl_kernel initIndexdist_kernel = clCreateKernel(program, "initIndexdist_kernel", &status);
  //Indexdist src initialization
  int initdistvalsrc = 0;
  status = clSetKernelArg(initIndexdist_kernel , 0,sizeof(cl_mem) ,(void *)&d_dist);
  status = clSetKernelArg(initIndexdist_kernel, 1,sizeof(int), (void*)&src);
  status = clSetKernelArg(initIndexdist_kernel, 2, sizeof(int),(void *)&initdistvalsrc);
  global_size = local_size = 1;
  status  = clEnqueueNDRangeKernel(command_queue, initIndexdist_kernel,1,NULL, &global_size, &local_size, 0, NULL, &event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(initIndexdist_kernel);

  int *h_finished ;
  cl_mem d_finished= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int finished = false; 

  // Start of fixed point
  // creating %s kernel
  cl_kernel initd_modified_next = clCreateKernel(program, "initd_modified_next_kernel", &status);
  status = clSetKernelArg(initd_modified_next, 0,sizeof(cl_mem),(void*)&d_modified_next);
  int d_modified_nextValue = false;
  status = clSetKernelArg(initd_modified_next, 1,sizeof(int) ,(void*)&d_modified_nextValue);
  status = clSetKernelArg(initd_modified_next, 2 , sizeof(int),(void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initd_modified_next , 1, NULL , &global_size , &local_size ,0, NULL, &event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initd_modified_next);

  int k = 0;
  finished = false;
  while(!(finished)){
    finished = true;
     status  = clEnqueueWriteBuffer(command_queue,d_finished, CL_TRUE, 0,sizeof(int), &finished,0,0,NULL);


    //ForAll started here

    cl_kernel Compute_SSSP_new = clCreateKernel(program, "Compute_SSSP_new_kernel", &status);
    if(status != CL_SUCCESS){
      printf("Failed to create Compute_SSSP_new kernel.\n");
      return ;
    }
    status = clSetKernelArg(Compute_SSSP_new,0, sizeof(int),(void *)&V);
    status = clSetKernelArg(Compute_SSSP_new,1, sizeof(int),(void *)&E);
    status = clSetKernelArg(Compute_SSSP_new,2, sizeof(cl_mem),(void *)&d_meta);
    status = clSetKernelArg(Compute_SSSP_new,3, sizeof(cl_mem),(void *)&d_data);
    status = clSetKernelArg(Compute_SSSP_new,4, sizeof(cl_mem),(void *)&d_src);
    status = clSetKernelArg(Compute_SSSP_new,5, sizeof(cl_mem),(void *)&d_weight);
    status = clSetKernelArg(Compute_SSSP_new,6, sizeof(cl_mem),(void *)&d_rev_meta);
    status = clSetKernelArg(Compute_SSSP_new,7, sizeof(cl_mem),(void *)&d_finished);
    status = clSetKernelArg(Compute_SSSP_new,8, sizeof(cl_mem),(void *)&d_modified_next);
    status = clSetKernelArg(Compute_SSSP_new, 9, sizeof(cl_mem), (void *)&d_modified);
    status = clSetKernelArg(Compute_SSSP_new, 10, sizeof(cl_mem), (void *)&d_weight);
    status = clSetKernelArg(Compute_SSSP_new, 11, sizeof(cl_mem), (void *)&d_dist);

    local_size = 128;
    global_size = (V + local_size -1)/ local_size * local_size;

    status = clEnqueueNDRangeKernel(command_queue,Compute_SSSP_new, 1,NULL, &global_size, &local_size , 0,NULL,&event);
    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;

    status = clReleaseKernel(Compute_SSSP_new);

    status = clEnqueueCopyBuffer(command_queue, d_modified_next ,d_modified, 0,0, V*sizeof(int),0,NULL, NULL);
    // creating %s kernel
    cl_kernel initd_modified_next = clCreateKernel(program, "initd_modified_next_kernel", &status);
    status = clSetKernelArg(initd_modified_next, 0,sizeof(cl_mem),(void*)&d_modified_next);
    int d_modified_nextValue = false;
    status = clSetKernelArg(initd_modified_next, 1,sizeof(int) ,(void*)&d_modified_nextValue);
    status = clSetKernelArg(initd_modified_next, 2 , sizeof(int),(void*)&V);
    local_size = 128;
    global_size = (V + local_size -1)/ local_size * local_size;
    status = clEnqueueNDRangeKernel(command_queue, initd_modified_next , 1,NULL, &global_size , &local_size ,0, NULL, &event);
    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;
    status = clReleaseKernel(initd_modified_next);
    status =clEnqueueReadBuffer(command_queue, d_finished , CL_TRUE, 0, sizeof(int), &finished, 0, NULL, NULL );

    k++;
  } // End of While

  //Unmapping the mapped memory
  clEnqueueUnmapMemObject(command_queue, d_finished, h_finished, 0, NULL, NULL);
  // END FIXED POINT


  //Free up!! all propVars in this BLOCK!
  status = clReleaseMemObject(d_modified);
  free(h_modified);

  //TIMER STOP
  printf("Total Kernel time = %0.2lf ms.\n ", totalTime);

  clEnqueueReadBuffer(command_queue, d_dist , CL_TRUE, 0, sizeof(int)*V, dist, 0, NULL, NULL );
  clEnqueueReadBuffer(command_queue, d_weight , CL_TRUE, 0, sizeof(int)*E, weight, 0, NULL, NULL );
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

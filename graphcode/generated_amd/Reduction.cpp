#include "Reduction.h"

void reductionExample(graph& g)
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

  //BEGIN DSL PARSING 
  int *h_accum ;
  cl_mem d_accum= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int accum = 0; 
  int *h_A = (int *)malloc(V*sizeof(int));
  cl_mem d_A = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);


  //Reading kernel file
  FILE* kernelfp = fopen("Reduction.cl", "rb"); 
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
  // Creating initA_kernel  Kernel
  cl_kernel initA_kernel = clCreateKernel(program, "initA_kernel", &status);

  // Initialization for A variable
  int AValue = (int)1; 
  status = clSetKernelArg(initA_kernel, 0 , sizeof(cl_mem), (void *)& d_A);
  status = clSetKernelArg(initA_kernel, 1, sizeof(int) , (void*)& AValue);
  status = clSetKernelArg(initA_kernel, 2, sizeof(int), (void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initA_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initA_kernel);


  //ForAll started here

  status = clEnqueueWriteBuffer(command_queue, d_accum, CL_TRUE, 0, sizeof(int),&accum , 0, NULL, NULL);

  cl_kernel reductionExample = clCreateKernel(program, "reductionExample_kernel", &status);
  if(status != CL_SUCCESS){
    printf("Failed to create reductionExample kernel.\n");
    return ;
  }
  status = clSetKernelArg(reductionExample,0, sizeof(int),(void *)&V);
  status = clSetKernelArg(reductionExample,1, sizeof(int),(void *)&E);
  status = clSetKernelArg(reductionExample,2, sizeof(cl_mem),(void *)&d_meta);
  status = clSetKernelArg(reductionExample,3, sizeof(cl_mem),(void *)&d_data);
  status = clSetKernelArg(reductionExample,4, sizeof(cl_mem),(void *)&d_src);
  status = clSetKernelArg(reductionExample,5, sizeof(cl_mem),(void *)&d_weight);
  status = clSetKernelArg(reductionExample,6, sizeof(cl_mem),(void *)&d_rev_meta);
  status = clSetKernelArg(reductionExample, 7, sizeof(cl_mem), (void *)&d_accum);
  status = clSetKernelArg(reductionExample, 8, sizeof(cl_mem), (void *)&d_A);

  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;

  status = clEnqueueNDRangeKernel(command_queue,reductionExample, 1,NULL, &global_size, &local_size , 0,NULL,&event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(reductionExample);

  status = clEnqueueReadBuffer(command_queue, d_accum, CL_TRUE, 0, sizeof(int),&accum , 0, NULL, NULL);


  //Free up!! all propVars in this BLOCK!
  status = clReleaseMemObject(d_A);
  free(h_A);

  //TIMER STOP
  printf("Total Kernel time = %0.2lf ms.\n ", totalTime);

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

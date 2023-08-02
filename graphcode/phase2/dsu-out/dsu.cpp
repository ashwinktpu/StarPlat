#include "dsu.h"

void DSU_test(graph& g)
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

  int* h_meta;
  int* h_data;
  int* h_weight;
  int* h_rev_meta;
  int* h_src;
  int* h_modified_next;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    h_meta[i] = g.indexofNodes[i];
    h_rev_meta[i] = g.rev_indexofNodes[i];
  }

  for(int i=0; i< E; i++) {
    h_data[i] = g.edgeList[i];
    h_src[i] = g.srcList[i];
    h_weight[i] = edgeLen[i];
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
  int *h_mydsu = (int*) malloc((V+1) *sizeof(int));
  cl_mem d_mydsu = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int*),NULL, &status);


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
  free(h_weight);
  free(h_rev_meta);
  free(h_src);
  free(h_modified_next);
} //end FUN

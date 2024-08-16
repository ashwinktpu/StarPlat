#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"
using namespace sycl;



void bp(graph& g , int k)
{   
    //std::cout<<INT_MAX<<" "<<LONG_MAX;
    queue Q(cpu_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<<std::endl;
	int V = g.num_nodes();
	int E = g.num_edges();
    std::cout<<"No. of Nodes"<<V<<std::endl;
    std::cout<<"No. of edges"<<E<<std::endl;
    
    // Declaring all the host variables
	int *h_meta; // storing the offset list
	int *h_data; // storing the csr list
	double *h_weight; // storing the weights of edges
    double *h_cur_prob;//storing the current probability of node
    double *h_prior_prob;//storing the previous probability of nodes
    int *edgeLen = g.getEdgeLen();
    
    // Initializing all host variables
    h_meta = (int *)malloc((V + 1) * sizeof(int));
	h_data = (int *)malloc(E * sizeof(int));
	h_weight = (double *)malloc(E * sizeof(double));
    h_cur_prob=(double *)malloc((V) * sizeof(double));
    h_prior_prob=(double *)malloc((V) * sizeof(double));
    
    
    // Storing the graph values in the respective host variables
	for (int i = 0; i <= V; i++)
	{
		h_meta[i] = g.indexofNodes[i];
        //std::cout<<h_meta[i]<<" ";
        if(i!=V)
        {   
            h_cur_prob[i]=0.01;
            h_prior_prob[i]=0.01;
        } 
	}
    for (int i = 0; i < E; i++)
    {   h_weight[i]=0.01;
        h_data[i] = g.edgeList[i];
    }
    
    //std::cout<<"AD"<<std::endl;
    //Initializing device variables
   	int *d_meta; 
	int *d_data; 
    double *d_weight; 
    double *d_cur_prob;
    double *d_prior_prob;
    
    
    // Initializing device variables
    d_meta = malloc_device<int>((V + 1), Q);
	d_data = malloc_device<int>((E), Q);
    d_weight = malloc_device<double>((E), Q);
    d_cur_prob = malloc_device<double>((V), Q);
    d_prior_prob = malloc_device<double>((V), Q);
    
    // Trasferring values from CPU to GPU memory
    //direction  of copying <--
	Q.submit([&](handler &h)
			{ h.memcpy(d_meta, h_meta, sizeof(int) * (V + 1)); })
		.wait();
		
	Q.submit([&](handler &h)
			{ h.memcpy(d_data, h_data, sizeof(int) * (E)); })
		.wait();
	Q.submit([&](handler &h)
			{ h.memcpy(d_weight, h_weight, sizeof(double) * (E)); })
		.wait();
    Q.submit([&](handler &h)
			{ h.memcpy(d_cur_prob, h_cur_prob, sizeof(double) * (V)); })
		.wait();
    Q.submit([&](handler &h)
			{ h.memcpy(d_prior_prob, h_prior_prob, sizeof(double) * (V)); })
		.wait();
    
    
    int NUM_THREADS = 1048576;
    int itr=0;
    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();
    while(itr<100)
    {
        Q.submit([&](handler &h)
                             { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                  {
                                      for (; v < V; v += NUM_THREADS)
                                      { 
                                          double new_prob=0;
                                          for(int i=d_meta[v];i<d_meta[v+1];i++)
                                          {
                                             new_prob+=double(d_weight[i])*d_prior_prob[d_data[i]];
                                          }
                                          d_cur_prob[v]=new_prob;   
                                      }
                               });      
                 
                             })
                            .wait();

         Q.submit([&](handler &h)
                             { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                  {
                                      for (; v < V; v += NUM_THREADS)
                                      { 
                                          d_prior_prob[v]=d_cur_prob[v];   
                                      }
                               }); 
                             })
                            .wait(); 
        		
         
        Q.submit([&](handler &h)
			{ h.memcpy(h_prior_prob, d_prior_prob, sizeof(double) * (V)); })
		.wait();
        
       itr++;   
    }
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() << "[ms]" << "   "<<std::endl;
       
      Q.submit([&](handler &h)
			{ h.memcpy(h_prior_prob, d_cur_prob, sizeof(double) * (V)); })
		.wait();
      
//       for(int i=0;i<V;i++)
//          std::cout<<h_prior_prob[i]<<" ";
//       std::cout<<std::endl;
      
      double minibelief=h_prior_prob[0],maxibelief=h_prior_prob[0];
      for(int i=1;i<V;i++)
      {
          minibelief=min(minibelief,h_prior_prob[i]);
          maxibelief=max(maxibelief,h_prior_prob[i]);
      }
      std::cout<<"MINIMUM_BELIEF: "<< minibelief << "  MAXIMUM_BELIEF: "<<maxibelief<<std::endl;
      int count=0;
      double midvalue=(maxibelief+minibelief)/2;
      std::cout<<"MIDVALUE: "<<midvalue<<std::endl; 
      for(int i=0;i<V;i++)
      { 
          if(midvalue>=h_prior_prob[i])
           count++;
      
      }
      std::cout<<"No of nodes having belief value less than midvalue "<<count<<std::endl;      
      
      return ;
}



int main(int argc, char** argv)
{
	char* inp = argv[1];
	std::cout << "Taking input from: " << inp << std::endl;
    int k=50;
	graph g(inp);
	g.parseGraph();
	bp(g,k);
	return 0;
}
#include "bc_parallel.h" 
void Compute_SSSP(graph g,int src,int* weight,vector<int> seeds){
	//vector <int> bc;
	//int* weight = g.getEdgeLen();
    int my_rank,np,part_size,startv,endv;
    int max_degree=g.max_degree();
    struct timeval start, end;
    long seconds,micros;
        
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Request request;
    
    part_size = g.num_nodes()/np;
    startv = my_rank*part_size;
    endv = startv + (part_size-1);

	float* bc = new float[g.num_nodes()];
	for(int t=0;t<g.num_nodes();t++)
	{
		bc[t] = 0;
	}

    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&start, NULL);
	for (int i=0;i<1;i++)
	{
		cout<<seeds[i] <<endl;
		stack <int> S;
		vector <vector<int>> p (g.num_nodes());
		cout <<"Test vector size is "<<p.size() << endl;
		//vector <int> p;
		float* sigma = new float[g.num_nodes()];
		int* d = new int[g.num_nodes()];
		float* delta = new float[g.num_nodes()];
		queue <int> Q;
		Q.push(seeds[i]);
        bool* active=new bool[g.num_nodes()];
        
        for (int t = 0; t < g.num_nodes(); t ++) 
        {
            active[t] = false;
        }
        active[seeds[i]]=1;
		for(int t=0;t<g.num_nodes();t++)
		{
			sigma[t] = 0;
			d[t] = -1;	
			delta[t] = 0;
		}
		sigma[seeds[i]] = 1;
		d[seeds[i]] = 0;	
		while(is_finished(startv,endv,active))
		{
				//Q.erase(it);
	    		//int v = Q.front();
                vector<int>active_next;
                for (int v = startv; v <= endv; v ++) 
                {   
                    int *count = new int[np];
                    int *pos = new int[np];
                    float *send_data = new float[np*4*max_degree];
                    float *recv_data = new float[np*4*max_degree];
                    int dest_pro;
                    for (int tem=0; tem<np; tem++)
                    {
                        count[tem]=0;
                        pos[tem]=1;
                        //send_data[tem*4*max_degree]=0;
                        //send_data[
                    }
                    for (int tem=0; tem<np*4*max_degree; tem++)
                    {
                        send_data[tem]=0;
                        recv_data[tem]=0;
                    }
                    
                    if(active[v] == true)
                    {   
                        active[v] = false;
                        cout <<"Popped element from queue "<< v << endl;
                        //Q.pop();
                        //S.push(v);
                        for (int j = g.indexofNodes[v]; j<g.indexofNodes[v+1]; j ++)
                        {
                            int w = g.edgeList[j]; 
                            if(w >= startv && w <= endv)
                            {
                                if (d[w] < 0 /*|| d[w] > d[v]+1*/)
                                {
                                    //Q.push(w);
                                    //active[w] = true;
                                    active_next.push_back(w);
                                    d[w] = d[v] + 1;
                                }
                                if (d[w] == d[v]+1)
                                {
                                    sigma[w] = sigma[w] + sigma [v];
                                    p[w].push_back(v);
                                        
                                }
                            }
                            else
                            {
                                dest_pro = w / part_size;
                                count[dest_pro]=count[dest_pro]+1;
                                int p=pos[dest_pro];
                                send_data[dest_pro*4*max_degree] = count[dest_pro];
                                send_data[dest_pro*4*max_degree+p] =  d[v];
                                send_data[dest_pro*4*max_degree+(p+1)] = w;
                                //send_data[dest_pro*4*max_degree+(p+2)] = dist[v] ;
                                send_data[dest_pro*4*max_degree+(p+2)] = sigma[v];
                                send_data[dest_pro*4*max_degree+(p+3)] = v;
                                pos[dest_pro] = pos[dest_pro]+4;
                                printf("The data sent is %d,%d,%f,%d\n",d[v],w,sigma[v],v);
                            }  
                        }
                    }
                    //MPI_Alltoall(send_data,4*max_degree,MPI_INT,recv_data,4*max_degree,MPI_INT,MPI_COMM_WORLD);
                    MPI_Alltoall(send_data,4*max_degree*4,MPI_BYTE,recv_data,4*max_degree*4,MPI_BYTE,MPI_COMM_WORLD);
                    //Receive data
                    for(int t=0;t<np;t++)
                    {
                        if(t!=my_rank)
                        {
                            int amount = recv_data[t*4*max_degree];
                            if(amount>0)
                            {
                                for(int k=1;k<=(amount*4);k+=4)
                                {
                                    int d_v = recv_data[(t*4*max_degree)+k];
                                    int w = recv_data[(t*4*max_degree)+k+1];
                                    float sigma_v = (float) recv_data[(t*4*max_degree)+k+2];
                                    int v = recv_data[(t*4*max_degree)+k+3];
                                    printf("The data received is %d,%d,%f,%d\n",d_v,w,sigma_v,v);
                                    //src = recv_data[(t*4*max_degree)+k+2],
                                    if (d[w] < 0 /*|| d[w] > d_v +1*/)
                                    {
                                        //Q.push(w);
                                        //active[w] = true;
                                        active_next.push_back(w);
                                        d[w] = d_v + 1;
                                    }
                                    if (d[w] == d_v+1)
                                    {
                                        sigma[w] = sigma[w] + sigma_v;
                                        p[w].push_back(v);
                                            
                                    }
                                }
                            }
                        }
                    }
                    
                }
                for(int i=0;i<active_next.size();i++)
                        active[active_next[i]] = true;
                active_next.clear();
		}
        for (int v=startv;v<=endv;v++)
            printf("dist[%d] from src %d is %d\n",v,seeds[i],d[v]);
        for (int v=startv;v<=endv;v++)
            printf("sigma[%d] = %f\n",v,sigma[v]);
        int max_dist = get_maxDistance(startv,endv,d);
        printf("The max levels %d\n",max_dist);
		/*
		while (!S.empty())
		{
			int w = S.top();
			cout << "Popped element from stack is "<<w << endl;
	    		S.pop();
	    		//for (int i=0;i<p.size();i++)
	    		//{
	    			for (int j=0;j<p[w].size();j++)
	    			{
	    				int v = p[w][j];
	    				delta[v] = delta[v] + (sigma[v]/sigma[w]) * (1+delta[w]); 
	    			}
	    		//}
	    		if (w != seeds[i])
	    		{
	    			bc[w] = bc[w] + delta[w];
	    		}
		}
	}*/
    /*
		while (!S.empty())
		{
				int v = S.top();
				cout << "Popped element from stack is "<<v << endl;
					S.pop();
					//for (int i=0;i<p.size();i++)
					//{
						for (int j = g.indexofNodes[v]; j<g.indexofNodes[v+1]; j++)
						{
							int w = g.edgeList[j]; 
							if(d[w] == d[v]+1)
							{
								delta[v] = delta[v] + (sigma[v]/sigma[w])*(1+delta[w]);
							}
						}
					if (v != seeds[i])
					{
						bc[v] = bc[v] + delta[v];
					}
			
		}
	}
    */
    MPI_Barrier(MPI_COMM_WORLD);
    int k = max_dist;
    while(k>0) //(for(int k=max_dist; k>0;k--)
    {
        vector <int> list;
        for(int v=startv; v<=endv; v++)
        {   
            int *count = new int[np];
            int *pos = new int[np];
            float *send_data = new float[np*4*max_degree];
            float *recv_data = new float[np*4*max_degree];
            int dest_pro;
            for (int tem=0; tem<np; tem++)
            {
                count[tem]=0;
                pos[tem]=1;
                //send_data[tem*4*max_degree]=0;
                //send_data[
            }
            for (int tem=0; tem<np*4*max_degree; tem++)
            {
                send_data[tem]=0;
                recv_data[tem]=0;
            }
            if(d[v] == k)
            {
                list.push_back(v);
                printf("The value of k is %d\n",k);
                printf("The element popped from stack %d\n",v);
                for (int j = g.indexofNodes[v]; j<g.indexofNodes[v+1]; j ++)
                    {
                        int w = g.edgeList[j];  
                        if(w >= startv && w <= endv)
                        {
                            if(d[w] == d[v]+1)
							{
								delta[v] = delta[v] + (sigma[v]/sigma[w])*(1+delta[w]);
							}
                        }
                    }
            }
            //printf("The value of k is %d\n",k);
            if(d[v] == (k+1))
            {   
                printf("The value of k is %d\n",k);
                printf("The element with next level %d\n",v);
                for (int j=0;j<p[v].size();j++)
	    		{
	    			int w = p[v][j];
                    printf("The parent of element with next level %d\n",w);
                    if(!(w >= startv && w <= endv))
                    {
                        printf("[%d]The parent of element with next level is remote %d\n",my_rank,w);
                        dest_pro = w / part_size;
                        count[dest_pro]=count[dest_pro]+1;
                        int p=pos[dest_pro];
                        send_data[dest_pro*4*max_degree] = count[dest_pro];
                        send_data[dest_pro*4*max_degree+p] =  v;
                        send_data[dest_pro*4*max_degree+(p+1)] = w;
                        //send_data[dest_pro*4*max_degree+(p+2)] = dist[v] ;
                        send_data[dest_pro*4*max_degree+(p+2)] = sigma[v];
                        send_data[dest_pro*4*max_degree+(p+3)] = delta[v];
                        pos[dest_pro] = pos[dest_pro]+4;
                        printf("The data sent is %d,%d,%f,%f\n",v,w,sigma[v],delta[v]);
                    }
                }
            }
            //MPI_Alltoall(send_data,4*max_degree,MPI_INT,recv_data,4*max_degree,MPI_INT,MPI_COMM_WORLD);
            MPI_Alltoall(send_data,4*max_degree*4,MPI_BYTE,recv_data,4*max_degree*4,MPI_BYTE,MPI_COMM_WORLD);
            for(int t=0;t<np;t++)
                    {
                        if(t!=my_rank)
                        {
                            int amount = recv_data[t*4*max_degree];
                            if(amount>0)
                            {
                                for(int k=1;k<=(amount*4);k+=4)
                                {
                                    int w = recv_data[(t*4*max_degree)+k];
                                    int v = recv_data[(t*4*max_degree)+k+1];
                                    float sigma_w = (float)recv_data[(t*4*max_degree)+k+2];
                                    float delta_w = (float)recv_data[(t*4*max_degree)+k+3];
                                    printf("The data recived is %d,%d,%f,%f\n",w,v,sigma_w,delta_w);
                                    //if (std::find(list.begin(), list.end(), v) == list.end()) {
                                        // someName not in name, add it
                                        //list.push_back(v);
                                    //}
                                    delta[v] = delta[v] + (sigma[v]/sigma_w)*(1+delta_w);
                                }
                            }
                        }
                    }
                    
              MPI_Barrier(MPI_COMM_WORLD);      
        }
        
        for(int t = 0;t<list.size();t++)
        {
        //if (v != seeds[i])
            printf("List item (%d) is %d\n",t,list[t]);
            bc[list[t]] = bc[list[t]] + delta[list[t]];
        }
        k--;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    for(int t=startv;t<=endv;t++)
		{
			cout << "delta["<<t<<"] = "<<delta[t]<<endl;
		}
	
	
    }
    for(int t=startv;t<=endv;t++)
		{
			cout << "BC["<<t<<"] = "<<bc[t]<<endl;
		}
    MPI_Finalize();
}

int main()
{ 

 	struct timeval start, end;
   //graph G("soc-Slashdot0811.txt");
   graph G("sample_graph.txt");
   gettimeofday(&start, NULL);
   G.parseGraph();
   gettimeofday(&end, NULL);
   long seconds = (end.tv_sec - start.tv_sec);
   long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
   printf("The graph loading time = %ld micro secs.\n",micros);
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=0;
  vector <int> seeds;
  for (int i=0;i<10;i++)
	seeds.push_back(i);
	  //seeds.push_back(rand()%G.num_nodes());
  //for (int i=0;i<5;i++)
  //	cout<<seeds[i]<<endl;
  /*
  printf("The number of nodes is %d\n",G.num_nodes());
  printf("The number of edges is %d\n",G.num_edges());
  printf("The maximum degree of the graph is %d\n",G.max_degree());
  for(int i=0;i<G.num_nodes()+1;i++)
  {
  	std::cout<<G.indexofNodes[i]<< " ";
  }
  printf("\nEdgedetails\n");
  for(int i=0;i<G.num_edges();i++)
  {
  	std::cout<<G.edgeList[i]<< " ";
  }
  printf("\nNeighbordetails\n");
  for(int i=0;i<G.num_nodes();i++)
  {
  printf("The neighbors of node %d is ",i);
  for (int j = G.indexofNodes[i]; j<G.indexofNodes[i+1]; j ++)
            {
                int nbr=G.edgeList[j];
                printf("%d : %d, ",nbr,G.edgeLen[j]);
              }
              printf("\n");
  }
   */           
  //for(int i=1;i<G.num_nodes()+1;i++)
  //{
  //	std::cout<<G.edge[i]<< " ";
  //}
   Compute_SSSP(G,src,edgeLen,seeds);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  return 0;
}

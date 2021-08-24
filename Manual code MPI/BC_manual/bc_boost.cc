#include "bc_boost.h" 
void Compute_SSSP(graph g,int src,vector<int> seeds){
	//vector <int> bc;
	//int* weight = g.getEdgeLen();
    int my_rank,np,part_size,startv,endv;
    struct timeval start, end, start1, end1;
    long seconds,micros;
        
    //MPI_Init(NULL,NULL);
    //MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    //MPI_Comm_size(MPI_COMM_WORLD, &np);
    //MPI_Request request;
    mpi::communicator world;
    my_rank = world.rank();
    np = world.size();

    gettimeofday(&start1, NULL);
    g.parseGraph();
    gettimeofday(&end1, NULL);
    seconds = (end1.tv_sec - start1.tv_sec);
    micros = ((seconds * 1000000) + end1.tv_usec) - (start1.tv_usec);
    if(my_rank == 0)
    {
        printf("The graph loading time = %ld secs.\n",seconds);
    }
    int max_degree = g.max_degree();
    int *weight = g.getEdgeLen();
    
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
	for (int i=0;i<seeds.size();i++)
	{
		//cout<<seeds[i] <<endl;
		stack <int> S;
		vector <vector<int>> p (g.num_nodes());
		//cout <<"Test vector size is "<<p.size() << endl;
		//vector <int> p;
		float* sigma = new float[g.num_nodes()];
		int* d = new int[g.num_nodes()];
		float* delta = new float[g.num_nodes()];
		queue <int> Q;
		Q.push(seeds[i]);
        //bool* active=new bool[g.num_nodes()];
        vector <int> active;
        vector<int> active_next;
        active.push_back(seeds[i]);
        
        //for (int t = 0; t < g.num_nodes(); t ++) 
        //{
          //  active[t] = false;
        //}
        //active[seeds[i]]=1;
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
                //vector < map<int,vector <float> > > send_data(np);
                //vector < map<int,vector <float> > > receive_data(np);
                vector < vector <float> > send_data(np);
                vector < vector <float> > receive_data(np);

                std::map<int,int>::iterator itr;
                int dest_pro;
                
                //for (int v = startv; v <= endv; v ++) 
                while (active.size() > 0)
                {   
                    //int *count = new int[np];
                    //int *pos = new int[np];
                    //float *send_data = new float[np*4*max_degree];
                    //float *recv_data = new float[np*4*max_degree];
                    
                    /*
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
                    */
                    //if(active[v] == true)
                    //{   
                        //active[v] = false;
                        int v = active.back();
                        active.pop_back();
                        //cout <<"Popped element from queue "<< v << endl;
                        //Q.pop();
                        //S.push(v);
                        if(v >=startv && v<= endv)
                        {
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
                                    vector <float> temp;
                                    //temp.push_back(d[v]);
                                    //temp.push_back(sigma[v]);
                                    //temp.push_back(v);
                                    //itr = send_data[dest_pro].find(w);
                                    //if (itr != send_data[dest_pro].end())
                                    //{
                                      //  itr->second = min( send_data[dest_pro][nbr], dist[v] + weight[e]);
                                    //}
                                    //else
                                    //{
                                        //temp.push_back(d[v]);
                                        //temp.push_back(w);
                                        //temp.push_back(sigma[v]);
                                        //temp.push_back(v);
                                        //send_data[dest_pro].push_back(temp);
                                        send_data[dest_pro].push_back(d[v]);
                                        send_data[dest_pro].push_back(w);
                                        send_data[dest_pro].push_back(sigma[v]);
                                        send_data[dest_pro].push_back(v);
                                        //send_data[dest_pro][w] = temp;
                                    //}
                                    
                                    /*
                                    count[dest_pro]=count[dest_pro]+1;
                                    int p=pos[dest_pro];
                                    send_data[dest_pro*4*max_degree] = count[dest_pro];
                                    send_data[dest_pro*4*max_degree+p] =  d[v];
                                    send_data[dest_pro*4*max_degree+(p+1)] = w;
                                    //send_data[dest_pro*4*max_degree+(p+2)] = dist[v] ;
                                    send_data[dest_pro*4*max_degree+(p+2)] = sigma[v];
                                    send_data[dest_pro*4*max_degree+(p+3)] = v;
                                    pos[dest_pro] = pos[dest_pro]+4;*/
                                    //printf("The data sent is %d,%d,%f,%d\n",d[v],w,sigma[v],v);
                                }  
                            }
                        }
                   // }
                }
                    //MPI_Alltoall(send_data,4*max_degree,MPI_INT,recv_data,4*max_degree,MPI_INT,MPI_COMM_WORLD);
                   // MPI_Alltoall(send_data,4*max_degree*4,MPI_BYTE,recv_data,4*max_degree*4,MPI_BYTE,MPI_COMM_WORLD);
                    all_to_all(world, send_data, receive_data);
                    
                    //Receive data
                    for(int t=0;t<np;t++)
                    {
                        if(t!=my_rank)
                        {   
                            //for (auto x : receive_data[t])
                            for (int x=0; x < receive_data[t].size();x+=4)
                            {
                                //int dist_new = x.second;
                                //int w = x.first;
                                //vector <float> temp = x.second;
                                //int d_v = temp[0]; 
                                //float sigma_v = temp[1];
                                //int v = temp[2];
                                //vector <float> temp = receive_data[t][x];
                                int d_v = receive_data[t][x]; 
                                int w = receive_data[t][x+1];
                                float sigma_v = receive_data[t][x+2];
                                int v = receive_data[t][x+3];
                                //printf("The data received is %d,%d,%f,%d\n",d_v,w,sigma_v,v);
                                if (d[w] < 0 )
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
                            /*
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
                                    //if (d[w] < 0 || d[w] > d_v +1)
                                    if (d[w] < 0 )
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
                            }*/
                        }
                    }
                    
                active.swap(active_next);
                //for(int i=0;i<active_next.size();i++)
                  //      active[active_next[i]] = true;
                active_next.clear();
                MPI_Barrier(MPI_COMM_WORLD);
        //printf("[%d] after barrier2\n",my_rank);
                send_data.clear();
                receive_data.clear();
		}
        
        for (int v=startv;v<=endv;v++)
            printf("dist[%d] from src %d is %d\n",v,seeds[i],d[v]);
        for (int v=startv;v<=endv;v++)
            printf("sigma[%d] = %f\n",v,sigma[v]);
        
        
        
       int max_dist = get_maxDistance(startv,endv,d);
       printf("The max levels %d\n",max_dist);
    MPI_Barrier(MPI_COMM_WORLD);
    int k = max_dist;
    while(k>0) //(for(int k=max_dist; k>0;k--)
    {
        vector <int> list;

       // vector<float> m;
        vector <vector <float> > send_data(np);
        vector <vector <float> > receive_data(np);
        int dest_pro;

        for(int v=startv; v<=endv; v++)
        {   

             if(d[v] == k)
            {
                list.push_back(v);
               // printf("The value of k is %d\n",k);
                //printf("The element popped from stack %d\n",v);
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
               // printf("The value of k is %d\n",k);
                //printf("The element with next level %d\n",v);
                for (int j=0;j<p[v].size();j++)
	    		{
	    			int w = p[v][j];
                   // printf("The parent of element with next level %d\n",w);
                    if(!(w >= startv && w <= endv))
                    {
                     //   printf("[%d]The parent of element with next level is remote %d\n",my_rank,w);
                        dest_pro = w / part_size;
                        vector <float> temp;
                        //temp.push_back(v);
                        //temp.push_back(w);
                        //temp.push_back(sigma[v]);
                        //temp.push_back(delta[v]);
                        send_data[dest_pro].push_back(v);
                        send_data[dest_pro].push_back(w);
                        send_data[dest_pro].push_back(sigma[v]);
                        send_data[dest_pro].push_back(delta[v]);
                       
                      //  printf("The data sent is %d,%d,%f,%f\n",v,w,sigma[v],delta[v]);
                    }
                }
            }
        }
            //MPI_Alltoall(send_data,4*max_degree,MPI_INT,recv_data,4*max_degree,MPI_INT,MPI_COMM_WORLD);
           // MPI_Alltoall(send_data,4*max_degree*4,MPI_BYTE,recv_data,4*max_degree*4,MPI_BYTE,MPI_COMM_WORLD);
            all_to_all(world, send_data,receive_data);
            for(int t=0;t<np;t++)
                    {
                        if(t!=my_rank)
                        {
                            //int amount = recv_data[t*4*max_degree];
                            //if(amount>0)
                            //{
                                for(int k=0;k<receive_data[t].size();k+=4)
                                {
                                    int w = receive_data[t][k];//recv_data[(t*4*max_degree)+k];
                                    int v = receive_data[t][k+1];//recv_data[(t*4*max_degree)+k+1];
                                    float sigma_w = receive_data[t][k+2];//(float)recv_data[(t*4*max_degree)+k+2];
                                    float delta_w = receive_data[t][k+3];//(float)recv_data[(t*4*max_degree)+k+3];
                                    //printf("The data recived is %d,%d,%f,%f\n",w,v,sigma_w,delta_w);
                                    //if (std::find(list.begin(), list.end(), v) == list.end()) {
                                        // someName not in name, add it
                                        //list.push_back(v);
                                    //}
                                    delta[v] = delta[v] + (sigma[v]/sigma_w)*(1+delta_w);
                                }
                            //}
                        }
                    }
                    
              //MPI_Barrier(MPI_COMM_WORLD);      
       // }
        
        for(int t = 0;t<list.size();t++)
        {
        //if (v != seeds[i])
            //printf("List item (%d) is %d\n",t,list[t]);
            bc[list[t]] = bc[list[t]] + delta[list[t]];
        }
        k--;
        MPI_Barrier(MPI_COMM_WORLD);
        send_data.clear();
        receive_data.clear();
    }
    
    for(int t=startv;t<=endv;t++)
		{
			cout << "delta["<<t<<"] = "<<delta[t]<<endl;
		}
	
	
    }
    gettimeofday(&end, NULL);
    seconds = (end.tv_sec - start.tv_sec);
    micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    //for(int t=startv;t<=endv;t++)
	//	{
	//		cout << "BC["<<t<<"] = "<<bc[t]<<endl;
	//	}
      vector <float> final_bc;
    gather(world, bc+my_rank*part_size,part_size,final_bc,0);
    if(my_rank==0)
    {
        //printf("The number of iterations taken %d \n",num_iter);
        printf("The iteration time = %ld micro secs.\n",micros);
        printf("The iteration time = %ld secs.\n",seconds);
        
        //Additional code to check the correctness
        char buf[1024];
        sprintf(buf,"output%d.txt",g.ori_num_nodes());
        FILE *fptr = fopen(buf, "w");
        if (fptr == NULL)
            {
                printf("Could not open output file to write output");
                return;
            }
        for(int i=0;i< g.ori_num_nodes();i++)
        {
            fprintf(fptr,"BC[%d] = %f\n",i,final_bc[i]);
        }
        fclose(fptr);
        
    }
    MPI_Finalize();
}



int main(int argc, char* argv[])
{ 

 	if(argc < 3)
   {
   	printf("Execute ./a.out input_graph_file numberOfProcesses\n");
   	exit(0);
   }
   int np = strtol(argv[2], NULL, 10);
   //printf("The number of process entered : %d\n",np);
   graph G(argv[1],np);
  int src=0;
  vector <int> seeds;
  for (int i=0;i<1;i++)
	seeds.push_back(i);
	  //seeds.push_back(rand()%G.num_nodes());

  mpi::environment env(argc, argv);
   Compute_SSSP(G,src,seeds);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  return 0;
}

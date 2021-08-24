#include "bc.h" 
void Compute_SSSP(graph g,int src,int* weight,vector<int> seeds){
	//vector <int> bc;
	//int* weight = g.getEdgeLen();
	float* bc = new float[g.num_nodes()];
		for(int t=0;t<g.num_nodes();t++)
		{
			bc[t] = 0;
		}
	for (int i=0;i<10;i++)
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
		for(int t=0;t<g.num_nodes();t++)
		{
			sigma[t] = 0;
			d[t] = -1;	
			delta[t] = 0;
		}
		sigma[seeds[i]] = 1;
		d[seeds[i]] = 0;	
		while(Q.size() > 0)
		{
			//vector<int>::iterator it = Q.begin();
			//int v = *it;
			//cout << v << endl;
	    		//Q.erase(it);
	    		int v = Q.front();
	    		cout <<"Popped element from queue "<< v << endl;
	    		Q.pop();
	    		S.push(v);
	    		for (int j = g.indexofNodes[v]; j<g.indexofNodes[v+1]; j ++)
	    		{
	    			int w = g.edgeList[j]; 
	    			if (d[w] < 0)
	    			{
	    				Q.push(w);
	    				d[w] = d[v] + 1;
	    			}
	    			if (d[w] == d[v]+1)
	    			{
	    				sigma[w] = sigma[w] + sigma [v];
	    				p[w].push_back(v);
	    					
	    			}
	    				
	    		}
		}
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
		for(int t=0;t<g.num_nodes();t++)
		{
			cout << "sigma["<<t<<"] = "<<sigma[t]<<endl;
		}
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
		for(int t=0;t<g.num_nodes();t++)
		{
			cout << "delta["<<t<<"] = "<<delta[t]<<endl;
		}
	}
	for(int t=0;t<g.num_nodes();t++)
		{
			cout << "BC["<<t<<"] = "<<bc[t]<<endl;
		}
	
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

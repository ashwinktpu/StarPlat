#include"community_detection.h"
#include"../mpi_header/graph_mpi.h"
#include <chrono>
#include <iomanip>
#include <string>

void syncadjlist(Graph& g,HashMap<int,HashMap<int,int>>& weighadjlist,std::vector<int>& diffproclist,boost::mpi::communicator world)
{
	int nprocs=world.size();
	int selfprocid=world.rank();
	
	std::vector<std::vector<int>> senddata(nprocs),recvdata(nprocs);
	for(int i=0;i<diffproclist.size();i+=3)
	{
		int procid=g.get_node_owner(diffproclist[i]);
		senddata[procid].push_back(diffproclist[i]);
		senddata[procid].push_back(diffproclist[i+1]);
		senddata[procid].push_back(diffproclist[i+2]);
	}
	
	std::vector<boost::mpi::request> recvrequests(nprocs-1),sendrequests(nprocs-1);
	
	for(int pid=0;pid<nprocs;pid++)
	{
		if(pid<selfprocid)
		{
			recvrequests[pid]=world.irecv(pid,(selfprocid*nprocs)+pid,recvdata[pid]);
		}
		else if(pid>selfprocid)
		{
			recvrequests[pid-1]=world.irecv(pid,(selfprocid*nprocs)+pid,recvdata[pid]);
		}
	}
	
	for(int pid=0;pid<nprocs;pid++)
	{
		if(pid<selfprocid)
		{
			sendrequests[pid]=world.isend(pid,(pid*nprocs)+selfprocid,senddata[pid]);
		}
		else if(pid>selfprocid)
		{
			sendrequests[pid-1]=world.isend(pid,(pid*nprocs)+selfprocid,senddata[pid]);
		}
	}
	
	boost::mpi::wait_all(recvrequests.begin(), recvrequests.end());
	
	for(int pid=0;pid<nprocs;pid++)
	{
		if(pid!=selfprocid)
		{
			for(int i=0;i<recvdata[pid].size();i+=3)
			{
				auto& vlist=weighadjlist.get(recvdata[pid][i]);
				auto& edgeweight=vlist.get(recvdata[pid][i+1]);
				edgeweight+=(recvdata[pid][i+2]);
			}
		}
	}
	
	boost::mpi::wait_all(sendrequests.begin(), sendrequests.end());
}


auto Compute_CD(Graph& g, EdgeProperty<int>& weight, boost::mpi::communicator world )
{
  HashSet<int> workingset;
  HashMap<int,HashMap<int,int>> weighadjlist;
  NodeProperty<int> community;
  NodeProperty<double> selfloops;
  NodeProperty<double> newselfloops;
  NodeProperty<double> neighweights;
  NodeProperty<double> sigin;
  NodeProperty<double> sigtot;
  NodeProperty<double> kin;
  community.attachToGraph(&g, (int)-1);
  selfloops.attachToGraph(&g, (double)0);
  newselfloops.attachToGraph(&g, (double)0);
  neighweights.attachToGraph(&g, (double)0);
  sigin.attachToGraph(&g, (double)0);
  sigtot.attachToGraph(&g, (double)0);
  kin.attachToGraph(&g, (double)0);
  double totalweight = 0;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    workingset.insert(v);

    community.setValue(v,v);
    int _t1 = 0 ;

    for (int nbr : g.getNeighbors(v)) 
    {
      nbr_leader_rank = world.rank();
      nbr = g.get_other_vertex(v, _t1);
      Edge e = g.get_edge_i(v, _t1);
      sigtot.atomicAdd(v,weight.getValue(e));
      neighweights.atomicAdd(v,weight.getValue(e));
      totalweight = ( totalweight + weight.getValue(e)) ;
      auto& vlist = weighadjlist.get(v);
      vlist.put(nbr,weight.getValue(e));

      _t1++;
    }


  }
  world.barrier();

  double totalweight_temp = totalweight;
  MPI_Allreduce(&totalweight_temp,&totalweight,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);


  double threshold = 0.00011;
  double currmodularity = 0;
  double oldmodularity = 0.0 ;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    currmodularity = ( currmodularity + ((sigin.getValue(v) / totalweight) - (sigtot.getValue(v) * sigtot.getValue(v) / (totalweight * totalweight)))) ;
  }
  world.barrier();

  double currmodularity_temp = currmodularity;
  MPI_Allreduce(&currmodularity_temp,&currmodularity,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);


  double newmodularity = currmodularity;
  do
  {
    currmodularity = newmodularity;
    do
    {
      oldmodularity = newmodularity;
      workingset.reset( );

      while (!workingset.atend( ) ){
        int v = workingset.getcurrent( );
        int currcommunity = community.getValue(v);
        int newcommunity = 0;
        HashSet<int> neighcommset;
        HashMap<int,int> neighcommkin;
        auto& vlist = weighadjlist.get(v);
        vlist.reset( );

        while (!vlist.atend( ) ){
          int nbr = vlist.getcurrentkey( );
          int temp = community.getValue(nbr);
          if (neighcommset.contains(community.getValue(nbr)) == 0 )
          {
            neighcommset.insert(community.getValue(nbr));

            neighcommkin.put(temp,0);

          }
          auto& kinval = neighcommkin.get(temp);
          kinval = kinval + vlist.getcurrentval( );
          vlist.advance( );

        }
        auto& currcommkin = neighcommkin.get(currcommunity);
        sigin.atomicAdd(currcommunity,(-1 * ((2 * currcommkin) + selfloops.getValue(v))));
        sigtot.atomicAdd(currcommunity,(-1 * (neighweights.getValue(v) + selfloops.getValue(v))));
        double maxmodularitychange = kin.getValue(currcommunity) - (sigtot.getValue(currcommunity) * (neighweights.getValue(v) + selfloops.getValue(v)) / totalweight);
        newcommunity = currcommunity;
        double modularitychange = 0.0 ;
        neighcommset.reset( );

        while (!neighcommset.atend( ) ){
          int neighcomm = neighcommset.getcurrent( );
          auto& kincomm = neighcommkin.get(neighcomm);
          modularitychange = kincomm - (sigtot.getValue(neighcomm) * (neighweights.getValue(v) + selfloops.getValue(v)) / totalweight);
          if (modularitychange > maxmodularitychange )
          {
            maxmodularitychange = modularitychange;
            newcommunity = neighcomm;
          }
          neighcommset.advance( );

        }
        auto& newcommkin = neighcommkin.get(newcommunity);
        community.setValue(v,newcommunity);
        sigin.atomicAdd(newcommunity,((2 * newcommkin) + selfloops.getValue(v)));
        sigtot.atomicAdd(newcommunity,(neighweights.getValue(v) + selfloops.getValue(v)));
        workingset.advance( );

      }
      newmodularity = 0;
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        newmodularity = ( newmodularity + ((sigin.getValue(v) / totalweight) - (sigtot.getValue(v) * sigtot.getValue(v) / (totalweight * totalweight)))) ;
      }
      world.barrier();

      double newmodularity_temp = newmodularity;
      MPI_Allreduce(&newmodularity_temp,&newmodularity,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);


    }while((newmodularity - oldmodularity) > threshold);
	
	HashMap<int,HashMap<int,int>> tempweighadjlist;
    weighadjlist.swap(tempweighadjlist);

    std::vector<int> diffproclist;
    workingset.reset( );

    while (!workingset.atend( ) ){
      int v = workingset.getcurrent( );
      int comm = community.getValue(v);
      newselfloops.atomicAdd(comm,selfloops.getValue(v));
      auto& vlist = tempweighadjlist.get(v);
      vlist.reset( );

      while (!vlist.atend( ) ){
        int nbr = vlist.getcurrentkey( );
        int nbrcomm = community.getValue(nbr);
        if (comm == nbrcomm )
        {
          newselfloops.atomicAdd(comm,vlist.getcurrentval( ));
        }
        else
        {
          //if (g.get_node_owner(comm) == world.rank( ) )
          {
            auto& newlist = weighadjlist.get(comm);
            auto& edgeweight = newlist.get(nbrcomm);
            edgeweight = edgeweight + vlist.getcurrentval( );
          }
          else
          {
            diffproclist.push_back(comm);

            diffproclist.push_back(nbrcomm);

            diffproclist.push_back(vlist.getcurrentval( ));

          }
        }
        vlist.advance( );

      }
      selfloops.setValue(v,0);
      workingset.advance( );

    }
    tempweighadjlist.clear( );

    syncadjlist(g,weighadjlist,diffproclist, world);

    HashSet<int> tempworkingset;
    workingset.swap(tempworkingset);

    tempworkingset.reset( );

    while (!tempworkingset.atend( ) ){
      int v = tempworkingset.getcurrent( );
      community.setValue(v,v);
      selfloops.setValue(v,newselfloops.getValue(v));
      newselfloops.setValue(v,0);
      sigin.setValue(v,selfloops.getValue(v));
      neighweights.setValue(v,0);
      auto& vlist = weighadjlist.get(v);
      vlist.reset( );

      while (!vlist.atend( ) ){
        int nbr = vlist.getcurrentkey( );
        neighweights.atomicAdd(v,vlist.getcurrentval( ));
        vlist.advance( );

      }
      if ((selfloops.getValue(v) == 0) && (neighweights.getValue(v) == 0) )
      {
        sigin.setValue(v,0);
        sigtot.setValue(v,0);
      }
      else
      {
        workingset.insert(v);

      }
      sigtot.setValue(v,sigin.getValue(v) + neighweights.getValue(v));
      tempworkingset.advance( );

    }
    tempworkingset.clear( );

    newmodularity = 0;
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      newmodularity = ( newmodularity + ((sigin.getValue(v) / totalweight) - (sigtot.getValue(v) * sigtot.getValue(v) / (totalweight * totalweight)))) ;
    }
    world.barrier();

    double newmodularity_temp = newmodularity;
    MPI_Allreduce(&newmodularity_temp,&newmodularity,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);


  }while((newmodularity - currmodularity) > threshold);
  
  return newmodularity;

}


int main(int argc, char *argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    //printf("program started\n"); 
    Graph graph(argv[1],world);
    world.barrier();

		if(world.rank() == 0) start = std::chrono::system_clock::now();
    double ans= Compute_CD(graph, graph.weights,world);
	std::cout<<"Final Modularity "<<ans<<"\n";
		if(world.rank() == 0) end = std::chrono::system_clock::now();
   
		if(world.rank() == 0)
		{
			std::ofstream file(argv[2]);
  		if(file.is_open())
  		{
    		file <<std::fixed<<std::setprecision(9);
			file << ans <<"\n";
  		}
  	else {printf("Error while opening output file\n");}
  
		std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::ctime(&end_time);
    file << "Execution Time: "<<elapsed_seconds.count();
		file.close();
	
		} 
          
    world.barrier();
    return 0;
}
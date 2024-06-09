#include"kcore_dir_mpi.h"
#include"../mpi_header/graph_mpi.h"
#include <chrono>
#include <iomanip>
#include <string>

auto Compute_KC(Graph& g, int k, boost::mpi::communicator world )
{
  NodeProperty<int> degree;
  NodeProperty<int> state;
  degree.attachToGraph(&g, (int)0);
  state.attachToGraph(&g, (int)0);
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    for (int nbr : g.getInNeighbors(v)) 
    {
      degree.atomicAdd(v,1);
    }


    if (degree.getValue(v) < k )
    {
      state.setValue(v,1);
    }
  }
  world.barrier();


  bool finished = false;
  do
  {
    finished = true;
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (state.getValue(v) == 1 )
      {
        for (int nbr : g.getNeighbors(v)) 
        {
          if (state.getValue(nbr) == 0 )
          {
            degree.atomicAdd(nbr,-1);
            if (degree.getValue(nbr) < k )
            {
              state.setValue(nbr,1);
              finished = ( finished && false) ;
            }
          }
        }


        state.setValue(v,2);
      }
    }
    world.barrier();

    bool finished_temp = finished;
    MPI_Allreduce(&finished_temp,&finished,1,MPI_C_BOOL,MPI_LAND,MPI_COMM_WORLD);


  }
  while(!finished);int ans = 0;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (state.getValue(v) == 0 )
    {
      ans = ( ans + 1) ;
    }
  }
  world.barrier();

  int ans_temp = ans;
  MPI_Allreduce(&ans_temp,&ans,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);


  return ans;

}

int main(int argc, char *argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    Graph graph(argv[1],world);
    world.barrier();
	
	int k=std::stoi(argv[2]);

	if(world.rank() == 0)
		start = std::chrono::system_clock::now();
    
	int ans= Compute_KC(graph, k,world);
	
	if(world.rank() == 0) 
		end = std::chrono::system_clock::now();
   
	if(world.rank() == 0)
	{
		std::cout<<"Num process = "<<world.size()<<"\n";
		std::cout<<"For K = "<<k<<"\n";
		std::cout<<"K Core Count = "<<ans<<"\n";
		
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		std::ctime(&end_time);
		std::cout << "Execution Time: "<<elapsed_seconds.count();
	
	} 
          
    world.barrier();
    return 0;
}

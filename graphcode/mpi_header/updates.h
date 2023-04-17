#ifndef MPI_UPDATES_HEADER
#define MPI_UPDATES_HEADER

#include<vector>
#include<type_traits>
#include<mpi.h>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include"graph_mpi.h"

enum UpdateType
{
  Addition,
  Delition,
};

class Update
{
  public:
  UpdateType type;
  int32_t source;
  int32_t destination;
  int32_t weight;

  private:
  friend class boost::serialization::access;
  template<class Archive> 
  void serialize(Archive & ar, const unsigned int version)
  {
        ar & type;
        ar & source;
        ar & destination;
        ar & weight;
  }
};
class Updates
{
  Graph* g;
  int num_local_updates;
  std::vector<Update> updates;
  
  int localBatchSize;
  int num_batches;
  int total_updates;
  int current_batch;
  boost::mpi::communicator world;

  bool is_batch;
  Updates * currentBatch = NULL;
  Updates * currentAddBatch = NULL;
  Updates * currentDelBatch = NULL;

  Updates(std::vector<Update> & updates)
  {
    is_batch = true;
    this->updates = updates; 
  }


  public:
  Updates()
  {

  }
  
  Updates(char* file,boost::mpi::communicator world, Graph * g, float percent_updates=100);

  void splitIntoSmallerBatches(int size);
  
  bool nextBatch();
  
  void updateCsrAdd(Graph * g);

  void updateCsrDel(Graph * g);

  std::vector<Update> & getUpdates()
  {
    return updates;
  }

  Updates * getBatch()
  {
    return currentBatch;
  }
  Updates * getCurrentAddBatch()
  {
    return currentAddBatch;
  }
  Updates * getCurrentDeleteBatch()
  {
    return currentDelBatch;
  }

  ~Updates()
  {
    delete currentBatch;
    delete currentAddBatch;
    delete currentDelBatch;
  }
};

#endif
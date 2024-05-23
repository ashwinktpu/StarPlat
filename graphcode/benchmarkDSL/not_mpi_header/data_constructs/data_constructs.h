#ifndef MPI_DATA_CONSTRUCTS
#define MPI_DATA_CONSTRUCTS

#include"../../mpi_header/graph_properties/node_property/node_property.h"
#include"../../mpi_header/graph_properties/edge_property/edge_property.h"
#include "../../mpi_header/graph_mpi.h"
#include <assert.h>
#include <vector>
#include <mpi.h>
#include <cmath>
#include <unordered_map>

void checkMPIComm (int mpiStatus, char* errorMessage) ;

template<typename T>
class Container {

    private :
      MPI_Win array ; // MPI Window.
      int * baseArray ; // Buffer array for the MPI Window.
      int globalSize ; // Total size of the distributed Array.
      int localSize ; // Size of the distributed Array in local.
      int dispUnit ; // Offset for the process.
      int rank ; // Rank of native process.
      int lockAssertion ; // What type of lock assertion to use.
      int numProcs ; // total number of process in comm.
			int __idx ; // The end index of a vector.
      std::vector<T> vect; // This should store the values.
      MPI_Comm comm ; // communicator.
      std::unordered_map <int, std::vector<std::vector<int> > > sync_later ;

    public :
        
      // Constructors.
      Container();
        
      // Constructors not implemented.
      Container(int size);
      Container(int size, T intital_value);

			// serialize the vector.
		  void serializeContainer () ;

			// deserialize the vector.
			void deserializeContainer () ;

      // Will use this in workaround.
      void assign (const int &size, const int &initVal, MPI_Comm comm) ;

		  // adding a value at the container.
      void push_back (T value);

			// Remove a value from the end of the container.
      void pop_back ();
        
      // Helper Functions.
      int calculateTargetRank (const int &idx) ;
      int calculateTargetDisp (const int &idx) ;
      int calculateTargetCount (const int &idx) ; 
        
      void clear();
        
      // Getters and Setters.
      int getValue (const int &node_owner, const int &idx) ;
      void setValue (const int &node_owner, const int &idx, const int &value) ;
      int getIdx (const int& idx) ;
			void atomicAdd (const int &idx, const int &val) ;

      // Might get deprecated functions.
      void queue_assignment(int index , T value);
      void queue_push(T value);
        
      // Synchronizeation assist.
      void sync_assignments();
        
      // Useful for unit testing.
      void printArr () ;
};


#endif

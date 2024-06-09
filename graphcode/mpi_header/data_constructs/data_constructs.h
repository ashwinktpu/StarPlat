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

template<typename T1, typename T2>
class HashMap {

    private :
        std::map<T1,T2> mapvar;
        typename std::map<T1,T2>::iterator it;

    public :
        HashMap()
		{
			this->it=mapvar.begin();
		}

		void clear()
		{
			mapvar.clear();
			this->it=mapvar.begin();
		}

        void reset()
		{
			this->it=mapvar.begin();
		}
        
        void advance()
		{
			(this->it)++;
		}
        
        bool atend()
		{
			return this->it==mapvar.end();
		}

        void put(T1 key , T2 value)
		{
			this->mapvar.insert({key,value});
		}

        T2& get(T1 key)
		{
			return this->mapvar[key];
		}
		
		T1 getcurrentkey()
		{
			return this->it->first;
		}
		
		T2& getcurrentval()
		{
			return this->it->second;
		}
		
		std::map<T1,T2>& getmap()
		{
			return this->mapvar;
		}
		
		void swap(HashMap<T1,T2>& tgt)
		{
			this->mapvar.swap(tgt.getmap());
			this->reset();
			tgt.reset();
		}
		
		bool contains(T1 key)
		{
			return mapvar.count(key)==1;
		}
};

template<typename T1>
class HashSet {

    private :
        std::set<T1> setvar;
        typename std::set<T1>::iterator it;

    public :
        HashSet()
		{
			this->it=setvar.begin();
		}

		void clear()
		{
			setvar.clear();
			this->it=setvar.begin();
		}

        void reset()
		{
			this->it=setvar.begin();
		}
        
        void advance()
		{
			(this->it)++;
		}
        
        bool atend()
		{
			return this->it==setvar.end();
		}

        void insert(T1 val)
		{
			this->setvar.insert(val);
		}
		
		T1 getcurrent()
		{
			return *(this->it);
		}
		
		std::set<T1>& getset()
		{
			return this->setvar;
		}
		
		void swap(HashSet<T1>& tgt)
		{
			this->setvar.swap(tgt.getset());
			this->reset();
			tgt.reset();
		}
		
		void erase(T1 val)
		{
			this->setvar.erase(val);
		}
		
		bool contains(T1 val)
		{
			return setvar.count(val)==1;
		}
};

#endif

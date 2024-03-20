#ifndef MPI_DATA_CONSTRUCTS
#define MPI_DATA_CONSTRUCTS

#include"../graph_properties/node_property/node_property.h"
#include"../graph_properties/edge_property/edge_property.h"
#include<unordered_set>

void mpiAssert (int mpiStatus, char* errorMessage) {
    if (mpiStatus != MPI_SUCCESS) {
        printf ("%s\n", errorMessage) ;
    }
}

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
        std::vector<T> vect; 
        std::unordered_map<int, T> modified_values;
        boost::mpi::communicator comm ;

    public :
        
        // Constructors.
        Container();
        
        // Constructors not implemented.
        Container(int size);
        Container(int size, T intital_value);

        // Will use this in workaround.
        void assign (const int &size, const int &initVal, boost::mpi::communicator comm) ;
        // Functions not in use.
        void push(T value);
        void pop();
        
        // Helper Functions.
        void calculateTargetRank (const int &idx) ;
        void calculateTargetDisp (const int &idx) ;
        void calculateTargetCount (const int &idx) ; 
        
        void clear();
        
        // Getters and Setters.
        T& getValue (const int &idx) ;
        void setValue (const int &idx, const int &value) ;

        // Might get deprecated functions.
        void queue_assignment(int index , T value);
        void queue_push(T value);
        
        // Synchronizeation assist.
        void sync_assignments();
        
        // Useful for unit testing.
        void printArr () ;
};


#endif

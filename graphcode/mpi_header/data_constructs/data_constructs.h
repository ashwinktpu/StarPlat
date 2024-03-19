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
        MPI_Win array ;
        int * baseArray ;
        int globalSize ; // Total size of the distributed Array.
        int localSize ; // Size of the distributed Array in local.
        int dispUnit ; // Offset for the process.
        std::vector<T> vect; 
        std::unordered_map<int, T> modified_values;
        boost::mpi::communicator comm ;

    public :
        Container();
        Container(int size);
        Container(int size, T intital_value);
        void assign (const int &size, const int &initVal, boost::mpi::communicator comm) ;
        void push(T value);
        
        void pop();
        
        void clear();
        
        T& operator[](int index);

        void queue_assignment(int index , T value);

        void queue_push(T value);

        void sync_assignments();

};


#endif

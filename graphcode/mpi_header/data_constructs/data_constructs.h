#ifndef MPI_DATA_CONSTRUCTS
#define MPI_DATA_CONSTRUCTS

#include"../graph_properties/node_property/node_property.h"
#include"../graph_properties/edge_property/edge_property.h"
#include<unordered_set>

template<typename T>
class Container {

    private :
        std::vector<T> vect;
        std::unordered_map<int, T> modified_values;

    public :
        Container();
        Container(int size);
        Container(int size, T intital_value);

        void push(T value);
        
        void pop();
        
        void clear();
        
        T& operator[](int index);

        void queue_assignment(int index , T value);

        void queue_push(T value);

        void sync_assignments();

};


#endif
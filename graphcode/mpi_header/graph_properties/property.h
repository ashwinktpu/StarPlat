#ifndef MPI_PROPERTY_HEADER
#define MPI_PROPERTY_HEADER

#include <type_traits>
#include <cstdint>
#include<vector>
#include<mpi.h>
class Property
{
    
    public :
    bool is_node_property;
    Property(bool is_node_property)
    {
        this->is_node_property = true;
    }

    virtual int getPropertyId()=0;
    virtual void initialize_reduction_queue()=0;
    virtual std::vector<std::vector<int32_t>> perform_reduction(MPI_Op op)=0;
    virtual void assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids)=0;
    virtual ~Property(){};
};

enum Aggregate_Op{
  NO_OP,
  NOT,
};

#endif
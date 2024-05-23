#ifndef MPI_EDGE_PROPERTY_HEADER
#define MPI_EDGE_PROPERTY_HEADER

#include"../../types.h"
#include"../property.h"
#include"../../rma_datatype/rma_datatype.h"


//TODO : Reduction on edges is not checked for correctness.

// For dynamic case when edges are added/deleted and diff_csr is reallocated, currently there is no straightforward
// way or implementation in the below class to handle the reaarangement of edge properties when edges get rearranged in
// diff csr and added/deleted in diff csr. only the weights which is also an edgepeoperty is being handled 
// explicitly when edges are added or deleted. 
// Thus with the current implementation of EdgeProperty class one should be careful in using it with dynamic algorithms
// as, if an EdgeProperty instance is used both before and after a dynamic add/deletion edges step then the property values might 
// be inconsistent (Only weights property will be consisitent as it is explicitly handled)   
class Graph;
template<typename T>
class EdgeProperty : public Property
{
    private :
    int length;
    
    std::unordered_map<int, std::vector<std::vector<int> > > sync_later ;

    bool diff_propList_present;
    
    int diff_length;

    bool attached_to_graph;
    int propertyId;
    Graph * graph;
    boost::mpi::communicator world;
    T * history_data;
    T * diff_history_data;

    //TODO : Have to modify logic for reduction when diff_proplist is present
    std::vector<std::vector<std::pair<int32_t, T>>> reduction_queue;
    std::vector<bool> already_locked_processors;
    std::vector<bool> already_locked_processors_shared;

    public :
    Rma_Datatype<T> propList;
    Rma_Datatype<T> diff_propList;
    void fatBarrier () ;
    void leaveAllSharedLocks () ;

    EdgeProperty() : Property(true) 
    {
        if constexpr (std::is_same_v<T, int>)
        {    
            propList.mpi_datatype = MPI_INT32_T;  
            diff_propList.mpi_datatype = MPI_INT32_T;
        }
        else if (std::is_same_v<T, long>)
        {
            propList.mpi_datatype = MPI_INT64_T;
            diff_propList.mpi_datatype = MPI_INT64_T;
        }    
        else if (std::is_same_v<T, float>)
        {    
            propList.mpi_datatype = MPI_FLOAT;
            diff_propList.mpi_datatype = MPI_FLOAT;
        }
        else if (std::is_same_v<T, double>)
        {    
            propList.mpi_datatype = MPI_DOUBLE;
            diff_propList.mpi_datatype = MPI_DOUBLE;
        }
        else if (std::is_same_v<T, bool>) 
        {    
            propList.mpi_datatype = MPI_CXX_BOOL;
            diff_propList.mpi_datatype = MPI_CXX_BOOL;
        }
        else 
        {
          std::cerr<<"Invalid EdgeProperty type";
          exit(-1);
        }
        attached_to_graph = false;
        diff_propList_present = false;       
    }

    void initialize_reduction_queue() override;
    
    void add_to_reduction_queue(int reduction_node,std::pair<int32_t, T> pair);
    
    std::vector<std::vector<int32_t>> perform_reduction(MPI_Op op) override;

    void assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids) override;

    void attachToGraph(Graph * graph, T initial_value, bool attach_only_to_diff_csr= false, bool create_new_diff= true);

    void attachToGraph(Graph * graph, T * initial_values, T* diff_initial_values = NULL, bool attach_only_to_diff_csr= false, bool create_new_diff = true);
    
    int32_t getPropertyId() override;

    T getValue(Edge edge ,bool check_concurrency = true);
    
    void setValue(Edge edge ,T value, bool check_concurrency = true);
    
    void rememberHistory();
    
    T getHistoryValue(Edge edge);
    
    void operator =(const EdgeProperty<T>& property);
    
    void deleteHistory();
    
    bool aggregateValue(Aggregate_Op op);
    
    T compareAndSwap(Edge edge, T compare, T swap);

    void atomicAdd(Edge &edge, T value);
    
};
    
#endif

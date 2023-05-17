#ifndef GRAPH_MPI_HEADER
#define GRAPH_MPI_HEADER

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <mpi.h>
#include <string.h>
#include <algorithm>
#include <unordered_map>
#include <type_traits>
#include <unordered_set>

#include "types.h"
#include"graph_properties/node_property/node_property.h"
#include"graph_properties/edge_property/edge_property.h"
#include"rma_datatype/rma_datatype.h"

#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/unordered_set.hpp>

class Graph
{
  private:
  int32_t nodesTotal;
  int32_t edgesTotal;
  char* filePath;
  int32_t nodesPartitionSize; // The number of vertices this particular proc will own.
  int32_t startNode; // The start Node that this proc owns
  int32_t endNode;   // The end Node that this proc owns
  boost::mpi::communicator world;
  int32_t properties_counter;
  std::unordered_map<int32_t, Property *> properties;
  MPI_Win reduction_window;
  int32_t * reduction_lock;
  std::vector<int32_t> edgeProcMap; 
  MPI_Op reduction_op;
  int32_t reduction_property_id;
  std::vector<int32_t> other_reduction_properties_id;
  std::vector<std::vector<int32_t>> bfs_level_nodes;
  std::vector<std::vector<int32_t>> bfs_children;
  Rma_Datatype<int32_t> indexofNodes; /* stores prefix sum for outneighbours of a node*/
  Rma_Datatype<int32_t> rev_indexofNodes; /* stores prefix sum for inneighbours of a node*/
  Rma_Datatype<int32_t> destList; /*stores destination corresponding to edgeNo.
                       required for iteration over out neighbours */
  Rma_Datatype<int32_t> srcList;  /*stores source corresponding to edgeNo.
                       required for iteration over in neighbours */
  
  // Dynamic CSR Variables to be added later
  bool diff_csr_created ;
  bool rev_diff_csr_created;                      
  Rma_Datatype<int32_t> diff_indexofNodes;
  Rma_Datatype<int32_t> diff_destList; 
  Rma_Datatype<int32_t> diff_rev_indexofNodes;
  Rma_Datatype<int32_t> diff_srcList;
  
  Rma_Datatype<int32_t> perNodeCSRSpace;
  Rma_Datatype<int32_t> perNodeRevCSRSpace;
  Rma_Datatype<int32_t> perNodeDiffCSRSpace;
  Rma_Datatype<int32_t> perNodeDiffRevCSRSpace;
  std::vector<int32_t> diff_edgeProcMap;
  

  int bfs_phases ;   

  template<typename T>
  void friend NodeProperty<T>::attachToGraph(Graph * graph, T initial_value);
  
  template<typename T>
  void friend NodeProperty<T>::attachToGraph(Graph * graph, T * initial_values);

  template<typename T>
  void friend EdgeProperty<T>::attachToGraph(Graph * graph, T initial_value, bool attach_only_to_diff_csr, bool create_new_diff);

  template<typename T>
  void friend EdgeProperty<T>::attachToGraph(Graph * graph, T * initial_values, T* diff_initial_values , bool attach_only_to_diff_csr, bool create_new_diff );

  
  public:
  EdgeProperty<int32_t> weights;

  void create_bfs_dag(int src);
  int num_bfs_phases();
  std::vector<int32_t>& get_bfs_nodes_for_phase(int phase);
  std::vector<int32_t>& get_bfs_children(int node);
  


  Graph(char* file, boost::mpi::communicator world );
  void print_csr();  
  void get_lock_for_reduction_statement();
  void unlock_for_reduction_statement();
  int num_nodes();
  int num_edges();
  int start_node();
  int end_node();
  Edge start_edge();
  Edge end_edge();
  Edge next_edge(Edge & edge);
  int num_nodes_owned();
  int num_out_nbrs(int node);
  int get_node_owner(int node);
  int  get_node_local_index(int node);
  int get_edge_owner(Edge edge);
  int get_edge_local_index(Edge edge);
  Edge get_edge(int v, int nbr);
  bool check_if_nbr(int s, int d);
  std::vector<int32_t> getNeighbors( int node);
  std::vector<int32_t> getInNeighbors( int node);
  void readFromFile(std::string filePath, int32_t &num_nodes, int32_t &num_edges, std::vector<int32_t> & src, std::vector<int32_t> & dest , std::vector<int32_t> & weights);

  void addEdges_Csr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates);
  void delEdges_Csr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates);
  void addEdges_RevCsr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates);
  void delEdges_RevCsr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates);

  void initialise_reduction(MPI_Op op, Property* reduction_property, std::vector<Property*> other_properties);

  void sync_reduction();

  void propagateNodeFlags(NodeProperty<bool>& property);

  template<typename T, typename ... Types>
  void queue_for_reduction(std::pair<int, T> reduction_property_data , Types ... other_properties_data)
  {
      if(properties[reduction_property_id]->is_node_property)
      {
        NodeProperty<T> * node_property_ptr= static_cast<NodeProperty<T>*>(properties[reduction_property_id]);
        if(node_property_ptr == NULL)
        {
          std::cerr<<"(queue_for_reduction) Invalid dynamic cast\n"<<std::endl;
          exit(-1);
        }
        node_property_ptr->add_to_reduction_queue(reduction_property_data.first, reduction_property_data);
      }
      else{
        EdgeProperty<T> * edge_property_ptr= static_cast<EdgeProperty<T>*>(properties[reduction_property_id]);
        if(edge_property_ptr == NULL)
        {
          std::cerr<<"(queue_for_reduction) Invalid dynamic cast\n"<<std::endl;
          exit(-1);
        }
        edge_property_ptr->add_to_reduction_queue(reduction_property_data.first,reduction_property_data);
      }
      
      queue_for_reduction_helper(reduction_property_data.first,0,other_properties_data...);
  }

  template<typename T, typename ... Types>
  void queue_for_reduction_helper(int reduction_node, int counter,std::pair<int, T> head_data, Types ... remaining_data)
  {
    if(properties[other_reduction_properties_id[counter]]->is_node_property)
      {
        NodeProperty<T> * node_property_ptr= static_cast<NodeProperty<T>*>(properties[other_reduction_properties_id[counter]]);
        if(node_property_ptr == NULL)
        {
          std::cerr<<"(queue_for_reduction_helper) Invalid dynamic cast\n"<<std::endl;
          exit(-1);
        }
        node_property_ptr->add_to_reduction_queue(reduction_node,head_data);
      }
    else
    {
        EdgeProperty<T> * edge_property_ptr= static_cast<EdgeProperty<T>*>(properties[other_reduction_properties_id[counter]]);
        if(edge_property_ptr == NULL)
        {
          std::cerr<<"(queue_for_reduction_helper) Invalid dynamic cast\n"<<std::endl;
          exit(-1);
        }
        edge_property_ptr->add_to_reduction_queue(reduction_node,head_data);
    }
  
    queue_for_reduction_helper(reduction_node,counter+1, remaining_data...);
  }

  void queue_for_reduction_helper(int reduction_node,int counter){}

};  

#endif
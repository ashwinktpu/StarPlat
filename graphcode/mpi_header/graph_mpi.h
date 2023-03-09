#ifndef MPI_GRAPH_HEADER
#define MPI_GRAPH_HEADER

/** Header Files **/
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
#include"rma_datatype.h"
/** Boost and MPI Specific **/
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/unordered_set.hpp>


enum Aggregate_Op{
  NO_OP,
  NOT,
};


class Graph;
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

template<typename T>
class NodeProperty : public Property
{
    private :
    int length;
    Rma_Datatype<T> propList;
    bool attached_to_graph;
    int propertyId;
    Graph * graph;
    boost::mpi::communicator world;
    T * history_data;
    std::vector<bool> already_locked_processors;
    std::vector<std::vector<std::pair<int32_t, T>>> reduction_queue;

    public :
    NodeProperty() : Property(true) 
    {
        if constexpr (std::is_same_v<T, int>)
            propList.mpi_datatype = MPI_INT32_T;  
        else if (std::is_same_v<T, long>)
            propList.mpi_datatype = MPI_INT64_T;
        else if (std::is_same_v<T, float>)
            propList.mpi_datatype = MPI_FLOAT;
        else if (std::is_same_v<T, double>)
            propList.mpi_datatype = MPI_DOUBLE;
        else if (std::is_same_v<T, bool>) 
            propList.mpi_datatype = MPI_CXX_BOOL;
        else 
        {
          std::cerr<<"Invalid NodeProperty type";
          exit(-1);
        }
        attached_to_graph = false;       
    }

    
    void initialize_reduction_queue () override;
    
    void add_to_reduction_queue (int reduction_node,std::pair<int32_t, T> pair) ;
    
    std::vector<std::vector<int32_t>> perform_reduction(MPI_Op op) override;

    void assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids) override;

    void attachToGraph(Graph * graph, T initial_value);

    void attachToGraph(Graph * graph, T * initial_values);
    
    int32_t getPropertyId() override;

    T getValue(int node_id ,bool check_concurrency = true);

    void setValue(int node_id ,T value, bool check_concurrency = true);

    void remeberHistory();

    T getHistoryValue(int node_id);
 
    // TODO : Add lock here too
    void assignCopy(NodeProperty<T>& property);

    void deleteHistory();

    bool aggregateValue(Aggregate_Op op);

    void getExclusiveLock(int node_id);

    void releaseExclusiveLock(int node_id);

    T compareAndSwap(int node_id, T compare, T swap);

    void atomicAdd(int node_id, T value);
};

template<typename T>
class EdgeProperty : public Property
{
    private :
    int length;
    Rma_Datatype<T> propList;
    bool attached_to_graph;
    int propertyId;
    Graph * graph;
    boost::mpi::communicator world;
    T * history_data;
    std::vector<bool> already_locked_processors;
    std::vector<std::vector<std::pair<int32_t, T>>> reduction_queue;

    public :
    EdgeProperty() : Property(true) 
    {
        if constexpr (std::is_same_v<T, int>)
            propList.mpi_datatype = MPI_INT32_T;  
        else if (std::is_same_v<T, long>)
            propList.mpi_datatype = MPI_INT64_T;
        else if (std::is_same_v<T, float>)
            propList.mpi_datatype = MPI_FLOAT;
        else if (std::is_same_v<T, double>)
            propList.mpi_datatype = MPI_DOUBLE;
        else if (std::is_same_v<T, bool>) 
            propList.mpi_datatype = MPI_CXX_BOOL;
        else 
        {
          std::cerr<<"Invalid EdgeProperty type";
          exit(-1);
        }
        attached_to_graph = false;       
    }

    void initialize_reduction_queue() override;
    
    void add_to_reduction_queue(int reduction_node,std::pair<int32_t, T> pair);
    
    std::vector<std::vector<int32_t>> perform_reduction(MPI_Op op) override;

    void assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids) override;

    void attachToGraph(Graph * graph, T initial_value);

    void attachToGraph(Graph * graph, T * initial_values);
    
    int32_t getPropertyId() override;

    T getValue(int edge_id ,bool check_concurrency = true);
    
    void setValue(int edge_id ,T value, bool check_concurrency = true);
    
    void remeberHistory();
    
    T getHistoryValue(int edge_id);
    
    void assignCopy(EdgeProperty<T>& property);
    
    void deleteHistory();
    
    bool aggregateValue(Aggregate_Op op);
    
    void getExclusiveLock(int edge_id);
    
    void releaseExclusiveLock(int edge_id);
    
    T compareAndSwap(int edge_id, T compare, T swap);

    void atomicAdd(int node_id, T value);
    
};


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
  int bfs_phases ;   
  public:
  void create_bfs_dag(int src)
  {
    bfs_level_nodes.clear();
    bfs_level_nodes.push_back(std::vector<int32_t>());
    std::vector<bool> visited(nodesPartitionSize,false) ;
  
    if(get_node_owner(src) == world.rank())
    {
        bfs_level_nodes[0].push_back(src);
        visited[get_node_local_index(src)] = true;
    }
    bfs_children.resize(nodesPartitionSize);
    bfs_phases=1;
    int bfs_count = 1;
    while(bfs_count>0)
    {   
        bfs_level_nodes.push_back(std::vector<int32_t>());
        
        std::vector<std::unordered_set<int32_t>> next_level(world.size());
        for(int v : bfs_level_nodes[bfs_phases-1])
        {
            for(int nbr : getNeighbors(v))
            {
                next_level[get_node_owner(nbr)].insert(nbr);
            }
        }
        boost::mpi::all_to_all(world,next_level,next_level);
        
        std::vector<std::unordered_set<int32_t>> children(world.size());
        std::unordered_set<int32_t> this_level;
        for(int i=0;i<world.size();i++)
        {
          for(int v: next_level[i])
          {
            if(!visited[get_node_local_index(v)])
            {
              children[i].insert(v);
              this_level.insert(v);      
            }
          }
        }
        int temp_bfs_count = this_level.size();
        for(int v :this_level)
        {
            visited[get_node_local_index(v)] = true;
            bfs_level_nodes[bfs_phases].push_back(v);  
        }
        boost::mpi::all_to_all(world, children, children);
        MPI_Allreduce(&temp_bfs_count, &bfs_count,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
        
        for(int v : bfs_level_nodes[bfs_phases-1])
        {   
          for(int nbr : getNeighbors(v))
            {
              if(children[get_node_owner(nbr)].find(nbr)!=children[get_node_owner(nbr)].end())
                  bfs_children[get_node_local_index(v)].push_back(nbr);
    
            }
        }
        
        bfs_phases++;
    }
  }
  int num_bfs_phases()
  {
    return bfs_phases;
  }
  std::vector<int32_t>& get_bfs_nodes_for_phase(int phase)
  {
    return bfs_level_nodes[phase];
  }
  std::vector<int32_t>& get_bfs_children(int node)
  {
    if(get_node_owner(node) != world.rank())
    {
      std::cerr<<"Should not reach here"<<std::endl;
      exit(-1);
    }
    return bfs_children[get_node_local_index(node)];
  }
  Rma_Datatype<int32_t> indexofNodes; /* stores prefix sum for outneighbours of a node*/
  Rma_Datatype<int32_t> rev_indexofNodes; /* stores prefix sum for inneighbours of a node*/
  Rma_Datatype<int32_t> destList; /*stores destination corresponding to edgeNo.
                       required for iteration over out neighbours */
  Rma_Datatype<int32_t> srcList;  /*stores source corresponding to edgeNo.
                       required for iteration over in neighbours */
  
  EdgeProperty<int32_t> weights;

  template<typename T>
  void friend NodeProperty<T>::attachToGraph(Graph * graph, T initial_value);
  
  template<typename T>
  void friend NodeProperty<T>::attachToGraph(Graph * graph, T * initial_values);

  template<typename T>
  void friend EdgeProperty<T>::attachToGraph(Graph * graph, T initial_value);
  
  template<typename T>
  void friend EdgeProperty<T>::attachToGraph(Graph * graph, T * initial_values);
  /* Dynamic CSR Variables to be added later                      
  int32_t* diff_indexofNodes;
  int32_t* diff_edgeList; 
  int32_t* diff_rev_indexofNodes;
  int32_t* diff_rev_edgeList;
  int32_t* perNodeCSRSpace;
  int32_t* perNodeRevCSRSpace;
  int32_t* edgeMap;
  */


  Graph(char* file, boost::mpi::communicator world )
  {
    filePath=file;
    nodesTotal=0;
    edgesTotal=0;
    this->world = world;
    properties_counter =0;
    // Proc 0 reads all the edges from the file
    std::vector<int32_t> src , dest, weight;
    if (world.rank() == 0)
    {
      readFromFile(filePath, nodesTotal, edgesTotal, src , dest, weight);
    }

    world.barrier();

    // broadcast the number of edges and number of nodes to other procs
    boost::mpi::broadcast(world, nodesTotal, 0);
    boost::mpi::broadcast(world, edgesTotal, 0);

    // distribute the edges to all procs for processing and formation of csr
    std::vector<int32_t> sizes, offsets;
    offsets.push_back(0);
    for (int i = 0; i < world.size(); i++)
    {
      sizes.push_back(edgesTotal / world.size() + (i < (edgesTotal % world.size()) ? 1 : 0));
      if(i!= world.size()-1)
        offsets.push_back(offsets[i] + sizes[i]);
    }
    int32_t scatter_size = edgesTotal / world.size() + (world.rank() < (edgesTotal % world.size()) ? 1 : 0);

    // localEdges recives the a part of all the edges which are distributed by proc 0 to all procs.
    int32_t *localsrc = new int32_t[scatter_size];
    int32_t *localdest = new int32_t[scatter_size];
    int32_t *localweight = new int32_t[scatter_size];
    
    MPI_Scatterv(src.data(), sizes.data(), offsets.data(), MPI_INT32_T, localsrc, scatter_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatterv(dest.data(), sizes.data(), offsets.data(), MPI_INT32_T, localdest, scatter_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatterv(weight.data(), sizes.data(), offsets.data(), MPI_INT32_T, localweight, scatter_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
   
    // The number of vertices this particular proc will own.
    nodesPartitionSize = (nodesTotal + world.size() - 1) / world.size();
    startNode = world.rank() * nodesPartitionSize;
    endNode = std::min(startNode + nodesPartitionSize -1, nodesTotal -1 );

    /* We first build local adjacency list according to current edges that each proc has, and then redestribute this
     edges using all to all so that each proc gets the edges corresponding to nodes which the proc owns */
    std::vector<std::vector<int32_t>> adjacency_list_group(nodesPartitionSize);
    std::vector<std::vector<std::vector<int32_t>>> adjacency_list_3d(world.size(), adjacency_list_group);
    std::vector<std::vector<std::vector<int32_t>>> weight_list_3d(world.size(), adjacency_list_group);
    std::vector<std::vector<std::vector<int32_t>>> rev_adjacency_list_3d(world.size(), adjacency_list_group);
    

    for (int i = 0; i < scatter_size; i++)
    {
      int proc_num = localsrc[i] / nodesPartitionSize;
      adjacency_list_3d[proc_num][localsrc[i] % nodesPartitionSize].push_back((localdest[i]));
      weight_list_3d[proc_num][localsrc[i] % nodesPartitionSize].push_back(localweight[i]);

      proc_num = localdest[i] / nodesPartitionSize;
      rev_adjacency_list_3d[proc_num][localdest[i] % nodesPartitionSize].push_back((localsrc[i]));
    
    }
    delete[] localsrc ;
    delete[] localdest;
    delete[] localweight;

    int32_t * indexofNodes = new int32_t[nodesPartitionSize+1];
    int32_t * rev_indexofNodes = new int32_t[nodesPartitionSize+1];
    indexofNodes[0]=0; rev_indexofNodes[0]=0;

    boost::mpi::all_to_all(world, adjacency_list_3d, adjacency_list_3d);
    boost::mpi::all_to_all(world, weight_list_3d, weight_list_3d);
    boost::mpi::all_to_all(world, rev_adjacency_list_3d, rev_adjacency_list_3d); 
    
    
    std::vector<int32_t> destList, srcList, weightList;
    indexofNodes[0] = 0;
    rev_indexofNodes[0] = 0;
    for (int i = 0; i < nodesPartitionSize; i++)
    {
      std::vector<std::pair<int32_t,int32_t>> temp1;
      std::vector<int32_t> temp2;
      for (int j = 0; j < world.size(); j++)
      {
        for(int k=0;k<(int)adjacency_list_3d[j][i].size();k++)
        {
          temp1.push_back(std::make_pair(adjacency_list_3d[j][i][k], weight_list_3d[j][i][k]));
        }
        temp2.insert(temp2.end(), rev_adjacency_list_3d[j][i].begin(), rev_adjacency_list_3d[j][i].end());
      }

      sort(temp1.begin(), temp1.end(), [](const std::pair<int32_t,int32_t>& e1,const std::pair<int32_t,int32_t>& e2) { return e1.first < e2.first; });
      sort(temp2.begin(), temp2.end(), [](const int32_t& e1,const int32_t& e2) { return e1 < e2; });

      indexofNodes[i+1] = indexofNodes[i] + temp1.size();
      rev_indexofNodes[i+1] = rev_indexofNodes[i] + temp2.size();

      for(int k=0;k<(int)temp1.size();k++)
      {
        destList.push_back(temp1[k].first);
        weightList.push_back(temp1[k].second);
      }
      srcList.insert(srcList.end(), temp2.begin(), temp2.end());
	  }
    
    int32_t destListSize = destList.size();
    int32_t * destListSizes = new int32_t [world.size()];
    MPI_Allgather(&destListSize,1,MPI_INT32_T,destListSizes,1,MPI_INT32_T, MPI_COMM_WORLD);

    edgeProcMap.push_back(0);
    for(int i=0;i<world.size();i++)
    {
      edgeProcMap.push_back(edgeProcMap[i]+destListSizes[i]);
    }
    delete [] destListSizes;

    world.barrier();
    this->indexofNodes.mpi_datatype = MPI_INT32_T;
    this->indexofNodes.create_window(indexofNodes, nodesPartitionSize +1, sizeof(int32_t),world);
    
    this->rev_indexofNodes.mpi_datatype = MPI_INT32_T;
    this->rev_indexofNodes.create_window(rev_indexofNodes, nodesPartitionSize +1, sizeof(int32_t), world);
    
    this->destList.mpi_datatype = MPI_INT32_T;
    this->destList.create_window(destList.data(), destList.size(), sizeof(int32_t), world);

    this->srcList.mpi_datatype = MPI_INT32_T;
    this->srcList.create_window(srcList.data(), srcList.size(), sizeof(int32_t), world);

    weights.attachToGraph(this, weightList.data());

    MPI_Win_allocate(sizeof(int),  sizeof(int) , MPI_INFO_NULL, world, &reduction_lock,&reduction_window);
    if(world.rank()==0)
      *reduction_lock = 0;
    world.barrier();
    
  }
 
  void initialise_reduction(MPI_Op op, Property* reduction_property, std::vector<Property*> other_properties)
  {
      reduction_op = op;
      reduction_property_id = reduction_property->getPropertyId();
      reduction_property->initialize_reduction_queue();

      other_reduction_properties_id.clear();
      for(auto p : other_properties)
      {
        other_reduction_properties_id.push_back(p->getPropertyId());
        p->initialize_reduction_queue();
      }
      
  }

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


  void sync_reduction()
  {
      std::vector<std::vector<int32_t>> modifed_ids = properties[reduction_property_id]->perform_reduction(reduction_op);
      
      for(int id :other_reduction_properties_id)
      {
        properties[id]->assign_reduction_values(modifed_ids);
      }
  }

  void queue_for_reduction_helper(int reduction_node,int counter)
  {
    return;
  }

  void get_lock_for_reduction_statement()
  { 
    int new_value = 1;
    int old_value = 0;
    int lock_value = 1;
    while(lock_value == 1)
    {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0,reduction_window);
        MPI_Compare_and_swap(&new_value, &old_value, &lock_value,MPI_INT,0,0,reduction_window);
        MPI_Win_unlock (0, reduction_window);
    }

  }
  void unlock_for_reduction_statement()
  {
    int new_value = 0;
    int old_value = 1;
    int lock_value;
    
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0,reduction_window);
    MPI_Compare_and_swap(&new_value, &old_value, &lock_value,MPI_INT,0,0,reduction_window);
    MPI_Win_unlock (0, reduction_window);

    if(lock_value != 1)
    {
      std::cerr<<"Unlock called even though no lock was present on reduction window\n";
      exit(-1);
    }
  }

  int num_nodes()
  {
    return nodesTotal;
  }

  int num_edges()
  {
    return edgesTotal;
  }

  int start_node()
  {
    return startNode;
  }

  int end_node()
  {
    return endNode;
  }

  int start_edge()
  {
    return edgeProcMap[world.rank()];
  }
  int end_edge()
  {
    return edgeProcMap[world.rank()+1];
  }
  int num_nodes_owned()
  {
    return endNode - startNode +1;
  }

  int num_out_nbrs(int node)
  {   
      int owner_proc = get_node_owner(node);
      int index = get_node_local_index(node);
      int count =0;
      if(owner_proc==world.rank())
      {
        indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
        count = indexofNodes.data[index+1] - indexofNodes.data[index];
        indexofNodes.unlock(owner_proc, SHARED_LOCK);
            
      }
      else {
        indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
        int * destListIndices = indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
        indexofNodes.unlock(owner_proc, SHARED_LOCK);
        count = destListIndices[1] - destListIndices[0];
      }
      return count;
  }

  int get_node_owner(int node)
  {
    return node/nodesPartitionSize;
  }

  int  get_node_local_index(int node)
  {
    return node%nodesPartitionSize;
  }

  int get_edge_owner(int edge)
  {
      int owner = 0;
      for(int i=0;i<world.size();i++)
      {
        if(edgeProcMap[i] <= edge)
          owner = i;
        else 
          break;  
      }

      return owner;
  }

  int get_edge_local_index(int edge)
  {
      return edge - edgeProcMap[get_edge_owner(edge)];
  }

  int get_edge(int v, int nbr)
  {
    
     std::vector<int> neighbors = getNeighbors(v);
     indexofNodes.get_lock(get_node_owner(v),SHARED_LOCK);
     int v_local_offset = indexofNodes.get_data(get_node_owner(v),get_node_local_index(v),1,SHARED_LOCK)[0];
     indexofNodes.unlock(get_node_owner(v),SHARED_LOCK);
     
      
     for(int i=0;i<(int)neighbors.size();i++)
     {
        if(neighbors[i]==nbr)
          return edgeProcMap[get_node_owner(v)] + v_local_offset +i;
     }

     std::cerr<<"invalid edge asked in get_edge\n";
     exit(-1);
  }

  

  /*Edge getEdge(int s , int d)
  {
      for( Edge e : getNeighbors(s))
      {
        if(e.destination == d)
        {
          return e;
        }
      }
  }*/
  
  bool check_if_nbr(int s, int d)
  {
    for ( int32_t e: getNeighbors(s))
    {
      if(e== d)
        return true;
    }
    return false;
  }

  std::vector<int32_t> getNeighbors( int node)
  {
    int owner_proc = node / nodesPartitionSize;
    int index = node % nodesPartitionSize;
    
    std::vector<int32_t> out_edges;
    if(world.rank()== owner_proc)
    {
      indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int start = indexofNodes.data[index], end = indexofNodes.data[index+1];
      indexofNodes.unlock(owner_proc, SHARED_LOCK); 
      
      destList.get_lock(owner_proc, SHARED_LOCK, false);
      for(int i=start ; i<end ; i++)
      {
        if(destList.data[i]!=INT_MAX/2)
        {
          out_edges.push_back(destList.data[i]);
        }
     }
     destList.unlock(owner_proc, SHARED_LOCK);
    }
    else 
    {
      indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int * destListIndices = indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
      indexofNodes.unlock(owner_proc, SHARED_LOCK);

      int32_t start = destListIndices[0];
      int32_t end = destListIndices[1];
      destList.get_lock(owner_proc, SHARED_LOCK, false);
      int32_t * edges = destList.get_data(owner_proc, start, end - start,SHARED_LOCK); 
      destList.unlock(owner_proc, SHARED_LOCK);

      out_edges.insert(out_edges.begin(), edges, edges + end - start);
      
    }

    // TODO : Update Diff CSR Logic during dynamic mpi addition

    return out_edges;
  }

  std::vector<int32_t> getInNeighbors( int node)
  {
    int owner_proc = node / nodesPartitionSize;
    int index = node % nodesPartitionSize;

    std::vector<int32_t> in_edges;
    if(world.rank()== owner_proc)
    {
      rev_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int start = rev_indexofNodes.data[index], end = rev_indexofNodes.data[index+1];
      rev_indexofNodes.unlock(owner_proc, SHARED_LOCK);

      srcList.get_lock(owner_proc, SHARED_LOCK, false);
      for(int i=start ; i<end ; i++)
      {
        if(srcList.data[i]!=INT_MAX/2)
        {
          in_edges.push_back(srcList.data[i]);
        }
      }
      srcList.unlock(owner_proc, SHARED_LOCK);
    }
    else 
    {
      rev_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int * srcListIndices = rev_indexofNodes.get_data(owner_proc, index, 2, SHARED_LOCK);
      rev_indexofNodes.unlock(owner_proc, SHARED_LOCK);

      srcList.get_lock(owner_proc, SHARED_LOCK, false);
      int32_t * edges = srcList.get_data(owner_proc, srcListIndices[0], srcListIndices[1] - srcListIndices[0],SHARED_LOCK); 
      srcList.unlock(owner_proc, SHARED_LOCK);
      
      in_edges.insert(in_edges.begin(), edges, edges + srcListIndices[1] - srcListIndices[0]);
    }

    // TODO : Update Diff CSR Logic during dynamic mpi addition

    return in_edges;
  }

  void readFromFile(std::string filePath, int32_t &num_nodes, int32_t &num_edges, std::vector<int32_t> & src, std::vector<int32_t> & dest , std::vector<int32_t> & weights)
  {
        num_nodes = 0;
        num_edges = 0;

        std::ifstream infile;
        infile.open(filePath);
        std::string line;

        while (std::getline(infile, line))
        {

            if (line.length() == 0 || line[0] < '0' || line[0] > '9')
            {
                continue;
            }

            std::stringstream ss(line);

            num_edges++;


            int32_t source;
            int32_t destination;
            int32_t weightVal;

            ss >> source;
            if (source > num_nodes)
                num_nodes = source;

            ss >> destination;
            if (destination > num_nodes)
                num_nodes = destination;

            if(!(ss >> weightVal))
              weightVal =1;

            src.push_back(source);
            dest.push_back(destination);
            weights.push_back(weightVal);
        }

        num_nodes++;

        infile.close();
    }
};  

    template <typename T>
    void NodeProperty<T>::attachToGraph(Graph * graph, T initial_value)
    { 
        // This means this property was already assigned and the window is already created in this case we just
        // reinitialise all the values to inital_value
        if(attached_to_graph == true)
        {
            for(int i=0;i<length;i++)
              propList.data[i] = initial_value;

            return;
        }
        attached_to_graph =true;

        propertyId = graph->properties_counter;
        graph->properties[propertyId] = (Property*)this;
        graph->properties_counter = graph->properties_counter + 1;

        

        length = graph->nodesPartitionSize;

        T * data = new T [length];
        for(int i=0;i<length;i++)
          data[i]=initial_value;

        world = graph->world;
        propList.create_window(data, length, sizeof(T), world);

        already_locked_processors = std::vector<bool>(world.size(),false);
        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    void NodeProperty<T>::attachToGraph(Graph * graph, T * initial_values)
    { 
        // This means this property was already assigned and the window is already created in this case we just
        // reinitialise all the values to inital_value
        if(attached_to_graph == true)
        {
            for(int i=0;i<length;i++)
              propList.data[i] = initial_values[i];

            return;
        }
        attached_to_graph =true;

        propertyId = graph->properties_counter;
        graph->properties[propertyId] = (Property*)this;
        graph->properties_counter = graph->properties_counter + 1;

        

        length = graph->nodesPartitionSize; 

        T * data = new T [length];
        for(int i=0;i<length;i++)
          data[i]=initial_values[i];

         
        world = graph->world;
        propList.create_window(data, length, sizeof(T), world);

        already_locked_processors = std::vector<bool>(world.size(),false);
        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    int32_t NodeProperty<T>::getPropertyId() {return propertyId;}
    
    template <typename T>
    T NodeProperty<T>::getValue(int node_id ,bool check_concurrency)
    {
      int owner_proc = graph->get_node_owner(node_id);
      int local_node_id = graph->get_node_local_index(node_id);
      
      bool no_checks_needed = !check_concurrency;

      if(owner_proc==world.rank())
      {
        //if(!already_locked_processors[owner_proc])
        //  propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        T value = propList.data[local_node_id];
        //if(!already_locked_processors[owner_proc])
        //  propList.unlock(owner_proc, SHARED_LOCK);
          
        return value;
      }
      else
      {
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        T* data = propList.get_data(owner_proc, local_node_id, 1, SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
        return data[0];
      }
      
    }

    template <typename T>
    void NodeProperty<T>::setValue(int node_id ,T value, bool check_concurrency)
    {
      
      int owner_proc = graph->get_node_owner(node_id);
      int local_node_id = graph->get_node_local_index(node_id);

      bool no_checks_needed = !check_concurrency;

      //if(owner_proc==world.rank())
      //{
      //  if(!already_locked_processors[owner_proc])
      //    propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
      //  propList.data[local_node_id] = value;
      //  if(!already_locked_processors[owner_proc])
      //    propList.unlock(owner_proc, SHARED_LOCK);
      //}
      //else
      //{
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        propList.put_data(owner_proc,&value, local_node_id, 1, SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
      //}
      
      return ;
    }

    template <typename T>
    void NodeProperty<T>::remeberHistory()
    {
        history_data = new T [length];
        propList.get_lock(world.rank(),SHARED_LOCK);
        for(int i=0;i<length;i++)
        {
            history_data[i] = propList.data[i];
        }
        propList.unlock(world.rank(), SHARED_LOCK);
    }

    template <typename T>
    T NodeProperty<T>::getHistoryValue(int node_id)
    {
        int owner_proc = graph->get_node_owner(node_id);
        int local_node_id = graph->get_node_local_index(node_id);

        // If it is required that we need history value from a different processor remove this
        // and instead make a separate window(separate RMA_Datatype variable) for copied data and
        // use MPI_GET like normal
        assert(owner_proc == world.rank());

        return history_data[local_node_id];
    }


    // TODO : Add lock here too
    template <typename T>
    void NodeProperty<T>::assignCopy(NodeProperty<T>& property)
    {
        for(int i=0;i<length;i++)
        {
           propList.data[i] = property.propList.data[i];
        }
    }

    template <typename T>
    void NodeProperty<T>::deleteHistory()
    {
      delete [] history_data;
    }

    template <typename T>
    bool NodeProperty<T>::aggregateValue(Aggregate_Op op)
    {
      bool value = true;
      if(op == NOT)
      {
        for(int i=0;i<length;i++)
          value &= !propList.data[i];  
      }
      else if (op == NO_OP)
      {
        for(int i=0;i<length;i++)
          value &= propList.data[i];
      }
      else
      {
          assert(false); // other type of aggregations on properties not yet supported
      }

      bool aggregate;
      MPI_Allreduce(&value,&aggregate,1,MPI_CXX_BOOL, MPI_LAND, world);

      return aggregate;
    }

    template <typename T>
    void NodeProperty<T>::getExclusiveLock(int node_id)
    {
        int owner_proc = graph->get_node_owner(node_id);

        already_locked_processors[owner_proc] = true;
        propList.get_lock(owner_proc,EXCLUSIVE_LOCK);

    }

    template <typename T>
    void NodeProperty<T>::releaseExclusiveLock(int node_id)
    {
        int owner_proc = graph->get_node_owner(node_id);

        propList.unlock(owner_proc,EXCLUSIVE_LOCK);
        already_locked_processors[owner_proc] = false;
    }

    template <typename T>
    T NodeProperty<T>::compareAndSwap(int node_id, T compare, T swap)
    { 
        int owner_proc = graph->get_node_owner(node_id);
        int local_node_id = graph->get_node_local_index(node_id);
        T oldValue;
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK);
        propList.compareAndSwap(owner_proc,local_node_id,&compare, &swap, &oldValue);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
            
      return oldValue;
    }

    template <typename T>
    void NodeProperty<T>::initialize_reduction_queue()
    {
      reduction_queue.clear();
      reduction_queue.resize(world.size());
    }

    template <typename T>
    void NodeProperty<T>::add_to_reduction_queue(int reduction_node,std::pair<int32_t, T> pair)
    {
      reduction_queue[graph->get_node_owner(reduction_node)].push_back(pair);
    }

    template <typename T>
    std::vector<std::vector<int32_t>> NodeProperty<T>::perform_reduction(MPI_Op op)
    {
      boost::mpi::all_to_all(world, reduction_queue, reduction_queue);
      std::unordered_map<int32_t, std::pair<int32_t,int32_t>> reductions_taken;

      propList.get_lock(world.rank(),EXCLUSIVE_LOCK);
      
      for(int i=0;i<world.size();i++)
      {
        for(int j=0;j<(int)reduction_queue[i].size();j++)
        { 
          std::pair<int32_t, T> pair = reduction_queue[i][j];
          
          if(op == MPI_MIN)
          {
            if(propList.data[graph->get_node_local_index(pair.first)] > pair.second)
            {  
              propList.data[graph->get_node_local_index(pair.first)] = pair.second;
              reductions_taken[pair.first] = std::make_pair(i,j);
            }  
          }
          else{
            std::cerr<<"Not Implemented"<<std::endl;
            exit(-1);
          }

        }
      }
      propList.unlock(world.rank(),EXCLUSIVE_LOCK);

      std::vector<std::vector<int32_t>> modified_ids(world.size());
      for(auto key_value_pair : reductions_taken)
      {
        std::pair<int32_t,int32_t> value = key_value_pair.second;
        modified_ids[value.first].push_back(value.second);
      }
      boost::mpi::all_to_all(world, modified_ids, modified_ids);
      return modified_ids;
    }

    template <typename T>
    void NodeProperty<T>::assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids)
    {
      std::vector<std::vector<std::pair<int32_t,T>>> reductions_taken(world.size());
      for(int i=0;i<(int)modified_ids.size();i++)
      {
        for(int j : modified_ids[i])
        {  
          std::pair<int32_t,T> p = reduction_queue[i][j]; 
          reductions_taken[graph->get_node_owner(p.first)].push_back(p);
        }
      }
      
      boost::mpi::all_to_all(world, reductions_taken, reductions_taken);
      propList.get_lock(world.rank(),EXCLUSIVE_LOCK);
      for(int i=0;i<(int)world.size();i++)
      {
        for(std::pair<int32_t,T> pair : reductions_taken[i])
          propList.data[graph->get_node_local_index(pair.first)] = pair.second;
      }
      propList.unlock(world.rank(),EXCLUSIVE_LOCK);
    }
    template <typename T>
    void NodeProperty<T>::atomicAdd(int node_id, T value)
    {
        int owner_proc = graph->get_node_owner(node_id);
        int local_node_id = graph->get_node_local_index(node_id);

        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK);
        propList.accumulate(owner_proc,&value,local_node_id,1,MPI_SUM,SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
            
    }

    template <typename T>
    void EdgeProperty<T>::attachToGraph(Graph * graph, T initial_value)
    { 
        // This means this property was already assigned and the window is already created in this case we just
        // reinitialise all the values to inital_value
        if(attached_to_graph == true)
        {
            for(int i=0;i<length;i++)
              propList.data[i] = initial_value;

            return;
        }
        attached_to_graph =true;

        propertyId = graph->properties_counter;
        graph->properties[propertyId] = (Property*)this;
        graph->properties_counter = graph->properties_counter + 1;

        

        length = graph->edgeProcMap[world.rank() + 1] - graph->edgeProcMap[world.rank()];

        T * data = new T [length];
        for(int i=0;i<length;i++)
          data[i]=initial_value;
  
        world = graph->world;
        
        propList.create_window(data, length, sizeof(T), world);

        already_locked_processors = std::vector<bool>(world.size(), false);
        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    void EdgeProperty<T>::attachToGraph(Graph * graph, T * initial_values)
    { 
        // This means this property was already assigned and the window is already created in this case we just
        // reinitialise all the values to inital_value
        if(attached_to_graph == true)
        {
            for(int i=0;i<length;i++)
              propList.data[i] = initial_values[i];

            return;
        }
        attached_to_graph =true;
      
        propertyId = graph->properties_counter;
        graph->properties[propertyId] = (Property*)this;
        graph->properties_counter = graph->properties_counter + 1;

        

        length = graph->edgeProcMap[world.rank() + 1] - graph->edgeProcMap[world.rank()];
        
        T * data = new T [length];
        for(int i=0;i<length;i++)
          data[i]=initial_values[i];

        world = graph->world;
        
        propList.create_window(data, length, sizeof(T), world);
        already_locked_processors = std::vector<bool>(world.size(), false);

        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    int32_t EdgeProperty<T>::getPropertyId() {return propertyId;}

    template <typename T>
    T EdgeProperty<T>::getValue(int edge_id ,bool check_concurrency)
    {
      int owner_proc = graph->get_edge_owner(edge_id);
      int local_edge_id = graph->get_edge_local_index(edge_id);
      
      bool no_checks_needed = !check_concurrency;

      if(owner_proc==world.rank())
      {
        //if(!already_locked_processors[owner_proc])
        //  propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        T value = propList.data[local_edge_id];
        //if(!already_locked_processors[owner_proc])
        //  propList.unlock(owner_proc, SHARED_LOCK);
        return value;
      }
      else
      {
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        T* data = propList.get_data(owner_proc, local_edge_id, 1, SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
        return data[0];
      }
      
    }

    template <typename T>
    void EdgeProperty<T>::setValue(int edge_id ,T value, bool check_concurrency)
    {
      
      int owner_proc = graph->get_edge_owner(edge_id);
      int local_edge_id = graph->get_edge_local_index(edge_id);

      bool no_checks_needed = !check_concurrency;

      //if(owner_proc==world.rank())
      //{
      //  if(!already_locked_processors[owner_proc])
      //    propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
      //  propList.data[local_edge_id] = value;
      //  if(!already_locked_processors[owner_proc])
      //    propList.unlock(owner_proc, SHARED_LOCK);
      //}
      //else
     // {
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        propList.put_data(owner_proc,&value, local_edge_id, 1, SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
     // }
      
      return ;
    }

    template <typename T>
    void EdgeProperty<T>::remeberHistory()
    {
        history_data = new T [length];
        propList.get_lock(world.rank(),SHARED_LOCK);
        for(int i=0;i<length;i++)
        {
            history_data[i] = propList.data[i];
        }

        propList.unlock(world.rank(), SHARED_LOCK);
    }

    template <typename T>
    T EdgeProperty<T>::getHistoryValue(int edge_id)
    {
        int owner_proc = graph->get_edge_owner(edge_id);
        int local_edge_id = graph->get_edge_local_index(edge_id);

        // If it is required that we need history value from a different processor remove this
        // and instead make a separate window(separate RMA_Datatype variable) for copied data and
        // use MPI_GET like normal
        assert(owner_proc == world.rank());

        return history_data[local_edge_id];
    }

    template <typename T>
    void EdgeProperty<T>::assignCopy(EdgeProperty<T>& property)
    {
        for(int i=0;i<length;i++)
        {
           propList.data[i] = property.propList.data[i];
        }
    }

    template <typename T>
    void EdgeProperty<T>::deleteHistory()
    {
      delete [] history_data;
    }

    template <typename T>
    bool EdgeProperty<T>::aggregateValue(Aggregate_Op op)
    {
      bool value = true;
      if(op == NOT)
      {
        for(int i=0;i<length;i++)
          value &= !propList.data[i];  
      }
      else if (op == NO_OP)
      {
        for(int i=0;i<length;i++)
          value &= propList.data[i];
      }
      else
      {
          assert(false); // other type of aggregations on properties not yet supported
      }

      bool aggregate;
      MPI_Allreduce(&value,&aggregate,1,MPI_CXX_BOOL, MPI_LAND, MPI_COMM_WORLD);

      return aggregate;
    }

    template <typename T>
    void EdgeProperty<T>::getExclusiveLock(int edge_id)
    {
        int owner_proc = graph->get_edge_owner(edge_id);
        already_locked_processors[owner_proc] = true;
        propList.get_lock(owner_proc,EXCLUSIVE_LOCK);

    }

    template <typename T>
    void EdgeProperty<T>::releaseExclusiveLock(int edge_id)
    {
        int owner_proc = graph->get_edge_owner(edge_id);
      
        propList.unlock(owner_proc,EXCLUSIVE_LOCK);
        already_locked_processors[owner_proc] = false;
    }

    template <typename T>
    T EdgeProperty<T>::compareAndSwap(int edge_id, T compare, T swap)
    { 
        int owner_proc = graph->get_edge_owner(edge_id);
        int local_edge_id = graph->get_node_local_index(edge_id);

        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK);
        T oldValue = propList.compareAndSwap(owner_proc,local_edge_id,compare, swap);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
            
      return oldValue;
    }

    template <typename T>
    void EdgeProperty<T>::initialize_reduction_queue()
    {
      reduction_queue.clear();
      reduction_queue.resize(world.size());
    }

    template <typename T>
    void EdgeProperty<T>::add_to_reduction_queue(int reduction_node,std::pair<int32_t, T> pair)
    {
      reduction_queue[graph->get_edge_owner(reduction_node)].push_back(pair);
    }

    template <typename T>
    std::vector<std::vector<int32_t>> EdgeProperty<T>::perform_reduction(MPI_Op op)
    {
      boost::mpi::all_to_all(world, reduction_queue, reduction_queue);
      std::unordered_map<int32_t, std::pair<int32_t,int32_t>> reductions_taken;

      propList.get_lock(world.rank(),EXCLUSIVE_LOCK);
      
      for(int i=0;i<world.size();i++)
      {
        for(int j=0;j<(int)reduction_queue[i].size();j++)
        { 
          std::pair<int32_t, T> pair = reduction_queue[i][j];
          
          if(op == MPI_MIN)
          {
            if(propList.data[graph->get_edge_local_index(pair.first)] > pair.second)
            {  
              propList.data[graph->get_edge_local_index(pair.first)] = pair.second;
              reductions_taken[pair.first] = std::make_pair(i,j);
            }  
          }
          else{
            std::cerr<<"Not Implemented"<<std::endl;
            exit(-1);
          }

        }
      }
      propList.unlock(world.rank(),EXCLUSIVE_LOCK);

      std::vector<std::vector<int32_t>> modified_ids(world.size());
      for(auto key_value_pair : reductions_taken)
      {
        std::pair<int32_t,int32_t> value = key_value_pair.second;
        modified_ids[value.first].push_back(value.second);
      }
      boost::mpi::all_to_all(world, modified_ids, modified_ids);
      return modified_ids;
    }

    template <typename T>
    void EdgeProperty<T>::assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids)
    {
      std::vector<std::vector<std::pair<int32_t,T>>> reductions_taken(world.size());
      for(int i=0;i<(int)modified_ids.size();i++)
      {
        for(int j : modified_ids[i])
        {  
          std::pair<int32_t,T> p = reduction_queue[i][j]; 
          reductions_taken[graph->get_edge_owner(p.first)].push_back(p);
        }
      }
      
      boost::mpi::all_to_all(world, reductions_taken, reductions_taken);
      propList.get_lock(world.rank(),EXCLUSIVE_LOCK);
      for(int i=0;i<(int)world.size();i++)
      {
        for(std::pair<int32_t,T> pair : reductions_taken[i])
          propList.data[graph->get_edge_local_index(pair.first)] = pair.second;
      }
      propList.unlock(world.rank(),EXCLUSIVE_LOCK);
    }

    template <typename T>
    void EdgeProperty<T>::atomicAdd(int edge_id, T value)
    {
        int owner_proc = graph->get_edge_owner(edge_id);
        int local_edge_id = graph->get_edge_local_index(edge_id);
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK);
        propList.accumulate(owner_proc,&value,local_edge_id,1,MPI_SUM,SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
            
    }
#endif
#include"graph_mpi.h"
#include"rma_datatype/rma_datatype.h"
#include<iostream>
#include <fstream>
#include <sstream>

  void Graph::print_csr()
  {
    get_lock_for_reduction_statement();
    for(int i=start_node();i<=end_node();i++)
    {
        std::cout<<"node "<<i<<std::endl;
        std::cout<<"csr neighbors: ";
        for(int j=indexofNodes.data[get_node_local_index(i)]; j < indexofNodes.data[get_node_local_index(i)+1];j++)
        {
          std::cout<<destList.data[j]<<" ";
        }std::cout<<std::endl;
        std::cout<<"csr weights: ";
        for(int j=indexofNodes.data[get_node_local_index(i)]; j < indexofNodes.data[get_node_local_index(i)+1];j++)
        {
          std::cout<<weights.propList.data[j]<<" ";
        }std::cout<<std::endl;
        std::cout<<"rev csr neighbors: ";
        for(int j=rev_indexofNodes.data[get_node_local_index(i)]; j < rev_indexofNodes.data[get_node_local_index(i)+1];j++)
        {
          std::cout<<srcList.data[j]<<" ";
        }std::cout<<std::endl;

        if(diff_csr_created)
        { std::cout<<"diff csr neighbors: ";
        for(int j=diff_indexofNodes.data[get_node_local_index(i)]; j < diff_indexofNodes.data[get_node_local_index(i)+1];j++)
        {
          std::cout<<diff_destList.data[j]<<" ";
        }std::cout<<std::endl;
        std::cout<<"diff csr weights: ";
        for(int j=diff_indexofNodes.data[get_node_local_index(i)]; j < diff_indexofNodes.data[get_node_local_index(i)+1];j++)
        {
          std::cout<<weights.diff_propList.data[j]<<" ";
        }std::cout<<std::endl;
        }
        if(rev_diff_csr_created)
        {
        std::cout<<"diff rev csr neighbors: ";
        for(int j=diff_rev_indexofNodes.data[get_node_local_index(i)]; j < diff_rev_indexofNodes.data[get_node_local_index(i)+1];j++)
        {
          std::cout<<diff_srcList.data[j]<<" ";
        }std::cout<<std::endl;
        
        }std::cout<<std::endl;
    }
    
    
    unlock_for_reduction_statement();
  }
   void Graph::create_bfs_dag(int src)
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
  int Graph::num_bfs_phases()
  {
    return bfs_phases;
  }
  std::vector<int32_t>& Graph::get_bfs_nodes_for_phase(int phase)
  {
    return bfs_level_nodes[phase];
  }
  std::vector<int32_t>& Graph::get_bfs_children(int node)
  {
    if(get_node_owner(node) != world.rank())
    {
      std::cerr<<"Should not reach here"<<std::endl;
      exit(-1);
    }
    return bfs_children[get_node_local_index(node)];
  }

  
  Graph::Graph(char* file, boost::mpi::communicator world , bool undirected)
  {
    this->undirected = undirected;
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
    src.clear();dest.clear();weight.clear();sizes.clear();offsets.clear();
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
    
    std::vector<int32_t> destList, srcList, weightList;
    

    for (int i = 0; i < scatter_size; i++)
    {
      int proc_num = localsrc[i] / nodesPartitionSize;
      adjacency_list_3d[proc_num][localsrc[i] % nodesPartitionSize].push_back((localdest[i]));
    }
    boost::mpi::all_to_all(world, adjacency_list_3d, adjacency_list_3d);
    int32_t * indexofNodes = new int32_t[nodesPartitionSize+1]; indexofNodes[0]=0; 
    for (int i = 0; i < nodesPartitionSize; i++)
    {
      std::vector<int32_t> temp1;
      for (int j = 0; j < world.size(); j++)
      {
        for(int k=0;k<(int)adjacency_list_3d[j][i].size();k++)
        {
          temp1.push_back(adjacency_list_3d[j][i][k]);
        }
      }
      indexofNodes[i+1] = indexofNodes[i] + temp1.size();
      
      for(int k=0;k<(int)temp1.size();k++)
      {
        destList.push_back(temp1[k]);
      }
	  }
    adjacency_list_3d.clear();



    for (int i = 0; i < scatter_size; i++)
    {
      int proc_num = localsrc[i] / nodesPartitionSize;
      weight_list_3d[proc_num][localsrc[i] % nodesPartitionSize].push_back(localweight[i]);
    }
    delete[] localweight;
    boost::mpi::all_to_all(world, weight_list_3d, weight_list_3d);
    for (int i = 0; i < nodesPartitionSize; i++)
    {
      std::vector<int32_t> temp1;
      for (int j = 0; j < world.size(); j++)
      {
        for(int k=0;k<(int)weight_list_3d[j][i].size();k++)
        {
          temp1.push_back(weight_list_3d[j][i][k]);
        }
      }

      for(int k=0;k<(int)temp1.size();k++)
      {
        weightList.push_back(temp1[k]);
      }
	  }
    weight_list_3d.clear();



    for (int i = 0; i < scatter_size; i++)
    {
      int proc_num = localdest[i] / nodesPartitionSize;
      rev_adjacency_list_3d[proc_num][localdest[i] % nodesPartitionSize].push_back((localsrc[i]));
    }
    delete[] localsrc ;
    delete[] localdest;
    boost::mpi::all_to_all(world, rev_adjacency_list_3d, rev_adjacency_list_3d); 
    int32_t * rev_indexofNodes = new int32_t[nodesPartitionSize+1];
    rev_indexofNodes[0]=0;    
    for (int i = 0; i < nodesPartitionSize; i++)
    {
      std::vector<int32_t> temp2;
      for (int j = 0; j < world.size(); j++)
      {
        temp2.insert(temp2.end(), rev_adjacency_list_3d[j][i].begin(), rev_adjacency_list_3d[j][i].end());
      }
      rev_indexofNodes[i+1] = rev_indexofNodes[i] + temp2.size();
      srcList.insert(srcList.end(), temp2.begin(), temp2.end());
	  }
    rev_adjacency_list_3d.clear();

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

    this->diff_indexofNodes.mpi_datatype = MPI_INT32_T;
    this->diff_rev_indexofNodes.mpi_datatype = MPI_INT32_T;
    this->diff_destList.mpi_datatype = MPI_INT32_T;
    this->diff_srcList.mpi_datatype = MPI_INT32_T;

    diff_csr_created = false;
    rev_diff_csr_created = false;
    
    weights.attachToGraph(this, weightList.data());

    MPI_Win_allocate(sizeof(int),  sizeof(int) , MPI_INFO_NULL, world, &reduction_lock,&reduction_window);
    if(world.rank()==0)
      *reduction_lock = 0;
    world.barrier();
    
    this->perNodeCSRSpace.mpi_datatype = MPI_INT32_T;
    this->perNodeRevCSRSpace.mpi_datatype = MPI_INT32_T;
    this->perNodeDiffCSRSpace.mpi_datatype = MPI_INT32_T;
    this->perNodeDiffRevCSRSpace.mpi_datatype = MPI_INT32_T;
    
    std::vector<int32_t> temp = std::vector<int32_t>(nodesPartitionSize,0);
    this->perNodeCSRSpace.create_window(temp.data(),nodesPartitionSize,sizeof(int32_t),world);
    this->perNodeRevCSRSpace.create_window(temp.data(),nodesPartitionSize,sizeof(int32_t),world);
    this->perNodeDiffCSRSpace.create_window(temp.data(),nodesPartitionSize,sizeof(int32_t),world);
    this->perNodeDiffRevCSRSpace.create_window(temp.data(),nodesPartitionSize,sizeof(int32_t),world);
    
    
  }
 
  void Graph::initialise_reduction(MPI_Op op, Property* reduction_property, std::vector<Property*> other_properties)
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



  void Graph::sync_reduction()
  {
      std::vector<std::vector<int32_t>> modifed_ids = properties[reduction_property_id]->perform_reduction(reduction_op);
      
      for(int id :other_reduction_properties_id)
      {
        properties[id]->assign_reduction_values(modifed_ids);
      }
  }


  void Graph::get_lock_for_reduction_statement()
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
  void Graph::unlock_for_reduction_statement()
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

  int Graph::num_nodes()
  {
    return nodesTotal;
  }

  int Graph::num_edges()
  {
    return edgesTotal;
  }

  int Graph::start_node()
  {
    return startNode;
  }

  int Graph::end_node()
  {
    return endNode;
  }

  Edge Graph::start_edge()
  {
    return Edge(true,edgeProcMap[world.rank()]);
  }
  Edge Graph::end_edge()
  {
    if(!diff_csr_created)
      return Edge(true,edgeProcMap[world.rank()+1]-1);
    else
      return Edge(false,diff_edgeProcMap[world.rank()+1]-1);
  }
  Edge Graph::next_edge(Edge & edge)
  {
    if(edge.is_in_csr())
    {
      if(edge.get_id()==edgeProcMap[world.rank()+1]-1)
        return Edge(false,diff_edgeProcMap[world.rank()]);
      else
        return Edge(true, edge.get_id()+1);  
    }
    else
        return Edge(false, edge.get_id()+1);
  }

  int Graph::num_nodes_owned()
  {
    return endNode - startNode +1;
  }

  int Graph::num_out_nbrs(int node)
  {   
      int owner_proc = get_node_owner(node);
      int index = get_node_local_index(node);
      int count =0;
      if(owner_proc==world.rank())
      {
        count = indexofNodes.data[index+1] - indexofNodes.data[index] - perNodeCSRSpace.data[index];    
      }
      else {
        indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
        int * destListIndices = indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
        indexofNodes.unlock(owner_proc, SHARED_LOCK);

        perNodeCSRSpace.get_lock(owner_proc, SHARED_LOCK, false);
        int * emptySpace = perNodeCSRSpace.get_data(owner_proc, index, 1,SHARED_LOCK);
        perNodeCSRSpace.unlock(owner_proc, SHARED_LOCK);

        count = destListIndices[1] - destListIndices[0] - emptySpace[0];
      }
      
      if(diff_csr_created)
      {
        if(owner_proc==world.rank())
        {
          count += diff_indexofNodes.data[index+1] - diff_indexofNodes.data[index] - perNodeDiffCSRSpace.data[index]; 
        }
        else {
          diff_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
          int * diff_destListIndices = diff_indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
          diff_indexofNodes.unlock(owner_proc, SHARED_LOCK);

          perNodeDiffCSRSpace.get_lock(owner_proc, SHARED_LOCK, false);
          int * diff_emptySpace = perNodeDiffCSRSpace.get_data(owner_proc, index, 1,SHARED_LOCK);
          perNodeDiffCSRSpace.unlock(owner_proc, SHARED_LOCK);


          count += diff_destListIndices[1] - diff_destListIndices[0] - diff_emptySpace[0];
        } 
      }
      return count;
  }

  int Graph::get_node_owner(int node)
  {
    return node/nodesPartitionSize;
  }

  int  Graph::get_node_local_index(int node)
  {
    return node%nodesPartitionSize;
  }

  int Graph::get_edge_owner(Edge edge)
  {
      int owner = 0;
      if(edge.is_in_csr())
      {
        for(int i=0;i<world.size();i++)
        {
          if(edgeProcMap[i] <= edge.get_id())
            owner = i;
          else 
            break;  
        }
      }
      else
      {
        for(int i=0;i<world.size();i++)
        {
          if(diff_edgeProcMap[i] <= edge.get_id())
            owner = i;
          else 
            break;  
        }
      }  

      return owner;
  }

  int Graph::get_edge_local_index(Edge edge)
  {
    if(edge.is_in_csr())
      return edge.get_id() - edgeProcMap[get_edge_owner(edge)];
    else
      return edge.get_id() - diff_edgeProcMap[get_edge_owner(edge)];  
  }

  Edge Graph::get_edge(int v, int nbr)
  {
    int owner_proc = get_node_owner(v);
    int index = get_node_local_index(v);

    if(world.rank()==owner_proc)
    {    
     int start = indexofNodes.data[index], end = indexofNodes.data[index+1];
     for(int i=start ; i<end ; i++)
     {
        if(destList.data[i]==nbr)
        {
          return Edge(true,edgeProcMap[owner_proc] + i);
        }
     } 
     if(diff_csr_created)
     {
        int diff_start = diff_indexofNodes.data[index], diff_end = diff_indexofNodes.data[index+1];
        for(int i=diff_start ; i<diff_end ; i++)
        {
          if(diff_destList.data[i]==nbr)
          {
            return Edge(false,diff_edgeProcMap[owner_proc] + i);
          }
        }  
     }
     std::cerr<<"invalid edge asked in get_edge: src "<<v<<" , nbr "<<nbr<<"\n";
     exit(-1);
    }
    else
    {
      Edge e(true, -1); 

      indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int * destListIndices = indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
      indexofNodes.unlock(owner_proc, SHARED_LOCK);
      int start = destListIndices[0], end = destListIndices[1];

      destList.get_lock(owner_proc, SHARED_LOCK, false);
      int32_t * edges = destList.get_data(owner_proc, start, end - start,SHARED_LOCK); 
      destList.unlock(owner_proc, SHARED_LOCK);

      for(int i=start ; i<end ; i++)
      {
        if(edges[i-start]==nbr)
        {
          e = Edge(true,edgeProcMap[owner_proc] + i);
          break;
        }
      }
      if(e.get_id()!=-1)
      {
        delete [] destListIndices;
        delete [] edges;
        return e;   
      } 
     if(diff_csr_created)
     {
        diff_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
        int * diff_destListIndices = diff_indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
        diff_indexofNodes.unlock(owner_proc, SHARED_LOCK);
        int diff_start = diff_destListIndices[0], diff_end = diff_destListIndices[1];

        diff_destList.get_lock(owner_proc, SHARED_LOCK, false);
        int32_t * diff_edges = diff_destList.get_data(owner_proc, diff_start, diff_end - diff_start,SHARED_LOCK); 
        diff_destList.unlock(owner_proc, SHARED_LOCK);

        for(int i=diff_start ; i<diff_end ; i++)
        {
          if(diff_edges[i-diff_start]==nbr)
          {
            e = Edge(false,diff_edgeProcMap[owner_proc] + i);
            break;
          }
        }
        if(e.get_id()!=-1)
        { 
          delete [] diff_destListIndices;
          delete [] diff_edges;
          return e;   
        }

     }
     std::cerr<<"invalid edge asked in get_edge: src "<<v<<" , nbr "<<nbr<<"\n";
     exit(-1);
    }
    
     
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
  
  bool Graph::check_if_nbr(int s, int d)
  {
    for ( int32_t e: getNeighbors(s))
    {
      if(e== d)
        return true;
    }
    return false;
  }

  std::vector<int32_t> Graph::getNeighbors( int node)
  {
    
    int owner_proc = get_node_owner(node);
    int index = get_node_local_index(node);
    std::vector<int32_t> out_edges;
    if(world.rank()== owner_proc)
    {
      //indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int start = indexofNodes.data[index], end = indexofNodes.data[index+1];
      //indexofNodes.unlock(owner_proc, SHARED_LOCK); 
      
      //destList.get_lock(owner_proc, SHARED_LOCK, false);
      for(int i=start ; i<end ; i++)
      {
        if(destList.data[i]!=INT_MAX/2)
        {
          out_edges.push_back(destList.data[i]);
        }
     }
     //destList.unlock(owner_proc, SHARED_LOCK);

     if(diff_csr_created)
     {
        int diff_start = diff_indexofNodes.data[index], diff_end = diff_indexofNodes.data[index+1];
        for(int i=diff_start ; i<diff_end ; i++)
        {
          if(diff_destList.data[i]!=INT_MAX/2)
          {
            out_edges.push_back(diff_destList.data[i]);
          }
        }  
     }
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

      for(int i=0;i<end-start;i++)
      {
        if(edges[i]!=INT_MAX/2)
          out_edges.push_back(edges[i]);
      }
      delete [] destListIndices;
      delete [] edges;
      if(diff_csr_created)
      {
        
        diff_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
        int * diff_destListIndices = diff_indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
        diff_indexofNodes.unlock(owner_proc, SHARED_LOCK);
        int diff_start = diff_destListIndices[0], diff_end = diff_destListIndices[1];

        diff_destList.get_lock(owner_proc, SHARED_LOCK, false);
        int32_t * diff_edges = diff_destList.get_data(owner_proc, diff_start, diff_end - diff_start,SHARED_LOCK); 
        diff_destList.unlock(owner_proc, SHARED_LOCK);

        for(int i=0;i<diff_end-diff_start;i++)
        {
          if(diff_edges[i]!=INT_MAX/2)
            out_edges.push_back(diff_edges[i]);
        }
        delete [] diff_destListIndices;
        delete [] diff_edges;
      }  
    }

    return out_edges;
  }

  std::vector<int32_t> Graph::getInNeighbors( int node)
  {
    int owner_proc = get_node_owner(node);
    int index = get_node_local_index(node);

    std::vector<int32_t> in_edges;
    if(world.rank()== owner_proc)
    {
      //rev_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int start = rev_indexofNodes.data[index], end = rev_indexofNodes.data[index+1];
      //rev_indexofNodes.unlock(owner_proc, SHARED_LOCK);

      //srcList.get_lock(owner_proc, SHARED_LOCK, false);
      for(int i=start ; i<end ; i++)
      {
        if(srcList.data[i]!=INT_MAX/2)
        {
          in_edges.push_back(srcList.data[i]);
        }
      }
      //srcList.unlock(owner_proc, SHARED_LOCK);

      if(rev_diff_csr_created)
      {
        int diff_start = diff_rev_indexofNodes.data[index], diff_end = diff_rev_indexofNodes.data[index+1];
        for(int i=diff_start ; i<diff_end ; i++)
        {
          if(diff_srcList.data[i]!=INT_MAX/2)
          {
            in_edges.push_back(diff_srcList.data[i]);
          }
        }  
     }
    }
    else 
    {
      rev_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
      int * srcListIndices = rev_indexofNodes.get_data(owner_proc, index, 2, SHARED_LOCK);
      rev_indexofNodes.unlock(owner_proc, SHARED_LOCK);
      int start = srcListIndices[0], end = srcListIndices[1];

      srcList.get_lock(owner_proc, SHARED_LOCK, false);
      int32_t * edges = srcList.get_data(owner_proc, start, end - start ,SHARED_LOCK); 
      srcList.unlock(owner_proc, SHARED_LOCK);
      
      
      for(int i=0;i<end-start;i++)
      {
        if(edges[i]!=INT_MAX/2)
          in_edges.push_back(edges[i]);
      }
      delete [] srcListIndices;
      delete [] edges;
      if(rev_diff_csr_created)
      {
        diff_rev_indexofNodes.get_lock(owner_proc, SHARED_LOCK, false);
        int * diff_srcListIndices = diff_rev_indexofNodes.get_data(owner_proc, index, 2,SHARED_LOCK);
        diff_rev_indexofNodes.unlock(owner_proc, SHARED_LOCK);
        int diff_start = diff_srcListIndices[0], diff_end = diff_srcListIndices[1];

        diff_srcList.get_lock(owner_proc, SHARED_LOCK, false);
        int32_t * diff_edges = diff_srcList.get_data(owner_proc, diff_start, diff_end - diff_start,SHARED_LOCK); 
        diff_srcList.unlock(owner_proc, SHARED_LOCK);

        for(int i=0;i<diff_end-diff_start;i++)
        {
          if(diff_edges[i]!=INT_MAX/2)
            in_edges.push_back(diff_edges[i]);
        }
        delete [] diff_srcListIndices;
        delete [] diff_edges;
      }
    }

    return in_edges;
  }

  void Graph::readFromFile(std::string filePath, int32_t &num_nodes, int32_t &num_edges, std::vector<int32_t> & src, std::vector<int32_t> & dest , std::vector<int32_t> & weights)
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



  void Graph::addEdges_Csr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates)
  { 
      std::vector<std::vector<std::pair<int32_t,int32_t>>> src_separated_updates(nodesPartitionSize);
      for(std::pair<int32_t,std::pair<int32_t,int32_t>> p : updates)
      {
         int src = p.first;
         int dest = p.second.first;
         int weight = p.second.second;
         int owner_proc = get_node_owner(src);
         int index = get_node_local_index(src);
         assert(owner_proc==world.rank());

         src_separated_updates[index].push_back(std::make_pair(dest,weight));
      }
      
      int total_new_diff_csr_space = 0;
      bool need_new_diff_csr = false;
      std::vector<int32_t> newDiffCsrSpaceRequired(nodesPartitionSize,0);
      for(int i=0;i<nodesPartitionSize;i++)
      {
        int space = perNodeCSRSpace.data[i];
        if(diff_csr_created)
        {
          space+= perNodeDiffCSRSpace.data[i];
          
          newDiffCsrSpaceRequired[i]= diff_indexofNodes.data[i+1] - diff_indexofNodes.data[i] - perNodeDiffCSRSpace.data[i];
        }
        if(space < (int)src_separated_updates[i].size())
        {  
          need_new_diff_csr = true;
          newDiffCsrSpaceRequired[i] += src_separated_updates[i].size() - perNodeCSRSpace.data[i] ;
        }
      }
      
      bool need_new_diff_csr_local = need_new_diff_csr;
      MPI_Allreduce(&need_new_diff_csr_local,&need_new_diff_csr,1,MPI_CXX_BOOL,MPI_LOR, MPI_COMM_WORLD);


      if(need_new_diff_csr)
      { 
        std::vector<int32_t> new_diff_indexOfNodes(nodesPartitionSize+1);
        new_diff_indexOfNodes[0]=0;
        for(int i=1;i<nodesPartitionSize+1;i++)
        {
            new_diff_indexOfNodes[i]=new_diff_indexOfNodes[i-1]+newDiffCsrSpaceRequired[i-1];
        }
        total_new_diff_csr_space = new_diff_indexOfNodes[nodesPartitionSize]; 
        
        int32_t * new_diff_csr = new int32_t[total_new_diff_csr_space];
        int32_t * new_diff_weights = new int32_t[total_new_diff_csr_space];  

        int32_t * diff_destListSizes = new int32_t [world.size()];
        MPI_Allgather(&total_new_diff_csr_space,1,MPI_INT32_T,diff_destListSizes,1,MPI_INT32_T, MPI_COMM_WORLD);

        if(!diff_csr_created)
          diff_edgeProcMap = std::vector<int>(nodesPartitionSize+1);
        diff_edgeProcMap[0]=0;
        for(int i=0;i<world.size();i++)
        {
          diff_edgeProcMap[i+1]=diff_edgeProcMap[i]+diff_destListSizes[i];
        }
        delete [] diff_destListSizes;

        
        if(diff_csr_created)
        {
          int index = 0;

          for(int i=0;i<nodesPartitionSize;i++)
          {
            for(int j=diff_indexofNodes.data[i];j<diff_indexofNodes.data[i+1];j++)
            {
              if(diff_destList.data[j]!=INT_MAX/2)
              {
                new_diff_csr[index]=diff_destList.data[j];
                new_diff_weights[index] = weights.diff_propList.data[j];
                index++;
              }   
            }
            index = new_diff_indexOfNodes[i+1];
          }

          for(int i=0;i<nodesPartitionSize;i++)
          {
            int index = indexofNodes.data[i]; 
            int diff_index = new_diff_indexOfNodes[i] + (diff_indexofNodes.data[i+1] - diff_indexofNodes.data[i]-perNodeDiffCSRSpace.data[i]);
            for(std::pair<int32_t,int32_t> p : src_separated_updates[i])
            { 
                if(perNodeCSRSpace.data[i]>0)
                {
                  while(destList.data[index]!=INT_MAX/2){index++;}
                  destList.data[index]= p.first;
                  weights.propList.data[index] = p.second;
                  index++;
                  perNodeCSRSpace.data[i]--;
                }
                else
                {
                  new_diff_csr[diff_index] = p.first;
                  new_diff_weights[diff_index] = p.second;
                  diff_index++;
                }
            }
          }

        }
        else{
          diff_csr_created = true;

          for(int i=0;i<nodesPartitionSize;i++)
          {
            int index = indexofNodes.data[i];
            int diff_index = new_diff_indexOfNodes[i];
            for(std::pair<int32_t,int32_t> p : src_separated_updates[i])
            { 
                if(perNodeCSRSpace.data[i]>0)
                {
                  while(destList.data[index]!=INT_MAX/2){index++;}
                  destList.data[index]= p.first;
                  weights.propList.data[index] = p.second;
                  index++;
                  perNodeCSRSpace.data[i]--;
                }
                else
                {
                  new_diff_csr[diff_index] = p.first;
                  new_diff_weights[diff_index] = p.second;
                  diff_index++;
                }
            }
          }

        }
          diff_indexofNodes.create_window(new_diff_indexOfNodes.data(),nodesPartitionSize+1,sizeof(int32_t),world);
          diff_destList.create_window(new_diff_csr,total_new_diff_csr_space, sizeof(int32_t), world);
          weights.attachToGraph(this,NULL,new_diff_weights,true, true);
          
          for(int i=0;i<nodesPartitionSize;i++)
          {
            perNodeDiffCSRSpace.data[i]=0;
          }
      }
      else
      {   
        for(int i=0;i<nodesPartitionSize;i++)
          {
            int index = indexofNodes.data[i];
            int diff_index = -1;
            if(diff_csr_created)
              diff_index = diff_indexofNodes.data[i] ;
            for(std::pair<int32_t,int32_t> p : src_separated_updates[i])
            { 
                if(perNodeCSRSpace.data[i]>0)
                {
                  while(destList.data[index]!=INT_MAX/2){index++;}
                  destList.data[index]= p.first;
                  weights.propList.data[index] = p.second;
                  index++;
                  perNodeCSRSpace.data[i]--;
                }
                else
                {
                  if(!diff_csr_created)
                    assert(false);
                    
                  while(diff_destList.data[diff_index]!=INT_MAX/2){diff_index++;}
                  diff_destList.data[diff_index]= p.first;
                  weights.diff_propList.data[diff_index] = p.second;
                  diff_index++;
                  perNodeDiffCSRSpace.data[i]--;
                }
            }
          }
      }

  }

  void Graph::delEdges_Csr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates)
  {
      for(std::pair<int32_t,std::pair<int32_t,int32_t>> p : updates)
      {
         int src = p.first;
         int dest = p.second.first;
         int owner_proc = get_node_owner(src);
         int index = get_node_local_index(src);
         assert(owner_proc==world.rank());

        bool deleted = false;
        int start = indexofNodes.data[index], end = indexofNodes.data[index+1];
        for(int i=start ; i<end ; i++)
        {
          if(destList.data[i]==dest)
          {
            destList.data[i]=INT_MAX/2;
            deleted = true;
            perNodeCSRSpace.data[index]++;
            break;
          } 
        }
        if(deleted)
          continue;

        if(diff_csr_created)
        {
          int diff_start = diff_indexofNodes.data[index], diff_end = diff_indexofNodes.data[index+1];
          for(int i=diff_start ; i<diff_end ; i++)
          {
            if(diff_destList.data[i]==dest)
            {
              diff_destList.data[i]=INT_MAX/2;
              deleted = true;
              perNodeDiffCSRSpace.data[index]++;
              break;
            }
          }  
        }
        if(deleted)
          continue;

       // TODO : Remove this
        std::cerr<<"invalid edge asked to be deleted edge\n";
    
      }
  }

  void Graph::addEdges_RevCsr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates)
  {
      std::vector<std::vector<std::pair<int32_t,int32_t>>> dest_separated_updates(nodesPartitionSize);
      for(std::pair<int32_t,std::pair<int32_t,int32_t>> p : updates)
      {
         int src = p.first;
         int dest = p.second.first;
         int weight = p.second.second;
         int owner_proc = get_node_owner(dest);
         int index = get_node_local_index(dest);
         assert(owner_proc==world.rank());

         dest_separated_updates[index].push_back(std::make_pair(src,weight));
      }
      int total_new_diff_csr_space = 0;
      bool need_new_diff_csr = false;
      std::vector<int32_t> newDiffRevCsrSpaceRequired(nodesPartitionSize,0);
      for(int i=0;i<nodesPartitionSize;i++)
      {
        int space = perNodeRevCSRSpace.data[i];
        if(rev_diff_csr_created)
        {
          space+= perNodeDiffRevCSRSpace.data[i];
          
          newDiffRevCsrSpaceRequired[i]= diff_rev_indexofNodes.data[i+1] - diff_rev_indexofNodes.data[i] - perNodeDiffRevCSRSpace.data[i];
        }
        if(space < (int)dest_separated_updates[i].size())
        {  
          need_new_diff_csr = true;
          newDiffRevCsrSpaceRequired[i] += dest_separated_updates[i].size() - perNodeRevCSRSpace.data[i] ;
        }    
      }

      bool need_new_diff_csr_local = need_new_diff_csr;
      MPI_Allreduce(&need_new_diff_csr_local,&need_new_diff_csr,1,MPI_CXX_BOOL,MPI_LOR, MPI_COMM_WORLD);

      
      if(need_new_diff_csr)
      {
        
        std::vector<int32_t> new_diff_rev_indexOfNodes(nodesPartitionSize+1);
        new_diff_rev_indexOfNodes[0]=0;
        for(int i=1;i<nodesPartitionSize+1;i++)
        {
            new_diff_rev_indexOfNodes[i]=new_diff_rev_indexOfNodes[i-1]+newDiffRevCsrSpaceRequired[i-1];
        }
        total_new_diff_csr_space = new_diff_rev_indexOfNodes[nodesPartitionSize]; 
        
        int32_t * new_diff_rev_csr = new int32_t[total_new_diff_csr_space];
        
        int32_t * diff_srcListSizes = new int32_t [world.size()];
        MPI_Allgather(&total_new_diff_csr_space,1,MPI_INT32_T,diff_srcListSizes,1,MPI_INT32_T, MPI_COMM_WORLD);

        if(rev_diff_csr_created)
        {
          int index = 0;

          for(int i=0;i<nodesPartitionSize;i++)
          {
            for(int j=diff_rev_indexofNodes.data[i];j<diff_rev_indexofNodes.data[i+1];j++)
            {
              if(diff_srcList.data[j]!=INT_MAX/2)
              {
                new_diff_rev_csr[index]=diff_srcList.data[j];
                index++;
              }   
            }
            index = new_diff_rev_indexOfNodes[i+1];
          }
          
          for(int i=0;i<nodesPartitionSize;i++)
          {
            int index = rev_indexofNodes.data[i];
            int diff_index = new_diff_rev_indexOfNodes[i] + (diff_rev_indexofNodes.data[i+1] - diff_rev_indexofNodes.data[i]-perNodeDiffRevCSRSpace.data[i]);
            for(std::pair<int32_t,int32_t> p : dest_separated_updates[i])
            { 
                if(perNodeRevCSRSpace.data[i]>0)
                {
                  while(srcList.data[index]!=INT_MAX/2){index++;}
                  srcList.data[index]= p.first;
                  index++;
                  perNodeRevCSRSpace.data[i]--;
                }
                else
                {
                  new_diff_rev_csr[diff_index] = p.first;
                  diff_index++;
                }
            }
          }

        }
        else{
          rev_diff_csr_created = true;
          
          for(int i=0;i<nodesPartitionSize;i++)
          {
            int index = rev_indexofNodes.data[i];
            int diff_index = new_diff_rev_indexOfNodes[i];
            for(std::pair<int32_t,int32_t> p : dest_separated_updates[i])
            { 
                if(perNodeRevCSRSpace.data[i]>0)
                {
                  while(srcList.data[index]!=INT_MAX/2){index++;}
                  srcList.data[index]= p.first;
                  index++;
                  perNodeRevCSRSpace.data[i]--;
                }
                else
                {
                  new_diff_rev_csr[diff_index] = p.first;
                  diff_index++;
                }
            }
          }

        }
          
          diff_rev_indexofNodes.create_window(new_diff_rev_indexOfNodes.data(),nodesPartitionSize+1,sizeof(int32_t),world);
          diff_srcList.create_window(new_diff_rev_csr,total_new_diff_csr_space, sizeof(int32_t), world);
          for(int i=0;i<nodesPartitionSize;i++)
          {
            perNodeDiffRevCSRSpace.data[i]=0;
          }
      }
      else
      {
          for(int i=0;i<nodesPartitionSize;i++)
          {
            int index = rev_indexofNodes.data[i]; 
            int diff_index = -1;
            if(rev_diff_csr_created)
              diff_index = diff_indexofNodes.data[i] ;
            
            for(std::pair<int32_t,int32_t> p : dest_separated_updates[i])
            { 
                if(perNodeRevCSRSpace.data[i]>0)
                {
                  while(srcList.data[index]!=INT_MAX/2){index++;}
                  srcList.data[index]= p.first;
                  index++;
                  perNodeRevCSRSpace.data[i]--;
                }
                else
                {
                  if(!rev_diff_csr_created)
                    assert(false);
                  while(diff_srcList.data[diff_index]!=INT_MAX/2){diff_index++;}
                  diff_srcList.data[diff_index]= p.first;
                  diff_index++;
                  perNodeDiffRevCSRSpace.data[i]--;
                }
            }
          }
      }

  }
  void Graph::delEdges_RevCsr(std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>& updates)
  {
      for(std::pair<int32_t,std::pair<int32_t,int32_t>> p : updates)
      {
         int src = p.first;
         int dest = p.second.first;
         int owner_proc = get_node_owner(dest);
         int index = get_node_local_index(dest);
         assert(owner_proc==world.rank());

         bool deleted = false;
         int start = rev_indexofNodes.data[index], end = rev_indexofNodes.data[index+1];
        for(int i=start ; i<end ; i++)
        {
          if(srcList.data[i]==src)
          {
            srcList.data[i]=INT_MAX/2;
            deleted = true;
            perNodeRevCSRSpace.data[index]++;
            break;
          } 
        }
        if(deleted)
          continue;

        if(rev_diff_csr_created)
        {
          int diff_start = diff_rev_indexofNodes.data[index], diff_end = diff_rev_indexofNodes.data[index+1];
          for(int i=diff_start ; i<diff_end ; i++)
          {
            if(diff_srcList.data[i]==src)
            {
              diff_srcList.data[i]=INT_MAX/2;
              deleted = true;
              perNodeDiffRevCSRSpace.data[index]++;
              break;
            }
          }  
        }
        if(deleted)
          continue;
          
        // TODO : Remove this  
        std::cerr<<"invalid edge asked in get_edge\n";
    
      }
  }

  void Graph::propagateNodeFlags(NodeProperty<bool>& property)
  {
      
      int prev_total_count = 0;
      int total_count = 0;      
      while(1){
        
        int local_count = 0;

        while(1)
        {
          int local_count_2 = 0;
          for(int v = start_node();v<=end_node();v++)
          {
            if(property.propList.data[get_node_local_index(v)] == true)
            { local_count_2++;
              for(int nbr : getNeighbors(v))
              {
                 if(get_node_owner(nbr)==world.rank())
                 {
                    property.propList.data[get_node_local_index(nbr)] = true;
                 }
              }
            }
          }
          if(local_count==local_count_2)
            break;

          local_count= local_count_2;  
        }

        local_count = 0;
        std::vector<std::unordered_set<int>> newNodesToSet(world.size());
        for(int v = start_node();v<=end_node();v++)
        {
          if(property.propList.data[get_node_local_index(v)] == true)
          {
            local_count++;
            for(int nbr : getNeighbors(v))
            { 
              //if(get_node_owner(nbr)!=world.rank())
              {
               newNodesToSet[get_node_owner(nbr)].insert(nbr);
              }
              //else
              {
              //  if(property.propList.data[get_node_local_index(v)] == false)
              //    assert(false);
              } 
            }
          }
        }
      
        MPI_Allreduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if(total_count == prev_total_count)
          break;

        boost::mpi::all_to_all(world,newNodesToSet,newNodesToSet);

        for(int i=0;i<world.size();i++)
        {
          for(int v : newNodesToSet[i])
          {
            assert(get_node_owner(v)==world.rank());
            if(property.propList.data[get_node_local_index(v)] == false)
              property.propList.data[get_node_local_index(v)] = true;
          }
        }  

        prev_total_count = total_count;
        }
  }
#include"node_property.h"
#include"../../graph_mpi.h"

template <typename T>
  void NodeProperty<T>::fatBarrier () {
      /* Major slowdown ?? Investigating...    
      // synchronize the window.
      propList.get_lock (0, SHARED_ALL_PROCESS_LOCK) ;
      // MPI_Win_lock_all (0, window) ;
      for (auto &sync_now:sync_later) {
        int idx = sync_now.first ;
        for (auto &message:sync_now.second) {
            T owner_proc = message[0] ;
            T local_node_id = message[1] ;
            T value = message[2] ;
            propList.accumulate(owner_proc,&value,local_node_id,1,MPI_SUM,SHARED_LOCK);
          }
      }
      //MPI_Win_unlock_all (window) ;
      propList.unlock (0, SHARED_ALL_PROCESS_LOCK) ;
      sync_later.clear () ;
      */
  }

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
        already_locked_processors_shared = std::vector<bool>(world.size(),false);
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
        already_locked_processors_shared = std::vector<bool>(world.size(),false);
        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    int32_t NodeProperty<T>::getPropertyId() {return propertyId;}

    template <typename T>
    void NodeProperty<T>::leaveAllSharedLocks () {
      for (int lockNo = 0 ; lockNo < world.size () ; lockNo++ ) {
        if (already_locked_processors_shared [lockNo]) {
          propList.unlock (lockNo, SHARED_LOCK) ;
          already_locked_processors_shared[lockNo] = false ;
        }
      }
    }
    
    template <typename T>
    T NodeProperty<T>::getValue(int node_id ,bool check_concurrency)
    {
      

      int owner_proc = graph->get_node_owner(node_id);
      int local_node_id = graph->get_node_local_index(node_id);
      
      bool no_checks_needed = !check_concurrency;
      /* Barenya ==> Trying one more optimisation */
      /*
      if (already_locked_processors_shared [owner_proc]) {
        propList.flush (owner_proc) ;
      } else {
        already_locked_processors_shared[owner_proc]=true ;
	      propList.get_lock (owner_proc, SHARED_LOCK, no_checks_needed) ;
      }*/
	    propList.get_lock (owner_proc, SHARED_LOCK, no_checks_needed) ;
	    T* data = propList.get_data(owner_proc, local_node_id, 1, SHARED_LOCK);
      propList.unlock(owner_proc, SHARED_LOCK); // doing an unlock later experiment.
			int val = data[0] ;
			delete[] data ;
			data =NULL ;
      return val ;
      
			
/*
      if(owner_proc==world.rank())
      { // This may be optimized further.
        if(!already_locked_processors_shared[owner_proc]) {
          // already_locked_processors_shared[owner_proc]=true ;
          // propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        }
        else { 
          propList.flush (owner_proc) ;
        }
        T value = propList.data[local_node_id];
        if(!already_locked_processors_shared[owner_proc]) {
          propList.unlock(owner_proc, SHARED_LOCK);
          already_locked_processors_shared[owner_proc]=false ;
        }
        return value;
      }
      else
      {
        if(!already_locked_processors_shared[owner_proc]) {
          propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
          already_locked_processors_shared[owner_proc]=true ;
        }
		    else {
		      propList.flush (owner_proc) ;
        }
        T* data = propList.get_data(owner_proc, local_node_id, 1, SHARED_LOCK);
        
        if(!already_locked_processors_shared[owner_proc]) {
          already_locked_processors_shared[owner_proc]=false ;
          propList.unlock(owner_proc, SHARED_LOCK);
        }
				int val = data[0] ;
				delete[] data ;
				data =NULL ;
	  		return val ;
      }
      */

      assert (false) ;
			return -1;
      
    }

    template <typename T>
    void NodeProperty<T>::setValue(int node_id ,T value, bool check_concurrency)
    {
      
      int owner_proc = graph->get_node_owner(node_id);
      int local_node_id = graph->get_node_local_index(node_id);

      bool no_checks_needed = !check_concurrency;
     /* 
      if (already_locked_processors_shared[owner_proc] == false) {
        propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        already_locked_processors_shared[owner_proc]=true ;
      }*/   
      propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
      propList.put_data(owner_proc,&value, local_node_id, 1, SHARED_LOCK);
      propList.unlock(owner_proc, SHARED_LOCK);
			/*
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
          propList.get_lock(owner_proc,EXCLUSIVE_LOCK,  no_checks_needed);
		//else
		  //propList.flush (owner_proc) ;
        propList.put_data(owner_proc,&value, local_node_id, 1, SHARED_LOCK);
		// propList.flush (owner_proc) ;
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, EXCLUSIVE_LOCK);
		//else
		  //propList.flush (owner_proc) ;
      //}
	 */ 
      
      return ;
    }

    template <typename T>
    void NodeProperty<T>::rememberHistory()
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

    template <typename T>
    void NodeProperty<T>::operator =(const NodeProperty<T>& property)
    {


        if(property.attached_to_graph == false)
          return;
        else 
        {
          attachToGraph(property.graph, property.propList.data);
          return;
        }  
    }
    
    template <typename T>
    void NodeProperty<T>::deleteHistory()
    {
      delete [] history_data;
    }

    template<>
    bool NodeProperty<bool>::aggregateValue(Aggregate_Op op)
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

        // sync_later[node_id].push_back ({owner_proc, local_node_id, value}) ;

        /* Barenya ==> attempting an optimisation
*/

       /* if(!already_locked_processors_shared[owner_proc]) {
          already_locked_processors_shared[owner_proc]=true ;
          propList.get_lock(owner_proc,SHARED_LOCK);
        }*/
        propList.get_lock(owner_proc,SHARED_LOCK);
        propList.accumulate(owner_proc,&value,local_node_id,1,MPI_SUM,SHARED_LOCK);
        // if(!already_locked_processors_shared[owner_proc])
        propList.unlock(owner_proc, SHARED_LOCK);
            
    }

template class NodeProperty<int>;
template class NodeProperty<float>;
template class NodeProperty<double>;
template class NodeProperty<bool>;

#include"edge_property.h"
#include"../../graph_mpi.h"

// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define ENABLE_DEBUG to turn on debugging; comment it out to disable
// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
    #define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG(fmt, ...) // Define it as an empty macro when debugging is disabled
#endif

#endif // DEBUG_H


    template <typename T>
    void EdgeProperty<T>::attachToGraph(Graph * graph, T initial_value, bool attach_only_to_diff_csr, bool create_new_diff)
    { 
        // This means this property was already assigned and the window is already created in this case we just
        // reinitialise all the values to inital_value
        if(attached_to_graph == true)
        {
          if(attach_only_to_diff_csr == false)
          {
            for(int i=0;i<length;i++)
              propList.data[i] = initial_value;
          }
          if(graph->diff_csr_created)
            {
              if(diff_propList_present && !create_new_diff)
              {
                for(int i=0;i<diff_length;i++)
                  diff_propList.data[i]=initial_value;
              }
              else
              {
                 diff_propList_present = true;
                  diff_length = graph->diff_edgeProcMap[world.rank() + 1] - graph->diff_edgeProcMap[world.rank()];
                  T* diff_data = new T[diff_length];
                  for(int i=0;i<diff_length;i++)
                    diff_data[i]=initial_value;
                  diff_propList.create_window(diff_data, diff_length, sizeof(T), world);
              }
            }  
            return;
        }
        attached_to_graph =true;

        already_locked_processors = std::vector<bool>(world.size(),false);
        already_locked_processors_shared = std::vector<bool>(world.size(),false);

        propertyId = graph->properties_counter;
        graph->properties[propertyId] = (Property*)this;
        graph->properties_counter = graph->properties_counter + 1;

        

        length = graph->edgeProcMap[world.rank() + 1] - graph->edgeProcMap[world.rank()];

        T * data = new T [length];
        for(int i=0;i<length;i++)
          data[i]=initial_value;
  
        world = graph->world;
        
        propList.create_window(data, length, sizeof(T), world);

        if(graph->diff_csr_created)
        {
          diff_propList_present = true;
          diff_length = graph->diff_edgeProcMap[world.rank() + 1] - graph->diff_edgeProcMap[world.rank()];
          T* diff_data = new T[diff_length];
          for(int i=0;i<diff_length;i++)
            diff_data[i]=initial_value;
          diff_propList.create_window(diff_data, diff_length, sizeof(T), world);
        }

        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    void EdgeProperty<T>::attachToGraph(Graph * graph, T * initial_values, T* diff_initial_values, bool attach_only_to_diff_csr, bool create_new_diff)
    { 
        // This means this property was already assigned and the window is already created in this case we just
        // reinitialise all the values to inital_value
        if(attached_to_graph == true)
        {
            if(attach_only_to_diff_csr == false)
            { 
              for(int i=0;i<length;i++)
                propList.data[i] = initial_values[i];
            }
            if(graph->diff_csr_created)
            {
              if(diff_initial_values == NULL)
              {
                std::cerr<<"diff csr present but initialisation values for edges belonging to diff csr not provided"<<std::endl;
                exit(-1);
              }

              if(diff_propList_present && !create_new_diff)
              {
                for(int i=0;i<diff_length;i++)
                  diff_propList.data[i]=diff_initial_values[i];
              }
              else
              {
                 diff_propList_present = true;
                  diff_length = graph->diff_edgeProcMap[world.rank() + 1] - graph->diff_edgeProcMap[world.rank()];
                  T* diff_data = new T[diff_length];
                  for(int i=0;i<diff_length;i++)
                    diff_data[i]=diff_initial_values[i];
                  diff_propList.create_window(diff_data, diff_length, sizeof(T), world);
              }
            }  
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

        if(graph->diff_csr_created)
        {
          if(diff_initial_values == NULL)
          {
            std::cerr<<"diff csr [resent but initialisation values for edges belonging to diff csr not provided"<<std::endl;
            exit(-1);
          }
          diff_propList_present = true;
          diff_length = graph->diff_edgeProcMap[world.rank() + 1] - graph->diff_edgeProcMap[world.rank()];
          T* diff_data = new T[diff_length];
          for(int i=0;i<diff_length;i++)
            diff_data[i]=diff_initial_values[i];
          diff_propList.create_window(diff_data, diff_length, sizeof(T), world);
        }
        already_locked_processors = std::vector<bool>(world.size(),false);
        already_locked_processors_shared = std::vector<bool>(world.size(),false);

        this->graph = graph;

        delete [] data;
    }

    template <typename T>
    int32_t EdgeProperty<T>::getPropertyId() {return propertyId;}

    template<typename T>
    void EdgeProperty<T>::leaveAllSharedLocks () {
      for (int lockNo = 0 ; lockNo < world.size () ; lockNo++ ) {
        if (already_locked_processors_shared [lockNo]) {
          propList.unlock (lockNo, SHARED_LOCK) ;
          already_locked_processors_shared[lockNo] = false ;
        }
      }
    }
    template <typename T>
    T EdgeProperty<T>::getValue(Edge edge ,bool check_concurrency)
    {
      int owner_proc = graph->get_edge_owner(edge);
      int local_edge_id = graph->get_edge_local_index(edge);
      
      bool no_checks_needed = !check_concurrency;

      if(owner_proc==world.rank())
      {
        T value;
        if(edge.is_in_csr())
        {
        /*
          if(!already_locked_processors[owner_proc])
            propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
          value = propList.data[local_edge_id];
          if(!already_locked_processors[owner_proc])
            propList.unlock(owner_proc, SHARED_LOCK);
            */
          // if (already_locked_processors_shared [owner_proc]) {
            // propList.flush (owner_proc) ;
          // } else {
            // already_locked_processors_shared[owner_proc]=true ;
            // already_locked_processors_shared[owner_proc]=false;
            // propList.get_lock (owner_proc, SHARED_LOCK, no_checks_needed) ;
          // }

          propList.get_lock (owner_proc, SHARED_LOCK, no_checks_needed) ;
          T* data = propList.get_data(owner_proc, local_edge_id, 1, SHARED_LOCK);
          propList.unlock(owner_proc, SHARED_LOCK);

          // propList.unlock(owner_proc, SHARED_LOCK); // doing an unlock later experiment.
          value = data[0] ;
          delete[] data ;
          data =NULL ;
        }
        else
        {
          value = diff_propList.data[local_edge_id];

        }
        return value;
      }
      else
      {
        T* data;
        if(edge.is_in_csr())
        {
          //if(!already_locked_processors[owner_proc])
            propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
          data = propList.get_data(owner_proc, local_edge_id, 1, SHARED_LOCK);
          //if(!already_locked_processors[owner_proc])
            propList.unlock(owner_proc, SHARED_LOCK);
          
        }
        else
        {
          //if(!already_locked_processors[owner_proc])
            diff_propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
          data = diff_propList.get_data(owner_proc, local_edge_id, 1, SHARED_LOCK);
          //if(!already_locked_processors[owner_proc])
            diff_propList.unlock(owner_proc, SHARED_LOCK);
        }
        return data[0];  
      }
      
    }

    template <typename T>
    void EdgeProperty<T>::setValue(Edge edge ,T value, bool check_concurrency)
    {
      
      int owner_proc = graph->get_edge_owner(edge);
      int local_edge_id = graph->get_edge_local_index(edge);

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

      /*if (already_locked_processors_shared[owner_proc] == true) {
        propList.unlock (owner_proc, SHARED_LOCK) ;
        already_locked_processors_shared[owner_proc]=false ;
      }*/

      if(edge.is_in_csr())
      {
        //if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
          propList.put_data(owner_proc,&value, local_edge_id, 1, SHARED_LOCK);
        //if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
      }
      else
      {
        //if(!already_locked_processors[owner_proc])
          diff_propList.get_lock(owner_proc,SHARED_LOCK,  no_checks_needed);
        diff_propList.put_data(owner_proc,&value, local_edge_id, 1, SHARED_LOCK);
        //if(!already_locked_processors[owner_proc])
          diff_propList.unlock(owner_proc, SHARED_LOCK);
      }    
     // }
      
      return ;
    }

    template <typename T>
    void EdgeProperty<T>::rememberHistory()
    {
        history_data = new T [length];
        propList.get_lock(world.rank(),SHARED_LOCK);
        for(int i=0;i<length;i++)
        {
            history_data[i] = propList.data[i];
        }

        propList.unlock(world.rank(), SHARED_LOCK);

        if(diff_propList_present)
        {
          diff_history_data = new T [diff_length];
          diff_propList.get_lock(world.rank(),SHARED_LOCK);
          for(int i=0;i<diff_length;i++)
          {
            diff_history_data[i] = diff_propList.data[i];
          }

          diff_propList.unlock(world.rank(), SHARED_LOCK);
        }
    }

    template <typename T>
    T EdgeProperty<T>::getHistoryValue(Edge edge)
    {
        int owner_proc = graph->get_edge_owner(edge);
        int local_edge_id = graph->get_edge_local_index(edge);

        // If it is required that we need history value from a different processor remove this
        // and instead make a separate window(separate RMA_Datatype variable) for copied data and
        // use MPI_GET like normal
        assert(owner_proc == world.rank());

        if(edge.is_in_csr())
          return history_data[local_edge_id];
        else
          return diff_history_data[local_edge_id];  
    }

    template <typename T>
    void EdgeProperty<T>::operator=(const EdgeProperty<T>& property)
    {
      if(property.attached_to_graph == false)
      {
        // Barenya :
        // Would like for this branch to be used for copying by value.
        // Would it cause other algorithms to crash ?
        attachToGraph (property.graph, property.propList.data, (T*)NULL) ;
        return;
      }
      else{
        if(property.diff_propList_present)
          attachToGraph(property.graph, property.propList.data, (T*)NULL);
        else
          attachToGraph(property.graph, property.propList.data, property.diff_propList.data,false, true );
      }
    }

    template <typename T>
    void EdgeProperty<T>::deleteHistory()
    {
      delete [] history_data;
      if(diff_propList_present)
        delete [] diff_history_data;
    }

    template<>
    bool EdgeProperty<bool>::aggregateValue(Aggregate_Op op)
    {
      bool value = true;
      if(op == NOT)
      {
        for(int i=0;i<length;i++)
          value &= !propList.data[i];

        if(diff_propList_present)
        {
          for(int i=0;i<diff_length;i++)
            value &= !diff_propList.data[i];
        }    
      }
      else if (op == NO_OP)
      {
        for(int i=0;i<length;i++)
          value &= propList.data[i];
        
        if(diff_propList_present)
        {
          for(int i=0;i<diff_length;i++)
            value &= !diff_propList.data[i];
        }  
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
    T EdgeProperty<T>::compareAndSwap(Edge edge, T compare, T swap)
    { 
        int owner_proc = graph->get_edge_owner(edge);
        int local_edge_id = graph->get_edge_local_index(edge);
        T oldValue;
        
        if(edge.is_in_csr())
        {
        //if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK);
          propList.compareAndSwap(owner_proc,local_edge_id,&compare, &swap,&oldValue);
        //if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
        }
        else
        {
          diff_propList.get_lock(owner_proc,SHARED_LOCK);
          diff_propList.compareAndSwap(owner_proc,local_edge_id,&compare, &swap,&oldValue);
        //if(!already_locked_processors[owner_proc])
          diff_propList.unlock(owner_proc, SHARED_LOCK);
        }    
      return oldValue;
    }

    template <typename T>
    void EdgeProperty<T>::initialize_reduction_queue()
    {
     /* reduction_queue.clear();
      reduction_queue.resize(world.size());*/
    }

    template <typename T>
    void EdgeProperty<T>::add_to_reduction_queue(int reduction_node,std::pair<int32_t, T> pair)
    {
      //reduction_queue[graph->get_edge_owner(reduction_node)].push_back(pair);
    }

    template <typename T>
    std::vector<std::vector<int32_t>> EdgeProperty<T>::perform_reduction(MPI_Op op)
    {
      /*boost::mpi::all_to_all(world, reduction_queue, reduction_queue);
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
      return modified_ids;*/
    }

    template <typename T>
    void EdgeProperty<T>::assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids)
    {
      /*std::vector<std::vector<std::pair<int32_t,T>>> reductions_taken(world.size());
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
      propList.unlock(world.rank(),EXCLUSIVE_LOCK);*/
    }

    template <typename T>
    void EdgeProperty<T>::atomicAdd(Edge &edge, T value)
    {
        int owner_proc = graph->get_edge_owner(edge);
        int local_edge_id = graph->get_edge_local_index(edge);
        // sync_later[edge.get_id()].push_back ({owner_proc, local_edge_id, value}) ;
        DEBUG ("Attempting atomic Add of value %d on an edge property\n", value) ;

        if(edge.is_in_csr())
        {
          
        if(!already_locked_processors[owner_proc])
          propList.get_lock(owner_proc,SHARED_LOCK);
          propList.accumulate(owner_proc,&value,local_edge_id,1,MPI_SUM,SHARED_LOCK);
        if(!already_locked_processors[owner_proc])
          propList.unlock(owner_proc, SHARED_LOCK);
         /* 
          if(!already_locked_processors_shared[owner_proc]) {
            // already_locked_processors_shared[owner_proc]=true ;
            already_locked_processors_shared[owner_proc]=false ;
            propList.get_lock(owner_proc,SHARED_LOCK);
            DEBUG ("lock taken by %d on %d\n", owner_proc, world.rank ()) ;

          }
          propList.accumulate(owner_proc,&value,local_edge_id,1,MPI_SUM,SHARED_LOCK);
          DEBUG ("atomic add of vale %d on edge property success\n", value) ;
          */

        }
        else
        {
          DEBUG ("unfortunately, got sent here\n") ;
           
          diff_propList.get_lock(owner_proc,SHARED_LOCK);
          diff_propList.accumulate(owner_proc,&value,local_edge_id,1,MPI_SUM,SHARED_LOCK);
        //if(!already_locked_processors[owner_proc])
          diff_propList.unlock(owner_proc, SHARED_LOCK);
          
        }
      
    }
    template <typename T>
    void EdgeProperty<T>::fatBarrier () {
       
      /* experiencing slow down because of this
      propList.get_lock (0, SHARED_ALL_PROCESS_LOCK) ;
      for (auto &sync_now:sync_later) {
        int idx = sync_now.first ;
        for (auto &message:sync_now.second) {
            T owner_proc = message[0] ;
            T local_node_id = message[1] ;
            T value = message[2] ;
            propList.accumulate(owner_proc,&value,local_node_id,1,MPI_SUM,SHARED_LOCK);
          }
      }
      propList.unlock (0, SHARED_ALL_PROCESS_LOCK) ;
      */
      sync_later.clear () ;
    }


template class EdgeProperty<int>;
template class EdgeProperty<float>;
template class EdgeProperty<double>;
template class EdgeProperty<bool>;

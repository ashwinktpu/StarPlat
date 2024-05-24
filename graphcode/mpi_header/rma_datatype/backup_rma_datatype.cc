#include"rma_datatype.h"  
  
  template<typename T>
  Rma_Datatype<T>::Rma_Datatype()
  {
    window_created = false;
    is_custom_dataype = false;
    length = 0;
  }

  template<typename T>
  Rma_Datatype<T>::~Rma_Datatype()
  {
    if(window_created)
    {
      MPI_Win_free(&win);
      MPI_Free_mem(this->data);
      if(is_custom_dataype)
        MPI_Type_free(&mpi_datatype);
    }  
  }

  template<typename T>
  void Rma_Datatype<T>::create_window(T* data, int32_t length, int32_t dataTypeSizeInBytes, boost::mpi::communicator world,  MPI_Info info  )
  {
    if(window_created)
    {
      MPI_Win_free(&win);
      MPI_Free_mem(this->data);
    }

    window_created = true;
    MPI_Alloc_mem(MPI_Aint(length * dataTypeSizeInBytes), info, &this->data);
    for(int i=0;i<length;i++)
    {
      this->data[i] = data[i]; 
    }
    this-> length = length;
    this->dataTypeSizeInBytes = dataTypeSizeInBytes;
    this->world = world;
    MPI_Win_create(this->data , MPI_Aint(length * dataTypeSizeInBytes) , dataTypeSizeInBytes, MPI_INFO_NULL, world, &win);

    if(is_custom_dataype)
      MPI_Type_commit(&mpi_datatype);
  }

  template<typename T>  
  void Rma_Datatype<T>::get_lock(int32_t proc_num, locktype lock,  bool no_checks_needed )  
  {
    int assert = no_checks_needed == false ? 0 : MPI_MODE_NOCHECK;
	MPI_Win_lock (MPI_LOCK_EXCLUSIVE, proc_num, 0, win) ;
     /*if(lock == SHARED_LOCK || lock == EXCLUSIVE_LOCK)
    {   
        int mpi_lock = lock == SHARED_LOCK  ? MPI_LOCK_SHARED : MPI_LOCK_EXCLUSIVE;
        // MPI_Win_lock(mpi_lock, proc_num, assert , win);
        MPI_Win_lock(mpi_lock, proc_num, 0, win);
    }
    else if(lock == SHARED_ALL_PROCESS_LOCK)
    {
        MPI_Win_lock_all(assert , win);
    }
    else {
        std::cerr <<"Invalid Lock Type\n";
        exit(-1);
    }*/
  }

  // Barenya : Need to flush and sync before get. 
  template<typename T>
  void Rma_Datatype<T>::flush (int32_t targetRank) {
 	MPI_Win_flush (targetRank, win) ; 
	MPI_Win_sync (win) ;
  }
  
  template<typename T>
  void Rma_Datatype<T>::unlock(int32_t proc_num ,locktype lock )
  {
    MPI_Win_unlock(proc_num, win);
	/*
    if(lock != SHARED_ALL_PROCESS_LOCK)
        MPI_Win_unlock(proc_num, win);
    else
        MPI_Win_unlock_all(win);
	*/
  }


  template<typename T>
  T * Rma_Datatype<T>::get_data(int32_t proc_num, int startIndex , int length, locktype lock)
  {
    T* data_array = new T[length];
    MPI_Get_accumulate(0,0,mpi_datatype, data_array,length,mpi_datatype, proc_num,startIndex,length,mpi_datatype, MPI_NO_OP,win);
    //MPI_Get(data_array , length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, win);
    return data_array;
  }

  template<typename T>
  void Rma_Datatype<T>::put_data(int32_t proc_num, T* data_array, int startIndex , int length, locktype lock)
  {
    MPI_Accumulate(data_array,length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, MPI_REPLACE, win);
    //MPI_Put(data_array , length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, win);
	// accumulate (proc_num, data_array, startIndex, length, MPI_SUM, lock) ;
    return ;
  }

  template<typename T>
  void Rma_Datatype<T>::compareAndSwap(int32_t proc_num ,int index,T* compare,T* swap, T* oldValue)
  {
    MPI_Compare_and_swap(swap, compare, oldValue,mpi_datatype, proc_num, index, win);
  
  }
  
  template<typename T>
  void Rma_Datatype<T>::accumulate(int32_t proc_num,  T* data_array,int startIndex,int length, MPI_Op op,locktype lock)
  {
    MPI_Accumulate(data_array , length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, op, win);
    return;
  }

template class Rma_Datatype<int>;
template class Rma_Datatype<float>;
template class Rma_Datatype<double>;
template class Rma_Datatype<bool>;

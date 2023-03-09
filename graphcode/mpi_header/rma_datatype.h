
#include <mpi.h>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

enum locktype
{
    SHARED_LOCK,
    EXCLUSIVE_LOCK,
    SHARED_ALL_PROCESS_LOCK,
};

// This is RMA_DATAYPE class used for one sided communication in mpi
// Note :- before window creation the public variable dataype mpi_dataype has to be assigned the appropriate 
// mpi dataype built-in or custom and in the later case the public variable is_custom_dataype is to be set to true 
// before calling create window (This could not be done in the cunstructor or a function of the class as primitive 
// MPI_TYPES and custom MPI_TYPES are different from each other to handle)
template<typename T> 
class Rma_Datatype {
  private :
    MPI_Win win;
    bool window_created;
    int32_t dataTypeSizeInBytes;
    boost::mpi::communicator world;

  public :  
    int32_t length;
    T * data;
    MPI_Datatype mpi_datatype;
    bool is_custom_dataype;

  Rma_Datatype()
  {
    window_created = false;
    is_custom_dataype = false;
    length = 0;
  }

  ~Rma_Datatype()
  {
    if(window_created)
    {
      MPI_Win_free(&win);
      if(is_custom_dataype)
        MPI_Type_free(&mpi_datatype);
    }  
  }

  void create_window(T* data, int32_t length, int32_t dataTypeSizeInBytes, boost::mpi::communicator world,  MPI_Info info = MPI_INFO_NULL )
  {
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

  void get_lock(int32_t proc_num, locktype lock,  bool no_checks_needed = false)  
  {
    int assert = no_checks_needed == false ? 0 : MPI_MODE_NOCHECK;
     if(lock == SHARED_LOCK || lock == EXCLUSIVE_LOCK)
    {   
        int mpi_lock = lock == SHARED_LOCK  ? MPI_LOCK_SHARED : MPI_LOCK_EXCLUSIVE;
        MPI_Win_lock(mpi_lock, proc_num, assert , win);
    }
    else if(lock == SHARED_ALL_PROCESS_LOCK)
    {
        MPI_Win_lock_all(assert , win);
    }
    else {
        std::cerr <<"Invalid Lock Type\n";
        exit(-1);
    }
  }
  
  void unlock(int32_t proc_num ,locktype lock )
  {
    if(lock != SHARED_ALL_PROCESS_LOCK)
        MPI_Win_unlock(proc_num, win);
    else
        MPI_Win_unlock_all(win);
  }


  T * get_data(int32_t proc_num, int startIndex , int length, locktype lock)
  {
    T* data_array = new T[length];
    MPI_Get_accumulate(0,0,mpi_datatype, data_array,length,mpi_datatype, proc_num,startIndex,length,mpi_datatype, MPI_NO_OP,win);
    //MPI_Get(data_array , length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, win);
    return data_array;
  }

  void put_data(int32_t proc_num, T* data_array, int startIndex , int length, locktype lock)
  {
    MPI_Accumulate(data_array,length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, MPI_REPLACE, win);
    //MPI_Put(data_array , length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, win);
    return ;
  }

  void compareAndSwap(int32_t proc_num ,int index,T* compare,T* swap, T* oldValue)
  {
    MPI_Compare_and_swap(swap, compare, oldValue,mpi_datatype, proc_num, index, win);
  
  }
  
  void accumulate(int32_t proc_num,  T* data_array,int startIndex,int length, MPI_Op op,locktype lock)
  {
    MPI_Accumulate(data_array , length, mpi_datatype, proc_num, startIndex, length, mpi_datatype, op, win);
    return;
  }
};
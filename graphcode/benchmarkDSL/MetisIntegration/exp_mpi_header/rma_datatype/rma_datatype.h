#ifndef RMA_DATATYPE_HEADER
#define RMA_DATATYPE_HEADER

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

  Rma_Datatype();

  ~Rma_Datatype();
  

  void create_window(T* data, int32_t length, int32_t dataTypeSizeInBytes, boost::mpi::communicator world,  MPI_Info info = MPI_INFO_NULL );

  void get_lock(int32_t proc_num, locktype lock,  bool no_checks_needed = false);

  void flush (int32_t targetRank) ;

  void unlock(int32_t proc_num ,locktype lock );

  T * get_data(int32_t proc_num, int startIndex , int length, locktype lock);

  void put_data(int32_t proc_num, T* data_array, int startIndex , int length, locktype lock);

  void compareAndSwap(int32_t proc_num ,int index,T* compare,T* swap, T* oldValue);

  void accumulate(int32_t proc_num,  T* data_array,int startIndex,int length, MPI_Op op,locktype lock);

};
#endif

#include"data_constructs.h"

// Barenya : todo : switch to advanced passive synchronization with lock_all and flush_all.
void checkMPIComm (int mpiStatus, char* errorMessage) {
    if (mpiStatus != MPI_SUCCESS) {
        printf ("%s\n", errorMessage) ;
    }
}

template<typename T>
Container<T>::Container()
{
    this->array = NULL ;
}

template<typename T>
void Container<T>::assign (const int &size, const int &initVal, MPI_Comm comm) {
    this->globalSize = size ;
    MPI_Comm_size (comm, &this->numProcs) ;
    this->localSize = (ceil(float(size)/this->numProcs)) ;
    this->comm = comm ;
    MPI_Comm_rank (comm, &this->rank) ;
    this->lockAssertion = 0 ;
    if (this->rank == this->numProcs-1) {
        this->localSize = globalSize - (this->rank * this->localSize) ;
    }
    printf ("global size = %d\n", this->globalSize) ;
    printf ("set localSize for rank %d to %d\n", this->rank, this->localSize) ;
    // allocate an array using new.
    this->baseArray = new int[this->localSize] ;
    memset (baseArray, 0 , sizeof(int) * this->localSize) ;
    MPI_Win_create (this->baseArray, (MPI_Aint) this->localSize * sizeof(int), sizeof(int), MPI_INFO_NULL, this->comm, &this->array) ;
}

template<typename T>
int Container<T>::getIdx (const int &val) {
    // Must be called from all processes in communicator/group.
    int idx ;
    bool foundFlag = false ;
    // sync_assignments() ;
    checkMPIComm (MPI_Win_lock_all (this->lockAssertion, this->array), "getting all lock in getIdx failed") ;
    for (idx = 0; idx < this->localSize; idx++) {
        if (this->baseArray[idx] == 0) {
            foundFlag = true ;
            break ;
        } 
    } 
    
    idx += this->rank * this->localSize ;

    MPI_Allreduce (&foundFlag, &foundFlag, 1, MPI_C_BOOL, MPI_LOR, comm) ;
    if (!foundFlag) return -1 ;
    checkMPIComm(MPI_Win_unlock_all (this->array), "unlocking failed in getIdx") ;
    checkMPIComm(MPI_Allreduce (&idx, &idx, 1, MPI_INT, MPI_MIN, comm), "all reduce failed") ;
    printf ("returning idx %d\n", idx) ;
    return this->rank * this->localSize + idx ;
}

template<typename T>
Container<T>::Container(int size) : vect(size)
{
    // Barenya : todo : fix compilation errors with constructors.
    assert (false) ;
}

template<typename T>
Container<T>::Container(int size, T initial_value) : vect(size,initial_value)
{
    // Barenya : todo : fix compilation errors with constructors.
    assert (false) ;
}

template<typename T>
int Container<T>::calculateTargetRank (const int &idx) {
    return (int)(floor(float(idx)/(this->localSize))) ; 
}

template<typename T>
int Container<T>::calculateTargetDisp (const int &idx) {
    int targetRank = calculateTargetRank (idx) ;
    return idx - targetRank*this->localSize ;
}

template<typename T>
int Container<T>::calculateTargetCount (const int &idx) {
    return this->localSize ;
}

template<typename T>
void Container<T>::push(T value)
{
    // Barenya : todo : make distributed amortized logic for push.
    assert (false) ;
    vect.push_back(value);
}

template<typename T>
void Container<T>::pop()
{
    // Barenya : todo : make distributed amortized logic for pop.
    assert (false) ;
    vect.pop_back();
}

template<typename T>
void Container<T>::clear()
{
    MPI_Win_free (&this->array) ;
    this->array = NULL ;
}

template<typename T>
int  Container<T>::getValue(const int &node_owner, const int &idx)
{
    if(idx>= this->globalSize)
    {
        std::cerr<<"Invalid index for cotainer access "<< idx << std::endl;
        assert (false) ;
    }

    T actualValue ;
    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount(idx) ;

    //sync_assignments () ;
    //if (this->rank == node_owner) {
    printf ("from inside a get call sourceRank = %d targetRank = %d  with displacement %d\n", node_owner, targetRank , targetDisp) ;
      MPI_Win_lock (MPI_LOCK_EXCLUSIVE, targetRank, this->lockAssertion, this->array) ;
      printf ("lock request sent\n") ;
      MPI_Get (&actualValue, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, this->array);
      if (actualValue < 0) {
        printf ("failed at get op from rank %d targetRank = %d and disp = %d\n", this->rank, targetRank, targetDisp) ;
        assert (false) ;
      }
      printf ("get returned with value %d\n", actualValue) ;
      MPI_Win_unlock (targetRank, this->array);
    // }
    return actualValue ;
}

template<typename T>
void Container<T>::setValue(const int &node_owner, const int &idx, const int &value) {

    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount (idx) ;
    //if (this->rank == node_owner) {
    printf ("from inside a set call sourceRank = %d targetRank = %d with displacement = %d \n", node_owner, targetRank , targetDisp) ;
      checkMPIComm (MPI_Win_lock (MPI_LOCK_EXCLUSIVE, targetRank, this->lockAssertion, this->array), "failed to acquire lock while setting value") ;
      printf ("acquired lock\n") ;
      checkMPIComm (MPI_Put (&value, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, this->array), "failed while assignment\n") ;
      printf ("put successful??\n"); 
      checkMPIComm (MPI_Win_unlock (targetRank, this->array), "failed to release lock") ;
    // }
    // MPI_Win_flush (this->rank, this->array) ;
}

template<typename T>
void Container<T>::queue_assignment(int index, T value)
{
   // Why would this even be needed if it's RMA ?
   assert (false) ; 
}

template<typename T>
void Container<T>::queue_push(T value)
{
   // This should not be needed.
   assert (false) ; 
}

template<typename T>
void Container<T>::sync_assignments()
{
    MPI_Win_flush_all (this->array) ; // Synchronize the window.
}

template<typename T>
void Container<T>::printArr () {
    MPI_Win_lock_all (this->lockAssertion, this->array) ;
    for (int pNo = 0; pNo < this->numProcs; pNo++) {
        if (pNo == this->rank) {
            for (int i=0; i<this->localSize; i++) {
                printf ("%d ", this->baseArray[i]) ;
            } 
        }
        MPI_Barrier (this->comm) ;
    }
    printf ("\n") ;
    MPI_Win_unlock_all (this->array) ;
}


template class Container<int>;
template class Container<float>;
template class Container<double>;
template class Container<bool>;


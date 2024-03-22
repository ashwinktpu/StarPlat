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
    this->dispUnit = this->rank * this->localSize ; 
    checkMPIComm (MPI_Win_allocate (this->localSize * sizeof (int), sizeof(int), MPI_INFO_NULL, this->comm, &this->baseArray, &this->array), "Allocation for MPI Window failed!!") ;
}

template<typename T>
int Container<T>::getIdx (const int &val) {
    // Must be called from all processes.
    // Locally check if there is any node with count = 0 .
    // But what about ongoing transactions in RMA ?
    int idx ;
    bool foundFlag = false ;
    sync_assignments() ;
    MPI_Win_lock_all (this->lockAssertion, this->array) ;
    for (int idx = 0; idx < this->localSize; idx++) {
        if (this->baseArray[idx] == 0) {
            foundFlag = true ;
            break ;
        } 
    } 
    MPI_Win_unlock_all (this->array) ;
    MPI_Allreduce (&idx, &idx, 1, MPI_INT, MPI_MIN, comm) ;
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
    return (int)(ceil(float(idx)/(this->localSize))) ; 
}

template<typename T>
int Container<T>::calculateTargetDisp (const int &idx) {
    return (idx - (int)ceil (float(idx)/(this->localSize))) ;
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
T& Container<T>::getValue(const int &idx)
{
    if(idx>= this->globalSize)
    {
        std::cerr<<"Invalid index for cotainer access"<<std::endl;
        exit(-1);
    }

    T actualValue ;
    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount(idx) ;

    checkMPIComm (MPI_Win_lock (MPI_LOCK_SHARED, this->rank, lockAssertion, this->array), "failed to acquire lock") ;
    checkMPIComm (MPI_Get (&actualValue, 1, MPI_INT, targetRank, targetDisp, targetCount, MPI_INT, this->array), "MPI Get failed while [] operation")  ;
    checkMPIComm (MPI_Win_unlock (this->rank, this->array), "failed to release lock") ;

    return actualValue ;
}

template<typename T>
void Container<T>::setValue(const int &idx, const int &value) {

    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount (idx) ;
    checkMPIComm (MPI_Win_lock (MPI_LOCK_EXCLUSIVE, this->rank, this->lockAssertion, this->array), "failed to acquire lock while setting value") ;
    checkMPIComm (MPI_Put (&value, 1, MPI_INT, targetRank, targetDisp, targetCount, MPI_INT, this->array), "failed while assignment\n") ;
    checkMPIComm (MPI_Win_unlock (this->rank, this->array), "failed to release lock") ;
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
    // todo.
    assert (false) ;
}


template class Container<int>;
template class Container<float>;
template class Container<double>;
template class Container<bool>;


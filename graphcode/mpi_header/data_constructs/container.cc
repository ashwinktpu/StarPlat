#include"data_constructs.h"
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
    /* Experiment : 
    if (this->rank == this->numProcs-1) {
        this->localSize = globalSize - (this->rank * this->localSize) ;
    }*/
//    printf ("global size = %d\n", this->globalSize) ;
    // printf ("set localSize for rank %d to %d\n", this->rank, this->localSize) ;
    // allocate an array using new.
//     this->localSize = std::max (0, this->localSize) ;
    this->baseArray = new int[this->localSize] ;
    memset (baseArray, 0 , sizeof(int) * this->localSize) ;
    MPI_Win_create (this->baseArray, (MPI_Aint) this->localSize * sizeof(int), sizeof(int), MPI_INFO_NULL, this->comm, &this->array) ;
}

template<typename T>
int Container<T>::getIdx (const int &val) {
    // Must be called from all processes in communicator/group.
    MPI_Barrier (comm) ;
    int idx ;
    bool foundFlag = false ;
    // sync_assignments() ;
    DEBUG ("Hola TOdos\n") ;
    
    MPI_Win_lock_all (0, this->array) ; // Risky optimisation.
    for (auto &sync_now:sync_later) {
      int idx = sync_now.first ;
      for (auto &message:sync_now.second) {
          int targetRank = message[0] ;
          int targetDisp = message[1] ;
          int targetVal = message[2] ;
          DEBUG ("Accumulating at rank %d\n", targetRank) ;
          MPI_Accumulate (&targetVal, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, MPI_SUM, this->array) ;
        }
    }
    sync_later.clear () ;
    MPI_Win_unlock_all (this->array) ;
    // checkMPIComm (MPI_Win_lock_all (this->lockAssertion, this->array), "getting all lock in getIdx failed") ;
	  MPI_Win_lock (MPI_LOCK_EXCLUSIVE, this->rank, this->lockAssertion, this->array) ;
    for (idx = 0; idx < this->localSize; idx++) {
        if (this->baseArray[idx] == 0) {
            foundFlag = true ;
            break ;
        } 
    } 
	  MPI_Win_unlock (this->rank, this->array) ;
    
    idx += this->rank * (this->globalSize/this->numProcs);

    MPI_Allreduce (&foundFlag, &foundFlag, 1, MPI_C_BOOL, MPI_LOR, comm) ;
    if (!foundFlag) return -1 ;
    //checkMPIComm(MPI_Win_unlock_all (this->array), "unlocking failed in getIdx") ;
    checkMPIComm(MPI_Allreduce (&idx, &idx, 1, MPI_INT, MPI_MIN, comm), "all reduce failed") ;
    //printf ("returning idx %d\n", idx) ;
    MPI_Barrier (comm) ;
    return idx ;
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
    // return (int)(floor(float(idx)/(ceil ((float)this->globalSize/this->numProcs)))) ; 
    return idx / this->localSize ;
}

template<typename T>
int Container<T>::calculateTargetDisp (const int &idx) {
    int targetRank = calculateTargetRank (idx) ;
    // return idx - targetRank*(int)(ceil((float)this->globalSize/this->numProcs)) ;
    return idx % this->localSize ;
}

template<typename T>
int Container<T>::calculateTargetCount (const int &idx) {
    return this->localSize ;
}

template<typename T>
void Container<T>::push_back(T value)
{
    // Barenya : todo : make distributed amortized logic for push.
    assert (false) ;
    vect.push_back(value);
}

template<typename T>
void Container<T>::pop_back()
{
    // Barenya : todo : make distributed amortized logic for pop.
    assert (false) ;
    vect.pop_back();
}

template<typename T>
void Container<T>::clear()
{
    MPI_Win_free (&this->array) ;
		delete[] this->array ;
    this->array = NULL ;
}

template<typename T>
int  Container<T>::getValue(const int &node_owner, const int &idx)
{
    if(idx>= this->globalSize)
    {
        std::cerr<<"Invalid index for cotainer access "<< idx << std::endl;
		    printf ("overflow at idx %d\n", idx) ;
        assert (false) ;
    }


    MPI_Win_lock_all (0, this->array) ; // Risky optimisation.
    for (auto &sync_now:sync_later) {
      int idx = sync_now.first ;
      for (auto &message:sync_now.second) {
          int targetRank = message[0] ;
          int targetDisp = message[1] ;
          int targetVal = message[2] ;
          MPI_Accumulate (&targetVal, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, MPI_SUM, this->array) ;
        }
    }
    MPI_Win_unlock_all (this->array) ;
    
    sync_later.clear () ;

    int actualValue ;
    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount(idx) ;
    // sync_assignments () ;

//    printf ("from inside a get call sourceRank = %d targetRank = %d  with displacement %d\n", node_owner, targetRank , targetDisp) ;
      MPI_Win_lock (MPI_LOCK_SHARED, targetRank, this->lockAssertion, this->array) ;
//    printf ("lock request sent\n") ;
      MPI_Get (&actualValue, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, this->array);
      if (actualValue < 0) {
        // Specifically for push relabel. Remove later, depending on application.
        assert (false) ;
      }
      // printf ("get returned with value %d\n", actualValue) ;
      MPI_Win_unlock (targetRank, this->array);
    return actualValue ;
}

template<typename T>
void Container<T>::setValue(const int &node_owner, const int &idx, const int &value) {

    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount (idx) ;
    //if (this->rank == node_owner) {
    // printf ("from inside a set call sourceRank = %d targetRank = %d with displacement = %d \n", node_owner, targetRank , targetDisp) ;
      checkMPIComm (MPI_Win_lock (MPI_LOCK_EXCLUSIVE, targetRank, this->lockAssertion, this->array), "failed to acquire lock while setting value") ;
      // printf ("acquired lock\n") ;
      checkMPIComm (MPI_Put (&value, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, this->array), "failed while assignment\n") ;
      // printf ("put successful??\n"); 
      checkMPIComm (MPI_Win_unlock (targetRank, this->array), "failed to release lock") ;
    // }
    // MPI_Win_flush (this->rank, this->array) ;
}

template<typename T>
void Container<T>::atomicAdd (const int &idx, const int &val) {
    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount (idx) ;
    // Commenting out for experimenting with optimisation.
    /*
	  MPI_Win_lock (MPI_LOCK_EXCLUSIVE, targetRank, this->lockAssertion, this->array) ;
    MPI_Accumulate(&val,1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, MPI_SUM, this->array);
	  MPI_Win_unlock (targetRank, this->array) ;
    */
    

    //  optimization failed now
    // Optimization :
    DEBUG ("atomic Add on %d deduced rank = %d\n", targetRank, idx) ;
    sync_later[idx].push_back ({targetRank, targetDisp, val}) ;
    
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


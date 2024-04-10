#include"data_constructs.h"

// Barenya : todo : switch to advanced passive synchronization with lock_all and flush_all.
void checkMPIComm (int mpiStatus, char* errorMessage) {
    if (mpiStatus != MPI_SUCCESS) {
        printf ("%s\n", errorMessage) ;
    }
}

void customAssert (bool conditionVal, char * failMessage) {
	if (!conditionVal) {
		printf ("custom assertion failed : %s\n", failMessage) ;	
		assert (false) ;
	}
}

template<typename T>
Container<T>::Container () {
	// set implementation.
	this->implementation = 0 ;
	// set array to null.
	this->array = NULL ;
}

template<typename T>
Container<T>::Container(const int &dims, MPI_Comm comm)
{
		// Initialize the numPorcs
		MPI_Comm_size (comm, &this->size) ;
		// Initialize the rank
		MPI_Comm_rank (comm, &this->rank) ;
		// clear the unordered_map
		this->mapObj.clear () ;
		// set number of dimensions.
		this->mapDims = dims ;
		// set implementation
		this->implementation = 1 ;
}

template<typename T>
void Container<T>::push_back (const int &idx, const int &val) {
	// Check if the implementation is correct.
	customAssert (this->implementation == 1, "push_back is not implemented for single dimensional array") ;
	// Deduce rank of owner and push back there.
	int targetRank = calculateTargetRank (idx) ;
	//acquire lock.
	acquireLock (targetRank, idx, EXCLUSIVE) ;
	// Send the message via an RMA interface.
	sendMessage (targetRank, idx, val, PUSH_BACK) ;	
	// release lock.
	releaseLock (targetRank, idx, EXCLUSIVE) ;
}

template<typename T>
void Container<T>::pop_back (const int &idx, const int &val) {
	// Check if implementation is correct.
	customAssert (this->implemetation == 1, "pop_back is not implemented for single dimensional array") ;
	// Deduce the rank of owner to pop_back at.
	int targetRank = calculateTargetRank (idx) ;
	// acquire lock.
	acquireLock (targetRank, idx, EXCLUSIVE) ;
	// send message to RMA interface.
	sendMessage (targetRank, idx, val, POP_BACK) ;
	// relase lock.
	releaseLock (targetRank, idx, EXCLUSIVE) ;
}

template<typename T>
void Container<T>::setValue (int * idx, const int value) {
	// Check if implementation is correct.
	customAssert (this->implementation == 1, "Wrong kind of set Vale") ;
	// deduce target rank
	int targetRank = calculateTargetRank (idx) ;
	// acquire lock.
	acquireLock (targetRank, idx, EXCLUSIVE) ;
	// send message to RMA interface.
	sendMessage (targetRank, idx, val, setValue) ;
	// release lock.
	releaseLock (targetRank, idx, GET_VAL) ;
}

template<typename T>
int Container<T>::getValue (int * idx) {
	// Check if implementation is correct.
	customAssert (this->implementation == 1, "wrong kind of get value") ;
	// deduce the target rank.
	int targetRank = calculateTargetRank (idx) ;
	// acquire lock.
	acquireLock (targetRank, idx, SHARED) ;
	// send message to RMA interface.
	sendMessage (targetRank, idx, val, GET_VALUE) ;
	// receive actual value.
	recvMessage (targetRank, idx, val, GET_VALUE) ;
	// reqlese lock.
	releaseLock (targetRank, idx, SHARED) ;
}


template<typename T>
void Container<T>::assign (const int &size, const int &initVal, MPI_Comm comm) {

	// Check if assign is not implemented.
	customAssert (this->implementation == 0, "assign is not implemented for multidimensional array.") ;

    this->globalSize = size ;
    MPI_Comm_size (comm, &this->numProcs) ;
    this->localSize = (ceil(float(size)/this->numProcs)) ;
    this->comm = comm ;
    MPI_Comm_rank (comm, &this->rank) ;
    this->lockAssertion = 0 ;
    if (this->rank == this->numProcs-1) {
        this->localSize = globalSize - (this->rank * this->localSize) ;
    }
//    printf ("global size = %d\n", this->globalSize) ;
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
    return (int)(floor(float(idx)/(ceil ((float)this->globalSize/this->numProcs)))) ; 
}

template<typename T>
int Container<T>::calculateTargetDisp (const int &idx) {
    int targetRank = calculateTargetRank (idx) ;
    return idx - targetRank*(int)(ceil((float)this->globalSize/this->numProcs)) ;
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
		printf ("overflow at idx %d\n", idx) ;
        assert (false) ;
    }

    int actualValue ;
    int targetRank = this->calculateTargetRank (idx) ;
    int targetDisp = this->calculateTargetDisp (idx) ;
    int targetCount = this->calculateTargetCount(idx) ;

    //sync_assignments () ;
    //if (this->rank == node_owner) {
    // printf ("from inside a get call sourceRank = %d targetRank = %d  with displacement %d\n", node_owner, targetRank , targetDisp) ;
      MPI_Win_lock (MPI_LOCK_EXCLUSIVE, targetRank, this->lockAssertion, this->array) ;
//      printf ("lock request sent\n") ;
      MPI_Get (&actualValue, 1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, this->array);
      if (actualValue < 0) {
        printf ("failed at get op from rank %d targetRank = %d and disp = %d value = %d\n", this->rank, targetRank, targetDisp, actualValue) ;
        assert (false) ;
      }
      // printf ("get returned with value %d\n", actualValue) ;
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
	MPI_Win_lock (MPI_LOCK_EXCLUSIVE, targetRank, this->lockAssertion, this->array) ;
    MPI_Accumulate(&val,1, MPI_INT, targetRank, targetDisp, 1, MPI_INT, MPI_SUM, this->array);
	MPI_Win_unlock (targetRank, this->array) ;
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


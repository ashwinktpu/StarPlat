#include"data_constructs.h"

// Barenya : todo : switch to advanced passive synchronization with lock_all and flush_all.

template<typename T>
Container<T>::Container()
{
    this->array = NULL ;
}

template<typename T>
void Contaner<T>::assign (const int &size, const int &initVal, boost::mpi::communicator comm) {
    this->globalSize = size ;
    this->localSize = (ceil(float(size)/comm.size())) ;
    this->comm = comm ;
    this->rank = comm.rank () ;
    this->dispUnit = comm.rank () * this->localSize ; 
    mpiAssert (MPI_Win_allocate (this->localSize, this->dispUnit, this->MPI_INFO_NULL, (MPI_COMM_WORLD)this->comm, this->baseArray, &this->array), "Allocation for MPI Window failed!!") ;
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
    MPI_Win_free (this->array) ;
    this->array = NULL ;
}

template<typename T>
T& Container<T>::getValue(const int &idx)
{
    if(index>= this->globalSize)
    {
        std::cerr<<"Invalid index for cotainer access"<<std::endl;
        exit(-1);
    }

    int actualValue ;

    mpiAssert (MPI_Win_lock (SHARED, this->rank, lockAssertion, &this->array), "failed to acquire lock") ;
    mpiAssert (MPI_Get (&actualValue, 1, MPI_INT, targetRank, targetDisp, targetSize, MPI_INT, &this->array), "MPI Get failed while [] operation")  ;
    mpiAssert (MPI_Win_unlock (this->rank, &this->array), "failed to release lock") ;

    return actualValue ;
}

void Container<T>::setValue(const int &idx, const int &value) {

    mpiAssert (MPI_Win_lock (SHARED, this->rank, this->lockAssertion, &this->array), "failed to acquire lock while setting value") ;
    mpiAssert (MPI_Put (&value, 1, MPI_INT, targetRank, targetDisp, targetCount, MPI_INT, &this->array), "failed while assignment\n") ;
    mpiAssert (MPI_Win_unlock (this->rank, &this->array) ;
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
    MPI_Win_Flush (&this->array) ; // Synchronize the window.
}


template class Container<int>;
template class Container<float>;
template class Container<double>;
template class Container<bool>;


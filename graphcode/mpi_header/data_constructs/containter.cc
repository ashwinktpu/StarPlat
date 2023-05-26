#include"data_constructs.h"

template<typename T>
Container<T>::Container()
{
    
}

template<typename T>
Container<T>::Container(int size) : vect(size)
{

}

template<typename T>
Container<T>::Container(int size, T initial_value) : vect(size,initial_value)
{

}

template<typename T>
void Container<T>::push(T value)
{
    vect.push_back(value);
}

template<typename T>
void Container<T>::pop()
{
    vect.pop_back();
}

template<typename T>
void Container<T>::clear()
{
    vect.clear();
}

template<typename T>
T& Container<T>::operator[](int index)
{
    if(index>= vect.size())
    {
        std::cerr<<"Invalid index for cotainer access"<<std::endl;
        exit(-1);
    }
    return vect[index];
}

template<typename T>
void Container<T>::queue_assignment(int index, T value)
{

    
}

template<typename T>
void Container<T>::queue_push(T value)
{

    
}

template<typename T>
void Container<T>::sync_assignments()
{

}


template class Container<int>;
template class Container<float>;
template class Container<double>;
template class Container<bool>;


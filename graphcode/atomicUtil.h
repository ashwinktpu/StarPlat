#include<atomic>
#include<algorithm>

template<typename T>
inline void atomicMin(T* targetVar, T update_val)
{ 
    T oldVal, newVal;
  do {
       oldVal = *targetVar;
       newVal = std::min(oldVal,update_val);
       if (oldVal == newVal) 
            break;
     } while ( __sync_val_compare_and_swap(targetVar, oldVal, newVal) == false);
   

}
template<typename T>
inline void atomicAdd(T* targetVar, T update_val) {

    if (update_val == 0) return;

    T  oldValue, newValue;
    do {
        oldValue = *targetVar;
        newValue = oldValue + update_val;
    } while (!__sync_bool_compare_and_swap(targetVar,oldValue,newValue));
}

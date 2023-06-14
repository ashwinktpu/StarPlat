/********Add required atomics for float and double data types*******/


#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
inline void atomicAddD(volatile __global double *source, const double operand)
{
    union{
        long u64;
        double d64;
    } next, expected, current;
    current.d64 = *source;
    do{
        expected.d64 = current.d64;
        next.d64 = expected.d64 + operand;
        current.u64 = atom_cmpxchg((volatile __global long *)source, expected.u64, next.u64);
    }while(current.u64 != expected.u64);
}

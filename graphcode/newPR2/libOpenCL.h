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

inline void atomicAddF(volatile __global float *source, const float operand)
{
    union 
    {
        unsigned int u32;
        float d32;
    }next, expected, current;

    current.d32 = *source;

    do{
        expected.d32 = current.d32;
        next.d32 = expected.d32 + operand;
        current.u32 = atomic_cmpxchg((volatile __global unsigned int *)source, expected.u32, next.u32);
    }while (current.u32 != expected.u32);
    
}

// isNeighbour functionality to check if two vertices are neighbours--> used by TC

//  return (edge from u --> w)? 1: 0
int isNeighbour(int u, int w, __global int *d_offset, __global int *d_edgeList)
{
    int start = d_offset[u];
    int end = d_offset[u+1]-1;
    if(d_edgeList[start]==w || d_edgeList[end]==w)
    {
        return 1;
    }
    else
    {
        while(start<= end)
        {
            int mid = start + (end-start)/2;
            if(d_edgeList[mid]==w)
            {
                return 1;
                break;
            }
            else if(d_edgeList[mid]<w)
            {
                start = mid+1;
            }
            else
            {
                end = mid - 1;
            }

        }
        
    }
    return 0;
}


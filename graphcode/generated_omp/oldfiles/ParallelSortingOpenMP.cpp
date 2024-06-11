#include <omp.h>
#define TASK_LIMIT 1000
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <utility>
#include <iostream>

#ifndef H_UTILS
#define H_UTILS
#include <math.h>
#include <vector>
#include <utility>
#include <string>
#include <chrono>

// typedef std::milli time_ratio; // in what unit will we be measuring time.
// extern const std::string time_ratio_name;
#endif

using namespace std;

class ParallelSort
{
    public:
    ParallelSort(){
        omp_set_nested(1);
        int num_threads = omp_get_num_procs();
        omp_set_num_threads(num_threads);
    }

    template <class T>
    int partition(int p, int r, T* data){
        T pivot = data[p];
        int left = p;
        int right = r;
        while (left < right){
            // increase until you get to a point where the element is greater that the pivot
            while (left < r && data[left] <= pivot) ++left;
            // increase until you get to a point where the element is less or equal to the pivot
            while (right >= 0 && data[right] > pivot) --right;
            // swap if within bounds
            if (left < right && left <r && right >= 0){
                std::swap(data[left], data[right]);
            }
        }

        // swap at the end
        if (left < right && left <r && right >= 0){
            std::swap(data[left], data[right]);
        }
        data[p] = data[right];
        data[right] = pivot;
        return right;
    }

    template <class T>
    void seq_qsort(int p, int r, T* data) {
        if (p < r) {
            int q = partition<T>(p, r, data);
            seq_qsort(p, q - 1, data);
            seq_qsort(q + 1, r, data);
        }
    }
    template <class T>
    void q_sort_tasks(int p, int r, T* data, int low_limit) {
        if (p < r) {
            if (r - p < low_limit) 
            {
                // small list => sequential.
                return seq_qsort<T>(p, r, data);
            }else{

                int q = partition<T>(p, r, data);
                // create two tasks
                #pragma omp task shared(data)
                    q_sort_tasks<T>(p, q - 1, data, low_limit);
                #pragma omp task shared(data)
                    q_sort_tasks<T>(q + 1, r, data, low_limit);
            }
        } 
    }

    template <class T>
    void par_q_sort_tasks(int p, int r, T* data){
        cout << "p = " << p << ", r = " << r << endl;
        #pragma omp parallel
            {
                #pragma omp single
                q_sort_tasks<T>(p, r, data, TASK_LIMIT - 1);
                #pragma omp taskwait
            }
    }
};


// int main(){
    
//     ParallelSort ob;
//     int Ar[10] = {3,1,2,6,4,8,5,9,7,0};
//     ob.par_q_sort_tasks(0,9,Ar);
//     for(int i = 0;i<10;i++){
//         cout << Ar[i] << " ";
//     }
//     return 0;
// }
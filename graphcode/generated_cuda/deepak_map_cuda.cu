#include<iostream>
#include<sys/time.h>
#include<cuda.h>
#include<vector>
#include<cstdlib>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/iterator/reverse_iterator.h>

#define maxSize 10000000
#define block_size 1024

using namespace std;


int map_size = 100000002;


struct cmap{
    int key;
    int value;
    int fill;
};


struct element_pair{
    int key;
    int value;
    int find;
};

struct element{
    int key;
    int find;
};

cmap *d_map, *h_map;



__global__ void kernel_index_to_fill(cmap *d_map, int *d_index_to_fill_in_hash_table, int *counter, int map_size){
    
     int tid = blockDim.x * blockIdx.x + threadIdx.x;                                              
     if(tid < map_size){
      
          if(d_map[tid].fill == 0){               
              
                int index = atomicInc((unsigned *)counter, maxSize);
                d_index_to_fill_in_hash_table[index] = tid;                
          }    
        
     }                     
    
}

__global__ void kernel_to_insert(cmap *d_map, element_pair *d_input, int *d_index_to_fill_in_hash_table, int insert_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;                                              
    if(tid < insert_batch_size && d_input[tid].find==0){
          d_map[d_index_to_fill_in_hash_table[tid]].key = d_input[tid].key;
          d_map[d_index_to_fill_in_hash_table[tid]].value = d_input[tid].value;
          d_map[d_index_to_fill_in_hash_table[tid]].fill = 1; 
    }
    
}


__global__ void search_kernel_pair(element_pair *d_search_input_pair, cmap *d_map, int a_size, int search_batch_size){

    
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    
    if(tid < search_batch_size * a_size){

        int index_of_element = tid % search_batch_size;
        int index_of_location = tid / search_batch_size;

        if(d_search_input_pair[index_of_element].key == d_map[index_of_location].key
           && d_search_input_pair[index_of_element].value == d_map[index_of_location].value && d_map[index_of_location].fill == 1){
            
            d_search_input_pair[index_of_element].find = 1;
        }
    }
}


__global__ void search_kernel_key(element *d_search_input_key, cmap *d_map, int a_size, int search_batch_size, int *d_count){

    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < search_batch_size * a_size){

        int index_of_element = tid % search_batch_size;
        int index_of_location = tid / search_batch_size;

        if(d_search_input_key[index_of_element].key == d_map[index_of_location].key){

            int temp = atomicInc((unsigned int *)d_count, -1);
            //d_search_input_pair[index_of_element].find = 1;

        }
    }

}

__global__ void fill_search_kernel_key(element *d_search_input_key, cmap *d_map, int a_size, int search_batch_size, element_pair *d_search_input_key_ans, int *index){

    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < search_batch_size * a_size){

        int index_of_element = tid % search_batch_size;
        int index_of_location = tid / search_batch_size;

        if(d_search_input_key[index_of_element].key == d_map[index_of_location].key){
            int ind = atomicInc((unsigned int *)index, -1);
            d_search_input_key_ans[ind].key = d_map[index_of_location].key;
            d_search_input_key_ans[ind].value = d_map[index_of_location].value;
        }
    }
}



__global__ void delete_kernel_pair(element_pair *d_delete_input_pair, cmap *d_map, int a_size, int delete_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < delete_batch_size * a_size){

        int index_of_element = tid % delete_batch_size;
        int index_of_location = tid / delete_batch_size;

        if(d_delete_input_pair[index_of_element].key == d_map[index_of_location].key
           && d_delete_input_pair[index_of_element].value == d_map[index_of_location].value){

            d_map[index_of_location].key = 0;
            d_map[index_of_location].value = 0;
            d_map[index_of_location].fill = 0;
        }

    }

}

__global__ void delete_kernel(element *d_delete_input, cmap *d_map, int a_size, int delete_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < delete_batch_size * a_size){

        int index_of_element = tid % delete_batch_size;
        int index_of_location = tid / delete_batch_size;

        if(d_delete_input[index_of_element].key == d_map[index_of_location].key){

            d_map[index_of_location].key = 0;
            d_map[index_of_location].value = 0;
            d_map[index_of_location].fill = 0;
        }

    }

}



class Map{

  public:
    

  public:

    Map(){

      
      int map_element_counter = 0;

      
      h_map = (cmap*)malloc(map_size*sizeof(cmap));
      memset(h_map, 0, map_size*sizeof(cmap));

      cudaMalloc(&d_map, map_size*sizeof(cmap));
      cudaMemset(d_map, 0, map_size*sizeof(cmap));

    }

    void insertel(int insert_batch_size,  thrust::host_vector<thrust::host_vector<int>> &batch){        
        
      element_pair *d_input, *h_input;

      h_input = (element_pair*)malloc(insert_batch_size * sizeof(element_pair));

      for(int i=0 ; i<insert_batch_size ; i++){
          h_input[i].key = batch[i][0];
          h_input[i].value = batch[i][1];
          h_input[i].find = 0;
      }

      cudaMalloc(&d_input, insert_batch_size*sizeof(element_pair));
      cudaMemcpy(d_input, h_input, insert_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);

      int block = ceil((float)(map_size*insert_batch_size)/(float)block_size);

      search_kernel_pair<<<block, block_size>>>(d_input, d_map, map_size, insert_batch_size);

      cudaDeviceSynchronize();
      int *d_index_to_fill_in_hash_table, *counter;

      cudaMalloc(&d_index_to_fill_in_hash_table, map_size*sizeof(int));
      cudaMemset(d_index_to_fill_in_hash_table, 0, map_size*sizeof(int));

      cudaMalloc(&counter, sizeof(int));
      cudaMemset(counter, 0, sizeof(int));

      block = ceil((float)(map_size)/(float)block_size);

      kernel_index_to_fill<<<block, block_size>>>(d_map, d_index_to_fill_in_hash_table, counter, map_size);
      
      cudaDeviceSynchronize();

      block = ceil((float)(insert_batch_size)/(float)block_size);

      kernel_to_insert<<<block, block_size>>>(d_map, d_input, d_index_to_fill_in_hash_table, insert_batch_size);

      cudaDeviceSynchronize();

      cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

      
      // int count = 0;
      // for(int i=0 ; i<map_size ; i++){
      //     if(h_map[i].fill==1){
      //       cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
      //       count++;
      //     }
      // }

      // cout<<"Total Value1 = "<<count<<endl;
      // cout<<endl;


    }

    void searchPair(int search_batch_size, thrust::host_vector<thrust::host_vector<int>> &batch){

      element_pair *h_search_input_pair;
      element_pair *d_search_input_pair;


      h_search_input_pair = (element_pair*) malloc(search_batch_size * sizeof(element_pair));

      memset(h_search_input_pair, 0, search_batch_size*sizeof(element_pair));

      for(int i=0 ; i<search_batch_size ; i++){
          h_search_input_pair[i].key = batch[i][0];
          h_search_input_pair[i].value = batch[i][1];
      }

      cudaMalloc(&d_search_input_pair, search_batch_size*sizeof(element_pair));
      cudaMemcpy(d_search_input_pair, h_search_input_pair, search_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);

                                //existing array Size * batch_size
      int sblock = ceil((float)(map_size*search_batch_size)/(float)block_size);

      search_kernel_pair<<<sblock, block_size>>>(d_search_input_pair, d_map, map_size, search_batch_size);

      cudaMemcpy(h_search_input_pair, d_search_input_pair, search_batch_size*sizeof(element_pair), cudaMemcpyDeviceToHost);

      int flag = 1;

      // cout<<"\n\n\nFound value : \n";
      for(int i=0 ; i<search_batch_size ; i++){
          if(h_search_input_pair[i].find == 1){
            cout<<h_search_input_pair[i].key<<" "<<h_search_input_pair[i].value<<endl;
            // flag=0;
          }
      }

      cout<<endl;

    }


    void searchKey(int search_batch_size, thrust::host_vector<int> &keyvector){

      element *h_search_input_key;
      element *d_search_input_key;

      element_pair *h_search_input_key_ans;
      element_pair *d_search_input_key_ans;


      int *d_count, h_count, *d_index;


      cudaMalloc(&d_count, sizeof(int));
      cudaMemset(d_count, 0, sizeof(int));

      cudaMalloc(&d_index, sizeof(int));
      cudaMemset(d_index, 0, sizeof(int));

      h_search_input_key = (element*) malloc(search_batch_size * sizeof(element));

      memset(h_search_input_key, 0, search_batch_size*sizeof(element));

      for(int i=0 ; i<search_batch_size ; i++){
          h_search_input_key[i].key = keyvector[i];
      }

      cudaMalloc(&d_search_input_key, search_batch_size*sizeof(element));
      cudaMemcpy(d_search_input_key, h_search_input_key, search_batch_size*sizeof(element), cudaMemcpyHostToDevice);


                                //existing array Size * batch_size
      int sblock = ceil((float)(map_size*search_batch_size)/(float)block_size);

      search_kernel_key<<<sblock, block_size>>>(d_search_input_key, d_map, map_size, search_batch_size, d_count);

      cudaMemcpy(&h_count, d_count, sizeof(int), cudaMemcpyDeviceToHost);

      // cout<<"\nFound Entry with Keys = "<<h_count<<endl;

      h_search_input_key_ans = (element_pair*)malloc(h_count * sizeof(element_pair));
      memset(h_search_input_key_ans, 0, h_count*sizeof(element_pair));


      cudaMalloc(&d_search_input_key_ans, h_count*sizeof(element_pair));
      cudaMemset(d_search_input_key_ans, 0, h_count*sizeof(element_pair));


                                //existing array Size * batch_size
      sblock = ceil((float)(map_size*search_batch_size)/(float)block_size);

      fill_search_kernel_key<<<sblock, block_size>>>(d_search_input_key, d_map, map_size, search_batch_size, d_search_input_key_ans, d_index);



      cudaMemcpy(h_search_input_key_ans, d_search_input_key_ans, h_count*sizeof(element_pair), cudaMemcpyDeviceToHost);

      // cout<<"\n\n\nFound value with keys : \n";
      for(int i=0 ; i<h_count ; i++){
         
          cout<<h_search_input_key_ans[i].key<<" "<<h_search_input_key_ans[i].value<<endl;
       
          
      }

      
    }

    void deletePair(int delete_batch_size, thrust::host_vector<thrust::host_vector<int>> &batch){

      element_pair *h_delete_input_pair;
      element_pair *d_delete_input_pair;

      h_delete_input_pair = (element_pair*) malloc(delete_batch_size * sizeof(element_pair));

      memset(h_delete_input_pair, 0, delete_batch_size*sizeof(element_pair));

      for(int i=0 ; i<delete_batch_size ; i++){
          h_delete_input_pair[i].key = batch[i][0];
          h_delete_input_pair[i].value = batch[i][1];
      }

      cudaMalloc(&d_delete_input_pair, delete_batch_size*sizeof(element_pair));
      cudaMemcpy(d_delete_input_pair, h_delete_input_pair, delete_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);


                                //existing array Size * batch_size
      int sblock = ceil((float)(map_size*delete_batch_size)/(float)block_size);

      delete_kernel_pair<<<sblock, block_size>>>(d_delete_input_pair, d_map, map_size, delete_batch_size);

      cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

      // cout<<"\n\n After Delettion:\n\n";
      // cout<<endl;
      // for(int i=0 ; i<map_size ; i++){
      //     if(h_map[i].fill==1)
      //       cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
      //     //cout<<h_map[i].value<<" ";
      // }



    }

    void deleteKey(int delete_batch_size, thrust::host_vector<int> &keyvector){

      element *h_delete_input;
      element *d_delete_input;

      h_delete_input = (element*) malloc(delete_batch_size * sizeof(element));

      memset(h_delete_input, 0, delete_batch_size*sizeof(element));

      for(int i=0 ; i<delete_batch_size ; i++){
          h_delete_input[i].key = keyvector[i];
      }

      cudaMalloc(&d_delete_input, delete_batch_size*sizeof(element));
      cudaMemcpy(d_delete_input, h_delete_input, delete_batch_size*sizeof(element), cudaMemcpyHostToDevice);

                                //existing array Size * batch_size
      int sblock = ceil((float)(map_size*delete_batch_size)/(float)block_size);

      delete_kernel<<<sblock, block_size>>>(d_delete_input, d_map, map_size, delete_batch_size);

      cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

      // cout<<"\n\n After Delettion:\n\n";
      // cout<<endl;
      // for(int i=0 ; i<map_size ; i++){
      //     if(h_map[i].fill==1)
      //       cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
      //     //cout<<h_map[i].value<<" ";
      // }




    }




};


// int main(){

//   Map mp;

//   int insertSize = 10;
//   thrust::host_vector<thrust::host_vector<int>> insertEl(insertSize, thrust::host_vector<int>(2,0));
    


//   /*Start of Dummy Input*/
//   insertEl[0][0] = 59;
//   insertEl[0][1] = 6;
//   // workDone[0] = 0;
//   // searchDone[0] = 0;

//   insertEl[1][0] = 59;
//   insertEl[1][1] = 8;
//   // workDone[1] = 0;
//   // searchDone[1] = 0;

//   insertEl[2][0] = 59;
//   insertEl[2][1] = 9;
//   // workDone[2] = 0;
//   // searchDone[2] = 0;

//   for(int i=3 ; i<insertSize ; i++){
//       insertEl[i][0] = i;
//       insertEl[i][1] = i+7;
//       // workDone[i] = 0;	//This is important
//       // searchDone[i] = 0;
//   }
//   // /*End of Dummy Input*/

  
//   // //cout<<"size = "<<sizeof(insertEl)/sizeof(insertEl[0]);
//   // int sz = sizeof(insertEl)/sizeof(insertEl[0]);
//   int sz=10;
//   mp.insert(sz, insertEl);

//   insertEl[0][0] = 58;
//   insertEl[0][1] = 6;
//   // workDone[0] = 0;
//   // searchDone[0] = 0;

//   insertEl[1][0] = 58;
//   insertEl[1][1] = 8;
//   // workDone[1] = 0;
//   // searchDone[1] = 0;

//   insertEl[2][0] = 58;
//   insertEl[2][1] = 9;
//   // workDone[2] = 0;
//   // searchDone[2] = 0;

//   for(int i=3 ; i<insertSize ; i++){
//       insertEl[i][0] = i;
//       insertEl[i][1] = i+7;
//       // workDone[i] = 0;	//This is important
//       // searchDone[i] = 0;
//   }

//   mp.insertel(sz, insertEl);

//   sz = 3;
//   vector<vector<int>> searchEl(sz, vector<int>(2,0));
    
//   searchEl[0][0] = 58;
//   searchEl[0][1] = 6;
//   // workDone[0] = 0;
//   // searchDone[0] = 0;

//   searchEl[1][0] = 58;
//   searchEl[1][1] = 8;
//   // workDone[1] = 0;
//   // searchDone[1] = 0;

//   searchEl[2][0] = 58;
//   searchEl[2][1] = 9;
  
//   // for(int i=3 ; i<insertSize ; i++){
//   //     insertEl[i][0] = i;
//   //     insertEl[i][1] = i+7;
//   //     // workDone[i] = 0;	//This is important
//   //     // searchDone[i] = 0;
//   // }

//   // mp.search_pair(sz, searchEl);

//   // sz = 2;

//   // vector<int> skey(sz,0);

//   // skey[0] = 58;
//   // skey[1] = 3;

//   // mp.search_key(sz, skey);

//   // mp.delete_pair(sz, searchEl);

//   sz = 2;

//   thrust::host_vector<int> skey(sz,0);

//   skey[0] = 3;
//   skey[1] = 4;

//   mp.deleteKey(sz, skey);

//   skey[0] = 58;
//   skey[1] = 59;

//   mp.searchKey(sz, skey);


//   return 0;
// }
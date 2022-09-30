

# StarPlat: A Versatile DSL for Graph Analytics




| Branches   | Build Status | Generation |  
| -------------  | ------------- |------------- |  
| Working | [![Working Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml/badge.svg?branch=WorkingBranch)](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml) | [![Working Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-omp.yml/badge.svg?branch=WorkingBranch)](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-omp.yml) [![Working Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-cuda.yml/badge.svg?branch=WorkingBranch)](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-cuda.yml) | //TODO | 
| CUDA    | [![CUDA Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml/badge.svg?branch=cuda)](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml)  |   //TODO |   
| OMP    | [![OMP Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/omp.yml/badge.svg?branch=openmp)](https://github.com/nibeditabh/GraphDSL/actions/workflows/omp.yml)    |  //TODO  |   
| MPI    | [![MPI Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/mpi.yml/badge.svg?branch=updatedMPI)](https://github.com/nibeditabh/GraphDSL/actions/workflows/mpi.yml)    |  //TODO  |   



# How to build

```

# Step 1. Clone the repo from our Main branch
git clone -b WorkingBranch https://github.com/nibeditabh/GraphDSL.git

# Step 2. Create a StarPlat compiler
cd GraphDSL/src
make
```
# How to generate
```
# Step 3. Generate files from DSL files.  
./StarPlat [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc]

#Example
./StarPlat -s -f ../graphcode/generated_cuda/sssp_dslV2 -b omp

-s for static and -d for dynamic
-b select the type of backend
-f the dsl file to input
```
# How to complile generated codes
## SSSP
```
# Step 4.0 Append the main function to the generated file .cu or .cc file
# Below is the E.g. how to compile SSSP file after generation
# Additional equired files: graph.hpp libcuda.hxx present in one level up

# Step 4.1 Compile and run CUDA algorithm
nvcc -O3 -std=c++14 ../graphcode/generated_cuda/sssp_dslV2.cu -I .. -o ssspCuda.out
./ssspCuda.out ../dataset/inputfile.txt

# Step 4.2 Compile and run OMP algorithm
g++ -O3 -fopenmp -std=c++14 ../graphcode/generated_omp/sssp_dslV2.cu -I .. -o ssspOmp.out
./ssspOmp.out ../dataset/inputfile.txt

# Step 4.3 Compile and run MPI algorithm
mpcxx -fopenmp -std=c++14 ../graphcode/generated_mpi/sssp_dslV2.cu -I .. -I boost_1_74_0/ -o ssspMpi.out
./ssspMpi.out ../dataset/inputfile.txt

# Step 4.4 Compile and run ACC algorithm
pgc++ -std=c++14 ../graphcode/generated_OpenACC/sssp_dslV2.cu -I .. -o ssspAcc.out
./ssspAcc.out ../dataset/inputfile.txt

```


Graph DSL for basic graph algorithms 
- Phase1 (static)  **SSSP, BC, PR, and TC**


[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

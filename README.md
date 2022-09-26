

# GraphDSL

[![CUDA Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml/badge.svg?branch=cuda)](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml)

[![Working Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml/badge.svg?branch=WorkingBranch)](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml)


| Branches   | Build Status | Generation | Running |
| -------------  | ------------- |------------- |------------- |
| Working | [![Working Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml/badge.svg?branch=WorkingBranch)](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml) | [![Working Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-omp.yml/badge.svg?branch=WorkingBranch)](https://github.com/nibeditabh/GraphDSL/actions/workflows/working-omp.yml)| |
| CUDA    | [![CUDA Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml/badge.svg?branch=cuda)](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml)  |   |   |
| OMP    | [![OMP Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/omp.yml/badge.svg?branch=openmp)](https://github.com/nibeditabh/GraphDSL/actions/workflows/omp.yml)    |   |   |


# How to build.

```

# Step 1. Clone the repo from our Main branch
git clone -b WorkingBranch https://github.com/nibeditabh/GraphDSL.git

# Step 2. Create a StarPlat compiler
cd GraphDSL/src
make

# Step 3. Generate files from DSL files.  
./StarPlat [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc]

#Example
./StarPlat -s -f ../graphcode/generated_cuda/sssp_dslV2 -b omp

-s for static and -d for dynamic
-b select the type of backend
-f the dsl file to input


# Step 4.0 Copy and Paste the main function to generate file .cu or .cc file
# Below is the E.g. how to compile SSSP file afte generation

# Step 4.1 Compile and run CUDA algorithms
nvcc -std=c++14 ../graphcode/generated_cuda/sssp_dslV2.cu -I . -o ssspCuda.out
./ssspCuda.out ../dataset/inputfile.txt

# Step 4.2 Compile and run OMP algorithms
g++ -fopenmp -std=c++14 ../graphcode/generated_omp/sssp_dslV2.cu -I . -o ssspOmp.out
./ssspOmp.out ../dataset/inputfile.txt

# Step 4.3 Compile and run MPI algorithms
mpcxx -fopenmp -std=c++14 ../graphcode/generated_mpi/sssp_dslV2.cu -I . -o ssspMpi.out
./ssspMpi.out ../dataset/inputfile.txt

# Step 4.4 Compile and run ACC algorithm
pgc++ -std=c++14 ../graphcode/generated_OpenACC/sssp_dslV2.cu -I . -o ssspAcc.out
./ssspAcc.out ../dataset/inputfile.txt

```


Graph DSL for basic graph algorithms (such as **SSSP, BC, PR, and TC**) as Phase1 (static).
It contains the following steps :-

1. Generating the grammar using Lex and YACC 
2. Generating IR. 
3. Generate code from IR. 
4. Optomize the IR. 


[WBBuild]: https://github.com/nibeditabh/GraphDSL/actions/workflows/working-branch.yml/badge.svg?branch=WorkingBranch
[CUBuild]: https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml/badge.svg?branch=cuda

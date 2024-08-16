
# GraphDSL
[![CUDA Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml/badge.svg?branch=cuda)](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml)



# StarPlat: A Versatile DSL for Graph Analytics
# GraphDSL MPI Backend

### How to compile Generated MPI Codes on Aqua Cluster
```
$ module load openmpi316
$ mpicxx -I/lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/include program.cc ../boost/install_dir/lib/libboost_mpi.a ../boost/install_dir/lib/libboost_serialization.a -o output  

(Assuming your parent directory contains boost library files)
Done with the compilation!

```

### How to run MPI programs

```
$ /lfs/sware/openmpi316/bin/mpirun -np 64 -hostfile $PBS_NODEFILE $PBS_O_WORKDIR/output /lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs/sinaweibowt.txt > $PBS_O_WORKDIR/output_logfile_name.txt 

(The CS16d003 contains all the graphs which we are using)
Done with running!
```

### Sample Script to run on Aqua Cluster

```
#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile_slash.err
#PBS -l walltime=00:60:00
#PBS -l select=2:ncpus=32
#PBS -q rupesh_gpuq

module load openmpi316

#tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
#tempdir=$HOME/scratch/job$tpdir
#mkdir -p $tempdir
#cd $tempdir

#Compilation not needed already done
#mpicxx -I/lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/include triangle_count_dsl.cc ../boost/install_dir/lib/libboost_mpi.a ../boost/install_dir/lib/libboost_serialization.a -o tc_exe

#Execution
/lfs/sware/openmpi316/bin/mpirun -np 64 -hostfile $PBS_NODEFILE $PBS_O_WORKDIR/sssp_exe /lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs/sinaweibowt.txt > $PBS_O_WORKDIR/output_sinaweibo.txt

#mv ../job$tpdir $PBS_O_WORKDIR/.

```
### Sample main() for generated CC  (SSSP)

```
int main(int argc, char* argv[])
{

  mpi::environment env(argc, argv);
  graph G1(argv[1]);
  int src=0;
  int* dist;
  Compute_SSSP(G1,dist,src);

  return 0;
}

```


### Aqua Cheats
```
$ qsub script.cmd : To run/subscribe script on specified queue
$ qstat : Gives the status of scripts running 
$ qdel <processid> : Removes script from queue
```




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

## Acknowledgments
```
This project is funded by India's National Supercomputing Mission, whose timely funding and regular reviews are greatly appreciated.
```

## Licensing
The StarPlat codebase is made available for academic and non-commercial use.
Commercial use of the code needs licensing. The interested may contact rupesh@cse.iitm.ac.in.

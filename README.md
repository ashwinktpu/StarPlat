
# GraphDSL
[![CUDA Branch](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml/badge.svg?branch=cuda)](https://github.com/nibeditabh/GraphDSL/actions/workflows/cuda.yml)



# StarPlat: A Versatile DSL for Graph Analytics

```diff
- Please use WorkingBranch
```

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

# How to Build (Mac OS | Tested on M1)

By default, MacOS comes with the `clang` compiler, which can cause syntax errors when running the `make` command due to differences between `gcc` and `clang` semantics. To resolve this, follow these steps:

## Installation:

1. Install `gcc` using Homebrew:

   ```sh
   brew install gcc
   ```

2. Check the installation location of the `g++` binaries:

   ```sh
   which brew
   ll /opt/homebrew/bin/g++-*
   ```

3. Create symbolic links to the installed version, (presumes that the above command returns gcc-13):

   ```sh
   sudo ln -sf /opt/homebrew/bin/gcc-13 /usr/local/bin/gcc
   sudo ln -sf /opt/homebrew/bin/g++-13 /usr/local/bin/g++
   sudo ln -sf /opt/homebrew/bin/c++-13 /usr/local/bin/c++
   sudo ln -sf /opt/homebrew/bin/cpp-13 /usr/local/bin/cpp
   ```

   Note: Modifying anything in `/usr/bin/*` is discouraged and may require disabling SIP (System Integrity Protection). Therefore, we create symlinks to `/usr/local/bin/*`.

4. After installation, check the versions:

   ```sh
   g++ --version # Should return clang version
   g++-13 --version # Should return (Homebrew GCC)
   ```

5. Update the Makefile (`StarPlat/src/Makefile`) to use the symlinked `g++`:

   ```make
   CC = /usr/local/bin/g++
   ```
6. Add `%token return_func` in the file `lrparser.y` (`StarPlat/src/parser/lrparser.y`)
 

# How to generate
```
# Step 3. Generate files from DSL files.  
./StarPlat [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc|sycl]

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

# Miscellaneous
## Installing Boost Libraries to run StarPlat for MPI

On the root directory of `StarPlat`, running the command:

```sh
grep -r '<boost/' .
```

shows the files that use boost. This section provides a walkthrough of building, installing, and linking the boost libraries and source to create a program. The workflow has been tested on an arm64 architecture with a 64-bit address type on a Mac machine. While this guide is compiled from various sources, for a comprehensive understanding, please refer to the official boost documentation.

Our main goal is to build the following file with `mpic++` and run with `mpirun`:

```c++
#include <iostream>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

namespace mpi = boost::mpi;

int main() {
    mpi::environment env;
    mpi::communicator world;
    std::cout << "I am process " << world.rank() << " of " << world.size() << "." << std::endl;
    return 0;
}
```

The default compiler that comes with MacOS is clang, and StarPlat is built on `gcc`. Note that a C++ library compiled with GCC is not compatible with Clang and vice versa. So, we will build the libraries and code exclusively on one platform.

There are options to compile boost as a universal library agnostic to architecture (discussed [here](https://stackoverflow.com/questions/64553398/compile-boost-as-universal-library-intel-and-apple-silicon-architectures)), but for brevity, this walkthrough aims to run it on gcc/g++.

We will be building the boost library from the source, providing more autonomy and control over packages and libraries, and installation directories from where we can include headers and link libraries.

- **Step 1**: Download the tar file from boost/downloads.
- **Step 2**: Extract the tar file to a location using the command `tar -xvf boost.tar.gz`.
- **Step 3**: Create a directory anywhere with `mkdir -p /path/to/boost_version`.
- **Step 4**: Navigate to the extracted boost directory.
- **Step 5**: Run `./bootstrap.sh --prefix=/path/to/boost_version` to extract all the headers and binaries in the specified prefix location.
- **Step 6**: The bootstrap.sh will create a file called `project-config.jam`.
- **Step 7**: In the `project-config.jam` file, comment out the section related to the compiler configuration.

```jam
# Compiler configuration. This definition will be used unless
# you already have defined some toolsets in your user-config.jam
# file.
# if ! clang in [ feature.values <toolset> ]
# {
#    using clang ;
# }

# project : default-build <toolset>clang ;
```

- **Step 8**: Configure the project-config.jam to use gcc.

```jam
using gcc : <version> : /path/to/gcc
# Example
using gcc : 13.2 : /usr/local/bin/gcc
```

- **Step 9**: Install MPI by following the steps [here](https://docs.open-mpi.org/en/v5.0.x/installing-open-mpi/quickstart.html).
- **Step 10**: Add the following line to the `project-config.jam` file (notice the space between mpi and ;).

```jam
using mpi ;
```

- **Step 11**: Run the command `./b2 cxxflags=-std=c++17 install` to build boost with the standard library of choice and install it. If you encounter permission denied errors, use `sudo` before the command.

- **Step 12**: If your editor's autocomplete looks for environment variables for autocompletion and code completion, run:

```sh
export DYLD_LIBRARY_PATH=/path/to/boost/lib/:$DYLD_LIBRARY_PATH
```

- **Step 13**: Copy the following content:

```c++
#include <iostream>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

namespace mpi = boost::mpi;

int main() {
    mpi::environment env;
    mpi::communicator world;
    std::cout << "I am process " << world.rank() << " of " << world.size() << "." << std::endl;
    return 0;
}
```

```sh
OMPI_CXX=g++ mpic++ -I/Users/durwasa/boost/include -L/Users/durwasa/boost/lib helloworld.cpp -o helloworld -lboost_mpi
```

The `OMPI_CXX` sets the environment variable to `gcc` instead of `clang` (since `mpic++` has been installed using Homebrew/MacPorts). `-I` includes the header files, `-L` links the library, and `-lboost_mpi` links the boost.mpi, which we configured using our .jam file.

- **Step 14**: Run the command `OMPI_CXX=g++ mpirun helloworld`. If the default compiler in the system is `gcc`, omit the `OMPI_CXX` flag. `mpicxx` and other variants are technically compiler wrappers that translate the compiler commands and inject flags into it. Using the `OMPI_CXX` flag specifies which C++ compiler it should use while injecting the flags. Learn more about compiler wrappers [here](https://www.ibm.com/docs/en/smpi/10.2?topic=administering-compiling-applications).

Graph DSL for basic graph algorithms 
- Phase1 (static)  **SSSP, BC, PR, and TC**

## Acknowledgments
```
This project is funded by India's National Supercomputing Mission, whose timely funding and regular reviews are greatly appreciated.
```
## Licensing
The StarPlat codebase is made available for academic and non-commercial use.
Commercial use of the code needs licensing. The interested may contact rupesh@cse.iitm.ac.in.

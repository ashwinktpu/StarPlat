#!/bin/bash
#PBS -e errorlog1.err
#PBS -o logSSSP.log
#PBS -q gpuq
#PBS -l select=1:ncpus=1:ngpus=1

tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
cp -R $PBS_O_WORKDIR/* .
module load cuda10.1
module load gcc640
nvcc -o "BC1".out "graphcode/generated_cuda/bc_dslV21.cu"  -std=c++14 -arch=sm_70 -rdc=true
./BC1.out /lfs1/usrscratch/phd/cs16d003/11suiteDSL/USAud.txt > outputBC1.txt
rm *.out
mv * $PBS_O_WORKDIR/.
rmdir $tempdir

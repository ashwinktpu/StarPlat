#!/bin/bash
#PBS -e errorlog1.err
#PBS -o logSSSP.log
#PBS -q rupesh_gpuq
#PBS -l select=1:ncpus=1:ngpus=1

tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
cp -R $PBS_O_WORKDIR/* .
module load cuda10.1
module load gcc640
nvcc -o $tempdir/"MST_V1_Manual".out "../MST_V1_Manual.cu"  -std=c++14  #-arch=sm_70
./$tempdir/MST_V1_Manual.out < input1.txt > output_MST_V1_Manual.txt

rm *.out
mv * $PBS_O_WORKDIR/.
rmdir $tempdir

# /lfs1/usrscratch/phd/cs16d003/11suiteDSL
# qsub run.sh
# qstat
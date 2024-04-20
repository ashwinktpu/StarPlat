#!/bin/bash
#PBS -e errorfile.err
#PBS -o logfile.log
#PBS -l walltime=40:00:00
#PBS -l select=1:ngpus=1
#PBS -q rupesh_gpuq
tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
cp $PBS_O_WORKDIR/* .

g++ main.cpp -lOpenCL
filename=samplegraph.txt
./a.out $filename >>output 2>>output
mv ../job$tpdir $PBS_O_WORKDIR/.

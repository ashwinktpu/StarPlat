#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile_slash.err
#PBS -l walltime=00:60:00
#PBS -l select=2:ncpus=40

tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
mpicxx -O3 -std=c++11 $PBS_O_WORKDIR/sssp_dslV2.cc -L/lfs/usrhome/phd/cs20d012/boost_1_76_0/stage/lib -lboost_mpi -lboost_serialization -o slash_exe
#mpicxx -O3 $PBS_O_WORKDIR/sssp_sparse_collComm.cc -o slash_exe
mpiexec -np 80 -ppn 40 -hostfile $PBS_NODEFILE ./slash_exe /lfs/usrhome/phd/cs20d012/scratch/soc-Slashdot0811.txt 80 > $PBS_O_WORKDIR/outputSlash80.txt
#rm a.out
mv ../job$tpdir $PBS_O_WORKDIR/.

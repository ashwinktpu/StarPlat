#!/bin/bash
#PBS -e errorLogAPFB.err
#PBS -o logAPFB.log
#PBS -q rupesh_gpuq
#PBS -l select=1:ncpus=1:ngpus=1

# rmatud.txt  sinaweiboud.txt  u10m_80mud.txt  wikiud.txt
tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
cleandir=/lfs1/usrscratch/phd/cs16d003/11suiteDSL
tempdir=$HOME/scratch/job$tpdir
mkdir -p $tempdir
cd $tempdir
# pwd is nothing but $tempdir
cp -R $PBS_O_WORKDIR/* . # PBS_O_WORKDIR is the directory from which the job was submitted
module load cuda10.1
module load gcc640
nvcc -o "APFB_V1".out "generated_cuda/APFB_V1_main.cu"  -std=c++14  #-arch=sm_70
# time (./APFB_V1.out $cleandir/GermanyRoadud.txt 0 > output_APFB_V1.txt) # 0 - Unweighted Graph
time (./APFB_V1.out bipartite1.txt 0 > output_APFB_V1.txt) # 0 - Unweighted Graph

rm *.out
# move everything from $tempdir to $PBS_O_WORKDIR
cp -R * $PBS_O_WORKDIR/. && rm -r *
rmdir $tempdir

# /lfs1/usrscratch/phd/cs16d003/11suiteDSL
# qsub run.sh
# qstat
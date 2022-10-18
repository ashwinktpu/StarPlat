#!/bin/bash
#PBS -e errorLogMST.err
#PBS -o logMST.log
#PBS -q gpuq
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

echo "" > output_MST_V1_Manual.txt

nvcc -o "MST_V1_Manual".out "generated_cuda/MST_V2_Verified.cu"  -std=c++14  -arch=sm_70

# time (./MST_V1_Manual.out $cleandir/GermanyRoadud.txt 0 > output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/cleancom-orkutud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/livud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/rmatud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/u10m_80mud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/USAud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/livjournalud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
time (./MST_V1_Manual.out $cleandir/pokecud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/sinaweiboud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/twitud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph
# time (./MST_V1_Manual.out $cleandir/wikiud.txt 0 >> output_MST_V1_Manual.txt) # 0 - Unweighted Graph

rm *.out
# move everything from $tempdir to $PBS_O_WORKDIR
cp -R * $PBS_O_WORKDIR/. && rm -r *
rmdir $tempdir

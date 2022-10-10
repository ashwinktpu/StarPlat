#!/bin/bash
#PBS -e errorLogAPFB.err
#PBS -o logAPFB.log
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

nvcc -o "APFB_V1".out "generated_cuda/APFB_V1_main.cu"  -std=c++14  #-arch=sm_70

echo "" > output_APFB_V1.txt

INPUT="dblp/dblp_clean.txt"
LEFT="1953085"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

INPUT="reuters/reuters_clean.txt"
LEFT="781265"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

INPUT="nytimes/nytimes_clean.txt"
LEFT="299752"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

INPUT="livejournal/livejournal_clean.txt"
LEFT="3201203"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

INPUT="web_trackers/web_trackers_clean.txt"
LEFT="27665730"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

INPUT="trec/trec_clean.txt"
LEFT="556077"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

INPUT="yahoo_song/yahoo_song_clean.txt"
LEFT="1000990"
./APFB_V1.out "/lfs/usrhome/btech/cs19b080/konect_aqua/$INPUT" 0 $LEFT >> output_APFB_V1.txt

rm *.out
# move everything from $tempdir to $PBS_O_WORKDIR
cp -R * $PBS_O_WORKDIR/. && rm -r *
rmdir $tempdir


#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile.err
#PBS -l walltime=12:00:00
#PBS -l select=1:mpiprocs=32:ncpus=32
#PBS -q rupesh_gpuq

export PMIX_MCA_gds=hash
export LD_LIBRARY_PATH=/lfs1/usrscratch/mtech/cs22m028/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH

lscpu

module load openmpi411
wait

path=/lfs/usrhome/mtech/cs22m028/max-flow/distributed_bfs
graphs=/lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs

file_names=( "u10m80mwt.txt")
#file_names=("pokecwt.txt" "livjournalwt.txt" "orkutudwt.txt" "twitterwt.txt" "sinaweibowt.txt" "pokecwt.txt" "livjournalwt.txt" "orkutudwt.txt" "twitterwt.txt" "sinaweibowt.txt")
cd $path/distributedv2.0
mpic++ -std=c++17 -O3 -O2 -Ofast -o testing_distributed.out init_maxflow.cpp 

for file_name in "${file_names[@]}"; do
		echo $graphs/$file_name >>../evaluation/output.txt
	if [ -e "$graphs/$file_name" ]; then
		echo "Processing $file_name"
		echo $graphs/$file_name
		now=$(date)
		echo $now >>../evaluation/output.txt
		echo $graphs/$file_name >>../evaluation/output.txt
		cd $path/evaluation
		cp $graphs/$file_name input.txt
		wait
		echo "copy OK"
		wait
		g++ -std=c++17 splitter.cpp -o splitter
		./splitter
		echo "splitter OK" 
		wait
		cd $path/distributedv2.0
		/lfs/sware/openmpi411/bin/mpirun -np 32 testing_distributed.out >>../evaluation/output.txt 
		echo "MPI OK"
		wait
	else
		echo "$file_name not found in $folder_path"
	fi
done
rm testing_distributed.out

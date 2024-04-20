
#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile.err
#PBS -l walltime=24:00:00
#PBS -l select=3:mpiprocs=96:ncpus=96
#PBS -q rupesh_gpuq

export PMIX_MCA_gds=hash
export LD_LIBRARY_PATH=/lfs1/usrscratch/mtech/cs22m028/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH=/lfs/usrhome/mtech/cs22m028/local/lib:$LD_LIBRARY_PATH
lscpu

module load openmpi411
wait

path=/lfs/usrhome/mtech/cs22m028/max-flow/wiki_bench/evaluation
graphs=/lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs

cd $path
cd ../metis

COMPILER=g++
FLAGS="-std=c++17" # Add any additional flags you might need
METIS_INCLUDE_PATH="/lfs/usrhome/mtech/cs22m028/local/include/" # Set this to the path of your METIS library, if required
METIS_LIB_PATH="/lfs/usrhome/mtech/cs22m028/local/lib/" # Set this to the path of your METIS include files, if required
GKLIB_INCLUDE_PATH="/lfs/usrhome/mtech/cs22m028/GKlib"

export LD_LIBRARY_PATH=/lfs/usrhome/mtech/cs22m028/local/lib:$LD_LIBRARY_PATH
# Compile the main program and its modules
$COMPILER $FLAGS -o main_program main.cpp graph_conversion.cpp csr_conversion.cpp metis_integration.cpp partitioned_writer.cpp -I$METIS_INCLUDE_PATH -IGKLIB_INCLUDE_PATH -L$METIS_LIB_PATH -LGKLIB_INCLUDE_PATH -lmetis -lGKlib

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
else
    echo "Compilation failed."
fi


file_name=("wikiwt.txt")
cd ../distributedv2.0
mpic++ -std=c++17 -O3 -O2 -Ofast -o testing_distributed.out init_maxflow.cpp 

for file_name in "${file_names[@]}"; do
	if [ -e "$graphs/$file_name" ]; then
		echo "Processing $file_name"
		echo $graphs/$file_name
		now=$(date)
		echo $now >>../evaluation/output.txt
		echo $graphs/$file_name >>../evaluation/output.txt
		echo "file name = $filename"
		rm ../part_*
		cd ../metis
		echo "file name = $filename"
		echo $graphs/$filename
    	./main_program $graphs/$file_name 32
		echo "splitter OK" 
		wait
		cd ../distributedv2.0
		/lfs/sware/openmpi411/bin/mpirun -np 32 testing_distributed.out >>../evaluation/output.txt 
		echo "MPI OK"
		wait
	else
		echo "$file_name not found in $folder_path"
	fi
done
rm testing_distributed.out

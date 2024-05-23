
#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile.err
#PBS -l walltime=24:00:00
#PBS -l select=1:mpiprocs=4:ncpus=4
#PBS -q rupesh_gpuq

export PMIX_MCA_gds=hash
export LD_LIBRARY_PATH=/lfs1/usrscratch/mtech/cs22m028/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH=/lfs/usrhome/mtech/cs22m028/local/lib:$LD_LIBRARY_PATH
lscpu

module load openmpi411
wait

path=/lfs/usrhome/mtech/cs22m028/benchmarkDSL/MetisIntegration/header_test_exp
graphs=/lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs

cd $path
echo Paht = $pwd
cd metis

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


file_names=("pokecwt.txt")
cd ../
make galloisDSL.out

for file_name in "${file_names[@]}"; do
	echo $graphs/$file_name
	if [ -e "$graphs/$file_name" ]; then
		echo "Processing $file_name"
		echo $graphs/$file_name
		now=$(date)
		echo "file name = $filename"
		rm part_*
		cd metis
		echo "file name = $filename"
		echo $graphs/$filename
    	./main_program $graphs/$file_name 4 
		echo "splitter OK" 
		wait
		cd ../
		#/lfs/sware/openmpi411/bin/mpirun -np 40 testing_distributed.out >>../evaluation/output.txt 
    /lfs/sware/openmpi411/bin/mpiruni -np 4 galloisDSL.out input.txt output.txt
		echo "MPI OK"
		wait
	else
		echo "$file_name not found in $folder_path"
	fi
done

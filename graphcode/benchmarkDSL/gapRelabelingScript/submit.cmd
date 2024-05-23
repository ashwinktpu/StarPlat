
#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile.err
#PBS -l walltime=24:00:00
#PBS -l select=1:ncpus=20:mpiprocs=20
#PBS -q rupesh_gpuq


export PMIX_MCA_gds=hash
export LD_LIBRARY_PATH=/lfs1/usrscratch/mtech/cs22m028/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/lfs/usrhome/mtech/cs22m028/local/lib:$LD_LIBRARY_PATH
module load openmpi411

currDir=$PBS_O_WORKDIR
cd $currDir

echo $date
echo $(pwd)

# change directory into a generated_mpi
#cd ../generated_mpi

# compile the source and header files
make clean
make graph_gen
make control
make anupDsl.out

if [$? -eq 0]; then
	echo "make file success"
else
	echo "make file failure"
fi

# test and mark timings
for i in {1..20}
do
	echo testing test case $i
	./graph_gen.out $i $((50*$i))
	./control.out input.txt > outputControl.txt
	echo "size of fize = $((50*$i))" >> timingsFile
	/lfs/sware/openmpi411/bin/mpirun -np 20 anupDsl.out input.txt outputActual.txt >> timingsFile
	diff -q outputControl.txt outputActual.txt
done

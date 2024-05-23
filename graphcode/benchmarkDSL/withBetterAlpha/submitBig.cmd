
#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile.err
#PBS -l walltime=24:00:00
#PBS -l select=1:ncpus=32:mpiprocs=32
#PBS -q rupesh_gpuq


export PMIX_MCA_gds=hash
export LD_LIBRARY_PATH=/lfs1/usrscratch/mtech/cs22m028/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/lfs/usrhome/mtech/cs22m028/local/lib:$LD_LIBRARY_PATH
module load openmpi411

currDir=$PBS_O_WORKDIR
cd $currDir

echo $date
echo $(pwd)


# compile the source and header files
make clean
make graph_gen
make control
make anupDslAnalysis.out


graphPath=/lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs

for graph in $graphPath/*
do
	echo $graph
	echo $graph >> BigTimingsFile
	timeout 1h /lfs/sware/openmpi411/bin/mpirun -np 32 anupDslAnalysis.out $graph outputActual.txt >> BigTimingsFile
done

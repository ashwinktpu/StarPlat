#!/bin/bash

# Define the compiler and the necessary flags
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
    ./main_program /lfs1/usrscratch/phd/cs16d003/11suiteDSL/weightedGraphs/pokecwt.txt 32
else
    echo "Compilation failed."
fi


#!/bin/bash

# Define the compiler and the necessary flags
COMPILER=g++
FLAGS="-std=c++17" # Add any additional flags you might need
METIS_LIB_PATH="/home/cs22m028/local/include/metis.h" # Set this to the path of your METIS library, if required
METIS_INCLUDE_PATH="/home/cs22m028/local/lib/libmetis.so" # Set this to the path of your METIS include files, if required

# Compile the main program and its modules
$COMPILER $FLAGS -o main_program main.cpp graph_conversion.cpp csr_conversion.cpp metis_integration.cpp partitioned_writer.cpp -I$METIS_INCLUDE_PATH -L$METIS_LIB_PATH -lmetis

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
    ./main_program $HOME/weightedGraphs/weightedGraphs/pokecwt.txt 4
else
    echo "Compilation failed."
fi


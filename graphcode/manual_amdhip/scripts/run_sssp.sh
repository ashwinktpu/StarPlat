#!/bin/bash

hipcc sssp_dsl.cc -o sssp

for file in ~/Graphs/weighted/*.txt; do
  
  echo "Running SSSP on $file"
  ./sssp $file
  printf "Completed\n\n\n"

done

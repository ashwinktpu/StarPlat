#!/bin/bash

rm -f *.out

module load openmpi411
export LD_LIBRARY_PATH=/lfs/usrhome/btech/cs20b013/boost_install/lib:$LD_LIBRARY_PATH

# -rw-r--r-- 1 cs20b013 rupeshgrp 495M Sep 14 13:56 germanyudwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 1.3G Sep 14 13:56 livjournalwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 4.4G Sep 14 13:56 orkutudwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 548M Sep 14 13:56 pokecwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 1.7G Sep 14 13:57 rmatwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 5.0G Sep 14 13:57 sinaweibowt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 5.6G Sep 14 13:57 twitterwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 1.6G Sep 14 13:57 u10m80mwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 1.2G Sep 14 13:57 USAudwt.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp 1.7G Sep 14 13:57 wikiwt.txt

# -rw-r--r-- 1 cs20b013 rupeshgrp 4049333 Sep 12 23:14 email-Enron.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp  192698 Sep 12 23:14 email-Eu-core.txt
# -rw-r--r-- 1 cs20b013 rupeshgrp  854362 Sep 12 23:14 facebook_combined.txt

graphs=("test-graph-connected.txt")
graph_path=/lfs/usrhome/btech/cs20b013/scratch/graphs

make main_prims.out

for graph in ${graphs[@]}
do
    echo "Starting ${graph//\.txt/}"
    mpirun -np 2 main_prims.out ${graph_path}/${graph}
done
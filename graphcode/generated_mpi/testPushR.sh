# compile mpic++
#mpic++ -g -std=c++17 -g -Wall -I /home/cs22m028/boost/include -L /home/cs22m028/boost/lib -lboost_mpi -l boost_serialization -o push_relabel.dsl.out ../mpi_header/graph_mpi.cc ../mpi_header/updates.cc ../mpi_header/graph_properties/node_property/node_property.cc ../mpi_header/graph_properties/edge_property/edge_property.cc ../mpi_header/rma_datatype/rma_datatype.cc ../mpi_header/data_constructs/container.cc push_relabel.dsl.cc push_relabel.dsl.h main.cc

# compile control code
g++ -std=c++17 control.cpp -o control.out

for i in {1..1000}
do 
    echo $i
    # generate input file
    ./graph_gen.out $i 1000
    ./control.out input.txt > outputControl.txt
    echo "control finished execution"
    mpirun -np 2 ./push_relabel.dsl.out input.txt outputActual.txt > log
    diff -q outputControl.txt outputActual.txt
    
done

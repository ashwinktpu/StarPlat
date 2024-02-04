$(if $(filter-out 1,$(words $(MAKECMDGOALS))),$(error Use as make 'filename'.out eg:- make bc_dslV2.out))
 
MPI_HEADER_FILES = ../mpi_header/graph_mpi.h ../mpi_header/updates.h ../mpi_header/graph_properties/property.h ../mpi_header/graph_properties/node_property/node_property.h ../mpi_header/graph_properties/edge_property/edge_property.h ../mpi_header/rma_datatype/rma_datatype.h
MPI_SOURCE_FILES = ../mpi_header/graph_mpi.cc ../mpi_header/updates.cc ../mpi_header/graph_properties/node_property/node_property.cc ../mpi_header/graph_properties/edge_property/edge_property.cc ../mpi_header/rma_datatype/rma_datatype.cc


CFLAGS=-g -Wall -I$HOME/boost/include -L$HOME/boost/lib -lboost_mpi -lboost_serialization  
CC=mpic++ -std=c++17

target1 : 
    echo "here" 

#all: %.out

#%.out : %.cc %.h $(MPI_SOURCE_FILES) $(MPI_HEADER_FILES)
#	$(CC) $(CFLAGS) -o $@ $(MPI_SOURCE_FILES) %.h %.cc 

clean:
#~ 	rm -f $(EXPENDABLES)
	rm *.out



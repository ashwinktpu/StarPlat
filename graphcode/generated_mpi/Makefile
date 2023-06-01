$(if $(filter-out 1,$(words $(MAKECMDGOALS))),$(error Use as make 'filename'.out eg:- make bc_dslV2.out))
 
MPI_HEADER_FILES = ../mpi_header/graph_mpi.h ../mpi_header/updates.h ../mpi_header/graph_properties/property.h ../mpi_header/graph_properties/node_property/node_property.h ../mpi_header/graph_properties/edge_property/edge_property.h ../mpi_header/rma_datatype/rma_datatype.h
MPI_SOURCE_FILES = ../mpi_header/graph_mpi.cc ../mpi_header/updates.cc ../mpi_header/graph_properties/node_property/node_property.cc ../mpi_header/graph_properties/edge_property/edge_property.cc ../mpi_header/rma_datatype/rma_datatype.cc

BOOST_PATH = /home/atharva1511/Desktop/Boost/boost_1_81_0/stage/lib


CFLAGS = -g -Wall -L$(BOOST_PATH) -lboost_mpi -lboost_serialization  
CC = mpic++ -std=c++17

all: %.out

%.out : %.cc %.h $(MPI_SOURCE_FILES) $(MPI_HEADER_FILES)
	$(CC) -o $@ $< $(MPI_SOURCE_FILES) $(CFLAGS)  

clean:
#~ 	rm -f $(EXPENDABLES)
	rm *.out



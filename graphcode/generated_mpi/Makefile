$(if $(filter-out 1,$(words $(MAKECMDGOALS))),$(error Use as make 'filename'.out eg:- make bc_dslV2.out))
 
MPI_HEADER_FILES = ../mpi_header/graph_mpi.h ../mpi_header/updates.h ../mpi_header/graph_properties/property.h ../mpi_header/graph_properties/node_property/node_property.h ../mpi_header/graph_properties/edge_property/edge_property.h ../mpi_header/rma_datatype/rma_datatype.h
MPI_SOURCE_FILES = ../mpi_header/graph_mpi.cc ../mpi_header/updates.cc ../mpi_header/graph_properties/node_property/node_property.cc ../mpi_header/graph_properties/edge_property/edge_property.cc ../mpi_header/rma_datatype/rma_datatype.cc

ALGORTHMS_SOURCE = mst_prims_new.cc
ALGORTHMS_HEADER = mst_prims_new.h

BOOST_PATH = /lfs/usrhome/btech/cs20b013/boost_install/lib
INCLUDE_PATH = /lfs/usrhome/btech/cs20b013/boost_install/include

CFLAGS = -g -Wall -I$(INCLUDE_PATH) -L$(BOOST_PATH) -lboost_serialization -lboost_mpi
CC = mpic++ -std=c++17 

all: %.out

%.out : %.cc $(MPI_SOURCE_FILES) $(MPI_HEADER_FILES) $(ALGORTHMS_SOURCE) $(ALGORTHMS_HEADER)
	$(CC) -o $@ $< $(MPI_SOURCE_FILES) $(ALGORTHMS_SOURCE) $(CFLAGS)  

clean:
	rm -f *.out
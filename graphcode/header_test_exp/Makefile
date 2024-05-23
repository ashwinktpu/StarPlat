
$(if $(filter-out 1,$(words $(MAKECMDGOALS))),$(error Use as make 'filename'.out eg:- make bc_dslV2.out))
MPI_HEADER_FILES = ../exp_mpi_header/graph_mpi.h ../exp_mpi_header/updates.h ../exp_mpi_header/graph_properties/property.h ../exp_mpi_header/graph_properties/node_property/node_property.h ../exp_mpi_header/graph_properties/edge_property/edge_property.h ../exp_mpi_header/rma_datatype/rma_datatype.h ../exp_mpi_header/data_constructs/data_constructs.h ../exp_mpi_header/metisReader/csrConverter.h
MPI_SOURCE_FILES = ../exp_mpi_header/graph_mpi.cc ../exp_mpi_header/updates.cc ../exp_mpi_header/graph_properties/node_property/node_property.cc ../exp_mpi_header/graph_properties/edge_property/edge_property.cc ../exp_mpi_header/rma_datatype/rma_datatype.cc ../exp_mpi_header/data_constructs/container.cc ../exp_mpi_header/metisReader/csrConverter.cc main.cc


CFLAGS=-g -O0 -Wall -I /home/cs22m028/boost/include -L /home/cs22m028/boost/lib -lboost_mpi -lboost_serialization  
CC=/home/cs22m028/local/bin/mpic++ -std=c++17

all: %.out

%.out : %.cc %.h $(MPI_SOURCE_FILES) $(MPI_HEADER_FILES)
	$(CC) $(CFLAGS) -o $@ $< $(MPI_SOURCE_FILES)  

clean:
#~ 	rm -f $(EXPENDABLES)
	rm *.out



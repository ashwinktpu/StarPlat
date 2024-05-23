#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Helper function to determine the size of MPI_Datatype
int sizeof_datatype(MPI_Datatype datatype) {
    int size;
    MPI_Type_size(datatype, &size);
    return size;
}

static double total_get_time = 0.0;
static long long total_get_bytes = 0;
static double total_accumulate_time = 0.0;
static long long total_accumulate_bytes = 0;

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win) {
    double start_time = MPI_Wtime();

    int result = PMPI_Get(origin_addr, origin_count, origin_datatype,
                          target_rank, target_disp, target_count,
                          target_datatype, win);

    double end_time = MPI_Wtime();
    double operation_time = end_time - start_time;
    total_get_time += operation_time;
    total_get_bytes += origin_count * sizeof_datatype(origin_datatype);

    return result;
}

int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                   int target_rank, MPI_Aint target_disp, int target_count,
                   MPI_Datatype target_datatype, MPI_Op op, MPI_Win win) {
    double start_time = MPI_Wtime();

    int result = PMPI_Accumulate(origin_addr, origin_count, origin_datatype,
                                 target_rank, target_disp, target_count,
                                 target_datatype, op, win);

    double end_time = MPI_Wtime();
    double operation_time = end_time - start_time;
    total_accumulate_time += operation_time;
    total_accumulate_bytes += origin_count * sizeof_datatype(origin_datatype);

    return result;
}

// Function to print accumulated results (call at the end of your program)
void print_mpi_statistics() {
    printf("Total MPI_Get: %lld bytes took %f seconds\n", total_get_bytes, total_get_time);
    printf("Total MPI_Accumulate: %lld bytes took %f seconds\n", total_accumulate_bytes, total_accumulate_time);
}



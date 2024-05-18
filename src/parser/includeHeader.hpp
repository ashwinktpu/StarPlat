#ifndef INCLUDEHEADER_H
#define INCLUDEHEADER_H

// #include "../maincontext/MainContext.hpp"
#include "../maincontext/enum_def.hpp"
// #include "../symbolutil/SymbolTable.hpp"
/*#include "../ast/ASTNodeTypes.hpp"
#include "../ast/ASTNode.hpp"
#include "../ast/ASTHelper.cpp"*/

/* UNCOMMENT IT TO GENERATE FOR OPENMP BACKEND */
#include "../backends/backend_omp/dsl_cpp_generator.h"
#include "../backends/backend_omp/dsl_dyn_cpp_generator.hpp"

/* UNCOMMENT IT TO GENERATE FOR MPI BACKEND */
#include "../backends/backend_mpi/dsl_cpp_generator.h"
#include "../backends/backend_mpi/dsl_dyn_cpp_generator.h"

/* UNCOMMENT IT TO GENERATE FOR CUDA BACKEND */
#include "../backends/backend_cuda/dsl_cpp_generator.h"

/* UNCOMMENT IT TO GENERATE FOR  OPENACC BACKEND */
#include "../backends/backend_openACC/dsl_cpp_generator.h"

/* UNCOMMENT IT TO GENERATE FOR  multigpu BACKEND */
#include "../backends/backend_multigpu/dsl_cpp_generator.h"

#include "../symbolutil/SymbolTableBuilder.h"

/* UNCOMMENT IT TO GENERATE FOR SYCL BACKEND */
#include "../backends/backend_sycl/dsl_cpp_generator.h"

/* UNCOMMENT IT TO GENERATE FOR AMD BACKEND */
#include "../backends/backend_amd/dsl_cpp_generator.h"

/* UNCOMMENT IT TO GENERATE FOR HIP BACKEND */
#include "../backends/backend_hip/dsl_cpp_generator.h"


extern "C" int yyparse(void);

#endif

#ifndef INCLUDEHEADER_H
#define INCLUDEHEADER_H


//#include "../maincontext/MainContext.hpp"
#include "../maincontext/enum_def.hpp"
//#include "../symbolutil/SymbolTable.hpp"
/*#include "../ast/ASTNodeTypes.hpp"
#include "../ast/ASTNode.hpp"
#include "../ast/ASTHelper.cpp"*/
//#include "../backends/backend_mpi/mpi_cpp_generator.h"


/* UNCOMMENT IT TO GENERATE FOR OPENMP BACKEND */
/*#include "../backends/backend_omp/dsl_cpp_generator.h"
#include "../backends/backend_omp/dsl_dyn_cpp_generator.hpp"*/


/* UNCOMMENT IT TO GENERATE FOR CUDA BACKEND */
//#include "../backends/backend_cuda/dsl_cpp_generator.h"

/* UNCOMMENT IT TO GENERATE FOR  OPENACC BACKEND */
#include "../backends/backend_openACC/dsl_cpp_generator.h"

#include "../symbolutil/SymbolTableBuilder.h"

extern "C" int yyparse (void);


#endif

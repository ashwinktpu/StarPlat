#ifndef INCLUDEHEADER_H
#define INCLUDEHEADER_H


//#include "../maincontext/MainContext.hpp"
#include "../maincontext/enum_def.hpp"
//#include "../symbolutil/SymbolTable.hpp"
#include "../ast/ASTNodeTypes.hpp"
#include "../ast/ASTNode.hpp"
#include "../ast/ASTHelper.cpp"
//#include "../backends/backend_mpi/mpi_cpp_generator.h"
#include "../backends/backend_omp/dsl_cpp_generator.h"
#include "../backends/backend_omp/dsl_dyn_cpp_generator.hpp"
//#include "../symbolutil/SymbolTable.h"
#include "../symbolutil/SymbolTableBuilder.h"

extern "C" int yyparse (void);


#endif

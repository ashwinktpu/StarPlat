#ifndef MPI_DSL_DYN_CPP_GENERATOR
#define MPI_DSL_DYN_CPP_GENERATOR

#include "dsl_cpp_generator.h"
namespace spdynmpi {
  class dsl_dyn_cpp_generator:public spmpi::dsl_cpp_generator
  {   

    private:
    Identifier* batchEnvSizeId;

    public:
  
    dsl_dyn_cpp_generator()
    {
      batchEnvSizeId = NULL;
    }

    void generateIncremental(Function* incrementalFunc); ///
    void generateDecremental(Function* decrementalFunc); ///
    void generateInDecHeader(Function* func, bool isMainFile); ///
    void generateDynamicFunc(Function* func); ///
    void generateDynamicHeader(Function* func, bool isMainFile); ///

    void generateFunc(ASTNode* proc) override;  ///
 
    void generate_exprProcCall(Expression* expr) override;  ///
    void generateStatement(statement* stmt) override; ///
    void generateBatchBlock(batchBlock* batchStmt); ///
    void generateOnAddBlock(onAddBlock* onaddStmt);  ///
    void generateOnDeleteBlock(onDeleteBlock* ondeleteStmt); ///
 
    void generation_begin() override; ///
 
    bool openFileforOutput() override; ///

  };
}
#endif
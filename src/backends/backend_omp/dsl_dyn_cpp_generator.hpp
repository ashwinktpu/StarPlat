#ifndef DSL_DYN_CPP_GENERATOR
#define DSL_DYN_CPP_GENERATOR

#include "dsl_cpp_generator.h"

namespace spdynomp {
class dsl_dyn_cpp_generator:public spomp::dsl_cpp_generator
{   

 private:
 Identifier* batchEnvSizeId;
 Identifier* updatesId;

 public:
  
  dsl_dyn_cpp_generator()
  {
    batchEnvSizeId = NULL;
    updatesId = NULL;
  }

 void generateIncremental(Function* incrementalFunc);
 void generateDecremental(Function* decrementalFunc);
 void generateInDecHeader(Function* func);
 void generateDynamicFunc(Function* func);
 void generateDynamicHeader(Function* func);

 void generateFunction(ASTNode* proc);
 void generateForAllSignature(forallStmt* forAll);
 void generateForAll(forallStmt* forAll);
 void generate_exprPropId(PropAccess* propId);
 void generate_exprProcCall(Expression* expr);
 void generateStatement(statement* stmt);
 void generateBatchBlock(batchBlock* batchStmt);
 void generateOnAddBlock(onAddBlock* onaddStmt);
 void generateOnDeleteBlock(onDeleteBlock* ondeleteStmt);
 bool generate();
 void generation_begin();
 void generation_end();
 void setBatchEnvIds(Identifier* updatesId);
 void setPreprocessEnv();
 bool getPreprocessEnv();
 void resetPreprocessEnv();
 Identifier* getUpdatesId();
 void resetBatchEnv();
 bool openFileforOutput();
 void closeOutputFile();
// void generateFreeInCurrentBatch();


};
}
#endif

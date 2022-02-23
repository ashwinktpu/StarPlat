#ifndef DSL_DYN_CPP_GENERATOR
#define DSL_DYN_CPP_GENERATOR

#include "dsl_cpp_generator.h"

class dsl_dyn_cpp_generator:public dsl_cpp_generator
{

 public:
  dsl_dyn_cpp_generator()
  {
   
  }

 void generateIncremental(Function* incrementalFunc);
 void generateDecremental(Function* decrementalFunc);
 void generateInDecHeader(Function* func);
 void generateFunction(ASTNode* proc);
 void generateForAllSignature(forallStmt* forAll);
 void generateForAll(forallStmt* forAll);
 void generate_exprPropId(PropAccess* propId);
 bool generate();
 void generation_begin();
 void generation_end();
 bool openFileforOutput();
 void closeOutputFile();

};
#endif
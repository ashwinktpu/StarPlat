#ifndef DSL_CPP_GENERATOR
#define DSL_CPP_GENERATOR

#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
//#include "dslCodePad.h"


class dsl_cpp_generator
{
  private:

  dslCodePad header;
  dslCodePad main;
  FILE *headerFile;
  FILE *bodyFile;
  char* fileName;

  //added here
  bool genCSR;	
  char* gName;	
  int kernelCount;	
  struct vars{	
    string varType;	
    string varName;	
    bool result;	
  };	
  vector<vars> varList;	
  bool isHeader;


  public:
  dsl_cpp_generator()
  {

    //added here
    genCSR=false;	
    isHeader=false;	
    kernelCount=0;	
    gName=new char[25];

    headerFile=NULL;
    bodyFile=NULL;
    fileName=new char[1024];

  }

  void setFileName(char* f);
  bool generate();
  void generation_begin();
  void generation_end();
  bool openFileforOutput();
  void closeOutputFile();
  const char* convertToCppType(Type* type);
  const char* getOperatorString(int operatorId);
  void generateFunc(ASTNode* proc);
  void generateFuncHeader(Function* proc,bool isMainFile);
  void generateProcCall(proc_callStmt* procCall, bool isMainFile);
  void generateVariableDecl(declaration* decl, bool isMainFile);
  void generateStatement(statement* stmt);
  //void generateAssignmentStmt(assignment* assignStmt);
   void generateAssignmentStmt(assignment* assignStmt, bool isMainFile);
  void generateWhileStmt(whileStmt* whilestmt);
  void generateForAll(forallStmt* forAll);
  void generateFixedPoint(fixedPointStmt* fixedPoint);
  void generateIfStmt(ifStmt* ifstmt, bool isMainFile);
  void generateDoWhileStmt(dowhileStmt* doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace=true, bool isMainFile = true);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);
  void generateExpr(Expression* expr);
  void generate_exprRelational(Expression* expr);
  void generate_exprInfinity(Expression* expr);
  void generate_exprLiteral(Expression* expr);
  void generate_exprIdentifier(Identifier* id);
  void generate_exprPropId(PropAccess* propId) ;
  void generate_exprProcCall(Expression* expr);
  void generate_exprArL(Expression* expr);
  void generateForAll_header();
  void generateForAllSignature(forallStmt* forAll, bool isKernel);
  //void includeIfToBlock(forallStmt* forAll);
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  blockStatement* includeIfToBlock(forallStmt* forAll);

  void generateId();
  void generateOid();
  void addIncludeToFile(char* includeName,dslCodePad& file,bool isCPPLib);
  void generatePropertyDefination(Type* type,char* Id, bool isMainFile);
  void findTargetGraph(vector<Identifier*> graphTypes,Type* type);
  void getDefaultValueforTypes(int type);

  void generateInitialization();
  void generateStatement1(statement* stmt);
  void generateFuncSSSPBody();
  void generateFuncCUDASizeAllocation();
  void generateFuncCudaMalloc();
  void generateBlockForSSSPBody(blockStatement* blockStmt,bool includeBrace);
  void generateStatementForSSSPBody(statement* stmt);
  void generateCudaMemCpyForSSSPBody();
  void generateFuncPrintingSSSPOutput();
  void generateFuncVariableINITForSSSP();
  void generateFuncTerminatingConditionForSSSP();
  //newly added for cuda speific handlings index
  void generateCudaIndex();
  void generateAtomicBlock();
  void generateVariableDeclForEdge(declaration* declStmt);
  void generateLocalInitForID();
  //new
  void castIfRequired(Type* type,Identifier* methodID,dslCodePad& main);
  void generateReductionCallStmt(reductionCallStmt* stmt, bool isMainFile);
  void generateReductionOpStmt(reductionCallStmt* stmt);
  void generate_exprUnary(Expression* expr);
  void generateForAll_header(forallStmt* forAll);
  void generatefixedpt_filter(Expression* filterExpr);

  bool elementsIteration(char* extractId);
  //void generateCudaMallocStr(const char* dVar,const char* typeStr, const char* sizeOfType, bool isMainFile);
  //void generateCudaMalloc(Type* type, const char* identifier, bool isMainFile);
  void generateCudaMemcpy(const char* dVar,const char* cVar,const char* typeStr, const char* sizeOfType, bool isMainFile,const char* from);

  //for algorithm specific function implementation headers

  void generatePrintAnswer();	
  void generateGPUTimerStop();	
  void generateGPUTimerStart();	
  void generateCudaMemCpyStr(const char* ,const char* , const char* , const char* , bool);	
  void generateInitkernel(const char* name);	
  void generateLaunchConfig(const char* name);	
  void generateCudaDeviceSync();	
  void generateForKernel();	
  void generateForKernel(Identifier*);	
  void generateCudaIndex(const char* idName);	
  void generateCudaMalloc(Type* type,const char* identifier);	
  void generateCudaMallocStr(const char* dVar,const char* type, const char* sizeOfType);	
  void generateThreadId(const char* str);

  
   void setGenCSR(bool yes=true){	
      genCSR=yes;	
    }	
  bool isGenCSR(){	
      return genCSR;	
    }	
  void setGName(const char* str){	
      strlen(str);	
      strcpy(gName,str);	
    }	
  char* getGName(){	
      return gName;	
    }	
  void IncrementKCount(){	
      kernelCount++;	
    }	
  int getKCount(){	
        return kernelCount;	
  }


};

 static const char* INTALLOCATION = "new int";
 static const char* BOOLALLOCATION = "new bool";
 static const char* FLOATALLOCATION = "new float";
 static const char* DOUBLEALLOCATION = "new double";
#endif

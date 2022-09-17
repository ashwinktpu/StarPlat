#ifndef MPI_CPP_GENERATOR
#define MPI_CPP_GENERATOR

#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
//#include "dslCodePad.h"


class mpi_cpp_generator
{
  private:

  dslCodePad header;
  dslCodePad main;
  FILE *headerFile;
  FILE *bodyFile;
  char* fileName;

  public:
  mpi_cpp_generator()
  {
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
  void generateProcCall(proc_callStmt* procCall);
  void generateVariableDecl(declaration* decl);
  void generateStatement(statement* stmt);
  void generateAssignmentStmt(assignment* assignStmt);
  void generateWhileStmt(whileStmt* whilestmt);
  void generateForAll(forallStmt* forAll);
  void generateFor(forallStmt* forStmt);
  void generateFixedPoint(fixedPointStmt* fixedPoint);
  void generateIfStmt(ifStmt* ifstmt);
  void generateDoWhileStmt(dowhileStmt* doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace=true);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateInnerReductionStmt(reductionCallStmt* stmt,int send);
  void generateReductionStmtForSend(reductionCallStmt* stmt,bool send);
  void generateExprForSend(Expression* expr,int send, Identifier* remote, Identifier* replace);
  void generateAssignmentForSend(assignment* stmt,int send,Identifier* remote,Identifier* replace);
  void generateIdentifierForSend(Identifier* stmt,int send, Identifier* remote,Identifier* replace);
  void generateProcCallForSend(Expression* expr,int send, Identifier* remote, Identifier* replace);
  void generatePropAccessForSend(PropAccess* lhs,int send,Identifier* remote,Identifier* replace);
  void generate_exprLiteralForSend(Expression* expr,int send,Identifier *remote,Identifier* replace);
  void generateArLForSend(Expression* stmt,int send, Identifier* remote,Identifier* replace);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);
  void generateExpr(Expression* expr);
  void generate_exprRelational(Expression* expr);
  void generate_exprInfinity(Expression* expr);
  void generate_exprLiteral(Expression* expr);
  void generate_exprIdentifier(Identifier* id);
  void generate_exprPropId(PropAccess* propId) ;
  void generate_exprPropId_Pull(PropAccess* propId, Identifier* replace_id);
  void generate_exprPropIdReceive(PropAccess* propId) ;
  void generateCheckIfNeighbor();
  void generate_exprProcCall(Expression* expr);
  void generate_exprUnary(Expression* expr);
  void generate_exprArL(Expression* expr);
  void generate_exprArLReceive(Expression* expr);
  void generateForAll_header();
  void generateForAllSignature(forallStmt* forAll);
  void generateForSignature(forallStmt* forAll);
  void generate_sendCall(statement* body);
  void generate_receiveCall(statement* body);
  void generate_receiveCallBFS(statement* body,int send,Identifier *id);
  void generate_addMessage(statement* body,int send,Identifier* id,Identifier* replace);
  void generateReceiveBlock(blockStatement* body);
  //void includeIfToBlock(forallStmt* forAll);
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  bool elementsIteration(char* extractId);
  blockStatement* includeIfToBlock(forallStmt* forAll);
 // void generateIsFinished();
  void generateId();
  void generateOid();
  void addIncludeToFile(char* includeName,dslCodePad& file,bool isCPPLib);
  void generatePropertyDefination(Type* type,char* Id);
  void findTargetGraph(vector<Identifier*> graphTypes,Type* type);
  void getDefaultValueforTypes(int type);


};
class reduction_details
{
  bool has_reduction;
  int reduction_op;
  Identifier* reduction_var;
  //PropAccess* reduction_var;
  public:
  reduction_details()
  {
      has_reduction = false;
      reduction_op = -1;
      reduction_var = NULL;
      //reduction_var = NULL;
  }
  void set_reductionDetails(bool has_red, int red_op,Identifier* red_var)
  {
    has_reduction = has_red;
      reduction_op = red_op;
      reduction_var = red_var;
  }
  bool contains_reduction()
  {
    return this->has_reduction;
  }
  Identifier* get_reductionVar()
  {
    return this->reduction_var;
  }
  int get_reductionOp()
  {
    return this->reduction_op;
  }
};
class make_par
{
  bool make_parallel;
  Identifier* par_id;
  Identifier* iter;
  public:
  make_par()
  {
    make_parallel = false;
    par_id = NULL;
    iter = NULL;
  }
  void set_par(bool val,Identifier* id)
  {
    make_parallel = val;
    par_id = id;
  }
  void set_iter(Identifier* it)
  {
    iter = it;
  }
  bool check_makeParallel()
  {
    return make_parallel;
  }
  Identifier* get_parId()
  {
    return par_id;
  }
  Identifier* get_Iter()
  {
    return iter;
  }

};
bool checkExpr(Expression* expr,Identifier* remote);
bool checkPropAccess(PropAccess* stmt,Identifier* remote);
static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
static const char* FLOATALLOCATION = "new float";
static const char* DOUBLEALLOCATION = "new double";
#endif

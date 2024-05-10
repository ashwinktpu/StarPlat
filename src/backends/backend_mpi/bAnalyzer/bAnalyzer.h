/*
  Analyzer made specific to mpi backend. May be extended to other backends if needs be.
  There are multiple areas where this may be improved.
*/
#ifndef B_ANALYZER
#define B_ANALYZER
#include "../../../ast/ASTNodeTypes.hpp"
#include "../../../ast/ASTNode.hpp"
#include "../../../ast/ASTHelper.cpp"
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <list>
class bAnalyzer {
  private:
    std::unordered_map<int, std::string> functionMapping ; 
    std::unordered_map<int, std::vector<int> > callGraph ;
    std::unordered_map<int, statement*> forallstatmentIndexing ;
    std::unordered_map<int, forallStmt * > forAllNesting ;
    std::vector<std::string> updatedNodes ;
    std::vector<std::string> trackNodes ;
    int analysisStatus ;
    blockStatement * newStatement ;
    int counter ; 
    list<argument *> newArgList ;
    argList * paramsForFrontierPush ;
    stack<char *> iterVar ;
    char * lastIter ;
    blockStatement * statementForAll ;
    statement * statementWhile ;
    bool filterAnalysisStatus ;

  public:
    bAnalyzer () ;
    void generateNewRelationCheck (blockStatement *) ;
    void TraverseAST (statement *) ;
    int analyzeForAllStmt (forallStmt * ) ;
    void analyseStatement(statement* );
    int canImproveEdge (declaration * , char * , char *) ;
    int getAnalysisStatus () ;
    blockStatement * getNewBody () ;
    statement * createNewEdgeStatement (declaration * stmt, int status, const char * vIdx) ;
    char *  getIteratorVar () ;
    void clearAllAnalysis () ;
    statement * createNewV (declaration * stmt, Identifier * u, Identifier * v, const char * _tx) ;
    bool filterAnalysis (Expression * condition) ; //add to updatedNodeProperties within this functiond
    statement * statementToAddIntoFrontier () ; // one increment statement.
    statement * actualStatementWhile () ; // copy down the entire statement chunk.
    void setFilterAnalysisStatement1 (blockStatement *) ; 
    void setFilterAnalysisStatement2 (whileStmt *) ;
    void evaluateFilter (forallStmt *, blockStatement *, Expression *) ;
    void createNewForAllStmtBlock () ;
    Expression * filterExprDeepCopy (Expression *, const char *) ;
    void createNewFrontierStatement(Expression *, blockStatement *) ;
    statement * createFrontierPopStatement () ;
    blockStatement * getStatementForLoop () ;
    statement * getStatementWithinWhileLoop () ;
    bool checkSignature (forallStmt *) ;
    bool getFilterAnalysisStatus () ;
} ;

#endif

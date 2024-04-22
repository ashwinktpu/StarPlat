/*
  Analyzer made specific to mpi backend. May be extended to other backends if needs be.
  There are multiple areas where this may be improved.
*/
#ifndef B_ANALYZER
#define B_ANALYZER
#include "../../../ast/ASTNodeTypes.hpp"
#include <unordered_map>
#include <vector>
#include <list>
class bAnalyzer {
  private:
    std::unordered_map<int, std::string> functionMapping ; 
    std::unordered_map<int, std::vector<int> > callGraph ;
    std::unordered_map<int, statement*> forallstatmentIndexing ;
    std::unordered_map<int, forallStmt * > forAllNesting ;
    int analysisStatus ;
    blockStatement * newStatement ;
    int counter ; 
//    std::vector<list<argument *> > argListList ;
    list<argument *> newArgList ;
    stack<char *> iterVar ;
    char * lastIter ;

  public:
    bAnalyzer () ;
    void TraverseAST (statement *) ;
    int analyzeForAllStmt (forallStmt * ) ;
    void analyseStatement(statement* );
    int canImproveEdge (declaration * , char * , char *) ;
    int getAnalysisStatus () ;
    blockStatement * getNewBody () ;
    statement * createNewEdgeStatement (declaration * stmt, int status, const char * vIdx) ;
    char *  getIteratorVar () ;
    void clearAllAnalysis () ;
} ;
#endif

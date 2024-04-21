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
    std::unordered_map<int, std::vector<int> > forAllNesting ;
    int analysisStatus ;
    blockStatement * newStatement ;

  public:
    bAnalyzer () ;
    void TraverseAST (statement *) ;
    int analyzeForAllStmt (forallStmt * ) ;
    void analyseStatement(statement* );
    int canImproveEdge (declaration * , char * , char *) ;
    int analysisStatus () ;
    blockStatement * getNewBody () ;
    statement * createNewEdgeStatement (Statement * stmt, int status) ;
    void clearAllAnalysis () ;
} ;
#endif

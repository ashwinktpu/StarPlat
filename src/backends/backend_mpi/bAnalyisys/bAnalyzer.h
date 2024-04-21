/*
  Analyzer made specific to mpi backend. May be extended to other backends if needs be.
  There are multiple areas where this may be improved.
*/
#ifndef B_ANALYZER
#define B_ANALYZER
#include "../../../ast/ASTNodeTypes.hpp"
class bAnalazyer {
  private:
    std::unoredred_map<int, std::string> functionMapping ; 
    std::unordered_map<int, std::vector<int> > callGraph ;
    std::unordered_map<int, statment*> forallstatmentIndexing ;
    std::unordered_map<int, std::vector<int> > forAllNesting ;

  public:
    bAnalyzer (statement * stmt) ;
    void TraverseAST (statment *) ;
    void analyzeForAllStmt (forallStmt * ) ;
    void analyseStatement(statement* );
    int canImproveEdge (declatation * ) ;
} ;
#endif

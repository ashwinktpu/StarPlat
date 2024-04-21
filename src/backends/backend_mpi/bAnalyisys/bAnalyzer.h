/*
  Analyzer made specific to mpi backend. May be extended to other backends if needs be.
*/
#ifndef B_ANALYZER
#define B_ANALYZER
class bAnalazyer {
  private:
    std::unoredred_map<int, std::string> functionMapping ; 
    std::unordered_map<int, std::vector<int> > callGraph ;
    std::unordered_map<int, statment*> forallstatmentIndexing ;
    std::unordered_map<int, std::vector<int> > forAllNesting ;

  public:
    void TraverseAST (list<Function * > funcList) ;
    void analyzeForAllStmt (forallStmt * ) ;
    void analyseStatement(statement* );
} ;
#endif

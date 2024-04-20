/*
  Analyzer made specific to mpi backend. May be extended to other backends if needs be.
*/

class bAnalazyer {
  private:
    std::unoredred_map<int, string> functionMapping ; 
    std::unordered_map<int, std::vector<int> > callGraph ;
    std::unordered_map<int, statment*> forallstatmentIndexing ;
    std::unordered_map<int, std::vector<int> > forAllNesting ;

  public:
    void TraverseAST (list<function> funcList) ;
    void analyzeForAllStmt (..) ;
    void analyzeRMAConflicts () ;
} ;

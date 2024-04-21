class bOptimizer {
  
  private:
    statement * stmt ;

  
  public:
    bOptimizer (statment * stmt) ;
    statement * improveEdgeForAll (forAllStmt * stmt, int analysisStatus) ;

} ;

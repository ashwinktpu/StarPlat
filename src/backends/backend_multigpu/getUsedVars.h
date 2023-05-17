#ifndef GET_USED_VARS
#define GET_USED_VARS

#include "analyserUtil.cpp"
namespace multigpu{
usedVariables getVarsStatement(statement *stmt);
usedVariables getVarsExpr(Expression *expr);


usedVariables getVarsWhile(whileStmt *stmt);

usedVariables getVarsDoWhile(dowhileStmt *stmt);


usedVariables getVarsAssignment(assignment *stmt);


usedVariables getVarsIf(ifStmt *stmt);

usedVariables getVarsFixedPoint(fixedPointStmt *stmt);

usedVariables getVarsReduction(reductionCallStmt *stmt);

usedVariables getVarsUnary(unary_stmt *stmt);

/*
usedVariables getVarsBFS(iterateBFS *stmt)
{
  usedVariables currVars = getVarsStatement(stmt->getBody());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(RBFSstmt->getBFSFilter());
    currVars.merge(RBFSstmt->getBody());
  }

  return currVars;
}*/

usedVariables getVarsForAll(forallStmt *stmt);
usedVariables getVarsBlock(blockStatement *blockStmt);
    /* Function: getDeclaredPropertyVarsOfBlock
     * Return all the Property variables declared in a the block!
     * Used for cudaFree those variables
     * usedVariables may be a misnomer?! for this function.
     * --rajesh
     */
    usedVariables getDeclaredPropertyVarsOfBlock(blockStatement *blockStmt);
}
#endif
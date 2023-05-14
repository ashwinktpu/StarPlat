#ifndef GET_USED_CPU_VARS
#define GET_USED_CPU_VARS

#include "analyserUtil.cpp"
usedVariables getVarsStatementCpu(statement *stmt);
usedVariables getVarsExprCpu(Expression *expr);

usedVariables getVarsWhileCpu(whileStmt *stmt);

usedVariables getVarsDoWhileCpu(dowhileStmt *stmt);

usedVariables getVarsAssignmentCpu(assignment *stmt);

usedVariables getVarsIfCpu(ifStmt *stmt);

usedVariables getVarsFixedPointCpu(fixedPointStmt *stmt);

usedVariables getVarsReductionCpu(reductionCallStmt *stmt);

usedVariables getVarsUnaryCpu(unary_stmt *stmt);

/*
usedVariables getVarsBFSCpu(iterateBFS *stmt)
{
  usedVariables currVars = getVarsStatementCpu(stmt->getBodyCpu());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(RBFSstmt->getBFSFilter());
    currVars.merge(RBFSstmt->getBody());
  }

  return currVars;
}*/

usedVariables getVarsForAllCpu(forallStmt *stmt);
usedVariables getVarsBlockCpu(blockStatement *blockStmt);
/* Function: getDeclaredPropertyVarsOfBlock
  * Return all the Property variables declared in a the block!
  * Used for cudaFree those variables
  * usedVariables may be a misnomer?! for this function.
  * --rajesh
  */
usedVariables getDeclaredPropertyVarsOfBlockCpu(blockStatement *blockStmt);

#endif
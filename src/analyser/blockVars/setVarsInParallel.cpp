#include "blockVarsAnalyser.h"

void blockVarsAnalyser::setVarsInParallelStatment(statement* stmt)
{
    switch (stmt->getTypeofNode()) 
    {
        case NODE_BLOCKSTMT:
            setVarsInParallelBlock((blockStatement*)stmt); break;
        case NODE_ASSIGN:
            setVarsInParallelAssign((assignment*)stmt); break;
        case NODE_DECL:
            setVarsInParallelDeclaration((declaration*)stmt); break;
        case NODE_FORALLSTMT:
            setVarsInParallelForAll((forallStmt*)stmt); break;
        case NODE_DOWHILESTMT:
            setVarsInParallelDoWhile((dowhileStmt*)stmt); break;
        case NODE_WHILESTMT:
            setVarsInParallelWhile((whileStmt*)stmt); break;
        case NODE_FIXEDPTSTMT:
            setVarsInParallelFixedPoint((fixedPointStmt*)stmt); break;
        case NODE_ITRBFS:
            setVarsInParallelBFS((iterateBFS*)stmt); break;
        default:
            break;
    }
}

void blockVarsAnalyser::setVarsInParallelBlock(blockStatement* block)
{
    // Iterate over all the statements in the block
    list<statement*> stmts = block->returnStatements();
    for (auto stmt: stmts)
    {
        setVarsInParallelStatment(stmt);
    }
}

void blockVarsAnalyser::setVarsInParallelAssign(assignment* assign)
{
    if(assign->lhs_isIdentifier())
    {
        Identifier* lhs = assign->getId();
        // Check if it has a property copy
        if (assign->hasPropCopy()) 
        {
            // Set lhs and rhs identifiers as parallel
            // assert(assign->lhs_isIdentifier() && assign->getExpr()->getExpressionFamily() == EXPR_ID);
            Identifier* rhs = assign->getExpr()->getId();
            lhs->getSymbolInfo()->setInParallelSection(true);
            rhs->getSymbolInfo()->setInParallelSection(true);
        }
    }
}

void blockVarsAnalyser::setVarsInParallelDeclaration(declaration* declStmt)
{
    // Check if the declaration is a parallel declaration initialisation
    if(!declStmt->isInitialized()) return;
    Type* type=declStmt->getType();
    if(type->isPropType())
    {
        if(type->getInnerTargetType()->isPrimitiveType())
        {
            if (declStmt->getExpressionAssigned()->getExpressionFamily() == EXPR_ID && declStmt->getExpressionAssigned()->getId()->getSymbolInfo()->getType()->gettypeId() == type->gettypeId())
            {
                // get propid
                Identifier* lhs = declStmt->getdeclId();
                Identifier* rhs = declStmt->getExpressionAssigned()->getId();
                // Set lhs and rhs identifiers as parallel
                lhs->getSymbolInfo()->setInParallelSection(true);
                rhs->getSymbolInfo()->setInParallelSection(true);
            }
        }
    }
}

void blockVarsAnalyser::setVarsInParallelForAll(forallStmt* forall)
{
    // Check if the statement is a forall
    if (!forall->isForall()) 
        return;

    // get used variables in the forall body
    usedVariables_t usedVars = getVarsStatement(forall->getBody());
    // Set the flag of the tableentry of the variables in the parallel section
    for (auto var: usedVars.getVariables()) {
        var->getSymbolInfo()->setInParallelSection(true);
    }
}

void blockVarsAnalyser::setVarsInParallelDoWhile(dowhileStmt* dowhile)
{
    // get used variables in the dowhile body
    setVarsInParallelStatment(dowhile->getBody());
}

void blockVarsAnalyser::setVarsInParallelWhile(whileStmt* whileStmt)
{
    // get used variables in the while body
    setVarsInParallelStatment(whileStmt->getBody());
}

void blockVarsAnalyser::setVarsInParallelFixedPoint(fixedPointStmt* fixedpoint)
{
    // get used variables in the fixedpoint body
    setVarsInParallelStatment(fixedpoint->getBody());
}

void blockVarsAnalyser::setVarsInParallelProcCall(proc_callStmt* proc)
{
    // get used variables in the procedure call
    usedVariables_t usedVars = getVarsProcCall(proc);
    // Set the flag of the tableentry of the variables in the parallel section
    for (auto var: usedVars.getVariables()) {
        var->getSymbolInfo()->setInParallelSection(true);
    }
}

void blockVarsAnalyser::setVarsInParallelBFS(iterateBFS* bfs)
{
    // get used variables in the BFS body
    usedVariables_t usedVars = getVarsStatement(bfs->getBody());

    // Check if bfs has a reverse bfs attached
    if (bfs->getRBFS() != NULL)
    {
        iterateReverseBFS* rbfs = bfs->getRBFS();
        // get used variables in the reverse BFS body
        usedVars.merge(getVarsStatement(rbfs->getBody()));
        if (rbfs->getBFSFilter() != NULL)
            usedVars.merge(getVarsExpr(rbfs->getBFSFilter()));
    }

    // Set the flag of the tableentry of the variables in the parallel section
    for (auto var: usedVars.getVariables()) 
        var->getSymbolInfo()->setInParallelSection(true);
}
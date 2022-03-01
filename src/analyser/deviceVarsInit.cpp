#include "deviceVarsAnalyser.h"

bool initBlock(blockStatement *blockStmt, list<Identifier *> &vars)
{
    ASTNodeWrap* blockNode = initWrapNode(blockStatement, vars);
    blockNode->inMap.setType(lattice::CPU_Preferenced);
    blockNode->outMap.setType(lattice::CPU_Preferenced);

    for(statement* stmt: blockStmt->returnStatements())
    {
        
    }
}
bool initStatement(statement *stmt, list<Identifier *> &);
bool initUnary(unary_stmt *blockStmt, list<Identifier *> &);
bool initIfElse(ifStmt *stmt, list<Identifier *> &);
bool initAssignment(assignment *stmt, list<Identifier *> &);
bool initDeclaration(declaration *, list<Identifier *> &);
bool initForAll(forallStmt *, list<Identifier *> &);
bool initWhile(whileStmt *, list<Identifier *> &);
bool initDoWhile(dowhileStmt *, list<Identifier *> &);
bool initFor(forallStmt *, list<Identifier *> &);
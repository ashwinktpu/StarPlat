#ifndef BLOCK_VARS_ANALYSER_H
#define BLOCK_VARS_ANALYSER_H

#include "analyserUtil.h"
#include "../../ast/ASTHelper.cpp"

enum VariableState
{
    IN_CPU = 1,
    IN_GPU,
    IN_BOTH
};

struct TableEntryWrapper
{
    TableEntry *entry;
    VariableState state;

    TableEntryWrapper(TableEntry *, VariableState);
};

class ASTNodeBlock
{
    ASTNode *node; // the block node

    set<TableEntry *> use;        // set of variables used in the block
    set<TableEntry *> def;        // set of variables defined in the block
    set<TableEntryWrapper *> in;  // set of variables needed to be sent to the block
    set<TableEntryWrapper *> out; // set of variables needed to be sent from the block

    set<ASTNodeBlock *> succ; // set of successor blocks

    bool inParallel;

public:
    ASTNodeBlock();
    ASTNodeBlock(ASTNode *node);
    ASTNode *getNode();
    set<TableEntry *> getUse();
    set<TableEntry *> getDef();
    set<TableEntryWrapper *> getIn();
    set<TableEntryWrapper *> getOut();
    set<ASTNodeBlock *> getSucc();
    void addUse(Identifier *id);
    void addDef(Identifier *id);
    void addIn(set<TableEntryWrapper *> ids);
    void addOut(set<TableEntryWrapper *> ids);
    void addIn(set<TableEntry *> ids);
    void removeIn(set<TableEntry *> ids);
    void addSucc(ASTNodeBlock *block);
    void addVars(usedVariables_t vars);
    void setInParallel(bool=true);
    bool isInParallel();
};

class NodeBlockData
{
    ASTNode *node;            // the block node
    ASTNodeBlock *block;      // Index of the block
    ASTNodeBlock *startBlock; // Index of the start block, Only for loops (for, while, do-while)
    ASTNodeBlock *endBlock;   // Index of the end block, Only for loops (for, while, do-while)

public:
    NodeBlockData();
    NodeBlockData(ASTNode *node);
    NodeBlockData(ASTNode *node, ASTNodeBlock *Block);
    NodeBlockData(ASTNode *node, ASTNodeBlock *startBlock, ASTNodeBlock *endBlock);
    NodeBlockData(ASTNode *node, ASTNodeBlock *Block, ASTNodeBlock *startBlock, ASTNodeBlock *endBlock);
    void setNodeDataBlock();

    ASTNode *getNode();
    ASTNodeBlock *getBlock();
    ASTNodeBlock *getStartBlock();
    ASTNodeBlock *getEndBlock();
    void setBlock(ASTNodeBlock *block);
    void setStartBlock(ASTNodeBlock *startBlock);
    void setEndBlock(ASTNodeBlock *endBlock);
};

class blockVarsAnalyser
{
private:
    ASTNodeBlock *initBlockNode();
    ASTNodeBlock *initBlockNode(ASTNode *node);

    list<ASTNodeBlock *> blockNodes;
    unordered_map<ASTNode *, NodeBlockData *> blockNodeMap;

public:
    blockVarsAnalyser();

    void addBlockNode(ASTNode *node, ASTNodeBlock *blockNode);
    void addBlockNode(ASTNode *node, ASTNodeBlock *startBlock, ASTNodeBlock *endBlock);

    NodeBlockData *getBlockData(ASTNode *node);

    void toString(NODETYPE); // Prints the type of the node verbose
    void printBlockNodes();  // Prints the block nodes

    // Performs liveness analysis on the block nodes
    void analyse(list<Function *>);
    void analyseFunc(ASTNode *func);
    void analyseBlockNodes();

    // Initialise the basic block node
    ASTNodeBlock *initStatement(statement *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initBlock(blockStatement *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initUnary(unary_stmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initIfElse(ifStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initAssignment(assignment *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initDeclaration(declaration *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initForAll(forallStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initWhile(whileStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initDoWhile(dowhileStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initFixedPoint(fixedPointStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initProcCall(proc_callStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initReduction(reductionCallStmt *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initItrBFS(iterateBFS *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initItrRBFS(iterateReverseBFS *, ASTNodeBlock *, bool = false);
    ASTNodeBlock *initReturn(returnStmt *, ASTNodeBlock *, bool = false);

    // Returns the used variables in each statment
    usedVariables_t getVarsStatement(statement *);
    usedVariables_t getVarsBlock(blockStatement *);
    usedVariables_t getVarsForAll(forallStmt *);
    usedVariables_t getVarsUnary(unary_stmt *);
    usedVariables_t getVarsDeclaration(declaration *);
    usedVariables_t getVarsWhile(whileStmt *);
    usedVariables_t getVarsDoWhile(dowhileStmt *);
    usedVariables_t getVarsAssignment(assignment *);
    usedVariables_t getVarsIf(ifStmt *);
    usedVariables_t getVarsExpr(Expression *);
    usedVariables_t getVarsReduction(reductionCallStmt *);
    usedVariables_t getVarsFixedPoint(fixedPointStmt *);
    usedVariables_t getVarsBFS(iterateBFS *);
    usedVariables_t getVarsRBFS(iterateReverseBFS *);
    usedVariables_t getVarsProcCall(proc_callStmt *);
    usedVariables_t getVarsExprProcCall(proc_callExpr *);

    // Sets the flag of tableentry of the variables in the parallel section of the statement
    void setVarsInParallelStatment(statement *);
    void setVarsInParallelBlock(blockStatement *);
    void setVarsInParallelForAll(forallStmt *);
    void setVarsInParallelBFS(iterateBFS *);
    void setVarsInParallelAssign(assignment *);
    void setVarsInParallelDeclaration(declaration *);
    void setVarsInParallelProcCall(proc_callStmt *);
    void setVarsInParallelFixedPoint(fixedPointStmt *);
    void setVarsInParallelDoWhile(dowhileStmt *);
    void setVarsInParallelWhile(whileStmt *);
};

#endif
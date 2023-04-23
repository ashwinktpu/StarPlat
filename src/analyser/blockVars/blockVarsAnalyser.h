#ifndef BLOCK_VARS_ANALYSER_H
#define BLOCK_VARS_ANALYSER_H


#include "analyserUtil.h"
#include "../../ast/ASTHelper.cpp"

class ASTNodeBlock 
{
    ASTNode* node;                  // the block node

    set<TableEntry*> use;          // set of variables used in the block
    set<TableEntry*> def;          // set of variables defined in the block
    set<TableEntry*> in;           // set of variables needed to be sent to the block
    set<TableEntry*> out;          // set of variables needed to be sent from the block

    set<ASTNodeBlock*> succ;       // set of successor blocks

    public:
    
    ASTNodeBlock();
    ASTNodeBlock(ASTNode* node);
    ASTNode* getNode();
    set<TableEntry*> getUse();
    set<TableEntry*> getDef();
    set<TableEntry*> getIn();
    set<TableEntry*> getOut();
    set<ASTNodeBlock*> getSucc();
    void addUse(Identifier* id);
    void addDef(Identifier* id);
    void addIn(Identifier* id);
    void addIn(set<TableEntry*> ids);
    void removeIn(Identifier* id);
    void removeIn(set<TableEntry*> ids);
    void addOut(Identifier* id);
    void addOut(set<TableEntry*> ids);
    void removeOut(Identifier* id);
    void removeOut(set<Identifier*> ids);
    void addSucc(ASTNodeBlock* block);
    void addVars(usedVariables_t vars);
};

class NodeBlockData
{
    ASTNode* node;                  // the block node
    ASTNodeBlock* block;                    // Index of the block
    ASTNodeBlock* startBlock;               // Index of the start block, Only for loops (for, while, do-while)
    ASTNodeBlock* endBlock;                 // Index of the end block, Only for loops (for, while, do-while)
    
    public:
    
    NodeBlockData();
    NodeBlockData(ASTNode* node);
    NodeBlockData(ASTNode* node, ASTNodeBlock* Block);
    NodeBlockData(ASTNode* node, ASTNodeBlock* startBlock, ASTNodeBlock* endBlock);
    NodeBlockData(ASTNode* node, ASTNodeBlock* Block, ASTNodeBlock* startBlock, ASTNodeBlock* endBlock);
    void setNodeDataBlock();

    ASTNode* getNode();
    ASTNodeBlock* getBlock();
    ASTNodeBlock* getStartBlock();
    ASTNodeBlock* getEndBlock();
    void setBlock(ASTNodeBlock* block);
    void setStartBlock(ASTNodeBlock* startBlock);
    void setEndBlock(ASTNodeBlock* endBlock);
};

class blockVarsAnalyser 
{
    private:
    ASTNodeBlock* initBlockNode();
    ASTNodeBlock* initBlockNode(ASTNode* node);

    list<ASTNodeBlock*> blockNodes;
    unordered_map<ASTNode*, NodeBlockData*> blockNodeMap;

    public:
    blockVarsAnalyser();

    void addBlockNode(ASTNode* node, ASTNodeBlock* blockNode);
    void addBlockNode(ASTNode* node, ASTNodeBlock* startBlock, ASTNodeBlock* endBlock);

    NodeBlockData* getBlockData(ASTNode* node);

    void toString(NODETYPE);    // Prints the type of the node verbose
    void printBlockNodes();     // Prints the block nodes

    void analyse(list<Function*>);
    void analyseFunc(ASTNode* func);
    void analyseBlockNodes();

    void analyseStatement(statement*);
    void analyseBlock(blockStatement*);
    void analyseUnary(unary_stmt*);
    void analyseIfElse(ifStmt*);
    void analyseAssignment(assignment*);
    void analyseDeclaration(declaration*);
    void analyseForAll(forallStmt*);
    void analyseWhile(whileStmt*);
    void analyseDoWhile(dowhileStmt*);
    void analyseFor(forallStmt*);
    void analyseFixedPoint(fixedPointStmt*);
    void analyseProcCall(proc_callStmt*);
    void analyseReduction(reductionCallStmt*);
    void analyseItrBFS(iterateBFS*);

    // Initialise the basic block node
    ASTNodeBlock* initStatement(statement*, ASTNodeBlock*);
    ASTNodeBlock* initBlock(blockStatement*, ASTNodeBlock*);
    ASTNodeBlock* initUnary(unary_stmt*, ASTNodeBlock*);
    ASTNodeBlock* initIfElse(ifStmt*, ASTNodeBlock*);
    ASTNodeBlock* initAssignment(assignment*, ASTNodeBlock*);
    ASTNodeBlock* initDeclaration(declaration*, ASTNodeBlock*);
    ASTNodeBlock* initForAll(forallStmt*, ASTNodeBlock*);
    ASTNodeBlock* initWhile(whileStmt*, ASTNodeBlock*);
    ASTNodeBlock* initDoWhile(dowhileStmt*, ASTNodeBlock*);
    ASTNodeBlock* initFor(forallStmt*, ASTNodeBlock*);
    ASTNodeBlock* initFixedPoint(fixedPointStmt*, ASTNodeBlock*);
    ASTNodeBlock* initProcCall(proc_callStmt*, ASTNodeBlock*);
    ASTNodeBlock* initReduction(reductionCallStmt*, ASTNodeBlock*);
    ASTNodeBlock* initItrBFS(iterateBFS*, ASTNodeBlock*);
    ASTNodeBlock* initReturn(returnStmt*, ASTNodeBlock*);

    //Returns the used variables in each statment
    usedVariables_t getVarsStatement(statement*);
    usedVariables_t getVarsBlock(blockStatement*);
    usedVariables_t getVarsForAll(forallStmt*);
    usedVariables_t getVarsUnary(unary_stmt*);
    usedVariables_t getVarsDeclaration(declaration*);
    usedVariables_t getVarsWhile(whileStmt*);
    usedVariables_t getVarsDoWhile(dowhileStmt*);
    usedVariables_t getVarsAssignment(assignment*);
    usedVariables_t getVarsIf(ifStmt*);
    usedVariables_t getVarsExpr(Expression*);
    usedVariables_t getVarsReduction(reductionCallStmt*);
    usedVariables_t getVarsFixedPoint(fixedPointStmt*);
    usedVariables_t getVarsBFS(iterateBFS*);
    usedVariables_t getVarsProcCall(proc_callStmt*);
    usedVariables_t getVarsExprProcCall(proc_callExpr*);

    void setVarsInParallelStatment(statement*);  // Sets the flag of tableentry of the variables in the parallel section in the statement
    void setVarsInParallelBlock(blockStatement*);  // Sets the flag of tableentry of the variables in the parallel section in the block
    void setVarsInParallelForAll(forallStmt*);  // Sets the flag of tableentry of the variables in the parallel section in the forall
    void setVarsInParallelBFS(iterateBFS*);  // Sets the flag of tableentry of the variables in the parallel section in the BFS
    void setVarsInParallelAssign(assignment*);  // Sets the flag of tableentry of the variables in the parallel section in the assignment
};

#endif
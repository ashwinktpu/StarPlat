#ifndef BLOCK_VARS_ANALYSER_H
#define BLOCK_VARS_ANALYSER_H


#include "../analyserUtil.cpp"
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
    void addVars(usedVariables vars);
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
    blockVarsAnalyser() {}

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
    usedVariables getVarsStatement(statement* stmt);
    usedVariables getVarsBlock(blockStatement* stmt);
    usedVariables getVarsForAll(forallStmt* stmt);
    usedVariables getVarsUnary(unary_stmt* stmt);
    usedVariables getVarsDeclaration(declaration* stmt);
    usedVariables getVarsWhile(whileStmt* stmt);
    usedVariables getVarsDoWhile(dowhileStmt* stmt);
    usedVariables getVarsAssignment(assignment* stmt);
    usedVariables getVarsIf(ifStmt* stmt);
    usedVariables getVarsExpr(Expression* stmt);
    usedVariables getVarsReduction(reductionCallStmt *stmt);
    usedVariables getVarsFixedPoint(fixedPointStmt *stmt);
    usedVariables getVarsBFS(iterateBFS *stmt);
    usedVariables getVarsProcCall(proc_callStmt *stmt);
    usedVariables getVarsExprProcCall(proc_callExpr *stmt);
};

#endif
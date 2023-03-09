#ifndef BLOCK_VARS_ANALYSER_H
#define BLOCK_VARS_ANALYSER_H


#include "../analyserUtil.cpp"
#include "../../ast/ASTHelper.cpp"

class ASTNodeBlock 
{
    ASTNode* node;                  // the block node

    list<Identifier*> use;          // list of variables used in the block
    list<Identifier*> def;          // list of variables defined in the block
    list<Identifier*> in;           // list of variables needed to be sent to the block
    list<Identifier*> out;          // list of variables needed to be sent from the block

    list<ASTNodeBlock*> succ;       // list of successor blocks

    public:
    
    ASTNodeBlock(ASTNode* node) {
        this->node = node;
        use = list<Identifier*>();
        def = list<Identifier*>();
        in = list<Identifier*>();
        out = list<Identifier*>();
        succ = list<ASTNodeBlock*>();
    }

    ASTNode* getNode() {
        return node;
    }

    list<Identifier*> getUse() {
        return use;
    }

    list<Identifier*> getDef() {
        return def;
    }

    list<Identifier*> getIn() {
        return in;
    }

    list<Identifier*> getOut() {
        return out;
    }

    list<ASTNodeBlock*> getSucc() {
        return succ;
    }

    void addUse(Identifier* id) {
        use.push_back(id);
    }

    void addDef(Identifier* id) {
        def.push_back(id);
    }

    void addIn(Identifier* id) {
        in.push_back(id);
    }

    void addOut(Identifier* id) {
        out.push_back(id);
    }

    void addSucc(ASTNodeBlock* block) {
        if(block != NULL) succ.push_back(block);
    }

    void addVars(usedVariables usedVars) {
        for(Identifier* id : usedVars.getVariables(READ))
            use.push_back(id);
        
        for(Identifier* id : usedVars.getVariables(WRITE))
            def.push_back(id);
    }
};

class blockVarsAnalyser {
    private:
    ASTNodeBlock* initBlockNode(ASTNode* node) 
    {
        ASTNodeBlock* blockNode = new ASTNodeBlock(node);
        return blockNode;
    }

    list<ASTNodeBlock*> blockNodes;
    map<ASTNode*, ASTNodeBlock*> blockNodeMap;

    public:
    blockVarsAnalyser() {}

    void addBlockNode(ASTNode* node, ASTNodeBlock* blockNode) 
    {
        blockNodes.push_back(blockNode);
        blockNodeMap[node] = blockNode;
    }

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
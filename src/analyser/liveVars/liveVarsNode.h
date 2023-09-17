#ifndef LIVE_VARS_NODE_H
#define LIVE_VARS_NODE_H

#include "../../ast/ASTHelper.cpp"

class liveVarsNode
{
    private:
    ASTNode* node;
    set<TableEntry*> use;
    set<TableEntry*> def;
    set<TableEntry*> in;
    set<TableEntry*> out;

    set<liveVarsNode*> predecessors;
    set<liveVarsNode*> successors;

    public:
    liveVarsNode(ASTNode*);
    
    ASTNode* getNode();
    set<TableEntry*> getUse();
    set<TableEntry*> getDef();
    set<TableEntry*> getIn();
    set<TableEntry*> getOut();
    set<liveVarsNode*> getPredecessors();
    set<liveVarsNode*> getSuccessors();

    void addPredecessor(liveVarsNode*);
    void addSuccessor(liveVarsNode*);
};

#endif
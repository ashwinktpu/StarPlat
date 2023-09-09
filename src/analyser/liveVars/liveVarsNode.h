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

    public:
    liveVarsNode(ASTNode*);
};

#endif
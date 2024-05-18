#include "liveVarsNode.h"

liveVarsNode::liveVarsNode()
    : node(nullptr)
{}

liveVarsNode::liveVarsNode(ASTNode* astnode)
    : node(astnode)
{
    astnode->setLiveVarsNode(this);
}

ASTNode* liveVarsNode::getNode()
{
    return node;
}

set<TableEntry*> liveVarsNode::getUse()
{
    return use;
}

set<TableEntry*> liveVarsNode::getDef()
{
    return def;
}

set<TableEntry*> liveVarsNode::getIn()
{
    return in;
}

set<TableEntry*> liveVarsNode::getOut()
{
    return out;
}

set<liveVarsNode*> liveVarsNode::getPredecessors()
{
    return predecessors;
}

set<liveVarsNode*> liveVarsNode::getSuccessors()
{
    return successors;
}

void liveVarsNode::setIn(set<TableEntry*> newIn)
{
    in = newIn;
}

void liveVarsNode::addOut(Identifier* id)
{
    out.insert(id->getSymbolInfo());
}

void liveVarsNode::addOut(set<TableEntry*> ids)
{
    for(TableEntry* id : ids)
        out.insert(id);
}

void liveVarsNode::addUse(Identifier* id)
{
    if(!id->getSymbolInfo())
        return;
    use.insert(id->getSymbolInfo());
}

void liveVarsNode::addDef(Identifier* id)
{
    if(!id->getSymbolInfo())
        return;
    def.insert(id->getSymbolInfo());
}

void liveVarsNode::addVars(usedVariables vars)
{
    for(Identifier* id : vars.getVariables(READ))
        addUse(id);
    for(Identifier* id : vars.getVariables(WRITE))
        addDef(id);
}

void liveVarsNode::addPredecessor(liveVarsNode* pred)
{
    if(pred == nullptr)
        return;
    predecessors.insert(pred);
    pred->addSuccessor(this);
}

void liveVarsNode::addPredecessors(set<liveVarsNode*> pred)
{
    for(auto p = pred.begin(); p != pred.end(); p++)
        predecessors.insert(*p);
}

void liveVarsNode::addSuccessor(liveVarsNode* succ)
{
    successors.insert(succ);
}

void liveVarsNode::addSuccessors(set<liveVarsNode*> succ)
{
    for(auto s = succ.begin(); s != succ.end(); s++)
        successors.insert(*s);
}
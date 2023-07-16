#include "blockVarsAnalyser.h"

VariableState meet(VariableState first, VariableState second)
{
    return (VariableState)(first | second);
}

NodeBlockData::NodeBlockData()
{
    this->node = NULL;
    this->block = NULL;
    this->startBlock = NULL;
    this->endBlock = NULL;
}

NodeBlockData::NodeBlockData(ASTNode *node)
{
    this->node = node;
    setNodeDataBlock();
    this->block = NULL;
    this->startBlock = NULL;
    this->endBlock = NULL;
    node->setBlockData(this);
}

NodeBlockData::NodeBlockData(ASTNode *node, ASTNodeBlock *Block)
{
    this->node = node;
    setNodeDataBlock();
    this->block = Block;
    this->startBlock = NULL;
    this->endBlock = NULL;
    node->setBlockData(this);
}

NodeBlockData::NodeBlockData(ASTNode *node, ASTNodeBlock *startBlock, ASTNodeBlock *endBlock)
{
    this->node = node;
    setNodeDataBlock();
    this->block = NULL;
    this->startBlock = startBlock;
    this->endBlock = endBlock;
    node->setBlockData(this);
}

NodeBlockData::NodeBlockData(ASTNode *node, ASTNodeBlock *Block, ASTNodeBlock *startBlock, ASTNodeBlock *endBlock)
{
    this->node = node;
    setNodeDataBlock();
    this->block = Block;
    this->startBlock = startBlock;
    this->endBlock = endBlock;
    node->setBlockData(this);
}

void NodeBlockData::setNodeDataBlock()
{
    if (node == NULL)
        return;

    node->setBlockData(this);
    return;
}

ASTNode *NodeBlockData::getNode()
{
    return node;
}

ASTNodeBlock *NodeBlockData::getBlock()
{
    return block;
}

ASTNodeBlock *NodeBlockData::getStartBlock()
{
    return startBlock;
}

ASTNodeBlock *NodeBlockData::getEndBlock()
{
    return endBlock;
}

void NodeBlockData::setBlock(ASTNodeBlock *block)
{
    this->block = block;
}

void NodeBlockData::setStartBlock(ASTNodeBlock *startBlock)
{
    this->startBlock = startBlock;
}

void NodeBlockData::setEndBlock(ASTNodeBlock *endBlock)
{
    this->endBlock = endBlock;
}

ASTNodeBlock::ASTNodeBlock()
{
    this->node = NULL;
    use = set<TableEntry *>();
    def = set<TableEntry *>();
    in = set<TableEntryWrapper *>();
    out = set<TableEntryWrapper *>();
    succ = set<ASTNodeBlock *>();
}

ASTNodeBlock::ASTNodeBlock(ASTNode *node)
{
    this->node = node;
    use = set<TableEntry *>();
    def = set<TableEntry *>();
    in = set<TableEntryWrapper *>();
    out = set<TableEntryWrapper *>();
    succ = set<ASTNodeBlock *>();
}

ASTNode *ASTNodeBlock::getNode()
{
    return node;
}

set<TableEntry *> ASTNodeBlock::getUse()
{
    return use;
}

set<TableEntry *> ASTNodeBlock::getDef()
{
    return def;
}

set<TableEntryWrapper *> ASTNodeBlock::getIn()
{
    return in;
}

set<TableEntryWrapper *> ASTNodeBlock::getOut()
{
    return out;
}

set<ASTNodeBlock *> ASTNodeBlock::getSucc()
{
    return succ;
}

void ASTNodeBlock::addUse(Identifier *id)
{
    if (!id->getSymbolInfo())
        return;
    use.insert(id->getSymbolInfo());
}

void ASTNodeBlock::addDef(Identifier *id)
{
    if (!id->getSymbolInfo())
        return;
    def.insert(id->getSymbolInfo());
}

void ASTNodeBlock::addIn(set<TableEntry *> ids)
{
    bool inParallel = this->getNode()->isInParallel();
    VariableState state = inParallel ? IN_GPU : IN_CPU;

    for (TableEntry *id : ids)
    {
        TableEntryWrapper *wrapper = new TableEntryWrapper(id, state);
        if (id != NULL)
            in.insert(wrapper);
    }
}

void ASTNodeBlock::addIn(set<TableEntryWrapper *> ids)
{
    // Find if the in set already contains the id
    // If it does, then we need to update the state
    // If it doesn't, then we need to insert it

    for (TableEntryWrapper *id : ids)
    {
        if (id != NULL)
        {
            auto it = in.find(id);
            if (it != in.end())
            {
                // Update state
                (*it)->state = meet((*it)->state, id->state);
            }
            else
            {
                // Insert
                in.insert(id);
            }
        }
    }
}

void ASTNodeBlock::removeIn(set<TableEntryWrapper *> ids)
{
    // Find if the in set already contains the id
    // If it does, then delete it

    for (TableEntryWrapper *id : ids)
    {
        if (id != NULL)
        {
            auto it = in.find(id);
            if (it != in.end())
            {
                // Delete
                in.erase(it);
            }
        }
    }
}

void ASTNodeBlock::addOut(set<TableEntry *> ids)
{
    bool inParallel = this->getNode()->isInParallel();
    VariableState state = inParallel ? IN_GPU : IN_CPU;

    for (TableEntry *id : ids)
    {
        TableEntryWrapper *wrapper = new TableEntryWrapper(id, state);
        if (id != NULL)
            in.insert(wrapper);
    }
}

void ASTNodeBlock::addOut(set<TableEntryWrapper *> ids)
{
    // Find if the out set already contains the id
    // If it does, then we need to update the state
    // If it doesn't, then we need to insert it

    for (TableEntryWrapper *id : ids)
    {
        if (id != NULL)
        {
            auto it = out.find(id);
            if (it != out.end())
            {
                // Update state
                (*it)->state = meet((*it)->state, id->state);
            }
            else
            {
                // Insert
                out.insert(id);
            }
        }
    }
}

void ASTNodeBlock::addSucc(ASTNodeBlock *block)
{
    if (block != NULL)
        succ.insert(block);
}

void ASTNodeBlock::addVars(usedVariables_t vars)
{
    for (Identifier *id : vars.getVariables(USED))
        addUse(id);
    for (Identifier *id : vars.getVariables(DEFINED))
        addDef(id);
}
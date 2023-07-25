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
    VariableState state = inParallel ? IN_GPU : IN_CPU;

    for (TableEntry *id : ids)
    {
        if (id == NULL) continue;

        // Delete already present instance of id
        for (auto it = in.begin(); it != in.end(); it++)
        {
            if ((*it)->entry == id)
            {
                in.erase(it);
                break;
            }
        }
        TableEntryWrapper *wrapper = new TableEntryWrapper(id, state);
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
            bool found = false;
            for (auto it = in.begin(); it != in.end(); it++)
            {
                if ((*it)->entry == id->entry)
                {
                    // Update state
                    (*it)->state = meet((*it)->state, id->state);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                // Insert
                in.insert(id);
            }
        }
    }
}

void ASTNodeBlock::removeIn(set<TableEntry *> ids) 
{
    // Find if the in set already contains the id
    // If it does, then we need to erase it

    for (TableEntry *id : ids)
    {
        if (id != NULL)
        {
            for (auto it = in.begin(); it != in.end(); it++)
            {
                if ((*it)->entry == id)
                {
                    in.erase(it);
                    break;
                }
            }
        }
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
            bool found = false;
            for (auto it = out.begin(); it != out.end(); it++)
            {
                if ((*it)->entry == id->entry)
                {
                    // Update state
                    (*it)->state = meet((*it)->state, id->state);
                    found = true;
                    break;
                }
            }

            if (!found)
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

void ASTNodeBlock::setInParallel(bool inParallel)
{
    this->inParallel = inParallel;
}

bool ASTNodeBlock::isInParallel()
{
    return inParallel;
}
#include "blockVarsAnalyser.h"

ostream &operator<<(ostream &os, const VariableState state)
{
    switch (state)
    {
    case VariableState::IN_CPU:
        os << "IN_CPU";
        break;
    case VariableState::IN_GPU:
        os << "IN_GPU";
        break;
    case VariableState::IN_BOTH:
        os << "IN_BOTH";
        break;
    }
    return os;
}

TableEntryWrapper::TableEntryWrapper(TableEntry *entry, VariableState state)
{
    this->entry = entry;
    this->state = state;
}

blockVarsAnalyser::blockVarsAnalyser()
{
    blockNodes.clear();
    blockNodeMap.clear();
}

ASTNodeBlock *blockVarsAnalyser::initBlockNode()
{
    ASTNodeBlock *blockNode = new ASTNodeBlock();
    return blockNode;
}

ASTNodeBlock *blockVarsAnalyser::initBlockNode(ASTNode *node)
{
    ASTNodeBlock *blockNode = new ASTNodeBlock(node);
    return blockNode;
}

void blockVarsAnalyser::addBlockNode(ASTNode *node, ASTNodeBlock *blockNode)
{
    blockNodes.push_back(blockNode);
    blockNodeMap[node] = new NodeBlockData(node, blockNode);
}

void blockVarsAnalyser::addBlockNode(ASTNode *node, ASTNodeBlock *startBlock, ASTNodeBlock *endBlock)
{
    blockNodeMap[node] = new NodeBlockData(node, startBlock, endBlock);
}

NodeBlockData *blockVarsAnalyser::getBlockData(ASTNode *node)
{
    return blockNodeMap[node];
}

void blockVarsAnalyser::toString(NODETYPE type)
{
    switch (type)
    {
    case NODE_ID:
        cout << "NODE_ID";
        break;
    case NODE_PROPACCESS:
        cout << "NODE_PROPACCESS";
        break;
    case NODE_FUNC:
        cout << "NODE_FUNC";
        break;
    case NODE_TYPE:
        cout << "NODE_TYPE";
        break;
    case NODE_FORMALPARAM:
        cout << "NODE_FORMALPARAM";
        break;
    case NODE_STATEMENT:
        cout << "NODE_STATEMENT";
        break;
    case NODE_BLOCKSTMT:
        cout << "NODE_BLOCKSTMT";
        break;
    case NODE_DECL:
        cout << "NODE_DECL";
        break;
    case NODE_ASSIGN:
        cout << "NODE_ASSIGN";
        break;
    case NODE_WHILESTMT:
        cout << "NODE_WHILESTMT";
        break;
    case NODE_DOWHILESTMT:
        cout << "NODE_DOWHILESTMT";
        break;
    case NODE_FIXEDPTSTMT:
        cout << "NODE_FIXEDPTSTMT";
        break;
    case NODE_IFSTMT:
        cout << "NODE_IFSTMT";
        break;
    case NODE_ITRBFS:
        cout << "NODE_ITRBFS";
        break;
    case NODE_ITRRBFS:
        cout << "NODE_ITRRBFS";
        break;
    case NODE_EXPR:
        cout << "NODE_EXPR";
        break;
    case NODE_PROCCALLEXPR:
        cout << "NODE_PROCCALLEXPR";
        break;
    case NODE_PROCCALLSTMT:
        cout << "NODE_PROCCALLSTMT";
        break;
    case NODE_FORALLSTMT:
        cout << "NODE_FORALLSTMT";
        break;
    case NODE_REDUCTIONCALL:
        cout << "NODE_REDUCTIONCALL";
        break;
    case NODE_REDUCTIONCALLSTMT:
        cout << "NODE_REDUCTIONCALLSTMT";
        break;
    case NODE_UNARYSTMT:
        cout << "NODE_UNARYSTMT";
        break;
    case NODE_RETURN:
        cout << "NODE_RETURN";
        break;
    case NODE_BATCHBLOCKSTMT:
        cout << "NODE_BATCHBLOCKSTMT";
        break;
    case NODE_ONADDBLOCK:
        cout << "NODE_ONADDBLOCK";
        break;
    case NODE_ONDELETEBLOCK:
        cout << "NODE_ONDELETEBLOCK";
        break;
    case NODE_TRANSFERSTMT:
        cout << "NODE_TRANSFERSTMT";
        break;
    default:
        cout << "UNKNOWN";
        break;
    };
}

void blockVarsAnalyser::printBlockNodes()
{
    int i = 0;
    for (auto blockNode : blockNodes)
    {
        cout << "Block number " << ++i << ": ";
        if (blockNode->getNode() == NULL)
            cout << "NULL";
        else
            toString(blockNode->getNode()->getTypeofNode());
        cout << endl;
        for (TableEntry *te : blockNode->getDef())
            cout << "Def: " << te->getId()->getIdentifier() << endl;
        for (TableEntry *te : blockNode->getUse())
            cout << "Use: " << te->getId()->getIdentifier() << endl;
        for (TableEntryWrapper *te : blockNode->getIn())
            cout << "In: " << te->entry->getId()->getIdentifier() << ", State:" << te->state << endl;
        for (TableEntryWrapper *te : blockNode->getOut())
            cout << "Out: " << te->entry->getId()->getIdentifier() << ", State:" << te->state << endl;
        for (ASTNodeBlock *succ : blockNode->getSucc())
        {
            cout << "Succ: ";
            if (succ->getNode() == NULL)
                cout << "NULL";
            else
                toString(succ->getNode()->getTypeofNode());
            cout << endl;
        }
    }
}

void blockVarsAnalyser::analyseBlockNodes()
{
    bool changed = true;
    int count = 0;
    do
    {
        // reset changed flag
        changed = false;

        // iterate over all block nodes (in reverse order implicit*)
        for (ASTNodeBlock *blockNode : blockNodes)
        {
            // get old in and out sets
            set<TableEntryWrapper *> oldIn = blockNode->getIn();
            set<TableEntryWrapper *> oldOut = blockNode->getOut();

            // update in and out sets
            for (ASTNodeBlock *succ : blockNode->getSucc())
                blockNode->addOut(succ->getIn());
            blockNode->addIn(blockNode->getOut());
            blockNode->removeIn(blockNode->getDef());
            blockNode->addIn(blockNode->getUse());

            if (changed)
                continue;

            // check if in or out sets have changed
            if (oldIn.size() != blockNode->getIn().size() || oldOut.size() != blockNode->getOut().size())
            {
                changed = true;
            }
            else
            {
                for (TableEntryWrapper *te : oldIn)
                {
                    bool found = false;
                    for (TableEntryWrapper *te2 : blockNode->getIn())
                    {
                        if (te->entry == te2->entry)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        changed = true;
                        break;
                    }
                }
                if (changed)
                    continue;

                for (TableEntryWrapper *te : blockNode->getIn())
                {
                    bool found = false;
                    for (TableEntryWrapper *te2 : oldIn)
                    {
                        if (te->entry == te2->entry)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        changed = true;
                        break;
                    }
                }
                if (changed)
                    continue;

                for (TableEntryWrapper *te : oldOut)
                {
                    bool found = false;
                    for (TableEntryWrapper *te2 : blockNode->getOut())
                    {
                        if (te->entry == te2->entry)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        changed = true;
                        break;
                    }
                }
                if (changed)
                    continue;

                for (TableEntryWrapper *te : blockNode->getOut())
                {
                    bool found = false;
                    for (TableEntryWrapper *te2 : oldOut)
                    {
                        if (te->entry == te2->entry)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        changed = true;
                        break;
                    }
                }
            }
        }

    } while (changed && count < 100);
}

void blockVarsAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;

    // setting all variable present inside parallel region
    setVarsInParallelStatment(func->getBlockStatement());

    // creating end block
    ASTNodeBlock *endBlock = new ASTNodeBlock();
    blockNodes.push_back(endBlock);

    // creating start block
    ASTNodeBlock *blockNode = initBlockNode(func);
    for (formalParam *param : func->getParamList())
        blockNode->addDef(param->getIdentifier());

    blockNode->addSucc(initStatement(func->getBlockStatement(), NULL));
    addBlockNode(func, blockNode);

    analyseBlockNodes();
    // printBlockNodes();
}

void blockVarsAnalyser::analyse(list<Function *> functions)
{
    for (Function *function : functions)
        analyseFunc(function);
}
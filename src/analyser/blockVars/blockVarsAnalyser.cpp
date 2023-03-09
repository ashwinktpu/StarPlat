#include "blockVarsAnalyser.h"

void blockVarsAnalyser::toString(NODETYPE type)
{
    switch(type)
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
    for (auto blockNode: blockNodes) 
    {
        cout << "Block number " << ++i << ": "; 
        toString(blockNode->getNode()->getTypeofNode());
        cout << endl;
        for (Identifier* id : blockNode->getDef()) 
        {
            cout << "Def: " << id->getIdentifier() << endl;
        }
        for (Identifier* id : blockNode->getUse()) 
        {
            cout << "Use: " << id->getIdentifier() << endl;
        }
        for (Identifier* id : blockNode->getIn()) 
        {
            cout << "In: " << id->getIdentifier() << endl;
        }
        for (Identifier* id : blockNode->getOut()) 
        {
            cout << "Out: " << id->getIdentifier() << endl;
        }
        for (ASTNodeBlock* succ : blockNode->getSucc()) 
        {
            cout << "Succ: ";
            toString(succ->getNode()->getTypeofNode());
            cout << endl;
        }
    }
}

void blockVarsAnalyser::analyseFunc(ASTNode* proc) 
{
    Function* func = (Function*)proc;

    cout << "Function Name: " << func->getIdentifier()->getIdentifier() << endl;

    ASTNodeBlock* blockNode = initBlockNode(func);
    for (formalParam* param : func->getParamList()) 
    {
        blockNode->addDef(param->getIdentifier());
    }

    blockNode->addSucc(initStatement(func->getBlockStatement(), NULL));
    addBlockNode(func, blockNode);

    printBlockNodes();
    // analyseBlockNodes();
}

void blockVarsAnalyser::analyse(list<Function*> functions) 
{
    for(Function* function : functions) 
        analyseFunc(function);
}
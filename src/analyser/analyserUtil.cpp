#ifndef ANALYSER_UTILITY
#define ANALYSER_UTILITY

#include "../ast/ASTNodeTypes.hpp"
#include <unordered_map>
//#include "../symbolutil/SymbolTable.h"

#define propKey pair<TableEntry *, TableEntry *>

enum variable_type
{
    READ = 1,
    WRITE,
    READ_WRITE,
    READ_ONLY, 
    WRITE_ONLY,
    READ_AND_WRITE
};

/*
enum depeandancy
{
    READ_READ,
    READ_WRITE,
    WRTIE_READ,
    WRITE_WRTIE,
    ALL_READ,
    ALL_WRITE,
    READ_ALL,
    WRITE_ALL
}

struct IdentifierWrap
{
    string id;
    Identifier *iden;

    IdentifierWrap() {}
    IdentifierWrap(Identifier *iden)
    {
        this->id = string(iden->getIdentifier());
        this->iden = iden;
    }
    bool operator==(const IdentifierWrap &iden) const{
        return (this->id == iden.id);
    }

    struct HashFunction
    {
        size_t operator()(const IdentifierWrap &iden) const{
            return hash<string>()(iden.id);
        }
    };
};*/

class usedVariables
{
private:
    struct hash_pair
    {
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2> &p) const
        {
            auto hash1 = hash<T1>{}(p.first);
            auto hash2 = hash<T2>{}(p.second);
            return hash1 ^ hash2;
        }
    };

    unordered_map<TableEntry *, Identifier *> readVars, writeVars;
    unordered_map<propKey, PropAccess *, hash_pair> readProp, writeProp;

public:
    usedVariables() {}

    void addVariable(Identifier *iden, int type)
    {
        if (iden->getSymbolInfo() == NULL)  // To not include the iterators in the list
            return;
        if (type & 1)
            readVars.insert({iden->getSymbolInfo(), iden});
        if (type & 2)
            writeVars.insert({iden->getSymbolInfo(), iden});
    }

    void addPropAccess(PropAccess *prop, int type)
    {
        Identifier *iden1 = prop->getIdentifier1();
        Identifier *iden2 = prop->getIdentifier2();
        propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

        if (type & 1)
            readProp.insert({prop_key, prop});
        if (type & 2)
            writeProp.insert({prop_key, prop});
    }

    void merge(usedVariables usedVars1)
    {
        for (pair<TableEntry *, Identifier *> iden : usedVars1.readVars)
            this->readVars.insert({iden.first, iden.second});

        for (pair<TableEntry *, Identifier *> iden : usedVars1.writeVars)
            this->writeVars.insert({iden.first, iden.second});

        for (pair<propKey, PropAccess *> iden : usedVars1.readProp)
            this->readProp.insert({iden.first, iden.second});

        for (pair<propKey, PropAccess *> iden : usedVars1.writeProp)
            this->writeProp.insert({iden.first, iden.second});
    }

    void removeVariable(Identifier *iden, int type)
    {
        TableEntry *symbInfo = iden->getSymbolInfo();
        if (type & 1)
        {
            if (readVars.find(symbInfo) != readVars.end())
                readVars.erase(symbInfo);
        }

        if (type & 2)
        {
            if (writeVars.find(symbInfo) != writeVars.end())
                writeVars.erase(symbInfo);
        }
    }

    bool isUsedVar(Identifier *iden, int type = READ_WRITE)
    {
        TableEntry *symbInfo = iden->getSymbolInfo();
        if (type == READ)
            return (readVars.find(symbInfo) != readVars.end());
        else if (type == WRITE)
            return (writeVars.find(symbInfo) != writeVars.end());
        else
            return ((readVars.find(symbInfo) != readVars.end()) || (writeVars.find(symbInfo) != writeVars.end()));
    }

    list<Identifier *> getVariables(int type = READ_WRITE)
    {
        list<Identifier *> result;

        if (type == READ)
        {
            for (pair<TableEntry *, Identifier *> iden : readVars)
                result.push_back(iden.second);
        }
        else if (type == WRITE)
        {
            for (pair<TableEntry *, Identifier *> iden : writeVars)
                result.push_back(iden.second);
        }
        else if(type == READ_WRITE)
        {
            for (pair<TableEntry *, Identifier *> iden : readVars)
                result.push_back(iden.second);
            
            for (pair<TableEntry *, Identifier *> iden : writeVars)
            {
                if(readVars.find(iden.first) == readVars.end())
                    result.push_back(iden.second);
            }
        }
        else if(type == READ_AND_WRITE)
        {
            for (pair<TableEntry *, Identifier *> iden: writeVars)
            {
                if(readVars.find(iden.first) != readVars.end())
                    result.push_back(iden.second);
            }
        }
        else if(type == READ_ONLY)
        {
            for (pair<TableEntry *, Identifier *> iden : readVars)
            {
                if(writeVars.find(iden.first) == writeVars.end())
                    result.push_back(iden.second);
            }
        }
        else if(type == WRITE_ONLY)
        {
            for (pair<TableEntry *, Identifier *> iden: writeVars)
            {
                if(readVars.find(iden.first) == readVars.end())
                    result.push_back(iden.second);
            }
        }
        return result;
    }

    bool hasVariables(int type = READ_WRITE)
    {
        if (type == READ)
            return (readVars.size() > 0);
        else if (type == WRITE)
            return (writeVars.size() > 0);
        else
            return ((writeVars.size() > 0) || (readVars.size() > 0));
    }

    bool isUsedPropAcess(PropAccess *propId, int type = READ_WRITE)
    {
        Identifier *iden1 = propId->getIdentifier1();
        Identifier *iden2 = propId->getIdentifier2();
        propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

        if (type == READ)
            return (readProp.find(prop_key) != readProp.end());
        else if (type == WRITE)
            return (writeProp.find(prop_key) != writeProp.end());
        else
            return ((readProp.find(prop_key) != readProp.end()) || (writeProp.find(prop_key) != writeProp.end()));
    }

    bool isUsedProp(PropAccess *propId, int type = READ_WRITE)
    {
        Identifier *iden2 = propId->getIdentifier2();
        TableEntry *symbInfo = iden2->getSymbolInfo();

        if (type & 1)
        {
            for (pair<propKey, PropAccess *> iden : readProp)
                if (iden.first.second == symbInfo)
                    return true;
        }
        if (type & 2)
        {
            for (pair<propKey, PropAccess *> iden : writeProp)
                if (iden.first.second == symbInfo)
                    return true;
        }
        return false;
    }

    list<PropAccess *> getPropAcess(int type = READ_WRITE)
    {
        list<PropAccess *> result;
        if (type & 1)
        {
            for (pair<propKey, PropAccess *> iden : readProp)
                result.push_back(iden.second);
        }
        if (type & 2)
        {
            for (pair<propKey, PropAccess *> iden : writeProp)
                result.push_back(iden.second);
        }
        return result;
    }

    void clear()
    {
        readVars.clear();
        writeVars.clear();

        readProp.clear();
        writeProp.clear();
    }
};

class analyserUtils
{
public:
    static usedVariables getVarsExpr(Expression *expr)
    {
        usedVariables result;

        if (expr->isIdentifierExpr())
        {
            Identifier *iden = expr->getId();
            result.addVariable(iden, READ);
        }
        else if (expr->isPropIdExpr())
        {
            PropAccess *propExpr = expr->getPropId();
            result.addPropAccess(propExpr, READ);
        }
        else if (expr->isUnary())
        {
            if (expr->getOperatorType() == OPERATOR_NOT)
                result = getVarsExpr(expr->getUnaryExpr());
            else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC))
            {
                Expression *uExpr = expr->getUnaryExpr();
                if (uExpr->isIdentifierExpr())
                    result.addVariable(uExpr->getId(), READ_WRITE);
                else if (uExpr->isPropIdExpr())
                    result.addPropAccess(uExpr->getPropId(), READ_WRITE);
            }
        }
        else if (expr->isLogical() || expr->isArithmetic() || expr->isRelational())
        {
            result = getVarsExpr(expr->getLeft());
            result.merge(getVarsExpr(expr->getRight()));
        }
        return result;
    }
};

class printAST
{
    void printTabs()
    {
        int temp = tabSpace;
        while (temp--)
            cout << '\t';
    }

public:
    int tabSpace = 0;

    void printFunction(Function *func)
    {
        printTabs();
        cout << "Function " << string(func->getIdentifier()->getIdentifier()) << '\n';
        tabSpace++;
        printBlock(func->getBlockStatement());
        tabSpace--;
    }

    void printBlock(blockStatement *stmt)
    {
        printTabs();
        cout << "{\n";
        tabSpace++;

        for (statement *body : stmt->returnStatements())
            printStatement(body);

        tabSpace--;
        printTabs();
        cout << "}\n";
    }

    void printStatement(statement *stmt)
    {
        if (stmt == nullptr)
            return;

        switch (stmt->getTypeofNode())
        {
        case NODE_DECL:
            printTabs();
            cout << "Declaration\n";
            break;

        case NODE_ASSIGN:
            printTabs();
            cout << "Assignment\n";
            break;

        case NODE_REDUCTIONCALLSTMT:
            printTabs();
            cout << "Reduction\n";
            break;

        case NODE_PROCCALLSTMT:
            printTabs();
            cout << "Procedure Call\n";
            break;

        case NODE_UNARYSTMT:
            printTabs();
            cout << "Unary statment\n";
            break;

        case NODE_BLOCKSTMT:
            printBlock((blockStatement *)stmt);
            break;

        case NODE_WHILESTMT:
            printTabs();
            cout << "While\n";
            tabSpace++;
            printStatement(((whileStmt *)stmt)->getBody());
            tabSpace--;
            break;

        case NODE_DOWHILESTMT:
            printTabs();
            cout << "Do While\n";
            tabSpace++;
            printStatement(((dowhileStmt *)stmt)->getBody());
            tabSpace--;
            break;

        case NODE_FORALLSTMT:
            printTabs();
            cout << "For all\n";
            tabSpace++;
            printStatement(((forallStmt *)stmt)->getBody());
            tabSpace--;
            break;

        case NODE_FIXEDPTSTMT:
            printTabs();
            cout << "Fixed Point\n";
            tabSpace++;
            printStatement(((fixedPointStmt *)stmt)->getBody());
            tabSpace--;
            break;

        case NODE_ITRBFS:
            printTabs();
            cout << "ITRBFS\n";
            tabSpace++;
            printStatement(((iterateBFS *)stmt)->getBody());
            tabSpace--;
            if (((iterateBFS *)stmt)->getRBFS())
            {
                cout << "ITRBFS Reverse\n";
                tabSpace++;
                printStatement(((iterateBFS *)stmt)->getRBFS()->getBody());
                tabSpace--;
            }
            break;

        case NODE_IFSTMT:
            printTabs();
            cout << "If statment\n";
            tabSpace++;
            printStatement(((ifStmt *)stmt)->getIfBody());
            tabSpace--;
            if (((ifStmt *)stmt)->getElseBody())
            {
                printTabs();
                cout << "Else statment\n";
                tabSpace++;
                printStatement(((ifStmt *)stmt)->getElseBody());
                tabSpace--;
            }
            break;
        
        case NODE_TRANSFERSTMT:
        {
            varTransferStmt* cstmt = (varTransferStmt*) stmt;
            printTabs();
            cout<<"Tranfer statement\n";

            tabSpace++;
            printTabs();
            cout<<cstmt->transferVar->getIdentifier()<<' ';
            cout<<(cstmt->direction ? "GPU to CPU" : "CPU to GPU")<<endl;
            tabSpace--;
        }
        default:
            break;
        }
    }
};

/*
usedVariables getVarsBlock(statement *inp_stmt)
{
    list<statement *> stmtList = ((blockStatement*) inp_stmt)->returnStatements();
    usedVariables result;

    for (statement *stmt: stmtList)
    {
        if (stmt->getTypeofNode() == NODE_ASSIGN)
        {
            assignment *assign = stmt->getAssignment();
            Expression *lExpr = assign->getLeft();
            Expression *rExpr = assign->getRight();

            if (lExpr->isIdentifierExpr())
            {
                Identifier *iden = lExpr->getId();
                result.addVariable(iden, WRITE);
            }
            else if (lExpr->isPropIdExpr())
            {
                PropAccess *propExpr = lExpr->getPropId();
                result.addPropAccess(propExpr, WRITE);
            }

            result.merge(getVarsExpr(rExpr));

        }
        else if (stmt->getTypeOfNode() == NODE_UNARYSTMT)
        {
            result.merge(getVarsExpr(stmt->getUnaryExpr()));
        }
        else if (stmt->getTypeOfNode() == NODE_DECL)
        {
            result.addVariable(stmt->getDeclId(), WRITE);
            if (stmt->getExpressionAssigned() != NULL) {
                result.merge(getVarsExpr(stmt->getExprressionAssigned()));
            }
        }
        else if (stmt->getTypeOfNode() == NODE_BLOCKSTMT)
        {
            result.merge(getVarsBlock(stmt));
        }
    }

    return result;
}

usedVariables getDeclBlock(statement *inp_stmt)
{
    list<statement *> stmtList = inp_stmt->returnStatements();
    usedVariables result;

    for (statement *stmt: stmtList)
    {
        if (inp_stmt->getTypeOfNode() == NODE_DECL)
        {
            result.addVariable(stmt->getDeclId(), WRITE);
        }
        else if (inp_stmt->getTypeOfNode() == NODE_BLOCKSTMT)
        {
            list<statement *> stmtList = inp_stmt->returnStatements();
            for (statement *stmt: stmtList)
                result.merge(getDeclBlock(stmt));
        }
    }
    return result;
}*/

#endif
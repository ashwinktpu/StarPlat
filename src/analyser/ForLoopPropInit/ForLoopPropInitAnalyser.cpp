#include"ForLoopPropInitAnalyser.h"

#include<vector>
#include <string.h>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include "../analyserUtil.cpp"
#include "../../ast/ASTHelper.cpp"

#include <iostream>
using namespace std;

list<statement*> filter_statement(statement* stmt){
    list<statement*> declarations;
    list<statement*> recursive_declarations;
    switch (stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:{
            list<statement*> new_statements;
            blockStatement* block_stmt = (blockStatement*)stmt;
            for(auto temp_stmt:block_stmt->returnStatements()){
                if(temp_stmt->getTypeofNode()==NODE_DECL){
                    Type *type = ((declaration *)temp_stmt)->getType();
                    if (type->isPropType()){
                        declarations.push_back(temp_stmt);
                    }
                    else{
                        recursive_declarations = filter_statement(temp_stmt);
                        declarations.splice(declarations.end(),recursive_declarations);
                        new_statements.push_back(temp_stmt);
                    }
                }
                else{
                    recursive_declarations = filter_statement(temp_stmt);
                    declarations.splice(declarations.end(),recursive_declarations);
                    new_statements.push_back(temp_stmt);
                }
            }
            block_stmt->clearStatements();
            for(auto temp_stmt:new_statements){
                block_stmt->addStmtToBlock(temp_stmt);
            }
            break;
        }

        case NODE_WHILESTMT:
            recursive_declarations = filter_statement(((whileStmt *)stmt)->getBody());
            declarations.splice(declarations.end(),recursive_declarations);
            break;

        case NODE_DOWHILESTMT:
            recursive_declarations = filter_statement(((dowhileStmt *)stmt)->getBody());
            declarations.splice(declarations.end(),recursive_declarations);
            break;

        case NODE_FIXEDPTSTMT:
            recursive_declarations = filter_statement(((fixedPointStmt *)stmt)->getBody());
            declarations.splice(declarations.end(),recursive_declarations);
            break;

        case NODE_IFSTMT:
        {
            statement *ifBody = ((ifStmt *)stmt)->getIfBody();
            statement *elseBody = ((ifStmt *)stmt)->getElseBody();
            if (ifBody != NULL){
                recursive_declarations = filter_statement(ifBody);
                declarations.splice(declarations.end(),recursive_declarations);
            }
            if (elseBody != NULL){
                recursive_declarations = filter_statement(elseBody);
                declarations.splice(declarations.end(),recursive_declarations);
            }
            break;
        }

        case NODE_ITRBFS:
        {
            recursive_declarations = filter_statement(((iterateBFS *)stmt)->getBody());
            declarations.splice(declarations.end(),recursive_declarations);
            iterateReverseBFS *revBFS = ((iterateBFS *)stmt)->getRBFS();
            if (revBFS != NULL){
                recursive_declarations = filter_statement(revBFS->getBody());
                declarations.splice(declarations.end(),recursive_declarations);
            }

            break;
        }

        case NODE_FORALLSTMT:
        {
            recursive_declarations = filter_statement(((forallStmt*)stmt)->getBody());
            declarations.splice(declarations.end(),recursive_declarations);
            break;
        }
    }
    return declarations;
}


void ForLoopPropInitAnalyser::analyse(list<Function*> funcList){
    for(Function* fun:funcList){
        analyseFunc(fun);
    }
}

void ForLoopPropInitAnalyser::analyseFunc(Function* fun){
    analyseBlockStatement(fun->getBlockStatement());
}

void ForLoopPropInitAnalyser::analyseBlockStatement(blockStatement* stmt){
    list<statement*> new_statements;
    list<statement*> prop_declarations;
    for(auto temp_stmt:stmt->returnStatements()){
        if(temp_stmt->getTypeofNode()==NODE_FORALLSTMT){
            list<statement*> recursive_declarations = filter_statement(((forallStmt*)temp_stmt)->getBody());
            prop_declarations.splice(prop_declarations.end(),recursive_declarations);
            new_statements.push_back(temp_stmt);
        }
        else{
            analyseStatement(temp_stmt);
            new_statements.push_back(temp_stmt);
        }
    }
    stmt->clearStatements();
    for(auto temp_stmt:prop_declarations){
        stmt->addStmtToBlock(temp_stmt);
    }
    for(auto temp_stmt:new_statements){
        stmt->addStmtToBlock(temp_stmt);
    }
}



void ForLoopPropInitAnalyser::analyseStatement(statement* stmt){
    switch (stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:
            analyseBlockStatement((blockStatement*)stmt);
            break;

        case NODE_WHILESTMT:
            analyseStatement(((whileStmt *)stmt)->getBody());
            break;

        case NODE_DOWHILESTMT:
            analyseStatement(((dowhileStmt *)stmt)->getBody());
            break;

        case NODE_FIXEDPTSTMT:
            analyseStatement(((fixedPointStmt *)stmt)->getBody());
            break;

        case NODE_IFSTMT:
        {
            statement *ifBody = ((ifStmt *)stmt)->getIfBody();
            statement *elseBody = ((ifStmt *)stmt)->getElseBody();
            if (ifBody != NULL)
            analyseStatement(ifBody);
            if (elseBody != NULL)
            analyseStatement(elseBody);
            break;
        }

        case NODE_ITRBFS:
        {
            analyseStatement(((iterateBFS *)stmt)->getBody());
            iterateReverseBFS *revBFS = ((iterateBFS *)stmt)->getRBFS();
            if (revBFS != NULL)
            analyseStatement(revBFS->getBody());
            break;
        }

        case NODE_FORALLSTMT:
        {
            analyseStatement(((forallStmt*)stmt)->getBody());
            break;
        }
    }
}

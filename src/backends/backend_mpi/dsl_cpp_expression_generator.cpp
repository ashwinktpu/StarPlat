#include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

namespace spmpi {

    void dsl_cpp_generator::generate_exprPropId(PropAccess* propId) 
    {
        char strBuffer[1024];
        Identifier* id1 = propId->getIdentifier1();
        Identifier* id2 = propId->getIdentifier2();
        Expression * indexExpr = propId->getPropExpr();
        ASTNode* propParent = propId->getParent();
        
        
        if(propId->isPropertyExpression())
        {
            generate_exprIndexExpr(indexExpr);
            sprintf(strBuffer,".getValue(%s)", id1->getIdentifier());
            main.pushString(strBuffer);
        }
        else{
            if (id2->getSymbolInfo() != NULL){
                if (id2->get_forall_filter_association()) {
                    sprintf(strBuffer, "%s.getHistoryValue(%s)", id2->getIdentifier(), id1->getIdentifier());
            // printf("Inside this checked !\n");
                } 
                else {
                    sprintf(strBuffer, "%s.getValue(%s)",id2->getIdentifier(), id1->getIdentifier());
                }

            } 
            else {
                string s ="weight";
                if(s.compare(id2->getIdentifier()) == 0)
                    sprintf(strBuffer, "%s.getValue(%s)",id2->getIdentifier(), id1->getIdentifier());
                else     
                    sprintf(strBuffer, "%s.%s",id1->getIdentifier(),id2->getIdentifier());
            }
            main.pushString(strBuffer);
        }
    }


    /* generateExpr function is the entry point for generation of an expression. Based on 
    the type of expression it calls the approproate expression generator of that type*/
    void dsl_cpp_generator::generateExpr(Expression* expr) 
    {
        if (expr->isLiteral()) 
            generate_exprLiteral(expr);

        else if (expr->isInfinity()) 
            generate_exprInfinity(expr);

        else if (expr->isIdentifierExpr()) 
            generate_exprIdentifier(expr->getId());

        else if (expr->isPropIdExpr()) 
            generate_exprPropId(expr->getPropId());

        else if (expr->isArithmetic() || expr->isLogical()) 
            generate_exprArL(expr);

        else if (expr->isRelational()) 
            generate_exprRelational(expr);

        else if (expr->isProcCallExpr()) 
            generate_exprProcCall(expr);

        else if (expr->isUnary()) 
            generate_exprUnary(expr);

        else if (expr->isIndexExpr())
            generate_exprIndexExpr(expr);    

        else 
            assert(false);
    }

    void dsl_cpp_generator::generate_exprIndexExpr(Expression * expr)
    {
        char strBuffer[1024];
        Expression* mapExpr = expr->getMapExpr();
        Expression* indexExpr = expr->getIndexExpr();

        Identifier* mapExprId = mapExpr->getId();

        if(mapExpr->isIdentifierExpr()){  
            sprintf(strBuffer , "%s[", mapExprId->getIdentifier());
            main.pushString(strBuffer);
            generateExpr(indexExpr);
            main.pushString("]");
        }
        else if(mapExpr->isPropIdExpr()){
            PropAccess * propId = (PropAccess *)mapExpr;
            Identifier * id1 = propId->getIdentifier1();
            Identifier * id2 = propId->getIdentifier2();
            if (id2->getSymbolInfo() != NULL){
                sprintf(strBuffer,"%s[",id2->getIdentifier());
                main.pushString(strBuffer);
                generateExpr(indexExpr);
                sprintf(strBuffer,"][%s]",id1->getIdentifier());
                main.pushString(strBuffer);
            } 
            else {
                assert(false);
            }
        
        }
        else 
        {
            assert(false);
        }

    }

    /* generate_exprProcCall generates code fpr expressions which are procedure calls (inbuilt or otherwise). 
    Not support for all inbuilt might be present as of now, will be added as required. */
    void dsl_cpp_generator::generate_exprProcCall(Expression* expr) {
        char strBuffer[1024];
        proc_callExpr* proc = (proc_callExpr*)expr;
        string methodId(proc->getMethodId()->getIdentifier());
        if (methodId == "get_edge") {
                
                list<argument*> argList = proc->getArgList();
                assert(argList.size() == 2);
            
                Identifier* srcId = argList.front()->getExpr()->getId();
                Identifier* destId = argList.back()->getExpr()->getId();
                Identifier* objectId = proc->getId1();
                sprintf(strBuffer, "%s.%s(%s, %s)", objectId->getIdentifier(), "get_edge", srcId->getIdentifier(), destId->getIdentifier());
                main.pushString(strBuffer);
                // TODO : (Atharva) Some Dynamic specific code may come in here.
        } else if (methodId == "count_outNbrs") {
                list<argument*> argList = proc->getArgList();
                assert(argList.size() == 1);
                Identifier* srcId = argList.front()->getExpr()->getId();
                Identifier* objectId = proc->getId1();
                sprintf(strBuffer, "%s.%s(%s)", objectId->getIdentifier(), "num_out_nbrs", srcId->getIdentifier());
                main.pushString(strBuffer);
        } else if (methodId == "is_an_edge") {
            list<argument*> argList = proc->getArgList();
            assert(argList.size() == 2);
            Identifier* srcId = argList.front()->getExpr()->getId();
            Identifier* destId = argList.back()->getExpr()->getId();
            Identifier* objectId = proc->getId1();
            sprintf(strBuffer, "%s.%s(%s, %s)", objectId->getIdentifier(), "check_if_nbr", srcId->getIdentifier(), destId->getIdentifier());
            main.pushString(strBuffer);

        } 
        else if(methodId =="push") {
            Identifier* objectId = proc->getId1();
            Expression* indexExpr = proc->getIndexExpr();
            if(objectId != NULL)
            {
                sprintf(strBuffer,"%s.push(",objectId->getIdentifier());
                main.pushString(strBuffer);
                generateArgList(proc->getArgList());
                main.pushString(")");

            }
            else if(indexExpr != NULL)
            {
                generate_exprIndexExpr(indexExpr);
                main.pushString(".push(");
                generateArgList(proc->getArgList());
                main.pushString(")");

            }
            else{
                assert(false);
            }

        }
        else { // This is the default generation for non inbuilt(or non supported inbuilt)
            list<argument*> argList = proc->getArgList();
            if (argList.size() == 0) {
                Identifier* objectId = proc->getId1();
                sprintf(strBuffer, "%s.%s( )", objectId->getIdentifier(), proc->getMethodId()->getIdentifier());
                main.pushString(strBuffer);
            }
            else {
                
                Identifier* objectId = proc->getId1();

                Expression* indexExpr = proc->getIndexExpr();
                if(objectId!=NULL)
                {
                    Identifier* id2 = proc->getId2();
                    if(id2 != NULL)
                        sprintf(strBuffer,"%s.%s.%s",objectId->getIdentifier(), id2->getIdentifier(), proc->getMethodId()->getIdentifier());
                    else
                        sprintf(strBuffer,"%s.%s",objectId->getIdentifier(), proc->getMethodId()->getIdentifier());    
                }
                else if(indexExpr != NULL)
                {
                    generate_exprIndexExpr(indexExpr);
                    main.pushString(".");
                    main.pushString(proc->getMethodId()->getIdentifier());
                }
                else
                {
                    sprintf(strBuffer,"%s",proc->getMethodId()->getIdentifier());
                } 
                main.pushString(strBuffer);
                main.pushString("(");
                generateArgList(argList);
                if(objectId == NULL)
                {   
                    sprintf(strBuffer,", %s","world");
                    main.pushString(strBuffer);
                }
                main.pushString(")");
            }
        }
    }

    /* generate_exprArL generates code for expressions of arithmetic and logic type*/
    void dsl_cpp_generator::generate_exprArL(Expression* expr)
    {
        if (expr->hasEnclosedBrackets()) {
            main.pushString("(");
        }

        // generate Expression of left part of arithmetic and logic expression
        generateExpr(expr->getLeft());

        main.space();
        // generate the operator
        const char* operatorString = getOperatorString(expr->getOperatorType());
        main.pushstr_space(operatorString);

        // generate Expression of right part of arithmetic and logic expression
        generateExpr(expr->getRight());
        
        if (expr->hasEnclosedBrackets()) {
            main.pushString(")");
        }
    }

    /* generate_exprRelational generates code for expressions of Relation type (eg :- <= < )*/
    void dsl_cpp_generator::generate_exprRelational(Expression* expr)
    {
        if (expr->hasEnclosedBrackets())
            main.pushString("(");
        
        // generate Expression of left part of arithmetic and logic expression
        generateExpr(expr->getLeft());

        main.space();
        // generate the operator
        const char* operatorString = getOperatorString(expr->getOperatorType());
        main.pushstr_space(operatorString);

        // generate Expression of right part of arithmetic and logic expression
        generateExpr(expr->getRight());
        
        if (expr->hasEnclosedBrackets()) {
            main.pushString(")");
        }
    }


    /*generate_exprUnary generates code for a unary expression (not made of two expressions like relational or AL expression) */
    void dsl_cpp_generator::generate_exprUnary(Expression* expr) 
    {
        if (expr->hasEnclosedBrackets()) 
            main.pushString("(");


        // If unary expression is of type !(expression) then first generate the not operator then the expression
        if (expr->getOperatorType() == OPERATOR_NOT) 
        {
            const char* operatorString = getOperatorString(expr->getOperatorType());
            main.pushString(operatorString);
            generateExpr(expr->getUnaryExpr());
        }

        // if unary expression is of type expression++ or expression-- then generate the expression then the operator
        if (expr->getOperatorType() == OPERATOR_INC || expr->getOperatorType() == OPERATOR_DEC) 
        {
            generateExpr(expr->getUnaryExpr());
            const char* operatorString = getOperatorString(expr->getOperatorType());
            main.pushString(operatorString);
        }

        if (expr->hasEnclosedBrackets()) 
        {
            main.pushString(")");
        }
    }

    /* generate_exprLiteral generates expression for literals (i.e integer, boolean or floating point literals) */
    void dsl_cpp_generator::generate_exprLiteral(Expression* expr) {
        char valBuffer[1024];

        int expr_valType = expr->getExpressionFamily();

        // Just print the constant literal as it is
        switch (expr_valType) 
        {
            case EXPR_INTCONSTANT:
                sprintf(valBuffer, "%ld", expr->getIntegerConstant());
                break;
            case EXPR_FLOATCONSTANT:
                sprintf(valBuffer, "%lf", expr->getFloatConstant());
                break;
            case EXPR_BOOLCONSTANT:
                sprintf(valBuffer, "%s", expr->getBooleanConstant() ? "true" : "false");
                break;
            default:
            assert(false);
        }
        main.pushString(valBuffer);
    }

    /*generate_exprInfinity generates the INFINITY Constant into corresponding C type */
    void dsl_cpp_generator::generate_exprInfinity(Expression* expr)
    {
        char valBuffer[1024];
        if (expr->getTypeofExpr() != 0) 
        {
            // Generate the appropriate C constant based on the type of expression
            int typeClass = expr->getTypeofExpr();
            switch (typeClass) {
                case TYPE_INT:
                    sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
                    break;
                case TYPE_LONG:
                    sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "LLONG_MAX" : "LLONG_MIN");
                    break;
                case TYPE_FLOAT:
                    sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "FLT_MAX" : "FLT_MIN");
                    break;
                case TYPE_DOUBLE:
                    sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "DBL_MAX" : "DBL_MIN");
                    break;
                default:
                    sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
                    break;
            }

        } 
        else
        {
            sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
        }

        main.pushString(valBuffer);
    }

    /* generate_exprIdentifier generates expression of type identifier */
    void dsl_cpp_generator::generate_exprIdentifier(Identifier* id)
    {
        main.pushString(id->getIdentifier());
    }

}

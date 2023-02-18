#include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"
#include "getUsedVars.cpp"

//~ using namespace spsycl;
namespace spsycl
{

    void dsl_cpp_generator::addIncludeToFile(const char *includeName, dslCodePad &file, bool isCppLib)
    {
        if (!isCppLib)
            file.push('"');
        else
            file.push('<');

        file.pushString(includeName);
        if (!isCppLib)
            file.push('"');
        else
            file.push('>');
        file.NewLine();
    }

    void dsl_cpp_generator::generation_begin()
    {
        char temp[1024];
        main.pushString("#ifndef GENCPP_");
        main.pushUpper(fileName);
        main.pushstr_newL("_H");
        main.pushString("#define GENCPP_");
        main.pushUpper(fileName);
        main.pushstr_newL("_H");
        main.pushString("#include ");
        addIncludeToFile("CL/sycl.hpp", main, true);
        main.pushString("#include ");
        addIncludeToFile("iostream", main, true);
        main.pushString("#include ");
        addIncludeToFile("fstream", main, true);
        main.pushString("#include ");
        addIncludeToFile("../graph.hpp", main, false);
        main.pushstr_newL("using namespace sycl;");
        main.NewLine();
    }

    void dsl_cpp_generator::setCurrentFunc(Function *func)
    {
        currentFunc = func;
    }

    const char *dsl_cpp_generator::convertToCppType(Type *type)
    {
        if (type->isPrimitiveType())
        {
            int typeId = type->gettypeId();
            switch (typeId)
            {
            case TYPE_INT:
                return "int";
            case TYPE_BOOL:
                return "bool";
            case TYPE_LONG:
                return "long";
            case TYPE_FLOAT:
                return "float";
            case TYPE_DOUBLE:
                return "double";
            case TYPE_NODE:
                return "int";
            case TYPE_EDGE:
                return "int";
            default:
                assert(false);
            }
        }
        else if (type->isPropType())
        {
            Type *targetType = type->getInnerTargetType();
            if (targetType->isPrimitiveType())
            {
                int typeId = targetType->gettypeId();
                //~ cout << "TYPEID IN CPP" << typeId << "\n";
                switch (typeId)
                {
                case TYPE_INT:
                    return "int*";
                case TYPE_BOOL:
                    return "bool*";
                case TYPE_LONG:
                    return "long*";
                case TYPE_FLOAT:
                    return "float*";
                case TYPE_DOUBLE:
                    return "double*";
                default:
                    assert(false);
                }
            }
        }
        else if (type->isNodeEdgeType())
        {
            return "int"; // need to be modified.
        }
        else if (type->isGraphType())
        {
            return "graph&";
        }
        else if (type->isCollectionType())
        {
            int typeId = type->gettypeId();

            switch (typeId)
            {
            case TYPE_SETN:
                return "std::set<int>&";

            default:
                assert(false);
            }
        }

        return "NA";
    }

    void dsl_cpp_generator::generateCSRArrays(const char *gId)
    {
        char strBuffer[1024];
        // assuming DSL does not contain variables V and E
        sprintf(strBuffer, "int V = %s.num_nodes();", gId);
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer, "int E = %s.num_edges();", gId);
        main.pushstr_newL(strBuffer);
        main.NewLine();

        main.pushstr_newL("printf(\"#nodes:%d\\n\",V);");
        main.pushstr_newL("printf(\"#edges:\%d\\n\",E);");
        //~ main.pushstr_newL("printf(\"#srces:\%d\\n\",sourceSet.size()); /// TODO get from var");

        sprintf(strBuffer, "int* edgeLen = %s.getEdgeLen();",
                gId); // assuming DSL  do not contain variables as V and E
        main.pushstr_newL(strBuffer);
        main.NewLine();

        // These H & D arrays of CSR do not change. Hence hardcoded!
        main.pushstr_newL("int *h_meta;");
        main.pushstr_newL("int *h_data;");
        main.pushstr_newL("int *h_src;");
        main.pushstr_newL("int *h_weight;");
        main.pushstr_newL("int *h_rev_meta;"); // done only to handle PR since other doesn't uses it
        main.NewLine();

        main.pushstr_newL("h_meta = (int *)malloc( (V+1)*sizeof(int));");
        main.pushstr_newL("h_data = (int *)malloc( (E)*sizeof(int));");
        main.pushstr_newL("h_src = (int *)malloc( (E)*sizeof(int));");
        main.pushstr_newL("h_weight = (int *)malloc( (E)*sizeof(int));");
        main.pushstr_newL("h_rev_meta = (int *)malloc( (V+1)*sizeof(int));");
        main.NewLine();

        main.pushstr_newL("for(int i=0; i<= V; i++) {");
        sprintf(strBuffer, "int temp = %s.indexofNodes[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_meta[i] = temp;");
        sprintf(strBuffer, "temp = %s.rev_indexofNodes[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_rev_meta[i] = temp;");
        main.pushstr_newL("}");
        main.NewLine();

        main.pushstr_newL("for(int i=0; i< E; i++) {");
        sprintf(strBuffer, "int temp = %s.edgeList[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_data[i] = temp;");
        sprintf(strBuffer, "temp = %s.srcList[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_src[i] = temp;");
        main.pushstr_newL("temp = edgeLen[i];");
        main.pushstr_newL("h_weight[i] = temp;");
        main.pushstr_newL("}");
        main.NewLine();
    }

    void dsl_cpp_generator::generateFuncHeader(Function *proc)
    {
        main.pushString("void ");
        main.pushString(proc->getIdentifier()->getIdentifier());

        main.push('(');
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = proc->getParamList().size();
        list<formalParam *> paramList = proc->getParamList();
        list<formalParam *>::iterator itr;

        for (itr = paramList.begin(); itr != paramList.end(); itr++)
        {
            arg_currNo++;
            argumentTotal--;

            Type *type = (*itr)->getType();
            main.pushString(convertToCppType(type));
            char *parName = (*itr)->getIdentifier()->getIdentifier();

            main.pushString(" ");
            char str[80];
            strcpy(str, "d_");
            strcat(str, (*itr)->getIdentifier()->getIdentifier());
            cout << "param :" << parName << endl;
            cout << "paramD:" << str << endl;

            if (type->isGraphType())
            {
                std::cout << "========== SET TRUE" << '\n';
                genCSR = true;
            }

            main.pushString(parName);
            if (argumentTotal > 0)
                main.pushString(",");
            if (arg_currNo == maximum_arginline)
            {
                main.NewLine();
                arg_currNo = 0;
            }
        }

        main.pushString(")");
        main.NewLine();
        return;
    }

    void dsl_cpp_generator::generateMallocDeviceStr(const char *dVar, const char *typeStr, const char *sizeOfType)
    {
        char strBuffer[1024];
        // dVar=malloc_device<type>(V, Q);
        sprintf(strBuffer, "%s=malloc_device<%s>(%s, Q);", dVar, typeStr, sizeOfType);
        main.pushstr_newL(strBuffer);
        //~ main.NewLine();
    }

    // Only for device variables
    void dsl_cpp_generator::generateMallocDevice(Type *type, const char *identifier)
    {
        char strBuffer[1024];
        //~ Type* targetType = type->getInnerTargetType();
        // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
        //                   1             2      3
        // dVar=malloc_device<type>(V, Q);
        sprintf(strBuffer, "d_%s=malloc_device<%s>(%s, Q);", identifier,
                convertToCppType(type->getInnerTargetType()),
                (type->isPropNodeType())
                    ? "V"
                    : "E"); // this assumes PropNode type  IS PROPNODE? V : E //else
                            // might error later

        main.pushstr_newL(strBuffer);
        main.NewLine();
    }

    void dsl_cpp_generator::generateMallocDeviceParams(list<formalParam *> paramList)
    {
        list<formalParam *>::iterator itr;
        for (itr = paramList.begin(); itr != paramList.end(); itr++)
        {
            Type *type = (*itr)->getType();
            if (type->isPropType())
            {
                Type *innerType = type->getInnerTargetType();
                main.pushString(convertToCppType(innerType));
                main.pushString("*");
                main.space();
                char str[80];
                strcpy(str, "d_");
                strcat(str, (*itr)->getIdentifier()->getIdentifier());
                main.pushString(str);
                main.pushstr_newL(";");
                generateMallocDevice(type, (*itr)->getIdentifier()->getIdentifier());
            }
        }
    }

    void dsl_cpp_generator::generateMemCpyStr(const char *sVarName, const char *tVarName, const char *type, const char *sizeV)
    {
        // Q.submit([&](handler &h)
        //      { h.memcpy(d_data, h_data, sizeof(T) * N); })
        //.wait();
        char strBuffer[1024];
        sprintf(strBuffer, "Q.submit([&](handler &h)");
        main.pushstr_newL(strBuffer);

        sprintf(strBuffer, "{ h.memcpy(%8s, %8s, sizeof(%3s)*(%s)); })", sVarName, tVarName, type, sizeV);
        main.pushstr_newL(strBuffer);

        sprintf(strBuffer, ".wait();");
        main.pushstr_newL(strBuffer);
    }

    void dsl_cpp_generator::generateLaunchConfig()
    {
        // int NUM_THREADS = 1048576;
        // int stride = NUM_THREADS;

        char strBuffer[1024];
        main.NewLine();

        sprintf(strBuffer, "int NUM_THREADS = 1048576;");
        main.pushstr_newL(strBuffer);

        sprintf(strBuffer, "int stride = NUM_THREADS;");
        main.pushstr_newL(strBuffer);
    }

    void dsl_cpp_generator::generateFuncBody(Function *proc)
    {
        char strBuffer[1024];
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = proc->getParamList().size();
        list<formalParam *> paramList = proc->getParamList();
        list<formalParam *>::iterator itr;

        const char *gId;

        for (itr = paramList.begin(); itr != paramList.end(); itr++)
        {
            arg_currNo++;
            argumentTotal--;

            Type *type = (*itr)->getType();

            const char *parName = (*itr)->getIdentifier()->getIdentifier();
            cout << "param:" << parName << endl;
            if (type->isGraphType())
            {
                genCSR = true;
                gId = parName;
            }

            if (arg_currNo == maximum_arginline)
            {
                arg_currNo = 0;
            }
        }

        // sprintf(strBuffer, "default_selector d_selector;");
        // main.pushstr_newL(strBuffer);
        sprintf(strBuffer, "queue Q(default_selector_v);");
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer, "std::cout << \"Selected device: \" << Q.get_device().get_info<info::device::name>() << std::endl;");
        main.pushstr_newL(strBuffer);

        if (genCSR)
        {
            main.pushstr_newL("// CSR BEGIN");
            generateCSRArrays(gId);

            main.NewLine();
            sprintf(strBuffer, "int* d_meta;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_data;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_src;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_weight;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_rev_meta;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "bool* d_modified_next;");
            main.pushstr_newL(strBuffer);
            main.NewLine();

            generateMallocDeviceStr("d_meta", "int", "(1+V)");
            generateMallocDeviceStr("d_data", "int", "(E)");
            generateMallocDeviceStr("d_src", "int", "(E)");
            generateMallocDeviceStr("d_weight", "int", "(E)");
            generateMallocDeviceStr("d_rev_meta", "int", "(V+1)");
            generateMallocDeviceStr("d_modified_next", "bool", "(V)");

            main.NewLine();

            generateMemCpyStr("d_meta", "h_meta", "int", "V+1");
            generateMemCpyStr("d_data", "h_data", "int", "E");
            generateMemCpyStr("d_src", "h_src", "int", "E");
            generateMemCpyStr("d_weight", "h_weight", "int", "E");
            generateMemCpyStr("d_rev_meta", "h_rev_meta", "int", "(V+1)");
            main.NewLine();

            main.pushstr_newL("// CSR END");

            main.pushString("//LAUNCH CONFIG");

            generateLaunchConfig();

            main.NewLine();

            main.pushstr_newL("// TODO: TIMER START");
        }
    }

    void dsl_cpp_generator::castIfRequired(Type *type, Identifier *methodID, dslCodePad &main)
    {
        /* This needs to be made generalized, extended for all predefined function,
            made available by the DSL*/
        string predefinedFunc("num_nodes");
        if (predefinedFunc.compare(methodID->getIdentifier()) == 0)
        {
            if (type->gettypeId() != TYPE_INT)
            {
                char strBuffer[1024];
                sprintf(strBuffer, "(%s)", convertToCppType(type));
                main.pushString(strBuffer);
            }
        }
    }

    void dsl_cpp_generator::generate_exprLiteral(Expression *expr, bool isMainFile)
    {
        char valBuffer[1024];

        int expr_valType = expr->getExpressionFamily();

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

    void dsl_cpp_generator::generate_exprInfinity(Expression *expr, bool isMainFile)
    {
        char valBuffer[1024];
        if (expr->getTypeofExpr())
        {
            int typeClass = expr->getTypeofExpr();
            switch (typeClass)
            {
            case TYPE_INT:
                sprintf(valBuffer, "%s",
                        expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
                break;
            case TYPE_LONG:
                sprintf(valBuffer, "%s",
                        expr->isPositiveInfinity() ? "LLONG_MAX" : "LLONG_MIN");
                break;
            case TYPE_FLOAT:
                sprintf(valBuffer, "%s",
                        expr->isPositiveInfinity() ? "FLT_MAX" : "FLT_MIN");
                break;
            case TYPE_DOUBLE:
                sprintf(valBuffer, "%s",
                        expr->isPositiveInfinity() ? "DBL_MAX" : "DBL_MIN");
                break;
            default:
                sprintf(valBuffer, "%s",
                        expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
                break;
            }
        }
        else
        {
            sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
        }
        main.pushString(valBuffer);
    }

    void dsl_cpp_generator::generate_exprPropId(PropAccess *propId, bool isMainFile)
    {
        char strBuffer[1024];
        Identifier *id1 = propId->getIdentifier1();
        Identifier *id2 = propId->getIdentifier2();
        ASTNode *propParent = propId->getParent();
        bool relatedToReduction =
            propParent != NULL ? propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT
                               : false;
        if (id2->getSymbolInfo() != NULL &&
            id2->getSymbolInfo()->getId()->get_fp_association() &&
            relatedToReduction)
        {
            sprintf(strBuffer, "d_%s_next[%s]", id2->getIdentifier(),
                    id1->getIdentifier());
        }
        else
        {
            if (!isMainFile)
                sprintf(strBuffer, "d_%s[%s]", id2->getIdentifier(), id1->getIdentifier()); // PREFIX D
            else
                sprintf(strBuffer, "%s[%s]", id2->getIdentifier(), id1->getIdentifier());
        }
        main.pushString(strBuffer);
    }

    void dsl_cpp_generator::generate_exprArL(Expression *expr, bool isMainFile, bool isAtomic)
    {
        if (expr->hasEnclosedBrackets())
        {
            main.pushString("(");
        }
        if (!isAtomic)
            generateExpr(expr->getLeft(), isMainFile);
        main.space();
        const char *operatorString = getOperatorString(expr->getOperatorType());
        if (!isAtomic)
            main.pushstr_space(operatorString);
        generateExpr(expr->getRight(), isMainFile);
        if (expr->hasEnclosedBrackets())
        {
            main.pushString(")");
        }
    }

    void dsl_cpp_generator::generate_exprRelational(Expression *expr, bool isMainFile)
    {
        if (expr->hasEnclosedBrackets())
        {
            main.pushString("(");
        }
        generateExpr(expr->getLeft(), isMainFile);

        main.space();
        const char *operatorString = getOperatorString(expr->getOperatorType());
        main.pushstr_space(operatorString);
        generateExpr(expr->getRight(), isMainFile);
        if (expr->hasEnclosedBrackets())
        {
            main.pushString(")");
        }
    }

    void dsl_cpp_generator::generate_exprProcCall(Expression *expr, bool isMainFile)
    {
        proc_callExpr *proc = (proc_callExpr *)expr;
        string methodId(proc->getMethodId()->getIdentifier());
        if (methodId == "get_edge")
        {
            main.pushString("edge");
        }
        else if (methodId == "count_outNbrs")
        {
            char strBuffer[1024];
            list<argument *> argList = proc->getArgList();
            assert(argList.size() == 1);
            Identifier *nodeId = argList.front()->getExpr()->getId();
            //~ Identifier* objectId = proc->getId1();
            sprintf(strBuffer, "(%s[%s+1]-%s[%s])", "d_meta", nodeId->getIdentifier(), "d_meta", nodeId->getIdentifier());
            main.pushString(strBuffer);
        }
        else if (methodId == "is_an_edge")
        {
            char strBuffer[1024];
            list<argument *> argList = proc->getArgList();
            assert(argList.size() == 2);
            Identifier *srcId = argList.front()->getExpr()->getId();
            Identifier *destId = argList.back()->getExpr()->getId();
            //~ Identifier* objectId = proc->getId1();
            sprintf(strBuffer, "%s(%s, %s, %s, %s)", "findNeighborSorted", srcId->getIdentifier(), destId->getIdentifier(), "d_meta", "d_data");
            main.pushString(strBuffer);
        }
        else
        {
            char strBuffer[1024];
            list<argument *> argList = proc->getArgList();
            if (argList.size() == 0)
            {
                Identifier *objectId = proc->getId1();
                sprintf(strBuffer, "%s.%s( )", objectId->getIdentifier(), proc->getMethodId()->getIdentifier());
                main.pushString(strBuffer);
            }
        }
    }

    void dsl_cpp_generator::generate_exprUnary(Expression *expr, bool isMainFile)
    {
        if (expr->hasEnclosedBrackets())
        {
            main.pushString("(");
        }

        if (expr->getOperatorType() == OPERATOR_NOT)
        {
            const char *operatorString = getOperatorString(expr->getOperatorType());
            main.pushString(operatorString);
            generateExpr(expr->getUnaryExpr(), isMainFile);
        }

        if (expr->getOperatorType() == OPERATOR_INC ||
            expr->getOperatorType() == OPERATOR_DEC)
        {
            generateExpr(expr->getUnaryExpr(), isMainFile);
            const char *operatorString = getOperatorString(expr->getOperatorType());
            main.pushString(operatorString);
        }

        if (expr->hasEnclosedBrackets())
        {
            main.pushString(")");
        }
    }

    void dsl_cpp_generator::generate_exprIdentifier(Identifier *id, bool isMainFile)
    {
        main.pushString(id->getIdentifier());
    }

    void dsl_cpp_generator::generateExpr(Expression *expr, bool isMainFile, bool isAtomic)
    {
        if (expr->isLiteral())
        {
            generate_exprLiteral(expr, isMainFile);
        }
        else if (expr->isInfinity())
        {
            generate_exprInfinity(expr, isMainFile);
        }
        else if (expr->isIdentifierExpr())
        {
            generate_exprIdentifier(expr->getId(), isMainFile);
        }
        else if (expr->isPropIdExpr())
        {
            generate_exprPropId(expr->getPropId(), isMainFile);
        }
        else if (expr->isArithmetic() || expr->isLogical())
        {
            generate_exprArL(expr, isMainFile, isAtomic);
        }
        else if (expr->isRelational())
        {
            generate_exprRelational(expr, isMainFile);
        }
        else if (expr->isProcCallExpr())
        {
            generate_exprProcCall(expr, isMainFile);
        }
        else if (expr->isUnary())
        {
            generate_exprUnary(expr, isMainFile);
        }
        else
        {
            assert(false);
        }
    }

    void dsl_cpp_generator::generateVariableDecl(declaration *declStmt, bool isMainFile)
    {
        Type *type = declStmt->getType();

        if (type->isPropType())
        {
            if (type->getInnerTargetType()->isPrimitiveType())
            {
                Type *innerType = type->getInnerTargetType();
                main.pushString(convertToCppType(innerType)); // convertToCppType need to be modified.
                main.pushString("*");
                main.space();
                main.pushString("d_");
                main.pushString(declStmt->getdeclId()->getIdentifier());
                main.pushstr_newL(";");
                generateMallocDevice(type, declStmt->getdeclId()->getIdentifier());
            }
        }
        else if (type->isPrimitiveType())
        {
            char strBuffer[1024];
            const char *varType = convertToCppType(type);
            const char *varName = declStmt->getdeclId()->getIdentifier();
            cout << "varT:" << varType << endl;
            cout << "varN:" << varName << endl;

            sprintf(strBuffer, "%s %s", varType, varName);
            main.pushString(strBuffer);

            if (declStmt->isInitialized())
            {
                main.pushString(" = ");
                if (declStmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL)
                {
                    proc_callExpr *pExpr = (proc_callExpr *)declStmt->getExpressionAssigned();
                    Identifier *methodId = pExpr->getMethodId();
                    castIfRequired(type, methodId, main);
                }
                generateExpr(declStmt->getExpressionAssigned(), isMainFile); // PRINTS RHS? YES
            }
            main.pushstr_newL("; // asst in main");
            main.NewLine();
        }
        else if (type->isNodeEdgeType())
        {
            main.pushstr_space(convertToCppType(type));
            main.pushString(declStmt->getdeclId()->getIdentifier());
            if (declStmt->isInitialized())
            {
                main.pushString(" = ");
                generateExpr(declStmt->getExpressionAssigned(), isMainFile);
                main.pushstr_newL(";");
            }
        }
    }

    void dsl_cpp_generator::generateDeviceAssignmentStmt(assignment *asmt, bool isMainFile)
    {
        main.pushstr_newL("// Device assignment statement");
    }

    void dsl_cpp_generator::generateAtomicDeviceAssignmentStmt(assignment *asmt, bool isMainFile)
    {
        main.pushstr_newL("// Atomic device assignment statement");
    }

    void dsl_cpp_generator::generateIfStmt(ifStmt *ifstmt, bool isMainFile)
    {
        Expression *condition = ifstmt->getCondition();
        main.pushString("if (");
        std::cout << "=======IF FILTER" << '\n';
        generateExpr(condition, isMainFile);
        main.pushstr_newL("){ // if filter begin ");

        generateStatement(ifstmt->getIfBody(), isMainFile);
        main.pushstr_newL("} // if filter end");
        if (ifstmt->getElseBody() == NULL)
            return;
        main.pushstr_newL("else");
        generateStatement(ifstmt->getElseBody(), isMainFile);
    }

    void dsl_cpp_generator::generateDoWhileStmt(dowhileStmt *doWhile, bool isMainFile)
    {
        main.pushstr_newL("// Generate do while statement");
    }

    void dsl_cpp_generator::generateCudaMemCpySymbol(char *var, const char *typeStr, bool direction)
    {
        char strBuffer[1024];
        // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
        //                   1             2      3
        if (direction)
        {
            sprintf(strBuffer, "%s *dev_%s = malloc_device<%s>(1, Q);", typeStr, var, typeStr);
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("Q.submit([&](handler &h)");
            sprintf(strBuffer, "{ h.memcpy(dev_%s, &%s, 1 * sizeof(%s)); })", var, var, typeStr);
            main.pushstr_newL(strBuffer);
            main.pushstr_newL(".wait();");
            main.NewLine();
        }
        else
        {
            main.pushstr_newL("Q.submit([&](handler &h)");
            sprintf(strBuffer, "{ h.memcpy(&%s, dev_%s, 1 * sizeof(%s)); })", var, var, typeStr);
            main.pushstr_newL(strBuffer);
            main.pushstr_newL(".wait();");
            main.NewLine();
        }
    }

    blockStatement *dsl_cpp_generator::includeIfToBlock(forallStmt *forAll)
    {
        Expression *filterExpr = forAll->getfilterExpr();
        Expression *modifiedFilterExpr = filterExpr;
        if (filterExpr->getExpressionFamily() == EXPR_RELATIONAL)
        {
            Expression *expr1 = filterExpr->getLeft();
            Expression *expr2 = filterExpr->getRight();
            if (expr1->isIdentifierExpr())
            {
                /*if it is a nodeproperty, the filter is on the nodes that are iterated on
                One more check can be applied to check if the iterating type is a neigbor
                iteration or allgraph iterations.
                */
                if (expr1->getId()->getSymbolInfo() != NULL)
                {
                    if (expr1->getId()->getSymbolInfo()->getType()->isPropNodeType())
                    {
                        Identifier *iterNode = forAll->getIterator();
                        Identifier *nodeProp = expr1->getId();
                        PropAccess *propIdNode = (PropAccess *)Util::createPropIdNode(iterNode, nodeProp);
                        Expression *propIdExpr = Expression::nodeForPropAccess(propIdNode);
                        modifiedFilterExpr = (Expression *)Util::createNodeForRelationalExpr(propIdExpr, expr2, filterExpr->getOperatorType());
                    }
                }
            }
        }

        ifStmt *ifNode = (ifStmt *)Util::createNodeForIfStmt(modifiedFilterExpr, forAll->getBody(), NULL);
        blockStatement *newBlock = new blockStatement();
        newBlock->setTypeofNode(NODE_BLOCKSTMT);
        newBlock->addStmtToBlock(ifNode);
        return newBlock;
    }

    void dsl_cpp_generator::addCudaKernel(forallStmt *forAll)
    {
        const char *loopVar = "v";
        char strBuffer[1024];

        usedVariables usedVars = getVariablesForAll(forAll);
        list<Identifier *> vars = usedVars.getVariables();

        sprintf(strBuffer, "Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS)");
        main.pushString(strBuffer);
        main.pushstr_newL("{ // BEGIN KER FUN via ADDKERNEL");

        if (forAll->hasFilterExpr())
        {
            blockStatement *changedBody = includeIfToBlock(forAll);
            cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
            forAll->setBody(changedBody);
            // cout<<"FORALL BODY
            // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
        }

        statement *body = forAll->getBody();
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        blockStatement *block = (blockStatement *)body;
        list<statement *> statementList = block->returnStatements();

        printf("start of kernel block \n");

        for (statement *stmt : statementList)
        {
            generateStatement(stmt, false); // false. All these stmts should be inside kernel
                                            //~ if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
        }
        main.pushstr_newL("}");
        main.pushstr_newL("}); }).wait(); // end KER FUNC");
        main.NewLine();
    }

    bool dsl_cpp_generator::allGraphIteration(char *methodId)
    {
        string methodString(methodId);

        return (methodString == "nodes" || methodString == "edges");
    }

    bool dsl_cpp_generator::neighbourIteration(char *methodId)
    {
        string methodString(methodId);
        return (methodString == "neighbors" || methodString == "nodes_to");
    }

    bool dsl_cpp_generator::elementsIteration(char *extractId)
    {
        string extractString(extractId);
        return (extractString == "elements");
    }

    void dsl_cpp_generator::generateForAllSignature(forallStmt *forAll, bool isMainFile)
    {
        cout << "GenerateForAllSignature = " << isMainFile << endl;

        char strBuffer[1024];
        Identifier *iterator = forAll->getIterator();

        if (forAll->isSourceProcCall())
        {
            proc_callExpr *extractElemFunc = forAll->getExtractElementFunc();
            Identifier *iteratorMethodId = extractElemFunc->getMethodId();
            if (allGraphIteration(iteratorMethodId->getIdentifier()))
            {
            }
            else if (neighbourIteration(iteratorMethodId->getIdentifier()))
            {
                char *methodId = iteratorMethodId->getIdentifier();
                string s(methodId);
                if (s.compare("neighbors") == 0)
                {
                    list<argument *> argList = extractElemFunc->getArgList();
                    assert(argList.size() == 1);
                    sprintf(strBuffer, "for (%s %s = %s[%s]; %s < %s[%s+1]; %s++) { // FOR NBR ITR ", "int", "edge", "d_meta", "v", "edge", "d_meta", "v", "edge");
                    main.pushstr_newL(strBuffer);
                    sprintf(strBuffer, "%s %s = %s[%s];", "int", iterator->getIdentifier(), "d_data", "edge"); // needs to move the addition of
                    main.pushstr_newL(strBuffer);
                }
                if (s.compare("nodes_to") == 0) // for pageRank
                {
                    main.pushstr_newL("// Nodes to neighbour iteration");
                }
            }
            else if (forAll->isSourceField())
            {
            }
            else
            {
                main.pushstr_newL("// For signature of set");
            }
        }
    }

    void dsl_cpp_generator::generateForAll(forallStmt *forAll, bool isMainFile)
    {
        main.pushstr_newL("// Generate for all statement");
        proc_callExpr *extractElemFunc = forAll->getExtractElementFunc();
        PropAccess *sourceField = forAll->getPropSource();
        Identifier *sourceId = forAll->getSource();

        Identifier *collectionId;
        if (sourceField != NULL)
        {
            collectionId = sourceField->getIdentifier1();
        }
        if (sourceId != NULL)
        {
            collectionId = sourceId;
        }
        Identifier *iteratorMethodId;
        if (extractElemFunc != NULL)
            iteratorMethodId = extractElemFunc->getMethodId();
        statement *body = forAll->getBody();
        char strBuffer[1024];

        if (forAll->isForall())
        {
            std::cout << "Entered for all" << std::endl;
            if (!isOptimized)
            {
                std::cout << "============EARLIER NOT OPT=============" << '\n';
                usedVariables usedVars = getVariablesForAll(forAll);
                list<Identifier *> vars = usedVars.getVariables();

                for (Identifier *iden : vars)
                {
                    std::cout << "varName:" << iden->getIdentifier() << '\n';
                    Type *type = iden->getSymbolInfo()->getType();

                    if (type->isPrimitiveType())
                        generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
                    /*else if(type->isPropType())
                    {
                        Type* innerType = type->getInnerTargetType();
                        string dIden = "d_" + string(iden->getIdentifier());
                        generateCudaMemCpyStr(dIden.c_str(), iden->getIdentifier(), convertToCppType(innerType), "V", true);
                    }*/
                }
            }

            addCudaKernel(forAll);

            if (!isOptimized)
            {
                usedVariables usedVars = getVariablesForAll(forAll);
                list<Identifier *> vars = usedVars.getVariables();
                for (Identifier *iden : vars)
                {
                    Type *type = iden->getSymbolInfo()->getType();
                    if (type->isPrimitiveType())
                        generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), false);
                    /*else if(type->isPropType())
                    {
                        Type* innerType = type->getInnerTargetType();
                        string dIden = "d_" + string(iden->getIdentifier());
                        generateCudaMemCpyStr(iden->getIdentifier(),dIden.c_str(), convertToCppType(innerType), "V", false);
                    }*/
                }
                /*memcpy from symbol*/
            }
        }
        else
        { // IS FOR
            main.pushstr_newL("// for all else part");

            generateForAllSignature(forAll, false); // FOR LINE

            if (forAll->hasFilterExpr())
            {
                blockStatement *changedBody = includeIfToBlock(forAll);
                cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
                forAll->setBody(changedBody);
            }

            if (extractElemFunc != NULL)
            {
                if (neighbourIteration(iteratorMethodId->getIdentifier()))
                {
                    cout << "\t ITERATE Neighbour \n";
                    char *wItr = forAll->getIterator()->getIdentifier();
                    std::cout << "src:" << wItr << '\n';
                    char *nbrVar;

                    if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
                    {
                        list<argument *> argList = extractElemFunc->getArgList();
                        assert(argList.size() == 1);
                        Identifier *nodeNbr = argList.front()->getExpr()->getId();
                        nbrVar = nodeNbr->getIdentifier();

                        sprintf(strBuffer, "if(d_level[%s] == -1) {", wItr);
                        main.pushstr_newL(strBuffer);
                        sprintf(strBuffer, "d_level[%s] = *d_hops_from_source + 1;", wItr);

                        main.pushstr_newL(strBuffer);
                        main.pushstr_newL("*d_finished = false;");
                        main.pushstr_newL("}");

                        sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source + 1) {", wItr);
                        main.pushstr_newL(strBuffer);

                        generateBlock((blockStatement *)forAll->getBody(), false, false);

                        main.pushstr_newL("}");

                        main.pushstr_newL("}");
                    }
                    else if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
                    { // ITERATE REV BFS
                        main.pushstr_newL("// rev bfs iterate");
                    }
                    else
                    {
                        generateStatement(forAll->getBody(), isMainFile);
                        main.pushstr_newL("} //  end FOR NBR ITR. TMP FIX!");
                        std::cout << "FOR BODY END" << '\n';
                    }
                }
                else
                {
                    printf("FOR NORML");
                    generateStatement(forAll->getBody(), false);
                }
            }
            else
            {
                if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN)
                { // FOR SET
                    // if (body->getTypeofNode() == NODE_BLOCKSTMT) {
                    // targetFile.pushstr_newL("{");  // uncomment after fixing NBR FOR brackets } issues.
                    // //~ targetFile.pushstr_newL("//HERE");
                    // printf("FOR");
                    // sprintf(strBuffer, "int %s = *itr;", forAll->getIterator()->getIdentifier());
                    // targetFile.pushstr_newL(strBuffer);
                    // generateBlock((blockStatement*)body, false);  //FOR BODY for
                    // targetFile.pushstr_newL("}");
                    // } else
                    // generateStatement(forAll->getBody(), false);
                    main.pushstr_newL("set todo");
                }
                else
                {
                    cout << iteratorMethodId->getIdentifier() << "\n";
                    generateStatement(forAll->getBody(), false);
                }
                if (forAll->isForall() && forAll->hasFilterExpr())
                {
                    Expression *filterExpr = forAll->getfilterExpr();
                    generatefixedpt_filter(filterExpr, false);
                }
            }
        }
    }

    void dsl_cpp_generator::generatefixedpt_filter(Expression *filterExpr, bool isMainFile)
    {
        Expression *lhs = filterExpr->getLeft();
        //~ char strBuffer[1024];
        if (lhs->isIdentifierExpr())
        {
            //~ Identifier* filterId = lhs->getId();
            //~ TableEntry* tableEntry = filterId->getSymbolInfo();
            /*
            if(tableEntry->getId()->get_fp_association())
            {
                targetFile.pushstr_newL("#pragma omp for");
                sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
            ","int","v","v",graphId[0]->getIdentifier(),"num_nodes","v");
                targetFile.pushstr_newL(strBuffer);
                targetFile.pushstr_space("{");
                sprintf(strBuffer,"%s[%s] =  %s_nxt[%s]
            ;",filterId->getIdentifier(),"v",filterId->getIdentifier(),"v");
                targetFile.pushstr_newL(strBuffer);
                Expression* initializer =
            filterId->getSymbolInfo()->getId()->get_assignedExpr();
                assert(initializer->isBooleanLiteral());
                sprintf(strBuffer,"%s_nxt[%s] = %s
            ;",filterId->getIdentifier(),"v",initializer->getBooleanConstant()?"true":"false");
                targetFile.pushstr_newL(strBuffer);
                targetFile.pushstr_newL("}");
                targetFile.pushstr_newL("}");

            }
            */
        }
    }

    void dsl_cpp_generator::generateFixedPoint(fixedPointStmt *fixedPointConstruct, bool isMainFile)
    {
        main.pushstr_newL("// Generate fixed point statement");
    }

    void dsl_cpp_generator::generateReductionCallStmt(reductionCallStmt *stmt, bool isMainFile)
    {
        // reductionCall* reduceCall = stmt->getReducCall();
        // char strBuffer[1024];

        // if (reduceCall->getReductionType() == REDUCE_MIN)
        // {
        //     if (stmt->isListInvolved())
        //     {

        //     }
        // }
        main.pushstr_newL("// Generate reduction call statement");
    }

    void dsl_cpp_generator::generateReductionOpStmt(reductionCallStmt *stmt, bool isMainFile)
    {
        char strBuffer[1024];
        if (stmt->isLeftIdentifier())
        {
            Identifier *id = stmt->getLeftId();
            const char *typVar = convertToCppType(id->getSymbolInfo()->getType());
            sprintf(strBuffer, "atomic_ref<%s, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(dev_%s[0])", typVar, id->getIdentifier());
            main.pushString(strBuffer);
            main.pushstr_newL(";");
            sprintf(strBuffer, "atomic_data += ");
            main.pushString(strBuffer);
            generateExpr(stmt->getRightSide(), isMainFile);
            main.pushstr_newL(";");
        }
        else
        {
            generate_exprPropId(stmt->getPropAccess(), isMainFile);
            main.pushString(" = ");
            generate_exprPropId(stmt->getPropAccess(), isMainFile);
            const char *operatorString = getOperatorString(stmt->reduction_op());
            main.pushstr_space(operatorString);
            generateExpr(stmt->getRightSide(), isMainFile);
            main.pushstr_newL(";");
        }
    }

    void dsl_cpp_generator::generateReductionStmt(reductionCallStmt *stmt, bool isMainFile)
    {
        main.pushstr_newL("// Generate reduction statement");
        if (stmt->is_reducCall())
        {
            generateReductionCallStmt(stmt, isMainFile);
        }
        else
        {
            generateReductionOpStmt(stmt, isMainFile);
        }
    }

    void dsl_cpp_generator::generateBFSAbstraction(iterateBFS *bfsAbstraction, bool isMainFile)
    {
        main.pushstr_newL("// Generate bfs abstraction statement");
    }

    void dsl_cpp_generator::generateInitkernel1(assignment *assign, bool isMainFile)
    { // const char* typ,
        //~ initKernel<double> <<<numBlocks,numThreads>>>(V,d_BC, 0.0);
        char strBuffer[1024];

        Identifier *inId = assign->getId();
        Expression *exprAssigned = assign->getExpr();

        const char *inVarType =
            convertToCppType(inId->getSymbolInfo()->getType()->getInnerTargetType());
        const char *inVarName = inId->getIdentifier();

        sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V,d_%s,(%s)",
                inVarType, inVarName, inVarType);
        main.pushString(strBuffer);

        std::cout << "varName:" << inVarName << '\n';
        generateExpr(exprAssigned, isMainFile); // asssuming int/float const literal // OUTPUTS INIT VALUE

        main.pushstr_newL(");");
        main.NewLine();
    }

    void dsl_cpp_generator::generateProcCall(proc_callStmt *proc_callStmt, bool isMainFile)
    {
        proc_callExpr *procedure = proc_callStmt->getProcCallExpr();
        string methodID(procedure->getMethodId()->getIdentifier());
        string IDCoded("attachNodeProperty");
        int x = methodID.compare(IDCoded);

        if (x == 0)
        {
            //~ char strBuffer[1024];
            list<argument *> argList = procedure->getArgList();
            list<argument *>::iterator itr;

            for (itr = argList.begin(); itr != argList.end(); itr++)
            {
                assignment *assign = (*itr)->getAssignExpr();

                if (argList.size() == 1)
                {
                    generateInitkernel1(assign, isMainFile);
                    //~ std::cout << "%%%%%%%%%%" << '\n';

                    /// initKernel<double> <<<numBlocks,threadsPerBlock>>>(V,d_BC, 0);
                }
                else if (argList.size() == 2)
                {
                    //~ std::cout << "===============" << '\n';
                    generateInitkernel1(assign, isMainFile);
                    //~ std::cout<< "initType:" <<
                    // convertToCppType(lhsId->getSymbolInfo()->getType()) << '\n'; ~
                    // std::cout<< "===============" << '\n'; ~
                    // generateInitkernel1(lhsId,"0"); //TODO
                }
            }
        }
    }

    void dsl_cpp_generator::generateTransferStmt(varTransferStmt *stmt)
    {
        main.pushstr_newL("// Generate transfer statement");
    }

    void dsl_cpp_generator::generateStatement(statement *stmt, bool isMainFile)
    {
        if (stmt->getTypeofNode() == NODE_BLOCKSTMT)
        {
            generateBlock((blockStatement *)stmt, false, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_DECL)
        {
            generateVariableDecl((declaration *)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_ASSIGN)
        {
            // generateAssignmentStmt((assignment*)stmt);
            assignment *asst = (assignment *)stmt;
            if (asst->isDeviceAssignment())
                generateDeviceAssignmentStmt(asst, isMainFile);
            else // atomic or normal asmt
                generateAtomicDeviceAssignmentStmt(asst, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_WHILESTMT)
        {
            // generateWhileStmt((whileStmt*) stmt);
        }

        if (stmt->getTypeofNode() == NODE_IFSTMT)
        {
            generateIfStmt((ifStmt *)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_DOWHILESTMT)
        {
            generateDoWhileStmt((dowhileStmt *)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_FORALLSTMT)
        {
            std::cout << "STMT: For" << '\n';
            printf("isMainFile val %d\n", isMainFile);
            generateForAll((forallStmt *)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT)
        {
            generateFixedPoint((fixedPointStmt *)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
        {
            generateReductionStmt((reductionCallStmt *)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_ITRBFS)
        {
            generateBFSAbstraction((iterateBFS *)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_PROCCALLSTMT)
        {
            generateProcCall((proc_callStmt *)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_UNARYSTMT)
        {
            unary_stmt *unaryStmt = (unary_stmt *)stmt;
            generateExpr(unaryStmt->getUnaryExpr(), isMainFile);
            main.pushstr_newL(";");
        }
        if (isOptimized && (stmt->getTypeofNode() == NODE_TRANSFERSTMT))
        {
            varTransferStmt *transferStmt = (varTransferStmt *)stmt;
            generateTransferStmt(transferStmt);
        }
    }

    void dsl_cpp_generator::generateBlock(blockStatement *blockStmt, bool includeBrace, bool isMainFile)
    {
        usedVariables usedVars = getDeclaredPropertyVariablesOfBlock(blockStmt);
        list<Identifier *> vars = usedVars.getVariables();
        std::cout << "\t==VARSIZE:" << vars.size() << '\n';

        list<statement *> stmtList = blockStmt->returnStatements();
        list<statement *>::iterator itr;
        if (includeBrace)
        {
            main.pushstr_newL("{");
        }

        for (itr = stmtList.begin(); itr != stmtList.end(); itr++)
        {
            statement *stmt = *itr;
            generateStatement(stmt, isMainFile);
        }

        // FREE memory
        char strBuffer[1024];

        if (vars.size() > 0)
        {
            main.NewLine();
            main.pushstr_newL("//cudaFree up!! all propVars in this BLOCK!");
        }
        for (Identifier *iden : vars)
        {
            sprintf(strBuffer, "cudaFree(d_%s);", iden->getIdentifier());
            main.pushstr_newL(strBuffer);
        }
        main.NewLine();

        if (includeBrace)
        {
            main.pushstr_newL("}");
        }
    }

    void dsl_cpp_generator::generateCudaMemCpyParams(list<formalParam *> paramList)
    {
        list<formalParam *>::iterator itr;
        for (itr = paramList.begin(); itr != paramList.end(); itr++)
        {
            Type *type = (*itr)->getType();
            if (type->isPropType())
            {
                if (type->getInnerTargetType()->isPrimitiveType())
                {
                    const char *sizeofProp = NULL;
                    if (type->isPropEdgeType())
                        sizeofProp = "E";
                    else
                        sizeofProp = "V";
                    const char *temp = "d_";
                    char *temp1 = (*itr)->getIdentifier()->getIdentifier();
                    char *temp2 = (char *)malloc(1 + strlen(temp) + strlen(temp1));
                    strcpy(temp2, temp);
                    strcat(temp2, temp1);

                    generateMemCpyStr((*itr)->getIdentifier()->getIdentifier(), temp2, convertToCppType(type->getInnerTargetType()), sizeofProp);
                }
            }
        }
    }

    void dsl_cpp_generator::incFuncCount(int funcType)
    {
        if (funcType == GEN_FUNC)
            genFuncCount++;
        else if (funcType == STATIC_FUNC)
            staticFuncCount++;
        else if (funcType == INCREMENTAL_FUNC)
            inFuncCount++;
        else
            decFuncCount++;
    }

    void dsl_cpp_generator::generateFunc(ASTNode *proc)
    {
        Function *func = (Function *)proc;
        generateFuncHeader(func);

        main.pushstr_newL("{");
        generateFuncBody(func);

        main.NewLine();

        main.pushstr_newL("//DECLAR DEVICE AND HOST vars in params");
        /* function for generation of cudamalloc for property type params*/
        generateMallocDeviceParams(func->getParamList());

        main.NewLine();

        main.pushstr_newL("//BEGIN DSL PARSING ");

        generateBlock(func->getBlockStatement(), false);

        generateCudaMemCpyParams(func->getParamList());
        //~ sprintf(strBuffer, "cudaMemcpy(%s,d_%s , sizeof(%s) * (V), cudaMemcpyDeviceToHost);", outVarName, outVarName, outVarType);

        //~ main.pushstr_newL(strBuffer);

        main.pushstr_newL("} //end FUN");

        incFuncCount(func->getFuncType());

        return;
    }

    void dsl_cpp_generator::generation_end()
    {
        main.NewLine();
        main.pushstr_newL("#endif");
    }

    bool dsl_cpp_generator::openFileforOutput()
    {
        char temp[1024];
        printf("fileName %s\n", fileName);
        sprintf(temp, "%s/%s.cpp", "../graphcode/generated_sycl", fileName);
        bodyFile = fopen(temp, "w");
        if (bodyFile == NULL)
            return false;
        main.setOutputFile(bodyFile);
        return true;
    }

    void dsl_cpp_generator::closeOutputFile()
    {

        if (bodyFile != NULL)
        {
            main.outputToFile();
            fclose(bodyFile);
        }

        bodyFile = NULL;
    }

    const char *dsl_cpp_generator::getOperatorString(int operatorId)
    {
        switch (operatorId)
        {
        case OPERATOR_ADD:
            return "+";
        case OPERATOR_DIV:
            return "/";
        case OPERATOR_MUL:
            return "*";
        case OPERATOR_MOD:
            return "%";
        case OPERATOR_SUB:
            return "-";
        case OPERATOR_EQ:
            return "==";
        case OPERATOR_NE:
            return "!=";
        case OPERATOR_LT:
            return "<";
        case OPERATOR_LE:
            return "<=";
        case OPERATOR_GT:
            return ">";
        case OPERATOR_GE:
            return ">=";
        case OPERATOR_AND:
            return "&&";
        case OPERATOR_OR:
            return "||";
        case OPERATOR_INC:
            return "++";
        case OPERATOR_DEC:
            return "--";
        case OPERATOR_ADDASSIGN:
            return "+";
        case OPERATOR_ANDASSIGN:
            return "&&";
        case OPERATOR_ORASSIGN:
            return "||";
        case OPERATOR_MULASSIGN:
            return "*";
        case OPERATOR_SUBASSIGN:
            return "-";
        default:
            return "NA";
        }
    }

    bool dsl_cpp_generator::generate()
    {
        cout << "INSIDE SYCL GENERATOR" << endl;
        cout << "FRONTEND VALUES" << frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size() << endl;
        // //openFileforOutput();
        if (!openFileforOutput())
            return false;
        generation_begin();

        list<Function *> funcList = frontEndContext.getFuncList();
        for (Function *func : funcList)
        {
            setCurrentFunc(func);
            generateFunc(func);
        }

        generation_end();

        closeOutputFile();

        return true;
    }

    void dsl_cpp_generator::setFileName(char *f)
    {
        char *token = strtok(f, "/");
        char *prevtoken;

        while (token != NULL)
        {
            prevtoken = token;
            token = strtok(NULL, "/");
        }
        fileName = prevtoken;
    }
}
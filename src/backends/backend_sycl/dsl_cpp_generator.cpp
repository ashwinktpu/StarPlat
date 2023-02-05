#include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"
#include "getUsedVars.cpp"

//~ using namespace spsycl;
namespace spsycl{

    void dsl_cpp_generator::addIncludeToFile(const char* includeName, dslCodePad& file, bool isCppLib) 
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

    void dsl_cpp_generator::generation_begin() {
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
        main.pushstr_newL("using namespace sycl");
        main.NewLine();
    }

    void dsl_cpp_generator::setCurrentFunc(Function* func) 
    {
        currentFunc = func;
    }

    const char* dsl_cpp_generator::convertToCppType(Type* type) 
    {
        if (type->isPrimitiveType()) {
            int typeId = type->gettypeId();
            switch (typeId) {
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
        } else if (type->isPropType()) {
            Type* targetType = type->getInnerTargetType();
            if (targetType->isPrimitiveType()) {
            int typeId = targetType->gettypeId();
            //~ cout << "TYPEID IN CPP" << typeId << "\n";
            switch (typeId) {
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
        } else if (type->isNodeEdgeType()) {
            return "int";  // need to be modified.

        } else if (type->isGraphType()) {
            return "graph&";
        } else if (type->isCollectionType()) {
            int typeId = type->gettypeId();

            switch (typeId) {
            case TYPE_SETN:
                return "std::set<int>&";

            default:
                assert(false);
            }
        }

        return "NA";
    }

    void dsl_cpp_generator::generateCSRArrays(const char* gId) 
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
                gId);  // assuming DSL  do not contain variables as V and E
        main.pushstr_newL(strBuffer);
        main.NewLine();

        // These H & D arrays of CSR do not change. Hence hardcoded!
        main.pushstr_newL("int *h_meta;");
        main.pushstr_newL("int *h_data;");
        main.pushstr_newL("int *h_src;");
        main.pushstr_newL("int *h_weight;");
        main.pushstr_newL("int *h_rev_meta;");  //done only to handle PR since other doesn't uses it
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

    void dsl_cpp_generator::generateFuncHeader(Function* proc) 
    {
        main.pushString("void ");
        main.pushString(proc->getIdentifier()->getIdentifier());

        main.push('(');
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = proc->getParamList().size();
        list<formalParam*> paramList = proc->getParamList();
        list<formalParam*>::iterator itr;

        for (itr = paramList.begin(); itr != paramList.end(); itr++) 
        {
            arg_currNo++;
            argumentTotal--;

            Type* type = (*itr)->getType();
            main.pushString(convertToCppType(type));
            char* parName = (*itr)->getIdentifier()->getIdentifier();

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
            if(argumentTotal > 0) main.pushString(",");
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

    void dsl_cpp_generator::generateMallocDeviceStr(const char* dVar, const char* typeStr, const char* sizeOfType) 
    {
        char strBuffer[1024];
        // dVar=malloc_device<type>(V, Q);
        sprintf(strBuffer, "%s=malloc_device<%s>(%s, Q);", dVar, typeStr, sizeOfType);
        main.pushstr_newL(strBuffer);
        //~ main.NewLine();
    }

    // Only for device variables
    void dsl_cpp_generator::generateMallocDevice(Type* type, const char* identifier) {
        char strBuffer[1024];
        //~ Type* targetType = type->getInnerTargetType();
        // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
        //                   1             2      3
        // dVar=malloc_device<type>(V, Q);
        sprintf(strBuffer, "d_%s=malloc_device<%s>(%s, Q);", identifier,
                convertToCppType(type->getInnerTargetType()),
                (type->isPropNodeType())
                    ? "V"
                    : "E");  // this assumes PropNode type  IS PROPNODE? V : E //else
                            // might error later

        main.pushstr_newL(strBuffer);
        main.NewLine();
    }

    void dsl_cpp_generator::generateMallocDeviceParams(list<formalParam*> paramList) 
    {
        list<formalParam*>::iterator itr;
        for (itr = paramList.begin(); itr != paramList.end(); itr++) {
            Type* type = (*itr)->getType();
            if (type->isPropType()) 
            {
                Type* innerType = type->getInnerTargetType();
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

    void dsl_cpp_generator::generateMemCpyStr(const char* sVarName, const char* tVarName, const char* type, const char* sizeV) 
    {
        //Q.submit([&](handler &h)
        //     { h.memcpy(d_data, h_data, sizeof(T) * N); })
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
        //int NUM_THREADS = 1048576;
        //int stride = NUM_THREADS;

        char strBuffer[1024];
        main.NewLine();

        sprintf(strBuffer, "int NUM_THREADS = 1048576;");
        main.pushstr_newL(strBuffer);

        sprintf(strBuffer, "int stride = NUM_THREADS;");
        main.pushstr_newL(strBuffer);
    }

    void dsl_cpp_generator::generateFuncBody(Function* proc)
    {
        char strBuffer[1024];
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = proc->getParamList().size();
        list<formalParam*> paramList = proc->getParamList();
        list<formalParam*>::iterator itr;

        const char* gId;

        for (itr = paramList.begin(); itr != paramList.end(); itr++) 
        {
            arg_currNo++;
            argumentTotal--;

            Type* type = (*itr)->getType();

            const char* parName = (*itr)->getIdentifier()->getIdentifier();
            cout << "param:" <<  parName << endl;
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

        sprintf(strBuffer, "default_selector d_selector;");
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer, "queue Q(d_selector);");
        main.pushstr_newL(strBuffer);
        if(genCSR){
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

    void dsl_cpp_generator::castIfRequired(Type* type, Identifier* methodID,dslCodePad& main) 
    {
        /* This needs to be made generalized, extended for all predefined function,
            made available by the DSL*/
        string predefinedFunc("num_nodes");
        if (predefinedFunc.compare(methodID->getIdentifier()) == 0) {
            if (type->gettypeId() != TYPE_INT) {
            char strBuffer[1024];
            sprintf(strBuffer, "(%s)", convertToCppType(type));
            main.pushString(strBuffer);
            }
        }
    }

    void dsl_cpp_generator::generate_exprLiteral(Expression* expr, bool isMainFile) 
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

    void dsl_cpp_generator::generate_exprInfinity(Expression* expr, bool isMainFile) 
    {
        char valBuffer[1024];
        if (expr->getTypeofExpr()) {
            int typeClass = expr->getTypeofExpr();
            switch (typeClass) {
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

        } else
        {
            sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
        }
        main.pushString(valBuffer);
    }

    void dsl_cpp_generator::generate_exprPropId(PropAccess* propId, bool isMainFile)
    {
        char strBuffer[1024];
        Identifier* id1 = propId->getIdentifier1();
        Identifier* id2 = propId->getIdentifier2();
        ASTNode* propParent = propId->getParent();
        bool relatedToReduction =
            propParent != NULL ? propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT
                                : false;
        if (id2->getSymbolInfo() != NULL &&
            id2->getSymbolInfo()->getId()->get_fp_association() &&
            relatedToReduction) {
            sprintf(strBuffer, "d_%s_next[%s]", id2->getIdentifier(),
                    id1->getIdentifier());
        } else {
            if (!isMainFile)
            sprintf(strBuffer, "d_%s[%s]", id2->getIdentifier(), id1->getIdentifier());  // PREFIX D
            else
            sprintf(strBuffer, "%s[%s]", id2->getIdentifier(), id1->getIdentifier());
        }
        main.pushString(strBuffer);
    }

    void dsl_cpp_generator::generate_exprArL(Expression* expr, bool isMainFile, bool isAtomic) 
    {
        if (expr->hasEnclosedBrackets()) {
            main.pushString("(");
        }
        if (!isAtomic)
            generateExpr(expr->getLeft(), isMainFile);
        main.space();
        const char* operatorString = getOperatorString(expr->getOperatorType());
        if (!isAtomic)
            main.pushstr_space(operatorString);
        generateExpr(expr->getRight(), isMainFile);
        if (expr->hasEnclosedBrackets()) {
            main.pushString(")");
        }
    }

    void dsl_cpp_generator::generate_exprRelational(Expression* expr, bool isMainFile) 
    {
        if (expr->hasEnclosedBrackets()) {
            main.pushString("(");
        }
        generateExpr(expr->getLeft(), isMainFile);

        main.space();
        const char* operatorString = getOperatorString(expr->getOperatorType());
        main.pushstr_space(operatorString);
        generateExpr(expr->getRight(), isMainFile);
        if (expr->hasEnclosedBrackets()) {
            main.pushString(")");
        }
    }

    void dsl_cpp_generator::generate_exprProcCall(Expression* expr, bool isMainFile)
    {
        proc_callExpr* proc = (proc_callExpr*)expr;
        string methodId(proc->getMethodId()->getIdentifier());
        if (methodId == "get_edge") {
            main.pushString("edge");                           
                                                
        } else if (methodId == "count_outNbrs")
        {
            char strBuffer[1024];
            list<argument*> argList = proc->getArgList();
            assert(argList.size() == 1);
            Identifier* nodeId = argList.front()->getExpr()->getId();
            //~ Identifier* objectId = proc->getId1();
            sprintf(strBuffer, "(%s[%s+1]-%s[%s])", "d_meta", nodeId->getIdentifier(), "d_meta", nodeId->getIdentifier());
            main.pushString(strBuffer);
        } else if (methodId == "is_an_edge") {
            char strBuffer[1024];
            list<argument*> argList = proc->getArgList();
            assert(argList.size() == 2);
            Identifier* srcId = argList.front()->getExpr()->getId();
            Identifier* destId = argList.back()->getExpr()->getId();
            //~ Identifier* objectId = proc->getId1();
            sprintf(strBuffer, "%s(%s, %s, %s, %s)", "findNeighborSorted", srcId->getIdentifier(), destId->getIdentifier(), "d_meta", "d_data");
            main.pushString(strBuffer);

        } else {
            char strBuffer[1024];
            list<argument*> argList = proc->getArgList();
            if (argList.size() == 0) {
            Identifier* objectId = proc->getId1();
            sprintf(strBuffer, "%s.%s( )", objectId->getIdentifier(),proc->getMethodId()->getIdentifier());
            main.pushString(strBuffer);
            }
        }
    }

    void dsl_cpp_generator::generate_exprUnary(Expression* expr, bool isMainFile) 
    {
        if (expr->hasEnclosedBrackets()) {
            main.pushString("(");
        }

        if (expr->getOperatorType() == OPERATOR_NOT) {
            const char* operatorString = getOperatorString(expr->getOperatorType());
            main.pushString(operatorString);
            generateExpr(expr->getUnaryExpr(), isMainFile);
        }

        if (expr->getOperatorType() == OPERATOR_INC ||
            expr->getOperatorType() == OPERATOR_DEC) {
            generateExpr(expr->getUnaryExpr(), isMainFile);
            const char* operatorString = getOperatorString(expr->getOperatorType());
            main.pushString(operatorString);
        }

        if (expr->hasEnclosedBrackets()) {
            main.pushString(")");
        }
    }

    void dsl_cpp_generator::generate_exprIdentifier(Identifier* id, bool isMainFile) 
    {
        main.pushString(id->getIdentifier());
    }

    void dsl_cpp_generator::generateExpr(Expression* expr, bool isMainFile, bool isAtomic) 
    {
        if (expr->isLiteral()) {
            generate_exprLiteral(expr, isMainFile);
        } else if (expr->isInfinity()) {
            generate_exprInfinity(expr, isMainFile);
        } else if (expr->isIdentifierExpr()) {
            generate_exprIdentifier(expr->getId(), isMainFile);
        } else if (expr->isPropIdExpr()) {
            generate_exprPropId(expr->getPropId(), isMainFile);
        } else if (expr->isArithmetic() || expr->isLogical()) {
            generate_exprArL(expr, isMainFile, isAtomic);
        } else if (expr->isRelational()) {
            generate_exprRelational(expr, isMainFile);
        } else if (expr->isProcCallExpr()) {
            generate_exprProcCall(expr, isMainFile);
        } else if (expr->isUnary()) {
            generate_exprUnary(expr, isMainFile);
        } else {
            assert(false);
        }
    }

    void dsl_cpp_generator::generateVariableDecl(declaration* declStmt, bool isMainFile)
    {
        Type* type = declStmt->getType();
        
        if (type->isPropType())
        {
            if (type->getInnerTargetType()->isPrimitiveType())
            {
                Type* innerType = type->getInnerTargetType();
                main.pushString(convertToCppType(innerType));  // convertToCppType need to be modified.
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
            const char* varType = convertToCppType(type);
            const char* varName = declStmt->getdeclId()->getIdentifier();
            cout << "varT:" << varType << endl;
            cout << "varN:" << varName << endl;

            sprintf(strBuffer, "%s %s", varType, varName);
            main.pushString(strBuffer);

            if (declStmt->isInitialized()) 
            {
                main.pushString(" = ");
                if (declStmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
                    proc_callExpr* pExpr = (proc_callExpr*)declStmt->getExpressionAssigned();
                    Identifier* methodId = pExpr->getMethodId();
                    castIfRequired(type, methodId, main);
                }
                generateExpr(declStmt->getExpressionAssigned(), isMainFile);  // PRINTS RHS? YES
            }
            main.pushstr_newL("; // asst in main");
            main.NewLine();
        }
        else if (type->isNodeEdgeType()) 
        {
            main.pushstr_space(convertToCppType(type));
            main.pushString(declStmt->getdeclId()->getIdentifier());
            if (declStmt->isInitialized()) {
            main.pushString(" = ");
            generateExpr(declStmt->getExpressionAssigned(), isMainFile);
            main.pushstr_newL(";");
            }
        }
    }

    void dsl_cpp_generator::generateDeviceAssignmentStmt(assignment* asmt, bool isMainFile)
    {
        main.pushstr_newL("// Device assignment statement");
    }

    void dsl_cpp_generator::generateAtomicDeviceAssignmentStmt(assignment* asmt, bool isMainFile)
    {
        main.pushstr_newL("// Atomic device assignment statement");
    }

    void dsl_cpp_generator::generateStatement(statement* stmt, bool isMainFile)
    {
        if (stmt->getTypeofNode() == NODE_BLOCKSTMT) {
            generateBlock((blockStatement*)stmt, false, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_DECL) {
            generateVariableDecl((declaration*)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_ASSIGN) {
            // generateAssignmentStmt((assignment*)stmt);
            assignment* asst = (assignment*)stmt;
            if (asst->isDeviceAssignment())
                generateDeviceAssignmentStmt(asst, isMainFile);
            else  // atomic or normal asmt
                generateAtomicDeviceAssignmentStmt(asst, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_WHILESTMT) {
            // generateWhileStmt((whileStmt*) stmt);
        }

        if (stmt->getTypeofNode() == NODE_IFSTMT) {
            generateIfStmt((ifStmt*)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_DOWHILESTMT) {
            generateDoWhileStmt((dowhileStmt*)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
            std::cout << "STMT: For" << '\n';
            printf("isMainFile val %d\n", isMainFile);
            generateForAll((forallStmt*)stmt, isMainFile);
        }

        if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) {
            generateFixedPoint((fixedPointStmt*)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
            generateReductionStmt((reductionCallStmt*)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_ITRBFS) {
            generateBFSAbstraction((iterateBFS*)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_PROCCALLSTMT) {
            generateProcCall((proc_callStmt*)stmt, isMainFile);
        }
        if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
            unary_stmt* unaryStmt = (unary_stmt*)stmt;
            generateExpr(unaryStmt->getUnaryExpr(), isMainFile);
            main.pushstr_newL(";");
        }
        if (isOptimized && (stmt->getTypeofNode() == NODE_TRANSFERSTMT)) {
            varTransferStmt* transferStmt = (varTransferStmt*)stmt;
            generateTransferStmt(transferStmt);
        }

    }

    void dsl_cpp_generator::generateBlock(blockStatement* blockStmt, bool includeBrace, bool isMainFile)
    {
        usedVariables usedVars = getDeclaredPropertyVariablesOfBlock(blockStmt);
        list<Identifier*> vars = usedVars.getVariables();
        std::cout << "\t==VARSIZE:" << vars.size() << '\n';

        list<statement*> stmtList = blockStmt->returnStatements();
        list<statement*>::iterator itr;
        if (includeBrace) {
            main.pushstr_newL("{");
        }

        for (itr = stmtList.begin(); itr != stmtList.end(); itr++) {
            statement* stmt = *itr;
            generateStatement(stmt, isMainFile);
        }
    }

    void dsl_cpp_generator::generateFunc(ASTNode* proc) 
    {
        Function* func = (Function*)proc;
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
        if (bodyFile == NULL) return false;
        main.setOutputFile(bodyFile);
        return true;
    }

    void dsl_cpp_generator::closeOutputFile() 
    {

        if (bodyFile != NULL) {
            main.outputToFile();
            fclose(bodyFile);
        }

        bodyFile = NULL;
    }

    const char* dsl_cpp_generator::getOperatorString(int operatorId) 
    {
        switch (operatorId) {
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
        cout<<"INSIDE SYCL GENERATOR"<<endl;
        cout<<"FRONTEND VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size()<<endl;
        // //openFileforOutput();
        if (!openFileforOutput()) return false;
        generation_begin();

        list<Function*> funcList = frontEndContext.getFuncList();
        for (Function* func : funcList) {
            setCurrentFunc(func);
            generateFunc(func);
        }

        generation_end();

        closeOutputFile();

        return true;
    }

    void dsl_cpp_generator::setFileName(char* f)
    {
        char* token = strtok(f, "/");
        char* prevtoken;

        while (token != NULL) {
            prevtoken = token;
            token = strtok(NULL, "/");
        }
        fileName = prevtoken;
    }
    }
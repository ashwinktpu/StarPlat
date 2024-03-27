/**
 * Implementation of Code Generator for AMD HIP Backend.
 * Implements dsl_cpp_generator.h
 * 
 * @author cs22m056
*/

#include <cctype>
#include <sstream>
#include <stdexcept>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

#define HIT_CHECK std::cout << "Hit at line " << __LINE__ << " of function " << __func__ << " in file " << __FILE__ << "\n";
// #define DUMP_INFO 

namespace sphip {

    DslCppGenerator::DslCppGenerator(const std::string& fileName, const int threadsPerBlock) : 
        fileName(StripName(fileName)), 
        HEADER_GUARD("GENHIP_" + ToUpper(StripName(fileName)) + "_H"),
        threadsPerBlock(threadsPerBlock) {

        generateCsr = false;

        if(threadsPerBlock > 1024)
            throw std::runtime_error("Threads per block should be less than 1024");
    }   

    bool DslCppGenerator::Generate() {

        if(!OpenOutputFile()) return false;

        GenerateIncludeFiles();

        list<Function*> funcList = frontEndContext.getFuncList();
        for (Function* func : funcList) {
            SetCurrentFunction(func);
            GenerateFunction(func);
        }

        GenerateEndOfFile(); 
        CloseOutputFile();  

        return true;
    }

    void DslCppGenerator::GenerateIncludeFiles() {

        // TODO: Add meta comments for both header and main files

        header.pushStringWithNewLine("#ifndef " + HEADER_GUARD);
        header.pushStringWithNewLine("#define " + HEADER_GUARD);
        header.NewLine();

        header.addHeaderFile("iostream", true);
        header.addHeaderFile("climits", true);

        header.addHeaderFile("hip/hip_runtime.h", true);
        header.addHeaderFile("hip/hip_cooperative_groups.h", true); // TODO
        header.addHeaderFile("../graph.hpp", false);
        header.NewLine();

        main.addHeaderFile(this->fileName + ".h", false);
        main.NewLine();
        main.NewLine();
    }

    void DslCppGenerator::GenerateEndOfFile() {

        header.NewLine();
        header.pushStringWithNewLine("#endif");
    }

    void DslCppGenerator::GenerateFunction(Function* func) {

        GenerateFunctionHeader(func, false);
        GenerateFunctionHeader(func, true);

        main.pushStringWithNewLine("{");

        GenerateFunctionBody(func);
        main.NewLine();

        GenerateTimerStart();
        
        // GenerateHipMallocParams(func->getParamList()); //TODO: impl
        GenerateBlock(func->getBlockStatement(), false); //TODO: 
        
        GenerateTimerStop();

        GenerateHipMemcpyParams(func->getParamList());

        main.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateFunctionHeader(Function* func, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main:header;

        // TODO: Add function to increment indentation

        targetFile.pushString("void");
        targetFile.AddSpace();
        std::cout << func->getIdentifier()->getIdentifier() << std::endl;
        targetFile.pushString(func->getIdentifier()->getIdentifier());
        targetFile.pushString("(");
        targetFile.NewLine();
        // TODO: Add indent by "\t"

        /* Adding function parameters*/

        list<formalParam*> parameterList = func->getParamList();

        for(auto itr = parameterList.begin(); itr != parameterList.end(); itr++) {

            Type *type = (*itr)->getType();
            targetFile.pushString(ConvertToCppType(type)); // TODO: add function in header and impl
            targetFile.AddSpace();

            std::string parameterName = (*itr)->getIdentifier()->getIdentifier();
            // parameterName[0] = std::toupper(parameterName[0]);
            // parameterName = "d" + parameterName;

            targetFile.pushString(parameterName);

            if(!isMainFile && type->isGraphType())
                generateCsr = true;

            if(std::next(itr) != parameterList.end())
                targetFile.pushString(",");
            targetFile.NewLine();
        }

        //TODO: Remove indent
        targetFile.pushString(")");
        if(!isMainFile)
            targetFile.pushString(";");

        targetFile.NewLine();
    }

    void DslCppGenerator::GenerateFunctionBody(Function* func) {

        list<formalParam*> parameterList = func->getParamList();
        std::string graphId;

        for(auto itr = parameterList.begin(); itr != parameterList.end(); itr++) {

            Type *type = (*itr)->getType();
            const std::string parameterName = (*itr)->getIdentifier()->getIdentifier();

            if(type->isGraphType()) {
             
                generateCsr = true;
                graphId = parameterName;
            }
        }

        if(generateCsr) {

            GenerateCsrArrays(graphId, func);
            main.NewLine();
        }

        CheckAndGenerateVariables(func, "d");
        CheckAndGenerateHipMalloc(func);
        CheckAndGenerateMemcpy(func);
        GenerateLaunchConfiguration();
    }

    void DslCppGenerator::SetCurrentFunction(Function* func) {

        this->function = func;
    }

    std::string DslCppGenerator::ConvertToCppType(Type *type) {

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

    void DslCppGenerator::GenerateCsrArrays(const std::string &graphId, Function *func) {

        main.pushStringWithNewLine("int V = " + graphId + ".num_nodes();");
        main.pushStringWithNewLine("int E = " + graphId + ".num_edges();");

        main.NewLine();

        if(func->getIsWeightUsed())
            main.pushStringWithNewLine("int *edgeLens = " + graphId + ".getEdgeLen();");

        main.NewLine();

        CheckAndGenerateVariables(func, "h");
        CheckAndGenerateMalloc(func);

        if(func->getIsMetaUsed() || func->getIsRevMetaUsed()) {

            main.pushStringWithNewLine(
                "for(int i = 0; i <= V; i++) {"
            );

            if(func->getIsMetaUsed())
                main.pushStringWithNewLine(
                    "hOffsetArray[i] = " + graphId + ".indexOfNodes[i];"
                );

            if(func->getIsRevMetaUsed())
                main.pushStringWithNewLine(
                    "hRevOffsetArray[i] = " + graphId + ".rev_indexofNodes[i];"
                );

            main.pushStringWithNewLine("}");
        }

        if(
            func->getIsDataUsed() ||
            func->getIsSrcUsed()  ||
            func->getIsWeightUsed()
        ) {

            main.pushStringWithNewLine(
                "for(int i = 0; i < E; i++) {"
            );

            if(func->getIsDataUsed()) 
                main.pushStringWithNewLine(
                    "hEdgelist[i] = " + graphId + ".edgeList[i];"
                );

            if(func->getIsSrcUsed()) 
                main.pushStringWithNewLine(
                    "hSrcList[i] = " + graphId + ".srcList[i];"
                );

            if(func->getIsWeightUsed()) 
                main.pushStringWithNewLine(
                    "hWeight[i] = edgeLens[i];"
                );

            main.pushStringWithNewLine("}");
        }

        main.NewLine();
    }

    void DslCppGenerator::GenerateStatement(statement* stmt, bool isMainFile) {

        cout << "FLAG " << stmt->getTypeofNode() << "\n";

        switch (stmt->getTypeofNode()) {

        case NODE_BLOCKSTMT:
            GenerateBlock(static_cast<blockStatement*>(stmt), false, isMainFile);
            break;

        case NODE_DECL:
            GenerateVariableDeclaration(static_cast<declaration*>(stmt), isMainFile);
            break;

        case NODE_ASSIGN:
            {
                assignment *asst = static_cast<assignment*>(stmt);

                if(asst->isDeviceAssignment())
                    GenerateDeviceAssignment(asst, isMainFile);
                else
                    GenerateAtomicOrNormalAssignment(asst, isMainFile);

                break;
            }

        case NODE_IFSTMT:
            GenerateIfStmt(static_cast<ifStmt*>(stmt), isMainFile);
            break;

        case NODE_FORALLSTMT:
            GenerateForAll(static_cast<forallStmt*>(stmt), isMainFile);
            break;

        case NODE_FIXEDPTSTMT:
            GenerateFixedPoint(static_cast<fixedPointStmt*>(stmt), isMainFile);
            break;

        case NODE_REDUCTIONCALLSTMT:
            GenerateReductionCallStmt(static_cast<reductionCallStmt*>(stmt), isMainFile);
            break;

        case NODE_ITRBFS:
            GenerateItrBfs(static_cast<iterateBFS*>(stmt), isMainFile);
            break;

        case NODE_ITRRBFS:
            // HIT_CHECK
            // GenerateItrRevBfs(static_cast<iterateReverseBFS*>(stmt), isMainFile);
            break;

        case NODE_PROCCALLSTMT:
            GenerateProcCallStmt(static_cast<proc_callStmt*>(stmt), isMainFile);
            break;
        
        default:
            throw std::runtime_error("Generation function not implemented for this node!");
            break;
        }
    }

    void DslCppGenerator::GenerateBlock(
        blockStatement* blockStmt,
        bool includeBrace,
        bool isMainFile
    ) {

        dslCodePad &targetFile = isMainFile ? main : header;

        UsedVariables usedVariables = GetDeclaredPropertyVariablesOfBlock(blockStmt);
        //TODO : Add the cuda free handling and what not
        
        list<statement*> stmtList = blockStmt->returnStatements();

        if(includeBrace)
            targetFile.pushStringWithNewLine("{");

        for(auto itr = stmtList.begin(); itr != stmtList.end(); itr++)
            GenerateStatement(*itr, isMainFile);

        if(includeBrace)
            targetFile.pushStringWithNewLine("}");

        main.NewLine();
    }

    void DslCppGenerator::GenerateVariableDeclaration(declaration* stmt, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main : header;

        Type *type = stmt->getType();
        // cout << stmt->getdeclId()->getIdentifier() << "\n";

        if(type->isPrimitiveType()) {
            
            cout << stmt->getdeclId()->getIdentifier() << "\n";
        }

        if(type->isPropType()) {

            if(type->getInnerTargetType()->isPrimitiveType()) {

                Type *innerType = type->getInnerTargetType();

                main.pushString(ConvertToCppType(innerType));
                main.AddSpace();
                main.pushString("*");
                main.pushString("d");

                std::string idName = stmt->getdeclId()->getIdentifier();
                idName[0] = std::toupper(idName[0]);
                main.pushString(idName);
                main.pushStringWithNewLine(";");

                GenerateHipMalloc(type, idName);
                
                // TODO: Check if the below stuff is reqd

                // if(stmt->getdeclId()->getSymbolInfo()->getId()->require_redecl()) {

                //     main.pushString(ConvertToCppType(innerType));
                //     main.AddSpace();
                //     main.pushString("*");
                //     main.pushString("d");
                //     main.pushString(idName);
                //     main.pushString("Next");
                //     main.pushString(";");
                //     main.NewLine();

                //     GenerateHipMalloc(type, idName + "Next");
                // }
            }
        } else if(type->isNodeEdgeType()) {
        
            targetFile.pushString(ConvertToCppType(type));
            targetFile.AddSpace();
            targetFile.pushString(stmt->getdeclId()->getIdentifier());

            if(stmt->isInitialized()) {

                targetFile.pushString(" = ");
                GenerateExpression(stmt->getExpressionAssigned(), isMainFile); // TODO
                targetFile.pushStringWithNewLine(";");
            }
        
        } else if(type->isPrimitiveType()) {

            std::cerr << "Hit in primitive type\n";

            std::string strBuffer;

            const std::string varType = ConvertToCppType(type);
            const std::string varName = stmt->getdeclId()->getIdentifier();

            cout << varName << " " << varType << "\n";
        }
    }

    void DslCppGenerator::GenerateDeviceAssignment(assignment* asmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        bool isDevice = false;
        string str;        

        if (asmt->lhs_isProp()) {

            PropAccess* propId = asmt->getPropId();
            if (asmt->isDeviceAssignment()) {
            isDevice = true;
            Type* typeB = propId->getIdentifier2()->getSymbolInfo()->getType()->getInnerTargetType();
            std::string varType = ConvertToCppType(typeB);  //DONE: get the type from id
            std::string temp2(propId->getIdentifier2()->getIdentifier());
            temp2[0] = toupper(temp2[0]);
            std::string temp1(propId->getIdentifier1()->getIdentifier());
            // temp1[0] = toupper(temp1[0]);

            str = "initIndex<" + varType + "><<<1,1>>>(V, d" 
                    + temp2 + ", " + temp1 + ", ";
            targetFile.pushString(str);
            } 
        }

        GenerateExpression(asmt->getExpr(), isMainFile);

        targetFile.pushString(");");
        targetFile.NewLine();
    }

    void DslCppGenerator::GenerateAtomicOrNormalAssignment(assignment* stmt, bool isMainFile) {

        cout << "Inside GenerateAtomicOrNormalAssignment\n";

    }
    

    void DslCppGenerator::GenerateIfStmt(ifStmt* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        Expression* condition = stmt->getCondition();

        targetFile.pushString("if (");
        GenerateExpression(condition, isMainFile);
        targetFile.pushStringWithNewLine(") {");
        GenerateStatement(stmt->getIfBody(), isMainFile);
        targetFile.pushString("} ");

        if(stmt->getElseBody() != NULL) {

            targetFile.pushStringWithNewLine(" else {");
            GenerateStatement(stmt->getElseBody(), isMainFile);
            targetFile.pushStringWithNewLine("}");
        } else {
            targetFile.NewLine();
        }
    }

    void DslCppGenerator::GenerateForAll(forallStmt* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        proc_callExpr* extractElemFunc = stmt->getExtractElementFunc();
        PropAccess* sourceField = stmt->getPropSource();
        Identifier* sourceId = stmt->getSource();
        Identifier* collectionId;
                
        if(sourceField){ 
            collectionId = sourceField->getIdentifier1();
        }

        if(sourceId){
            collectionId = sourceId;
        }

        Identifier* iteratorMethodId;

        if(extractElemFunc)
            iteratorMethodId = extractElemFunc->getMethodId();

        statement* body = stmt->getBody();
        string buffer;

        if(stmt->isForall()) { // This makes sure that this a parallelizable outer for-loop and not the inside for loop, which won't be parallelized.

            // // TODO: Check OMP Line 1160

            // if(stmt->hasFilterExpr() || stmt->hasFilterExprAssoc()) {
                
            //     Expression *filterExpression = stmt->hasFilterExpr() ? stmt->getfilterExpr() : stmt->getAssocExpr();
            //     Expression *lhs = filterExpression->getLeft();
            //     Identifier *filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
            //     TableEntry *entry = (filterId != NULL) ? filterId->getSymbolInfo() : NULL;

            //     if(entry && entry->getId()->get_fp_association()) {

            //         main.pushString(string(function->getIdentifier()->getIdentifier()) + "Kernel");
            //         main.pushString("<<<numBlocks, numThreads>>>");
            //         main.pushString("(V, E");
                    
            //         if(stmt->getIsMetaUsed())
            //             main.pushString(", dOffsetArray");

            //         if(stmt->getIsDataUsed())
            //             main.pushString(", dEdgeList");

            //         if(stmt->getIsSrcUsed())
            //             main.pushString(", dSrcList");

            //         if(stmt->getIsWeightUsed()) 
            //             main.pushString(", dWeight");

            //         if(stmt->getIsRevMetaUsed())
            //             main.pushString(", dRevOffsetArray");

            //         main.pushStringWithNewLine(");");
            //     }


            // } else {
            //     cout << "HIT filter else\n";
            // }

            // UsedVariables usedVariables = GetUsedVariablesInForAll(stmt);
            UsedVariables use;

            for(auto iden: use.GetUsedVariables()) {

                std::cout << "MAXXX" << iden->getIdentifier() << "\n";
            }

        } else {

        }
            cout << "HIT inside FORALL ELSE\n";
    }

    void DslCppGenerator::GenerateFixedPoint(fixedPointStmt* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;

        string buffer;

        Expression* convergenceExpr = stmt->getDependentProp();
        Identifier* fixedPointId = stmt->getFixedPointId();
        
        //TODO : Add assertions

        Identifier* dependentId = convergenceExpr->getUnaryExpr()->getId();  // TODO: Possible checks avoided

        string modifiedVar = dependentId->getIdentifier();
        string fixedPointVar = fixedPointId->getIdentifier();
        string fixedPointVarType = ConvertToCppType(fixedPointId->getSymbolInfo()->getType());

        modifiedVar[0] = std::toupper(modifiedVar[0]);
        modifiedVar = "d" + modifiedVar;

        string modifiedVarNext = modifiedVar + "Next";

        if(convergenceExpr->getExpressionFamily() == EXPR_ID) {
            dependentId = convergenceExpr->getId();
        }

        if(dependentId != NULL && dependentId->getSymbolInfo()->getType()->isPropNodeType()) {

            targetFile.pushStringWithNewLine(
                "initKernel<" + fixedPointVarType + "><<<numBlocks, numThreads>>>(V, " + modifiedVarNext + ", false);"
            );

            targetFile.pushStringWithNewLine("int k = 0;");
            targetFile.NewLine();
            targetFile.pushStringWithNewLine("while(!" + fixedPointVar + ") {");
            targetFile.NewLine();
            targetFile.pushStringWithNewLine(fixedPointVar + " = true;");

            GenerateHipMemcpySymbol(fixedPointVar, fixedPointVarType, true); //! TODO: Implement
            
            if(stmt->getBody()->getTypeofNode() != NODE_BLOCKSTMT) {
                GenerateStatement(stmt->getBody(), isMainFile);
            } else {
                GenerateBlock(static_cast<blockStatement*>(stmt->getBody()), false, isMainFile);
            }

            GenerateHipMemcpySymbol(fixedPointVar, fixedPointVarType, false); //! TODO: Implement
            GenerateHipMemcpyStr(modifiedVar, modifiedVarNext, fixedPointVarType, "V", false); //! TODO: Implement
            
            targetFile.pushStringWithNewLine(
                "initKernel<" + fixedPointVarType + "><<<numBlocks, numThreads>>>(V, " + modifiedVarNext + ", false);"
            );

            targetFile.pushStringWithNewLine("k++;");
            targetFile.pushStringWithNewLine("}");
        }

    }

    void DslCppGenerator::GenerateReductionCallStmt(reductionCallStmt* stmt, bool isMainFile) {

        cout << "Inside GenerateReductionCallStmt\n";

    }

    void DslCppGenerator::GenerateItrBfs(iterateBFS* stmt, bool isMainFile) {

        cout << "Inside GenerateItrRevBfs\n";
       
    }

    void DslCppGenerator::GenerateItrRevBfs(iterateReverseBFS* stmt, bool isMainFile) {

        cout << "Inside GenerateItrRevBfs\n";

    }

    void DslCppGenerator::GenerateProcCallStmt(proc_callStmt* stmt, bool isMainFile) {

        proc_callExpr *proc = stmt->getProcCallExpr();
        string methodId = proc->getMethodId()->getIdentifier();
        string nodeCall = "attachNodeProperty";
        string edgeCall = "attachEdgeProperty";

        if(methodId.compare(nodeCall) == 0) {

            list<argument*> argList = proc->getArgList();

            for(auto itr =  argList.begin(); itr != argList.end(); itr++)
                GenerateInitKernelCall((*itr)->getAssignExpr(), isMainFile);
            

        } else if(methodId.compare(edgeCall) == 0) {

            HIT_CHECK

        } else {

            HIT_CHECK

        }
        
    }

    void DslCppGenerator::GenerateExpression(
        Expression *expr, bool isMainFile, bool isAtomic
    ) {

        if(expr->isLiteral()) 
            GenerateExpressionLiteral(expr, isMainFile);

        else if(expr->isInfinity())
            GenerateExpressionInfinity(expr, isMainFile);

        else if(expr->isIdentifierExpr())
            GenerateExpressionIdentifier(expr->getId(), isMainFile);

        else if(expr->isPropIdExpr())
            GenerateExpressionPropId(expr->getPropId(), isMainFile);

        else if(expr->isArithmetic() || expr->isLogical())
            GenerateExpressionArithmeticOrLogical(expr, isMainFile, isAtomic);

        else if(expr->isRelational())
            GenerateExpressionRelational(expr, isMainFile);

        else if(expr->isProcCallExpr())
            GenerateExpressionProcCallExpression(static_cast<proc_callExpr*>(expr), isMainFile);

        else if(expr->isUnary())
            GenerateExpressionUnary(expr, isMainFile);

        else    
            assert(false);
    }

    void DslCppGenerator::GenerateExpressionLiteral(Expression* expr, bool isMainFile) {

        int expressionType = expr->getExpressionFamily();
        std::ostringstream oss;

        switch(expressionType) {

            case EXPR_INTCONSTANT:
                oss << expr->getIntegerConstant();
                break;

            case EXPR_FLOATCONSTANT:
                oss << expr->getFloatConstant();
                break;

            case EXPR_BOOLCONSTANT:
                oss << (expr->getBooleanConstant() ? "true" : "false");
                break;

            default:
                assert(false);
        }

        (isMainFile ? main : header).pushString(oss.str());
    }

    void DslCppGenerator::GenerateExpressionInfinity(Expression* expr, bool isMainFile) {

        std::string value = expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN";

        if(expr->getTypeofExpr()) {

            switch(expr->getTypeofExpr()) {

                case TYPE_INT:
                    value = expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN";
                    break;

                case TYPE_LONG:
                    value = expr->isPositiveInfinity() ? "LLONG_MAX" : "LLONG_MIN";
                    break;

                case TYPE_FLOAT:
                    value = expr->isPositiveInfinity() ? "FLT_MAX" : "FLT_MIN";
                    break;

                case TYPE_DOUBLE:
                    value = expr->isPositiveInfinity() ? "DBL_MAX" : "DBL_MIN";
                    break;
            }
        }

        (isMainFile ? main : header).pushString(value);
    }

    void DslCppGenerator::GenerateExpressionIdentifier(Identifier* id, bool isMainFile) {

        (isMainFile ? main : header).pushString(id->getIdentifier());
    }

    void DslCppGenerator::GenerateExpressionPropId(PropAccess* propId, bool isMainFile) {

        std::string value;

        Identifier *id1 = propId->getIdentifier1();
        Identifier *id2 = propId->getIdentifier2();
        ASTNode *propParent = propId->getParent();
        bool isRelatedToReduction = false;
        
        if(propParent)
            isRelatedToReduction = propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT;

        if(
            id2->getSymbolInfo() &&
            id2->getSymbolInfo()->getId()->get_fp_association() &&
            isRelatedToReduction
        ) 
            value = string("d") + id2->getIdentifier() + "Next[" + id1->getIdentifier() + "]";
        else {

            if(isMainFile)
                value = id2->getIdentifier() + string("[") + id1->getIdentifier() + "]";
            else
                value = string("d") + id2->getIdentifier() + "[" + id1->getIdentifier() + "]";
        }

        (isMainFile ? main : header).pushString(value);
    }

    void DslCppGenerator::GenerateExpressionArithmeticOrLogical(
        Expression* expr, bool isMainFile, bool isAtomic
    ) {

        dslCodePad &target = isMainFile ? main : header;

        if(expr->hasEnclosedBrackets())
            target.pushString("(");

        if(!isAtomic)
            GenerateExpression(expr->getLeft(), isMainFile);

        target.AddSpace();

        const std::string operatorString = GetOperatorString(expr->getOperatorType());

        if(!isAtomic) {
            target.pushString(operatorString);
            target.AddSpace();
        }

        GenerateExpression(expr->getRight(), isMainFile);

        if(expr->hasEnclosedBrackets())
            target.pushString(")");
    }

    void DslCppGenerator::GenerateExpressionRelational(Expression* expr, bool isMainFile) {

        dslCodePad & target = isMainFile ? main : header;

        if(expr->hasEnclosedBrackets())
            target.pushString("(");

        GenerateExpression(expr->getLeft(), isMainFile);
        target.AddSpace();

        const string op = GetOperatorString(expr->getOperatorType());
        target.pushString(op);
        GenerateExpression(expr->getRight(), isMainFile);

        if(expr->hasEnclosedBrackets())
            target.pushString(")");
    }

    void DslCppGenerator::GenerateExpressionProcCallExpression(
        proc_callExpr* expr, bool isMainFile
    ) {

        dslCodePad & targetFile = isMainFile ? main : header;

        string methodId(expr->getMethodId()->getIdentifier());

        if (methodId == "get_edge") 
            targetFile.pushString("edge");
        
        else if (methodId == "count_outNbrs"){

            string strBuffer;
            list<argument*> argList = expr->getArgList();
            assert(argList.size() == 1);

            Identifier* nodeId = argList.front()->getExpr()->getId();

            strBuffer = string("(dOffsetArray[") + nodeId->getIdentifier() + " + 1] -" +
                         "dOffsetArray[" + nodeId->getIdentifier() + "]";

            targetFile.pushString(strBuffer);

        } else if (methodId == "is_an_edge") {

            //TODO: Implement
            cout << "DANGER HIT";

        } else {
            
            cout << "DANGER HIT";
        }
    }

    void DslCppGenerator::GenerateExpressionUnary(
        Expression* expr, bool isMainFile
    ) {

        dslCodePad& targetFile = isMainFile ? main : header;

        if (expr->hasEnclosedBrackets()) {
            targetFile.pushString("(");
        }

        if (expr->getOperatorType() == OPERATOR_NOT) {

            const string operatorString = GetOperatorString(expr->getOperatorType());
            targetFile.pushString(operatorString);
            GenerateExpression(expr->getUnaryExpr(), isMainFile);
        }

        if (
            expr->getOperatorType() == OPERATOR_INC ||
            expr->getOperatorType() == OPERATOR_DEC
        ) {

            GenerateExpression(expr->getUnaryExpr(), isMainFile);
            const string operatorString = GetOperatorString(expr->getOperatorType());
            targetFile.pushString(operatorString);
        }

        if (expr->hasEnclosedBrackets()) {
            targetFile.pushString(")");
        }
    }


    void DslCppGenerator::GenerateMallocStr(
        const std::string &hVar, 
        const std::string &typeStr, 
        const std::string &sizeOfType
    ) {
        cout << "FLAGFLAG2\n";

        main.pushStringWithNewLine(
            hVar + " = (" + typeStr + "*) malloc(sizeof(" + typeStr + ")" +
            " * (" + sizeOfType + "));"
        );
    }

    /**
     * Simple functions to reduce cognitive complexity
    */

   /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateVariables(Function *function, const std::string &loc) {


        if(function->getIsMetaUsed()) 
            main.pushStringWithNewLine("int *" + loc +"OffsetArray;");

        if(function->getIsDataUsed())
            main.pushStringWithNewLine("int *" + loc +"Edgelist;");

        if(function->getIsSrcUsed())
            main.pushStringWithNewLine("int *" + loc +"SrcList;");

        if(function->getIsWeightUsed())
            main.pushStringWithNewLine("int *" + loc +"Weight;");

        if(function->getIsRevMetaUsed())
            main.pushStringWithNewLine("int *" + loc +"RevOffsetArray;");

        main.NewLine();
    }

    /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateHipMalloc(Function *function) {
        cout << "FLAGFLAG\n";

        if(function->getIsMetaUsed()) 
            GenerateHipMallocStr("dOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateHipMallocStr("dEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateHipMallocStr("dSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateHipMallocStr("dWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateHipMallocStr("dRevOffsetArray", "int", "V + 1");

        main.NewLine();
    }

    /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateMalloc(Function *function) {

        if(function->getIsMetaUsed()) 
            GenerateMallocStr("hOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateMallocStr("hEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateMallocStr("hSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateMallocStr("hWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateMallocStr("hRevOffsetArray", "int", "V + 1");

        main.NewLine();
    }

    /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateMemcpy(Function *function) {

        if(function->getIsMetaUsed())
            GenerateHipMemcpyStr("dOffsetArray", "hOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateHipMemcpyStr("dEdgelist", "hEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateHipMemcpyStr("dSrcList", "hSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateHipMemcpyStr("dWeight", "hWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateHipMemcpyStr("dRevOffsetArray", "hRevOffsetArray", "int", "V + 1");

        main.NewLine();
    }
}
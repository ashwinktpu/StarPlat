/**
 * Implementation of Code Generator for AMD HIP Backend.
 * Implements dsl_cpp_generator.h
 * 
 * @author cs22m056
*/

#include <cctype>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

const bool debug = false; // Used for testing

namespace sphip {

    DslCppGenerator::DslCppGenerator(const std::string& fileName, const int threadsPerBlock) : 
        fileName(StripName(fileName)), 
        HEADER_GUARD("GENHIP_" + ToUpper(StripName(fileName)) + "_H"),
        threadsPerBlock(threadsPerBlock) {

        generateCsr = false;
        generateIsAnEdgeFunction = false;

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

        GenerateAuxillaryKernels();
        GenerateAuxillaryFunctions();
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

        // TODO: make this a seperate function or add the returnType part into function header things
        std::string returnType = "void";
        bool isReturn = false;
        blockStatement* blockStmt = func->getBlockStatement();
        list<statement*> stmtList = blockStmt->returnStatements();
        for(auto stmt: stmtList) {
            if(stmt->getTypeofNode() == NODE_RETURN) {
                returnType = ConvertToCppType(static_cast<returnStmt*>(stmt)->getReturnExpression()->getId()->getSymbolInfo()->getType());
                isReturn = true;
                break;
            }
        } 

        main.pushString(returnType);
        header.pushString(returnType);
        // TODO: ends here
        
        GenerateFunctionHeader(func, false);
        GenerateFunctionHeader(func, true);

        GenerateFunctionBody(func);

        GenerateTimerStart(); // TODO: Make this flag dependent
        
        main.NewLine();
        GenerateBlock(func->getBlockStatement(), false);

        if(!isReturn) // In case of return, we will generate timer while handling NODE_RETURN
            GenerateTimerStop();
        
        GenerateCopyBackToHost(func->getParamList()); // TODO: This is required when the resullt is to be pass back via a paramenter
        HIT_CHECK // TODO: Add increment function count method. refer cuda
        //TODO: Add hipfree and other host free. 
        main.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateFunctionHeader(Function* func, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main:header;

        // targetFile.pushString("void");
        targetFile.AddSpace();
        targetFile.pushString(func->getIdentifier()->getIdentifier());
        targetFile.pushString("(");
        targetFile.NewLine();

        /* Adding function parameters*/

        list<formalParam*> parameterList = func->getParamList();

        for(auto itr = parameterList.begin(); itr != parameterList.end(); itr++) {

            Type *type = (*itr)->getType();
            targetFile.pushString(ConvertToCppType(type)); // TODO: add function in header and impl
            targetFile.AddSpace();

            std::string parameterName = (*itr)->getIdentifier()->getIdentifier();

            if(type->isPropType() || type->isPrimitiveType()) {
                parameterName[0] = std::toupper(parameterName[0]);
                parameterName = "h" + parameterName;
            }

            

            targetFile.pushString(parameterName);

            if(!isMainFile && type->isGraphType())
                generateCsr = true;

            if(std::next(itr) != parameterList.end())
                targetFile.pushString(",");
            targetFile.NewLine();
        }

        targetFile.pushString(")");
        if(!isMainFile) {
            targetFile.pushString(";");
            targetFile.NewLine();
            targetFile.NewLine();
            /**
             * All auxillary function declaratiions should be added here
            */
            targetFile.pushStringWithNewLine("__device__ \nbool IsAnEdge(const int, const int, const int*, const int*);");
        } else {
            targetFile.pushStringWithNewLine(" {");        
        }
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

        // TODO The below code will generate all variables
        // ie the ones which are part of the function formal
        // parameters and the ones which we get from the graph(IsMetaUsed etc)
        // TODO: Check if this is okay or if we need seperate 
        // functions for both 
        CheckAndGenerateVariables(func, "d");
        CheckAndGenerateHipMalloc(func);
        CheckAndGenerateMemcpy(func);
        GenerateFormalParameterDeclAllocCopy(func->getParamList());
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
            return "int"; 

        } else if (type->isGraphType()) {
            return "graph&";
        } else if (type->isCollectionType()) {

            switch (type->gettypeId()) {
                case TYPE_SETN:
                    return "std::set<int>&";

                default:
                    throw std::runtime_error("Collection type not implemented.");
            }
        }

        throw std::runtime_error("Unknown type in ConvertToCppType");
    }

    void DslCppGenerator::GenerateCsrArrays(const std::string &graphId, Function *func) {

        main.pushStringWithNewLine("int V = " + graphId + ".num_nodes();");
        main.pushStringWithNewLine("int E = " + graphId + ".num_edges();");

        main.NewLine();

        if(func->getIsWeightUsed()) {
            
            main.pushStringWithNewLine("int *edgeLens = " + graphId + ".getEdgeLen();");
            main.NewLine();
        }

        CheckAndGenerateVariables(func, "h");
        CheckAndGenerateMalloc(func);

        if(func->getIsMetaUsed() || func->getIsRevMetaUsed()) {

            main.pushStringWithNewLine(
                "for(int i = 0; i <= V; i++) {"
            );

            if(func->getIsMetaUsed())
                main.pushStringWithNewLine(
                    "hOffsetArray[i] = " + graphId + ".indexofNodes[i];"
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

        nodeStack.push(stmt);

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
                if (asst->isDeviceAssignment())
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
            throw std::runtime_error("Generation function not implemented for NODETYPE " + std::to_string(stmt->getTypeofNode()));
            break;

        case NODE_PROCCALLSTMT:
            GenerateProcCallStmt(static_cast<proc_callStmt*>(stmt), isMainFile);
            break;

        case NODE_WHILESTMT:
            GenerateWhileStmt(static_cast<whileStmt*>(stmt), isMainFile);
            break;

        case NODE_DOWHILESTMT:
            GenerateDoWhile(static_cast<dowhileStmt*>(stmt), isMainFile);
            break;

        case NODE_UNARYSTMT:
            GenerateUnaryStmt(static_cast<unary_stmt*>(stmt), isMainFile);   
            break;
        
        case NODE_RETURN:{
            GenerateTimerStop();
            returnStmt *retStmt = static_cast<returnStmt*>(stmt);
            main.pushString("return h");
            main.pushString(CapitalizeFirstLetter(retStmt->getReturnExpression()->getId()->getIdentifier()));
            main.pushString(";");
            break; 
        }

        default:
            throw std::runtime_error("Generation function not implemented for NODETYPE " + std::to_string(stmt->getTypeofNode()));
            break;
        }

        nodeStack.pop();
    }

    void DslCppGenerator::GenerateBlock(
        blockStatement* blockStmt,
        bool includeBrace,
        bool isMainFile
    ) {

        dslCodePad &targetFile = isMainFile ? main : header;

        //TODO : Add the cuda free handling and what not
        // usedVariables usedVars = GetDeclaredPropertyVariablesOfBlock(blockStmt);
        
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

            } else {
                HIT_CHECK
            }
        } else if(type->isNodeEdgeType()) {
            
            targetFile.pushString(ConvertToCppType(type));
            targetFile.AddSpace();
            const std::string prefix(isMainFile ? "h" : "d");
            targetFile.pushString(prefix +  CapitalizeFirstLetter(stmt->getdeclId()->getIdentifier()));

            if(stmt->isInitialized()) {

                targetFile.pushString(" = ");
                GenerateExpression(stmt->getExpressionAssigned(), isMainFile); // TODO
                targetFile.pushStringWithNewLine(";");
            }
        
        } else if(type->isPrimitiveType()) {

            if(isMainFile) {

                const std::string varType(ConvertToCppType(type));
                const std::string varName(stmt->getdeclId()->getIdentifier());
                targetFile.pushString(varType);
                targetFile.AddSpace();
                targetFile.pushString("h");
                targetFile.pushString(CapitalizeFirstLetter(varName));
                if(stmt->isInitialized()) {

                    targetFile.pushString(" = ");

                    if (stmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
                        HIT_CHECK
                    }
                    GenerateExpression(stmt->getExpressionAssigned(), isMainFile);
                }
                targetFile.pushStringWithNewLine(";");

                /**
                 * ALTERNATIVE POSSIBLE (BETTER) APPROACH
                 * 
                 * Usage of hipMallocManaged
                 * Currently, it is not supported on all GPUs
                 * Hence we will use the normal memory allocation
                */

                targetFile.pushString(varType);
                targetFile.AddSpace();
                targetFile.pushString("*d");
                targetFile.pushString(CapitalizeFirstLetter(varName));
                targetFile.pushStringWithNewLine(";");
            
                targetFile.pushStringWithNewLine(
                    "hipMalloc(&d" + CapitalizeFirstLetter(varName) + ", sizeof(" + varType + "));"
                );

                // GenerateHipMemcpyStr(
                //     "d" + CapitalizeFirstLetter(varName),
                //     "&h" + CapitalizeFirstLetter(varName),
                //     varType, "1", true
                // );
            } else {

                const std::string varType(ConvertToCppType(type));
                const std::string varName(stmt->getdeclId()->getIdentifier());
                targetFile.pushString(varType);
                targetFile.AddSpace();
                targetFile.pushString("d");
                targetFile.pushString(CapitalizeFirstLetter(varName));
                
                if(stmt->isInitialized()) {
                    targetFile.pushString(" = ");

                    if (stmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
                        HIT_CHECK
                    }
                    GenerateExpression(stmt->getExpressionAssigned(), isMainFile);
                }
                targetFile.pushStringWithNewLine(";");
            }
            
        } else {
            HIT_CHECK
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
                HIT_CHECK
                str = "InitArrayIndex<" + varType + "><<<1,1>>>(V, d" 
                        + temp2 + ", " + temp1 + ", ";
                targetFile.pushString(str);
            } 
        }

        GenerateExpression(asmt->getExpr(), isMainFile);

        targetFile.pushString(");");
        targetFile.NewLine();
    }

    //TODO: Rename this function properly
    void DslCppGenerator::GenerateAtomicOrNormalAssignment(assignment* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        bool isAtomic = false;
        bool isResult = false;

        if(stmt->lhs_isIdentifier()) {

            if(stmt->hasPropCopy()) {
                HIT_CHECK
                //TODO: Make generic. Possibly add a new function for this.
                targetFile.pushStringWithNewLine(
                    "hipMemcpy(d" + CapitalizeFirstLetter(stmt->getId()->getIdentifier()) + 
                    ", d" + CapitalizeFirstLetter(stmt->getExpr()->getId()->getIdentifier()) + ", sizeof(" +
                    ConvertToCppType(stmt->getId()->getSymbolInfo()->getType()->getInnerTargetType()) 
                    + ") * (V), hipMemcpyDeviceToDevice);"
                );

            } else {
                const std::string prefix(isMainFile ? "h" : "d");
                targetFile.pushString(prefix + CapitalizeFirstLetter(stmt->getId()->getIdentifier()));
            }
        } else if(stmt->lhs_isProp()) {

            PropAccess *propId = stmt->getPropId();
            if(stmt->getAtomicSignal()) {

                targetFile.pushString("atomicAdd(&");
                isAtomic = true;                
            }

            if(stmt->isAccumulateKernel()) {
                isResult = true;
            }

            targetFile.pushString("d");
            targetFile.pushString(CapitalizeFirstLetter(propId->getIdentifier2()->getIdentifier()));
            targetFile.pushString("[d");
            targetFile.pushString(CapitalizeFirstLetter(propId->getIdentifier1()->getIdentifier()));
            targetFile.pushString("]");

        } else {
            HIT_CHECK
        }

        if(isAtomic) {
            targetFile.pushString(", ");
        } else if(!stmt->hasPropCopy()) {
            targetFile.pushString(" = ");
        }

        if(!stmt->hasPropCopy())
            GenerateExpression(stmt->getExpr(), isMainFile, false, isAtomic);

        if(isAtomic) {
            targetFile.pushStringWithNewLine(");"); 
        } else if(isResult) {
            targetFile.pushStringWithNewLine(";");
        } else if(!stmt->hasPropCopy()) {
            targetFile.pushStringWithNewLine(";");
        }
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

            targetFile.pushStringWithNewLine("else {");
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
        Identifier* iteratorMethodId;

        if(extractElemFunc)
            iteratorMethodId = extractElemFunc->getMethodId();

        statement* body = stmt->getBody();
        string buffer;


        if(stmt->isForall()) { 
            // This block deals with the outer for loop which is parallelizable
            
            
            usedVariables usedVars = GetUsedVariablesInForAll(stmt);

            for(auto identifier: usedVars.getVariables()) {

                Type *type = identifier->getSymbolInfo()->getType();

                if(type->isPrimitiveType()) {

                    GenerateHipMemcpyStr(
                        "d" + CapitalizeFirstLetter(identifier->getIdentifier()),
                        "&h" + CapitalizeFirstLetter(identifier->getIdentifier()),
                        ConvertToCppType(type), "1", true
                    );
                }
            }

            main.pushString(this->function->getIdentifier()->getIdentifier());
            main.pushString("Kernel<<<numBlocks, numThreads>>>(V, E");

            if(stmt->getIsMetaUsed())
                main.pushString(", dOffsetArray");
            
            if(stmt->getIsDataUsed())
                main.pushString(", dEdgelist");

            if(stmt->getIsSrcUsed())
                main.pushString(", dSrcList");

            if(stmt->getIsWeightUsed())
                main.pushString(", dWeight");

            if(stmt->getIsRevMetaUsed())
                main.pushString(", dRevOffsetArray");

            for(auto identifier: usedVars.getVariables()) {

                if(
                    identifier->getSymbolInfo()->getType()->isPropType() ||
                    identifier->getSymbolInfo()->getType()->isPrimitiveType() 
                ) {

                    main.pushString(", d");
                    main.pushString(CapitalizeFirstLetter(identifier->getIdentifier()));
                } else {

                    HIT_CHECK
                }
            }

            if(
                nodeStack.getCurrentNode() != nullptr &&
                nodeStack.getParentNode() != nullptr &&
                nodeStack.getCurrentNode()->getTypeofNode() == NODE_FORALLSTMT &&
                nodeStack.getParentNode()->getTypeofNode() == NODE_FIXEDPTSTMT
            ) {

                usedVariables usedVars = GetUsedVariablesInFixedPoint(static_cast<fixedPointStmt*>(nodeStack.getParentNode()));

                for(auto identifier: usedVars.getVariables()) {

                    // TODO: This is added specifically to fix SSSP.
                    // This should be done such that, all varaiables of enclosing
                    // nodes and fixed point should taken together and added to
                    // the kernel call, so that no variable repeats twice.
                    if(identifier->getSymbolInfo()->getType()->isPrimitiveType()) {

                        main.pushString(", d");
                        main.pushString(CapitalizeFirstLetter(identifier->getIdentifier()));
                    }
                }
            }

            main.pushStringWithNewLine(");");
            main.pushStringWithNewLine("hipDeviceSynchronize();");

            // TODO: Add necessary cudaMemCopy calls

            for(auto iden: usedVars.getVariables()) {

                Type *type = iden->getSymbolInfo()->getType();
                if(type->isPrimitiveType()) {
                    GenerateHipMemcpyStr(
                        "&h" + CapitalizeFirstLetter(iden->getIdentifier()),
                        "d" + CapitalizeFirstLetter(iden->getIdentifier()),
                        ConvertToCppType(type), "1", false
                    );
                }
            }

            GenerateHipKernel(stmt);

        } else { 

            GenerateInnerForAll(stmt, false);

            if(stmt->hasFilterExpr()) {
                stmt->setBody(UpdateForAllBody(stmt));
            }
            if(extractElemFunc != NULL) {

                if(IsNeighbourIteration(iteratorMethodId->getIdentifier())) {

                    list<argument*> argList = extractElemFunc->getArgList();
                    Identifier *nodeNeighbour = argList.front()->getExpr()->getId();
                    const std::string src(stmt->getIterator()->getIdentifier());
                    const std::string nbr(nodeNeighbour->getIdentifier());

                    if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS) {

                        /**
                         * The below piece of code is not at all Generic. It is specific to 
                         * the BC implementation.
                        */
                       targetFile.pushStringWithNewLine("if (dD[d" + CapitalizeFirstLetter(src) + "] == -1) {");
                       targetFile.pushStringWithNewLine("dD[d" + CapitalizeFirstLetter(src) + "] = *dLevel + 1;");
                       targetFile.pushStringWithNewLine("*dIsAllNodesTraversed = false;");
                       targetFile.pushStringWithNewLine("}");
                       targetFile.NewLine();
                       targetFile.pushStringWithNewLine("if (dD[d" + CapitalizeFirstLetter(src) + "] == *dLevel + 1) {");
                       GenerateBlock(static_cast<blockStatement*>(body), false, false);
                       targetFile.pushStringWithNewLine("}");
                       targetFile.pushStringWithNewLine("}");
                        
                    } else if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {

                        /**
                         * The below piece of code is not at all Generic. It is specific to 
                         * the BC implementation.
                        */ 

                        targetFile.pushStringWithNewLine("if (dD[d" + CapitalizeFirstLetter(src) + "] == *dLevel) {");
                        GenerateBlock(static_cast<blockStatement*>(body), false, false);
                        targetFile.pushStringWithNewLine("}");
                        targetFile.pushStringWithNewLine("}");
                        targetFile.NewLine();
                        // targetFile.pushStringWithNewLine("grid.sync();");
                    } else {
                        HIT_CHECK
                        GenerateStatement(stmt->getBody(), isMainFile);
                        targetFile.pushStringWithNewLine("}");
                    }

                } else {

                    GenerateStatement(stmt->getBody(), false);
                } 

                if(stmt->isForall() && stmt->hasFilterExpr()) {
                    HIT_CHECK 
                    // GenerateFixedPointFilter(stmt->getfilterExpr(), false);
                }
           } else {

                Identifier *collectionId;

                if(sourceField)
                    collectionId = sourceField->getIdentifier1();

                if(sourceId)
                    collectionId = sourceId;

                if(collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {

                    if(body->getTypeofNode() == NODE_BLOCKSTMT) {

                        targetFile.pushStringWithNewLine("{");
                        // FIXME: TODO Why do the below not work? 
                        // targetFile.pushString(ConvertToCppType(stmt->getIterator()->getSymbolInfo()->getType()));
                        // TODO: Make this more generic instead of int
                        // targetFile.pushString("int");
                        // targetFile.AddSpace();
                        // targetFile.pushString(stmt->getIterator()->getIdentifier());
                        // targetFile.pushString(" = *itr;");
                        // targetFile.NewLine();
                        GenerateBlock(static_cast<blockStatement*>(body), false);
                        targetFile.pushStringWithNewLine("}"); 
                    } else {
                        GenerateStatement(stmt->getBody(), false);
                    }
                } else {

                    GenerateStatement(stmt->getBody(), false);
                }

                if(stmt->isForall() && stmt->hasFilterExpr()) {
                    HIT_CHECK 
                    // GenerateFixedPointFilter(stmt->getfilterExpr(), false);
                }
           }

        }
    }

    void DslCppGenerator::GenerateInnerForAll(forallStmt* stmt, bool isMainFile) {

        /**
         * FIXME: TODO The whole GenerateForAll function needs to be restructured.
        */

        dslCodePad& targetFile = isMainFile ? main : header;
        
        Identifier* iterator = stmt->getIterator();

        if(stmt->isSourceProcCall()) {

            if(IsNeighbourIteration(stmt->getExtractElementFunc()->getMethodId()->getIdentifier())) {

                list<argument*> argumentList = stmt->getExtractElementFunc()->getArgList();

                if(strcmp(stmt->getExtractElementFunc()->getMethodId()->getIdentifier(), "neighbors") == 0) {

                    assert(stmt->getExtractElementFunc()->getArgList().size() == 1);

                    // TODO: Make this generic
                    targetFile.pushStringWithNewLine(
                        "for(int edge = dOffsetArray[dV]; edge < dOffsetArray[dV + 1]; edge++) {"
                    );
                    targetFile.pushStringWithNewLine(
                        "int d" + CapitalizeFirstLetter(iterator->getIdentifier()) + " = dEdgelist[edge];" 
                    );
                    // GenerateStatement(stmt->getBody(), isMainFile);
                    // targetFile.pushStringWithNewLine("}");


                } else if(strcmp(stmt->getExtractElementFunc()->getMethodId()->getIdentifier(), "nodes_to") == 0) {

                    assert(stmt->getExtractElementFunc()->getArgList().size() == 1);
                    /**
                     * WARNING
                     * 
                     * See if the below part can be made generic. The for loop is being opened here
                     * But closed at a later point. The GenerateInnerForAll function is not closing the
                     * for loop. This is done in the GenerateForAll function. 
                     * 
                     * FIXME: TODO The whole GenerateForAll function needs to be restructured.
                    */
                    Identifier *nodeNeighbour = argumentList.front()->getExpr()->getId();
                    targetFile.pushStringWithNewLine(
                        "for(int edge = dRevOffsetArray[d" + CapitalizeFirstLetter(nodeNeighbour->getIdentifier()) + "];"
                        " edge < dRevOffsetArray[d" + CapitalizeFirstLetter(nodeNeighbour->getIdentifier()) + " + 1]; edge++) {"
                    );
                    targetFile.pushStringWithNewLine(
                        "int d" + CapitalizeFirstLetter(iterator->getIdentifier()) + " = dSrcList[edge];"
                    );
                    // GenerateStatement(stmt->getBody(), isMainFile);
                    // targetFile.pushStringWithNewLine("}");
                    
                } else {
                    HIT_CHECK
                }
            } else {

                HIT_CHECK
            }
        } else if(stmt->isSourceField()) {

            HIT_CHECK
        } else {

            Identifier *source = stmt->getSource();
            if(source != NULL) {

                switch(source->getSymbolInfo()->getType()->gettypeId()) {

                    case TYPE_SETN: {

                        // TODO: Why itr? Is this generic
                        const std::string sourceId = source->getIdentifier();
                        main.NewLine();
                        main.pushString(
                            "for(auto " + std::string(stmt->getIterator()->getIdentifier()) + ": " + sourceId + ") "
                        );
                        break;
                    }

                    default:
                        break;
                }
            }
        }
    }

    bool DslCppGenerator::IsNeighbourIteration(const std::string input) {

        return input == "neighbors" || input == "nodes_to";
    }

    void DslCppGenerator::GenerateHipKernel(forallStmt* stmt) {

        usedVariables usedVars = GetUsedVariablesInForAll(stmt);

        header.NewLine();
        header.pushStringWithNewLine("__global__");
        header.pushString("void ");
        header.pushString(this->function->getIdentifier()->getIdentifier());
        header.pushString("Kernel(int V, int E");

        if(stmt->getIsMetaUsed())
            header.pushString(", int *dOffsetArray");

        if(stmt->getIsDataUsed())
            header.pushString(", int *dEdgelist");

        if(stmt->getIsSrcUsed())
            header.pushString(", int *dSrcList");

        if(stmt->getIsWeightUsed())
            header.pushString(", int *dWeight");

        if(stmt->getIsRevMetaUsed())    
            header.pushString(", int *dRevOffsetArray");

        for(auto identifier: usedVars.getVariables()) {

            Type *type = identifier->getSymbolInfo()->getType();

            // TODO: Merge both to reduce LOCs
            if(type->isPropType()) {
                header.pushString(", ");
                header.pushString(ConvertToCppType(type));
                header.pushString(" d");
                header.pushString(CapitalizeFirstLetter(identifier->getIdentifier()));
            } else if (type->isPrimitiveType()) {
                header.pushString(", ");
                header.pushString(ConvertToCppType(type));
                header.pushString(" *d");
                header.pushString(CapitalizeFirstLetter(identifier->getIdentifier()));
                primitiveVarsInKernel.insert(identifier->getIdentifier());
            }
        }

        if(
            nodeStack.getCurrentNode() != nullptr &&
            nodeStack.getParentNode() != nullptr &&
            nodeStack.getCurrentNode()->getTypeofNode() == NODE_FORALLSTMT &&
            nodeStack.getParentNode()->getTypeofNode() == NODE_FIXEDPTSTMT
        ) {

            usedVariables usedVars = GetUsedVariablesInFixedPoint(static_cast<fixedPointStmt*>(nodeStack.getParentNode()));

            for(auto identifier: usedVars.getVariables()) {

                // TODO: This is added specifically to fix SSSP.
                // This should be done such that, all varaiables of enclosing
                // nodes and fixed point should taken together and added to
                // the kernel call, so that no variable repeats twice.
                if(identifier->getSymbolInfo()->getType()->isPrimitiveType()) {

                    header.pushString(", ");
                    header.pushString(ConvertToCppType(identifier->getSymbolInfo()->getType()));
                    header.pushString(" *d");
                    header.pushString(CapitalizeFirstLetter(identifier->getIdentifier()));
                    primitiveVarsInKernel.insert(identifier->getIdentifier());
                }
            }
        }

        header.pushStringWithNewLine(") {");
        header.NewLine();

        // TODO: Get the variable name  (now dV) from the DSL.
        // forall (v in g.nodes().filter(modified == True)) 
        header.pushStringWithNewLine(
            "unsigned d" + CapitalizeFirstLetter(stmt->getIterator()->getIdentifier()) + " = blockIdx.x * blockDim.x + threadIdx.x;"
        );
        header.NewLine();
        header.pushStringWithNewLine("if (d" + CapitalizeFirstLetter(stmt->getIterator()->getIdentifier()) +" >= V) {");
        header.pushStringWithNewLine("return;");
        header.pushStringWithNewLine("}");
        header.NewLine();

        if(stmt->hasFilterExpr()) {            
            stmt->setBody(UpdateForAllBody(stmt));
        }
        assert(stmt->getBody()->getTypeofNode() == NODE_BLOCKSTMT);
        list<statement*> statementList = static_cast<blockStatement*>(stmt->getBody())->returnStatements();

        for(auto statement: statementList)
            GenerateStatement(statement, false);

        header.pushStringWithNewLine("}");
        header.NewLine();
        primitiveVarsInKernel.clear();
    }

    blockStatement* DslCppGenerator::UpdateForAllBody(forallStmt *stmt) {

        Expression *filterExpr = stmt->getfilterExpr();
        Expression *modifiedFilterExpr = filterExpr;

        if(filterExpr->getExpressionFamily() == EXPR_RELATIONAL) {

            Expression *exprRight = filterExpr->getRight();
            Expression *exprLeft = filterExpr->getLeft();  

            if(exprLeft->isIdentifierExpr()) {

                if(
                    exprLeft->getId()->getSymbolInfo() != NULL &&
                    exprLeft->getId()->getSymbolInfo()->getType()->isPropNodeType() 
                ) {
                    PropAccess *propIdNode = (PropAccess*) Util::createPropIdNode(
                        stmt->getIterator(),
                        exprLeft->getId()
                    );

                    modifiedFilterExpr = (Expression*) Util::createNodeForRelationalExpr(
                        Expression::nodeForPropAccess(propIdNode),
                        exprRight, filterExpr->getOperatorType()
                    );
                }
            } 
        }

        ifStmt *modifiedIfStmt = (ifStmt*) Util::createNodeForIfStmt(
            modifiedFilterExpr, stmt->getBody(), NULL
        );

        blockStatement *newBlock = new blockStatement();
        newBlock->setTypeofNode(NODE_BLOCKSTMT);
        newBlock->addStmtToBlock(modifiedIfStmt);

        return newBlock;
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

        if(convergenceExpr->getExpressionFamily() == EXPR_ID) {
            dependentId = convergenceExpr->getId();
        }

        if(dependentId != NULL && dependentId->getSymbolInfo()->getType()->isPropNodeType()) {

            targetFile.pushStringWithNewLine("hipDeviceSynchronize();"); // TODO: This might not be generic
            targetFile.NewLine();
            targetFile.pushStringWithNewLine("while(!h" + CapitalizeFirstLetter(fixedPointVar) + ") {");
            targetFile.NewLine();
            targetFile.pushStringWithNewLine("h" + CapitalizeFirstLetter(fixedPointVar) + " = true;"); //TODO: Can this be generic?

            GenerateHipMemcpyStr(
                "d" + CapitalizeFirstLetter(fixedPointVar),
                "&h" + CapitalizeFirstLetter(fixedPointVar),
                fixedPointVarType, "1"
            );
            // GenerateHipMemcpySymbol(fixedPointVar, fixedPointVarType, true); //! TODO: Implement
            
            if(stmt->getBody()->getTypeofNode() != NODE_BLOCKSTMT) {
                GenerateStatement(stmt->getBody(), isMainFile);
            } else {
                GenerateBlock(static_cast<blockStatement*>(stmt->getBody()), false, isMainFile);
            }

            GenerateHipMemcpyStr(
                "&h" + CapitalizeFirstLetter(fixedPointVar),
                "d" + CapitalizeFirstLetter(fixedPointVar),
                fixedPointVarType, "1", false  
            );

            targetFile.pushStringWithNewLine("}");
        }

    }

    void DslCppGenerator::GenerateReductionCallStmt(reductionCallStmt* stmt, bool isMainFile) {


        if(stmt->is_reducCall()) {
            GenerateReductionCall(stmt, isMainFile);
        } else {
            GenerateReductionOperation(stmt, isMainFile);
        }

    }

    void DslCppGenerator::GenerateReductionCall(reductionCallStmt *stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;

        reductionCall *reductionCall = stmt->getReducCall();

        if(reductionCall->getReductionType() == REDUCE_MIN) {

            if(stmt->isListInvolved()) {

                if(stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType()) {

                    targetFile.pushString(
                        ConvertToCppType(
                            stmt->getAssignedId()->getSymbolInfo()->getType()->getInnerTargetType()
                        )
                    );
                    targetFile.AddSpace();
                }

                targetFile.pushString("dUpdated" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                targetFile.pushString(" = ");                
                GenerateExpression(
                    (*std::next(reductionCall->getargList().begin()))->getExpr(), 
                    isMainFile
                );
                targetFile.pushStringWithNewLine(";");

                //TODO: refactor. extract lists outside for readability

                list<ASTNode*> leftList = stmt->getLeftList();
                list<ASTNode*> rightList = stmt->getRightList();

                for(
                    auto itrLeft = std::next(leftList.begin()), itrRight = rightList.begin();
                    itrLeft != leftList.end(); itrLeft++, itrRight++
                ) {

                    ASTNode *node1 = *itrRight;
                    ASTNode *node2 = *itrLeft;

                    if(node2->getTypeofNode() == NODE_ID) {

                        targetFile.pushString(
                            ConvertToCppType(
                                static_cast<Identifier*>(
                                    node2
                                )->getSymbolInfo()->getType()
                            )
                        );
                        targetFile.AddSpace();
                        targetFile.pushString(
                            "dUpdated" + CapitalizeFirstLetter(static_cast<Identifier*>(node2)->getIdentifier())
                        );
                        targetFile.pushString(" = ");

                        GenerateExpression(
                            static_cast<Expression*>(node1), isMainFile
                        );
                    }

                    if(node2->getTypeofNode() == NODE_PROPACCESS) {

                        Type *type = static_cast<PropAccess*>(node2)->getIdentifier2()->getSymbolInfo()->getType();
                        if(type->isPropType()) {

                            targetFile.pushString(
                                ConvertToCppType(
                                    type->getInnerTargetType()
                                )
                            );
                            targetFile.AddSpace();
                            targetFile.pushString(
                                "dUpdated" + 
                                CapitalizeFirstLetter(std::string(static_cast<PropAccess*>(node2)->getIdentifier2()->getIdentifier()))
                            );
                            targetFile.pushString(" = ");
                            GenerateExpression(
                                static_cast<Expression*>(node1), isMainFile
                            );
                            targetFile.pushStringWithNewLine(";");
                        }
                    }
                }

                // The whole block below is specific to SSSP.
                // The DSL doesn't have an if check or comparision with
                // INF at this point in the DSL code. This needs to be
                // explored. 
                // FIXME TODO
                targetFile.pushString("if(");
                targetFile.pushString("d" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                //TODO: Get these variables from the DSL and INT_MAX as well
                //Check stmt->getIterator() to get v
                targetFile.pushString("[dV] != INT_MAX && ");
                GenerateExpressionPropId(stmt->getTargetPropId(), isMainFile);
                targetFile.pushString(" > ");
                targetFile.pushString("dUpdated" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                targetFile.pushStringWithNewLine(") {");
                
                if(stmt->isTargetId()) {
                    HIT_CHECK
                } else {
                    HIT_CHECK
                }

                // TODO: Only atomic Min? What the ..?
                // This seems specific to SSSP. This has to be made generic
                targetFile.pushString("atomicMin(&");
                GenerateExpressionPropId(stmt->getTargetPropId(), isMainFile);
                targetFile.pushString(", dUpdated" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                targetFile.pushString(");");
                targetFile.NewLine();

                ASTNode *node = nodeStack.getNearestAncestorOfType(NODE_FIXEDPTSTMT);
                if(node != NULL) {

                    usedVariables usedVars = GetUsedVariablesInFixedPoint(static_cast<fixedPointStmt*>(nodeStack.getNearestAncestorOfType(NODE_FIXEDPTSTMT)));

                    for(auto id: usedVars.getVariables()) {

                        if(id->getSymbolInfo()->getType()->isPrimitiveType()) {

                            targetFile.pushString("*d");
                            targetFile.pushString(CapitalizeFirstLetter(id->getIdentifier()));
                            targetFile.pushString(" = ");
                            targetFile.pushString(" false;");
                            targetFile.NewLine();
                        }
                    }
                }
                

                for(auto itr = std::next(leftList.begin()); itr != leftList.end(); itr++) {

                    ASTNode *node = *itr;
                    Identifier *id = NULL;

                    if(node->getTypeofNode() == NODE_ID) {
                        GenerateExpressionIdentifier(
                            static_cast<Identifier*>(node), isMainFile
                        );
                    }

                    if(node->getTypeofNode() == NODE_PROPACCESS) {
                        GenerateExpressionPropId(
                            static_cast<PropAccess*>(node), isMainFile
                        );
                    }

                    targetFile.pushString(" = ");
                    targetFile.pushString("dUpdated");
                    
                    if(node->getTypeofNode() == NODE_ID) {
                        id = static_cast<Identifier*>(node);
                        GenerateExpressionIdentifier(id, isMainFile, false);
                    }

                    if(node->getTypeofNode() == NODE_PROPACCESS) {
                        id = static_cast<PropAccess*>(node)->getIdentifier2();
                        GenerateExpressionIdentifier(id, isMainFile, false);
                    }

                    targetFile.pushStringWithNewLine(";");
                }
                targetFile.pushStringWithNewLine("}");
            }
        }
    }

    void DslCppGenerator::GenerateReductionOperation(reductionCallStmt *stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;

        if(stmt->isLeftIdentifier()) {

            GenerateAtomicStatementFromReductionOp(stmt, isMainFile);
        } else {

            HIT_CHECK

        }
    }

    void DslCppGenerator::GenerateItrBfs(iterateBFS* stmt, bool isMainFile) {

        assert(stmt->getBody()->getTypeofNode() == NODE_BLOCKSTMT);
        GenerateAdditionalVariablesAndInitializeForBfs(stmt);

        blockStatement *blockBfs = static_cast<blockStatement*>(stmt->getBody());
        blockStatement *blockRbfs = static_cast<blockStatement*>(stmt->getRBFS()->getBody());
        
        // TODO: None of this is generic. 
        main.pushStringWithNewLine("do {");
        main.NewLine();
        GenerateItrBfsBody(blockBfs);
        main.pushStringWithNewLine("} while(!hIsAllNodesTraversed);");

        main.pushStringWithNewLine("hLevel--;");
        GenerateHipMemcpyStr("dLevel", "&hLevel", "int", "1", true);
        main.NewLine();

        main.pushStringWithNewLine("while(hLevel > 0) {");
        main.NewLine();
        GenerateItrRevBfsBody(blockRbfs);
        main.pushStringWithNewLine("}");

        GenerateForwardBfsKernel(blockBfs); 
        GenerateReverseBfsKernel(blockRbfs);
    }

    void DslCppGenerator::GenerateItrBfsBody(blockStatement *stmt) {

        /**
         * The below kernel call/memcpy can be removed if we use hipHostMalloc
         * TODO: Use Kernel Call if it is better. Check with Rupesh/Test it out
         * TODO: Change to hipHostMalloc
        */
        main.pushStringWithNewLine("hIsAllNodesTraversed = true;");
        GenerateHipMemcpyStr("dIsAllNodesTraversed", "&hIsAllNodesTraversed", "bool", "1", true);
        GenerateHipMemcpyStr("dLevel", "&hLevel", "int", "1", true);
        main.pushStringWithNewLine("ForwardBfsKernel<<<numBlocks, numThreads>>>(");
        main.pushString("V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed");

        GeneratePropParamsAsFormalParams(false, true);
        GenerateUsedVarsInBlockAsFormalParams(stmt, false, true);
        main.NewLine();
        main.pushStringWithNewLine(");");
        main.pushStringWithNewLine("hLevel++;");
        // GenerateHipMemcpyStr("&hLevel", "dLevel", "int", "1", false);

        main.NewLine();
        GenerateHipMemcpyStr("&hIsAllNodesTraversed", "dIsAllNodesTraversed", "bool", "1", false); 
    }

    void DslCppGenerator::GenerateItrRevBfsBody(blockStatement *stmt) {
        main.pushStringWithNewLine("ReverseBfsKernel<<<numBlocks, numThreads>>>(");
        main.pushString("V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed");

        GeneratePropParamsAsFormalParams(false, true); 
        GenerateUsedVarsInBlockAsFormalParams(stmt, false, true);   
        main.NewLine();
        main.pushStringWithNewLine(");");
        main.pushStringWithNewLine("ReverseBfsKernelAddition<<<numBlocks, numThreads>>>(");
        main.pushString("V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed");

        GeneratePropParamsAsFormalParams(false, true); 
        GenerateUsedVarsInBlockAsFormalParams(stmt, false, true);   
        main.NewLine();
        main.pushStringWithNewLine(");");
        main.pushStringWithNewLine("hLevel--;");
        GenerateHipMemcpyStr("dLevel", "&hLevel", "int", "1", true);
    }

    void DslCppGenerator::GenerateForwardBfsKernel(blockStatement *body) {

        const std::string loopVar("v");

        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        list<statement*> stmtList = body->returnStatements();

        header.pushStringWithNewLine("__global__ \nvoid ForwardBfsKernel(");
        header.pushString(
            "int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed"
        );
        GeneratePropParamsAsFormalParams(true, false); 
        GenerateUsedVarsInBlockAsFormalParams(body, true, false);
        header.NewLine();
        header.pushStringWithNewLine(") {");
        header.NewLine();
        header.pushStringWithNewLine(
            "unsigned d" + CapitalizeFirstLetter(loopVar) + " = threadIdx.x + blockIdx.x * blockDim.x;"
        );

        header.NewLine();
        header.pushStringWithNewLine("if(d" + CapitalizeFirstLetter(loopVar) + " >= V) {");
        header.pushStringWithNewLine("return;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("if(dD[d" + CapitalizeFirstLetter(loopVar) + "] == *dLevel) {");
        header.NewLine();

        for(auto stmt: stmtList) {
            GenerateStatement(stmt, false);
        }

        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
        header.NewLine();
    }

    void DslCppGenerator::GenerateReverseBfsKernel(blockStatement *body) {

        const std::string loopVar("v"); //TODO Make generic
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        list<statement*> stmtList = body->returnStatements();

        header.pushStringWithNewLine("__global__ \nvoid ReverseBfsKernel(");
        header.pushString(
            "int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed"
        );
        GeneratePropParamsAsFormalParams(true, false);
        GenerateUsedVarsInBlockAsFormalParams(body, true, false);
        header.NewLine();
        header.pushStringWithNewLine(") {");
        header.NewLine();
        header.pushStringWithNewLine(
            "unsigned d" + CapitalizeFirstLetter(loopVar) + " = threadIdx.x + blockIdx.x * blockDim.x;"
        );
        header.NewLine();
        header.pushStringWithNewLine("if(d" + CapitalizeFirstLetter(loopVar) + " >= V) {"); 
        header.pushStringWithNewLine("return;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("if(dD[d" + CapitalizeFirstLetter(loopVar) + "] == *dLevel - 1) {");
        
        /**
         * FIXME: the below fix is temporary. 
         * 
         * This should be somehow fixed. Currently since HIP doesn't support
         * grid.sync() we are using a workaround. The updation to bc should 
         * happen in another kernel(for global sync) Since that assignment is in a 
         * NODE_ASSIGN we are handling that seperately. This is not generic and 
         * needs to be fixed.
        */
        for(auto stmt: stmtList)
            if(stmt->getTypeofNode() != NODE_ASSIGN)   
                GenerateStatement(stmt, false);

        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
        header.NewLine();

        header.pushStringWithNewLine("__global__ \nvoid ReverseBfsKernelAddition(");
        header.pushString(
            "int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed"
        );
        GeneratePropParamsAsFormalParams(true, false);
        GenerateUsedVarsInBlockAsFormalParams(body, true, false);
        header.NewLine();
        header.pushStringWithNewLine(") {");
        header.NewLine();
        header.pushStringWithNewLine(
            "unsigned d" + CapitalizeFirstLetter(loopVar) + " = threadIdx.x + blockIdx.x * blockDim.x;"
        );
        header.NewLine();
        header.pushStringWithNewLine("if(d" + CapitalizeFirstLetter(loopVar) + " >= V) {"); 
        header.pushStringWithNewLine("return;");
        header.pushStringWithNewLine("}");
        header.NewLine();

        header.pushStringWithNewLine("if(dD[d" + CapitalizeFirstLetter(loopVar) + "] == *dLevel - 1) {");

        for(auto stmt: stmtList)
            if(stmt->getTypeofNode() == NODE_ASSIGN)   
                GenerateStatement(stmt, false);

        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
        header.NewLine();
    }

    void DslCppGenerator::GeneratePropParamsAsFormalParams(bool isFunctionDefinition, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main : header;

        list<formalParam*> paramList = function->getParamList();

        for(auto itr = paramList.begin(); itr != paramList.end(); itr++) {

            Type *type = (*itr)->getType();

            if(type->isPropType()) {

                if(type->getInnerTargetType()->isPrimitiveType()) {

                    std::string var("d" + CapitalizeFirstLetter((*itr)->getIdentifier()->getIdentifier()));
                    if(isFunctionDefinition) {
                        targetFile.pushString(", ");
                        targetFile.pushString(ConvertToCppType(type->getInnerTargetType()));
                        targetFile.AddSpace();
                        targetFile.pushString("*");
                        targetFile.pushString(var);
                    } else {
                        targetFile.pushString(", ");
                        targetFile.pushString(var);
                    }
                } else {

                    HIT_CHECK
                }
            } else {

                HIT_CHECK
            }
        }
    }

    void DslCppGenerator::GenerateUsedVarsInBlockAsFormalParams(
        blockStatement *stmt, bool isFunctionDefinition, bool isMainFile
    ) {

        dslCodePad &targetFile = isMainFile ? main : header;
        usedVariables usedVars = GetUsedVariablesInBlock(stmt); 

        /**
         * NOTE:
         * 
         * Below section and the first for loop is used to find out
         * the variable which are prop variables. If it is a prop variable
         * then we ignore it as it will be handled by the GeneratePropParamsAsFormalParams
        */
        std::unordered_set<std::string> propVars;
        list<formalParam*> paramList = function->getParamList();
        for(auto itr = paramList.begin(); itr != paramList.end(); itr++) {
            propVars.insert((*itr)->getIdentifier()->getIdentifier());
        }

        for(auto identifier: usedVars.getVariables()) {

            if(propVars.find(identifier->getIdentifier()) != propVars.end()) {
                continue;
            }

            Type *type = identifier->getSymbolInfo()->getType();

            if(type->isPropType()) {

                if(type->getInnerTargetType()->isPrimitiveType()) {

                    std::string var("d" + CapitalizeFirstLetter(identifier->getIdentifier()));
                    if(isFunctionDefinition) {
                        targetFile.pushString(", ");
                        targetFile.pushString(ConvertToCppType(type->getInnerTargetType()));
                        targetFile.AddSpace();
                        targetFile.pushString("*");
                        targetFile.pushString(var);
                    } else {
                        targetFile.pushString(", ");
                        targetFile.pushString(var);
                    }
                } else {
                    HIT_CHECK
                }
            } else {

                HIT_CHECK
            }
        }
    }

    void DslCppGenerator::GenerateItrRevBfs(iterateReverseBFS* stmt, bool isMainFile) {

        throw std::runtime_error("GenerateItrRevBfs not implemented");
    }

    void DslCppGenerator::GenerateAdditionalVariablesAndInitializeForBfs(iterateBFS* stmt) {

        main.NewLine();
        main.pushStringWithNewLine("// Additional variables added for BC. Not present in the DSL");
        main.pushStringWithNewLine("bool hIsAllNodesTraversed = false; /*Non-DSL variable*/"); // TODO: Use hipHostMalloc instead of two vars
        main.pushStringWithNewLine("bool *dIsAllNodesTraversed; /*Non-DSL variable*/");
        main.pushStringWithNewLine("int hLevel = 0; /*Non-DSL variable*/ // Indicates the level of the BFS"); // TODO: Remove the comment from buffer 
        main.pushStringWithNewLine("int *dLevel; /*Non-DSL variable*/");
        main.pushStringWithNewLine("// Explore the possibility of using hipHostMalloc for simple vars");
        main.pushStringWithNewLine("// Check if hipHostMalloc works on all platforms");
        // TODO: Usage of hipHostMalloc to be explored for simple vars. Support on all platforms?
        main.pushStringWithNewLine("int *dD; /*Non-DSL variable*/");

        GenerateHipMallocStr("dIsAllNodesTraversed", "bool", "1");
        GenerateHipMallocStr("dLevel", "int", "1");
        GenerateHipMallocStr("dD", "int", "V");
        GenerateInitIndexString("int", "dLevel", "0", "0");
        GenerateInitArrayString("int", "dD", "-1");
        GenerateInitIndexString("int", "dD", "0", std::string(stmt->getRootNode()->getIdentifier()));
        main.NewLine();
    }

    void DslCppGenerator::GenerateProcCallStmt(proc_callStmt* stmt, bool isMainFile) {

        proc_callExpr *proc = stmt->getProcCallExpr();
        string methodId = proc->getMethodId()->getIdentifier();
        string nodeCall = "attachNodeProperty";
        string edgeCall = "attachEdgeProperty";

        if(methodId.compare(nodeCall) == 0) {

            list<argument*> argList = proc->getArgList();
            
            for(auto itr =  argList.begin(); itr != argList.end(); itr++) {
                GenerateInitKernelCall((*itr)->getAssignExpr(), isMainFile);
            }

        } else if(methodId.compare(edgeCall) == 0) {

            HIT_CHECK

        } else {

            HIT_CHECK

        }
        
    }

    void DslCppGenerator::GenerateWhileStmt(whileStmt *stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        
        targetFile.pushString("while(");
        GenerateExpression(stmt->getCondition(), isMainFile);
        targetFile.pushStringWithNewLine(") {");
        GenerateStatement(stmt->getBody(), isMainFile);
        targetFile.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateDoWhile(dowhileStmt* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;

        targetFile.pushStringWithNewLine("do {");
        GenerateStatement(stmt->getBody(), isMainFile);
        targetFile.pushString("} while(");
        GenerateExpression(stmt->getCondition(), isMainFile);
        targetFile.pushStringWithNewLine(");");
    }

    void DslCppGenerator::GenerateUnaryStmt(unary_stmt* stmt, bool isMainFile) {

        GenerateExpression(stmt->getUnaryExpr(), isMainFile);
        (isMainFile ? main : header).pushStringWithNewLine(";"); // FIXME: TODO Shouldn't this be to main or header?
    }        
    
    void DslCppGenerator::GenerateExpression(Expression *expr, bool isMainFile, bool shouldPrefix, bool isAtomic) {


        // (isMainFile ? main : header).pushStringWithNewLine("\n/* Expression */");
        // TODO: Remove
        if(debug){
        if (expr->isLiteral()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Literal */");
        } else if (expr->isInfinity()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Infinity */");
        } else if (expr->isIdentifierExpr()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Identifier Expression */");
        } else if (expr->isPropIdExpr()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Property ID Expression */");
        } else if (expr->isArithmetic() || expr->isLogical()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Arithmetic or Logical Expression */");
        } else if (expr->isRelational()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Relational Expression */");
        } else if (expr->isProcCallExpr()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Procedure Call Expression */");
        } else if (expr->isUnary()) {
            (isMainFile ? main : header).pushStringWithNewLine("/* Condition: Unary Expression */");
        } else {
            assert(false);
        }}

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

    void DslCppGenerator::GenerateExpressionIdentifier(Identifier* id, bool isMainFile, bool shouldPrefix) {

        

        // This is not a generic solution. Please check the comment in 
        // GenerateExpressionArithmeticOrLogical for more details.

        if(primitiveVarsInKernel.find(id->getIdentifier()) != primitiveVarsInKernel.end()) {
            // (isMainFile ? main : header).pushString(CapitalizeFirstLetter(id->getIdentifier()));
            header.pushString("(*d" + CapitalizeFirstLetter(id->getIdentifier()) + ")");
            return;
        }

        const std::string prefix(shouldPrefix ? (isMainFile ? "h" : "d") : "");
        (isMainFile ? main : header).pushString(prefix + CapitalizeFirstLetter(id->getIdentifier()));


        // bool shouldDereference = false;
        
        // if(id->getSymbolInfo() != NULL)
        //     shouldDereference = id->getSymbolInfo()->getType()->isPrimitiveType() &&  isCurrentExpressionArithmeticOrLogical;
        // // This is just bad coding.
        // const std::string prefix1 = shouldPrefix ? (isMainFile ? "h" : (shouldDereference ? "*d" : "d")) : "";
        // // std::string prefix = isMainFile ? "h" : "d";
        // (isMainFile ? main : header).pushString(prefix1 + CapitalizeFirstLetter(id->getIdentifier()));
    }

    void DslCppGenerator::GenerateExpressionPropId(PropAccess* propId, bool isMainFile) {

        std::string value;
        Identifier *id1 = propId->getIdentifier1();
        Identifier *id2 = propId->getIdentifier2();


        const std::string prefix(isMainFile ? "h" : "d");
        value = prefix + CapitalizeFirstLetter(id2->getIdentifier()) 
        + "[" + prefix + CapitalizeFirstLetter(id1->getIdentifier()) + "]";
    
        (isMainFile ? main : header).pushString(value);
    }

    void DslCppGenerator::GenerateExpressionArithmeticOrLogical(
        Expression* expr, bool isMainFile, bool isAtomic
    ) {

        dslCodePad &target = isMainFile ? main : header;
        /**
         * FIXME: TODO: The below approach is not good. This has been added
         * to avoid the issue of not dereferencing the pointer. This 
         * boolean is checked while generating Identifiers. This will go 
         * wrong when Relational Expressions use passed parameters in
         * device code. The underlying problem is that we are passing all
         * variables as pointers into the kernel.
         * The solution is to use something like hipHostMalloc, this way the 
         * primitives can be used as primitives and not as pointers.
         * 
         * This can go wrong in Arithmetic ops itself if we have the variable i
         * local and we still append * to it. This is a hacky solution.
         */ 
        isCurrentExpressionArithmeticOrLogical = true;

        if(expr->hasEnclosedBrackets())
            target.pushString("(");

        if(!isAtomic) {
            GenerateExpression(expr->getLeft(), isMainFile);
            target.AddSpace();
        }

        const std::string operatorString = GetOperatorString(expr->getOperatorType());

        if(!isAtomic) {
            target.pushString(operatorString);
            target.AddSpace();
        }

        GenerateExpression(expr->getRight(), isMainFile);

        if(expr->hasEnclosedBrackets())
            target.pushString(")");
        isCurrentExpressionArithmeticOrLogical = false;
    }

    void DslCppGenerator::GenerateExpressionRelational(Expression* expr, bool isMainFile) {

        dslCodePad & target = isMainFile ? main : header;

        if(expr->hasEnclosedBrackets())
            target.pushString("(");

        GenerateExpression(expr->getLeft(), isMainFile, true);
        target.AddSpace();
        const string op = GetOperatorString(expr->getOperatorType());
        target.pushString(op);
        target.AddSpace();
        GenerateExpression(expr->getRight(), isMainFile, true);

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

            /*dOffsetArray will only be accessed on device. Hence prefixing d to Identifier*/
            strBuffer = string("(dOffsetArray[d") + CapitalizeFirstLetter(nodeId->getIdentifier()) + " + 1] - " +
                         "dOffsetArray[d" + CapitalizeFirstLetter(nodeId->getIdentifier()) + "])";

            targetFile.pushString(strBuffer);

        } else if (methodId == "is_an_edge") {

            assert(expr->getArgList().size() == 2);
            Identifier *src = expr->getArgList().front()->getExpr()->getId();
            Identifier *dest = expr->getArgList().back()->getExpr()->getId();
            targetFile.pushString(
                "IsAnEdge(d" + CapitalizeFirstLetter(src->getIdentifier())
                + ", d" + CapitalizeFirstLetter(dest->getIdentifier()) 
                + ", dOffsetArray, dEdgelist)"
            );

            generateIsAnEdgeFunction = true;
        } else if(methodId == "num_nodes") {

            if(expr->getArgList().size() == 0) {

                targetFile.pushString(expr->getId1()->getIdentifier());
                targetFile.pushString(".");
                targetFile.pushString(expr->getMethodId()->getIdentifier());
                targetFile.pushString("()");
            } else {
                HIT_CHECK
            }
        } else {
            HIT_CHECK
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

        main.pushStringWithNewLine(
            hVar + " = (" + typeStr + "*) malloc(sizeof(" + typeStr + ")" +
            " * (" + sizeOfType + "));"
        );
    }

    /**
     * Simple functions to reduce cognitive complexity
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

    void DslCppGenerator::CheckAndGenerateHipMalloc(Function *function) {

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
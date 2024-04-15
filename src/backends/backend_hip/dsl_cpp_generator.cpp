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

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

#define HIT_CHECK std::cout << "UNIMPL Hit at line " << __LINE__ << " of function " << __func__ << " in file " << __FILE__ << "\n";

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

        GenerateFunctionHeader(func, false);
        GenerateFunctionHeader(func, true);

        GenerateTimerStart(); // TODO: Make this flag dependent

        GenerateFunctionBody(func);
        main.NewLine();
        GenerateBlock(func->getBlockStatement(), false);
        GenerateHipMemcpyParams(func->getParamList()); // TODO: Check if this is required
        
        GenerateTimerStop();
        main.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateFunctionHeader(Function* func, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main:header;

        targetFile.pushString("void");
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

            if(type->isPropType()) {
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

        // TODO" The below code will generate all variables
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
            return "int";  // TODO: need to be modified. Maybe other types as well?

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

        cout << "STMT " << stmt->getTypeofNode() << "\n";

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
                cout << "STMT " << (asst->isDeviceAssignment() ? "DDDDD\n" : "NNNNN\n");
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
            HIT_CHECK
            // GenerateItrRevBfs(static_cast<iterateReverseBFS*>(stmt), isMainFile);
            break;

        case NODE_PROCCALLSTMT:
            GenerateProcCallStmt(static_cast<proc_callStmt*>(stmt), isMainFile);
            break;

        case NODE_WHILESTMT:
            // TODO: Make something for this?
            HIT_CHECK
            break;

        case NODE_DOWHILESTMT:
            GenerateDoWhile(static_cast<dowhileStmt*>(stmt), isMainFile);
            break;

        case NODE_UNARYSTMT:
            GenerateUnaryStmt(static_cast<unary_stmt*>(stmt), isMainFile);   
            break;
        
        case NODE_RETURN:
            break; //TODO: No implementation but gets called;

        default:
            throw std::runtime_error("Generation function not implemented for NODETYPE " + std::to_string(stmt->getTypeofNode()));
            break;
        }
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
            targetFile.pushString(stmt->getdeclId()->getIdentifier());

            if(stmt->isInitialized()) {

                targetFile.pushString(" = ");
                GenerateExpression(stmt->getExpressionAssigned(), isMainFile); // TODO
                targetFile.pushStringWithNewLine(";");
            }
        
        } else if(type->isPrimitiveType()) {

            std::string strBuffer;

            const std::string varType(ConvertToCppType(type));
            const std::string varName(stmt->getdeclId()->getIdentifier());
            targetFile.pushString(varType);
            targetFile.AddSpace();
            targetFile.pushString("h");
            targetFile.pushString(CapitalizeFirstLetter(varName));
            if(stmt->isInitialized()) {

                targetFile.pushString(" = ");

                if (stmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
                    cout << varType << " UNIMPL " << varName << "\n";
                    HIT_CHECK
                    // TODO: Is cast required here? How to make this more general?
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
                HIT_CHECK
                // cout << "UNIMPL " + string(stmt->getId()->getIdentifier()) << "\n";
                targetFile.pushString(stmt->getId()->getIdentifier());
            }
        } else if(stmt->lhs_isProp()) {

            PropAccess *propId = stmt->getPropId();
            if(stmt->getAtomicSignal()) {

                targetFile.pushString("atomicAdd(&d");
                isAtomic = true;                
            }

            if(stmt->isAccumulateKernel()) {
                isResult = true;
            }

            targetFile.pushString(CapitalizeFirstLetter(propId->getIdentifier2()->getIdentifier()));
            targetFile.pushString("[");
            targetFile.pushString(propId->getIdentifier1()->getIdentifier());
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
        Identifier* iteratorMethodId;

        if(extractElemFunc)
            iteratorMethodId = extractElemFunc->getMethodId();

        statement* body = stmt->getBody();
        string buffer;

        if(stmt->isForall()) { 
            // This deals with the outer for loop which is parallelizable
            
            usedVariables usedVars = GetUsedVariablesInForAll(stmt);

            for(auto identifier: usedVars.getVariables()) {

                Type *type = identifier->getSymbolInfo()->getType();

                if(type->isPrimitiveType()) {

                    GenerateHipMemcpyStr(
                        "d" + CapitalizeFirstLetter(identifier->getIdentifier()),
                        "h" + CapitalizeFirstLetter(identifier->getIdentifier()),
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

                if(identifier->getSymbolInfo()->getType()->isPropType()) {

                    main.pushString(", d");
                    std::string temp = identifier->getIdentifier();
                    temp[0] = toupper(temp[0]);
                    main.pushString(temp);
                }
            }

            for(auto identifier: stmt->getUsedVariables()) {

                // Either the above for block or this for block is required. I think!
                // Look into it.
                //TODO: Implement
                //! IMPORTANT
                HIT_CHECK
            }

            main.pushStringWithNewLine(");");
            main.pushStringWithNewLine("hipDeviceSynchronize();");

            // TODO: Add necessary cudaMemCopy calls

            for(auto iden: usedVars.getVariables()) {

                Type *type = iden->getSymbolInfo()->getType();
                
                if(type->isPrimitiveType()) {
                    GenerateHipMemcpyStr(
                        "h" + CapitalizeFirstLetter(iden->getIdentifier()),
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
                       targetFile.pushStringWithNewLine("if (dD[" + src + "] == -1) {");
                       targetFile.pushStringWithNewLine("dD[" + src + "] = *dLevel + 1;");
                       targetFile.pushStringWithNewLine("*dIsAllNodesTraversed = false;");
                       targetFile.pushStringWithNewLine("}");
                       targetFile.NewLine();
                       targetFile.pushStringWithNewLine("if (dD[" + src + "] == *dLevel + 1) {");
                       GenerateBlock(static_cast<blockStatement*>(body), false, false);
                       targetFile.pushStringWithNewLine("}");
                       targetFile.pushStringWithNewLine("}");
                        
                    } else if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {

                        /**
                         * The below piece of code is not at all Generic. It is specific to 
                         * the BC implementation.
                        */ 

                        targetFile.pushStringWithNewLine("if (dD[" + src + "] == *dLevel) {");
                        GenerateBlock(static_cast<blockStatement*>(body), false, false);
                        targetFile.pushStringWithNewLine("}");
                        targetFile.pushStringWithNewLine("}");
                        targetFile.NewLine();
                        targetFile.pushStringWithNewLine("grid.sync();");
                        cout << "STMT XXXXXXX\n";
                    } else {
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
                        // FIXME: Why do the below not work? 
                        // targetFile.pushString(ConvertToCppType(stmt->getIterator()->getSymbolInfo()->getType()));
                        // TODO: Make this more generic instead of int
                        targetFile.pushString("int");
                        targetFile.AddSpace();
                        targetFile.pushString(stmt->getIterator()->getIdentifier());
                        targetFile.pushString(" = *itr;");
                        targetFile.NewLine();
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

        dslCodePad& targetFile = isMainFile ? main : header;
        
        Identifier* iterator = stmt->getIterator();

        if(stmt->isSourceProcCall()) {

            if(false) {


                HIT_CHECK
            } else if(IsNeighbourIteration(stmt->getExtractElementFunc()->getMethodId()->getIdentifier())) {

                if(strcmp(stmt->getExtractElementFunc()->getMethodId()->getIdentifier(), "neighbors") == 0) {

                    assert(stmt->getExtractElementFunc()->getArgList().size() == 1);

                    // TODO: Make this generic
                    targetFile.pushStringWithNewLine(
                        "for(int edge = dOffsetArray[v]; edge < dOffsetArray[v + 1]; edge++) {"
                    );
                    targetFile.pushStringWithNewLine(
                        "int " + std::string(iterator->getIdentifier()) + " = dEdgelist[edge];" 
                    );
                }
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
                        main.pushStringWithNewLine(
                            "for(auto itr = " + sourceId + ".begin(); itr != " 
                            + sourceId + ".end(); itr++) "
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

            if(type->isPropType()) {

                header.pushString(", ");
                header.pushString(ConvertToCppType(type));
                header.pushString(" d");
                std::string temp = identifier->getIdentifier();
                temp[0] = toupper(temp[0]);
                header.pushString(temp);
            }
        }

        for(auto identifier: stmt->getUsedVariables()) {
            // ! Probably not required
            //TODO: Implement
            //! IMPORTANT
            HIT_CHECK
        }

        header.pushStringWithNewLine(") {");
        header.NewLine();

        // TODO: Get the varaible name from the DSL.
        // forall (v in g.nodes().filter(modified == True)) 
        header.pushStringWithNewLine("unsigned v = threadIdx.x + blockIdx.x * blockDim.x;");
        header.NewLine();
        header.pushStringWithNewLine("if (v >= V) {");
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

            targetFile.pushStringWithNewLine("int k = 0;"); // TODO: Is this required?
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

            // GenerateHipMemcpySymbol(fixedPointVar, fixedPointVarType, false); //! TODO: Implement
            // GenerateHipMemcpyStr(modifiedVar, modifiedVarNext, fixedPointVarType, "V", false); //! TODO: Implement
            
            // targetFile.pushStringWithNewLine(
            //     "initKernel<" + fixedPointVarType + "><<<numBlocks, numThreads>>>(V, " + modifiedVarNext + ", false);"
            // );

            targetFile.pushStringWithNewLine("k++;"); // TODO: Is this required?
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

                std::string temp = stmt->getAssignedId()->getIdentifier();
                temp[0] = toupper(temp[0]);
                targetFile.pushString("updated" + temp);
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
                            "updated" + std::string(static_cast<Identifier*>(node2)->getIdentifier())
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
                                "updated" + 
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

                targetFile.pushString("if(");
                targetFile.pushString("d" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                //TODO: Get these variables from the DSL and INT_MAX as well
                targetFile.pushString("[v] != INT_MAX && ");
                GenerateExpressionPropId(stmt->getTargetPropId(), isMainFile);
                targetFile.pushString(" > ");
                targetFile.pushString("updated" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                targetFile.pushStringWithNewLine(") {");
                
                if(stmt->isTargetId()) {
                    HIT_CHECK
                } else {
                    HIT_CHECK
                }

                targetFile.pushString("atomicMin(&");
                GenerateExpressionPropId(stmt->getTargetPropId(), isMainFile);
                targetFile.pushString(", updated" + CapitalizeFirstLetter(stmt->getAssignedId()->getIdentifier()));
                targetFile.pushString(");");
                targetFile.NewLine();

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
                    targetFile.pushString("updated");
                    
                    if(node->getTypeofNode() == NODE_ID) {
                        id = static_cast<Identifier*>(node);
                        GenerateExpressionIdentifier(id, isMainFile);
                    }

                    if(node->getTypeofNode() == NODE_PROPACCESS) {
                        id = static_cast<PropAccess*>(node)->getIdentifier2();
                        GenerateExpressionIdentifier(id, isMainFile);
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

        GenerateForwardBfsKernel(blockBfs); //! TODO
        GenerateReverseBfsKernel(blockRbfs); //! TODO
    }

    void DslCppGenerator::GenerateItrBfsBody(blockStatement *stmt) {

        main.pushStringWithNewLine("hIsAllNodesTraversed = true;");
        GenerateHipMemcpyStr("dIsAllNodesTraversed", "&hIsAllNodesTraversed", "bool", "1", true);
        main.pushStringWithNewLine("ForwardBfsKernel<<<numBlocks, numThreads>>>(");
        main.pushString("V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed");

        GeneratePropParamsAsFormalParams(function->getParamList(), true, false); //! TODO
        main.NewLine();
        main.pushStringWithNewLine(");");

        /**
         * The below kernel call/memcpy can be removed if we use hipHostMalloc
         * TODO: Use Kernel Call if it is better. Check with Rupesh/Test it out
         * TODO: Change to hipHostMalloc
        */
        main.pushStringWithNewLine("hLevel++;");
        GenerateHipMemcpyStr("&hLevel", "dLevel", "int", "1", false);

        main.NewLine();
        GenerateHipMemcpyStr("&hIsAllNodesTraversed", "dIsAllNodesTraversed", "bool", "1", false); 
    }

    void DslCppGenerator::GenerateItrRevBfsBody(blockStatement *stmt) {
        main.pushStringWithNewLine("ReverseBfsKernel<<<numBlocks, numThreads>>>(");
        main.pushString("V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed");

        GeneratePropParamsAsFormalParams(function->getParamList(), true, false); //! TODO
        main.NewLine();
        main.pushStringWithNewLine(");");
        main.pushStringWithNewLine("hLevel--;");
        GenerateHipMemcpyStr("dLevel", "&hLevel", "int", "1", true);
    }

    void DslCppGenerator::GenerateForwardBfsKernel(blockStatement *body) {

        const std::string loopVar("v");

        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        list<statement*> stmtList = body->returnStatements();

        header.pushStringWithNewLine("__global__ void ForwardBfsKernel(");
        header.pushString(
            "int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed"
        );
        GeneratePropParamsAsFormalParams(function->getParamList(), true, false); 
        header.NewLine();
        header.pushStringWithNewLine(") {");
        header.NewLine();
        header.pushStringWithNewLine(
            "unsigned " + loopVar + " = threadIdx.x + blockIdx.x * blockDim.x;"
        );

        header.NewLine();
        header.pushStringWithNewLine("if(" + loopVar + " >= V) {");
        header.pushStringWithNewLine("return;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("if(dD[" + loopVar + "] == dLevel) {");
        header.NewLine();

        for(auto stmt: stmtList) {
            GenerateStatement(stmt, false);
        }

        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateReverseBfsKernel(blockStatement *body) {

        const std::string loopVar("v");
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        list<statement*> stmtList = body->returnStatements();

        header.pushStringWithNewLine("__global__ void ReverseBfsKernel(");
        header.pushString(
            "int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed"
        );
        header.NewLine();
        GeneratePropParamsAsFormalParams(function->getParamList(), true, false);
        header.pushStringWithNewLine(") {");
        header.NewLine();
        header.pushStringWithNewLine(
            "unsigned " + loopVar + " = threadIdx.x + blockIdx.x * blockDim.x;"
        );
        header.NewLine();
        header.pushStringWithNewLine("if(" + loopVar + " >= V) {"); 
        header.pushStringWithNewLine("return;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("auto grid = cooperative_groups::this_grid();");
        header.NewLine();
        header.pushStringWithNewLine("if(dD[" + loopVar + "] == dLevel - 1) {");
        
        for(auto stmt: stmtList)
            GenerateStatement(stmt, false);

        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
        header.NewLine();
    }

    void DslCppGenerator::GeneratePropParamsAsFormalParams(
        list<formalParam*> params, bool isTypeNeeded, bool isMainFile
    ) {
        // throw std::runtime_error("GeneratePropParamsAsFormalParams not implemented");
    }

    void DslCppGenerator::GenerateItrRevBfs(iterateReverseBFS* stmt, bool isMainFile) {

        throw std::runtime_error("GenerateItrRevBfs not implemented");
    }

    void DslCppGenerator::GenerateAdditionalVariablesAndInitializeForBfs(iterateBFS* stmt) {

        main.NewLine();
        main.pushStringWithNewLine("// Additional variables added for BC. Not present in the DSL");
        main.pushStringWithNewLine("bool hIsAllNodesTraversed; /*Non-DSL variable*/"); // TODO: Use hipHostMalloc instead of two vars
        main.pushStringWithNewLine("bool *dIsAllNodesTraversed; /*Non-DSL variable*/");
        main.pushStringWithNewLine("int hLevel; /*Non-DSL variable*/ // Indicates the level of the BFS"); // TODO: Remove the comment from buffer 
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
        main.pushStringWithNewLine(";");
    }        
    
    void DslCppGenerator::GenerateExpression(Expression *expr, bool isMainFile, bool isNotToUpper, bool isAtomic) {

        if (expr->isLiteral()) {
            std::cout << "GenerateExpressionLiteral" << std::endl;
        } else if (expr->isInfinity()) {
            std::cout << "GenerateExpressionInfinity" << std::endl;
        } else if (expr->isIdentifierExpr()) {
            std::cout << "GenerateExpressionIdentifier" << std::endl;
        } else if (expr->isPropIdExpr()) {
            std::cout << "GenerateExpressionPropId" << std::endl;
        } else if (expr->isArithmetic() || expr->isLogical()) {
            std::cout << "GenerateExpressionArithmeticOrLogical" << std::endl;
        } else if (expr->isRelational()) {
            std::cout << "GenerateExpressionRelational" << std::endl;
        } else if (expr->isProcCallExpr()) {
            std::cout << "GenerateExpressionProcCallExpression" << std::endl;
        } else if (expr->isUnary()) {
            std::cout << "GenerateExpressionUnary" << std::endl;
        } else {
            assert(false);
        }

        if(expr->isLiteral()) 
            GenerateExpressionLiteral(expr, isMainFile);

        else if(expr->isInfinity())
            GenerateExpressionInfinity(expr, isMainFile);

        else if(expr->isIdentifierExpr())
            GenerateExpressionIdentifier(expr->getId(), isMainFile, isNotToUpper);

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

    void DslCppGenerator::GenerateExpressionIdentifier(Identifier* id, bool isMainFile, bool isNotToUpper) {

        if(isNotToUpper) {
            (isMainFile ? main : header).pushString(id->getIdentifier());
            return;
        }

        (isMainFile ? main : header).pushString(CapitalizeFirstLetter(id->getIdentifier()));
    }

    void DslCppGenerator::GenerateExpressionPropId(PropAccess* propId, bool isMainFile) {

        std::string value;
        Identifier *id1 = propId->getIdentifier1();
        Identifier *id2 = propId->getIdentifier2();


        if(isMainFile)
            value = id2->getIdentifier() + string("[") + id1->getIdentifier() + "]";
        else
            value = string("d") + CapitalizeFirstLetter(id2->getIdentifier()) + "[" + id1->getIdentifier() + "]";
    
        (isMainFile ? main : header).pushString(value);
    }

    void DslCppGenerator::GenerateExpressionArithmeticOrLogical(
        Expression* expr, bool isMainFile, bool isAtomic
    ) {

        dslCodePad &target = isMainFile ? main : header;

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

            strBuffer = string("(dOffsetArray[") + nodeId->getIdentifier() + " + 1] -" +
                         "dOffsetArray[" + nodeId->getIdentifier() + "]";

            targetFile.pushString(strBuffer);

        } else if (methodId == "is_an_edge") {

            assert(expr->getArgList().size() == 2);
            Identifier *src = expr->getArgList().front()->getExpr()->getId();
            Identifier *dest = expr->getArgList().back()->getExpr()->getId();
            targetFile.pushString(
                "IsAnEdge(" + std::string(src->getIdentifier())
                + ", " + std::string(dest->getIdentifier()) 
                + ", dOffsetArray, dEdgelist)"
            );

            HIT_CHECK
            cout << "SET SOME FLAG TO GENERATE IS_AN_EDGE\n";
        } else if(methodId == "num_nodes") {

            if(expr->getArgList().size() == 0) {

                // targetFile.pushString("// Comment here");
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
        cout << "FLAGFLAG2\n";

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
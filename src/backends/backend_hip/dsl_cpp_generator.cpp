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

            std::string strBuffer;

            const std::string varType = ConvertToCppType(type);
            const std::string varName = stmt->getdeclId()->getIdentifier();

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

        if(stmt->lhs_isIdentifier()) {

            if(stmt->hasPropCopy()) {
                
                //TODO: Make generic. Possibly add a new function for this.
                targetFile.pushStringWithNewLine(
                    "hipMemcpy(d" + CapitalizeFirstLetter(stmt->getId()->getIdentifier()) + 
                    ", d" + CapitalizeFirstLetter(stmt->getExpr()->getId()->getIdentifier()) + ", sizeof(" +
                    ConvertToCppType(stmt->getId()->getSymbolInfo()->getType()->getInnerTargetType()) 
                    + ") * (V), hipMemcpyDeviceToDevice);"
                );

            } else {
                HIT_CHECK
            }


        } else if(stmt->lhs_isProp()) {

            HIT_CHECK

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
                    // TODO: Implement
                    //! REQUIRED commented below 
                    HIT_CHECK
                    // GenerateHipMemcpyStr(identifier->getIdentifier(), ConvertToCppType(type), ) 
                }
                // TODO: For proptype
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
                    HIT_CHECK
                }
            }

            GenerateHipKernel(stmt);

        } else { 

            // This is the inner for loop which is not parallelizable
            // Normal for loop as well, I think. 
            // TODO: Confirm above

            GenerateInnerForAll(stmt, false);

            if(stmt->hasFilterExpr()) {
                // stmt->setBody(UpdateForAllBody(stmt));
                HIT_CHECK
            }

            if(extractElemFunc != NULL) {

                if(IsNeighbourIteration(iteratorMethodId->getIdentifier())) {

                    if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS) {
                        HIT_CHECK 

                    } else if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {
                        HIT_CHECK 

                    } else {
                            
                        GenerateStatement(stmt->getBody(), isMainFile);
                        targetFile.pushStringWithNewLine("}");
                    }

                } else {

                    GenerateStatement(stmt->getBody(), isMainFile);
                } 

                if(stmt->isForall() && stmt->hasFilterExpr()) {
                        HIT_CHECK 

                    // GenerateFixedPointFilter(stmt->getfilterExpr(), false);
                }
           } else {

                HIT_CHECK
           }

        }
    }

    void DslCppGenerator::GenerateInnerForAll(forallStmt* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        
        Identifier* iterator = stmt->getIterator();

        if(stmt->isSourceProcCall()) {

            if(false) {

                //! TODO: Impl
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

            HIT_CHECK 
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

                // !note: Below code note required as it is handled from the DSL
                // if(identifier->getSymbolInfo()->getId()->get_fp_association()) {

                //     header.pushString(", ");
                //     header.pushString(ConvertToCppType(type->getInnerTargetType()));
                //     header.pushString(" d");
                //     header.pushString(temp);
                //     header.pushString("Next");
                // }
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
            targetFile.pushStringWithNewLine("while(!" + fixedPointVar + ") {");
            targetFile.NewLine();
            targetFile.pushStringWithNewLine(fixedPointVar + " = true;"); //TODO: Can this be generic?

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
                    (*std::next(reductionCall->getargList().begin()))->getExpr(), isMainFile
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

                    targetFile.pushString(";");
                }
                targetFile.pushStringWithNewLine("}");
            }
        }
    }

    void DslCppGenerator::GenerateReductionOperation(reductionCallStmt *stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;

        HIT_CHECK
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

            for(auto itr =  argList.begin(); itr != argList.end(); itr++) {
                GenerateInitKernelCall((*itr)->getAssignExpr(), isMainFile);
            }
            

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

        (isMainFile ? main : header).pushString(CapitalizeFirstLetter(id->getIdentifier()));
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
        ) {

        }
            // value = string("d") + id2->getIdentifier() + "Next[" + id1->getIdentifier() + "]";
        else {

            if(isMainFile)
                value = id2->getIdentifier() + string("[") + id1->getIdentifier() + "]";
            else
                value = string("d") + CapitalizeFirstLetter(id2->getIdentifier()) + "[" + id1->getIdentifier() + "]";
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
/**
 * @file hip_gen_functions.cpp
 * @brief Provides Hip related implementations for the generator.
 *  
 * @author cs22m056
 */

#include <sstream>

#include "dsl_cpp_generator.h"

namespace sphip {

    void DslCppGenerator::GenerateHipMallocStr(
        const std::string &dVar, 
        const std::string &typeStr, 
        const std::string &sizeOfType
    ) {

        std::ostringstream oss;
        oss << "hipMalloc" << "(&" << dVar
            << ", " << "sizeof(" << typeStr 
            << ") * (" << sizeOfType << "));";  
        main.pushStringWithNewLine(oss.str());
    }

    void DslCppGenerator::GenerateHipMemcpyStr(
        const std::string &dst,
        const std::string &src,
        const std::string &typeStr,
        const std::string &sizeOfType,
        bool isHostToDevice
    ) {

        std::ostringstream oss;
        oss << "hipMemcpy" << "(" << dst
            << ", " << src << ", " << "sizeof(" << typeStr  
            << ") * (" << sizeOfType << "), "
            << (isHostToDevice ? "hipMemcpyHostToDevice" : "hipMemcpyDeviceToHost")
            << ");";  

        main.pushStringWithNewLine(oss.str());
    }

    void DslCppGenerator::GenerateHipMemCpySymbol(const char* var, const std::string typeStr, bool isHostToDevice) {

        std::ostringstream oss;

        if(isHostToDevice) {
            oss << "hipMemcpyToSymbol(::" << var
            << ", &" << var << ", " << "sizeof(" << typeStr  
            << "), 0, hipMemcpyHostToDevice);";
        }
        else {
            oss << "hipMemcpyFromSymbol(&" << var
            << ", ::" << var << ", " << "sizeof(" << typeStr  
            << "), 0, hipMemcpyDeviceToHost);";
        }

        main.pushStringWithNewLine(oss.str());
    }

    void DslCppGenerator::GenerateLaunchConfiguration() {

        main.NewLine();
        main.pushStringWithNewLine("const unsigned threadsPerBlock = " + 
            std::to_string(threadsPerBlock) + ";"
        );
        main.pushStringWithNewLine(
            "const unsigned numThreads = (V < threadsPerBlock) ? V : " +
            std::to_string(threadsPerBlock) + ";"

        );
        main.pushStringWithNewLine(
            "const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;"
        );

        main.NewLine();
    }

    void DslCppGenerator::GenerateTimerStart() {
        main.NewLine();
        main.pushStringWithNewLine("// Timing starts here");
        main.pushStringWithNewLine("hipEvent_t start;");
        main.pushStringWithNewLine("hipEventCreate(&start);");
        main.pushStringWithNewLine("hipEventRecord(start);");
        main.NewLine();
    }


    void DslCppGenerator::GenerateTimerStop() {
        main.NewLine();
        main.pushStringWithNewLine("hipEvent_t stop;");
        main.pushStringWithNewLine("hipEventCreate(&stop);");
        main.pushStringWithNewLine("hipEventRecord(stop);");
        main.pushStringWithNewLine("hipEventSynchronize(stop);");
        main.pushStringWithNewLine("float milliseconds = 0;");
        main.pushStringWithNewLine("hipEventElapsedTime(&milliseconds, start, stop);");
        main.pushStringWithNewLine("std::cout << \"Time taken: \" << milliseconds << \" ms\" << std::endl;");
        main.pushStringWithNewLine("hipEventDestroy(start);");
        main.pushStringWithNewLine("hipEventDestroy(stop);");
        main.pushStringWithNewLine("// Timing ends here");
        main.NewLine();
    }

    void DslCppGenerator::GenerateFormalParameterDeclAllocCopy(
        const list<formalParam*> &paramList
    ) {

        for(auto itr = paramList.begin(); itr != paramList.end(); itr++) {

            Type *type = (*itr)->getType();

            if(
                (type->isPropType() && type->getInnerTargetType()->isPrimitiveType()) ||
                (type->isPrimitiveType())            
            ) {

                Type *innerType = type->isPrimitiveType() ? type : type->getInnerTargetType();

                main.pushString(ConvertToCppType(innerType));
                // main.pushString(" *");

                const std::string identifier = CapitalizeFirstLetter((*itr)->getIdentifier()->getIdentifier());
                main.pushString(" *d");
                main.pushString(identifier);
                main.pushStringWithNewLine(";");

                GenerateHipMalloc(type, identifier);
                GenerateHipMemcpyStr(
                    "d" + identifier, 
                    (type->isPrimitiveType() ? "&h" : "h") + identifier, 
                    ConvertToCppType(innerType), 
                    type->isPrimitiveType() ? "1" : (type->isPropNodeType() ? "V" : "E"), 
                    true
                );
            }
        }

    }

    void DslCppGenerator::GenerateHipMemcpyParams(const list<formalParam*> &paramList) {

        for (auto itr = paramList.begin(); itr != paramList.end(); itr++) {
            Type* type = (*itr)->getType();
            if (type->isPropType()) {
                if (type->getInnerTargetType()->isPrimitiveType()) {
                    //~ Type* innerType = type->getInnerTargetType();

                    const char* sizeofProp = NULL;
                    if (type->isPropEdgeType())
                    sizeofProp = "E";
                    else
                    sizeofProp = "V";
                    const char* temp = "d";
                    std::string temp1 = CapitalizeFirstLetter((*itr)->getIdentifier()->getIdentifier());
                    temp1 = "d" + temp1;
                    char* temp2 = (char*)malloc(1 + strlen(temp) + strlen(temp1.c_str()));
                    strcpy(temp2, temp);
                    strcat(temp2, temp1.c_str());

                    GenerateHipMemCpyStr((*itr)->getIdentifier()->getIdentifier(), temp2, ConvertToCppType(type->getInnerTargetType()), sizeofProp, 0);
                    
                }
    void DslCppGenerator::GenerateCopyBackToHost(const list<formalParam*> &paramList) {
        
        for(auto param: paramList) {

            if(param->getType()->isPropType() && param->getType()->getInnerTargetType()->isPrimitiveType()) {

                GenerateHipMemcpyStr(
                    "h" + CapitalizeFirstLetter(param->getIdentifier()->getIdentifier()), 
                    "d" + CapitalizeFirstLetter(param->getIdentifier()->getIdentifier()), 
                    ConvertToCppType(param->getType()->getInnerTargetType()), 
                    param->getType()->isPropEdgeType() ? "E" : "V", 
                    false
                );

            }
        }
    }

    void DslCppGenerator::GenerateHipMalloc(
        Type* type, 
        const std::string &identifier
    ) {

        const std::string typeStr = type->isPrimitiveType() ? 
            ConvertToCppType(type) : ConvertToCppType(type->getInnerTargetType());
        main.pushStringWithNewLine(
            "hipMalloc(&d" + identifier + ", sizeof(" + 
            typeStr + ") * (" +
            (type->isPrimitiveType() ? "1" : (type->isPropNodeType() ? "V" : "E")) + "));"
        );
    }

    void DslCppGenerator::GenerateInitKernelCall(assignment* assign, bool isMainFile) {

        Identifier *id = assign->getId();
        Expression *expr = assign->getExpr();

        std::string buffer;

        std::string parameterName(id->getIdentifier());
        parameterName[0] = toupper(parameterName[0]);

        //TODO: Use the init arrya call function
        buffer = "InitArray<" + 
                ConvertToCppType(id->getSymbolInfo()->getType()->getInnerTargetType()) +
                "><<<numBlocks, numThreads>>>(V, d" +
                parameterName + ", ";
        
        (isMainFile ? main : header).pushString(buffer);
        GenerateExpression(expr, isMainFile);
        buffer = ");";
        (isMainFile ? main : header).pushStringWithNewLine(buffer);

    }

    void DslCppGenerator::GenerateAuxillaryFunctions() {

        /**
         * NOTE
         * 
         * Make sure that the declaration of the function is done in the header file.
        */

        if(generateIsAnEdgeFunction) {
            
            GenerateIsAnEdgeFunction();
            generateIsAnEdgeFunction = false;
        }
    }

    void DslCppGenerator::GenerateIsAnEdgeFunction() {

        /**
         * This is definitely a very stupid way to do this.
         * If you are reading this, please don't judge me.
        */
        header.pushStringWithNewLine("__device__");
        header.pushStringWithNewLine("bool IsAnEdge(const int s, const int d, const int* dOffsetArray, const int *dEdgeList) {");
        header.NewLine();
        header.pushStringWithNewLine("int startEdge = dOffsetArray[s];");
        header.pushStringWithNewLine("int endEdge = dOffsetArray[s + 1] - 1;");
        header.NewLine();
        header.pushStringWithNewLine("if (dEdgeList[startEdge] == d) {");
        header.pushStringWithNewLine("return true;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("if (dEdgeList[endEdge] == d) {");
        header.pushStringWithNewLine("return true;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("int mid = (startEdge + endEdge) / 2;");
        header.NewLine();
        header.pushStringWithNewLine("while (startEdge <= endEdge) {");
        header.NewLine();
        header.pushStringWithNewLine("if (dEdgeList[mid] == d) {");
        header.pushStringWithNewLine("return true;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("if (d < dEdgeList[mid]) {");
        header.pushStringWithNewLine("endEdge = mid - 1;");
        header.pushStringWithNewLine("} else {");
        header.pushStringWithNewLine("startEdge = mid + 1;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("mid = (startEdge + endEdge) / 2;");
        header.pushStringWithNewLine("}");
        header.NewLine();
        header.pushStringWithNewLine("return false;");
        header.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateAuxillaryKernels() {

        GenerateInitArrayKernelDefinition();
        GenerateInitIndexKernelDefinition();
    }

    void DslCppGenerator::GenerateInitArrayString(
        const std::string type,
        const std::string identifier,
        const std::string value
    ) {
        // TODO: We are assuming V to be the size of the array. This may not be the case always.
        main.pushStringWithNewLine(
            "InitArray<" + type + "><<<numBlocks, numThreads>>>(V, " + identifier + ", " + value + ");"
        );
    }

    void DslCppGenerator::GenerateInitIndexString(
        const std::string type,
        const std::string identifier,
        const std::string value,
        const std::string index
    ) {
        // TODO: We are assuming V to be the size of the array. This may not be the case always.
        main.pushStringWithNewLine(
            "InitArrayIndex<" + type + "><<<1, 1>>>(V, " + identifier + ", " + index + ", " + value + ");"
        );
    }

    void DslCppGenerator::GenerateInitIndexKernelDefinition() {

        header.pushStringWithNewLine("template <typename T>");
        header.pushStringWithNewLine("__global__");
        header.pushStringWithNewLine("void InitArrayIndex(const unsigned V, T* dArray, int index, T value) {");
        header.pushStringWithNewLine("if(index < V) {");
        header.pushStringWithNewLine("dArray[index] = value;"); 
        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
        header.NewLine();
    }

    void DslCppGenerator::GenerateInitArrayKernelDefinition() {

        header.pushStringWithNewLine("template <typename T>");
        header.pushStringWithNewLine("__global__");
        header.pushStringWithNewLine("void InitArray(const unsigned V, T *dArray, T value) {");
        header.pushStringWithNewLine("unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;");
        header.pushStringWithNewLine("if(idx < V) {");
        header.pushStringWithNewLine("dArray[idx] = value;");
        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
        header.NewLine();
    }

    void DslCppGenerator::GenerateHipMemcpySymbol(
        const std::string &var, 
        const std::string &typeStr, 
        const bool direction
    ) {
        throw std::runtime_error("Not implemented");
    }

    void DslCppGenerator::GenerateAtomicStatementFromReductionOp(
        reductionCallStmt* stmt,
        bool isMainFile
    ) {

        /**
         * As of April 14, 2024, the reduction operations which are used by the DSL
         * are Add, Sub, Mul, And, Or and the date types are int, long, bool, float and double.
         * This information was obtained from the reduc_op variable set in the reductionCallStmt
         * class inside ASTNodeTypes.hpp (function or variable names may have changed since then).
         * The following OPERATOR_ADDASSIGN, OPERATOR_MULASSIGN, OPERATOR_ORASSIGN, 
         * OPERATOR_ANDASSIGN, OPERATOR_SUBASSIGN enums are used by the DSL.
         * Based on this we are implementing only the following atomic operations which 
         * are supported by ROCm:
         * 
         * int atomicAdd(int* address, int val) for int, Add
         * unsigned long long atomicAdd(unsigned long long* address,unsigned long long val) for long, Add
         * float atomicAdd(float* address, float val) for float, Add
         * double atomicAdd(double* address, double val) for double, Add
         * int atomicSub(int* address, int val) for int, Sub
         * int atomicAnd(int* address, int val) for int, And
         * unsigned long long atomicAnd(unsigned long long* address,unsigned long long val) for long, And
         * int atomicOr(int* address, int val) for int, Or
         * unsigned long long atomicOr(unsigned long long int* address,unsigned long long val) for long, Or
         * 
         * Since ROCm HIP doesn't support all operations by the DSL, we will throw an error here.
         * 
         * Check if the above information is still valid.
        */

        std::string type("int");
        std::string atomicOp("atomicAdd");

        switch(stmt->getLeftId()->getSymbolInfo()->getType()->gettypeId()) {

            case TYPE_INT:
                type = "int";
                break;

            case TYPE_LONG:
                type = "unsigned long long";
                break;

            case TYPE_FLOAT:
                type = "float";
                break;

            case TYPE_DOUBLE:
                type = "double";
                break;

            default:
                throw std::runtime_error(
                    "Reduction operation not supported by ROCm HIP. Please check GenerateAtomicStatementFromReductionOp(...) "
                    "function inside hip_gen_functions.cpp for HIP Backend for more information."
                );
                std::cerr << "Error: Reduction operation not supported by HIP\n";
                exit(1);
        } 

        switch(stmt->reduction_op()) {

            case OPERATOR_ADDASSIGN:
                atomicOp = "atomicAdd";
                break;

            case OPERATOR_SUBASSIGN:
                atomicOp = "atomicSub";
                break;

            case OPERATOR_ANDASSIGN:
                atomicOp = "atomicAnd";
                break;

            case OPERATOR_ORASSIGN:
                atomicOp = "atomicOr";
                break;

            default:
                throw std::runtime_error(
                    "Reduction operation not supported by ROCm HIP. Please check GenerateAtomicStatementFromReductionOp(...) "
                    "function inside hip_gen_functions.cpp for HIP Backend for more information."
                );
                std::cerr << "Error: Reduction operation not supported by HIP\n";
                exit(1);
        }

        assert(!isMainFile); // Well, if this is gonna be printed in the main file, then we have a problem.

        (isMainFile ? main : header).pushString(
            atomicOp + "((" + type + "*) d" + CapitalizeFirstLetter(stmt->getLeftId()->getIdentifier())
             + ", (" + type + ") "
        );
        GenerateExpression(stmt->getRightSide(), isMainFile);
        (isMainFile ? main : header).pushStringWithNewLine(");");
    }
}
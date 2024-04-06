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

    void DslCppGenerator::GenerateHipMemCpyStr(
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
        cout << "Inside GenerateTimerStart" << endl;
        main.pushstr_newL("hipEvent_t start, stop;");
        main.pushstr_newL("hipEventCreate(&start);");
        main.pushstr_newL("hipEventCreate(&stop);");
        main.pushstr_newL("float milliseconds = 0;");
        main.pushstr_newL("hipEventRecord(start,0);");
        main.NewLine();
    }

    void DslCppGenerator::GenerateTimerStop() {
        main.pushstr_newL("hipEventRecord(stop,0);");
        main.pushstr_newL("hipEventSynchronize(stop);");
        main.pushstr_newL("hipEventElapsedTime(&milliseconds, start, stop);");
        main.pushstr_newL("printf(\"GPU Time: %.6f ms\\n\", milliseconds);");
    }

    void DslCppGenerator::GenerateHipMallocParams(const list<formalParam*> &paramList) {

        for(auto itr = paramList.begin(); itr != paramList.end(); itr++) {

            Type *type = (*itr)->getType();

            if(type->isPropType() && type->getInnerTargetType()->isPrimitiveType()) {

                // ! FIXME
                // main.pushString(ConvertToCppType(type->getInnerTargetType()));
                // main.pushString(" *");

                std::string identifier = (*itr)->getIdentifier()->getIdentifier();
                identifier[0] = std::toupper(identifier[0]);
                // main.pushString("d" + identifier);
                // main.pushStringWithNewLine(";");

                // GenerateHipMalloc(type, identifier);
            } 
        }

        main.NewLine();
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
            }
        }
    }

    void DslCppGenerator::GenerateHipMalloc(
        Type* type, 
        const std::string &identifier
    ) {

        main.pushStringWithNewLine(
            "hipMalloc(&d" + identifier + ", sizeof(" + 
            ConvertToCppType(type->getInnerTargetType()) + ") * (" +
            (type->isPropNodeType() ? "V" : "E") + "));"
        );
    }

    void DslCppGenerator::GenerateInitKernelCall(assignment* assign, bool isMainFile) {

        Identifier *id = assign->getId();
        Expression *expr = assign->getExpr();

        std::string buffer;

        std::string parameterName(id->getIdentifier());
        parameterName[0] = toupper(parameterName[0]);

        cout << "--->" << parameterName << "\n"; //! TODO

        buffer = "initKernel<" + 
                ConvertToCppType(id->getSymbolInfo()->getType()->getInnerTargetType()) +
                "><<<numBlocks, numThreads>>>(V, d" +
                parameterName + ", ";
        
        (isMainFile ? main : header).pushString(buffer);
        GenerateExpression(expr, isMainFile);
        buffer = ");";
        (isMainFile ? main : header).pushStringWithNewLine(buffer);
    }

    void DslCppGenerator::GenerateInitKernel(const std::string str) {

    }
}
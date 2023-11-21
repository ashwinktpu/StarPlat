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

    void DslCppGenerator::GenerateLaunchConfiguration() {

        main.NewLine();
        main.pushStringWithNewLine("const unsigned threadsPerBlock = " + 
            std::to_string(threadsPerBlock) + ";"
        );
        main.pushStringWithNewLine(
            "const unsigned numThreads = (V < threadsPerBlock) ? " +
            std::to_string(threadsPerBlock) + " : V;"
        );
        main.pushStringWithNewLine(
            "const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;"
        );

        main.NewLine();
    }

    void DslCppGenerator::GenerateTimerStart() {
        //TODO
    }

    void DslCppGenerator::GenerateTimerStop() {
        //TODO
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

        //TODO
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

        // Identifier *id = assign->getId();
        // Expression *expr = assign->getExpr();

        // std::string buffer;

        // std::string parameterName(id->getIdentifier());
        // parameterName[0] = toupper(parameterName[0]);

        // cout << "--->" << parameterName << "\n"; //! TODO

        // buffer = "initKernel<" + 
        //         ConvertToCppType(id->getSymbolInfo()->getType()->getInnerTargetType()) +
        //         "><<<numBlocks, numThreads>>>(V, d" +
        //         parameterName + ", ";
        
        // (isMainFile ? main : header).pushString(buffer);
        // GenerateExpression(expr, isMainFile);
        // buffer = ");";
        // (isMainFile ? main : header).pushStringWithNewLine(buffer);
    }

    void DslCppGenerator::GenerateInitKernel(const std::string str) {

    }
}
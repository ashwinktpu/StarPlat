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
            << ", " << src << ", " << "sizeof( " << typeStr  
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
            std::to_string(threadsPerBlock) + " : V";
        );
        main.pushStringWithNewLine(
            ""
        );
    }
}
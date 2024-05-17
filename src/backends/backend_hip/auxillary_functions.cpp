/**
 * @file auxiliary_function.cpp
 * @brief Provides auxiliary function implementation for the generator.
 *  
 * @author cs22m056
 */

#include <iostream>
#include <fstream>

#include "dsl_cpp_generator.h"

#define NL "\n"

using std::string;
using std::cout;

namespace sphip {

    bool DslCppGenerator::OpenOutputFile() {

        const string headerName = std::string(DslCppGenerator::HIP_GEN_DIR) + "/" + this->fileName + ".h";
        const string mainfileName = std::string(DslCppGenerator::HIP_GEN_DIR) + "/" + this->fileName + ".hip";
        cout << "Filename: " << this->fileName 
             << "\nHeaderFile: "<< headerName 
             << "\nMainFile: " << mainfileName << NL;

        headerFile = fopen(headerName.c_str(), "w");
        if(headerFile == NULL) return false;
        header.setOutputFile(headerFile);

        mainFile = fopen(mainfileName.c_str(), "w");
        if(mainFile == NULL) return false;
        main.setOutputFile(mainFile);

        return true;
    }

    const string DslCppGenerator::GetOperatorString(int opId) {

        switch(opId) {

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

    void DslCppGenerator::SetFileName(const std::string& fileName) {

        // this->fileName = fileName;
    } 

    void DslCppGenerator::PushToOutputFile() {

        header.outputToFile();
        main.outputToFile();
    }

    bool DslCppGenerator::CloseOutputFile() {

        PushToOutputFile();

        if(headerFile != NULL)
            fclose(headerFile);

        if(mainFile != NULL)
            fclose(mainFile);

        headerFile = NULL;
        mainFile   = NULL;

        return true;
    }

    std::string DslCppGenerator::ToUpper(const std::string& str) {

        std::string name = str;

        for(size_t i = 0; i < str.length(); i++) {
            name[i] = std::toupper(str[i]);
        }

        return name;
    }

    std::string DslCppGenerator::StripName(const std::string& name) {

        std::size_t lastSlash = name.find_last_of("/");
        if (lastSlash != std::string::npos) {
            return name.substr(lastSlash + 1);
        }
        return name;
    }

    std::string DslCppGenerator::CapitalizeFirstLetter(const std::string &str) {
        if (str.empty()) {
            return str; 
        }
        std::string result = str;
        result[0] = std::toupper(result[0]); 
        return result;
    }

    char* DslCppGenerator::CapitalizeFirstLetter(char* str) {
        str[0] = std::toupper(str[0]);
        return str;
    }
}

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
        const string mainfileName = std::string(DslCppGenerator::HIP_GEN_DIR) + "/" + this->fileName + ".cc";
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
}

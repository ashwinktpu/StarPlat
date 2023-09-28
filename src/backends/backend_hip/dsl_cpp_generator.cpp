/**
 * Implementation of Code Generator for AMD HIP Backend.
 * Implements dsl_cpp_generator.h
 * 
 * @author cs22m056
*/

#include <cctype>
#include <stdexcept>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

namespace sphip {

    DslCppGenerator::DslCppGenerator(const std::string& , const int threadsPerBlock) : 
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

        // TODO: For BC, can we use global barriers? if so, add necessary comments
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
    }

    void DslCppGenerator::GenerateFunctionHeader(Function* func, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main:header;

        // TODO: Add function to increment indentation

        targetFile.pushString("void");
        targetFile.AddSpace();
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
            parameterName[0] = std::toupper(parameterName[0]);
            parameterName = "d" + parameterName;

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

        if(function->getIsMetaUsed()) 
            main.pushStringWithNewLine("int* dOffsetArray;");

        if(function->getIsDataUsed())
            main.pushStringWithNewLine("int* dEdgelist;");

        if(function->getIsSrcUsed())
            main.pushStringWithNewLine("int* dSrcList;");

        if(function->getIsWeightUsed())
            main.pushStringWithNewLine("int *dWeight;");

        if(function->getIsRevMetaUsed())
            main.pushStringWithNewLine("int* dRevOffsetArray;");

        main.NewLine();

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

        if(function->getIsMetaUsed())
            GenerateHipMemCpyStr("dOffsetArray", "hOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateHipMallocStr("dEdgelist", "hEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateHipMallocStr("dSrcList", "hSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateHipMallocStr("dWeight", "hWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateHipMallocStr("dRevOffsetArray", "hRevOffsetArray", "int", "V + 1");

        main.NewLine();

        GenerateLaunchConfiguration();
    }

    void DslCppGenerator::GenerateCsrArrays(const std::string &graphId, Function *func) {

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

    void GenerateCsrArrays(const std::string &graphId, Function *func) {


    }
}
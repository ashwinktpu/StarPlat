/**
 * Code Generator Interface for AMD HIP Backend.
 * Implemented in dsl_cpp_generator.h
 *
 * @author cs22m056
 */

#ifndef HIP_DSL_CPP_GENERATOR
#define HIP_DSL_CPP_GENERATOR

#include "../../ast/ASTNodeTypes.hpp"
#include "../dslCodePad.h"

namespace sphip {

    class DslCppGenerator {

    public:

        DslCppGenerator(const std::string& fileName, const int threadsPerBlock);

        /**
         * Generation Functions
         */

        /**
         * TODO
         */
        bool Generate();

        /**
         * TODO
         */
        void GenerateIncludeFiles();

        /**
         * TODO
         */
        void GenerateEndOfFile();

        /**
         * TODO
         */
        void SetCurrentFunction(Function* func);

        /**
         * TODO
         */
        void GenerateFunction(Function* func);

        /**
         * TODO
         */
        void GenerateFunctionHeader(Function* func, bool isMainFile);

        /**
         * TODO
         */
        void GenerateFunctionBody(Function* func);

        /**
         * TODO
         */
        std::string ConvertToCppType(Type* type);

        /**
         * TODO
         */
        void CheckAndGenerateVariables(Function *func, const std::string &loc);

        /**
         * TODO
         */
        void CheckAndGenerateHipMalloc(Function *func);

        /**
         * TODO
         */
        void CheckAndGenerateMemcpy(Function *func);

        /**
         * TODO
         */
        void CheckAndGenerateMalloc(Function *func);


        /**
         * TODO
         */
        void GenerateCsrArrays(const std::string &graphId, Function *func);

        /**
         * TODO
         */
        void GenerateLaunchConfiguration();

        /*STATEMENTS BEGIN*/

        /**
         * TODO
         */
        void GenerateBlock(blockStatement* blockStmt, bool includeBrace, bool isMainFile = true);

        /**
         * TODO
         */
        void GenerateStatement(statement* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateVariableDeclaration(declaration* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateDeviceAssignment(assignment* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateAtomicOrNormalAssignment(assignment* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateForAll(forallStmt* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateFixedPoint(fixedPointStmt* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateIfStmt(ifStmt* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateReductionCallStmt(reductionCallStmt* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateItrBfs(iterateBFS* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateItrRevBfs(iterateReverseBFS* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateProcCallStmt(proc_callStmt* stmt, bool isMainFile);

        /* STATEMENTS END*/

        /**
         * TODO
         */
        void GenerateMallocStr(const std::string &dVar, const std::string &typeStr, const std::string &sizeOfType);

        /**
         * HIP Specific Functions
         */

        /**
         * TODO
         */
        void GenerateHipMallocParams(const list<formalParam*> &paramList);

        /**
         * TODO
         */
        void GenerateHipMemcpyParams(const list<formalParam*> &paramList);

        /**
         * TODO
         */
        void GenerateHipMemCpySymbol(const std::string &var, const std::string &typeStr, bool direction);

        /**
         * TODO
         */
        void GenerateHipMallocStr(const std::string &dVar, const std::string &typeStr, const std::string &sizeOfType);

        /**
         * TODO
         */
        void GenerateHipMemCpyStr(const std::string &dst, const std::string &src, const std::string &typeStr, const std::string &sizeOfType, bool isHostToDevice = true);

        /**
         * TODO
         */
        void GenerateHipMalloc(Type* type, const std::string &identifier);

        /**
         * TODO
         */
        void GenerateTimerStart();

        /**
         * TODO
         */
        void GenerateTimerStop();

        /**
         * Auxillary Functions
         */

        /**
         * TODO
         */
        bool OpenOutputFile();

        /**
         * TODO
         */
        void PushToOutputFile();

        /**
         * TODO
         */
        bool CloseOutputFile();

        /**
         * TODO
         */
        std::string ToUpper(const std::string& name);

        /**
         * TODO
         */
        std::string StripName(const std::string& name);

        /**
         * DEPRECATED: Use the constructor for setting the file name
         */
        [[deprecated("Function is deprecated. Use constructor for setting names instead.")]] void SetFileName(const std::string& fileName);

    private:

        const std::string fileName;
        static constexpr const char* HIP_GEN_DIR = "../graphcode/generated_hip";
        const std::string HEADER_GUARD;
        const int threadsPerBlock;

        dslCodePad header;
        dslCodePad main;
        FILE* headerFile;  // Improvement: use ofstream
        FILE* mainFile;    // Improvement: use ofstream

        Function* function;
        bool generateCsr;
    };
}  

#endif
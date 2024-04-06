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

        Function* GetCurrentFunction();

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

        void GenerateForAllSignature(forallStmt* stmt, bool isMainFile);

        bool NeighbourIteration(char* methodId);

        bool AllGraphIteration(char* methodId);

        /**
         * TODO
         */
        void GenerateFixedPoint(fixedPointStmt* stmt, bool isMainFile);

        void GeneratefixedPoint_filter(Expression* stmt, bool isMainFile);

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

        void GenerateItrBfs2(iterateBFS2* stmt, bool isMainFile);

        /**
         * TODO
         */
        void GenerateItrRevBfs(iterateReverseBFS* stmt, bool isMainFile);

        void GenerateItrBfsRev(iterateBFSReverse* stmt, bool isMainFile);

        void AddHipBFSIterationLoop(iterateBFS* stmt, bool generateRevBfs);

        void AddHipBFS2IterationLoop(iterateBFS2* stmt, bool generateRevBfs);

        void AddHipRevBFSIterationLoop(iterateBFS* stmt);

        void AddHipRevBFS2IterationLoop(iterateBFS2* stmt);

        void AddHipBFSIterKernel(iterateBFS* stmt, bool generateRevBfs);

        void AddHipBFS2IterKernel(iterateBFS2* stmt, bool generateRevBfs);

        void AddHipRevBFSIterKernel(list<statement*>& revStmtList, iterateBFS* bfsAbstraction);

        void AddHipRevBFS2IterKernel(list<statement*>& revStmtList, iterateBFS2* bfsAbstraction);

        /**
         * TODO
         */
        void GenerateProcCallStmt(proc_callStmt* stmt, bool isMainFile);

        /* STATEMENTS END*/

        /* GenerateExpressions */
        /**
         * TODO
         */
        void GenerateExpression(Expression* expr, bool isMainFile, bool isAtomic = false);

        /**
         * TODO
         */
        void GenerateExpressionLiteral(Expression* expr, bool isMainFile);

        /**
         * TODO
         */
        void GenerateExpressionInfinity(Expression* expr, bool isMainFile);

        /**
         * TODO
         */
        void GenerateExpressionIdentifier(Identifier* id, bool isMainFile);

        /**
         * TODO
         */
        void GenerateExpressionPropId(PropAccess* propId, bool isMainFile);

        /**
         * TODO
         */
        void GenerateExpressionArithmeticOrLogical(Expression* expr, bool isMainFile, bool isAtomic = false);

        /**
         * TODO
         */
        void GenerateExpressionRelational(Expression* expr, bool isMainFile);

        /**
         * TODO
         */
        void GenerateExpressionProcCallExpression(proc_callExpr* expr, bool isMainFile);

        /**
         * TODO
         */
        void GenerateExpressionUnary(Expression* expr, bool isMainFile);

        /* EndGenerateExpressions*/

        /**
         * TODO
         */
        void GenerateMallocStr(const std::string &dVar, const std::string &typeStr, const std::string &sizeOfType);

        /**
         * TODO
         */
        void GenerateInitKernelCall(assignment* assign, bool isMainFile);

        void GenerateInitkernelStr(const char* inVarType, const char* inVarName, const char* initVal);

        /**
         * TODO
         */
        void GenerateInitKernel(const std::string str);

        void GenerateHipKernel(forallStmt *stmt);

        void GenerateInitIndex();

        void GenerateInitKernel();

        /**
         * HIP Specific Functions
         */

        void GenerateHipMallocParams(const list<formalParam*> &paramList);

        void GenerateHipMemcpyParams(const list<formalParam*> &paramList);

        void GeneratePropParams(list<formalParam*> paramList, bool isNeedType, bool isMainFile);

        /**
         * TODO
         */
        void GenerateHipMemCpySymbol(const char* var, const std::string typeStr, bool isHostToDevice);

        /**
         * TODO
         */
        void GenerateHipMallocStr(const std::string &dVar, const std::string &typeStr, const std::string &sizeOfType);

        /**
         * TODO
         */
        void GenerateHipMemCpyStr(const std::string &dst, const std::string &src, const std::string &typeStr, const std::string &sizeOfType, bool isHostToDevice = true);

        void GenerateHipMemCpySymbol(const char* var, const char* typeStr, bool isHostToDevice);

        /**
         * TODO
         */
        void GenerateHipMalloc(Type* type, const std::string &identifier);

        void GenerateTimerStart();

        void GenerateTimerStop();

        /**
         * Auxillary Functions
         */

        /**
         * TODO
         */
        const string GetOperatorString(int operatorId);

        blockStatement* IncludeIfToBlock(forallStmt* forAll);

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
        void CastIfRequired(Type* type, Identifier* methodID, dslCodePad& main);

        std::string CapitalizeFirstLetter(const std::string& name);

        char* CapitalizeFirstLetter(char* name);

        /**
         * DEPRECATED: Use the constructor for setting the file name
         */
        [[deprecated("Function is deprecated. Use constructor for setting names instead.")]] 
        void SetFileName(const std::string& fileName);

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
        bool generateInitKernel;
        bool generateInitIndex;
    };
}  

#endif
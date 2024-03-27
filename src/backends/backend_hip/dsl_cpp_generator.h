/**
 * Code Generator Interface for AMD HIP Backend.
 * Implemented in dsl_cpp_generator.h
 *
 * @author cs22m056
 */

#ifndef HIP_DSL_CPP_GENERATOR
#define HIP_DSL_CPP_GENERATOR

#include <unordered_map>

#include "../../ast/ASTNodeTypes.hpp"
#include "../../symbolutil/SymbolTable.h"
#include "../dslCodePad.h"

#define propKey pair<TableEntry *, TableEntry *>

namespace sphip {

    enum VariableType {
        READ = 1,
        WRITE,
        READ_WRITE
    };

    class UsedVariables {

    private:

        struct hash_pair {

            template <class T1, class T2>
            size_t operator()(const pair<T1, T2> &p) const {

                auto hash1 = hash<T1>{}(p.first);
                auto hash2 = hash<T2>{}(p.second);
                
                return hash1 ^ hash2;
            }
        };

        unordered_map<TableEntry *, Identifier *> readVars;
        unordered_map<TableEntry *, Identifier *> writeVars;
        unordered_map<propKey, PropAccess *, hash_pair> readProp;
        unordered_map<propKey, PropAccess *, hash_pair> writeProp;

    public:

        UsedVariables() {}

        /**
        * TODO
        */
        void AddVariable(Identifier *iden, int type);

        /**
        * TODO
        */
        void AddPropAccess(PropAccess *prop, int type);

        /**
        * TODO
        */
        void Merge(UsedVariables usedVars);

        /**
        * TODO
        */
        void RemoveVariable(Identifier *iden, int type);

        /**
        * TODO
        */
        bool IsUsedVariable(Identifier *iden, int type = READ_WRITE);

        /**
        * TODO
        */
        list<Identifier *> GetUsedVariables(int type = READ_WRITE);

        /**
        * TODO
        */
        bool HasVariables(int type = READ_WRITE);

        /**
        * TODO
        */
        bool IsUsedPropAccess(PropAccess *prop, int type = READ_WRITE);

        /**
        * TODO
        */
        bool IsUsedProp(PropAccess *prop, int type = READ_WRITE);

        /**
        * TODO
        */
        list<PropAccess *> GetPropAccess(int type = READ_WRITE);
    
        /**
        * TODO
        */
        void Clear();
    };

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

        /**
         * TODO
         */
        void GenerateInitKernel(const std::string str);

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
        void GenerateHipMallocStr(const std::string &dVar, const std::string &typeStr, const std::string &sizeOfType);

        /**
         * TODO
         */
        void GenerateHipMemcpyStr(const std::string &dst, const std::string &src, const std::string &typeStr, const std::string &sizeOfType, bool isHostToDevice = true);

        /**
         * TODO
         */
        void GenerateHipMalloc(Type* type, const std::string &identifier);

        /**
         * TODO
         */
        void GenerateHipMemcpySymbol(const std::string &var, const std::string &typeStr, const bool direction);

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
        const string GetOperatorString(int operatorId);

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
        [[deprecated("Function is deprecated. Use constructor for setting names instead.")]] 
        void SetFileName(const std::string& fileName);

        /**
         * Functions related to currently active variables.
         * Implemented in get_used_meta_data.cpp
        */

        /**
        * Returns the meta data consturcts that currently in use.
        */
        UsedVariables GetUsedVariablesInStatement(statement *statment);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInExpression(Expression *expr);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInWhile(whileStmt *whileStmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInDoWhile(dowhileStmt *dowhileStmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInAssignment(assignment *stmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInIf(ifStmt *ifStmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInFixedPoint(fixedPointStmt *fixedPt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInReductionCall(reductionCallStmt *stmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInUnaryStmt(unary_stmt *stmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInForAll(forallStmt *forAllStmt);

        /**
        * TODO
        */
        UsedVariables GetUsedVariablesInBlock(blockStatement *block);

        /**
         * TODO
        */
        UsedVariables GetDeclaredPropertyVariablesOfBlock(blockStatement *block);

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
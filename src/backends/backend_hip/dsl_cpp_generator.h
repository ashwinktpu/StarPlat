/**
 * Code Generator Interface for AMD HIP Backend.
 * Implemented in dsl_cpp_generator.h
 *
 * @author cs22m056
 */

#ifndef HIP_DSL_CPP_GENERATOR
#define HIP_DSL_CPP_GENERATOR

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../ast/ASTNodeTypes.hpp"
#include "../../symbolutil/SymbolTable.h"
#include "../dslCodePad.h"

#include "analyzer_utility.h"

#define HIT_CHECK std::cout << "UNIMPL Hit at line " << __LINE__ << " of function " << __func__ << " in file " << __FILE__ << "\n";
#define propKey pair<TableEntry *, TableEntry *>

/**
 * The below declaration is crucial. 
 * TODO: Add the remaining reason for this declaration.
*/
class usedVariables;

namespace sphip {

    /**
     * NOTE: This class might not be required as probably 
     * the tree can be obtained from the ASTNode itself.
     * This is implemented to get the finished variable in
     * SSSP, when inside the forAll loop. I did try to get
     * stmt->getParent()->getParent()->getVarSymbolTable(),
     * but that seems to be empty. 
     * 
     * TODO: If this is indeed required, then the implementation
     * can be made globally available. 
    */
    class NodeStack {

    private:
        std::vector<ASTNode*> stack;
        long top;

    public:

        NodeStack() {
            top = -1;
        }

        void push(ASTNode* node) {
            stack.push_back(node);
            top++;
            printStack();
        }

        ASTNode* pop() {
            if (top == -1) {
                return nullptr;
            }
            ASTNode* node = stack[top];
            stack.pop_back();
            top--;
            printStack();
            return node;
        }

        ASTNode* getNearestAncestorOfType(NODETYPE type) {
            for (long i = top; i >= 0; i--) {
                if (stack[i]->getTypeofNode() == type) {
                    return stack[i];
                }
            }
            return nullptr;
        }

        bool isEmpty() {
            return top == -1;
        }

        ASTNode* getCurrentNode() {
            if (top == -1) {
                return nullptr;
            }
            return stack[top];
        }

        ASTNode* getParentNode() {
            if (top < 1) {
                return nullptr;
            }
            return stack[top - 1];
        }

        bool hasParent() {
            return top > 0;
        }

        long size() {
            return top + 1;
        }

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

        /**
         * TODO
        */
        void GenerateDoWhile(dowhileStmt* stmt, bool isMainFile);

        /**
         * TODO
        */
        void GenerateWhileStmt(whileStmt* stmt, bool isMainFile);

        /**
         * TODO
        */
        void GenerateUnaryStmt(unary_stmt* stmt, bool isMainFile);

        /* STATEMENTS END*/

        /* GenerateExpressions */
        /**
         * TODO
         */
        void GenerateExpression(Expression* expr, bool isMainFile, bool isNotToUpper = false, bool isAtomic = false);

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
        void GenerateExpressionIdentifier(Identifier* id, bool isMainFile, bool isNotToUpper = true);

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
        void GenerateInitArrayString(const std::string type, const std::string identifier, const std::string value);
        
        /**
         * TODO
         */
        void GenerateInitIndexString(const std::string type, const std::string identifier, const std::string value, const std::string index);

        /**
         * TODO
         */
        void GenerateInitArrayKernelDefinition();

        /**
         * TODO
        */
        void GenerateInitIndexKernelDefinition();

        /**
         * TODO
        */
        void GenerateAuxillaryKernels();

        /**
         * TODO
        */
        void GenerateAuxillaryFunctions();

        /**
         * TODO
        */
        void GenerateIsAnEdgeFunction();

        /**
         * TODO
        */
       void GenerateAtomicStatementFromReductionOp(reductionCallStmt *stmt, bool isMainFile);

        /**
         * TODO
        */
        void GenerateHipKernel(forallStmt *forAll);

        /**
         * TODO
        */
        void GenerateInnerForAll(forallStmt *forAll, bool isMainFile);

        /**
         * TODO
        */
        bool IsNeighbourIteration(const std::string methodId);

        /**
         * TODO
         */
        void GenerateAdditionalVariablesAndInitializeForBfs(iterateBFS* stmt);

        /**
         * TODO
        */
        void GenerateItrBfsBody(blockStatement *stmt);
        
        /**
         * TODO
        */
        void GenerateItrRevBfsBody(blockStatement *stmt);
        
        /**
         * TODO
        */
        void GeneratePropParamsAsFormalParams(bool isFunctionDefinition = true, bool isMainFile = true);

        
        /**
         * TODO
        */
        void GenerateUsedVarsInBlockAsFormalParams(blockStatement *stmt, bool isFunctionDefinition = true, bool isMainFile = true);

        /**
         * TODO
        */
        void GenerateForwardBfsKernel(blockStatement *stmt);
        
        /**
         * TODO
        */
        void GenerateReverseBfsKernel(blockStatement *stmt);
        
        /**
         * Some other class of functions
        */
        /**
         * TODO
        */
        blockStatement *UpdateForAllBody(forallStmt *forAll);

        /**
         * TODO
        */
       void GenerateReductionCall(reductionCallStmt *stmt, bool isMainFile);
       
        /**
            * TODO
        */
        void GenerateReductionOperation(reductionCallStmt *stmt, bool isMainFile);

        /**
         * HIP Specific Functions
         */

        /**
         * TODO
         */
        void GenerateFormalParameterDeclAllocCopy(const list<formalParam*> &paramList);

        /**
         * TODO
         */
        void GenerateCopyBackToHost(const list<formalParam*> &paramList);

        /**
         * TODO
         */
        void GenerateHipMallocStr(const std::string &dVar, const std::string &typeStr, const std::string &sizeOfType);

        /**
         * This function will push the hipMemcpy statement of the form
         * hipMemcpy(dst, src, sizeof(typeStr) * sizeOfType, direction); to the output file, 
         * where direction is either hipMemcpyHostToDevice or hipMemcpyDeviceToHost.
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
        std::string CapitalizeFirstLetter(const std::string &str);

        /**
         * TODO
         */
        const std::string GetOperatorString(int operatorId);

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
        usedVariables GetUsedVariablesInStatement(statement *statment);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInExpression(Expression *expr);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInWhile(whileStmt *whileStmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInDoWhile(dowhileStmt *dowhileStmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInAssignment(assignment *stmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInIf(ifStmt *ifStmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInFixedPoint(fixedPointStmt *fixedPt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInReductionCall(reductionCallStmt *stmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInUnaryStmt(unary_stmt *stmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInForAll(forallStmt *forAllStmt);

        /**
        * TODO
        */
        usedVariables GetUsedVariablesInBlock(blockStatement *block);

        /**
         * TODO
        */
        usedVariables GetDeclaredPropertyVariablesOfBlock(blockStatement *block);

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

        NodeStack nodeStack;

        bool generateIsAnEdgeFunction = false;
        /**
         * TODO: The below boolean might not be a good idea. Check if it can be omitted.
         * Added to handle dereferencing of pointers in device code. Without this
         * the pointers are themselves taken for Arithmetic operations.
        */
        bool isCurrentExpressionArithmeticOrLogical = false; // Not used?

        /**
         * TODO: The below set can also be replaced. If HipHostMalloc is used then
         * most of the problems can be fixed.
        */
        std::unordered_set<std::string> primitiveVarsInKernel;
    };
}  

#endif
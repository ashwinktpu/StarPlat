/*
 * @brief Only the analyzer_utility.h file is required. The implementation
 *        exists inside the analyzer folder.
*/

#ifndef ANALYSER_UTILITY
#define ANALYSER_UTILITY

#include <unordered_map>

#include "../../ast/ASTNodeTypes.hpp"
#include "../../symbolutil/SymbolTable.h"

#define propKey pair<TableEntry *, TableEntry *>

enum VariableType {
    READ = 1,
    WRITE,
    READ_WRITE
};

class usedVariables {

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

    usedVariables() {}

    /**
    * TODO
    */
    void addVariable(Identifier *iden, int type);

    /**
    * TODO
    */
    void addPropAccess(PropAccess *prop, int type);

    /**
    * TODO
    */
    void merge(usedVariables usedVars);

    /**
    * TODO
    */
    void removeVariable(Identifier *iden, int type);

    /**
    * TODO
    */
    bool isUsedVariable(Identifier *iden, int type = READ_WRITE);

    /**
    * TODO
    */
    list<Identifier *> getVariables(int type = READ_WRITE);

    /**
    * TODO
    */
    bool hasVariables(int type = READ_WRITE);

    /**
    * TODO
    */
    bool isUsedPropAccess(PropAccess *prop, int type = READ_WRITE);

    /**
    * TODO
    */
    bool isUsedProp(PropAccess *prop, int type = READ_WRITE);

    /**
    * TODO
    */
    list<PropAccess *> getPropAccess(int type = READ_WRITE);

    /**
    * TODO
    */
    void clear();
};

#endif
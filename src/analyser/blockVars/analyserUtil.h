#ifndef ANALYSER_UTILITY_BLOCK
#define ANALYSER_UTILITY_BLOCK

#include "../../ast/ASTNodeTypes.hpp"
#include <unordered_map>
#include "../../symbolutil/SymbolTable.h"

#define propKey pair<TableEntry *, TableEntry *>

enum variable_type_t
{
    USED = 1,
    DEFINED,
    USED_DEFINED
};

class usedVariables_t
{
private:
    struct hash_pair
    {
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2> &p) const;
    };

    unordered_map<TableEntry *, Identifier *> readVars, writeVars;
    unordered_map<propKey, PropAccess *, hash_pair> readProp, writeProp;

public:
    usedVariables_t();
    void addVariable(Identifier *iden, int type);
    void addPropAccess(PropAccess *prop, int type);
    void merge(usedVariables_t usedVars1);
    void removeVariable(Identifier *iden, int type);
    bool isUsedVar(Identifier *iden, int type = USED_DEFINED);
    list<Identifier *> getVariables(int type = USED_DEFINED, bool = false);
    bool hasVariables(int type = USED_DEFINED);
    bool isUsedPropAcess(PropAccess *propId, int type = USED_DEFINED);
    bool isUsedProp(PropAccess *propId, int type = USED_DEFINED);
    list<PropAccess *> getPropAcess(int type = USED_DEFINED);
    void clear();
};

#endif
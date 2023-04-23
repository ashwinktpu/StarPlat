#include "analyserUtil.h"

template <class T1, class T2>
size_t usedVariables_t::hash_pair::operator()(const pair<T1, T2> &p) const
{
    auto hash1 = hash<T1>{}(p.first);
    auto hash2 = hash<T2>{}(p.second);
    return hash1 ^ hash2;
}

usedVariables_t::usedVariables_t() {}

void usedVariables_t::addVariable(Identifier *iden, int type)
{
    if (iden->getSymbolInfo() == nullptr)
        return;
    if (type & 1)
        readVars.insert({iden->getSymbolInfo(), iden});
    if (type & 2)
        writeVars.insert({iden->getSymbolInfo(), iden});
}

void usedVariables_t::addPropAccess(PropAccess *prop, int type)
{
    Identifier *iden1 = prop->getIdentifier1();
    Identifier *iden2 = prop->getIdentifier2();
    propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

    if (type & 1)
        readProp.insert({prop_key, prop});
    if (type & 2)
        writeProp.insert({prop_key, prop});
}

void usedVariables_t::merge(usedVariables_t usedVars1)
{
    for (pair<TableEntry *, Identifier *> iden : usedVars1.readVars)
        this->readVars.insert({iden.first, iden.second});

    for (pair<TableEntry *, Identifier *> iden : usedVars1.writeVars)
        this->writeVars.insert({iden.first, iden.second});

    for (pair<propKey, PropAccess *> iden : usedVars1.readProp)
        this->readProp.insert({iden.first, iden.second});

    for (pair<propKey, PropAccess *> iden : usedVars1.writeProp)
        this->writeProp.insert({iden.first, iden.second});
}

void usedVariables_t::removeVariable(Identifier *iden, int type)
{
    TableEntry *symbInfo = iden->getSymbolInfo();
    if (type & 1)
    {
        if (readVars.find(symbInfo) != readVars.end())
            readVars.erase(symbInfo);
    }

    if (type & 2)
    {
        if (writeVars.find(symbInfo) != writeVars.end())
            writeVars.erase(symbInfo);
    }
}

bool usedVariables_t::isUsedVar(Identifier *iden, int type)
{
    TableEntry *symbInfo = iden->getSymbolInfo();
    if (!symbInfo->isInParallelSection())
        return false;
    if (type == USED)
        return (readVars.find(symbInfo) != readVars.end());
    else if (type == DEFINED)
        return (writeVars.find(symbInfo) != writeVars.end());
    else
        return ((readVars.find(symbInfo) != readVars.end()) || (writeVars.find(symbInfo) != writeVars.end()));
}

list<Identifier *> usedVariables_t::getVariables(int type, bool inParallel)
{
    list<Identifier *> result;
    if (type & 1)
    {
        for (pair<TableEntry *, Identifier *> iden : readVars) {
            if (!inParallel || (iden.first && iden.first->isInParallelSection()))
                result.push_back(iden.second);
        }
    }
    if (type & 2)
    {
        for (pair<TableEntry *, Identifier *> iden : writeVars) {
            if (!inParallel || (iden.first && iden.first->isInParallelSection()))
                result.push_back(iden.second);
        }
    }
    return result;
}

bool usedVariables_t::hasVariables(int type)
{
    if (type == USED)
        return (readVars.size() > 0);
    else if (type == DEFINED)
        return (writeVars.size() > 0);
    else
        return ((writeVars.size() > 0) || (readVars.size() > 0));
}

bool usedVariables_t::isUsedPropAcess(PropAccess *propId, int type)
{
    Identifier *iden1 = propId->getIdentifier1();
    Identifier *iden2 = propId->getIdentifier2();
    propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

    if (type == USED)
        return (readProp.find(prop_key) != readProp.end());
    else if (type == DEFINED)
        return (writeProp.find(prop_key) != writeProp.end());
    else
        return ((readProp.find(prop_key) != readProp.end()) || (writeProp.find(prop_key) != writeProp.end()));
}

bool usedVariables_t::isUsedProp(PropAccess *propId, int type)
{
    Identifier *iden2 = propId->getIdentifier2();
    TableEntry *symbInfo = iden2->getSymbolInfo();

    if (type & 1)
    {
        for (pair<propKey, PropAccess *> iden : readProp)
            if (iden.first.second == symbInfo)
                return true;
    }
    if (type & 2)
    {
        for (pair<propKey, PropAccess *> iden : writeProp)
            if (iden.first.second == symbInfo)
                return true;
    }
    return false;
}

list<PropAccess *> usedVariables_t::getPropAcess(int type)
{
    list<PropAccess *> result;
    if (type & 1)
    {
        for (pair<propKey, PropAccess *> iden : readProp)
            result.push_back(iden.second);
    }
    if (type & 2)
    {
        for (pair<propKey, PropAccess *> iden : writeProp)
            result.push_back(iden.second);
    }
    return result;
}

void usedVariables_t::clear()
{
    readVars.clear();
    writeVars.clear();

    readProp.clear();
    writeProp.clear();
}
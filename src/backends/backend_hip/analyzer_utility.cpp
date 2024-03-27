/**
 * @file analyzer_utility.cpp
 * @brief Provides implementation for UsedVariables class.
 
 // TODO Add more doc
 * 
 *  
 * @author cs22m056
 */

#include <iostream> //TODO: Remove

#include "dsl_cpp_generator.h"

namespace sphip {

    void UsedVariables::AddVariable(Identifier *iden, int type) {

        if(type & 1)
            readVars.insert({iden->getSymbolInfo(), iden});

        if (type & 2)
            writeVars.insert({iden->getSymbolInfo(), iden});
    }

    void UsedVariables::AddPropAccess(PropAccess *prop, int type) {

        Identifier *iden1 = prop->getIdentifier1();
        Identifier *iden2 = prop->getIdentifier2();
        propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

        if (type & 1)
            readProp.insert({prop_key, prop});

        if (type & 2)
            writeProp.insert({prop_key, prop});
    }

    void UsedVariables::Merge(UsedVariables usedVars) {
         
         for (pair<TableEntry *, Identifier *> iden : usedVars.readVars)
            this->readVars.insert({iden.first, iden.second});

        for (pair<TableEntry *, Identifier *> iden : usedVars.writeVars)
            this->writeVars.insert({iden.first, iden.second});

        for (pair<propKey, PropAccess *> iden : usedVars.readProp)
            this->readProp.insert({iden.first, iden.second});

        for (pair<propKey, PropAccess *> iden : usedVars.writeProp)
            this->writeProp.insert({iden.first, iden.second});
    }

    void UsedVariables::RemoveVariable(Identifier *iden, int type) {

        TableEntry *symbInfo = iden->getSymbolInfo();

        if(symbInfo == NULL) {
            std::cout << "identifier : " << iden->getIdentifier() << std::endl;
            exit(0);
        }

        if (type & 1) {
            if (readVars.find(symbInfo) != readVars.end())
                readVars.erase(symbInfo);
        }

        if (type & 2) {

            if (writeVars.find(symbInfo) != writeVars.end())
                writeVars.erase(symbInfo);
        }
    }

    bool UsedVariables::IsUsedVariable(Identifier *iden, int type) {

        TableEntry *symbInfo = iden->getSymbolInfo();
        
        if (type == READ)
            return (readVars.find(symbInfo) != readVars.end());
        else if (type == WRITE)
            return (writeVars.find(symbInfo) != writeVars.end());
        else
            return ((readVars.find(symbInfo) != readVars.end()) || (writeVars.find(symbInfo) != writeVars.end()));
    }

    list<Identifier *> UsedVariables::GetUsedVariables(int type) {

        list<Identifier *> result;

        if (type == READ) {
            
            for (pair<TableEntry *, Identifier *> iden : readVars)
                result.push_back(iden.second);
        } else if (type == WRITE) {

            for (pair<TableEntry *, Identifier *> iden : writeVars)
                result.push_back(iden.second);
        
        } else if (type == READ_WRITE) {

            for (pair<TableEntry *, Identifier *> iden : readVars)
                result.push_back(iden.second);

            for (pair<TableEntry *, Identifier *> iden : writeVars) {

                if (readVars.find(iden.first) == readVars.end())
                    result.push_back(iden.second);
            }
        }
        return result;
    }

    bool UsedVariables::HasVariables(int type) {

        if (type == READ)
            return (readVars.size() > 0);
        else if (type == WRITE)
            return (writeVars.size() > 0);
        else
            return ((writeVars.size() > 0) || (readVars.size() > 0));
    }

    bool UsedVariables::IsUsedPropAccess(PropAccess *propId, int type) {

        Identifier *iden1 = propId->getIdentifier1();
        Identifier *iden2 = propId->getIdentifier2();
        propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

        if (type == READ)
            return (readProp.find(prop_key) != readProp.end());
        else if (type == WRITE)
            return (writeProp.find(prop_key) != writeProp.end());
        else
            return ((readProp.find(prop_key) != readProp.end()) || (writeProp.find(prop_key) != writeProp.end()));
    }

    bool UsedVariables::IsUsedProp(PropAccess *propId, int type) {

        Identifier *iden2 = propId->getIdentifier2();
        TableEntry *symbInfo = iden2->getSymbolInfo();

        if (type & 1) {

            for (pair<propKey, PropAccess *> iden : readProp)
                if (iden.first.second == symbInfo)
                    return true;
        }

        if (type & 2) {

            for (pair<propKey, PropAccess *> iden : writeProp)
                if (iden.first.second == symbInfo)
                    return true;
        }

        return false;
    }

    list<PropAccess*> UsedVariables::GetPropAccess(int type) {

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

    void UsedVariables::Clear() {

        readVars.clear();
        writeVars.clear();
        readProp.clear();
        writeProp.clear();
    }
}
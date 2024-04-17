/**
 * @deprecated file
 * @brief Only the analyzer_utility.h file is required. The implementation
 *        exists inside the analyzer folder.
 * @details TO BE DELETED post testing
*/

// #ifndef ANALYSER_UTILITY
// #define ANALYSER_UTILITY

// #include <unordered_map>

// #include "../../ast/ASTNodeTypes.hpp"
// #include "../../symbolutil/SymbolTable.h"

// #include "dsl_cpp_generator.h"

// #define propKey pair<TableEntry *, TableEntry *>

// enum variable_type {
//     READ = 1,
//     WRITE,
//     READ_WRITE
// };

// // class usedVariables {
// //    private:
// //     struct hash_pair {
// //         template <class T1, class T2>
// //         size_t operator()(const pair<T1, T2> &p) const {
// //             auto hash1 = hash<T1>{}(p.first);
// //             auto hash2 = hash<T2>{}(p.second);
// //             return hash1 ^ hash2;
// //         }
// //     };

// //     unordered_map<TableEntry *, Identifier *> readVars, writeVars;
// //     unordered_map<propKey, PropAccess *, hash_pair> readProp, writeProp;

// //    public:
// //     usedVariables() {
// //         std::cout << "THOMAS: usedVariables created\n";
// //     }

// //     void addVariable(Identifier *iden, int type) {
// //         if (type & 1)
// //             readVars.insert({iden->getSymbolInfo(), iden});
// //         if (type & 2)
// //             writeVars.insert({iden->getSymbolInfo(), iden});
// //     }

// //     void addPropAccess(PropAccess *prop, int type) {
// //         Identifier *iden1 = prop->getIdentifier1();
// //         Identifier *iden2 = prop->getIdentifier2();
// //         propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

// //         if (type & 1)
// //             readProp.insert({prop_key, prop});
// //         if (type & 2)
// //             writeProp.insert({prop_key, prop});
// //     }

// //     void merge(usedVariables usedVars1) {
// //         for (pair<TableEntry *, Identifier *> iden : usedVars1.readVars)
// //             this->readVars.insert({iden.first, iden.second});

// //         for (pair<TableEntry *, Identifier *> iden : usedVars1.writeVars)
// //             this->writeVars.insert({iden.first, iden.second});

// //         for (pair<propKey, PropAccess *> iden : usedVars1.readProp)
// //             this->readProp.insert({iden.first, iden.second});

// //         for (pair<propKey, PropAccess *> iden : usedVars1.writeProp)
// //             this->writeProp.insert({iden.first, iden.second});
// //     }

// //     void removeVariable(Identifier *iden, int type) {
// //         TableEntry *symbInfo = iden->getSymbolInfo();
// //         if (type & 1) {
// //             if (readVars.find(symbInfo) != readVars.end())
// //                 readVars.erase(symbInfo);
// //         }

// //         if (type & 2) {
// //             if (writeVars.find(symbInfo) != writeVars.end())
// //                 writeVars.erase(symbInfo);
// //         }
// //     }

// //     bool isUsedVar(Identifier *iden, int type = READ_WRITE) {
// //         TableEntry *symbInfo = iden->getSymbolInfo();
// //         if (type == READ)
// //             return (readVars.find(symbInfo) != readVars.end());
// //         else if (type == WRITE)
// //             return (writeVars.find(symbInfo) != writeVars.end());
// //         else
// //             return ((readVars.find(symbInfo) != readVars.end()) || (writeVars.find(symbInfo) != writeVars.end()));
// //     }

// //     list<Identifier *> getVariables(int type = READ_WRITE) {
// //         list<Identifier *> result;

// //         if (type == READ) {
// //             for (pair<TableEntry *, Identifier *> iden : readVars)
// //                 result.push_back(iden.second);
// //         } else if (type == WRITE) {
// //             for (pair<TableEntry *, Identifier *> iden : writeVars)
// //                 result.push_back(iden.second);
// //         } else if (type == READ_WRITE) {
// //             for (pair<TableEntry *, Identifier *> iden : readVars)
// //                 result.push_back(iden.second);

// //             for (pair<TableEntry *, Identifier *> iden : writeVars) {
// //                 if (readVars.find(iden.first) == readVars.end())
// //                     result.push_back(iden.second);
// //             }
// //         }
// //         return result;
// //     }

// //     bool hasVariables(int type = READ_WRITE) {
// //         if (type == READ)
// //             return (readVars.size() > 0);
// //         else if (type == WRITE)
// //             return (writeVars.size() > 0);
// //         else
// //             return ((writeVars.size() > 0) || (readVars.size() > 0));
// //     }

// //     bool isUsedPropAcess(PropAccess *propId, int type = READ_WRITE) {
// //         Identifier *iden1 = propId->getIdentifier1();
// //         Identifier *iden2 = propId->getIdentifier2();
// //         propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

// //         if (type == READ)
// //             return (readProp.find(prop_key) != readProp.end());
// //         else if (type == WRITE)
// //             return (writeProp.find(prop_key) != writeProp.end());
// //         else
// //             return ((readProp.find(prop_key) != readProp.end()) || (writeProp.find(prop_key) != writeProp.end()));
// //     }

// //     bool isUsedProp(PropAccess *propId, int type = READ_WRITE) {
// //         Identifier *iden2 = propId->getIdentifier2();
// //         TableEntry *symbInfo = iden2->getSymbolInfo();

// //         if (type & 1) {
// //             for (pair<propKey, PropAccess *> iden : readProp)
// //                 if (iden.first.second == symbInfo)
// //                     return true;
// //         }
// //         if (type & 2) {
// //             for (pair<propKey, PropAccess *> iden : writeProp)
// //                 if (iden.first.second == symbInfo)
// //                     return true;
// //         }
// //         return false;
// //     }

// //     list<PropAccess *> getPropAcess(int type = READ_WRITE) {
// //         list<PropAccess *> result;
// //         if (type & 1) {
// //             for (pair<propKey, PropAccess *> iden : readProp)
// //                 result.push_back(iden.second);
// //         }
// //         if (type & 2) {
// //             for (pair<propKey, PropAccess *> iden : writeProp)
// //                 result.push_back(iden.second);
// //         }
// //         return result;
// //     }

// //     void clear() {
// //         readVars.clear();
// //         writeVars.clear();

// //         readProp.clear();
// //         writeProp.clear();
// //     }
// // };

// #endif

// // /**
// //  * @file analyzer_utility.cpp
// //  * @brief Provides implementation for usedVariables class.

// //  // TODO Add more doc
// //  *
// //  *
// //  * @author cs22m056
// //  */

// // #include <iostream> //TODO: Remove

// // #include "dsl_cpp_generator.h"

// // namespace sphip {

// //     void usedVariables::AddVariable(Identifier *iden, int type) {

// //         if(type & 1)
// //             readVars.insert({iden->getSymbolInfo(), iden});

// //         if (type & 2)
// //             writeVars.insert({iden->getSymbolInfo(), iden});
// //     }

// //     void usedVariables::AddPropAccess(PropAccess *prop, int type) {

// //         Identifier *iden1 = prop->getIdentifier1();
// //         Identifier *iden2 = prop->getIdentifier2();
// //         propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

// //         if (type & 1)
// //             readProp.insert({prop_key, prop});

// //         if (type & 2)
// //             writeProp.insert({prop_key, prop});
// //     }

// //     void usedVariables::Merge(usedVariables usedVars) {

// //          for (pair<TableEntry *, Identifier *> iden : usedVars.readVars)
// //             this->readVars.insert({iden.first, iden.second});

// //         for (pair<TableEntry *, Identifier *> iden : usedVars.writeVars)
// //             this->writeVars.insert({iden.first, iden.second});

// //         for (pair<propKey, PropAccess *> iden : usedVars.readProp)
// //             this->readProp.insert({iden.first, iden.second});

// //         for (pair<propKey, PropAccess *> iden : usedVars.writeProp)
// //             this->writeProp.insert({iden.first, iden.second});
// //     }

// //     void usedVariables::RemoveVariable(Identifier *iden, int type) {

// //         TableEntry *symbInfo = iden->getSymbolInfo();

// //         if(symbInfo == NULL) {
// //             std::cout << "identifier : " << iden->getIdentifier() << std::endl;
// //             exit(0);
// //         }

// //         if (type & 1) {
// //             if (readVars.find(symbInfo) != readVars.end())
// //                 readVars.erase(symbInfo);
// //         }

// //         if (type & 2) {

// //             if (writeVars.find(symbInfo) != writeVars.end())
// //                 writeVars.erase(symbInfo);
// //         }
// //     }

// //     bool usedVariables::IsUsedVariable(Identifier *iden, int type) {

// //         TableEntry *symbInfo = iden->getSymbolInfo();

// //         if (type == READ)
// //             return (readVars.find(symbInfo) != readVars.end());
// //         else if (type == WRITE)
// //             return (writeVars.find(symbInfo) != writeVars.end());
// //         else
// //             return ((readVars.find(symbInfo) != readVars.end()) || (writeVars.find(symbInfo) != writeVars.end()));
// //     }

// //     list<Identifier *> usedVariables::GetUsedVariables(int type) {

// //         list<Identifier *> result;

// //         if (type == READ) {

// //             for (pair<TableEntry *, Identifier *> iden : readVars)
// //                 result.push_back(iden.second);
// //         } else if (type == WRITE) {

// //             for (pair<TableEntry *, Identifier *> iden : writeVars)
// //                 result.push_back(iden.second);

// //         } else if (type == READ_WRITE) {

// //             for (pair<TableEntry *, Identifier *> iden : readVars)
// //                 result.push_back(iden.second);

// //             for (pair<TableEntry *, Identifier *> iden : writeVars) {

// //                 if (readVars.find(iden.first) == readVars.end())
// //                     result.push_back(iden.second);
// //             }
// //         }
// //         return result;
// //     }

// //     bool usedVariables::HasVariables(int type) {

// //         if (type == READ)
// //             return (readVars.size() > 0);
// //         else if (type == WRITE)
// //             return (writeVars.size() > 0);
// //         else
// //             return ((writeVars.size() > 0) || (readVars.size() > 0));
// //     }

// //     bool usedVariables::IsUsedPropAccess(PropAccess *propId, int type) {

// //         Identifier *iden1 = propId->getIdentifier1();
// //         Identifier *iden2 = propId->getIdentifier2();
// //         propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

// //         if (type == READ)
// //             return (readProp.find(prop_key) != readProp.end());
// //         else if (type == WRITE)
// //             return (writeProp.find(prop_key) != writeProp.end());
// //         else
// //             return ((readProp.find(prop_key) != readProp.end()) || (writeProp.find(prop_key) != writeProp.end()));
// //     }

// //     bool usedVariables::IsUsedProp(PropAccess *propId, int type) {

// //         Identifier *iden2 = propId->getIdentifier2();
// //         TableEntry *symbInfo = iden2->getSymbolInfo();

// //         if (type & 1) {

// //             for (pair<propKey, PropAccess *> iden : readProp)
// //                 if (iden.first.second == symbInfo)
// //                     return true;
// //         }

// //         if (type & 2) {

// //             for (pair<propKey, PropAccess *> iden : writeProp)
// //                 if (iden.first.second == symbInfo)
// //                     return true;
// //         }

// //         return false;
// //     }

// //     list<PropAccess*> usedVariables::GetPropAccess(int type) {

// //         list<PropAccess *> result;
// //         if (type & 1)
// //         {
// //             for (pair<propKey, PropAccess *> iden : readProp)
// //                 result.push_back(iden.second);
// //         }
// //         if (type & 2)
// //         {
// //             for (pair<propKey, PropAccess *> iden : writeProp)
// //                 result.push_back(iden.second);
// //         }
// //         return result;
// //     }

// //     void usedVariables::Clear() {

// //         readVars.clear();
// //         writeVars.clear();
// //         readProp.clear();
// //         writeProp.clear();
// //     }
// // }
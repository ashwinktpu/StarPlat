#include "blockVarsAnalyser.h"

NodeBlockData::NodeBlockData() {
    this->node = NULL;
    this->block = NULL;
    this->startBlock = NULL;
    this->endBlock = NULL;
}

NodeBlockData::NodeBlockData(ASTNode* node) {
    this->node = node;
    setNodeDataBlock();
    this->block = NULL;
    this->startBlock = NULL;
    this->endBlock = NULL;
    node->setBlockData(this);
}

NodeBlockData::NodeBlockData(ASTNode* node, ASTNodeBlock* Block) {
    this->node = node;
    setNodeDataBlock();
    this->block = Block;
    this->startBlock = NULL;
    this->endBlock = NULL;
    node->setBlockData(this);
}

NodeBlockData::NodeBlockData(ASTNode* node, ASTNodeBlock* startBlock, ASTNodeBlock* endBlock) {
    this->node = node;
    setNodeDataBlock();
    this->block = NULL;
    this->startBlock = startBlock;
    this->endBlock = endBlock;
    node->setBlockData(this);
}

NodeBlockData::NodeBlockData(ASTNode* node, ASTNodeBlock* Block, ASTNodeBlock* startBlock, ASTNodeBlock* endBlock) {
    this->node = node;
    setNodeDataBlock();
    this->block = Block;
    this->startBlock = startBlock;
    this->endBlock = endBlock;
    node->setBlockData(this);
}

void NodeBlockData::setNodeDataBlock() {
    if (node == NULL)
        return;

    node->setBlockData(this);
    // switch (node->getTypeofNode())
    // {
    //     case NODE_DOWHILESTMT:
    //     {
    //         dowhileStmt* doWhile = (dowhileStmt*) node;
    //         doWhile->setBlockData(this);
    //         break;
    //     }
    //     case NODE_WHILESTMT:
    //     {
    //         whileStmt* whilestmt = (whileStmt*) node;
    //         whilestmt->setBlockData(this);
    //     }
    //     case NODE_FORALLSTMT:
    //     {
    //         forallStmt* forallstmt = (forallStmt*) node;
    //         forallstmt->setBlockData(this);
    //     }
    //     default:
    //         break;
    // }
    return;
}

ASTNode* NodeBlockData::getNode() {
    return node;
}

ASTNodeBlock* NodeBlockData::getBlock() {
    return block;
}

ASTNodeBlock* NodeBlockData::getStartBlock() {
    return startBlock;
}

ASTNodeBlock* NodeBlockData::getEndBlock() {
    return endBlock;
}

void NodeBlockData::setBlock(ASTNodeBlock* block) {
    this->block = block;
}

void NodeBlockData::setStartBlock(ASTNodeBlock* startBlock) {
    this->startBlock = startBlock;
}

void NodeBlockData::setEndBlock(ASTNodeBlock* endBlock) {
    this->endBlock = endBlock;
}

ASTNodeBlock::ASTNodeBlock() {
    this->node = NULL;
    use = set<TableEntry*>();
    def = set<TableEntry*>();
    in = set<TableEntry*>();
    out = set<TableEntry*>();
    succ = set<ASTNodeBlock*>();
}

ASTNodeBlock::ASTNodeBlock(ASTNode* node) {
    this->node = node;
    use = set<TableEntry*>();
    def = set<TableEntry*>();
    in = set<TableEntry*>();
    out = set<TableEntry*>();
    succ = set<ASTNodeBlock*>();
}

ASTNode* ASTNodeBlock::getNode() {
    return node;
}

set<TableEntry*> ASTNodeBlock::getUse() {
    return use;
}

set<TableEntry*> ASTNodeBlock::getDef() {
    return def;
}

set<TableEntry*> ASTNodeBlock::getIn() {
    return in;
}

set<TableEntry*> ASTNodeBlock::getOut() {
    return out;
}

set<ASTNodeBlock*> ASTNodeBlock::getSucc() {
    return succ;
}

void ASTNodeBlock::addUse(Identifier* id) {
    if (!id->getSymbolInfo()) return;
    use.insert(id->getSymbolInfo());
}

void ASTNodeBlock::addDef(Identifier* id) {
    if (!id->getSymbolInfo()) return;
    def.insert(id->getSymbolInfo());
}

void ASTNodeBlock::addIn(Identifier* id) {
    if (!id->getSymbolInfo()) return;
    in.insert(id->getSymbolInfo());
}

void ASTNodeBlock::addIn(set<TableEntry*> ids) {
    for(TableEntry* id : ids)
        if (id != NULL)
            in.insert(id);
}

void ASTNodeBlock::removeIn(Identifier* id) {
    in.erase(id->getSymbolInfo());
}

void ASTNodeBlock::removeIn(set<TableEntry*> ids) {
    for(TableEntry* id : ids)
        in.erase(id);
}

void ASTNodeBlock::addOut(Identifier* id) {
    if (!id->getSymbolInfo()) return;
    out.insert(id->getSymbolInfo());
}

void ASTNodeBlock::addOut(set<TableEntry*> ids) {
    for(TableEntry* id : ids)
        out.insert(id);
}

void ASTNodeBlock::removeOut(Identifier* id) {
    out.erase(id->getSymbolInfo());
}

void ASTNodeBlock::removeOut(set<Identifier*> ids) {
    for(Identifier* id : ids)
        out.erase(id->getSymbolInfo());
}

void ASTNodeBlock::addSucc(ASTNodeBlock* block) {
    if(block != NULL) succ.insert(block);
}

void ASTNodeBlock::addVars(usedVariables_t vars) {
    for(Identifier* id : vars.getVariables(USED))
        addUse(id);
    for(Identifier* id : vars.getVariables(DEFINED))
        addDef(id);
}
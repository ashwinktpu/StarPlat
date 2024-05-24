#include "delayedComms.h"

delayedComms::delayedComms () {
}

void delayedComms::analyse(list<Function *> funcList) {
  for (auto &function:funcList) {
    traverseASTNode ((ASTNode*) function->getBlockStatment ()) ;
  }
}

void delayedComms::traverseASTNode (ASTNode * currNode) {
  Statement * stmt = (statement *) currNode ;
}


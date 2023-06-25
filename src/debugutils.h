#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <ostream>

#include "ast/ASTNodeTypes.hpp"


std::ostream &operator<<(std::ostream &os, Type &t);


#endif

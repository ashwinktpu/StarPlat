
#ifndef SYMBOL_H
#define SYMBOL_H

#include <vector>
#include <string>
#include<stdio.h>

using namespace std;


class Symbol{
  public:
   string name;
   string type;
   string enclosedType;

    Symbol(string symName,string symType,string symEnclosedType);

};
#endif

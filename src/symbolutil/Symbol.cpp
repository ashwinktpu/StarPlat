#include "Symbol.h"

using namespace std;

Symbol::Symbol(string symName,string symType,string symEnclosedType)
{

	name = symName;
	type = symType;
	enclosedType=symEnclosedType;
}
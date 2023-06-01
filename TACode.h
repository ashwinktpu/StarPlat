#include<stdio.h>
#include<Symbol.h>

using namespace std;

class TACode 
{
  string opCode;
  string target;
  Symbol* destination;
  Symbol* operand1;
  Symbol* operand2;


  TACode()
  {
    operand1=NULL;
    operand2=NULL;
    destination=NULL;
  }

};

#include<stdio.h>
#include<string>
#include<vector>
#include<TACode.h>

using namespace std;

class genContext
{
  string place;
  string type;
  string enclosedType;
  vector<TACode*> code;

  genContext()
  {
     place="NONE";
     type="NONE";
     enclosedType="NONE";
  }

};
#include <string.h>
#include <cassert>
#include "dsl_cpp_generator.h"
#include "getUsedVars.cpp"

dslCodePad& dsl_cpp_generator::getTargetFile(int isMainFile)
{

  if(isMainFile==0)
  {
    return header;
  }
  else if(isMainFile==1)
  {
    return  main;
  }
  
  return  kernel;
  
}


void dsl_cpp_generator::generateGetPlatforms()
{
  main.pushstr_newL("//Getting platforms");
  main.pushstr_newL("cl_int status;");
  main.pushstr_newL("cl_platform_id *platforms = NULL;");
  main.pushstr_newL("cl_uint number_of_platforms;");
  main.pushstr_newL("status = clGetPlatformIDs(0, NULL, &number_of_platforms);");
  main.pushstr_newL("platforms = (cl_platform_id *)malloc(number_of_platforms*sizeof(cl_platform_id));");
  main.pushstr_newL("status = clGetPlatformIDs(number_of_platforms, platforms, NULL);");
  main.NewLine();
  return ;
}
void dsl_cpp_generator::generateGetDevices()
{
  main.pushstr_newL("//Getting Devices present on platform");
   main.pushstr_newL("cl_device_id *devices= NULL;");
   main.pushstr_newL("cl_uint number_of_devices;");
   main.pushstr_newL("status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU, 0, NULL, &number_of_devices);");
   main.pushstr_newL("devices = (cl_device_id *)malloc(number_of_devices*sizeof(cl_device_id));");
   main.pushstr_newL("status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, number_of_devices, devices, NULL);");
  main.NewLine();
  return ;
}
void dsl_cpp_generator::generateCreateContext()
{
  main.pushstr_newL("//Creating context");
  main.pushstr_newL("cl_context context;");
  main.pushstr_newL("context = clCreateContext(NULL, number_of_devices, devices, NULL, NULL, &status);");
  main.NewLine();
}
void dsl_cpp_generator::generateCreateCommandQueue()
{
  main.pushstr_newL("//Creating command queue");
  main.pushstr_newL("cl_command_queue command_queue ;");
  main.pushstr_newL("command_queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE , &status);");
  main.NewLine();
}
void dsl_cpp_generator::generateCudaMallocStr(const char* dVar,
                                              const char* typeStr,
                                              const char* sizeOfType) {
  char strBuffer[1024];
  //cl_mem vertices_device = clCreateBuffer(context, CL_MEM_READ_ONLY, (n+1)*sizeof(int), NULL, &status);
  sprintf(strBuffer, "cl_mem %s = clCreateBuffer(context, CL_MEM_READ_WRITE, %s*sizeof(%s), NULL, &status);",dVar, sizeOfType,typeStr);  
  main.pushstr_newL(strBuffer);
  //~ main.NewLine();
}
void dsl_cpp_generator::generateCudaMemcpyStr(const char* sVarName,
                                              const char* tVarName,
                                              const char* type,
                                              const char* sizeV,
                                              bool isH2D = true) {
  /*status = clEnqueueWriteBuffer(command_queue, d_data, 
  CL_TRUE,0, (e)*sizeof(int), h_data,0,NULL, NULL);*/
  /*status = clEnqueueReadBuffer(command_queue, d_data ,
   CL_TRUE, 0, n*sizeof(int), h_data, 0, NULL, NULL);
*/
  char strBuffer[1024];
  if(isH2D)
  {
    sprintf(strBuffer, "status = clEnqueueWriteBuffer(command_queue, %8s , CL_TRUE, 0, sizeof(%3s)*%s, %8s, 0, NULL, NULL );", sVarName, type, sizeV, tVarName);
  }
  else
  {
    sprintf(strBuffer, "clEnqueueReadBuffer(command_queue, %s , CL_TRUE, 0, sizeof(%s)*%s, %s, 0, NULL, NULL );", sVarName, type, sizeV, tVarName);
  }
  main.pushstr_newL(strBuffer);
}

// Oonly for device variables
void dsl_cpp_generator::generateCudaMalloc(Type* type, const char* identifier) {
  char strBuffer[1024];
  //~ Type* targetType = type->getInnerTargetType();
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3
  sprintf(strBuffer, "cl_mem d_%s = clCreateBuffer(context,CL_MEM_READ_WRITE,(%s)*sizeof(%s),NULL, &status);", identifier,
          convertToCppType(type->getInnerTargetType()),
          (type->isPropNodeType())
              ? "V"
              : "E");  // this assumes PropNode type  IS PROPNODE? V : E //else
                       // might error later

  main.pushstr_newL(strBuffer);
  main.NewLine();
}

const char* dsl_cpp_generator::convertToCppType(Type* type) {
  if (type->isPrimitiveType()) {
    int typeId = type->gettypeId();
    switch (typeId) {
      case TYPE_INT:
        return "int";
      case TYPE_BOOL:
        return "bool";
      case TYPE_LONG:
        return "long";
      case TYPE_FLOAT:
        return "float";
      case TYPE_DOUBLE:
        return "double";
      case TYPE_NODE:
        return "int";
      case TYPE_EDGE:
        return "int";
      default:
        assert(false);
    }
  } else if (type->isPropType()) {
    Type* targetType = type->getInnerTargetType();
    if (targetType->isPrimitiveType()) {
      int typeId = targetType->gettypeId();
      //~ cout << "TYPEID IN CPP" << typeId << "\n";
      switch (typeId) {
        case TYPE_INT:
          return "int*";
        case TYPE_BOOL:
          return "bool*";
        case TYPE_LONG:
          return "long*";
        case TYPE_FLOAT:
          return "float*";
        case TYPE_DOUBLE:
          return "double*";
        default:
          assert(false);
      }
    }
  } else if (type->isNodeEdgeType()) {
    return "int";  // need to be modified.

  } else if (type->isGraphType()) {
    return "graph&";
  } else if (type->isCollectionType()) {
    int typeId = type->gettypeId();

    switch (typeId) {
      case TYPE_SETN:
        return "std::set<int>&";

      default:
        assert(false);
    }
  }

  return "NA";
}
void dsl_cpp_generator::castIfRequired(Type* type, Identifier* methodID,
                                       dslCodePad& main) {
  /* This needs to be made generalized, extended for all predefined function,
     made available by the DSL*/
  string predefinedFunc("num_nodes");
  if (predefinedFunc.compare(methodID->getIdentifier()) == 0) {
    if (type->gettypeId() != TYPE_INT) {
      char strBuffer[1024];
      sprintf(strBuffer, "(%s)", convertToCppType(type));
      main.pushString(strBuffer);
    }
  }
}

void dsl_cpp_generator::generateCSRArrays(const char* gId) {
  char strBuffer[1024];

  sprintf(strBuffer, "int V = %s.num_nodes();",
          gId);  // assuming DSL  do not contain variables as V and E
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer, "int E = %s.num_edges();", gId);
  main.pushstr_newL(strBuffer);
  main.NewLine();

  main.pushstr_newL("printf(\"#nodes:%d\\n\",V);");
  main.pushstr_newL("printf(\"#edges:\%d\\n\",E);");
  //~ main.pushstr_newL("printf(\"#srces:\%d\\n\",sourceSet.size()); /// TODO get from var");

  sprintf(strBuffer, "int* edgeLen = %s.getEdgeLen();",
          gId);  // assuming DSL  do not contain variables as V and E
  main.pushstr_newL(strBuffer);
  main.NewLine();

  // These H & D arrays of CSR do not change. Hence hardcoded!
  main.pushstr_newL("int *h_meta;");
  main.pushstr_newL("int *h_data;");
  main.pushstr_newL("int *h_src;");
  main.pushstr_newL("int *h_weight;");
  main.pushstr_newL("int *h_rev_meta;"); //done only to handle PR since other doesn't uses it
  main.NewLine();

  main.pushstr_newL("h_meta = (int *)malloc( (V+1)*sizeof(int));");
  main.pushstr_newL("h_data = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_src = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_weight = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_rev_meta = (int *)malloc( (V+1)*sizeof(int));");
  main.NewLine();

  main.pushstr_newL("for(int i=0; i<= V; i++) {");
  sprintf(strBuffer, "int temp = %s.indexofNodes[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("h_meta[i] = temp;");
  sprintf(strBuffer, "temp = %s.rev_indexofNodes[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("h_rev_meta[i] = temp;");
  main.pushstr_newL("}");
  main.NewLine();

  main.pushstr_newL("for(int i=0; i< E; i++) {");
  sprintf(strBuffer, "int temp = %s.edgeList[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("h_data[i] = temp;");
  sprintf(strBuffer, "temp = %s.srcList[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("h_src[i] = temp;");
  main.pushstr_newL("temp = edgeLen[i];");
  main.pushstr_newL("h_weight[i] = temp;");
  main.pushstr_newL("}");
  main.NewLine();

  //to handle rev_offset array for pageRank only // MOVED TO PREV FOR LOOP
  //~ main.pushstr_newL("for(int i=0; i<= V; i++) {");
  //~ sprintf(strBuffer, "int temp = %s.rev_indexofNodes[i];", gId);
  //~ main.pushstr_newL(strBuffer);
  //~ main.pushstr_newL("h_rev_meta[i] = temp;");
  //~ main.pushstr_newL("}");
  //~ main.NewLine();


  //-------------------------------------//

  /*
  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);


  int *h_meta;
  int *h_data;
  int *h_weight;

   h_meta = (int *)malloc( (V+1)*sizeof(int));
   h_data = (int *)malloc( (E)*sizeof(int));
   h_weight = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = G.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = G.edgeList[i];
    h_data[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }
  */
}

// This function is going to generate signature of funtions in header file.
void dsl_cpp_generator::generateFuncHeader(Function* proc, int isMainFile) {
  cout<<"I am inside generate Function Header"<<endl;
  dslCodePad& targetFile = getTargetFile(isMainFile);

  //~ char strBuffer[1024];

  //~ if (!isMainFile) {
  //~ targetFile.pushString("__global__ void");
  //~ targetFile.push(' ');
  //~ }
  targetFile.pushString("void");
  targetFile.pushString(" ");
  targetFile.pushString(proc->getIdentifier()->getIdentifier());
  //~ if (!isMainFile) {
  //~ targetFile.pushString("_kernel");
  //~ }

  targetFile.push('(');

  int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = proc->getParamList().size();
  list<formalParam*> paramList = proc->getParamList();
  list<formalParam*>::iterator itr;

  //~ bool genCSR = false;// to generate csr  main.cpp file if g is a param ! var moved to .h file. May not be need. refractor later!
  //~ char* gId;// to generate csr  main.cpp file if g is a param
  for (itr = paramList.begin(); itr != paramList.end(); itr++) {
    arg_currNo++;
    argumentTotal--;

    Type* type = (*itr)->getType();
    targetFile.pushString(convertToCppType(type));
    char* parName = (*itr)->getIdentifier()->getIdentifier();

    targetFile.pushString(" ");
    char str[80];
    strcpy(str, "d_");
    strcat(str, (*itr)->getIdentifier()->getIdentifier());
    //~ strcat (str, '\0');
    // added here
    cout << "param :" << parName << endl;
    cout << "paramD:" << str << endl;

    if (!isMainFile) {
      //~ vars *vv = new vars;
      //~ vv->varType="int*";
      //~ vv->varName=str   ;
      //~ vv->result =false ;
      //~ varList.push_back(*vv);
      //~ varList.push_back({"double *", str,false});
      //~ Identifier* id = (*itr)->getIdentifier();
      if (type->isGraphType()) {
        std::cout << "========== SET TRUE" << '\n';
        genCSR = true;
        //~ gId = parName;
      }

      //varList.push_back(id); cout << "PUSHED =======> " << parName << endl;
      //~ varList.push_back({"int*","d_meta", false})  ;
      //~ varList.push_back({"int*","d_data", false})  ;
      //~ varList.push_back({"int*","d_weight", false});
    }
    //~ if(!type->isGraphType() && isMainFile){
    //~ varList.push_back({convertToCppType(type),parName, false});
    //~ }

    targetFile.pushString(/*createParamName(*/ parName);
    if (argumentTotal > 0) targetFile.pushString(",");

    if (arg_currNo == maximum_arginline) {
      targetFile.NewLine();
      arg_currNo = 0;
    }
  }

  targetFile.pushString(")");

  if (!isMainFile) // if isMainFile==0 then add semicollon
  {
    targetFile.pushString(";");
  }
  //~ if (!isMainFile) {
  //~ targetFile.NewLine();
  //~ targetFile.pushString("{");
  //~ }

  targetFile.NewLine();
  targetFile.NewLine();
 cout<<"exit from Header file"<<endl;
  return;
}

void dsl_cpp_generator::generateFuncBody(Function* proc, int isMainFile) {
  cout<<"Inside generateFunctionBody"<<endl;
  char strBuffer[1024];

  int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = proc->getParamList().size();
  list<formalParam*> paramList = proc->getParamList();
  list<formalParam*>::iterator itr;

  //~ bool genCSR =
  //~ false;  // to generate csr or not in main.cpp file if graph is a parameter
  const char* gId;  // to generate csr or not in main.cpp file if graph is a parameter
  for (itr = paramList.begin(); itr != paramList.end(); itr++) {
    arg_currNo++;
    argumentTotal--;

    Type* type = (*itr)->getType(); // get the type of parameter

    // get the parameter name
    const char* parName = (*itr)->getIdentifier()->getIdentifier();
    // cout << "param:" <<  parName << endl;
    if (!isMainFile && type->isGraphType()) {
      genCSR = true;
      gId = parName;
    }

    // targetFile.pushString(/*createParamName(*/(*itr)->getIdentifier()->getIdentifier());
    if (argumentTotal > 0)
      // targetFile.pushString(",");

      if (arg_currNo == maximum_arginline) {
        // targetFile.NewLine();
        arg_currNo = 0;
      }
  }

  if (genCSR) {
    main.pushstr_newL("// CSR BEGIN");
    generateCSRArrays(gId);

    main.NewLine();

    generateCudaMallocStr("d_meta", "int", "(1+V)");
    generateCudaMallocStr("d_data", "int", "(E)");
    generateCudaMallocStr("d_src", "int", "(E)");
    generateCudaMallocStr("d_weight", "int", "(E)");
    generateCudaMallocStr("d_rev_meta", "int", "(V+1)");
    generateCudaMallocStr("d_modified_next", "bool", "(V)");

    main.NewLine();

    // h_meta h_data h_weight has to be populated!

    generateCudaMemcpyStr("d_meta", "h_meta", "int", "V+1");
    generateCudaMemcpyStr("d_data", "h_data", "int", "E");
    generateCudaMemcpyStr("d_src", "h_src", "int", "E");
    generateCudaMemcpyStr("d_weight", "h_weight", "int", "E");
    generateCudaMemcpyStr("d_rev_meta", "h_rev_meta", "int", "(V+1)");
    main.NewLine();

    main.pushstr_newL("// CSR END");

    main.pushString("//LAUNCH CONFIG");

    //generateLaunchConfig("nodes");  // Most of the time unless we perform edgeBased!

    main.NewLine();

    main.pushstr_newL("// TIMER START");
    //generateStartTimer();


  }
  cout<<"Exiting generateFuncBody"<<endl;
}

const char* dsl_cpp_generator::getOperatorString(int operatorId) {
  switch (operatorId) {
    case OPERATOR_ADD:
      return "+";
    case OPERATOR_DIV:
      return "/";
    case OPERATOR_MUL:
      return "*";
    case OPERATOR_MOD:
      return "%";
    case OPERATOR_SUB:
      return "-";
    case OPERATOR_EQ:
      return "==";
    case OPERATOR_NE:
      return "!=";
    case OPERATOR_LT:
      return "<";
    case OPERATOR_LE:
      return "<=";
    case OPERATOR_GT:
      return ">";
    case OPERATOR_GE:
      return ">=";
    case OPERATOR_AND:
      return "&&";
    case OPERATOR_OR:
      return "||";
    case OPERATOR_INC:
      return "++";
    case OPERATOR_DEC:
      return "--";
    case OPERATOR_ADDASSIGN:
      return "+";
    case OPERATOR_ANDASSIGN:
      return "&&";
    case OPERATOR_ORASSIGN:
      return "||";
    case OPERATOR_MULASSIGN:
      return "*";
    case OPERATOR_SUBASSIGN:
      return "-";
    default:
      return "NA";
  }
}
/*************************************************Generate Satetments************************************/
void dsl_cpp_generator::generateVariableDecl(declaration* declStmt,
                                             int isMainFile) {
  dslCodePad& targetFile = getTargetFile(isMainFile);
  

  Type* type = declStmt->getType();
  //~ char strBuffer[1024];
  //~ if (type->isPrimitiveType()){
      //~ header.pushString("__device__ d_");
      //~ header.pushString(declStmt->getdeclId()->getIdentifier());
      //~ header.pushstr_newL(";");
      //~ std::cout<< "PRINT DEVICE VAR ======>" <<  declStmt->getdeclId()->getIdentifier()<< '\n';
  //~ }
  if (type->isPropType()) 
  {
    if (type->getInnerTargetType()->isPrimitiveType())
    {

      Type* innerType = type->getInnerTargetType();
      generateCudaMalloc(type, declStmt->getdeclId()->getIdentifier());
    }
  }

 //needs to handle carefully for PR code generation
  else if (type->isPrimitiveType()) {
     char strBuffer[1024];
    // targetFile.pushstr_space(convertToCppType(type));
    const char * varType = convertToCppType(type);
    const char * varName = declStmt->getdeclId()->getIdentifier();
    cout << "varT:" << varType << endl;
    cout << "varN:" << varName << endl;
    sprintf(strBuffer, "%s %s", varType, varName);
    targetFile.pushString(strBuffer);


    if (declStmt->isInitialized()) {
        // targetFile =
      // targetFile.pushString(" = ");
      /* the following if conditions is for cases where the
         predefined functions are used as initializers
         but the variable's type doesnot match*/


      //~ sprintf(strBuffer, "initIndex<<<1,1>>>(1,d_%s,0, 0);",varName);
      //~ targetFile.pushstr_newL(strBuffer);
     /* if (flag_for_device_var ==0){ // fix to fix the issues of PR __device__
        header.pushString(" = ");
      }*/
      targetFile.pushString(" = ");
      if (declStmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
        proc_callExpr* pExpr = (proc_callExpr*)declStmt->getExpressionAssigned();
        Identifier* methodId = pExpr->getMethodId();
        castIfRequired(type, methodId, main);

      }
      generateExpr(declStmt->getExpressionAssigned(), isMainFile); // PRINTS RHS? YES
      /*if(flag_for_device_var ==0){
        generateExpr(declStmt->getExpressionAssigned(), true);
      }*/
    }
    targetFile.pushString("; ");

  }

  else if (type->isNodeEdgeType())
  {
    targetFile.pushstr_space(convertToCppType(type));
    targetFile.pushString(declStmt->getdeclId()->getIdentifier());
    if (declStmt->isInitialized()) 
    {
      targetFile.pushString(" = ");
      generateExpr(declStmt->getExpressionAssigned(), isMainFile);
      targetFile.pushString("; ");
    }
  }
}




void dsl_cpp_generator::generate_exprLiteral(Expression* expr,
                                             int isMainFile) {
  dslCodePad& targetFile = getTargetFile(isMainFile);
  char valBuffer[1024];

  int expr_valType = expr->getExpressionFamily();

  switch (expr_valType) {
    case EXPR_INTCONSTANT:
      sprintf(valBuffer, "%ld", expr->getIntegerConstant());
      break;

    case EXPR_FLOATCONSTANT:
      sprintf(valBuffer, "%lf", expr->getFloatConstant());
      break;
    case EXPR_BOOLCONSTANT:
      sprintf(valBuffer, "%s", expr->getBooleanConstant() ? "true" : "false");
      break;
    default:
      assert(false);

      // return valBuffer;
  }

  //~ printf("VALBUFFER: %s\n", valBuffer);
  targetFile.pushString(valBuffer);
}
void dsl_cpp_generator::generate_exprInfinity(Expression* expr,
                                              int isMainFile) {
  dslCodePad& targetFile = getTargetFile(isMainFile);

  char valBuffer[1024];
  if (expr->getTypeofExpr()) {
    int typeClass = expr->getTypeofExpr();
    switch (typeClass) {
      case TYPE_INT:
        sprintf(valBuffer, "%s",
                expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
        break;
      case TYPE_LONG:
        sprintf(valBuffer, "%s",
                expr->isPositiveInfinity() ? "LLONG_MAX" : "LLONG_MIN");
        break;
      case TYPE_FLOAT:
        sprintf(valBuffer, "%s",
                expr->isPositiveInfinity() ? "FLT_MAX" : "FLT_MIN");
        break;
      case TYPE_DOUBLE:
        sprintf(valBuffer, "%s",
                expr->isPositiveInfinity() ? "DBL_MAX" : "DBL_MIN");
        break;
      default:
        sprintf(valBuffer, "%s",
                expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
        break;
    }

  } else

  {
    sprintf(valBuffer, "%s",expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
  }

  targetFile.pushString(valBuffer);
}



void dsl_cpp_generator::generate_exprRelational(Expression* expr,
                                                int isMainFile) {
  dslCodePad& targetFile = getTargetFile(isMainFile);

  if (expr->hasEnclosedBrackets()) {
    targetFile.pushString("(");
  }
  generateExpr(expr->getLeft(), isMainFile);

  targetFile.space();
  const char* operatorString = getOperatorString(expr->getOperatorType());
  targetFile.pushstr_space(operatorString);
  generateExpr(expr->getRight(), isMainFile);
  if (expr->hasEnclosedBrackets()) {
    targetFile.pushString(")");
  }
}

void dsl_cpp_generator::generate_exprIdentifier(Identifier* id,
                                                int isMainFile) {
  dslCodePad& targetFile = getTargetFile(isMainFile);
  targetFile.pushString(id->getIdentifier());
}
void dsl_cpp_generator::generate_exprPropId(
    PropAccess* propId, int isMainFile)  // This needs to be made more generic.
{
  dslCodePad& targetFile = getTargetFile(isMainFile);

  char strBuffer[1024];
  Identifier* id1 = propId->getIdentifier1();
  Identifier* id2 = propId->getIdentifier2();
  ASTNode* propParent = propId->getParent();
  bool relatedToReduction =
      propParent != NULL ? propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT
                         : false;
  if (id2->getSymbolInfo() != NULL &&
      id2->getSymbolInfo()->getId()->get_fp_association() &&
      relatedToReduction) {
    sprintf(strBuffer, "d_%s_next[%s]", id2->getIdentifier(),
            id1->getIdentifier());
  } else {
    if (!isMainFile)
      sprintf(strBuffer, "d_%s[%s]", id2->getIdentifier(), id1->getIdentifier());  // PREFIX D
    else
      sprintf(strBuffer, "%s[%s]", id2->getIdentifier(), id1->getIdentifier());
  }
  //~ std::cout<< id2->getIdentifier() << id1->getIdentifier() << '\n';
  targetFile.pushString(strBuffer);
}
void dsl_cpp_generator::generate_exprArL(Expression* expr, int isMainFile, bool isAtomic) {  //isAtomic default to false
  dslCodePad& targetFile = getTargetFile(isMainFile);

  if (expr->hasEnclosedBrackets()) {
    targetFile.pushString("(");
  }
  if (!isAtomic)
    generateExpr(expr->getLeft(), isMainFile);
  targetFile.space();
  const char* operatorString = getOperatorString(expr->getOperatorType());
  if (!isAtomic)
    targetFile.pushstr_space(operatorString);
  generateExpr(expr->getRight(), isMainFile);
  if (expr->hasEnclosedBrackets()) {
    targetFile.pushString(")");
  }
}
void dsl_cpp_generator::generate_exprProcCall(Expression* expr,
                                              int isMainFile) {
  //~ cout << "inside the expr_proCall ggggggggggggggggggggggggg" << isMainFile;

  dslCodePad& targetFile = getTargetFile(isMainFile);

  proc_callExpr* proc = (proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if (methodId == "get_edge") {
    //~ cout << "heloooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";
    targetFile.pushString(
        "edge");                           // To be changed..need to check for a neighbour iteration
                                           // and then replace by the iterator.
  } else if (methodId == "count_outNbrs")  // pageRank
  {
    char strBuffer[1024];
    list<argument*> argList = proc->getArgList();
    assert(argList.size() == 1);
    Identifier* nodeId = argList.front()->getExpr()->getId();
    //~ Identifier* objectId = proc->getId1();
    sprintf(strBuffer, "(%s[%s+1]-%s[%s])", "d_meta", nodeId->getIdentifier(),"d_meta", nodeId->getIdentifier());
    targetFile.pushString(strBuffer);
  } else if (methodId == "is_an_edge") {
    char strBuffer[1024];
    list<argument*> argList = proc->getArgList();
    assert(argList.size() == 2);
    Identifier* srcId = argList.front()->getExpr()->getId();
    Identifier* destId = argList.back()->getExpr()->getId();
    //~ Identifier* objectId = proc->getId1();
    sprintf(strBuffer, "%s(%s, %s, %s, %s)", "findNeighborSorted", srcId->getIdentifier(), destId->getIdentifier(),"d_meta","d_data");
    targetFile.pushString(strBuffer);

  } else {
    char strBuffer[1024];
    list<argument*> argList = proc->getArgList();
    if (argList.size() == 0) {
      Identifier* objectId = proc->getId1();
      sprintf(strBuffer, "%s.%s( )", objectId->getIdentifier(),
              proc->getMethodId()->getIdentifier());
      targetFile.pushString(strBuffer);
    }
  }
}

void dsl_cpp_generator::generate_exprUnary(Expression* expr, int isMainFile) {
  dslCodePad& targetFile = getTargetFile(isMainFile);

  if (expr->hasEnclosedBrackets()) {
    targetFile.pushString("(");
  }

  if (expr->getOperatorType() == OPERATOR_NOT) {
    const char* operatorString = getOperatorString(expr->getOperatorType());
    targetFile.pushString(operatorString);
    generateExpr(expr->getUnaryExpr(), isMainFile);
  }

  if (expr->getOperatorType() == OPERATOR_INC ||
      expr->getOperatorType() == OPERATOR_DEC) {
    generateExpr(expr->getUnaryExpr(), isMainFile);
    const char* operatorString = getOperatorString(expr->getOperatorType());
    targetFile.pushString(operatorString);
  }

  if (expr->hasEnclosedBrackets()) {
    targetFile.pushString(")");
  }
}
void dsl_cpp_generator::generateExpr(Expression* expr, int isMainFile, bool isAtomic) {  //isAtomic default to false
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  if (expr->isLiteral()) {
    //~ cout << "INSIDE THIS FOR LITERAL"
    //~ << "\n";
    //~ std::cout<< "------>PROP LIT"  << '\n';
    generate_exprLiteral(expr, isMainFile);
  } else if (expr->isInfinity()) {
    generate_exprInfinity(expr, isMainFile);
  } else if (expr->isIdentifierExpr()) {
    //~ std::cout<< "------>PROP ID"  << '\n';
    generate_exprIdentifier(expr->getId(), isMainFile);
  } else if (expr->isPropIdExpr()) {
    //~ std::cout<< "------>PROP EXP"  << '\n';
    generate_exprPropId(expr->getPropId(), isMainFile);
  } else if (expr->isArithmetic() || expr->isLogical()) {
    //~ std::cout<< "------>PROP AR/LG"  << '\n';
    generate_exprArL(expr, isMainFile, isAtomic);
  } else if (expr->isRelational()) {
    //~ std::cout<< "------>PROP RL"  << '\n';
    generate_exprRelational(expr, isMainFile);
  } else if (expr->isProcCallExpr()) {
    std::cout<< "------>PROP PRO CAL"  << '\n';
    generate_exprProcCall(expr, isMainFile);
  } else if (expr->isUnary()) {
    std::cout<< "------>PROP UNARY"  << '\n';
    generate_exprUnary(expr, isMainFile);
  } else {
    assert(false);
  }
}


void dsl_cpp_generator::generateIfStmt(ifStmt* ifstmt, int isMainFile) {

  dslCodePad& targetFile = getTargetFile(isMainFile);
  Expression* condition = ifstmt->getCondition();
  targetFile.pushString("if ("); std::cout<< "=======IF FILTER" << '\n';
  generateExpr(condition, isMainFile);
  //~ targetFile.pushString(" )");
  targetFile.pushstr_newL("){ // if filter begin ");

  generateStatement(ifstmt->getIfBody(), isMainFile);
  targetFile.pushstr_newL("} // if filter end");
  if (ifstmt->getElseBody() == NULL) return;
  targetFile.pushstr_newL("else");
  generateStatement(ifstmt->getElseBody(), isMainFile);
}

void dsl_cpp_generator::generateProcCall(proc_callStmt* proc_callStmt,
                                         int isMainFile) {
  //~ dslCodePad& targetFile = isMainFile ? main : header;
   cout<<"INSIDE PROCCALL OF GENERATION"<<"\n";
  proc_callExpr* procedure = proc_callStmt->getProcCallExpr();
  // cout<<"FUNCTION NAME"<<procedure->getMethodId()->getIdentifier();
  string methodID(procedure->getMethodId()->getIdentifier());
  string IDCoded("attachNodeProperty");
  int x = methodID.compare(IDCoded);

  if (x == 0) {
    //~ char strBuffer[1024];
    list<argument*> argList = procedure->getArgList();
    list<argument*>::iterator itr;

    for (itr = argList.begin(); itr != argList.end(); itr++) {
      assignment* assign = (*itr)->getAssignExpr();

      if (argList.size() == 1) {
        generateInitkernel1(assign, isMainFile);
        //~ std::cout << "%%%%%%%%%%" << '\n';

        /// initKernel<double> <<<numBlocks,threadsPerBlock>>>(V,d_BC, 0);
      } else if (argList.size() == 2) {
        //~ std::cout << "===============" << '\n';
        generateInitkernel1(assign, isMainFile);
        //~ std::cout<< "initType:" <<
        //convertToCppType(lhsId->getSymbolInfo()->getType()) << '\n'; ~
        //std::cout<< "===============" << '\n'; ~
        //generateInitkernel1(lhsId,"0"); //TODO
      }
    }
  }
  /*
   if(x==0)
       {
         // cout<<"MADE IT TILL HERE";
          char strBuffer[1024];
          list<argument*> argList=procedure->getArgList();
          list<argument*>::iterator itr;
         // targetFile.pushstr_newL("#pragma omp parallel for");
          //sprintf(strBuffer,"for (%s %s = 0; %s < V; %s ++)
   ","int","t","t",procedure->getId1()->getIdentifier(),"num_nodes","t");
          // to ashwina, there is something wrong in above! commenting it. -rajz
          sprintf(strBuffer,"for (%s %s = 0; %s < V; %s ++)
   ","int","t","t","t"); targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("{");
          targetFile.pushstr_newL("");
          for(itr=argList.begin();itr!=argList.end();itr++)
              {
                assignment* assign=(*itr)->getAssignExpr();
                Identifier* lhsId=assign->getId();
                Expression* exprAssigned=assign->getExpr();
                sprintf(strBuffer,"%s[%s] = ",lhsId->getIdentifier(),"t");
                targetFile.pushString(strBuffer);
                generateExpr(exprAssigned, isMainFile);

                targetFile.pushstr_newL(";");

                if(lhsId->getSymbolInfo()->getId()->require_redecl())
                   {
                     sprintf(strBuffer,"%s_nxt[%s] =
   ",lhsId->getIdentifier(),"t"); targetFile.pushString(strBuffer);
                     generateExpr(exprAssigned, isMainFile);
                     targetFile.pushstr_newL(";");
                   }
                sprintf(strBuffer,"***  %s",lhsId->getIdentifier());
                targetFile.pushString(strBuffer);
                sprintf(strBuffer,"***  %d",lhsId->getSymbolInfo()->getType());
                targetFile.pushString(strBuffer);
              }

        targetFile.pushstr_newL("}");

       }
       */
}

void dsl_cpp_generator::generateInitkernel1(assignment* assign, int isMainFile) {  

cout<<"I am inside generateInitkernel1"<<endl;
  char strBuffer[1024];

  Identifier* inId = assign->getId();
  Expression* exprAssigned = assign->getExpr();

  const char* inVarType =
      convertToCppType(inId->getSymbolInfo()->getType()->getInnerTargetType());
  const char* inVarName = inId->getIdentifier();
  cout<<"set argument for init kernel in main"<<endl;
  sprintf(strBuffer, "%s %sValue = (%s)",inVarType, inVarName, inVarType);
  main.pushString(strBuffer);
  generateExpr(exprAssigned,1);
  main.pushString("; ");
  main.NewLine();
  sprintf(strBuffer, "status = clSetKernelArg(init, 0 , sizeof(cl_mem), (void *)& d_%s);", inVarName);
  main.pushString(strBuffer);
  main.NewLine();
  sprintf(strBuffer, "status = clSetKernelArg(init, 1, sizeof(%s) , (void*)& %sValue);" , inVarType, inVarName);
  main.pushString(strBuffer);
  main.NewLine();
  cout<<"Generate init kernel in kernel file first"<<endl;
  //sprintf(strBuffer, "__kernel void init( __global %s d_%s , %s value",inVarType, inVarName,inVarType);
  if(inVarName==NULL)
  {
    cout<<"Error 1";
  }
  else if(inVarType==NULL)
  {
    cout<<" Error in inVar Type"<<endl;
  }
  //kernel.pushString(strBuffer);
  cout<<"first write to kernel"<<endl;
  kernel.pushString("){");
  kernel.NewLine();

  sprintf(strBuffer, "unsigned id = get_global_id(0);");
  kernel.pushString(strBuffer);
  sprintf(strBuffer, "d_%s[id] = value;");
  kernel.pushString(strBuffer);
  kernel.NewLine();
  kernel.push('}');

  //Launch the kernel from main
 sprintf(strBuffer , "status = clEnqueueNDRangeKernel(command_queue, init, 1, NULL, &global_size, &local_size, 0,NULL,NULL);");
 main.pushString(strBuffer);
}
/********************************************End of Generate statements**********************************/


// this function allocates buffer for property of node or edge
void dsl_cpp_generator::generateCudaMallocParams(list<formalParam*> paramList)
{
  list<formalParam*>::iterator itr;

  for(itr=paramList.begin();itr!=paramList.end();itr++)
  {
    Type* type=(*itr)->getType();
    if (type->isPropType()) 
    {
      if (type->getInnerTargetType()->isPrimitiveType())
      {
        Type* innerType = type->getInnerTargetType();

        generateCudaMalloc(type, (*itr)->getIdentifier()->getIdentifier());

      }
    }
  }
}
/***********************************************Dummy definitions******************/
void dsl_cpp_generator::generateDeviceAssignmentStmt(assignment* asmt, int isMainFile) 
{ return;}

void dsl_cpp_generator::generateAtomicDeviceAssignmentStmt(assignment* asmt, int isMainFile)
{
  return ;
}
void dsl_cpp_generator::generateDoWhileStmt(dowhileStmt* doWhile,int isMainFile){return;}
void dsl_cpp_generator::generateForAll(forallStmt* forAll, int isMainFile) {return ;}

void dsl_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct,int isMainFile){return;}

void dsl_cpp_generator::generateReductionStmt(reductionCallStmt* stmt,int isMainFile) {return ;}

void dsl_cpp_generator::generateBFSAbstraction(iterateBFS* bfsAbstraction,int isMainFile){return;}
/**************************************Dummy def ends******************************/



void dsl_cpp_generator::generateStatement(statement* stmt, int isMainFile)
{
  cout<<"I am inside generate statement"<<endl;
  if (stmt->getTypeofNode() == NODE_BLOCKSTMT) {
    generateBlock((blockStatement*)stmt, false, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_DECL) {
    generateVariableDecl((declaration*)stmt, isMainFile);
  }
  
  if (stmt->getTypeofNode() == NODE_ASSIGN) {
    // generateAssignmentStmt((assignment*)stmt);
    assignment* asst = (assignment*)stmt;
    if (asst->isDeviceAssignment())
      generateDeviceAssignmentStmt(asst, isMainFile);
    else  // atomic or normal asmt
      generateAtomicDeviceAssignmentStmt(asst, isMainFile);

  }

  if (stmt->getTypeofNode() == NODE_WHILESTMT) {
    // generateWhileStmt((whileStmt*) stmt);
  }

  if (stmt->getTypeofNode() == NODE_IFSTMT) {

    generateIfStmt((ifStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_DOWHILESTMT) {
    generateDoWhileStmt((dowhileStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
    std::cout << "STMT: For" << '\n';
    printf("isMainFile val %d\n",isMainFile);
    generateForAll((forallStmt*)stmt, isMainFile);
  }
  
 
  if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) {
    generateFixedPoint((fixedPointStmt*)stmt, isMainFile);
  }

  
  
  if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
    generateReductionStmt((reductionCallStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_ITRBFS) {
    generateBFSAbstraction((iterateBFS*)stmt, isMainFile);
  }
  
  if (stmt->getTypeofNode() == NODE_PROCCALLSTMT) {
    generateProcCall((proc_callStmt*)stmt, isMainFile);
  }
  
  if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
    unary_stmt* unaryStmt = (unary_stmt*)stmt;
    generateExpr(unaryStmt->getUnaryExpr(), isMainFile);
    main.pushstr_newL(";");
  }
  cout<<"Exiting Generate statement"<<endl;
}

 void dsl_cpp_generator::generateBlock(blockStatement* blockStmt,bool includeBrace, int isMainFile) {
  cout << "i am inside generateBlock for the first time and the value of isMainFile=" << isMainFile<<endl;
  dslCodePad& targetFile = getTargetFile(isMainFile);

  usedVariables usedVars = getDeclaredPropertyVarsOfBlock(blockStmt);
  list<Identifier*> vars = usedVars.getVariables();
  std::cout<< "\t==VARSIZE:" << vars.size() << '\n';

  //~ for(Identifier* iden: vars) {
    //~ Type* type = iden->getSymbolInfo()->getType();
    //~ char strBuffer[1024];
    //~ printf("\t===%s d_%s\n", convertToCppType(type), iden->getIdentifier());
    //~ main.pushstr_newL(strBuffer);
  //~ }


  list<statement*> stmtList = blockStmt->returnStatements();
  list<statement*>::iterator itr;
  if (includeBrace) {
    targetFile.pushstr_newL("{");
  }
  for (itr = stmtList.begin(); itr != stmtList.end(); itr++) {
    statement* stmt = *itr;
    generateStatement(stmt, isMainFile);
  }
/*
  // CUDA FREE
  char strBuffer[1024];

  if(vars.size()>0) {
    targetFile.NewLine();
    targetFile.pushstr_newL("//cudaFree up!! all propVars in this BLOCK!");
  }
  for(Identifier* iden: vars) {
    sprintf(strBuffer,"cudaFree(d_%s);",iden->getIdentifier());
    targetFile.pushstr_newL(strBuffer);
  }
  targetFile.NewLine();
*/
  if (includeBrace) {
    targetFile.pushstr_newL("}");
  }
  cout<<"Exiting the generate block"<<endl;
}
   
void dsl_cpp_generator::generateFunc(ASTNode* proc) {
  // dslCodePad &targetFile = isMainFile ? main : header;

  //~ char strBuffer[1024];
  Function* func = (Function*)proc;
  generateFuncHeader(func, 0);
  generateFuncHeader(func, 1);   // .cu or main file
  
  // to genearte the function body of the algorithm
  // Note that this we can change later point of time if required

  //~ char outVarName[] = "BC";  //inner type
  //~ char outVarType[] = "double";
  main.pushstr_newL("{");
  if(genCSR)
  {
  generateGetPlatforms();
  generateGetDevices();
  generateCreateContext();
  generateCreateCommandQueue();
  }
  generateFuncBody(func, 0);  // GEnerates CSR ..bool is meaningless

  main.NewLine();

  main.pushstr_newL("//DECLAR DEVICE AND HOST vars in params");
  /* function for generation of clCreateBuffer for property type params*/
  generateCudaMallocParams(func->getParamList());


  //~ sprintf(strBuffer, "%s* d_%s; cudaMalloc(&d_%s, sizeof(%s)*(V)); ///TODO from func", outVarType, outVarName, outVarName, outVarType);
  //~ main.pushstr_newL(strBuffer);

  main.NewLine();

  //~ for (list<formalParam*>::iterator itr = (func->getParamList()).begin(), itrEnd=(func->getParamList()).end(); itr != itrEnd; itr++){
  //~ Type* type = (*itr)->getType();
  //~ if(type->isPropType()){
  //~ const char* typ= convertToCppType(type);
  //~ const char* inTyp= convertToCppType(type->getInnerTargetType());
  //~ std::cout<< "TYP  :" << typ << '\n';
  //~ std::cout<< "INTYP:" << inTyp << '\n';
  //~ }
  //~ }

  main.pushstr_newL("//BEGIN DSL PARSING ");


  generateBlock(func->getBlockStatement(), false );

  // Assuming one function!
  main.pushstr_newL("//TIMER STOP");
  //generateStopTimer();
  main.NewLine();
 // generateCudaMemCpyParams(func->getParamList());
  //~ sprintf(strBuffer, "cudaMemcpy(%s,d_%s , sizeof(%s) * (V), cudaMemcpyDeviceToHost);", outVarName, outVarName, outVarType);

  //~ main.pushstr_newL(strBuffer);

  main.pushstr_newL("} //end FUN");

  return;
}

void dsl_cpp_generator::setCurrentFunc(Function* func)
{
  currentFunc = func;
}

Function* dsl_cpp_generator::getCurrentFunc()
{
  return currentFunc;
}

// This function, fopen those files in which codes will be generated.
bool dsl_cpp_generator::openFileforOutput() {
  char temp[1024];
  printf("fileName %s\n", fileName);
  sprintf(temp, "%s/%s.h", "../graphcode/generated_cuda", fileName);
  headerFile = fopen(temp, "w");
  if (headerFile == NULL) return false;
  header.setOutputFile(headerFile);

  sprintf(temp, "%s/%s_kernel.cl", "../graphcode/generated_cuda" ,fileName);
  kernelFile = fopen(temp, "w");
  if (kernelFile == NULL) return false;
  kernel.setOutputFile(kernelFile);

  //kernel.push('C');
  //kernel.pushString("Hello");

  sprintf(temp, "%s/%s.cu", "../graphcode/generated_cuda", fileName);
  bodyFile = fopen(temp, "w");
  if (bodyFile == NULL) return false;
  main.setOutputFile(bodyFile);

  return true;

}


void dsl_cpp_generator::closeOutputFile() {
  if (headerFile != NULL) {
    header.outputToFile();
    fclose(headerFile);
  }
  headerFile = NULL;

  if (bodyFile != NULL) {
    main.outputToFile();
    fclose(bodyFile);
  }
  bodyFile = NULL;

  if(kernelFile!=NULL)
  {
    kernel.outputToFile();
    fclose(kernelFile);
  }
  kernelFile = NULL;
}

void generation_begin()
{
  cout<<"Generation Begin"<<endl;
  
}
void dsl_cpp_generator::generation_end() {
  header.NewLine();
  header.pushstr_newL("#endif");
}

bool dsl_cpp_generator::generate() {
  // cout<<"FRONTEND
  // VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();
  // //openFileforOutput();
  if (!openFileforOutput()) {
    cout<<"File Open Failed"<<endl;
    return false;
  }
  
  //generation_begin();

  list<Function*> funcList = frontEndContext.getFuncList();
  for (Function* func : funcList) {
    setCurrentFunc(func);
    generateFunc(func);
  }

  generation_end();

  closeOutputFile();

  return true;
}

void dsl_cpp_generator::setFileName(
    char* f)  // to be changed to make it more universal.
{
  char* token = strtok(f, "/");
  char* prevtoken;

  while (token != NULL) {
    prevtoken = token;
    token = strtok(NULL, "/");
  }
  fileName = prevtoken;
}
#include <string.h>
#include <cassert>
#include "dsl_cpp_generator.h"
#include "getUsedVarsAMD.cpp"
#include "../../ast/ASTHelper.cpp"

int kernel_counter=1;
bool isNeighbourIter;

namespace spamd{

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

const char* dsl_cpp_generator :: getAtomicFromType(Type* type, int atomicType)
{
  // cout<<"Inside getAtomicFromType"<<endl;
  if(!type->isPrimitiveType()) // if not premitive => propType , get inner type
  {
    type = type->getInnerTargetType();
  }
  int typeId = type->gettypeId();
  if(atomicType==0) // add type
  {
    cout<< "typeId = "<< typeId <<endl;
    switch (typeId)
    {
    case TYPE_INT:
      return "atomic_add";
    case TYPE_FLOAT:
      return "atomicAddF";
    case TYPE_DOUBLE:
      return "atomicAddD";
    default:
      return "--";
    }
  }

  return "--";
}

void dsl_cpp_generator:: addMemObject(char *obj)
{
  memObjects.push_back(obj);
  //cout<<"memory Object Created :: d_"<<memObjects[memObjectCount]<<endl;
  memObjectCount++;
}
void dsl_cpp_generator:: addKernelObject(char* obj)
{
  // if this is the first kernel I am seeing then create the program from source
  char strBuffer[1024];
  if(kernelObjects.size()==0)
  {
    main.NewLine();
    main.pushstr_newL("//Reading kernel file");
    sprintf(strBuffer,"FILE* kernelfp = fopen(\"%s.cl\", \"rb\"); ", fileName);
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("size_t program_size;");
    main.pushstr_newL("fseek(kernelfp, 0, SEEK_END);");
    main.pushstr_newL("program_size = ftell(kernelfp);");
    main.pushstr_newL("rewind(kernelfp);");
    main.pushstr_newL("char* kernelSource;");
    main.pushstr_newL("kernelSource = (char *)malloc((program_size+1)* sizeof(char));");
    main.pushstr_newL("fread(kernelSource, sizeof(char), program_size, kernelfp);");
    main.pushstr_newL("kernelSource[program_size] = (char)NULL ;");
    main.pushstr_newL("fclose(kernelfp);");
    main.NewLine();
    main.pushstr_newL("//Creating program from source(Create and build Program)");
    main.pushstr_newL("cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &status);");
    main.pushstr_newL("printf(\"Progran created from source, status = %d \\n\", status);");
    main.pushstr_newL("status = clBuildProgram(program, number_of_devices, devices, \" -I ./\", NULL, NULL);");
    main.pushstr_newL("printf(\" Program building completed, status = %d \\n \",status);");
    main.NewLine();


    main.pushstr_newL("if (status != CL_SUCCESS) {");
    main.pushstr_newL(" // Get the build log size");
    main.pushstr_newL(" size_t log_size;");
    main.pushstr_newL(" clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);");
    main.pushstr_newL(" // Allocate memory for the build log");
    main.pushstr_newL(" char *log = (char *)malloc(log_size);");
    main.pushstr_newL(" if (!log) {");
    main.pushstr_newL("   printf(\"Failed to allocate memory for build log.\\n\");");
    main.pushstr_newL("   return; // Or handle the error appropriately");
    main.pushstr_newL(" }");
    main.pushstr_newL(" // Get the build log");
    main.pushstr_newL(" clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);");
    main.pushstr_newL(" // Print the build log");
    main.pushstr_newL(" printf(\"Build log is      : %s\\n\", log);");
    main.pushstr_newL(" // Free the memory");
    main.pushstr_newL(" free(log);");
    main.pushstr_newL(" return;");
    main.pushstr_newL(" }");

    // If we have kernel we are gonna use global size and local size
    main.pushstr_newL("//Variable for launch configuration");
    main.pushstr_newL("size_t global_size;");
    main.pushstr_newL("size_t local_size;");

    // Add header to kernel.cl file 
  }

  kernelObjects.push_back(obj);
  cout<<"kernel Object Created :: "<<kernelObjects[kernelCount]<<endl;
  kernelCount++;
}
bool dsl_cpp_generator::isKenelPresent(char *str)
{
  vector<char*>::iterator itr;
  for(itr=kernelObjects.begin(); itr!= kernelObjects.end(); itr++)
  {
    if(strcmp(*itr,str)==0) return true;
  }
  return false;
}

void dsl_cpp_generator::addProfilling(const char* eventName)
{ 
  char strBuffer[1000];
  sprintf(strBuffer,"clWaitForEvents(1,&%s);",eventName);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,"status = clGetEventProfilingInfo(%s, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);",eventName);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,"status = clGetEventProfilingInfo(%s, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);",eventName);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("kernelTime = (double)(end-start)/convertToMS;");
  main.pushstr_newL("totalTime = totalTime+ kernelTime;");
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
  main.pushstr_newL("printf(\"Got Platform, status = %d \\n\", status);");
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
   main.pushstr_newL("printf(\"Got Devices, status = %d \\n\", status);");
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
  main.pushstr_newL("printf(\"command queue created, status = %d\\n\", status);");
  main.NewLine();
}
void dsl_cpp_generator::generateCudaMallocStr(const char* dVar,
                                              const char* typeStr,
                                              const char* sizeOfType) {
  char strBuffer[1024];
  //cl_mem vertices_device = clCreateBuffer(context, CL_MEM_READ_ONLY, (n+1)*sizeof(int), NULL, &status);
  sprintf(strBuffer, "cl_mem %s = clCreateBuffer(context, CL_MEM_READ_WRITE, %s*sizeof(%s), NULL, &status);",dVar, sizeOfType,typeStr);  
  main.pushstr_newL(strBuffer);
  char *str = (char*)malloc(100*sizeof(char));
  strcpy(str,dVar);
  addMemObject(str);
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
cout<<"inside generateCuda Memcpystr and isH2D = "<<isH2D<<endl;
  char strBuffer[1024];
  if(isH2D)
  {
    sprintf(strBuffer, "status = clEnqueueWriteBuffer(command_queue, %8s , CL_TRUE, 0, sizeof(%3s)*%s, %8s, 0, NULL, NULL );", sVarName, type, sizeV, tVarName);
  }
  else
  {
    sprintf(strBuffer, "clEnqueueReadBuffer(command_queue, %s , CL_TRUE, 0, sizeof(%s)*%s, %s, 0, NULL, NULL );", tVarName, type, sizeV, sVarName);
  }
  main.pushstr_newL(strBuffer);
  //main.pushstr_newL("clWaitForEvents(1,&event);");
}

// Oonly for device variables
void dsl_cpp_generator::generateCudaMalloc(Type* type, const char* identifier) {
  char strBuffer[1024];
  //~ Type* targetType = type->getInnerTargetType();
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3

  sprintf(strBuffer, "cl_mem d_%s = clCreateBuffer(context,CL_MEM_READ_WRITE,(%s)*sizeof(%s),NULL, &status);", identifier,
          (type->isPropNodeType())? "V": "E",convertToCppType(type->getInnerTargetType()));  // this assumes PropNode type  IS PROPNODE? V : E //else
                       // might error later

  main.pushstr_newL(strBuffer);
  main.NewLine();
  char *str = (char*)malloc(100*sizeof(char));
  strcpy(str,"d_");
  strcat(str,identifier);
  //addMemObject(str);

}

const char* dsl_cpp_generator::convertToCppType(Type* type) {
  if (type->isPrimitiveType()) {
    int typeId = type->gettypeId();
    switch (typeId) {
      case TYPE_INT:
        return "int";
      case TYPE_BOOL:
        return "int";
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
          return "int *";
        case TYPE_BOOL:
          return "int *";
        case TYPE_LONG:
          return "long *";
        case TYPE_FLOAT:
          return "float *";
        case TYPE_DOUBLE:
          return "double *";
        default:
          assert(false);
      }
    }
  } else if (type->isNodeEdgeType()) {
    return "edge ";  // need to be modified.

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
  main.pushstr_newL("printf(\"#edges:%d\\n\",E);");   
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
  char *str ;
  str = (char*)malloc(50*sizeof(char));
  strcpy(str,"h_meta");
  hostMemObjects.push_back(str);
  str = (char*)malloc(50*sizeof(char));
  strcpy(str,"h_data");
  hostMemObjects.push_back(str);
  str = (char*)malloc(50*sizeof(char));
  strcpy(str,"h_src");
  hostMemObjects.push_back(str);
  str = (char*)malloc(50*sizeof(char));
  strcpy(str,"h_weight");
  hostMemObjects.push_back(str);
  str = (char*)malloc(50*sizeof(char));
  strcpy(str,"h_rev_meta");
  hostMemObjects.push_back(str);


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
  //cout<<"I am inside generate Function Header"<<endl;
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
  return;
}

void dsl_cpp_generator::generateFuncBody(Function* proc, int isMainFile) {
  //cout<<"Inside generateFunctionBody"<<endl;
  //char strBuffer[1024];

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
      //cout<<"*******GID*********"<<gId<<"********GID********\n";
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
    generateCudaMallocStr("d_modified_next", "int", "(V)");

    main.NewLine();

    // h_meta h_data h_weight has to be populated!

    generateCudaMemcpyStr("d_meta", "h_meta", "int", "(V+1)");
    generateCudaMemcpyStr("d_data", "h_data", "int", "E");
    generateCudaMemcpyStr("d_src", "h_src", "int", "E");
    generateCudaMemcpyStr("d_weight", "h_weight", "int", "E");
    generateCudaMemcpyStr("d_rev_meta", "h_rev_meta", "int", "(V+1)");
    main.NewLine();

    main.pushstr_newL("// CSR END");
    cout<<"//CSR END\n";

    //main.pushString("//LAUNCH CONFIG");

    //generateLaunchConfig("nodes");  // Most of the time unless we perform edgeBased!

    main.NewLine();

    main.pushstr_newL("// TIMER START");
    main.pushstr_newL("cl_event event;");
    main.pushstr_newL("double kernelTime ;");
    main.pushstr_newL("double totalTime = 0.0;");
    main.pushstr_newL("cl_ulong start, end;");
    main.pushstr_newL("cl_ulong convertToMS = 1e6;");
    //generateStartTimer();
  }
  //cout<<"Exiting generateFuncBody"<<endl;
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
  //cout<<"here i might be gettinf segmentation fault\n";
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
      char strBuffer[1024];
      sprintf(strBuffer, "%s *h_%s = (%s *)malloc(%s*sizeof(%s));", convertToCppType(innerType), 
            declStmt->getdeclId()->getIdentifier(), convertToCppType(innerType) , type->isPropNodeType()?"V":"E",
            convertToCppType(innerType));

      main.pushString(strBuffer);
      main.NewLine();
      generateCudaMalloc(type, declStmt->getdeclId()->getIdentifier());
      char *str = (char*)malloc(100*sizeof(char));
      strcpy(str,"h_");
      strcat(str,declStmt->getdeclId()->getIdentifier());
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
    if(isMainFile==1)
    {
      sprintf(strBuffer, "%s *h_%s ;", varType, varName);
      main.pushstr_newL(strBuffer);

      sprintf(strBuffer, "cl_mem d_%s= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(%s), NULL, &status);", varName, varType);
      main.pushstr_newL(strBuffer);
    
    }
    Expression* expr=declStmt->getExpressionAssigned();
    if(expr->isPropIdExpr()){
      PropAccess* propId=expr->getPropId();
      Identifier* id1 = propId->getIdentifier1();
      Identifier* id2 = propId->getIdentifier2();
      if(id2->getSymbolInfo()->getType()->isPropEdgeType()){
        if(isMainFile==1){
          sprintf(strBuffer, "%s %s=", varType, varName);
          targetFile.pushString(strBuffer);
          sprintf(strBuffer,"h_%s[%s_ind];",id2->getIdentifier(),id1->getIdentifier());
          //sprintf(strBuffer, "%s_weight;", id1->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.NewLine();
          return;
        }
        else if(isMainFile==2){
          sprintf(strBuffer, "%s %s=", varType, varName);
          targetFile.pushString(strBuffer);
          sprintf(strBuffer,"d_%s[%s_ind];",id2->getIdentifier(),id1->getIdentifier());
          //sprintf(strBuffer, "%s_weight;", id1->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.NewLine();
          return;
        }


      }
       
    }
    
    
    main.NewLine();
    sprintf(strBuffer, "%s %s", varType, varName);
    targetFile.pushString(strBuffer);
    /*
    char* str = (char*)malloc(100*sizeof(char));
    strcpy(str, "h_");
    strcat(str,varName);
    hostMemObjects.push_back(str);
    */
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
    targetFile.NewLine();
  }

  else if (type->isNodeEdgeType())
  {
    char strBuffer[1024];
    Expression* expr=declStmt->getExpressionAssigned();
    proc_callExpr* proc = (proc_callExpr*)expr;
    list<argument*> argList = proc->getArgList();
    Identifier* srcId = argList.front()->getExpr()->getId();
    Identifier* destId = argList.back()->getExpr()->getId();
    targetFile.NewLine();
    targetFile.NewLine();
    sprintf(strBuffer,"int %s_src=%s;",declStmt->getdeclId()->getIdentifier(),srcId->getIdentifier());
    targetFile.pushString(strBuffer);
    targetFile.NewLine();
    sprintf(strBuffer,"int %s_des=%s;",declStmt->getdeclId()->getIdentifier(),destId->getIdentifier());
    targetFile.pushString(strBuffer);
    targetFile.NewLine();


    sprintf(strBuffer,"int %s_ind=-1;",declStmt->getdeclId()->getIdentifier());
    targetFile.pushString(strBuffer);
    targetFile.NewLine();
    sprintf(strBuffer,"int %s_weight=-1;",declStmt->getdeclId()->getIdentifier());
    targetFile.pushString(strBuffer);
    targetFile.NewLine();

    if(isMainFile==1){
      sprintf(strBuffer, "for(int i=h_meta[%s_src]; i< h_meta[%s_src+1]; i++) {",declStmt->getdeclId()->getIdentifier(),declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer, "int v = h_data[i];");
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer, "if(v==%s_des){",declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer,"%s_ind=i;",declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer,"%s_weight=h_weight[i];",declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      targetFile.pushString("break;");
      targetFile.NewLine();
      targetFile.pushString("}");
      targetFile.NewLine();
      targetFile.pushString("}");
    }
    else if(isMainFile==2){
      sprintf(strBuffer, "for(int i=d_meta[%s_src]; i< d_meta[%s_src+1]; i++) {",declStmt->getdeclId()->getIdentifier(),declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer, "int v = d_data[i];");
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer, "if(v==%s_des){",declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer,"%s_ind=i;",declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      sprintf(strBuffer,"%s_weight=d_weight[i];",declStmt->getdeclId()->getIdentifier());
      targetFile.pushString(strBuffer);
      targetFile.NewLine();
      targetFile.pushString("break;");
      targetFile.NewLine();
      targetFile.pushString("}");
      targetFile.NewLine();
      targetFile.pushString("}");
    }
    
    targetFile.NewLine();
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
      //cout<<"value is an integer constant\n";
      break;

    case EXPR_FLOATCONSTANT:
      sprintf(valBuffer, "%lf", expr->getFloatConstant());
      //cout<<"value is an float constant\n";
      break;
    case EXPR_BOOLCONSTANT:
      sprintf(valBuffer, "%s", expr->getBooleanConstant() ? "true" : "false");
      //cout<<"value is an boolean constant\n";
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
  const char* idName = id->getIdentifier();
  if(isMainFile==2)
  {
    for(Identifier* iden:PremVarOfKernel)
    {
      
      //cout<<idName<<"=================="<<iden->getIdentifier()<<endl;
      if(!strcmp(idName, iden->getIdentifier()))
      {
        
        targetFile.pushString("(*d_");
        targetFile.pushString(id->getIdentifier());
        targetFile.pushString(")");
        //cout<<"(*"<<id->getIdentifier()<<")\n";
        return;
      }
    }
  }

  targetFile.pushString(id->getIdentifier());
}
void dsl_cpp_generator::generate_exprPropId(
    PropAccess* propId, int isMainFile)  // This needs to be made more generic.
{
  //cout<<"true worrior has no true enimies\n";
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
  }
  else if(id2->getSymbolInfo()->getType()->isPropNodeType()) {
    if (isMainFile==2){
      sprintf(strBuffer, "d_%s[%s]", id2->getIdentifier(), id1->getIdentifier());  // PREFIX D
      targetFile.pushString(strBuffer);
    }
    else{
      sprintf(strBuffer, "h_%s[%s]", id2->getIdentifier(), id1->getIdentifier());
      targetFile.pushString(strBuffer);
      }
  }
  
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

  dslCodePad& targetFile = getTargetFile(isMainFile);

  proc_callExpr* proc = (proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if (methodId == "get_edge") {
    targetFile.pushString(proc->getId1()->getIdentifier());
    targetFile.pushString(".getEdge(");
    list<argument*> argList = proc->getArgList();
    assert(argList.size() == 2);
    Identifier* srcId = argList.front()->getExpr()->getId();
    Identifier* destId = argList.back()->getExpr()->getId();
    targetFile.pushString(srcId->getIdentifier());
    targetFile.pushString(",");
    targetFile.pushString(destId->getIdentifier());
    targetFile.pushString(")");
    // if(srcId->getIdentifier())cout<<"source is "<<srcId->getIdentifier()<<endl;
    // else cout<<"no source srcId->getIdentifier()\n";
   // cout<<"destination iss"<<destId->getIdentifier()<<endl;

    

    // To be changed..need to check for a neighbour iteration  and then replace by the iterator.
 
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
    sprintf(strBuffer, "%s(%s, %s, %s, %s)", "isNeighbour", srcId->getIdentifier(), destId->getIdentifier(),"d_meta","d_data");
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
    //~ << "\n";
    //~ std::cout<< "------>PROP LIT"  << '\n';
    generate_exprLiteral(expr, isMainFile);
  } else if (expr->isInfinity()) {
    generate_exprInfinity(expr, isMainFile);
  } else if (expr->isIdentifierExpr()) {
    //std::cout<< "------>PROP ID"  << '\n';
    generate_exprIdentifier(expr->getId(), isMainFile);
  } else if (expr->isPropIdExpr()) {
     //std::cout<< "------>PROP EXP"  << '\n';
    generate_exprPropId(expr->getPropId(), isMainFile);
  } else if (expr->isArithmetic() || expr->isLogical()) {
     //std::cout<< "------>PROP AR/LG"  << '\n';
    generate_exprArL(expr, isMainFile, isAtomic);
  } else if (expr->isRelational()) {
    //std::cout<< "------>PROP RL"  << '\n';
    generate_exprRelational(expr, isMainFile);
  } else if (expr->isProcCallExpr()) {
    //std::cout<< "------>PROP PRO CAL"  << '\n';
    generate_exprProcCall(expr, isMainFile);
  } else if (expr->isUnary()) {
   // std::cout<< "------>PROP UNARY"  << '\n';
    generate_exprUnary(expr, isMainFile);
  } else {
    //cout<<"something else my friend\n";
    assert(false);
  }
}


void dsl_cpp_generator::generateIfStmt(ifStmt* ifstmt, int isMainFile) {

  dslCodePad& targetFile = getTargetFile(isMainFile);
  Expression* condition = ifstmt->getCondition();




  targetFile.pushString("if ("); //std::cout<< "=======IF FILTER" << '\n';
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
        cout<<"argList size is 1\n";
        generateInitkernel1(assign, isMainFile);
        //~ std::cout << "%%%%%%%%%%" << '\n';

        /// initKernel<double> <<<numBlocks,threadsPerBlock>>>(V,d_BC, 0);
      } else if (argList.size() == 2) {
        cout<<"argList size is 2\n";
        //~ std::cout << "===============" << '\n';
        generateInitkernel1(assign, isMainFile);
        //~ std::cout<< "initType:" <<
        //convertToCppType(lhsId->getSymbolInfo()->getType()) << '\n'; ~
        //std::cout<< "===============" << '\n'; ~
        //generateInitkernel1(lhsId,"0"); //TODO
      }
    }
  }
  x = methodID.compare("attachEdgeProperty");
  if(x==0){
    list<argument*> argList = procedure->getArgList();
    list<argument*>::iterator itr;
    for (itr = argList.begin(); itr != argList.end(); itr++) {
      assignment* assign = (*itr)->getAssignExpr();
      char strBuffer[1024];
      Identifier* inId = assign->getId();
      Expression* exprAssigned = assign->getExpr();
      //type of datatype it is of
      const char* inVarType =
          convertToCppType(inId->getSymbolInfo()->getType()->getInnerTargetType());
      const char* inVarName = inId->getIdentifier();
      //cout<<inVarName<<"is of type "<<inVarType<<endl;
      sprintf(strBuffer, "// Initialization for %s variable which is EdgeProperty", inVarName);
      main.pushString(strBuffer);
      main.NewLine();
      if(exprAssigned->isLiteral()){
        //here same value is assigned to property of all the edges
        char valBuffer[1024];
        int expr_valType = exprAssigned->getExpressionFamily();
        switch (expr_valType) {
          case EXPR_INTCONSTANT:
            sprintf(valBuffer, "%s %s_value = %ld;",inVarType ,inVarName,exprAssigned->getIntegerConstant());
            main.pushString(valBuffer);
            main.NewLine();
            break;

          case EXPR_FLOATCONSTANT:
            sprintf(valBuffer, "%s %s_value = %lf;",inVarType ,inVarName,exprAssigned->getFloatConstant());
            main.pushString(valBuffer);
            main.NewLine();
            break;
          case EXPR_BOOLCONSTANT:
          sprintf(valBuffer, "%s %s_value = %s;",inVarType ,inVarName,exprAssigned->getBooleanConstant() ? "true" : "false");
            break;
            default:
            assert(false);

      // return valBuffer;
        }
        main.pushstr_newL("for(int i=0; i< E; i++) {");
        sprintf(strBuffer, "h_%s[i]=%s_value;",inVarName,inVarName);
        main.pushString("}");
        main.pushstr_newL(strBuffer);
        main.NewLine();
      }
      else if(exprAssigned->isProcCallExpr()){
        char valBuffer[1024];
        sprintf(valBuffer, "%s *  %s_array =",inVarType ,inVarName);
        main.pushString(valBuffer);
        proc_callExpr* proc = (proc_callExpr*)exprAssigned;
        string methodId(proc->getMethodId()->getIdentifier());
        list<argument*> argList = proc->getArgList();
        if (argList.size() == 0) {
          Identifier* objectId = proc->getId1();
          sprintf(strBuffer, "%s.%s( );", objectId->getIdentifier(),
              proc->getMethodId()->getIdentifier());
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("for(int i=0; i< E; i++) {");
          sprintf(strBuffer, "h_%s[i]=%s_array[i];",inVarName,inVarName);
          printf("h_%s[i]=%s_array[i];",inVarName,inVarName);
          main.pushString(strBuffer);
          main.NewLine();
          main.pushstr_newL("}");
          main.NewLine();
    }  //here i am assigning values from a different array
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

  char strBuffer[1024];

  Identifier* inId = assign->getId();

  Expression* exprAssigned = assign->getExpr();

  const char* inVarType =
      convertToCppType(inId->getSymbolInfo()->getType()->getInnerTargetType());
  const char* inVarName = inId->getIdentifier();
  //cout<<"set argument for init kernel in main"<<endl;

  cout<<"VarName: "<<inVarName<<endl;

  //cout<<"Generate init kernel in kernel file first"<<endl;
  char* kernelName = (char*)malloc(100*sizeof(char));
  strcpy(kernelName,"init");
  strcat(kernelName,inVarName);
  strcat(kernelName,"_kernel");
  if(!isKenelPresent(kernelName))
  {
    addKernelObject(kernelName);
    sprintf(strBuffer, "__kernel void %s( __global %s *d_%s , %s value, int v",kernelName, inVarType, inVarName,inVarType);
    //cout<<kernelName<<"++++++"<<inVarType<<"++++++++"<<inVarName<<"++++++++";
    printf("__kernel void %s( __global %s *d_%s , %s value, int v",kernelName, inVarType, inVarName,inVarType);
    kernel.pushString(strBuffer);
    kernel.pushString("){");
    kernel.NewLine();
    sprintf(strBuffer, "unsigned int id = get_global_id(0);");
    kernel.pushstr_newL(strBuffer);
    kernel.pushstr_newL("if(id<v){");
    sprintf(strBuffer, "d_%s[id] = value;", inVarName);
    kernel.pushstr_newL(strBuffer);
    kernel.push('}');
    kernel.NewLine();
    kernel.push('}');
    kernel.NewLine();
    // Generate clCreateKernel
    sprintf(strBuffer,"// Creating %s  Kernel",kernelName);
    main.pushString(strBuffer);
    main.NewLine();
    sprintf(strBuffer,"cl_kernel %s = clCreateKernel(program, \"%s\", &status);", kernelName, kernelName);
    main.pushstr_newL(strBuffer);
  }

  main.NewLine();
  sprintf(strBuffer, "// Initialization for %s variable", inVarName);
  printf("// Initialization for %s variable", inVarName);
  main.pushString(strBuffer);
  main.NewLine();

  sprintf(strBuffer, "%s %sValue = (%s)",inVarType, inVarName, inVarType);
  printf("%s %sValue = (%s)",inVarType, inVarName, inVarType);
  main.pushString(strBuffer);

  generateExpr(exprAssigned,1);


  main.pushString("; ");
  main.NewLine();
  sprintf(strBuffer, "status = clSetKernelArg(%s, 0 , sizeof(cl_mem), (void *)& d_%s);",kernelName , inVarName);
  main.pushString(strBuffer);
  main.NewLine();
  sprintf(strBuffer, "status = clSetKernelArg(%s, 1, sizeof(%s) , (void*)& %sValue);", kernelName , inVarType, inVarName);
  main.pushString(strBuffer);
  main.NewLine();
  sprintf(strBuffer, "status = clSetKernelArg(%s, 2, sizeof(int), (void*)&V);", kernelName);
  main.pushString(strBuffer);
  main.NewLine();
  //Launch the kernel from main
  main.pushstr_newL("local_size = 128;");
  sprintf(strBuffer,"global_size = %s;",inId->getSymbolInfo()->getType()->isPropNodeType()?"(V + local_size -1)/ local_size * local_size":"(E + local_size -1)/ local_size * local_size" );
  main.pushstr_newL(strBuffer);

 sprintf(strBuffer , "status = clEnqueueNDRangeKernel(command_queue, %s, 1, NULL, &global_size, &local_size, 0,NULL,&event);",kernelName);
 main.pushstr_newL(strBuffer);
 main.NewLine();
 addProfilling("event");
 sprintf(strBuffer ,"status = clEnqueueReadBuffer(command_queue, d_%s, CL_TRUE, 0, sizeof(%s) * V, h_%s, 0, NULL, NULL);",inVarName,inVarType,inVarName);
 main.pushstr_newL(strBuffer);
 main.NewLine();
 sprintf(strBuffer, "status = clReleaseKernel(%s);",kernelName);
 main.pushstr_newL(strBuffer);
 /*
 sprintf(strBuffer,"printf(\" time  spent in %s kernel = ", kernelName);
 main.pushString(strBuffer);
 main.pushString("%lf ms \\n \", kernelTime);");
 */
 main.NewLine();
}
/********************************************End of Generate statements**********************************/


// this function allocates buffer for property of node or edge
void dsl_cpp_generator::generateCudaMallocParams(list<formalParam*> paramList)
{
  list<formalParam*>::iterator itr;
  char strBuffer[1024];
  for(itr=paramList.begin();itr!=paramList.end();itr++)
  {
    Type* type=(*itr)->getType();
    if (type->isPropType()) 
    {
      if (type->getInnerTargetType()->isPrimitiveType())
      {
        Type* innerType = type->getInnerTargetType();
        // for the host lets generate Here
        /*
        const char* size = (type->isPropNodeType())?"v":"e";
        char* temp = (*itr)->getIdentifier()->getIdentifier();
        char* varName = (char*)malloc((5+ strlen(temp))*sizeof(char));
        strcpy(varName,"h_");
        strcat(varName,temp);
        const char *cpptype = convertToCppType(innerType);
        sprintf(strBuffer,"%s *%s =(%s *)malloc(%s*sizeof(%s));",cpptype, varName,cpptype,size,cpptype);
        main.pushstr_newL(strBuffer);
        */
        generateCudaMalloc(type, (*itr)->getIdentifier()->getIdentifier());

      }
    }
    if(type->isPrimitiveType()) // for the premittive type just declare those variables in device memory
    {
      // Type* innerType = type->getInnerTargetType();
      Identifier* iden = (*itr)->getIdentifier();
      sprintf(strBuffer, "%s *h_%s;",convertToCppType(type), iden->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer, "cl_mem d_%s = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(%s),NULL, &status);",
      iden->getIdentifier(),convertToCppType(type));
      main.pushstr_newL(strBuffer);
      main.NewLine();
    }
  }
}

void dsl_cpp_generator::generateDeviceAssignmentStmt(assignment* asmt, int isMainFile) 
{
  dslCodePad& targetFile = getTargetFile(isMainFile);
  cout<<"Inside generateDeviceAssignmentStmt"<<endl;
  bool isDevice = false;
  char strBuffer[300];
  if (asmt->lhs_isIdentifier()) {
    Identifier* id = asmt->getId();
    if(isMainFile==2)targetFile.pushString("d_");
    targetFile.pushString(id->getIdentifier());
    targetFile.pushString(" = ");
    generateExpr(asmt->getExpr(), isMainFile);
    targetFile.pushstr_newL(";");
    targetFile.NewLine();
  }
  else if (asmt->lhs_isProp())  
  {
    // the check for node and edge property to be carried out.
    

    PropAccess* propId = asmt->getPropId();
    if (asmt->isDeviceAssignment()) 
    {
      isDevice = true;
      //~ src.dist = 0; ===>  initIndex() KERNEL call
      //Type *typeA = propId->getIdentifier1()->getSymbolInfo()->getType()->getInnerTargetType();
      Type *typeB = propId->getIdentifier2()->getSymbolInfo()->getType()->getInnerTargetType();
      //~ targetFile.pushstr_newL(convertToCppType(typeB));
      //~ targetFile.pushstr_newL(convertToCppType(typeA));

      const char *varTypeB = convertToCppType(typeB); //DONE: get the type from id
      //const char *varTypeA = convertToCppType(typeA);
      // Generate InitIndexVarName kernel in kernel file

      
      
        // cout<<"yaha p possible segmentation fault\n";
        // Identifier *inId= propId->getIdentifier2();
        // if(propId->getIdentifier2()==NULL){
        //   cout<<"oh my god\n";
        // }
        if(propId->getIdentifier2()->getSymbolInfo()->getType()->isPropNodeType()){
          sprintf(strBuffer, "h_%s[%s] = ", propId->getIdentifier2()->getIdentifier(),
              propId->getIdentifier1()->getIdentifier());
          main.pushString(strBuffer);
          generateExpr(asmt->getExpr(), isMainFile);
          targetFile.pushstr_newL(";");
          targetFile.NewLine();

        
        }
        else if(isMainFile==1){
          
          //sprintf(strBuffer, "for(int i=h_meta[%s_src]; i< h_meta[%s_src+1]; i++) {",propId->getIdentifier1()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
          //targetFile.pushString(strBuffer);
          //targetFile.NewLine();
          //sprintf(strBuffer, "int v = h_data[i]");
          // targetFile.pushString(strBuffer);
          // targetFile.NewLine();
          // sprintf(strBuffer, "if(v==%s_des){",propId->getIdentifier1()->getIdentifier());
          // targetFile.pushString(strBuffer);
          // targetFile.NewLine();
          sprintf(strBuffer,"h_%s[%s_ind]=",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
          targetFile.pushString(strBuffer);
          generateExpr(asmt->getExpr(), isMainFile);
          targetFile.pushstr_newL(";");
          targetFile.NewLine();
          //targetFile.pushString("}");
         // targetFile.NewLine();
          //targetFile.pushString("}");
          
        }
        else if(isMainFile==2){
          sprintf(strBuffer,"d_%s[%s_ind]=",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
          targetFile.pushString(strBuffer);
          generateExpr(asmt->getExpr(), isMainFile);
          targetFile.pushstr_newL(";");
          targetFile.NewLine();
          
        }
        
        
        
    }
    else {
      //cout<<"inside the second else part of the assignment\n";
      targetFile.pushString("d_");  /// IMPORTANT
      targetFile.pushString(propId->getIdentifier2()->getIdentifier());
      targetFile.push('[');
      targetFile.pushString(propId->getIdentifier1()->getIdentifier());
      targetFile.push(']');
    }
  }

    
  
    
}

// This function is not in Use.
void dsl_cpp_generator::checkForAllAndGenerate(blockStatement* blockStmt , int isMainFile)
{
  dslCodePad& targetFile = getTargetFile(isMainFile);
  list<statement*> stmtList = blockStmt->returnStatements();
  list<statement*>::iterator itr;
  char strBuffer[1024];
  //cout<<"Entered CheckForAllAndGenerate Function"<<endl;
  for(itr = stmtList.begin(); itr!= stmtList.end(); itr++)
  {
    statement* stmt = *itr;
    if(stmt->getTypeofNode() == NODE_FORALLSTMT)
    {
      forallStmt* forAll  =(forallStmt*)stmt;
      if(forAll->isForall())
      {
        //Genearate the Create kernal and clSetKernelArg
        char *kernelName = getCurrentFunc()->getIdentifier()->getIdentifier();
        char strBuffer[1024];
        targetFile.NewLine();
        targetFile.pushString("// Creating kaernel from program file");
        targetFile.NewLine();
        if(!isKenelPresent(kernelName))
        {
            sprintf(strBuffer,"cl_kernel %s = clCreateKernel(program,\"%s_kernel\",&status);", kernelName, kernelName);
            targetFile.pushString(strBuffer);
            targetFile.NewLine();
            addKernelObject(kernelName);
        }
        sprintf(strBuffer, "status = clSetKernelArg(%s, 0, sizeof(int) , (void*)&V);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 1, sizeof(int) , (void*)&E);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 2, sizeof(cl_mem) , (void*)&d_meta);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 3, sizeof(cl_mem) , (void*)&d_data);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 4, sizeof(cl_mem) , (void*)&d_src);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 5, sizeof(cl_mem) , (void*)&d_weight);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 6, sizeof(cl_mem) , (void*)&d_rev_meta);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(%s, 7, sizeof(cl_mem) , (void*)&d_modified_next);", kernelName);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        usedVariables usedVars = getVarsForAllAMD(forAll);
        list<Identifier*> vars = usedVars.getVariables();
        int argCount = 8;
        // If parent is fixedpoint Node then get the fixpoint variable and add it to the kernel that means
        // generate clSetKernelArg for that fixPoint variable
        ASTNode* p =forAll->getParent();
        cout<<"parent of for::"<<p->getParent()->getTypeofNode()<<endl;
        if(p->getParent()->getTypeofNode()==NODE_FIXEDPTSTMT)
        {
          cout<<"parent of for is NODE_FIXPTSTMT"<<endl;
          sprintf(strBuffer, "status = clSetKernelArg(%s, %d , sizeof(cl_mem), (void*)&d_finished);",kernelName, argCount);
          targetFile.pushstr_newL(strBuffer);
          argCount++;
        }
        for(Identifier* iden: vars)
        {
          if(iden->getSymbolInfo()==NULL) continue;
          Type* type = iden->getSymbolInfo()->getType();
          if(type->isPropType())
          {
            sprintf(strBuffer,"status = clSetKernelArg(%s, %d , sizeof(cl_mem) , (void*)&d_%s);",
                kernelName,argCount,iden->getIdentifier());
              targetFile.pushString(strBuffer);
              targetFile.NewLine();
              argCount++;
          }
        }
        
       
        break;
      }
    } 
  }
  //cout<<"Exiting checkForForAll"<<endl;
  return;
}

void dsl_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct,int isMainFile)
{
  dslCodePad& targetFile = getTargetFile(isMainFile);

  std::cout<< "IN FIX PT" << '\n';
  char strBuffer[1024];
  Expression* convergeExpr = fixedPointConstruct->getDependentProp();
  Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();


  //~ statement* blockStmt = fixedPointConstruct->getBody();
  assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
         convergeExpr->getExpressionFamily() == EXPR_ID);


  Identifier* dependentId = NULL;
  //~ bool isNot = false;
  assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
         convergeExpr->getExpressionFamily() == EXPR_ID);
  if (convergeExpr->getExpressionFamily() == EXPR_UNARY) {
    if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
      dependentId = convergeExpr->getUnaryExpr()->getId();
      //~ isNot = true;
    }
  }

  const char *modifiedVar   = dependentId->getIdentifier();
  char *fixPointVar = fixedPointId->getIdentifier();
  //fixed point variable's data type 
  const char *fixPointVarType = convertToCppType(fixedPointId->getSymbolInfo()->getType());
  // The variable which is getting modifies inside fixed point
  char modifiedVarNext[80] = "d_" ;
  strcat(modifiedVarNext, modifiedVar);strcat(modifiedVarNext, "_next");

  if (convergeExpr->getExpressionFamily() == EXPR_ID)
    dependentId = convergeExpr->getId();
  if (dependentId != NULL) {
     //~ std::cout<< "GENERATING FIX PT" << '\n';
    if (dependentId->getSymbolInfo()->getType()->isPropType()) {
      if (dependentId->getSymbolInfo()->getType()->isPropNodeType()) {
        targetFile.NewLine();
        targetFile.pushString("// Start of fixed point");
        targetFile.NewLine();
        /*
        // create clmem corresponding to fixPointVar in main codepad and to mapping/implement pinned memory
        sprintf(strBuffer,"d_%s = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR,sizeof(int), NULL, &status);", fixPointVar);
        main.NewLine();
        main.pushString(strBuffer);
        // Add the memory object to list
        char *str = (char*)malloc(100*sizeof(char));
        strcpy(str,"d_");
        strcat(str, fixPointVar);
        addMemObject(str);
        main.NewLine();
        sprintf(strBuffer, "h_%s = (%s *)clEnqueueMapBuffer(command_queue, d_%s , CL_TRUE, CL_MAP_READ|CL_MAP_WRITE, 0,sizeof(%s),0,NULL,NULL,&status);",
            fixPointVar, fixPointVarType, fixPointVar,fixPointVarType);
        main.pushString(strBuffer);
        main.NewLine();
        */
        // initailize modified_next
        // Create kernel for modified_next
        // Add a if condition Here, What if I have created a kernel for this already 
        // Don't want to repeat
        char *kernelName = (char*)malloc(100*sizeof(char));
        strcpy(kernelName,"init");
        strcat(kernelName,modifiedVarNext);
        if(!isKenelPresent(kernelName))
        {
          addKernelObject(kernelName);
            sprintf(strBuffer, "__kernel void %s_kernel(__global %s *%s, %s val ,  int v){", 
                kernelName, fixPointVarType, modifiedVarNext, fixPointVarType);
            kernel.pushString(strBuffer);
            kernel.NewLine();
            kernel.pushString("unsigned int id = get_global_id(0);");
            kernel.NewLine();
            kernel.pushString("if(id<v)");
            kernel.NewLine();
            kernel.push('{');
            kernel.NewLine();
            sprintf(strBuffer, "%s[id] = val;",modifiedVarNext);
            kernel.pushString(strBuffer);
            kernel.NewLine();
            kernel.push('}');
            kernel.NewLine();
            kernel.push('}');
            kernel.NewLine();
            // need to create kernel in main file
            main.pushstr_newL("// creating %s kernel");
            sprintf(strBuffer,"cl_kernel %s = clCreateKernel(program, \"%s_kernel\", &status);", kernelName,kernelName);
            main.pushstr_newL(strBuffer);
        }
        
        
        //Set kernel arguments in main for modified_next
        sprintf(strBuffer, "status = clSetKernelArg(init%s, 0,sizeof(cl_mem),(void*)&%s);", modifiedVarNext, modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        sprintf(strBuffer,"%s %sValue = false;", fixPointVarType, modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        sprintf(strBuffer,"status = clSetKernelArg(init%s, 1,sizeof(%s) ,(void*)&%sValue);", modifiedVarNext,fixPointVarType, modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(init%s, 2 , sizeof(int),(void*)&V);", modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        //call the kernel of modified_next
        
        main.pushString("local_size = 128;");
        main.NewLine();
        main.pushString("global_size = (V + local_size -1)/ local_size * local_size;");
        main.NewLine();
        sprintf(strBuffer, "status = clEnqueueNDRangeKernel(command_queue, init%s , 1, NULL , &global_size , &local_size ,0, NULL, &event);",
                  modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        addProfilling("event");
        sprintf(strBuffer, "status = clReleaseKernel(init%s);", modifiedVarNext);
        main.pushstr_newL(strBuffer);
        
        //Generate while
        targetFile.NewLine();
        targetFile.pushString("int k = 0;");
        targetFile.NewLine();
        sprintf(strBuffer,"%s = false;",fixPointVar );
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        sprintf(strBuffer,"while(!(%s)){", fixPointVar);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();

        // assert(graphId.size() == 1);
        sprintf(strBuffer,"%s = true;", fixPointVar);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        // Line below assumes there is a kernel call whithin fixedPoint construct
        sprintf(strBuffer, " status  = clEnqueueWriteBuffer(command_queue,d_%s, CL_TRUE, 0,sizeof(int), &%s,0,0,NULL);",fixPointVar, fixPointVar);
        targetFile.pushstr_newL(strBuffer);
        targetFile.NewLine();
        // Check for ForAll statement if yes then generate create kernel and set kernel argument
        //checkForAllAndGenerate((blockStatement*)fixedPointConstruct->getBody(),1);

        //Generates Inner parts of fixedPoint
        if (fixedPointConstruct->getBody()->getTypeofNode() != NODE_BLOCKSTMT)
            generateStatement(fixedPointConstruct->getBody(), isMainFile);
        else
          {
            generateBlock((blockStatement*)fixedPointConstruct->getBody(), false, isMainFile);
          }
            
        // generate clEnqueReadBuffer for d_modifiednext->h_modified;
        sprintf(strBuffer,"status = clEnqueueCopyBuffer(command_queue, %s ,d_%s, 0,0, %s*sizeof(%s),0,NULL, NULL);",
                modifiedVarNext,modifiedVar,dependentId->getSymbolInfo()->getType()->isPropNodeType()?"V":"E", 
                convertToCppType(dependentId->getSymbolInfo()->getType()->getInnerTargetType()));
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        // need to create kernel in main file
        main.pushstr_newL("// creating %s kernel");
        sprintf(strBuffer,"cl_kernel %s = clCreateKernel(program, \"%s_kernel\", &status);", kernelName,kernelName);
        main.pushstr_newL(strBuffer);
        
        //Set kernel arguments in main for modified_next
        sprintf(strBuffer, "status = clSetKernelArg(init%s, 0,sizeof(cl_mem),(void*)&%s);", modifiedVarNext, modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        sprintf(strBuffer,"%s %sValue = false;", fixPointVarType, modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        sprintf(strBuffer,"status = clSetKernelArg(init%s, 1,sizeof(%s) ,(void*)&%sValue);", modifiedVarNext,fixPointVarType, modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();
        sprintf(strBuffer, "status = clSetKernelArg(init%s, 2 , sizeof(int),(void*)&V);", modifiedVarNext);
        main.pushString(strBuffer);
        main.NewLine();

        targetFile.pushString("local_size = 128;");
        targetFile.NewLine();
        targetFile.pushString("global_size = (V + local_size -1)/ local_size * local_size;");
        targetFile.NewLine();

        sprintf(strBuffer, "status = clEnqueueNDRangeKernel(command_queue, init%s , 1,NULL, &global_size , &local_size ,0, NULL, &event);",
                  modifiedVarNext);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        addProfilling("event");
        sprintf(strBuffer, "status = clReleaseKernel(init%s);", modifiedVarNext);
        main.pushstr_newL(strBuffer);
        // something is there to do
        
        sprintf(strBuffer, "status =clEnqueueReadBuffer(command_queue, d_%s , CL_TRUE, 0, sizeof(int), &%s, 0, NULL, NULL );", fixPointVar, fixPointVar);
        targetFile.pushstr_newL(strBuffer);
        targetFile.NewLine();

        targetFile.pushString("k++;");
        targetFile.NewLine();
        targetFile.pushstr_newL("} // End of While");
        targetFile.NewLine();
        // Unmap the mapped memory
        targetFile.pushString("//Unmapping the mapped memory");
        targetFile.NewLine();
        sprintf(strBuffer,"clEnqueueUnmapMemObject(command_queue, d_%s, h_%s, 0, NULL, NULL);",fixPointVar,fixPointVar);
        targetFile.pushString(strBuffer);
        targetFile.NewLine();
        
      }
    }
  }
  targetFile.pushstr_newL("// END FIXED POINT");
  targetFile.NewLine();

  
}



void dsl_cpp_generator::generateCudaMemCpySymbol(char* var, const char* typeStr, bool direction)
{
  cout<<"Inside genrateCudaMemCpySymbol"<<endl;
  char strBuffer[1024];
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3
  // direction true => host--> device
  // direction false => device->host
  if(direction)
  {
    main.NewLine();
    sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue, d_%s, CL_TRUE, 0, sizeof(%s),&%s , 0, NULL, NULL);",
          var, typeStr,var);
    main.pushString(strBuffer);
    main.NewLine();
  }
  else
  {
     main.NewLine();
    sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue, d_%s, CL_TRUE, 0, sizeof(%s),&%s , 0, NULL, NULL);",
          var, typeStr,var);
    main.pushString(strBuffer);
    main.NewLine();
    // main.pushstr_newL("clWaitForEvents(1,&event);");
  }
}


blockStatement* dsl_cpp_generator::includeIfToBlock(forallStmt* forAll) {
  Expression* filterExpr = forAll->getfilterExpr();
  //~ statement* body = forAll->getBody();
  Expression* modifiedFilterExpr = filterExpr;
  if (filterExpr->getExpressionFamily() == EXPR_RELATIONAL) {
    Expression* expr1 = filterExpr->getLeft();
    Expression* expr2 = filterExpr->getRight();
    if (expr1->isIdentifierExpr()) {
      /*if it is a nodeproperty, the filter is on the nodes that are iterated on
       One more check can be applied to check if the iterating type is a neigbor
       iteration or allgraph iterations.
      */
      if (expr1->getId()->getSymbolInfo() != NULL) {
        if (expr1->getId()->getSymbolInfo()->getType()->isPropNodeType()) {
          Identifier* iterNode = forAll->getIterator();
          Identifier* nodeProp = expr1->getId();
          PropAccess* propIdNode = (PropAccess*)Util::createPropIdNode(iterNode, nodeProp);
          Expression* propIdExpr = Expression::nodeForPropAccess(propIdNode);
          modifiedFilterExpr = (Expression*)Util::createNodeForRelationalExpr(propIdExpr, expr2, filterExpr->getOperatorType());
        }
      }
    }
    /* if(expr1->isPropIdExpr()&&expr2->isBooleanLiteral()) //specific to sssp.
     Need to revisit again to change it. {   PropAccess*
     propId=expr1->getPropId(); bool value=expr2->getBooleanConstant();
         Expression* exprBool=(Expression*)Util::createNodeForBval(!value);
         assignment*
     assign=(assignment*)Util::createAssignmentNode(propId,exprBool);
         if(body->getTypeofNode()==NODE_BLOCKSTMT)
         {
           blockStatement* newbody=(blockStatement*)body;
           newbody->addToFront(assign);
           body=newbody;
         }

         modifiedFilterExpr = filterExpr;
     }
   */
  }
  ifStmt* ifNode = (ifStmt*)Util::createNodeForIfStmt(modifiedFilterExpr,
                                                      forAll->getBody(), NULL);
  blockStatement* newBlock = new blockStatement();
  newBlock->setTypeofNode(NODE_BLOCKSTMT);
  newBlock->addStmtToBlock(ifNode);
  return newBlock;

}

void dsl_cpp_generator::addCudaKernel(forallStmt* forAll)
{
  cout<<"inside addCuda Kernel function"<<endl;
  const char* loopVar = "v";// but it could be 'e' also
  char strBuffer[1024];
  usedVariables usedVars = getVarsForAllAMD(forAll);
  list<Identifier*> vars = usedVars.getVariables();
  kernel.pushString("__kernel void ");
  kernel.pushString(getCurrentFunc()->getIdentifier()->getIdentifier());
  kernel.pushString("_kernel");
  kernel.pushString("(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,");
  kernel.NewLine();
  kernel.pushString("__global int* d_weight,__global int* d_rev_meta ");

   // If parent is fixed point we need to padd modified_next and finished
    if(forAll->getParent()->getParent()!=NULL)
    {
      statement* parent = (statement*) forAll->getParent()->getParent();
      cout<<parent->getTypeofNode()<<endl;
      if(parent->getTypeofNode()== NODE_FIXEDPTSTMT)
      {
         fixedPointStmt* fixedPointConstruct = (fixedPointStmt*)parent;
        Expression* convergeExpr = fixedPointConstruct->getDependentProp();
        Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
        sprintf(strBuffer, ",__global int *%s",fixedPointId->getIdentifier());
        kernel.pushString(strBuffer);
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
          convergeExpr->getExpressionFamily() == EXPR_ID);


        Identifier* dependentId = NULL;

        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
              convergeExpr->getExpressionFamily() == EXPR_ID);
        if (convergeExpr->getExpressionFamily() == EXPR_UNARY) {
          if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
            dependentId = convergeExpr->getUnaryExpr()->getId();
            //~ isNot = true;
            
            sprintf(strBuffer, ",__global %s d_%s_next", convertToCppType(dependentId->getSymbolInfo()->getType()),dependentId->getIdentifier());
            kernel.pushString(strBuffer);
          }
        }
      }
    }


  for(Identifier* iden: vars)
  {
    if(iden->getSymbolInfo()==NULL) continue; 
    Type* type = iden->getSymbolInfo()->getType();
    if(type->isPropType())
    {
      sprintf(strBuffer, ",__global %s d_%s", convertToCppType(type), iden->getIdentifier());
      kernel.pushString(/*createParamName(*/strBuffer);
      cout<<"`````````````````pushing parameter "<<iden->getIdentifier()<<endl;
    }
    else if( type->isPrimitiveType())
    {
      sprintf(strBuffer, ",__global %s* d_%s", convertToCppType(type), iden->getIdentifier());
      kernel.pushString(/*createParamName(*/strBuffer);
      PremVarOfKernel.push_back(iden);
      cout<<"```````````````````````pushing parameter "<<iden->getIdentifier()<<endl;
    }
  }
  kernel.pushstr_newL("){ // BEGIN KER FUN via ADDKERNEL");
  sprintf(strBuffer, "unsigned int %s = get_global_id(0);", loopVar);
  //kernel.pushstr_newL("int num_nodes  = V;");
  kernel.pushstr_newL(strBuffer);

  sprintf(strBuffer, "if(%s >= V) return;", loopVar);
  kernel.pushstr_newL(strBuffer);
  

  if (forAll->hasFilterExpr()) {
    blockStatement* changedBody = includeIfToBlock(forAll);
    cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
    forAll->setBody(changedBody);
    // cout<<"FORALL BODY
    // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
  }
  statement* body = forAll->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement* block = (blockStatement*)body;
  list<statement*>statementList = block->returnStatements();

  printf("start of kernel block \n");

  for (statement* stmt : statementList) {
    generateStatement(stmt, 2);      //2 means in kernelFile. All these stmts should be inside kernel
                                     //~ if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
  }
  kernel.pushstr_newL("} // end KER FUNC");


}

bool dsl_cpp_generator::allGraphIteration(char* methodId) {
  string methodString(methodId);

  return (methodString == "nodes" || methodString == "edges");
}
bool dsl_cpp_generator::neighbourIteration(char* methodId) {
  string methodString(methodId);
  return (methodString == "neighbors" || methodString == "nodes_to");
}

bool dsl_cpp_generator::elementsIteration(char* extractId) {
  string extractString(extractId);
  return (extractString == "elements");
}

void dsl_cpp_generator::generateForAllSignature(forallStmt* forAll, int isMainFile) {

  cout << "GenerateForAllSignature , isMainfile = " << isMainFile << endl;
  dslCodePad& targetFile = getTargetFile(isMainFile);

  char strBuffer[1024];
  Identifier* iterator = forAll->getIterator();
  if (forAll->isSourceProcCall()) {
    //~ Identifier* sourceGraph = forAll->getSourceGraph();
    proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
    Identifier* iteratorMethodId = extractElemFunc->getMethodId();
    if (allGraphIteration(iteratorMethodId->getIdentifier())) {
      // char* graphId=sourceGraph->getIdentifier();
      // char* methodId=iteratorMethodId->getIdentifier();
      // string s(methodId);
      // if(s.compare("nodes")==0)
      //{
      // cout<<"INSIDE NODES VALUE"<<"\n";
      // sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
      // ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_nodes",iterator->getIdentifier());
      //}
      // else
      //;
      // sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
      // ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_edges",iterator->getIdentifier());

      // main.pushstr_newL(strBuffer);

    } else if (neighbourIteration(iteratorMethodId->getIdentifier())) {
      //~ // THIS SHOULD NOT BE EXECUTING FOR SIMPLE FOR LOOP BUT IT IS SO .
      //~ // COMMENTED OUT FOR NOW.
      //~ char* graphId=sourceGraph->getIdentifier();
      char* methodId=iteratorMethodId->getIdentifier();
      string s(methodId);
      if(s.compare("neighbors")==0)
      {
        list<argument*>  argList=extractElemFunc->getArgList();
        assert(argList.size()==1);
        //~ Identifier* nodeNbr=argList.front()->getExpr()->getId();
        //~ sprintf(strBuffer,"for (int edge = d_meta[v]; %s < %s[%s+1]; %s++) { // ","int","edge","d_meta","v","edge","d_meta","v","edge");
        sprintf(strBuffer,"for (%s %s = %s[%s]; %s < %s[%s+1]; %s++) { // FOR NBR ITR ","int","edge","d_meta","v","edge","d_meta","v","edge");
        targetFile.pushstr_newL(strBuffer);
        //~ targetFile.pushString("{");
        sprintf(strBuffer,"%s %s = %s[%s];","int",iterator->getIdentifier(),"d_data","edge"); //needs to move the addition of
        targetFile.pushstr_newL(strBuffer);
        isNeighbourIter = true;
      }
      if(s.compare("nodes_to")==0) //for pageRank
      {
        list<argument*> argList=extractElemFunc->getArgList();
        assert(argList.size()==1);
        Identifier* nodeNbr=argList.front()->getExpr()->getId();
        sprintf(strBuffer,"for (%s %s = %s[%s]; %s < %s[%s+1]; %s++)","int","edge","d_rev_meta",nodeNbr->getIdentifier(),"edge","d_rev_meta",nodeNbr->getIdentifier(),"edge");
        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("{");
        sprintf(strBuffer,"%s %s = %s[%s] ;","int",iterator->getIdentifier(),"d_src","edge"); //needs to move the addition of
         targetFile.pushstr_newL(strBuffer);
         isNeighbourIter = false;
      } //statement to  a different method.
    }
  } else if (forAll->isSourceField()) {
    /*PropAccess* sourceField=forAll->getPropSource();
    Identifier* dsCandidate = sourceField->getIdentifier1();
    Identifier* extractId=sourceField->getIdentifier2();

    if(dsCandidate->getSymbolInfo()->getType()->gettypeId()==TYPE_SETN)
    {

      main.pushstr_newL("std::set<int>::iterator itr;");
      sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",dsCandidate->getIdentifier(),dsCandidate->getIdentifier());
      main.pushstr_newL(strBuffer);

    }


    if(elementsIteration(extractId->getIdentifier()))
      {
        Identifier* collectionName=forAll->getPropSource()->getIdentifier1();
        int typeId=collectionName->getSymbolInfo()->getType()->gettypeId();
        if(typeId==TYPE_SETN)
        {
          main.pushstr_newL("std::set<int>::iterator itr;");
          sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",collectionName->getIdentifier(),collectionName->getIdentifier());
          main.pushstr_newL(strBuffer);
        }

      }*/

  } else {
    //~ std::cout << "+++++++++++++++" << '\n';
    Identifier* sourceId = forAll->getSource();
    if (sourceId != NULL) {
      if (sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {  //FOR SET
        //~ std::cout << "+++++     ++++++++++" << '\n';
        main.pushstr_newL("//FOR SIGNATURE of SET - Assumes set for on .cu only");
        main.pushstr_newL("std::set<int>::iterator itr;");
        sprintf(strBuffer, "for(itr=%s.begin();itr!=%s.end();itr++) ",sourceId->getIdentifier(), sourceId->getIdentifier());
        main.pushstr_newL(strBuffer);
      }
    }
  }
  cout<<"exiting generateSignature"<<endl;
}


void dsl_cpp_generator::generateForAll(forallStmt* forAll, int isMainFile) 
{
  dslCodePad& targetFile = getTargetFile(isMainFile);
  targetFile.NewLine();
  targetFile.pushString("//ForAll started here");
  targetFile.NewLine();
  proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
  PropAccess* sourceField = forAll->getPropSource();
  Identifier* sourceId = forAll->getSource();
  Identifier* collectionId = NULL;
  if (sourceField != NULL) 
  {
    collectionId = sourceField->getIdentifier1();
  }
  if (sourceId != NULL) 
  {
    collectionId = sourceId;
    //cout<<"SourceId = "<<sourceId->getIdentifier()<<endl;
  }
  Identifier* iteratorMethodId;
  if (extractElemFunc != NULL)
    iteratorMethodId = extractElemFunc->getMethodId();
  statement* body = forAll->getBody();
  char strBuffer[1024];
  if (forAll->isForall())// IS FORALL
  { 
    printf("Entered here for forall \n");
    usedVariables usedVars = getVarsForAllAMD(forAll);
    list<Identifier*> vars = usedVars.getVariables();
    cout<<"checkPoint 1 "<< vars.size()<<endl;
    // If there is any variable of premitive data type then need to do transfer it to device
    for(Identifier* iden: vars){
        //cout<<"CheckPoint 2"<<endl;
        //TableEntry* te = iden->getSymbolInfo();
        cout<<"Variable from ForAll:"<<iden->getIdentifier()<<endl;
        if(iden->getSymbolInfo()==NULL) continue; // for (v in g.nodes()) for 'v' I didn't find an enty in table
        Type* type = iden->getSymbolInfo()->getType();

        if(type->isPrimitiveType())
          generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
        // We will not transfer proptypes as we have already done it
        // For proptype data need to Create clSetKernelArg
        
    }
    //Create a kernel Event
    main.NewLine();
    char strBuffer[1024];
     char *KernelName = getCurrentFunc()->getIdentifier()->getIdentifier();
     string kstr= to_string(kernel_counter++);
    const char *kstrr=kstr.c_str();
    strcat(KernelName, kstrr);
     addKernelObject(KernelName);
     sprintf(strBuffer, "cl_kernel %s = clCreateKernel(program, \"%s_kernel\", &status);", KernelName, KernelName);
     main.pushstr_newL(strBuffer);


     main.pushstr_newL("if(status != CL_SUCCESS){");

     sprintf(strBuffer, "printf(\"Failed to create %s kernel.\\n\");", KernelName);
     main.pushstr_newL(strBuffer);
     main.pushstr_newL("return ;");
     main.pushstr_newL("}");
    // set kernel Argument


    //so before setting the kernel argument we have to make sure that the arguments are well updated that is before the kernel launch copy the required data from the
    //host to device and after the kernel launch copy the data back to host from the device

        if(forAll->getParent()->getParent()!=NULL)
    {
      cout<<"Parent is not null, parent node = ";
      statement* parent = (statement*) forAll->getParent()->getParent();
      cout<<parent->getTypeofNode()<<endl;
      if(parent->getTypeofNode()== NODE_FIXEDPTSTMT)
      {
         cout<<"Parent is fixed point"<<endl;
         fixedPointStmt* fixedPointConstruct = (fixedPointStmt*)parent;
        Expression* convergeExpr = fixedPointConstruct->getDependentProp();
        Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
        if(fixedPointId->getSymbolInfo()->getType()->isPropNodeType()){
          sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),fixedPointId->getIdentifier());
          main.pushstr_newL(strBuffer);
        }
        else if(fixedPointId->getSymbolInfo()->getType()->isPropEdgeType()){
          sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),fixedPointId->getIdentifier());
          main.pushstr_newL(strBuffer);
        }
        else {
          sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),fixedPointId->getIdentifier());
          main.pushstr_newL(strBuffer);
        }
        
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
          convergeExpr->getExpressionFamily() == EXPR_ID);


        Identifier* dependentId = NULL;
        //~ bool isNot = false;
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
              convergeExpr->getExpressionFamily() == EXPR_ID);
        if (convergeExpr->getExpressionFamily() == EXPR_UNARY) {
          if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
            dependentId = convergeExpr->getUnaryExpr()->getId();
            //~ isNot = true;
            if(dependentId->getSymbolInfo()->getType()->isPropNodeType()){
            sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer);
          }
          else if(dependentId->getSymbolInfo()->getType()->isPropEdgeType()){
            sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer);
          }
          else{
            sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer); 
          }
          }
        }
      }
    }

  for(Identifier* iden: vars)
  {
    if(iden->getSymbolInfo()==NULL) continue; 
    Type* type = iden->getSymbolInfo()->getType();
    if(type->isPrimitiveType())
    {
      sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropNodeType()){
      sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropEdgeType()){
      sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);
    }
    
  }
   
    int argCount = 0;
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(int),(void *)&V);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(int),(void *)&E);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_meta);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_data);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_src);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_weight);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_rev_meta);", KernelName, argCount++);
    main.pushstr_newL(strBuffer);
    
     // If parent is fixed point we need to padd modified_next and finished
    if(forAll->getParent()->getParent()!=NULL)
    {
      cout<<"Parent is not null, parent node = ";
      statement* parent = (statement*) forAll->getParent()->getParent();
      cout<<parent->getTypeofNode()<<endl;
      if(parent->getTypeofNode()== NODE_FIXEDPTSTMT)
      {
         cout<<"Parent is fixed point"<<endl;
         fixedPointStmt* fixedPointConstruct = (fixedPointStmt*)parent;
        Expression* convergeExpr = fixedPointConstruct->getDependentProp();
        Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
        sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_%s);", KernelName, argCount++, fixedPointId->getIdentifier());
        main.pushstr_newL(strBuffer);
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
          convergeExpr->getExpressionFamily() == EXPR_ID);


        Identifier* dependentId = NULL;
        //~ bool isNot = false;
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
              convergeExpr->getExpressionFamily() == EXPR_ID);
        if (convergeExpr->getExpressionFamily() == EXPR_UNARY) {
          if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
            dependentId = convergeExpr->getUnaryExpr()->getId();
            //~ isNot = true;

            sprintf(strBuffer, "status = clSetKernelArg(%s,%d, sizeof(cl_mem),(void *)&d_%s_next);", KernelName, argCount++, dependentId->getIdentifier());
            main.pushstr_newL(strBuffer);
          }
        }
      }
    }

    for(Identifier* iden: vars)
  {
    if(iden->getSymbolInfo()==NULL) continue; 
    Type* type = iden->getSymbolInfo()->getType();
    if(type->isPropType() || type->isPrimitiveType())
    {
      sprintf(strBuffer, "status = clSetKernelArg(%s, %d, sizeof(cl_mem), (void *)&d_%s);", KernelName, argCount++, iden->getIdentifier());
      main.pushstr_newL(strBuffer);
    }
    
  }

    main.NewLine();
   
    main.pushstr_newL("local_size = 128;");
     main.pushstr_newL("global_size = (V + local_size -1)/ local_size * local_size;");
     main.NewLine();
    // Call/Invoke the kernel from here
    sprintf(strBuffer,"status = clEnqueueNDRangeKernel(command_queue,%s, 1,NULL, &global_size, &local_size , 0,NULL,&event);",
          KernelName);
    main.pushstr_newL(strBuffer);
    addProfilling("event");
    main.NewLine();
    sprintf(strBuffer, "status = clReleaseKernel(%s);", KernelName);
        main.pushstr_newL(strBuffer);

    // Copy back ALL Premittive Variables
    for(Identifier* iden: vars){
        //cout<<"CheckPoint 2"<<endl;
        //TableEntry* te = iden->getSymbolInfo();
        cout<<"Variable from ForAll:"<<iden->getIdentifier()<<endl;
        if(iden->getSymbolInfo()==NULL) continue; // for (v in g.nodes()) for 'v' I didn't find an enty in table
        Type* type = iden->getSymbolInfo()->getType();

        if(type->isPrimitiveType())
          generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), false);
        // We will not transfer proptypes as we have already done it
        // For proptype data need to Create clSetKernelArg
        
    }
    //copy back all the parameters back to the cpu

        if(forAll->getParent()->getParent()!=NULL)
    {
      cout<<"Parent is not null, parent node = ";
      statement* parent = (statement*) forAll->getParent()->getParent();
      cout<<parent->getTypeofNode()<<endl;
      if(parent->getTypeofNode()== NODE_FIXEDPTSTMT)
      {
         cout<<"Parent is fixed point"<<endl;
         fixedPointStmt* fixedPointConstruct = (fixedPointStmt*)parent;
        Expression* convergeExpr = fixedPointConstruct->getDependentProp();
        Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
        if(fixedPointId->getSymbolInfo()->getType()->isPropNodeType()){
          sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),fixedPointId->getIdentifier());
          main.pushstr_newL(strBuffer);
        }
        else if(fixedPointId->getSymbolInfo()->getType()->isPropEdgeType()){
          sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),fixedPointId->getIdentifier());
          main.pushstr_newL(strBuffer);
        }
        else {
          sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),fixedPointId->getIdentifier());
          main.pushstr_newL(strBuffer);
        }
        
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
          convergeExpr->getExpressionFamily() == EXPR_ID);


        Identifier* dependentId = NULL;
        //~ bool isNot = false;
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
              convergeExpr->getExpressionFamily() == EXPR_ID);
        if (convergeExpr->getExpressionFamily() == EXPR_UNARY) {
          if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
            dependentId = convergeExpr->getUnaryExpr()->getId();
            //~ isNot = true;
            if(dependentId->getSymbolInfo()->getType()->isPropNodeType()){
            sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer);
          }
          else if(dependentId->getSymbolInfo()->getType()->isPropEdgeType()){
            sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer);
          }
          else{
            sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer); 
          }
          }
        }
      }
    }

  for(Identifier* iden: vars)
  {
    if(iden->getSymbolInfo()==NULL) continue; 
    Type* type = iden->getSymbolInfo()->getType();
    if(type->isPrimitiveType())
    {
      sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropNodeType()){
      sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropEdgeType()){
      sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);
    }
    
  }
    // generate functionName_kernel in kernel.cl

    addCudaKernel(forAll);
    cout<<"Exiting ForAll"<<endl;
  }
  else{ //IS FOR
    generateForAllSignature(forAll, 2);  // FOR LINE


    if (forAll->hasFilterExpr()) 
    {
      blockStatement* changedBody = includeIfToBlock(forAll);
      cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
      forAll->setBody(changedBody);
      // cout<<"FORALL BODY
      // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
    }


    if(extractElemFunc!=NULL)
    {
      if(neighbourIteration(iteratorMethodId->getIdentifier()))
      {
        // ForAll with neighbour iteration
        cout << "\t ITERATE Neighbour \n";
        char* wItr = forAll->getIterator()->getIdentifier(); // iterator of for loop
        cout<<"Iterator:: "<<wItr<<endl;
        char* nbrVar;
        if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)  // parent is IterateinBFS
        {
          cout<<"parent is Node_ItrBfs"<<endl;
          list<argument*> argList = extractElemFunc->getArgList();
          assert(argList.size() == 1);
          Identifier* nodeNbr = argList.front()->getExpr()->getId();
          nbrVar = nodeNbr->getIdentifier();

          sprintf(strBuffer, "if(d_level[%s] == -1)", wItr);
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("{");
          sprintf(strBuffer, "d_level[%s] = (*d_hops_from_source + 1);", wItr);

          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("*d_finished = 1;");
          targetFile.pushstr_newL("}");

          sprintf(strBuffer, "if(d_level[%s] == (*d_hops_from_source + 1))", wItr);

          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("{");
          //Generate all the statement present in For
          generateBlock((blockStatement*)forAll->getBody(), false, 2); // In kernelFile

          targetFile.pushstr_newL("}");

          targetFile.pushstr_newL("}");

        }
        else if(forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
        {
          cout<<"parent is Reverse Itr Bfs"<<endl;
          char strBuffer[1024];
        list<argument*> argList = extractElemFunc->getArgList();
        assert(argList.size() == 1);
        Identifier* nodeNbr = argList.front()->getExpr()->getId();
        nbrVar = nodeNbr->getIdentifier();
        std::cout << "V?:" << nbrVar << '\n';
        

        sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source)", wItr);
        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("{");
        //Generate statements of forAll Block
        generateBlock((blockStatement*)forAll->getBody(), false, 2);
        targetFile.pushstr_newL("} // end IF  ");
        targetFile.pushstr_newL("} // end FOR");
        targetFile.pushstr_newL("barrier(CLK_GLOBAL_MEM_FENCE);");
        }
        else
        {
          cout<<"Normal For"<<endl;
          generateStatement(forAll->getBody(),isMainFile);
          targetFile.pushstr_newL("} //  end FOR NBR ITR. TMP FIX!");
          std::cout<< "FOR BODY END" << '\n';

        }


      }
      else
      {
         printf("FOR NORML");
        generateStatement(forAll->getBody(), 2);
      }
    }
    else
    {
      if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN)  //FOR SET
      { 
          if (body->getTypeofNode() == NODE_BLOCKSTMT) 
          {
            targetFile.pushstr_newL("{"); 
            //targetFile.pushstr_newL("//HERE");
            printf("FOR");
            sprintf(strBuffer, "int %s = *itr;", forAll->getIterator()->getIdentifier());
            targetFile.pushstr_newL(strBuffer);
            generateBlock((blockStatement*)body, false);  //FOR BODY for
            targetFile.pushstr_newL("}");

          }
          else
          {
            generateStatement(forAll->getBody(), 2);
          }

      }
    }

  }
  
  return ;
}


void dsl_cpp_generator::generateReductionCallStmt(reductionCallStmt* stmt, int isMainFile)
{
  //cout<<"inside generate reduction call statement"<<endl;
  dslCodePad& targetFile = getTargetFile(isMainFile);
  reductionCall* reduceCall = stmt->getReducCall();
  char strBuffer[1024];
  targetFile.NewLine();
  if(reduceCall->getReductionType()== REDUCE_MIN)
  {
    cout<<"Reduction Type:: REDUCE_MIN"<<endl;
    if(stmt->isListInvolved())
    {
      list<argument*> argList = reduceCall->getargList();
      list<ASTNode*> leftList = stmt->getLeftList();
      //~ int i = 0;
      list<ASTNode*> rightList = stmt->getRightList();
      printf("LEFT LIST SIZE %lu \n", leftList.size());
      if (stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType()) {
        Type* type = stmt->getAssignedId()->getSymbolInfo()->getType();

        targetFile.pushstr_space(convertToCppType(type->getInnerTargetType()));
      }
      sprintf(strBuffer, "%s_new", stmt->getAssignedId()->getIdentifier());
      std::cout<< "ReducN VAR:" << stmt->getAssignedId()->getIdentifier() << '\n';
      targetFile.pushString(strBuffer);
       list<argument*>::iterator argItr;
      argItr = argList.begin(); // pointing to first element of right side list
      argItr++;
      targetFile.pushString(" = ");
      generateExpr((*argItr)->getExpr(), isMainFile);
      Expression* expr = (*argItr)->getExpr();
      //cout<< expr->isArithmetic()<<endl; // for SSSP right side expr is Arithmetic
      targetFile.push(';');
      targetFile.NewLine();
      list<ASTNode*>:: iterator itr1 = leftList.begin();

      list<ASTNode*>:: iterator itr2 = rightList.begin();
      itr1++;
      for( ; itr1 != leftList.end(); itr1++)
      {
        ASTNode* node = *itr1;
        ASTNode* node1 = *itr2;

        if(node->getTypeofNode()== NODE_ID)
        {
          Identifier* id = (Identifier*)node;
          targetFile.pushString(convertToCppType(id->getSymbolInfo()->getType()));
          sprintf(strBuffer, "%s_new = ",id->getIdentifier());
          targetFile.pushString(strBuffer);
          generateExpr((Expression*)node1, isMainFile);
        }
        if (node->getTypeofNode() == NODE_PROPACCESS) {
          PropAccess* p = (PropAccess*)node;
          Type* type = p->getIdentifier2()->getSymbolInfo()->getType();
          if (type->isPropType()) {
            targetFile.pushstr_space(
                convertToCppType(type->getInnerTargetType()));
          }

          sprintf(strBuffer, "%s_new", p->getIdentifier2()->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.pushString(" = ");
          //~ Expression* expr = (Expression*)node1;
          generateExpr((Expression*)node1, isMainFile);
          targetFile.pushstr_newL(";");
        }
        itr2++;
      }
      targetFile.pushString("if(");

      sprintf(strBuffer, "d_%s[v]!= INT_MAX && ",stmt->getAssignedId()->getIdentifier());
      targetFile.pushString(strBuffer);
      generate_exprPropId(stmt->getTargetPropId(), isMainFile);
      sprintf(strBuffer, " > %s_new)", stmt->getAssignedId()->getIdentifier());
      targetFile.pushstr_newL(strBuffer);
      targetFile.pushstr_newL("{");
      //targetFile.NewLine();
      if (stmt->isTargetId()) 
      {
        Identifier* targetId = stmt->getTargetId();
        targetFile.pushstr_space(
        convertToCppType(targetId->getSymbolInfo()->getType()));
        targetFile.pushstr_space("oldValue");
        targetFile.pushstr_space("=");
        generate_exprIdentifier(stmt->getTargetId(), isMainFile);
        targetFile.pushstr_newL(";");
      }
      else
      {
        PropAccess* targetProp = stmt->getTargetPropId();
        Type* type = targetProp->getIdentifier2()->getSymbolInfo()->getType();
        if (type->isPropType()) 
        {
          // targetFile.pushstr_space(convertToCppType(type->getInnerTargetType()));
          // targetFile.pushstr_space("oldValue");
          // targetFile.pushstr_space("=");
          // generate_exprPropId(stmt->getTargetPropId(), isMainFile);
          // targetFile.pushstr_newL(";");
        }
      }
      targetFile.pushString("atomic_min(&");
      generate_exprPropId(stmt->getTargetPropId(), isMainFile);
      sprintf(strBuffer, ",%s_new);", stmt->getAssignedId()->getIdentifier());
      targetFile.pushstr_newL(strBuffer);
      itr1 = leftList.begin();
      itr1++;
      for (; itr1 != leftList.end(); itr1++) 
      {
        ASTNode* node = *itr1;
        Identifier* affected_Id = NULL;
        if (node->getTypeofNode() == NODE_ID) {
          generate_exprIdentifier((Identifier*)node, isMainFile);
        }
        if (node->getTypeofNode() == NODE_PROPACCESS) {
          generate_exprPropId((PropAccess*)node, isMainFile);
        }
        targetFile.space();
        targetFile.pushstr_space("=");
        if (node->getTypeofNode() == NODE_ID) {
          generate_exprIdentifier((Identifier*)node, isMainFile);
          affected_Id = (Identifier*)node;
        }
        if (node->getTypeofNode() == NODE_PROPACCESS)  // here
        {
          generate_exprIdentifier(((PropAccess*)node)->getIdentifier2(),
                                  isMainFile);
          affected_Id = ((PropAccess*)node)->getIdentifier2();
        }
        targetFile.pushString("_new");
        targetFile.pushstr_newL(";");

        if (affected_Id->getSymbolInfo()->getId()->get_fp_association()) {
          char* fpId = affected_Id->getSymbolInfo()->getId()->get_fpId();
          sprintf(strBuffer, "*%s = %s ;", fpId, "false");
          std::cout<< "FPID ========> " << fpId << '\n';
          targetFile.pushstr_newL(strBuffer);
          //~ targetFile.pushstr_newL("}");  // needs to be removed
          //~ targetFile.pushstr_newL("}");  // needs to be removed
        }

      }

      targetFile.push('}');
      targetFile.NewLine();

    }
  }

}

void dsl_cpp_generator::generateReductionOpStmt(reductionCallStmt* stmt, int isMainFile)
{
  //stmt->getLeftId();
  //cout<<"anup kumar singh your identifier is "<<stmt->getLeftId()->getIdentifier()<<endl;
  //printf("%s\n",stmt->getLeftId()->getIdentifier());
  dslCodePad& targetFile = getTargetFile(isMainFile);

  char strBuffer[1024];

  if (stmt->isLeftIdentifier()) 
  {
    // Atomics required
      Identifier* id=stmt->getLeftId(); 

      printf("\n\n\naks%s\n",stmt->getLeftId()->getIdentifier()); //ye variable ka name de reha h
      
      Type *ty = id->getSymbolInfo()->getType();
      cout<<"\n ty->gettypeId()   "<<ty->gettypeId()<<endl;
      cout<<"\n ty->getRootType()   "<<ty->getRootType()<<endl;

      const char *typVar = convertToCppType(ty);
      cout<<"typVar   "<<typVar<<endl;

      cout<<getAtomicFromType(ty, 0)<<"( "<<id->getIdentifier()<<", ("<<typVar<<")"<<endl;

       sprintf(strBuffer, "%s( d_%s, (%s)",getAtomicFromType(ty, 0), id->getIdentifier(), typVar);
       // 0 for atomic Add
       // to implement other tpe of atomics modify getAtomicFromType function and libOpenCL.h
       // we need to implement for min, max, sub etc..

       targetFile.pushString(strBuffer);
      const char* operatorString = getOperatorString(stmt->reduction_op());
      //targetFile.pushString(",");
      if(*operatorString=='-')targetFile.pushString("-");
       generateExpr(stmt->getRightSide(), isMainFile);
       targetFile.pushstr_newL(");");
       targetFile.NewLine();

  } else {

    //sprintf(strBuffer, "%s( %s, (%s)",getAtomicFromType(ty, 0), id->getIdentifier(), typVar);


    
    Identifier* id1=stmt->getPropAccess()->getIdentifier1();
    Identifier* id2=stmt->getPropAccess()->getIdentifier2();

    if(id2->getSymbolInfo()->getType()->isPropNodeType()){
      Type *ty = id2->getSymbolInfo()->getType();
      const char *typVar = convertToCppType(ty);

      sprintf(strBuffer,     "%s(&"    ,getAtomicFromType(ty, 0));
      targetFile.pushString(strBuffer);
      generate_exprPropId(stmt->getPropAccess(), isMainFile);

      const char* operatorString = getOperatorString(stmt->reduction_op());
      targetFile.pushString(",");
      if(*operatorString=='-')targetFile.pushString("-");
      generateExpr(stmt->getRightSide(), isMainFile);
      targetFile.pushString(")");
      targetFile.pushstr_newL(";");
      targetFile.NewLine();
    }
    else {
      targetFile.NewLine();
          // sprintf(strBuffer,"int %s_src=%s.source;",id1->getIdentifier(),id1->getIdentifier());
          // targetFile.pushString(strBuffer);
          // targetFile.NewLine();
          // sprintf(strBuffer,"int %s_des=%s.destination;",id1->getIdentifier(),id1->getIdentifier());
          // targetFile.pushString(strBuffer);
          // targetFile.NewLine();
          // sprintf(strBuffer,"int %s_weight=%s.weight;",id1->getIdentifier(),id1->getIdentifier());
          // targetFile.pushString(strBuffer);
          // targetFile.NewLine();

          //sprintf(strBuffer, "for(int i=d_meta[%s_src]; i< d_meta[%s_src+1]; i++) {",id1->getIdentifier(),id1->getIdentifier());
          //targetFile.pushString(strBuffer);
          //targetFile.NewLine();
          //sprintf(strBuffer, "int v = d_data[i]");
          //targetFile.pushString(strBuffer);
          //targetFile.NewLine();
          //sprintf(strBuffer, "if(v==%s_des){",id1->getIdentifier());
          //targetFile.pushString(strBuffer);
          //targetFile.NewLine();
          /************************************************************************************/
          Type *ty = id2->getSymbolInfo()->getType();
          const char *typVar = convertToCppType(ty);
          sprintf(strBuffer,     "%s(&"    ,getAtomicFromType(ty, 0));
          targetFile.pushString(strBuffer);
          sprintf(strBuffer,"d_%s[%s_ind]",id2->getIdentifier(),id1->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.pushString(",");
          const char* operatorString = getOperatorString(stmt->reduction_op());
          if(*operatorString=='-')targetFile.pushString("-");
          generateExpr(stmt->getRightSide(), isMainFile);
          targetFile.pushString(")");
          targetFile.pushstr_newL(";");
          /*************************************************************************************/
          //targetFile.NewLine();
          //targetFile.pushString("}");
          //targetFile.NewLine();
          //targetFile.pushString("}");
    }
    




    // cout<<"typVar   "<<typVar<<endl;


    //generate_exprPropId(stmt->getPropAccess(), isMainFile);
    // if(id1!=NULL || id2!=NULL)cout<<"something is there for sure\n";    
    // cout<<id1->getIdentifier()<<"                "<<id2->getIdentifier()<<" hello i am \n";
    // targetFile.pushString(" = ");
    // generate_exprPropId(stmt->getPropAccess(), isMainFile);
    
    // const char* operatorString = getOperatorString(stmt->reduction_op());
    // cout<<"operatorString   ->>"<<operatorString<<endl;

    // targetFile.pushstr_space(operatorString);
    // generateExpr(stmt->getRightSide(), isMainFile);
    // targetFile.pushstr_newL(";");
    // targetFile.NewLine();
  } 
  
  cout<<"exiting Reduction operation Statement "<<endl;
}


void dsl_cpp_generator::generateReductionStmt(reductionCallStmt* stmt,
                                              int isMainFile) {
  //~ dslCodePad& targetFile = getTargetFile(isMain);

  //~ char strBuffer[1024];
  //cout<<"Inside Generate Reduction Statement"<<endl;
  if (stmt->is_reducCall()) {
    
    generateReductionCallStmt(stmt, isMainFile);
  } else {
    generateReductionOpStmt(stmt, isMainFile);
  }
}

// Generates kernel in kernel.cl file
void dsl_cpp_generator::generateInitkernelStr(const char *varType,const char* varName, const char* value, const char* sizeVE)
{
  char strBuffer[1024];
  sprintf(strBuffer,"__kernel void init%s_kernel(__global %s *%s, unsigned int %s)",varName, varType,varName, sizeVE);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("{");
  kernel.pushstr_newL("unsigned int id = get_global_id(0);");
  sprintf(strBuffer, "if(id>=%s) return ;",sizeVE);
  kernel.pushstr_newL(strBuffer);
  sprintf(strBuffer, "%s[id] = %s;", varName, value);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("}");
  char *kernelName = (char*)malloc(100*sizeof(char));
  strcpy(kernelName,"init");
  strcat(kernelName,varName);
  addKernelObject(kernelName);
  return;
  
}
void dsl_cpp_generator::generatePropParams(list<formalParam*> paramList,char *kernelName, bool isNeedType=true, int isMainFile = 1 ,int argnum = -1)
{
  cout<<"Inside generateProParams"<<endl;
  list<formalParam*>::iterator itr;
  dslCodePad& targetFile = getTargetFile(isMainFile);
  //~ Assumes that there is at least one param already. so prefix with  "," is okay
  char strBuffer[1024];
  for(itr=paramList.begin();itr!=paramList.end();itr++) {

    Type* type=(*itr)->getType();
    if (type->isPropType()) {
      if (type->getInnerTargetType()->isPrimitiveType()) {

        const char * temp = "d_";
        char* temp1 = (*itr)->getIdentifier()->getIdentifier();
        char* temp2 = (char*)malloc(1+strlen(temp) + strlen(temp1));
        strcpy(temp2,temp);
        strcat(temp2,temp1);
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++%s\n", temp2);
        if(isNeedType) //if isNeedType = true we are generating for kernel else for main
          sprintf(strBuffer,",__global %s* %s", convertToCppType(type->getInnerTargetType()), temp2);
        else // in main we need argument number for clSetKernelArg
         {
            sprintf(strBuffer, "status = clSetKernelArg(%s, %d, sizeof(cl_mem), (void*)&%s);", kernelName, argnum, temp2);
            // targetFile.pushstr_newL(strBuffer);
         }
         
         targetFile.pushString(strBuffer);
      }
    }
  }

  cout<<"leaving generateProParams "<<endl;
}


void dsl_cpp_generator::addCudaBFSIterKernel(iterateBFS* bfsAbstraction)
{
  char strBuffer[1024];
  // Target File will be header file
  const char* loopVar = bfsAbstraction->getIteratorNode()->getIdentifier();
  printf("loop Variable is -->%s\n", loopVar);

  //~ const char* nbbrVar = "w";

  statement* body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement* block = (blockStatement*)body;
  list<statement*> statementList = block->returnStatements();

  kernel.pushstr_newL("__kernel void fwd_pass_kernel(int V, __global int* d_meta, __global int* d_data, __global int* d_weight,__global int* d_rev_meta,__global int* d_src, __global int* d_level, __global int* d_hops_from_source, __global int* d_finished");
  // generate rest of prop Parameters
  //Kernel name not required
  Function* func =getCurrentFunc();
  blockStatement* blockStmt = func->getBlockStatement();
  list<statement*> listStmt = blockStmt->returnStatements();
  list<Identifier*> declaredProp;
  //printf("Print declare prop types\n");
  for(auto stmt:listStmt)
  {
    if(stmt->getType().compare("declaration")==0)
    {
      declaration* decl = (declaration*)stmt;
      Identifier* ident = decl->getdeclId();
      if(ident->getSymbolInfo()->getType()->isPropType() )
      {
        //printf("%s\n", ident->getIdentifier());
        //declaredProp.push_back(ident);
        sprintf(strBuffer,",__global %s* d_%s ",convertToCppType(ident->getSymbolInfo()->getType()->getInnerTargetType()),
    ident->getIdentifier());
    kernel.pushString(strBuffer);
      }
      if(ident->getSymbolInfo()->getType()->isPrimitiveType() )
      {
        //printf("%s\n", ident->getIdentifier());
        //declaredProp.push_back(ident);
        printf(",__global %s* d_%s  aks\n",convertToCppType(ident->getSymbolInfo()->getType()),ident->getIdentifier());
        sprintf(strBuffer,",__global %s* d_%s ",convertToCppType(ident->getSymbolInfo()->getType()),ident->getIdentifier());
        kernel.pushString(strBuffer);
        

      }

    }
  }
  // for(auto iden:declaredProp)
  // {
    
  //   sprintf(strBuffer,",__global %s* d_%s ",convertToCppType(iden->getSymbolInfo()->getType()->getInnerTargetType()),
  //   iden->getIdentifier());
  //   kernel.pushString(strBuffer);
  // }
  
  char kernelName[2] = ".";
  generatePropParams(getCurrentFunc()->getParamList(), kernelName, true, 2, 9); // true: typeneed 2:kernel file(0:header file, 1: main file)
  kernel.push(')');
  kernel.NewLine();
  kernel.pushstr_newL("{");
  sprintf(strBuffer, "unsigned int %s = get_global_id(0);", loopVar);
  kernel.pushstr_newL(strBuffer);
  sprintf(strBuffer, "if(%s>= V) return ;", loopVar);
  kernel.pushstr_newL(strBuffer);
  // sprintf(strBuffer,"if(d_level[%s]== (*d_hops_from_source))", loopVar);
  // kernel.pushstr_newL(strBuffer);
  //kernel.pushstr_newL("{");



  for (statement* stmt : statementList)
  {
    generateStatement(stmt, 2);  //2:kernel, All these stmts should be inside kernel
  
  }

  //kernel.pushstr_newL("}// end of if block"); // end of if block

  if(isNeighbourIter){
    sprintf(strBuffer,"if(d_level[%s]== (*d_hops_from_source))", loopVar);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("{");
  
  sprintf(strBuffer,"for (int edge = d_meta[%s]; edge < d_meta[%s+1]; edge++){",loopVar,loopVar);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("int w = d_data[edge] ;");
  kernel.pushstr_newL("if(d_level[w]==-1){");
  kernel.pushstr_newL("*d_finished = 1;");
  kernel.pushstr_newL("d_level[w]=(*d_hops_from_source)+1;");
  kernel.pushstr_newL("}// end of if block");
  kernel.pushstr_newL("}// end of for block");
  kernel.pushstr_newL("}// end of if block"); // end of if block
  kernel.pushstr_newL("} // end of kernel"); // end of kernel
  }
  else {
    sprintf(strBuffer,"if(d_level[%s]== (*d_hops_from_source))", loopVar);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("{");
  
  sprintf(strBuffer,"for (int edge = d_rev_meta[%s]; edge < d_rev_meta[%s+1]; edge++){",loopVar,loopVar);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("int w = d_src[edge] ;");
  kernel.pushstr_newL("if(d_level[w]==-1){");
  kernel.pushstr_newL("*d_finished = 1;");
  kernel.pushstr_newL("d_level[w]=(*d_hops_from_source)+1;");
  kernel.pushstr_newL("}// end of if block");
  kernel.pushstr_newL("}// end of for block");
  kernel.pushstr_newL("}// end of if block"); // end of if block
  kernel.pushstr_newL("} // end of kernel"); // end of kernel
  }
  
  return;
}

void dsl_cpp_generator::addCudaRevBFSIterKernel(iterateBFS* bfsAbstraction)
{

  char strBuffer[1024];
  
  if(bfsAbstraction==NULL || (blockStatement*)bfsAbstraction->getRBFS()==NULL){
    
    return ;
  }
  
  printf("Print declare prop types\n");
  blockStatement* revBlock = (blockStatement*)bfsAbstraction->getRBFS()->getBody();
  
  if(revBlock ==NULL) {
    //cout<<"gadbad h \n";
    return ;
  }
  
  list<statement*> revStmtList = revBlock->returnStatements();
  const char* loopVar = bfsAbstraction->getIteratorNode()->getIdentifier();
  //backward Pass kernel
  kernel.pushString("__kernel void back_pass_kernel(int V, __global int* d_meta, __global int* d_data, __global int* d_weight, __global int* d_level, __global int* d_hops_from_source, __global int* d_finished");
  
  //add other prop params
  Function* func =getCurrentFunc();
  blockStatement* blockStmt = func->getBlockStatement();
  list<statement*> listStmt = blockStmt->returnStatements();
  list<Identifier*> declaredProp;
  
  
  for(auto stmt:listStmt)
  {
    if(stmt->getType().compare("declaration")==0)
    {
      declaration* decl = (declaration*)stmt;
      Identifier* ident = decl->getdeclId();
      if(ident->getSymbolInfo()->getType()->isPropType() )
      {
        //printf("%s\n", ident->getIdentifier());
        declaredProp.push_back(ident);
      }

    }
  }
  for(auto iden:declaredProp)
  {
    
    sprintf(strBuffer,",__global %s* d_%s ",convertToCppType(iden->getSymbolInfo()->getType()->getInnerTargetType()),
    iden->getIdentifier());
    kernel.pushString(strBuffer);
  }
  char kernelName[2] = ".";
  generatePropParams(getCurrentFunc()->getParamList(), kernelName, true, 2, 9); // true: typeneed 2:kernel file(0:header file, 1: main file)
  kernel.push(')');
  kernel.NewLine();
  kernel.pushstr_newL("{");
  sprintf(strBuffer, "unsigned int %s = get_global_id(0);", loopVar);
  kernel.pushstr_newL(strBuffer);
  sprintf(strBuffer, "if(%s>= V) return ;", loopVar);
  kernel.pushstr_newL(strBuffer);
  sprintf(strBuffer,"if(d_level[%s]==(*d_hops_from_source)-1)", loopVar);
  kernel.pushstr_newL(strBuffer);
  kernel.pushstr_newL("{");

  cout<<"Generating statement from revIterateBFS"<<endl;
  for(statement* stmt:revStmtList)
  {
    generateStatement(stmt,2);// 2:kernel file
  }
  kernel.pushstr_newL("}//end of if");
  kernel.pushstr_newL("}//end of kernel");
  return;
}



void dsl_cpp_generator::addCudaBFSIterationLoop(iterateBFS* bfsAbstraction)
{
  char strBuffer[1024];
  main.pushstr_newL("finished = 0;");
  main.NewLine();

  main.pushstr_newL("status  = clEnqueueWriteBuffer(command_queue,d_finished, CL_TRUE, 0,sizeof(int), &finished,0,0,NULL);");
  main.pushstr_newL("status  = clEnqueueWriteBuffer(command_queue,d_hops_from_source, CL_TRUE, 0,sizeof(int), &hops_from_source,0,0,NULL);");

  main.pushstr_newL("//Forward Pass");
  // Create forward pass kernel from program
  //forwardPass(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,..Other variable..);
  main.pushstr_newL("cl_kernel fwd_pass_kernel = clCreateKernel(program, \"fwd_pass_kernel\", &status);");
  main.pushstr_newL("if(status != CL_SUCCESS)");
  main.pushstr_newL("{");
  main.pushstr_newL("cout<<\"Failed to create fwd_pass_kernel\"<<endl;");
  main.pushstr_newL("}");
  //Set kernel arguments
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 0, sizeof(int), (void *)&V);");
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 1, sizeof(cl_mem), (void *)&d_meta);");
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 2, sizeof(cl_mem), (void *)&d_data);");
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 3, sizeof(cl_mem), (void *)&d_weight);");
  // delta and sigma are hard coded. what if user uses a different name?
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 4, sizeof(cl_mem), (void *)&d_rev_meta);");
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 5, sizeof(cl_mem), (void *)&d_src);");

  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 6, sizeof(cl_mem), (void *)&d_level);");
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 7, sizeof(cl_mem), (void *)&d_hops_from_source);");
  main.pushstr_newL("status = clSetKernelArg(fwd_pass_kernel, 8, sizeof(cl_mem), (void *)&d_finished);");
  
 
 //other prop parameters
 Function* func =getCurrentFunc();
  blockStatement* blockStmt = func->getBlockStatement();
  list<statement*> listStmt = blockStmt->returnStatements();
  list<Identifier*> declaredProp;
  //printf("Print declare prop types\n");
  for(auto stmt:listStmt)
  {
    if(stmt->getType().compare("declaration")==0)
    {
      declaration* decl = (declaration*)stmt;
      Identifier* ident = decl->getdeclId();
      if(ident->getSymbolInfo()->getType()->isPropType() || ident->getSymbolInfo()->getType()->isPrimitiveType() )
      {
        printf("(((((((((((((((((pushing)))))))))))))))))))))))))))))))))%s\n", ident->getIdentifier());
        declaredProp.push_back(ident);
      }
      if(ident->getSymbolInfo()->getType()->isPrimitiveType()){
        PremVarOfKernel.push_back(ident);
      }
      

    }
  }
  int argNumber = 9;
  for(auto iden:declaredProp)
  {
    sprintf(strBuffer,"status = clSetKernelArg(fwd_pass_kernel, %d, sizeof(cl_mem), (void*)&d_%s);",argNumber, iden->getIdentifier());
    main.pushstr_newL(strBuffer);
    argNumber++;
  }
  //Pass parameters of the function
  char *kernelName = (char *)malloc(20* sizeof(char));
  strcpy(kernelName, "fwd_pass_kernel");
  generatePropParams(getCurrentFunc()->getParamList(), kernelName, false, 1, argNumber); // true: typeneed 1:inMainfile
  // Now call the fwd_pass kernel from main
  main.NewLine();
 
  main.pushstr_newL("local_size = 128;");
  main.pushstr_newL("global_size = (V + local_size -1)/ local_size * local_size;");
  sprintf(strBuffer,"status = clEnqueueNDRangeKernel(command_queue, %s, 1, NULL, &global_size, &local_size, 0, NULL, &event);", kernelName);
  main.pushstr_newL(strBuffer);
  addProfilling("event");

  sprintf(strBuffer, "status = clReleaseKernel(%s);", kernelName);
  main.pushstr_newL(strBuffer);

  main.pushstr_newL("status=clEnqueueReadBuffer(command_queue, d_finished , CL_TRUE, 0, sizeof(int), &finished, 0, NULL, NULL );");
  //main.pushstr_newL("status=clEnqueueReadBuffer(command_queue, d_hops_from_source , CL_TRUE, 0, sizeof(int), &hops_from_source, 0, NULL, NULL );");
  main.NewLine();

  main.pushstr_newL("hops_from_source = hops_from_source + 1;");
  addCudaBFSIterKernel(bfsAbstraction);  // KERNEL BODY!!!
}


void dsl_cpp_generator::addCudaRevBFSIterationLoop(iterateBFS* bfsAbstraction)
{
  char strBuffer[1024];

  main.pushstr_newL("cl_kernel back_pass_kernel = clCreateKernel(program,\"back_pass_kernel\", &status);");
  //set kernel arguments
  //back_pass(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,...")
  main.pushstr_newL("if(status != CL_SUCCESS)");
  main.pushstr_newL("{");
  main.pushstr_newL("cout<<\"Failed to create back_pass_kernel\"<<endl;");
  main.pushstr_newL("}");
  //Set kernel arguments
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 0, sizeof(int), (void *)&V);");
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 1, sizeof(cl_mem), (void *)&d_meta);");
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 2, sizeof(cl_mem), (void *)&d_data);");
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 3, sizeof(cl_mem), (void *)&d_weight);");
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 4, sizeof(cl_mem), (void *)&d_level);");
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 5, sizeof(cl_mem), (void *)&d_hops_from_source);");
  main.pushstr_newL("status = clSetKernelArg(back_pass_kernel, 6, sizeof(cl_mem), (void *)&d_finished);");
  
  // Pass all the decalared propNode types
  Function* func =getCurrentFunc();
  blockStatement* blockStmt = func->getBlockStatement();
  list<statement*> listStmt = blockStmt->returnStatements();
  list<Identifier*> declaredProp;
  for(auto stmt:listStmt)
  {
    if(stmt->getType().compare("declaration")==0)
    {
      declaration* decl = (declaration*)stmt;
      Identifier* ident = decl->getdeclId();
      if(ident->getSymbolInfo()->getType()->isPropType() )
      {
        //printf("%s\n", ident->getIdentifier());
        declaredProp.push_back(ident);
      }

    }
  }
  int argNumber = 7;
  for(auto iden:declaredProp)
  {
    sprintf(strBuffer,"status = clSetKernelArg(back_pass_kernel, %d, sizeof(cl_mem), (void*)&d_%s);",argNumber, iden->getIdentifier());
    main.pushstr_newL(strBuffer);
    argNumber++;
  }
  
  // from Argument if there is any prop type
  char *kernelName = (char *)malloc(20* sizeof(char));
  strcpy(kernelName, "back_pass_kernel");
  generatePropParams(getCurrentFunc()->getParamList(), kernelName, false, 1, argNumber); // true: typeneed 1:inMainfile
   // Now call the back_pass kernel from main
  main.NewLine();
  main.pushstr_newL("local_size = 128;");
  main.pushstr_newL("global_size = (V + local_size -1)/ local_size * local_size;");
  sprintf(strBuffer,"status = clEnqueueNDRangeKernel(command_queue, %s, 1, NULL, &global_size, &local_size, 0, NULL, &event);", kernelName);
  main.pushstr_newL(strBuffer);
  addProfilling("event");
  sprintf(strBuffer, "status = clReleaseKernel(%s);", kernelName);
  main.pushstr_newL(strBuffer);
  main.NewLine();
  main.pushstr_newL("hops_from_source--;");
  main.NewLine();
  
  // Add reverseBFS kernel
 

  addCudaRevBFSIterKernel(bfsAbstraction);

  return;
}

void dsl_cpp_generator::generateBFSAbstraction(iterateBFS* bfsAbstraction,int isMainFile)
{
  dslCodePad& targetFile = getTargetFile(isMainFile);
  char strBuffer[1024];
  sprintf(strBuffer,"//Iterate in BFS to be implemented.");
  targetFile.pushstr_newL(strBuffer); 
  statement* body = bfsAbstraction->getBody();
  assert(body->getTypeofNode()== NODE_BLOCKSTMT);
  blockStatement* block = (blockStatement*)body;
  list<statement*>listStatement = block->returnStatements();
  // Add variable for for BFS and revBFS

  main.pushstr_newL("int finished;");
  main.pushstr_newL("int hops_from_source;");
  main.pushstr_newL("cl_mem d_finished = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR,sizeof(int), NULL, &status);");
  main.pushstr_newL("cl_mem d_hops_from_source = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR, sizeof(int), NULL, &status);");
  main.pushstr_newL("cl_mem d_level = clCreateBuffer(context, CL_MEM_READ_WRITE, V*sizeof(int), NULL, &status);");
  //main.pushstr_newL("//map these varibles");
  //main.pushstr_newL("finished = (int *)clEnqueueMapBuffer(command_queue, d_finished , CL_TRUE, CL_MAP_READ|CL_MAP_WRITE, 0,sizeof(int),0,NULL,NULL,&status);");
  //main.pushstr_newL("hops_from_source = (int *)clEnqueueMapBuffer(command_queue, d_hops_from_source , CL_TRUE, CL_MAP_READ|CL_MAP_WRITE, 0,sizeof(int),0,NULL,NULL,&status);");

  sprintf(strBuffer,"//initialize d_level with -1 and for %s 0", bfsAbstraction->getRootNode()->getIdentifier());
  main.pushstr_newL(strBuffer);
  generateInitkernelStr("int", "d_level", "-1", "V");

  // Create kernel->setArgument->runKenrel
  sprintf(strBuffer,"cl_kernel initd_level_kernel = clCreateKernel(program, \"initd_level_kernel\", &status);");
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("if(status!=CL_SUCCESS)");
  main.pushstr_newL("{");
  main.pushstr_newL("cout<<\"Failed to create initd_level_kernel\"<<endl;");
  main.pushstr_newL("exit(0);");
  main.pushstr_newL("}");
  main.pushstr_newL("status = clSetKernelArg(initd_level_kernel , 0, sizeof(cl_mem) ,(void *)&d_level);");
  main.pushstr_newL("status = clSetKernelArg(initd_level_kernel , 1, sizeof(int) ,(void *)&V);");

  main.pushstr_newL("local_size = 128;");
  main.pushstr_newL("global_size = (V + local_size -1)/ local_size * local_size;");
  main.pushstr_newL("status  = clEnqueueNDRangeKernel(command_queue, initd_level_kernel,1,NULL, &global_size, &local_size, 0, NULL, &event);");
  addProfilling("event");
  main.pushstr_newL("status = clReleaseKernel(initd_level_kernel);");
  // initIndexd_level_kernel for src we need to set d_level=0;
  char *rootNode = bfsAbstraction->getRootNode()->getIdentifier();
  printf("Root Node: %s\n", rootNode);
  kernel.pushstr_newL("__kernel void initIndexd_level_kernel(__global int* d_level, int src)");
  kernel.pushstr_newL("{");
  kernel.pushstr_newL("d_level[src] = 0;");
  kernel.pushstr_newL("}");

  main.pushstr_newL("cl_kernel initIndexd_level_kernel = clCreateKernel(program,\"initIndexd_level_kernel\",&status);");
  
  main.pushstr_newL("status = clSetKernelArg(initIndexd_level_kernel, 0, sizeof(cl_mem), (void *)&d_level);");
  sprintf(strBuffer,"status = clSetKernelArg(initIndexd_level_kernel, 1, sizeof(int), (void*)&%s);",rootNode);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("global_size = 1;");
  main.pushstr_newL("local_size=1;");
  main.pushstr_newL("status = clEnqueueNDRangeKernel(command_queue, initIndexd_level_kernel, 1,NULL, &global_size, &local_size, 0, NULL, &event);");
  addProfilling("event");
  main.pushstr_newL("status = clReleaseKernel(initIndexd_level_kernel);");
  //  begin While loop of ITRBFS
  main.pushstr_newL("finished = 1;");
  main.pushstr_newL("hops_from_source = 0;");
  /**********************************************************************************************************************/
   //other prop parameters
 Function* func =getCurrentFunc();
  blockStatement* blockStmt = func->getBlockStatement();
  list<statement*> listStmt = blockStmt->returnStatements();
  list<Identifier*> declaredProp;
  //printf("Print declare prop types\n");
  for(auto stmt:listStmt)
  {
    if(stmt->getType().compare("declaration")==0)
    {
      declaration* decl = (declaration*)stmt;
      Identifier* iden = decl->getdeclId();
      Type* type = iden->getSymbolInfo()->getType();
      if(type->isPrimitiveType())
    {
      sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropNodeType()){
      sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropEdgeType()){
      sprintf(strBuffer,"status = clEnqueueWriteBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);
    }
      
      

    }
  }
  /**********************************************************************************************************************/

  main.pushstr_newL("while(finished)");
  main.pushstr_newL("{");

  // ADDS BODY OF ITERBFS + KERNEL LAUNCH
  addCudaBFSIterationLoop(bfsAbstraction);


  main.pushstr_newL("}");
  //printf("Print declare prop types\n");
  for(auto stmt:listStmt)
  {
    if(stmt->getType().compare("declaration")==0)
    {
      declaration* decl = (declaration*)stmt;
      Identifier* iden = decl->getdeclId();
      Type* type = iden->getSymbolInfo()->getType();
      if(type->isPrimitiveType())
    {
      sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int),   &%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropNodeType()){
      sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(V),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
    else if(type->isPropEdgeType()){
      sprintf(strBuffer,"status = clEnqueueReadBuffer(command_queue,   d_%s , CL_TRUE, 0, sizeof(int)*(E),   h_%s, 0, NULL, NULL );",iden->getIdentifier(),iden->getIdentifier());
      main.pushstr_newL(strBuffer);
    }
      
      

    }
  }
  /**********************************************************************************************************************/
  /**********************************************************************************************************************/
  main.NewLine();
  return; 
  /**************Iterate in Reverse BFS**********/
  /*Here we are assuming that if there is iterate in forward BFS
   then there will be iterate in reverse BFS.
   So we are directly generating that. Here need some improvement. What if user has not written
   Reverse part? what if user only need forward pass?
   */

  main.pushstr_newL("hops_from_source = hops_from_source-1;");
  main.NewLine();
  main.pushstr_newL("//Backward Pass");
  main.pushstr_newL("while(hops_from_source > 1)");
  main.pushstr_newL("{");
  main.NewLine();
  main.pushstr_newL("status  = clEnqueueWriteBuffer(command_queue,d_hops_from_source, CL_TRUE, 0,sizeof(int), &hops_from_source,0,0,NULL);");
  main.pushstr_newL("//KERNEL Launch");
  addCudaRevBFSIterationLoop(bfsAbstraction);
  //main.pushstr_newL("status=clEnqueueReadBuffer(command_queue, d_hops_from_source , CL_TRUE, 0, sizeof(int), &hops_from_source, 0, NULL, NULL );");
  main.pushstr_newL("}"); //end of while


  //unmap the mapped variables
  //main.pushstr_newL("status = clEnqueueUnmapMemObject(command_queue, d_finished, finished, 0, NULL, NULL);");
  //main.pushstr_newL("status = clEnqueueUnmapMemObject(command_queue, d_hops_from_source, hops_from_source, 0, NULL, NULL);");
  main.pushstr_newL("status = clReleaseMemObject(d_level);");
  main.pushstr_newL("status = clReleaseMemObject(d_finished);");
  main.pushstr_newL("status = clReleaseMemObject(d_hops_from_source);");
  return;
}

// w.sigma=w.sigma+v.sigma; from bc_dsl_v2
void dsl_cpp_generator::generateAtomicDeviceAssignmentStmt(assignment* asmt, int isMainFile)
{
  dslCodePad& targetFile = getTargetFile(isMainFile); 
  bool isAtomic = false;
  bool isResult = false;
  std::cout << "\tASST\n";


  if (asmt->lhs_isIdentifier()) {
    Identifier* id = asmt->getId();
    Expression* exprAssigned = asmt->getExpr();

    if(asmt->hasPropCopy()) // prop_copy is of the form (propId = propId)
       {
         char strBuffer[1024] ;
         Identifier* rhsPropId2 = exprAssigned->getId();
         Type* type = id->getSymbolInfo()->getType();
         const  char * copySize = (type->isPropNodeType())?"V":"E";
         sprintf(strBuffer, "status = clEnqueueCopyBuffer(command_queue, d_%s, d_%s, 0, 0, %s*sizeof(%s), 0, NULL, &event)",
                      rhsPropId2->getIdentifier(),id->getIdentifier(), copySize,convertToCppType(type->getInnerTargetType()));
         targetFile.pushString(strBuffer);
         targetFile.pushstr_newL(";");
       }
       else
    targetFile.pushString(id->getIdentifier());
  }


  else if(asmt->lhs_isProp())
  {
    PropAccess* propId = asmt->getPropId();
    Identifier *IdentifierVar1 = propId->getIdentifier1();
    Identifier *IdentifierVar2 = propId->getIdentifier2();

    if (asmt->isDeviceAssignment()) {
      std::cout << "\t  DEVICE ASST" << '\n';
    }
    if (asmt->getAtomicSignal()) {
      Type *typeVar = IdentifierVar2->getSymbolInfo()->getType();
      targetFile.pushString(getAtomicFromType(typeVar, 0)); // 0: Atomic add
      targetFile.pushString("(&");
      isAtomic = true;
      std::cout << "\t  ATOMIC ASST" << '\n';
    }

    if (asmt->isAccumulateKernel()) { // NOT needed
      isResult = true;
      std::cout << "\t  RESULT NO BC by 2 ASST" << '\n';
    }
    if(isMainFile==2){
      //cout<<"generating d_"<<IdentifierVar2->getIdentifier()<<"["<<IdentifierVar1->getIdentifier()<<
      targetFile.pushString("d_");  /// IMPORTANT
      targetFile.pushString(IdentifierVar2->getIdentifier());
      targetFile.push('[');
      targetFile.pushString(IdentifierVar1->getIdentifier());
      targetFile.push(']');
    }
    else if(isMainFile==1){
      targetFile.pushString("h_");  /// IMPORTANT
    targetFile.pushString(IdentifierVar2->getIdentifier());
    targetFile.push('[');
    targetFile.pushString(IdentifierVar1->getIdentifier());
    targetFile.push(']');
    }
  }
  
  if (isAtomic){
    //cout<<"3rd if part\n";
    targetFile.pushString(", ");
  }
  
  else if(!asmt->hasPropCopy()){
    //cout<<"4th if part\n";
    targetFile.pushString(" = ");
  }

  //~ std::cout<< "------>BEG EXP"  << '\n';
  
  
  if(!asmt->hasPropCopy()){
   // cout<<"5th if part\n";
    generateExpr(asmt->getExpr(), isMainFile, isAtomic);
  }
  //~ std::cout<< "------>END EXP"  << '\n';

  if (isAtomic){
    //cout<<"6th if part\n";
    targetFile.pushstr_newL(");");
  }
  else if (isResult){
    //cout<<"7th if part\n";
    targetFile.pushstr_newL(";"); // No need "/2.0;" for directed graphs
  }
  else if(!asmt->hasPropCopy()){
    //cout<<"8th if part\n";
    targetFile.pushstr_newL(";");
  }

  //cout<<"exiting generate Atomic Device Assignment"<<endl;
  return ;
}

void dsl_cpp_generator::generateDoWhileStmt(dowhileStmt* doWhile,int isMainFile)
{
  Expression* expr = doWhile->getCondition();
  statement* block = doWhile->getBody();
  dslCodePad& targetFile = getTargetFile(isMainFile);
  targetFile.pushstr_newL("do{");
  generateStatement(block, isMainFile);
  targetFile.pushString("}while(");
  generateExpr(expr, isMainFile);
  targetFile.pushString(");");
  targetFile.NewLine();
  return;
}






void dsl_cpp_generator::generateStatement(statement* stmt, int isMainFile) {

  if (stmt->getTypeofNode() == NODE_BLOCKSTMT) {
    generateBlock((blockStatement*)stmt, false, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_DECL) {
    generateVariableDecl((declaration*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_ASSIGN) 
  {
    // generateAssignmentStmt((assignment*)stmt);
    assignment* asst = (assignment*)stmt;
    if (asst->isDeviceAssignment())
    {
      generateDeviceAssignmentStmt(asst, isMainFile); 
    }  
    else 
    {
      // atomic or normal asmt
      generateAtomicDeviceAssignmentStmt(asst, isMainFile);
    } 

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
    //printf("isMainFile val %d\n",isMainFile);
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

}



void dsl_cpp_generator::generateBlock(blockStatement* blockStmt,bool includeBrace, int isMainFile)
{




  //cout << "i am inside generateBlock for the first time and the value of isMainFile=" << isMainFile<<endl;
  dslCodePad& targetFile = getTargetFile(isMainFile);

  usedVariables usedVars = getDeclaredPropertyVarsOfBlockAMD(blockStmt);
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
 
 //FREE UP the Variables declared inside the block
  char strBuffer[1024];

  if(vars.size()>0) {
    targetFile.NewLine();
    targetFile.pushstr_newL("//Free up!! all propVars in this BLOCK!");
  }
  for(Identifier* iden: vars) {
    sprintf(strBuffer,"status = clReleaseMemObject(d_%s);",iden->getIdentifier());
    targetFile.pushstr_newL(strBuffer);
    sprintf(strBuffer,"free(h_%s);", iden->getIdentifier());
    targetFile.pushstr_newL(strBuffer);
  }
  targetFile.NewLine();

  if (includeBrace) {
    targetFile.pushstr_newL("}");
  }
  //cout<<"Exiting the generate block"<<endl;
}
   
void dsl_cpp_generator::generateFunc(ASTNode* proc) {
  // dslCodePad &targetFile = isMainFile ? main : header;

  //~ char strBuffer[1024];
  Function* func = (Function*)proc;
  generateFuncHeader(func, 0); // header file
  generateFuncHeader(func, 1);   // main file

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
  main.pushstr_newL("printf(\"Total Kernel time = %0.2lf ms.\\n \", totalTime);");

  main.NewLine();
  generateCudaMemCpyParams(func->getParamList());

  
  // releasing all created objects in main file
  releaseObjects(1);
  
  main.pushstr_newL("} //end FUN");
  cout<<"Gerenration Completed :)"<<endl;
  return;
}


void dsl_cpp_generator::generateCudaMemCpyParams(list<formalParam*> paramList)
{
  cout<<"inside generate CudaMemCpyParams size= "<<paramList.size()<<endl;
  list<formalParam*>::iterator itr;
  for(itr=paramList.begin();itr!=paramList.end();itr++) {
  //cout<<(*itr)->getIdentifier()->getIdentifier()<<endl;
    Type* type=(*itr)->getType();
    if (type->isPropType()) {
      if (type->getInnerTargetType()->isPrimitiveType()) {
        //~ Type* innerType = type->getInnerTargetType();

        const char* sizeofProp = NULL;
        if(type->isPropEdgeType())
          sizeofProp = "E";
        else
          sizeofProp = "V";
        const char * temp = "d_";
        char* temp1 = (*itr)->getIdentifier()->getIdentifier();
        char* temp2 = (char*)malloc(1+strlen(temp) + strlen(temp1));
        strcpy(temp2,temp);
        strcat(temp2,temp1);
        char* hostvar = (char*)malloc((5+strlen(temp1))*sizeof(char));
        strcpy(hostvar,temp1);
        generateCudaMemcpyStr(hostvar, temp2,convertToCppType(type->getInnerTargetType()), sizeofProp, 0 );
       // generateCudaMalloc(type, (*itr)->getIdentifier()->getIdentifier());

        //~ Type* innerType=type->getInnerTargetType();
        //~ targetFile.pushString(convertToCppType(innerType)); //convertToCppType
        //need to be modified. ~ targetFile.pushString("*"); ~ targetFile.space();
        //~ targetFile.pushString(declStmt->getdeclId()->getIdentifier());
        //~
        //generatePropertyDefination(type,declStmt->getdeclId()->getIdentifier(),
        //isMainFile); ~ printf("added symbol
        //%s\n",declStmt->getdeclId()->getIdentifier()); ~ printf("value requ
        //%d\n",declStmt->getdeclId()->getSymbolInfo()->getId()->require_redecl());
        //~ /* decl with variable name as var_nxt is required for double buffering
        //~ ex :- In case of fixedpoint */
        //~ if(declStmt->getdeclId()->getSymbolInfo()->getId()->require_redecl())
        //~ {
        //~ targetFile.pushString(convertToCppType(innerType)); //convertToCppType
        //need to be modified. ~ targetFile.pushString("*"); ~ targetFile.space();
        //~ sprintf(strBuffer,"%s_nxt",declStmt->getdeclId()->getIdentifier());
        //~ targetFile.pushString(strBuffer);
        //~
        //generatePropertyDefination(type,declStmt->getdeclId()->getIdentifier(),
        //isMainFile);

        //~ }

        /*placeholder for adding code for declarations that are initialized as
         * well*/
      }
    }


  }
}


void dsl_cpp_generator::setCurrentFunc(Function* func)
{
  currentFunc = func;
}

Function* dsl_cpp_generator::getCurrentFunc()
{
  return currentFunc;
}
void dsl_cpp_generator:: releaseObjects(int isMainFile)
{
  dslCodePad& targefile = getTargetFile(isMainFile);
  char strBuffer[1024];
  vector<char*>::iterator itr;
  
  // Realeaseing memory objects
  targefile.pushstr_newL("//Release openCL objects");
  targefile.pushstr_newL("printf(\"Started releasing Objects\\n\");");
  
  for(itr = memObjects.begin(); itr!= memObjects.end(); itr++)
  {
    sprintf(strBuffer,"status = clReleaseMemObject(%s);", *(itr));
    //cout<<"Release d_"<<*itr<<endl;
    targefile.pushString(strBuffer);
    targefile.NewLine();
    free(*itr);
  }
  // Release kernel objects
  /*
  vector<char*>::iterator kitr;
    for(kitr = kernelObjects.begin(); kitr!= kernelObjects.end(); kitr++)
    {
      cout<<"kernelName :: "<<*kitr<<endl;
      sprintf(strBuffer, "status = clReleaseKernel(%s);", *kitr);
      targefile.pushString(strBuffer);
      targefile.NewLine();
      free(*kitr);
    }

  */
  // Release program
    targefile.pushString("status = clReleaseProgram(program);");
    targefile.NewLine();

  
  if(genCSR)
  {
    //Release command queue
    targefile.pushstr_newL("status = clFlush(command_queue);");
    targefile.pushstr_newL("status = clFinish(command_queue);");
    targefile.pushString("status = clReleaseCommandQueue(command_queue);");
    targefile.NewLine();
    // Release context 
    targefile.pushString("status = clReleaseContext(context);");
    targefile.NewLine();
    //targefile.pushstr_newL("free(devices);");
    //targefile.pushstr_newL("free(platforms);");
  }
  for(itr = hostMemObjects.begin(); itr != hostMemObjects.end();  itr++)
  {
    cout<<"Release host pointer::"<<*itr<<endl;
    sprintf(strBuffer, "free(%s);", *itr);
    targefile.pushstr_newL(strBuffer);
    free(*itr);
  }

}



// This function, fopen those files in which codes will be generated.
bool dsl_cpp_generator::openFileforOutput() {
  char temp[1024];
  printf("fileName %s\n", fileName);
  sprintf(temp, "%s/%s.h", "../graphcode/generated_amd", fileName);
  headerFile = fopen(temp, "w");
  if (headerFile == NULL) return false;
  header.setOutputFile(headerFile);

  sprintf(temp, "%s/%s.cl", "../graphcode/generated_amd" ,fileName);
  kernelFile = fopen(temp, "w");
  if (kernelFile == NULL) return false;
  kernel.setOutputFile(kernelFile);

  //kernel.push('C');
  //kernel.pushString("Hello");

  sprintf(temp, "%s/%s.cpp", "../graphcode/generated_amd", fileName);
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
void dsl_cpp_generator::addIncludeToFile(
    const char* includeName, dslCodePad& file,
    bool isCppLib) {  // cout<<"ENTERED TO THIS ADD INCLUDE FILE"<<"\n";
  if (!isCppLib)
    file.push('"');
  else
    file.push('<');

  file.pushString(includeName);
  if (!isCppLib)
    file.push('"');
  else
    file.push('>');
  file.NewLine();
}
void dsl_cpp_generator::generation_begin()
{
  char temp[1000];
  cout<<"Generation Begin"<<endl;
   header.pushString("#ifndef GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#define GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#include ");
  addIncludeToFile("stdio.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("stdlib.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("limits.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("CL/cl.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("../graph.hpp", header, false);
  //header.pushstr_newL("graph &g = NULL;");  //temporary fix - to fix the PageRank graph g instance


  header.NewLine();

  main.pushString("#include ");
  sprintf(temp, "%s.h", fileName);
  addIncludeToFile(temp, main, false);
  main.NewLine();


  //InKernel file 
  kernel.pushstr_newL("#include \"libOpenCL.h\"");
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
  
  generation_begin();

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

}
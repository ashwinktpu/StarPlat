#include <string.h>
#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"
#include "getUsedVars.cpp"


namespace spmultigpu{


const char* globalLoopVar; 
map<char*,int> declInForAll;
char MODIFIED[100];
int pullBased = 0;
 void dsl_cpp_generator::generateInitkernel(const char *name)
{
  char strBuffer[1024];
  header.pushstr_newL("template <typename T>");
  sprintf(strBuffer,
          "__gl+++obal__ void initKernel(unsigned %s, T* d_array, T initVal) {",
          name);
  header.pushstr_newL(strBuffer);
  header.pushstr_newL("unsigned id = threadIdx.x + blockDim.x * blockIdx.x;");
  sprintf(strBuffer, "if(id < %s) {", name);
  header.pushstr_newL(strBuffer);
  header.pushstr_newL("d_array[id]=initVal;");
  header.pushstr_newL("}");
  header.pushstr_newL("}");
}

//~ initKernel<double> <<<numBlocks,numThreads>>>(V,d_sigma,0);
void dsl_cpp_generator::generateInitkernelStr(const char *inVarType, const char *inVarName, const char *initVal)
{
  char strBuffer[1024];
  sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V,%s,%s);", inVarType, inVarName, initVal);
  main.pushstr_newL(strBuffer);
}
void dsl_cpp_generator::generateInitkernel1(
    assignment *assign, bool isMainFile , bool isPropEdge = false)
{ // const char* typ,
  //~ initKernel<double> <<<numBlocks,numThreads>>>(V,d_BC, 0.0);
  char strBuffer[1024];

  Identifier *inId = assign->getId();
  Expression *exprAssigned = assign->getExpr();

  const char *inVarType =
      convertToCppType(inId->getSymbolInfo()->getType()->getInnerTargetType());
  const char *inVarName = inId->getIdentifier();
  sprintf(strBuffer,"for(int i=0;i<devicecount;i++)");
  main.pushString(strBuffer);
  main.NewLine();
  sprintf(strBuffer,"{");
  main.pushString(strBuffer);
  main.NewLine();
  sprintf(strBuffer,"cudaSetDevice(i);");
  main.pushString(strBuffer);
main.NewLine();
if(!isPropEdge){
  sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V,d_%s[i],(%s)",
          inVarType, inVarName, inVarType);
  main.pushString(strBuffer);
}
else{
  sprintf(strBuffer, "initKernel<%s> <<<numBlocks_Edge,threadsPerBlock>>>(E,d_%s[i],(%s)",
          inVarType, inVarName, inVarType);
  main.pushString(strBuffer);
}
  std::cout << "varName:" << inVarName << '\n';
  generateExpr(exprAssigned,isMainFile); // asssuming int/float const literal // OUTPUTS INIT VALUE

  main.pushstr_newL(");");
  
  sprintf(strBuffer, "}");

  main.pushString(strBuffer);
  main.NewLine();
  main.pushstr_newL("for(int i=0;i<devicecount;i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaDeviceSynchronize();");
  main.pushstr_newL("}");
  main.NewLine();

}

void dsl_cpp_generator::generateLaunchConfig(const char *name)
{
  //~ LAUNCH CONFIG
  //~ const unsigned threadsPerBlock = 512;                                   //
  //~ unsigned numThreads   = V < threadsPerBlock ?  512: V;                  //
  //~ unsigned numBlocks    =(numThreads+threadsPerBlock-1)/threadsPerBlock;  //

  char strBuffer[1024];
  main.NewLine();
  const unsigned threadsPerBlock = 1024;
  const char *totalThreads = (strcmp(name, "nodes") == 0) ? "V" : "E";
  sprintf(strBuffer, "const unsigned threadsPerBlock = %u;", threadsPerBlock);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer, "unsigned numThreads   = (%s < threadsPerBlock)? %s: %u;", totalThreads,totalThreads, threadsPerBlock);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,
          "unsigned numBlocks    = "
          "(%s+threadsPerBlock-1)/threadsPerBlock;",
          totalThreads);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,
          "unsigned numBlocksKernel    = "
          "(%s+threadsPerBlock-1)/threadsPerBlock;",
          totalThreads);
  main.pushstr_newL(strBuffer);
  const char* totalThreads_Edge = "E";
sprintf(strBuffer,
          "unsigned numBlocks_Edge    = "
          "(%s+threadsPerBlock-1)/threadsPerBlock;",
          totalThreads_Edge);
  main.pushstr_newL(strBuffer);

  main.NewLine();
  
  // main.pushstr_newL("if(devicecount>1){");
  // main.pushstr_newL("numBlocksKernel = numBlocksKernel/devicecount+1;");
  // main.pushstr_newL("}");
  // main.NewLine();
  // main.pushstr_newL("}");
}

void dsl_cpp_generator::generateCudaMemCpyStr(const char *sVarName,
                                              const char *tVarName,
                                              const char *type,
                                              const char *sizeV,
                                              bool isH2D = true)
{
  //~ cudaMemcpy(  d_data,   h_data, sizeof(int)*(   E),
  // cudaMemcpyHostToDevice);
  //                1         2               3       4       5
  char strBuffer[1024];
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("int s = h_vertex_partition[i], e = h_vertex_partition[i+1] ; ") ; 
  sprintf(strBuffer, "cudaMemcpyAsync(%8s + s, %8s[i] + s, sizeof(%3s)*(e-s), %s);", sVarName,
          tVarName, type,
          (isH2D ? "cudaMemcpyHostToDevice" : "cudaMemcpyDeviceToHost"));
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("}") ; 

  main.pushstr_newL("for (int i = 0 ; i < devicecount; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i) ; ") ; 
  main.pushstr_newL("cudaDeviceSynchronize();") ; 
  main.pushstr_newL("}") ; 
}

void dsl_cpp_generator::addIncludeToFile(
    const char *includeName, dslCodePad &file,
    bool isCppLib)
{ // cout<<"ENTERED TO THIS ADD INCLUDE FILE"<<"\n";
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
  char temp[1024];
  header.pushstr_newL("// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after");
  main.pushstr_newL("// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after");
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
  addIncludeToFile("cuda.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("../graph.hpp", header, false);
  header.pushString("#include ");
  addIncludeToFile("../libcuda.cuh", header, false);
  header.pushString("#include ");
  addIncludeToFile("curand.h", header, true);
  header.pushstr_newL("#include <cooperative_groups.h>");
  // header.pushstr_newL("graph &g = NULL;");  //temporary fix - to fix the PageRank graph g instance

  header.NewLine();

  main.pushString("#include ");
  sprintf(temp, "%s.h", fileName);
  addIncludeToFile(temp, main, false);
  main.NewLine();
}

void add_InitialDeclarations(dslCodePad *main, iterateBFS *bfsAbstraction)
{
  char strBuffer[1024];
  char *graphId = bfsAbstraction->getGraphCandidate()->getIdentifier();
  sprintf(strBuffer, "std::vector<std::vector<int>> %s(%s.%s()) ;",
          "levelNodes", graphId, "num_nodes");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer, "std::vector<std::vector<int>>  %s(%s()) ;",
          "levelNodes_later", "omp_get_max_threads");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer, "std::vector<int>  %s(%s.%s()) ;", "levelCount", graphId,
          "num_nodes");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("int phase = 0 ;");
  sprintf(strBuffer, "levelNodes[phase].push_back(%s) ;",
          bfsAbstraction->getRootNode()->getIdentifier());
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer, "%s bfsCount = %s ;", "int", "1");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("levelCount[phase] = bfsCount;");
}

void add_BFSIterationLoop(dslCodePad *main, iterateBFS *bfsAbstraction)
{
  char strBuffer[1024];
  char *iterNode = bfsAbstraction->getIteratorNode()->getIdentifier();
  char *graphId = bfsAbstraction->getGraphCandidate()->getIdentifier();
  main->pushstr_newL("while ( bfsCount > 0 )");
  main->pushstr_newL("{");
  main->pushstr_newL(" int prev_count = bfsCount ;");
  main->pushstr_newL("bfsCount = 0 ;");
  main->pushstr_newL("#pragma omp parallel for");
  sprintf(strBuffer, "for( %s %s = %s; %s < prev_count ; %s++)", "int", "l",
          "0", "l", "l");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("{");
  sprintf(strBuffer, "int %s = levelNodes[phase][%s] ;", iterNode, "l");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer, "for(%s %s = %s.%s[%s] ; %s < %s.%s[%s+1] ; %s++) ", "int",
          "edge", graphId, "indexofNodes", iterNode, "edge", graphId,
          "indexofNodes", iterNode, "edge");
  main->pushString(strBuffer);
  main->pushstr_newL("{");
  sprintf(strBuffer, "%s %s = %s.%s[%s] ;", "int", "nbr", graphId, "edgeList",
          "edge");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("int dnbr ;");
  main->pushstr_newL("if(bfsDist[nbr]<0)");
  main->pushstr_newL("{");
  sprintf(strBuffer, "dnbr = %s(&bfsDist[nbr],-1,bfsDist[%s]+1);",
          "__sync_val_compare_and_swap", iterNode);
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("if (dnbr < 0)");
  main->pushstr_newL("{");
  sprintf(strBuffer, "%s %s = %s();", "int", "num_thread",
          "omp_get_thread_num");
  // sprintf(strBuffer,"int %s = bfsCount.fetch_add(%s,%s)
  // ;","loc","1","std::memory_order_relaxed");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer, " levelNodes_later[%s].push_back(%s) ;", "num_thread",
          "nbr");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("}");
  main->pushstr_newL("}");
  main->pushstr_newL("}");
}
void add_RBFSIterationLoop(dslCodePad *main, iterateBFS *bfsAbstraction)
{
  char strBuffer[1024];
  main->pushstr_newL("while (phase > 0)");
  main->pushstr_newL("{");
  main->pushstr_newL("#pragma omp parallel for");
  sprintf(strBuffer, "for( %s %s = %s; %s < levelCount[phase] ; %s++)", "int",
          "l", "0", "l", "l");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("{");
  sprintf(strBuffer, "int %s = levelNodes[phase][l] ;",
          bfsAbstraction->getIteratorNode()->getIdentifier());
  main->pushstr_newL(strBuffer);
}

// CUDA
void dsl_cpp_generator::addCudaRevBFSIterationLoop(iterateBFS *bfsAbstraction)
{
  main.NewLine();
  main.pushstr_newL("hops_from_source--;");
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);");
  // generateCudaMemCpyStr("d_hops_from_source[i]", "&hops_from_source", "int", "1");
  main.pushstr_newL("cudaMemcpyAsync(d_hops_from_source[i], &hops_from_source, sizeof(int), cudaMemcpyHostToDevice) ; ") ; 
  main.pushstr_newL("}"); 
  main.NewLine();

  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){"); 
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("cudaDeviceSynchronize();") ; 
  main.pushstr_newL("}") ; 
  //~ main.pushstr_newL("{");
  //~ main.pushstr_newL("#pragma omp parallel for");
  //~ sprintf(strBuffer,"for( %s %s = %s; %s < levelCount[phase] ;
  //%s++)","int","l","0","l","l"); ~ main.pushstr_newL(strBuffer); ~
  // main.pushstr_newL("{"); ~ sprintf(strBuffer,"int %s = levelNodes[phase][l]
  //;",bfsAbstraction->getIteratorNode()->getIdentifier()); ~
  // main.pushstr_newL(strBuffer);
}

void dsl_cpp_generator::generatePropParams(list<formalParam *> paramList, bool isNeedType = true, bool isMainFile = true)
{

  list<formalParam *>::iterator itr;
  dslCodePad &targetFile = isMainFile ? main : header;
  //~ Assumes that there is at least one param already. so prefix with  "," is okay
  char strBuffer[1024];
  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {

    Type *type = (*itr)->getType();
    if (type->isPropType())
    {
      if (type->getInnerTargetType()->isPrimitiveType())
      {

        const char *temp = "d_";
        char *temp1 = (*itr)->getIdentifier()->getIdentifier();
        char *temp2 = (char *)malloc(1 + strlen(temp) + strlen(temp1));
        strcpy(temp2, temp);
        strcat(temp2, temp1);

        if (isNeedType)
          sprintf(strBuffer, ",%s* %s", convertToCppType(type->getInnerTargetType()), temp2);
        else
          sprintf(strBuffer, ",%s", temp2);
        //~ generateCudaMemCpyStr((*itr)->getIdentifier()->getIdentifier(), temp2,convertToCppType(type->getInnerTargetType()), sizeofProp, 0 );
        targetFile.pushString(strBuffer);
      }
    }
  }
}

void dsl_cpp_generator::addCudaRevBFSIterKernel(list<statement *> &statementList)
{
  //~ var v
  //~ var w
  //~   __global__ void back_pass(int n, int* d_meta,int* d_data,int* d_weight, double* d_delta, double* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished, double* d_bc) {
  //~ unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  //~ if(v >= n) return;

  //~ if(d_level[v] == *d_hops_from_source - 1) {
  //~ for(unsigned i = d_offset[v],end = d_offset[v+1]; i < end; ++i) {
  //~ unsigned w = d_edgeList[i];
  //~ if(d_level[w] == *d_hops_from_source) {
  //~ d_delta[v] += (1.0 * d_sigma[v]) / d_sigma[w] * (1.0 + d_delta[w]);
  //~ }
  //~ }
  //~ }
  //~ }
  const char *loopVar = "v";
  //~ const char* nbbrVar = "w";
  char strBuffer[1024];

  //~ statement* body = bfsAbstraction->getBody();
  //~ assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  //~ blockStatement* block = (blockStatement*)body;
  //~ list<statement*> statementList = block->returnStatements();
  sprintf(strBuffer, "__global__ void back_pass(int n,int device,int start,int end,int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished");
  header.pushString(strBuffer);

  generatePropParams(getCurrentFunc()->getParamList(), true, false); // true: typeneed false:inMainfile
  // float* d_BC i.e ====> type* varName

  header.pushstr_newL(") {");

  sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
  header.pushstr_newL(strBuffer);

  sprintf(strBuffer, "if(%s >= n) return;", loopVar);
  header.pushstr_newL(strBuffer);
  // header.pushstr_newL("auto grid = cooper//ative_groups::this_grid();");

  sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source-1) {", loopVar);
  header.pushstr_newL(strBuffer);
  
  sprintf(strBuffer, "if ( %s >= start && %s < end) {", loopVar, loopVar); 
  header.pushstr_newL(strBuffer); 
  
  for (statement *stmt : statementList)
  {
    generateStatement(stmt, false); // false. All these stmts should be inside kernel
  }

  header.pushstr_newL("} // end if d lvl");
  header.pushstr_newL("} // end if start <= v && v < end");   
  header.pushstr_newL("} // kernel end");
  header.NewLine();
}
void dsl_cpp_generator::addCudaBFSIterKernel(iterateBFS *bfsAbstraction)
{
  const char *loopVar = "v";
  char strBuffer[1024];

  statement *body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement *block = (blockStatement *)body;
  list<statement *> statementList = block->returnStatements();

  header.pushString("__global__ void fwd_pass(int n,int device,int start,int end,int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished");

  generatePropParams(getCurrentFunc()->getParamList(), true, false); // true: typeneed false:inMainfile

  header.pushstr_newL(") {");

  sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
  header.pushstr_newL(strBuffer);
  
  sprintf(strBuffer, "%s = %s + start;", loopVar, loopVar);
  header.pushstr_newL(strBuffer);

  sprintf(strBuffer, "if(%s >= n) return;", loopVar);
  header.pushstr_newL(strBuffer);

  sprintf(strBuffer, "if ( %s >= start && %s < end ) {", loopVar, loopVar);
  header.pushstr_newL(strBuffer); 

  sprintf(strBuffer, "if(d_level[%s] == -1) {", loopVar);
  header.pushstr_newL(strBuffer);

  for (statement *stmt : statementList)
  {
    generateStatement(stmt, false); 
  }
    header.pushstr_newL("}");

  header.pushstr_newL("} // end if d lvl");
  header.pushstr_newL("} // kernel end");
  header.NewLine();
}

void dsl_cpp_generator::addCudaBFSIterationLoop(iterateBFS *bfsAbstraction)
{
  main.pushstr_newL("finished = true;"); // there vars are BFS specific
  // generateCudaMemCpyStr("d_finished", "&finished", "bool", "1");
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("cudaMemcpyAsync(d_finished[i], &finished, sizeof(bool), cudaMemcpyHostToDevice);") ;
  main.pushstr_newL("}");
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaDeviceSynchronize();"); 
  main.pushstr_newL("}");

  main.NewLine();
  main.pushstr_newL("//Kernel LAUNCH");
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushString("fwd_pass<<<numBlocksKernel,threadsPerBlock>>>(V, i, h_vertex_partition[i], h_vertex_partition[i+1], d_offset[i], d_edges[i], d_weight[i], d_delta[i], d_sigma[i], d_level[i], d_hops_from_source[i], d_finished[i]");

  generatePropParams(getCurrentFunc()->getParamList(), false, true); // true: typeneed false:inMainfile

  main.pushstr_newL("[i]); ///DONE from varList");
  main.pushstr_newL("}"); 
  main.NewLine();
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ;
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaDeviceSynchronize();");
  main.pushstr_newL("}");

  addCudaBFSIterKernel(bfsAbstraction); // KERNEL BODY!!!

  //~ main.pushstr_newL("cudaDeviceSynchronize(); //MUST - rupesh");
  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("incrementDeviceVar<<<1,1>>>(d_hops_from_source[i]);");
  main.pushstr_newL("}");
  main.NewLine() ; 

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ;
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaDeviceSynchronize();");
  main.pushstr_newL("}");
  
  main.pushstr_newL("++hops_from_source; // updating the level to process in the next iteration");
  main.pushstr_newL("// k++; //DEBUG");
}

void dsl_cpp_generator::generateRevBFSAbstraction(iterateBFS *bfsAbstraction,
                                                  bool isMainFile)
{
 
  statement *body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement *block = (blockStatement *)body;
  list<statement *> statementList = block->returnStatements();
  for (statement *stmt : statementList)
  {
    generateStatement(stmt, isMainFile); // false. All these stmts should be inside kernel
  }
}

/// MAIN CUDA ITER BFS GENERATION!
void dsl_cpp_generator::generateBFSAbstraction(iterateBFS *bfsAbstraction,
                                               bool isMainFile)
{
  char strBuffer[1024];
  //~ add_InitialDeclarations(&main,bfsAbstraction);
  // printf("BFS ON GRAPH
  // %s",bfsAbstraction->getGraphCandidate()->getIdentifier()); ~
  // add_BFSIterationLoop(&main,bfsAbstraction);

  statement *body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement *block = (blockStatement *)body;
  list<statement *> statementList = block->returnStatements();

  //~ bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  //~ int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));
  //~ int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

  //~ generateExtraVariable( "bool","d_finished","1");
  //~ generateExtraVariable( "int","d_hops_from_source","1");
  //~ generateExtraVariable( "int","d_level","V");

  main.NewLine();
  main.pushstr_newL("//EXTRA vars for ITBFS AND REVBFS"); // NOT IN DSL so hardcode is fine
  
  main.pushstr_newL("bool finished;");
  main.pushstr_newL("bool* h_finished = (bool*)malloc(devicecount * sizeof(bool));"); 
  main.pushstr_newL("int hops_from_source = 0 ; ");
  main.NewLine() ; 
  main.pushstr_newL("bool **d_finished;");
  main.pushstr_newL("d_finished = (bool**)malloc(sizeof(bool*)* devicecount) ; ") ;
  main.pushstr_newL(" for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaMalloc(&d_finished[i],sizeof(bool) *(1));") ;
  main.pushstr_newL("}") ; 


  main.pushstr_newL("") ; 

  main.pushstr_newL("int **d_hops_from_source;");
  main.pushstr_newL("d_hops_from_source = (int**)malloc(sizeof(int*)*devicecount); ") ; 
  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaMalloc(&d_hops_from_source[i],sizeof(int) *(1));") ;
  main.pushstr_newL("}") ; 
  main.NewLine() ; 


  main.pushstr_newL("int** d_level ; ");
  main.pushstr_newL("d_level = (int**)malloc(sizeof(int*)*devicecount) ; "); 
  main.pushstr_newL(" for (int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaMalloc(&d_level[i],sizeof(int) *(V+1));");
  main.pushstr_newL("}"); 
  main.NewLine() ; 

  main.pushstr_newL("int** h_level_temp = (int**)malloc(sizeof(int*) * devicecount);"); 
  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("h_level_temp[i] = (int*)malloc(sizeof(int) * (V+1));");
  main.pushstr_newL("}");
  main.NewLine() ; 

  main.pushstr_newL("int* h_level = (int*)malloc(sizeof(int) * (V+1));"); 
  // main.pushstr_newL("memset(h_level, 0, sizeof(int) * V);"); 
  main.NewLine();

  // main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){"); 
  // main.pushstr_newL("h_level[i] = -1 ; ") ; 
  // main.pushstr_newL("}") ; 
  main.pushstr_newL("//EXTRA vars INITIALIZATION");

  // generateInitkernelStr("int", "d_level", "-1");
  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("initKernel<int><<<numBlocks,threadsPerBlock>>>(V,d_level[i],-1);");
  sprintf(strBuffer, "initIndex<int><<<1,1>>>(V,d_level[i],%s, 0);", bfsAbstraction->getRootNode()->getIdentifier());
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("cudaDeviceSynchronize();"); 
  main.pushstr_newL("cudaMemcpy(d_hops_from_source[i], &hops_from_source, sizeof(int), cudaMemcpyHostToDevice);");
  main.pushstr_newL("}"); 


  main.NewLine();

  main.pushstr_newL("// long k =0 ;// For DEBUG");
  main.pushstr_newL("h_sigma[src] = 1;"); 
  main.NewLine() ; 
  main.pushstr_newL("for (int i =0  ; i < V ; i++){") ; 
  main.pushstr_newL("h_level[i] = -1;") ; 
  main.pushstr_newL("}"); 
  main.pushstr_newL("do {");

  addCudaBFSIterationLoop(bfsAbstraction); // ADDS BODY OF ITERBFS + KERNEL LAUNCH

  main.NewLine();

  //~ for (statement* stmt : statementList) {
  //~ generateStatement(stmt, false);
  //~ }
  main.pushstr_newL("if (devicecount > 1){"); 
  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("int s = h_vertex_partition[i], e = h_vertex_partition[i+1] ;") ; 
  main.pushstr_newL("cudaMemcpyAsync(h_sigma+s, d_sigma[i]+s, sizeof(float) * (e-s), cudaMemcpyDeviceToHost);") ;
  main.pushstr_newL("}") ;

  main.NewLine() ;
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ;
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaDeviceSynchronize();") ; 
  main.pushstr_newL("}") ; 
  main.pushstr_newL("}") ; 
  main.NewLine() ;

  // main.pushstr_newL("for(int ver = 0 ; ver < V ; ver++){") ; 
  // main.pushstr_newL("if (h_level[ver] == hops_from_source) {") ; 
  // main.pushstr_newL("for(int device = 0 ; device < devicecount ; device++){") ; 
  // main.pushstr_newL("h_sigma[devicecount][ver] += h_sigma[device][ver];") ;
  // main.pushstr_newL("}") ;
  // main.pushstr_newL("}") ;
  // main.pushstr_newL("}");

  main.NewLine() ; 


  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaMemcpyAsync(&h_finished[i], d_finished[i], sizeof(bool), cudaMemcpyDeviceToHost);");
  main.pushstr_newL("}");
  main.NewLine() ; 

  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ;
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaDeviceSynchronize();") ;
  main.pushstr_newL("}") ;
  main.NewLine() ;

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){"); 
  main.pushstr_newL("finished = finished && h_finished[i];");  
  main.pushstr_newL("}");  

  main.NewLine() ; 

  main.pushstr_newL("if (devicecount > 1){") ; 
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ;
  main.pushstr_newL("cudaMemcpyAsync(h_level_temp[i], d_level[i], sizeof(int) * (V), cudaMemcpyDeviceToHost);");
  main.pushstr_newL("}"); 
  main.NewLine() ; 

  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("cudaDeviceSynchronize();") ; 
  main.pushstr_newL("}") ; 
  main.NewLine() ; 

  main.pushstr_newL("for(int i = 0 ; i < V ; i++){");
  main.pushstr_newL("for(int j = 0 ; j < devicecount ; j++){");
  main.pushstr_newL("if(h_level_temp[j][i] > h_level[i]){");
  main.pushstr_newL("h_level[i] = h_level_temp[j][i];"); 
  main.pushstr_newL("break ; ");
  main.pushstr_newL("}");
  main.pushstr_newL("}");
  main.pushstr_newL("}");
  main.NewLine() ; 
  // generateCudaMemCpyStr("&finished", "d_finished", "bool", "1", false);

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){"); 
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaMemcpyAsync(d_level[i], h_level, sizeof(int) * (V), cudaMemcpyHostToDevice);");
  main.pushstr_newL("cudaMemcpyAsync(d_sigma[i], h_sigma, sizeof(float) * (V), cudaMemcpyHostToDevice);");
  main.pushstr_newL("}");
  main.NewLine() ;

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaDeviceSynchronize();");
  main.pushstr_newL("}");
  main.NewLine() ;
  main.pushstr_newL("}");

  main.pushstr_newL("}while(!finished);");
  //~ main.pushstr_newL("}");

  /*
  main.pushstr_newL("phase = phase + 1 ;");
 //~ main.pushstr_newL("levelCount[phase] = bfsCount ;");
  //~ main.pushstr_newL("
 levelNodes[phase].assign(levelNodes_later.begin(),levelNodes_later.begin()+bfsCount);");
  sprintf(strBuffer,"for(int %s = 0;%s <
 %s();%s++)","i","i","omp_get_max_threads","i"); main.pushstr_newL(strBuffer);
  main.pushstr_newL("{");
  sprintf(strBuffer,"
 levelNodes[phase].insert(levelNodes[phase].end(),levelNodes_later[%s].begin(),levelNodes_later[%s].end());","i","i");
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer," bfsCount=bfsCount+levelNodes_later[%s].size();","i");
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer," levelNodes_later[%s].clear();","i");
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("}");
  main.pushstr_newL(" levelCount[phase] = bfsCount ;");
  main.pushstr_newL("}");
  main.pushstr_newL("phase = phase -1 ;");
  */

  blockStatement *
      revBlock = (blockStatement *)bfsAbstraction->getRBFS()->getBody();
  list<statement *> revStmtList = revBlock->returnStatements();
  addCudaRevBFSIterationLoop(bfsAbstraction);

  main.pushstr_newL("//BACKWARD PASS");
  main.pushstr_newL("while(hops_from_source > 1) {");

  main.NewLine();
  main.pushstr_newL("//KERNEL Launch");
  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("back_pass<<<numBlocksKernel,threadsPerBlock>>>(V, i, h_vertex_partition[i], h_vertex_partition[i+1], d_offset[i], d_edges[i], d_weight[i], d_delta[i], d_sigma[i], d_level[i], d_hops_from_source[i], d_finished[i]");

  generatePropParams(getCurrentFunc()->getParamList(), false, true); // true: typeneed false:inMainfile

  main.pushstr_newL("[i]);"); ///DONE from varList"); /// TODO get all propnodes from function body and params
  main.pushstr_newL("}"); 
  main.NewLine();
  addCudaRevBFSIterKernel(revStmtList); // KERNEL BODY

  //~ for(statement* stmt:revStmtList) {
  //~ generateStatement(stmt, false);
  //~ }

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("cudaDeviceSynchronize();") ; 
  main.pushstr_newL("}") ; 


  main.pushstr_newL("hops_from_source--;");
  // main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){"); 
  // main.pushstr_newL("cudaSetDevice(i);");
  // generateCudaMemCpyStr("d_hops_from_source[i]", "&hops_from_source", "int", "1", true);
  // main.pushstr_newL("}"); 

  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i);") ; 
  main.pushstr_newL("decrementDeviceVar<<<1,1>>>(d_hops_from_source[i]);") ; 
  main.pushstr_newL("}") ; 

  main.NewLine() ; 

  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){") ; 
  main.pushstr_newL("cudaSetDevice(i) ; ") ; 
  main.pushstr_newL("cudaDeviceSynchronize() ; ") ; 
  main.pushstr_newL("}") ; 
  // main.pushstr_newL("for(int i = 0 ; i )")

  main.pushstr_newL("if (devicecount > 1) {") ; 

  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("int s = h_vertex_partition[i], e = h_vertex_partition[i+1];") ; 
  main.pushstr_newL("cudaMemcpyAsync(h_delta+s, d_delta[i]+s, sizeof(float) * (e-s), cudaMemcpyDeviceToHost);");
  main.pushstr_newL("}") ; 



  main.pushstr_newL("for (int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaDeviceSynchronize();");
  main.pushstr_newL("}") ; 
  main.NewLine(); 

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaMemcpyAsync(d_delta[i], h_delta, sizeof(float) * (V), cudaMemcpyHostToDevice);");
  main.pushstr_newL("}"); 
  main.NewLine() ;

  main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  main.pushstr_newL("cudaSetDevice(i);");
  main.pushstr_newL("cudaDeviceSynchronize();");
  main.pushstr_newL("}");

  main.pushstr_newL("}");
  // main.pushstr_newL("for(int ver = 0 ; ver <= V ; ver++){");
  // main.pushstr_newL("for(int device = 0 ; device < devicecount ; device++){");
  // main.pushstr_newL("h_delta[ver] = h_delta[ver] + h_delta_temp[device][ver];");
  // main.pushstr_newL("}"); 
  // main.pushstr_newL("}");

  // main.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
  // main.pushstr_newL("cudaSetDevice(i);");
  // main.pushstr_newL("cudaMemcpy(d_delta[i], h_delta, sizeof(int) * (V+1), cudaMemcpyHostToDevice);"); 
  // main.pushstr_newL("}");
  // main.pushstr_newL("}"); 
  main.NewLine() ; 

  main.pushstr_newL("}");
  main.pushstr_newL("//accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);");
  //~ main.NewLine();

  //~ main.pushstr_newL("}while(!finished);");
  //~ main.pushstr_newL("}");
  //~ main.pushstr_newL("phase = phase - 1 ;");
  //~ main.pushstr_newL("}");
}


void dsl_cpp_generator::  generateStatement(statement *stmt, bool isMainFile)
{
  // char strBuffer[1024];
  // sprintf(strBuffer,"//%s",(*stmt).getType().c_str());
  // dslCodePad &targetFile = isMainFile ? main : header;
  // targetFile.pushstr_newL(strBuffer);
  cout <<  "##### " << string((*stmt).getType()) << endl ; 
  if (stmt->getTypeofNode() == NODE_BLOCKSTMT)
  {
    generateBlock((blockStatement *)stmt, false, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_DECL)
  {
    generateVariableDecl((declaration *)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_ASSIGN)
  {

    // generateAssignmentStmt((assignment*)stmt);
    assignment *asst = (assignment *)stmt;
    if (asst->isDeviceAssignment()){
          cout<<"assasd"<<endl;
      generateDeviceAssignmentStmt(asst, isMainFile);
    }
    else {
      cout<<"assasd1"<<endl;
          generateAtomicDeviceAssignmentStmt(asst, isMainFile);
    }
  }

  if (stmt->getTypeofNode() == NODE_WHILESTMT)
  {
    // generateWhileStmt((whileStmt*) stmt);
  }

  if (stmt->getTypeofNode() == NODE_IFSTMT)
  {

    generateIfStmt((ifStmt *)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_DOWHILESTMT)
  {
    generateDoWhileStmt((dowhileStmt *)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FORALLSTMT)
  {
    std::cout << "STMT: For" << '\n';
    printf("isMainFile val %d\n", isMainFile);
    generateForAll((forallStmt *)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT)
  {
    if (!modified_variable_decl){
      modified_variable_decl = true ;
      dslCodePad &targetFile = isMainFile ? main : header;
      char strBuffer[1024];
      fixedPointStmt *fixedPointConstruct = (fixedPointStmt *)stmt;
      Expression *convergeExpr = fixedPointConstruct->getDependentProp();
      Identifier *fixedPointId = fixedPointConstruct->getFixedPointId();

      //~ statement* blockStmt = fixedPointConstruct->getBody();
      assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
             convergeExpr->getExpressionFamily() == EXPR_ID);

      Identifier *dependentId = NULL;

      assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
             convergeExpr->getExpressionFamily() == EXPR_ID);

      if (convergeExpr->getExpressionFamily() == EXPR_UNARY){
        if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID){
          dependentId = convergeExpr->getUnaryExpr()->getId();
        }
      }
      const char *modifiedVar = dependentId->getIdentifier();
      char *fixPointVar = fixedPointId->getIdentifier();

      const char *fixPointVarType = convertToCppType(fixedPointId->getSymbolInfo()->getType());
      const char* modifiedVarType = convertToCppType(dependentId->getSymbolInfo()->getType());  
      char modifiedVarNext[80] = "";

      // strcat(modifiedVarPrev, modifiedVar);strcat(modifiedVarPrev, "_prev");

      //ronaldo
      strcpy(MODIFIED,modifiedVar);
      strcpy(modifiedVarNext, modifiedVar);
      strcat(modifiedVarNext, "_next");
      int firstTime = 0;
      for(auto x :declInForAll){
        if(strcmp(modifiedVarNext,x.first)==0){
          firstTime = x.second;
        }
      }
      if(firstTime==0){
        strcpy(strBuffer, modifiedVarType); 
        strcat(strBuffer, "* ");
        strcat(strBuffer, "d_"); 
        strcat(strBuffer, modifiedVarNext);
        strcat(strBuffer, ";"); 
        targetFile.pushstr_newL(strBuffer);

        generateCudaMalloc(dependentId->getSymbolInfo()->getType(), modifiedVarNext) ; 
      }    
      cout<<"hdwhf"<<endl;
      main.NewLine() ; 
    }
      generateFixedPoint((fixedPointStmt *)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
  {
    cout<<"iufhsidfdihf"<<endl;
    generateReductionStmt((reductionCallStmt *)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_ITRBFS)
  {
    generateBFSAbstraction((iterateBFS *)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_PROCCALLSTMT)
  {
    generateProcCall((proc_callStmt *)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_UNARYSTMT)
  {
    unary_stmt *unaryStmt = (unary_stmt *)stmt;
    generateExpr(unaryStmt->getUnaryExpr(), isMainFile);
    main.pushstr_newL(";");
  }
}

void dsl_cpp_generator::generateAtomicBlock(bool isMainFile)
{
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  main.pushstr_newL("atomicMin(&gpu_dist[nbr] , dist_new);");
  main.pushstr_newL("gpu_modified_next[nbr]=true;");
  main.pushstr_newL("gpu_finished[0] = false;");
}


void dsl_cpp_generator::generateBody(reductionCallStmt *stmt,bool isMainFile){
  reductionCall *reduceCall = stmt->getReducCall();
  char strBuffer[1024];
  if (reduceCall->getReductionType() == REDUCE_MIN){
    Type* type = stmt->getAssignedId()->getSymbolInfo()->getType()->getInnerTargetType();
    char* type_name = (char*)convertToCppType(type);
    char* iden = stmt->getAssignedId()->getIdentifier();
    sprintf(strBuffer,"//global loop var %s iden %s",globalLoopVar,stmt->getTargetPropId()->getIdentifier1()->getIdentifier());
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("if(devicecount>1){");
    if(strcmp(globalLoopVar,stmt->getTargetPropId()->getIdentifier1()->getIdentifier())==0){
      main.pushstr_newL("//pull based\n");
      pullBased=1;
      main.pushstr_newL("for(int i=0;i<devicecount;i++){");
      main.pushstr_newL("cudaSetDevice(i);");
      sprintf(strBuffer,"cudaMemcpyAsync(h_%s+h_vertex_partition[i],d_%s[i]+h_vertex_partition[i],sizeof(%s)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);",iden,iden,type_name);
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("}");
      main.pushstr_newL("for(int i=0;i<devicecount;i++){");
      main.pushstr_newL("cudaSetDevice(i);");
      main.pushstr_newL("cudaDeviceSynchronize();");
      main.pushstr_newL("}");
      

    }
    else{
      pullBased=0;
    main.pushstr_newL("//push based\n");
    main.pushstr_newL("cudaSetDevice(0);");
    sprintf(strBuffer,"%s* d_%s_temp;",type_name,stmt->getAssignedId()->getIdentifier());
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer,"%s* d_%s_temp1;",type_name,stmt->getAssignedId()->getIdentifier());
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer,"cudaMalloc(&d_%s_temp , (V+1)*sizeof(%s));",stmt->getAssignedId()->getIdentifier(),type_name);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer,"cudaMalloc(&d_%s_temp1,(V+1)*(devicecount)*sizeof(%s));",stmt->getAssignedId()->getIdentifier(),type_name);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer,"initKernel<%s><<<numBlocks,threadsPerBlock>>>(V+1,d_%s_temp,(%s)INT_MAX);",type_name,iden,type_name);
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    sprintf(strBuffer,"cudaMemcpy(d_%s_temp1+i*(V+1),h_%s1+i*(V+1),sizeof(%s)*(V+1),cudaMemcpyHostToDevice);",iden,iden,type_name);
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("}");
    sprintf(strBuffer,"Compute_Min<<<numBlocks,numThreads>>>(d_%s_temp1,d_%s_temp,V,devicecount);",iden,iden);
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer,"cudaMemcpy(h_%s,d_%s_temp,sizeof(%s)*(V+1),cudaMemcpyDeviceToHost);",iden,iden,type_name);
    main.pushstr_newL(strBuffer);
    }

    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    main.pushstr_newL("cudaSetDevice(i);");
    sprintf(strBuffer,"cudaMemcpyAsync(d_%s[i],h_%s,sizeof(%s)*(V+1),cudaMemcpyHostToDevice);",iden,iden,type_name);
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("}");
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("cudaDeviceSynchronize();");
    main.pushstr_newL("}");
    main.pushstr_newL("}");

  }
}

void dsl_cpp_generator::generateReductionCallStmt(reductionCallStmt *stmt,
                                                  bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  reductionCall *reduceCall = stmt->getReducCall();
  char strBuffer[1024];
  if (reduceCall->getReductionType() == REDUCE_MIN)
  {
    if (stmt->isListInvolved())
    {
      //~ cout << "INSIDE THIS OF LIST PRESENT"
      //~ << "\n";
      list<argument *> argList = reduceCall->getargList();
      list<ASTNode *> leftList = stmt->getLeftList();
      //~ int i = 0;
      list<ASTNode *> rightList = stmt->getRightList();
      printf("LEFT LIST SIZE %lu \n", leftList.size());

      targetFile.space();
      if (stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType())
      {
        Type *type = stmt->getAssignedId()->getSymbolInfo()->getType();

        targetFile.pushstr_space(convertToCppType(type->getInnerTargetType()));
      }
      sprintf(strBuffer, "%s_new", stmt->getAssignedId()->getIdentifier());
      std::cout << "VAR:" << stmt->getAssignedId()->getIdentifier() << '\n';
      targetFile.pushString(strBuffer);
      list<argument *>::iterator argItr;
      argItr = argList.begin();
      argItr++;
      targetFile.pushString(" = ");
      generateExpr((*argItr)->getExpr(), isMainFile);
      targetFile.pushstr_newL(";");
      list<ASTNode *>::iterator itr1;
      list<ASTNode *>::iterator itr2;
      itr2 = rightList.begin();
      itr1 = leftList.begin();
      itr1++;
      for (; itr1 != leftList.end(); itr1++)
      {
        ASTNode *node = *itr1;
        ASTNode *node1 = *itr2;

        if (node->getTypeofNode() == NODE_ID)
        {
          targetFile.pushstr_space(convertToCppType(
              ((Identifier *)node)->getSymbolInfo()->getType()));
          sprintf(strBuffer, "%s_new", ((Identifier *)node)->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.pushString(" = ");
          generateExpr((Expression *)node1, isMainFile);
        }
        if (node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess *p = (PropAccess *)node;
          Type *type = p->getIdentifier2()->getSymbolInfo()->getType();
          if (type->isPropType())
          {
            targetFile.pushstr_space(
                convertToCppType(type->getInnerTargetType()));
          }

          sprintf(strBuffer, "%s_new", p->getIdentifier2()->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.pushString(" = ");
          //~ Expression* expr = (Expression*)node1;
          generateExpr((Expression *)node1, isMainFile);
          targetFile.pushstr_newL(";");
        }
        itr2++;
      }
      char* iden = (*argItr)->getExpr()->getLeft()->getPropId()->getIdentifier1()->getIdentifier();
      targetFile.pushString("if(");
      sprintf(strBuffer, "d_%s[%s]!= INT_MAX && ",stmt->getAssignedId()->getIdentifier(),iden);
      targetFile.pushString(strBuffer);
      generate_exprPropId(stmt->getTargetPropId(), isMainFile);

      sprintf(strBuffer, " > %s_new)", stmt->getAssignedId()->getIdentifier());
      targetFile.pushstr_newL(strBuffer);
      targetFile.pushstr_newL("{"); // added for testing then doing atomic min.
      /* storing the old value before doing a atomic operation on the node
       * property */
      if (stmt->isTargetId())
      {
        Identifier *targetId = stmt->getTargetId();
        targetFile.pushstr_space(
            convertToCppType(targetId->getSymbolInfo()->getType()));
        targetFile.pushstr_space("oldValue");
        targetFile.pushstr_space("=");
        generate_exprIdentifier(stmt->getTargetId(), isMainFile);
        targetFile.pushstr_newL(";");
      }
      else
      {
        PropAccess *targetProp = stmt->getTargetPropId();
        Type *type = targetProp->getIdentifier2()->getSymbolInfo()->getType();
        if (type->isPropType())
        {
          // targetFile.pushstr_space(convertToCppType(type->getInnerTargetType()));
          // targetFile.pushstr_space("oldValue");
          // targetFile.pushstr_space("=");
          // generate_exprPropId(stmt->getTargetPropId(), isMainFile);
          // targetFile.pushstr_newL(";");
        }
      }

      targetFile.pushString("atomicMin(&");
      generate_exprPropId(stmt->getTargetPropId(), isMainFile);

      sprintf(strBuffer, ",%s_new);", stmt->getAssignedId()->getIdentifier());

      targetFile.pushstr_newL(strBuffer);
      // sprintf(strBuffer,"if(%s > ","oldValue");
      // targetFile.pushString(strBuffer);
      // generate_exprPropId(stmt->getTargetPropId(), isMainFile);
      // targetFile.pushstr_newL(")");
      // targetFile.pushstr_newL("{");
      Identifier* id2 = stmt->getTargetPropId()->getIdentifier2();
      Type* type = id2->getSymbolInfo()->getType();
      if(strcmp(globalLoopVar,stmt->getTargetPropId()->getIdentifier1()->getIdentifier())!=0){
      sprintf(strBuffer,"%s* h_%s1;",convertToCppType(type->getInnerTargetType()),id2->getIdentifier());
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("if(devicecount>1){");
      main.pushstr_newL("//push based");
      sprintf(strBuffer," h_%s1 = (%s*)malloc((V+1)*(devicecount)*sizeof(%s));",id2->getIdentifier(),convertToCppType(type->getInnerTargetType()),convertToCppType(type->getInnerTargetType()));
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("for(int i=0;i<devicecount;i++){");
      main.pushstr_newL("cudaSetDevice(i);");
      sprintf(strBuffer,"cudaMemcpyAsync(h_%s1+i*(V+1),d_%s[i],sizeof(%s)*(V+1),cudaMemcpyDeviceToHost);",id2->getIdentifier(),id2->getIdentifier(),convertToCppType(type->getInnerTargetType()));
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("}");
      main.pushstr_newL("for(int i=0;i<devicecount;i++){");
      main.pushstr_newL("cudaSetDevice(i);");
      main.pushstr_newL("cudaDeviceSynchronize();");
      main.pushstr_newL("}");
      main.pushstr_newL("}");
      }
      itr1 = leftList.begin();
      itr1++;
      for (; itr1 != leftList.end(); itr1++)
      {
        ASTNode *node = *itr1;
        Identifier *affected_Id = NULL;
        if (node->getTypeofNode() == NODE_ID)
        {
          generate_exprIdentifier((Identifier *)node, isMainFile);
        }
        if (node->getTypeofNode() == NODE_PROPACCESS)
        {
          generate_exprPropId((PropAccess *)node, isMainFile);
        }
        targetFile.space();
        targetFile.pushstr_space("= true");
        if (node->getTypeofNode() == NODE_ID)
        {
          // generate_exprIdentifier((Identifier *)node, isMainFile);
          affected_Id = (Identifier *)node;
        }
        if (node->getTypeofNode() == NODE_PROPACCESS) // here
        {
          // generate_exprIdentifier(((PropAccess *)node)->getIdentifier2(),isMainFile);
          affected_Id = ((PropAccess *)node)->getIdentifier2();
        }
        // targetFile.pushString("_new");
        targetFile.pushstr_newL(";");

        if (affected_Id->getSymbolInfo()->getId()->get_fp_association())
        {
          char *fpId = affected_Id->getSymbolInfo()->getId()->get_fpId();
          sprintf(strBuffer, "d_%s[0] = %s ;", fpId, "false");
          std::cout << "FPID ========> " << fpId << '\n';
          targetFile.pushstr_newL(strBuffer);
          //~ targetFile.pushstr_newL("}");  // needs to be removed
          //~ targetFile.pushstr_newL("}");  // needs to be removed
        }
      }
      // targetFile.pushstr_newL("}");
      targetFile.pushstr_newL("}"); // added for testing condition..then atomicmin.
    }
  }
}

void dsl_cpp_generator::generateReductionOpStmt(reductionCallStmt *stmt,
                                                bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  char strBuffer[1024];

  if (stmt->isLeftIdentifier())
  {
    Identifier *id = stmt->getLeftId(); // For Atomic from ashwina
    const char *typVar = convertToCppType(id->getSymbolInfo()->getType());
    if(strcmp(typVar,"long")==0 || strcmp(typVar,"long long")==0 || strcmp(typVar,"long int")==0 || strcmp(typVar,"long long int")==0){
      sprintf(strBuffer, "atomicAdd((unsigned long long*) &d_%s[0], (unsigned long long)", id->getIdentifier());
    }
    else{
      sprintf(strBuffer, "atomicAdd(&d_%s[0], (%s)", id->getIdentifier(), typVar);
    }  
    targetFile.pushString(strBuffer);
    generateExpr(stmt->getRightSide(), isMainFile);
    targetFile.pushstr_newL(");");
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");  
    main.pushstr_newL("cudaSetDevice(i);");
    sprintf(strBuffer,"cudaMemcpyAsync(h_%s[i],d_%s[i],sizeof(%s),cudaMemcpyDeviceToHost);",id->getIdentifier(),id->getIdentifier(),typVar);
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("}");
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");  
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("cudaDeviceSynchronize();");
    main.pushstr_newL("}");
    sprintf(strBuffer,"%s %s_=0;",typVar,id->getIdentifier());
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");  
    sprintf(strBuffer, "%s_ += h_%s[i][0];",id->getIdentifier(),id->getIdentifier());
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("} //end of for");
    sprintf(strBuffer,"%s=%s_;",id->getIdentifier(),id->getIdentifier());
    main.pushstr_newL(strBuffer);
    
  }
  else
  {
    targetFile.pushString("atomicAdd(&") ; 
    generate_exprPropId(stmt->getPropAccess(), isMainFile);
    targetFile.pushString(" , ");
    // const char *operatorString = getOperatorString(stmt->reduction_op());
    // targetFile.pushstr_space(operatorString);
    generateExpr(stmt->getRightSide(), isMainFile);
    targetFile.pushstr_newL(");");
    helper(stmt->getPropAccess()->getIdentifier2(),stmt->getPropAccess()->getIdentifier1());
  }
}

void dsl_cpp_generator::helper(Identifier* iden,Identifier* loopVar) 
{
  char strBuffer[1024];
  Type *type = iden->getSymbolInfo()->getType();
  
  
     if(type->isPropType()){
        if(globalLoopVar==NULL){
          return;
        }
        char* ch = "V+1";
        if(type->isPropEdgeType()){
          ch="E";
        }
        char* ch1 = "V";
        if(type->isPropEdgeType()){
          ch1="E-1";
        }
        char* ch2 = "numBlocks";
        if(type->isPropEdgeType()){
          ch2 = "numBlocks_Edge";
        } 
        if(iden->getSymbolInfo()->getId()->get_fp_association()){
          main.pushstr_newL("if(devicecount==1){");
          sprintf(strBuffer,"cudaMemcpy(d_%s[0],d_%s_next[0],sizeof(%s)*(%s),cudaMemcpyDeviceToDevice);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()),ch);
          main.pushstr_newL(strBuffer);
          
        main.pushstr_newL("}");  
        }
        main.pushstr_newL("if(devicecount>1){");
        sprintf(strBuffer,"//%s %s",globalLoopVar,loopVar->getIdentifier());
        main.pushstr_newL(strBuffer);
        // return;
        if(strcmp(globalLoopVar,loopVar->getIdentifier())==0){
          
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          if(iden->getSymbolInfo()->getId()->get_fp_association()){
          sprintf(strBuffer,"cudaMemcpyAsync(h_%s+h_vertex_partition[i],d_%s_next[i]+h_vertex_partition[i],sizeof(%s)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
          }
          else{
            sprintf(strBuffer,"cudaMemcpyAsync(h_%s+h_vertex_partition[i],d_%s[i]+h_vertex_partition[i],sizeof(%s)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
          }
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("}");
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          main.pushstr_newL("cudaDeviceSynchronize();");
          main.pushstr_newL("}");
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          sprintf(strBuffer,"cudaMemcpyAsync(d_%s[i],h_%s,sizeof(%s)*(V+1),cudaMemcpyHostToDevice);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("}");
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          main.pushstr_newL("cudaDeviceSynchronize();");
          main.pushstr_newL("}");                
        }
        else{
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          if(iden->getSymbolInfo()->getId()->get_fp_association()){
          sprintf(strBuffer,"cudaMemcpyAsync(h_%s_temp1+i*(%s),d_%s_next[i],sizeof(%s)*(%s),cudaMemcpyDeviceToHost);",iden->getIdentifier(),ch,iden->getIdentifier(),convertToCppType(type->getInnerTargetType()),ch);
          }
          else{
            sprintf(strBuffer,"cudaMemcpyAsync(h_%s_temp1+i*(%s),d_%s[i],sizeof(%s)*(%s),cudaMemcpyDeviceToHost);",iden->getIdentifier(),ch,iden->getIdentifier(),convertToCppType(type->getInnerTargetType()),ch);
          }
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("}");  
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          main.pushstr_newL("cudaDeviceSynchronize();");
          main.pushstr_newL("}");        
          
          main.pushstr_newL("cudaSetDevice(0);");
          sprintf(strBuffer,"cudaMemcpy(d_%s_temp1,h_%s_temp1,(%s)*(devicecount)*sizeof(%s),cudaMemcpyHostToDevice);",iden->getIdentifier(),iden->getIdentifier(),ch,convertToCppType(type->getInnerTargetType()));
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          sprintf(strBuffer,"cudaMemcpy(d_%s_temp2+i*(%s),h_%s,sizeof(%s)*(%s),cudaMemcpyHostToDevice);",iden->getIdentifier(),ch,iden->getIdentifier(),convertToCppType(type->getInnerTargetType()),ch);
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("}");
          sprintf(strBuffer,"Compute_correct<%s><<<%s,threadsPerBlock>>>(d_%s_temp1,d_%s_temp2,%s,devicecount);",convertToCppType(type->getInnerTargetType()),ch2,iden->getIdentifier(),iden->getIdentifier(),ch1);
          main.pushstr_newL(strBuffer);
          sprintf(strBuffer,"cudaMemcpy(h_%s,d_%s_temp1,(%s)*sizeof(%s),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),ch,convertToCppType(type->getInnerTargetType()));
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          sprintf(strBuffer,"cudaMemcpyAsync(d_%s[i],h_%s,(%s)*sizeof(%s),cudaMemcpyHostToDevice);",iden->getIdentifier(),iden->getIdentifier(),ch,convertToCppType(type->getInnerTargetType()));
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("}");
          main.pushstr_newL("for(int i=0;i<devicecount;i++){");
          main.pushstr_newL("cudaSetDevice(i);");
          main.pushstr_newL("cudaDeviceSynchronize();");
          main.pushstr_newL("}");  
        }
          main.pushstr_newL("}");
      }
}


void dsl_cpp_generator::generateReductionStmt(reductionCallStmt *stmt,
                                              bool isMainFile)
{
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  //~ char strBuffer[1024]
  cout<<"sdkfdwjhf"<<endl;
  if (stmt->is_reducCall())
  {
    generateReductionCallStmt(stmt, isMainFile);
    generateBody(stmt,true);
  }
  else
  {
    generateReductionOpStmt(stmt, isMainFile);
  }
}

void dsl_cpp_generator::generateDoWhileStmt(dowhileStmt *doWhile,
                                            bool isMainFile)
{
  cout<<"do while"<<endl;
  dslCodePad &targetFile = isMainFile ? main : header;
  targetFile.pushstr_newL("do{");
  //~ targetFile.pushString("{");
  generateStatement(doWhile->getBody(), isMainFile);
  //~ targetFile.pushString("}");
  targetFile.pushString("}while(");
  generateExpr(doWhile->getCondition(), isMainFile);
  targetFile.pushstr_newL(");");
}

void dsl_cpp_generator::generateIfStmt(ifStmt *ifstmt, bool isMainFile)
{

  dslCodePad &targetFile = isMainFile ? main : header;
  Expression *condition = ifstmt->getCondition();
  targetFile.pushString("if (");
  std::cout << "=======IF FILTER" << '\n';
  generateExpr(condition, isMainFile);
  //~ targetFile.pushString(" )");
  targetFile.pushstr_newL("){ // if filter begin ");

  generateStatement(ifstmt->getIfBody(), isMainFile);
  targetFile.pushstr_newL("} // if filter end");
  if (ifstmt->getElseBody() == NULL)
    return;
  targetFile.pushstr_newL("else");
  generateStatement(ifstmt->getElseBody(), isMainFile);
}

void dsl_cpp_generator::findTargetGraph(vector<Identifier *> graphTypes,
                                        Type *type)
{
  if (graphTypes.size() > 1)
  {
    //~ cerr << "TargetGraph can't match";
  }
  else
  {
    Identifier *graphId = graphTypes[0];

    type->setTargetGraph(graphId);
  }
}

void dsl_cpp_generator::generateAssignmentStmt(assignment *asmt, bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;
  //~ bool isAtomic = false;
  std::cout << "\tGenerate ASST STMT" << '\n';
  if (asmt->lhs_isIdentifier())
  {
    Identifier *id = asmt->getId();
    Expression *exprAssigned = asmt->getExpr();
    if (asmt->hasPropCopy()) // prop_copy is of the form (propId = propId)
    {
      char strBuffer[1024];
      Identifier *rhsPropId2 = exprAssigned->getId();
      sprintf(strBuffer, "for (%s %s = 0; %s < %s; %s ++) ", "int", "node", "node", "V", "node");
      targetFile.pushstr_newL(strBuffer);
      /* the graph associated                          */
      targetFile.pushstr_newL("{");
      sprintf(strBuffer, "%s [%s] = %s [%s] ;", id->getIdentifier(), "node", rhsPropId2->getIdentifier(), "node");
      targetFile.pushstr_newL(strBuffer);
      targetFile.pushstr_newL("}");
    }
    else
      //~ if(prefixNeed)
      //~ targetFile.pushString("d_");
      targetFile.pushString(id->getIdentifier());
  }
  else if (asmt->lhs_isProp()) // the check for node and edge property to be
                               // carried out.
  {
    PropAccess *propId = asmt->getPropId();
    if (asmt->getAtomicSignal())
    {
      if (asmt->getParent()->getParent()->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
        if (asmt->getExpr()->isArithmetic() || asmt->getExpr()->isLogical())
        {
          std::cout << "!atomic!" << '\n';
          //~ targetFile.pushstr_newL("atomicAdd(&");
          //~ isAtomic = true;
        }
      // targetFile.pushstr_newL("#pragma omp atomic");
    }
    targetFile.pushString(propId->getIdentifier2()->getIdentifier());
    targetFile.push('[');
    targetFile.pushString(propId->getIdentifier1()->getIdentifier());
    targetFile.push(']');
  }

  if (!asmt->hasPropCopy())
  {
    targetFile.pushString(" = ");
    generateExpr(asmt->getExpr(), isMainFile);
    targetFile.pushstr_newL(";");
  }
}

void dsl_cpp_generator::generateAtomicDeviceAssignmentStmt(assignment *asmt,
                                                           bool isMainFile)
{
  
  std::cout << "\tASST kjdskfs\t";
  dslCodePad &targetFile = isMainFile ? main : header;
  bool isAtomic = false;
  bool isResult = false;
  int isForall = 0;
  int isForall1 = 0;
     
  if (asmt->lhs_isIdentifier())
  {
    
    Identifier *id = asmt->getId();
    Expression *exprAssigned = asmt->getExpr();
    if (asmt->hasPropCopy()) // prop_copy is of the form (propId = propId)
    {
      char strBuffer[1024];
      Identifier *rhsPropId2 = exprAssigned->getId();
      Type *type = id->getSymbolInfo()->getType();
      //need to change
      targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
      targetFile.pushstr_newL("cudaSetDevice(i);");
      sprintf(strBuffer,"cudaMemcpy(d_%s[i],d_%s[i],sizeof(%s)*(V+1),cudaMemcpyDeviceToDevice);",id->getIdentifier(),rhsPropId2->getIdentifier(),convertToCppType(type->getInnerTargetType()));
      targetFile.pushstr_newL(strBuffer);
      targetFile.pushstr_newL("}");
      targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
      targetFile.pushstr_newL("cudaSetDevice(i);");
      targetFile.pushstr_newL("cudaDeviceSynchronize();");
      targetFile.pushstr_newL("}");
      
    }
    else{
      ASTNode* tempstmt = asmt;
      while(tempstmt){
      if(tempstmt->getTypeofNode()==18){
        isForall1 = 1;
        break;
      }
      tempstmt = tempstmt->getParent();
    }
      for(auto x :declInForAll){
        if(strcmp(id->getIdentifier(),x.first)==0){
          isForall = x.second;
        }
      }
      if(isForall==2||isForall1==0){
        targetFile.pushString(id->getIdentifier());
      }
      else{
        targetFile.pushString("d_");
        targetFile.pushString(id->getIdentifier());
        targetFile.pushString("[0]");
        
      }
    }
  }
  else if (asmt->lhs_isProp()) // the check for node and edge property to be
                               // carried out.
  {
    PropAccess *propId = asmt->getPropId();
    if (asmt->isDeviceAssignment())
    {
      std::cout << "\t  DEVICE ASST" << '\n';
    }
    if (asmt->getAtomicSignal())
    {
      targetFile.pushString("atomicAdd(&");
      isAtomic = true;
      std::cout << "\t  ATOMIC ASST" << '\n';
    }
    if (asmt->isAccumulateKernel())
    { // NOT needed
      isResult = true;
      std::cout << "\t  RESULT NO BC by 2 ASST" << '\n';
    }
    targetFile.pushString("d_"); /// IMPORTANT
    targetFile.pushString(propId->getIdentifier2()->getIdentifier());
    targetFile.push('[');
    targetFile.pushString(propId->getIdentifier1()->getIdentifier());
    targetFile.push(']');
    if(!isMainFile)
     helper(propId->getIdentifier2(),propId->getIdentifier1());
  }

  if (isAtomic)
    targetFile.pushString(", ");
  else if (!asmt->hasPropCopy())
    targetFile.pushString(" = ");

  std::cout<< "------>BEG EXP"  << '\n';
  if (!asmt->hasPropCopy())
    generateExpr(asmt->getExpr(), isMainFile, isAtomic);
  //~ std::cout<< "------>END EXP"  << '\n';

  if (isAtomic)
    targetFile.pushstr_newL(");");
  else if (isResult)
    targetFile.pushstr_newL(";"); // No need "/2.0;" for directed graphs
  else if (!asmt->hasPropCopy())
    targetFile.pushstr_newL(";");

    if (asmt->lhs_isIdentifier())
  {
    if (asmt->hasPropCopy()){}
    else{
    char strBuffer[1024];
    Identifier *id = asmt->getId();
    Expression *exprAssigned = asmt->getExpr();
    int varFound =0;
    if(allGpuUsedVars.find(id->getIdentifier())!=allGpuUsedVars.end()){
      varFound = 1;
    }
    if(isForall1==0 && varFound==1){
          Type *type = id->getSymbolInfo()->getType();
          cout<<"TYPE : "<<convertToCppType(type)<<endl;
          const char *inVarName = id->getIdentifier();
        targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
       targetFile.pushstr_newL("cudaSetDevice(i);");
       targetFile.pushstr_newL("//printed here\n");
      sprintf(strBuffer, "initKernel<%s> <<<1,1>>>(1,d_%s[i],(%s)%s",convertToCppType(type), inVarName,convertToCppType(type),inVarName);
      targetFile.pushString(strBuffer);
      // generateExpr(exprAssigned,isMainFile); // asssuming int/float const literal // OUTPUTS INIT VALUE
      targetFile.pushstr_newL(");");
      targetFile.pushstr_newL("}");
      targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
      targetFile.pushstr_newL("cudaSetDevice(i);");
      targetFile.pushstr_newL("cudaDeviceSynchronize();");
      targetFile.pushstr_newL("}");        
      
      }
    }
  }
    
}
void dsl_cpp_generator::generateDeviceAssignmentStmt(assignment *asmt,
                                                     bool isMainFile)
{

  dslCodePad &targetFile = isMainFile ? main : header;
  bool isDevice = false;
  std::cout << "\t"<<asmt->lhs_isIdentifier()<<"\tASST SADJLAKD\n";
  char strBuffer[300];
  if (asmt->lhs_isIdentifier())
  {

    Identifier *id = asmt->getId();
    targetFile.pushString("//hi\n");
//     targetFile.pushString(id->getIdentifier());
       generate_exprIdentifier(id,isMainFile);
  }
  else if (asmt->lhs_isProp()) // the check for node and edge property to be
                               // carried out.
  {
    targetFile.pushString("//hi2\n");
    PropAccess *propId = asmt->getPropId();
    helper(propId->getIdentifier2(),propId->getIdentifier1());
    // if (asmt->isDeviceAssignment())
    if(isMainFile)
    {
      isDevice = true;
      //~ src.dist = 0; ===>  initIndex<int><<<1,1>>>(V,d_dist,src, 0);
      //                                  1              2     3   4
      Type *typeB = propId->getIdentifier2()->getSymbolInfo()->getType()->getInnerTargetType();
      //~ Type *typeA = propId->getIdentifier1()->getSymbolInfo()->getType();

      //~ targetFile.pushstr_newL(convertToCppType(typeB));
      //~ targetFile.pushstr_newL(convertToCppType(typeA));

      const char *varType = convertToCppType(typeB); // DONE: get the type from id
      int isForall = 0;
      ASTNode* tempstmt = asmt;
      while(tempstmt){
      if(tempstmt->getTypeofNode()==18){
        isForall = 1;
        break;
      }
      tempstmt = tempstmt->getParent();
    }
      // if(isForall){
      //   sprintf(strBuffer,"d_%s[%s] = ",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
      //   targetFile.pushString(strBuffer);
      //   helper(propId->getIdentifier2(),propId->getIdentifier1());
      // }
      // else{
        sprintf(strBuffer,"h_%s[%s]=",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
        targetFile.pushString(strBuffer);
        generateExpr(asmt->getExpr(), isMainFile);
        targetFile.pushstr_newL(";");
        sprintf(strBuffer,"for(int i=0;i<devicecount;i++)");
        targetFile.pushString(strBuffer);
        targetFile.pushstr_newL("{");
        targetFile.pushstr_newL("cudaSetDevice(i);");
        sprintf(strBuffer, "initIndex<%s><<<1,1>>>(V,d_%s[i],%s,(%s)",
                varType,
                propId->getIdentifier2()->getIdentifier(),
                propId->getIdentifier1()->getIdentifier(),
                varType);
        targetFile.pushString(strBuffer);  
      // }
      std::cout << "\tDEVICE ASST" << '\n';

      //~ Type *typeB = propId->getIdentifier2()->getSymbolInfo()->getType()->getInnerTargetType();
      //~ Type *typeA = propId->getIdentifier1()->getSymbolInfo()->getType();

      //~ targetFile.pushstr_newL(convertToCppType(typeB));
      //~ targetFile.pushstr_newL(convertToCppType(typeA));

      //~ targetFile.pushString("initIndex<<<1,1>>>(V,");

      //~ targetFile.push('(');

      //~ targetFile.pushString("d_"); /// IMPORTANT
      //~ targetFile.pushString(propId->getIdentifier2()->getIdentifier());
      //~ targetFile.pushString(", ");
      //~ targetFile.pushString(propId->getIdentifier1()->getIdentifier());
      //~ convertToCppType(propId->getIdentifier1()->getSymbolInfo()->getType());
      
    }
    else
    {
      targetFile.pushString("d_"); /// IMPORTANT
      targetFile.pushString(propId->getIdentifier2()->getIdentifier());
      targetFile.push('[');
      targetFile.pushString(propId->getIdentifier1()->getIdentifier());
      targetFile.push(']');
    }
  }

  if (!isDevice)
    //~ targetFile.pushString(",");
    //~ else
    targetFile.pushString(" = ");

  generateExpr(asmt->getExpr(), isMainFile);

  if (isMainFile){
    // targetFile.pushstr_newL("hi");
    targetFile.pushstr_newL("); //InitIndexDevice");
    targetFile.pushstr_newL("}");
    targetFile.pushstr_newL("for(int i=0;i<devicecount;i+=1){");
    targetFile.pushstr_newL("cudaSetDevice(i);");
    targetFile.pushstr_newL("cudaDeviceSynchronize();");
    targetFile.pushstr_newL("}");
  }
  else{
    targetFile.pushstr_newL("; //InitIndex");
    
    
  }
}

int cnt = 0;
void dsl_cpp_generator::generateProcCall(proc_callStmt *proc_callStmt,
                                         bool isMainFile)
{
  //~ dslCodePad& targetFile = isMainFile ? main : header;
  // cout<<"INSIDE PROCCALL OF GENERATION"<<"\n";
  proc_callExpr *procedure = proc_callStmt->getProcCallExpr();
  // cout<<"FUNCTION NAME"<<procedure->getMethodId()->getIdentifier();
  string methodID(procedure->getMethodId()->getIdentifier());
  string IDCoded("attachNodeProperty");
  int x = methodID.compare(IDCoded);




  if (x == 0)
  {
    //~ char strBuffer[1024];
    list<argument *> argList = procedure->getArgList();
    list<argument *>::iterator itr;

    for (itr = argList.begin(); itr != argList.end(); itr++)
    {
      assignment *assign = (*itr)->getAssignExpr();
      
        char strBuffer[1024];
        generateInitkernel1(assign, isMainFile);
        Identifier *iden = assign->getId();
        Type* type = iden->getSymbolInfo()->getType();
        main.pushstr_newL("for(int i=0;i<devicecount;i++){");
        main.pushstr_newL("cudaSetDevice(i);");
        sprintf(strBuffer,"cudaMemcpyAsync(h_%s+h_vertex_partition[i],d_%s[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(%s),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
        main.pushstr_newL(strBuffer);    
        main.pushstr_newL("}");    
        main.pushstr_newL("for(int i=0;i<devicecount;i+=1){");
        main.pushstr_newL("cudaSetDevice(i);");
        main.pushstr_newL("cudaDeviceSynchronize();");
        main.pushstr_newL("}");
    }
  }

  string IDCoded1("attachEdgeProperty");
  x = methodID.compare(IDCoded1);
  
  if(x == 0){
    list<argument*> argList = procedure->getArgList();
    list<argument*>::iterator itr;
    
    for (itr = argList.begin(); itr != argList.end(); itr++) {
      assignment* assign = (*itr)->getAssignExpr();
      bool isPropEdge = true;
      char strBuffer[1024];
      Identifier *iden = assign->getId();
      Type* type = iden->getSymbolInfo()->getType();
      generateInitkernel1(assign, isMainFile, isPropEdge);
      sprintf(strBuffer,"cudaMemcpyAsync(h_%s,d_%s[0],E*sizeof(%s),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
      main.pushstr_newL(strBuffer);    
      main.pushstr_newL("cudaDeviceSynchronize();");
    }
  }

  cout<<methodID<<endl;
  string IDCoded2("randSP");
  x = methodID.compare(IDCoded2);
  if(x==0){
    cnt+=1;
    char strBuffer[1024];
    list<argument *> argList = procedure->getArgList();
    list<argument *>::iterator itr;
    int cnt1 = 0;
    for (itr = argList.begin(); itr != argList.end(); itr++)
    {
      cnt1+=1;
      main.pushString("int x");
      main.pushString(to_string(cnt).c_str());
      main.pushString(to_string(cnt1).c_str());
      main.pushstr_newL("=rand();");
    }
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("curandGenerator_t gen;");
    main.pushstr_newL("curandCreateGenerator(&gen,CURAND_RNG_PSEUDO_PHILOX4_32_10);");
    cnt1 =0;
    for (itr = argList.begin(); itr != argList.end(); itr++)
    {
      cnt1+=1;
      Expression *expr = (*itr)->getExpr();
      Identifier *iden = expr->getId();
      main.pushString("curandSetPseudoRandomGeneratorSeed(gen,x");
      main.pushString(to_string(cnt).c_str());
      main.pushString(to_string(cnt1).c_str());
      main.pushstr_newL(");");
      sprintf(strBuffer,"curandGenerate(gen,(unsigned int*)d_%s[i],(V+1));",iden->getIdentifier());
      main.pushstr_newL(strBuffer);
        
    }
    main.pushstr_newL("}");
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("cudaDeviceSynchronize();");
    main.pushstr_newL("}");
  }

  string IDCoded3("combineRandom");
  x = methodID.compare(IDCoded3);
  if(x==0){
    list<argument *> argList = procedure->getArgList();
    list<argument *>::iterator itr;
    char strBuffer[1024];
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushString("combineRandom<<<numBlocks,numThreads>>>(V");
    for (itr = argList.begin(); itr != argList.end(); itr++)
    {

      Expression *expr = (*itr)->getExpr();
      Identifier *iden = expr->getId();  
      sprintf(strBuffer,",d_%s[i]",iden->getIdentifier());
      main.pushString(strBuffer);
    }
    main.pushstr_newL(");");
    main.pushstr_newL("}");
    main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("cudaDeviceSynchronize();");
    main.pushstr_newL("}");
      
  }
  


}

int dsl_cpp_generator::curFuncCount()
{
  int count;
  if (curFuncType == GEN_FUNC)
    count = genFuncCount;

  else if (curFuncType == STATIC_FUNC)
    count = staticFuncCount;

  else if (curFuncType == INCREMENTAL_FUNC)
    count = inFuncCount;

  else
    count = decFuncCount;

  return count;
}

void dsl_cpp_generator::incFuncCount(int funcType)
{
  if (funcType == GEN_FUNC)
    genFuncCount++;
  else if (funcType == STATIC_FUNC)
    staticFuncCount++;
  else if (funcType == INCREMENTAL_FUNC)
    inFuncCount++;
  else
    decFuncCount++;
}

void dsl_cpp_generator::generatePropertyDefination(Type *type, char *Id,
                                                   bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;
  vector<Identifier *> graphIds = graphId[curFuncType][curFuncCount()];

  Type *targetType = type->getInnerTargetType();
  if (targetType->gettypeId() == TYPE_INT)
  {
    targetFile.pushString("=");
    targetFile.pushString(INTALLOCATION);
    targetFile.pushString("[");
    // printf("%d SIZE OF VECTOR",)
    // findTargetGraph(graphId,type);

    if (graphId.size() > 1)
    {
      cerr << "TargetGraph can't match";
    }
    else
    {
      Identifier *id = graphIds[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }
  if (targetType->gettypeId() == TYPE_BOOL)
  {
    targetFile.pushString("=");
    targetFile.pushString(BOOLALLOCATION);
    targetFile.pushString("[");
    // findTargetGraph(graphId,type);
    if (graphId.size() > 1)
    {
      cerr << "TargetGraph can't match";
    }
    else
    {
      Identifier *id = graphIds[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }

  if (targetType->gettypeId() == TYPE_FLOAT)
  {
    targetFile.pushString("=");
    targetFile.pushString(FLOATALLOCATION);
    targetFile.pushString("[");
    // findTargetGraph(graphId,type);
    if (graphId.size() > 1)
    {
      cerr << "TargetGraph can't match";
    }
    else
    {
      Identifier *id = graphIds[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }

  if (targetType->gettypeId() == TYPE_DOUBLE)
  {
    targetFile.pushString("=");
    targetFile.pushString(DOUBLEALLOCATION);
    targetFile.pushString("[");
    // findTargetGraph(graphId,type);
    if (graphId.size() > 1)
    {
      cerr << "TargetGraph can't match";
    }
    else
    {
      Identifier *id = graphIds[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }
}

void dsl_cpp_generator::getDefaultValueforTypes(int type)
{
  switch (type)
  {
  case TYPE_INT:
  case TYPE_LONG:
    main.pushstr_space("0");
    break;
  case TYPE_FLOAT:
  case TYPE_DOUBLE:
    main.pushstr_space("0.0");
    break;
  case TYPE_BOOL:
    main.pushstr_space("false");
  
  default:
    assert(false);
    return;
  }
}

void dsl_cpp_generator::generateForAll_header(forallStmt *forAll,
                                              bool isMainFile)
{
  //~ cout << "the value of the generateForAll_header = " << isMainFile;
  dslCodePad &targetFile = isMainFile ? main : header;
  targetFile.pushString(
      "unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);");
  /*
  if(forAll->get_reduceKeys().size()>0)
    {
      printf("INSIDE GENERATE FOR ALL FOR KEYS\n");

      set<int> reduce_Keys=forAll->get_reduceKeys();
      assert(reduce_Keys.size()==1);
      char strBuffer[1024];
      set<int>::iterator it;
      it=reduce_Keys.begin();
      list<Identifier*> op_List=forAll->get_reduceIds(*it);
      list<Identifier*>::iterator list_itr;
      main.space();
      sprintf(strBuffer,"reduction(%s : ",getOperatorString(*it));
      main.pushString(strBuffer);
      for(list_itr=op_List.begin();list_itr!=op_List.end();list_itr++)
      {
        Identifier* id=*list_itr;
        main.pushString(id->getIdentifier());
        if(std::next(list_itr)!=op_List.end())
         main.pushString(",");
      }
      main.pushString(")");



    }
    */
  targetFile.NewLine();

  // to accomodate the v with id which is needed in almost all the algorithms
  targetFile.pushString("unsigned int v =id;");
  targetFile.NewLine();
  targetFile.pushString("if (id < V)");
  targetFile.NewLine();
  targetFile.pushString("{");
}

bool dsl_cpp_generator::allGraphIteration(char *methodId)
{
  string methodString(methodId);

  return (methodString == "nodes" || methodString == "edges");
}

bool dsl_cpp_generator::neighbourIteration(char *methodId)
{
  string methodString(methodId);
  return (methodString == "neighbors" || methodString == "nodes_to");
}

bool dsl_cpp_generator::elementsIteration(char *extractId)
{
  string extractString(extractId);
  return (extractString == "elements");
}

void dsl_cpp_generator::generateForAllSignature(forallStmt *forAll, bool isMainFile)
{

  cout << "GenerateForAllSignature = " << isMainFile << endl;
  dslCodePad &targetFile = isMainFile ? main : header;

  char strBuffer[1024];
  Identifier *iterator = forAll->getIterator();
  if (forAll->isSourceProcCall())
  {
    //~ Identifier* sourceGraph = forAll->getSourceGraph();
    proc_callExpr *extractElemFunc = forAll->getExtractElementFunc();
    Identifier *iteratorMethodId = extractElemFunc->getMethodId();
    if (allGraphIteration(iteratorMethodId->getIdentifier()))
    {
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
    }
    else if (neighbourIteration(iteratorMethodId->getIdentifier()))
    {
      //~ // THIS SHOULD NOT BE EXECUTING FOR SIMPLE FOR LOOP BUT IT IS SO .
      //~ // COMMENTED OUT FOR NOW.
      //~ char* graphId=sourceGraph->getIdentifier();
      char *methodId = iteratorMethodId->getIdentifier();
      string s(methodId);
      if (s.compare("neighbors") == 0)
      {
        list<argument *> argList = extractElemFunc->getArgList();
        assert(argList.size() == 1);
        Identifier* nodeNbr=argList.front()->getExpr()->getId();
        sprintf(strBuffer,"for (%s %s = %s[%s]; %s < %s[%s+1]; %s++) { ","int","edge","d_meta",nodeNbr->getIdentifier(),"edge","d_meta",nodeNbr->getIdentifier(),"edge");
        //sprintf(strBuffer, "for (%s %s = %s[%s]; %s < %s[%s+1]; %s++) { // FOR NBR ITR ", "int", "edge", "d_meta", "v", "edge", "d_meta", "v", "edge");
        targetFile.pushstr_newL(strBuffer);
        //~ targetFile.pushString("{");
        sprintf(strBuffer, "%s %s = %s[%s];", "int", iterator->getIdentifier(), "d_data", "edge"); // needs to move the addition of
        targetFile.pushstr_newL(strBuffer);
      }
      if (s.compare("nodes_to") == 0) // for pageRank
      {
        list<argument *> argList = extractElemFunc->getArgList();
        assert(argList.size() == 1);
        Identifier *nodeNbr = argList.front()->getExpr()->getId();
        sprintf(strBuffer, "for (%s %s = %s[%s]; %s < %s[%s+1]; %s++){", "int", "edge", "d_rev_meta", nodeNbr->getIdentifier(), "edge", "d_rev_meta", nodeNbr->getIdentifier(), "edge");
        targetFile.pushstr_newL(strBuffer);
        sprintf(strBuffer, "%s %s = %s[%s] ;", "int", iterator->getIdentifier(), "d_src", "edge"); // needs to move the addition of
        targetFile.pushstr_newL(strBuffer);
      } // statement to  a different method.
    }
  }
  else if (forAll->isSourceField())
  {
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
  }
  else
  {
    //~ std::cout << "+++++++++++++++" << '\n';
    Identifier *sourceId = forAll->getSource();
    if (sourceId != NULL)
    {
      if (sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN)
      { // FOR SET
        //~ std::cout << "+++++     ++++++++++" << '\n';
        main.pushstr_newL("//FOR SIGNATURE of SET - Assumes set for on .cu only");
        main.pushstr_newL("std::set<int>::iterator itr;");
        sprintf(strBuffer, "for(itr=%s.begin();itr!=%s.end();itr++) ", sourceId->getIdentifier(), sourceId->getIdentifier());
        main.pushstr_newL(strBuffer);
      }
    }
  }
}

blockStatement *dsl_cpp_generator::includeIfToBlock(forallStmt *forAll)
{
  Expression *filterExpr = forAll->getfilterExpr();
  //~ statement* body = forAll->getBody();
  Expression *modifiedFilterExpr = filterExpr;
  if (filterExpr->getExpressionFamily() == EXPR_RELATIONAL)
  {
    Expression *expr1 = filterExpr->getLeft();
    Expression *expr2 = filterExpr->getRight();
    if (expr1->isIdentifierExpr())
    {
      /*if it is a nodeproperty, the filter is on the nodes that are iterated on
       One more check can be applied to check if the iterating type is a neigbor
       iteration or allgraph iterations.
      */
      if (expr1->getId()->getSymbolInfo() != NULL)
      {
        if (expr1->getId()->getSymbolInfo()->getType()->isPropNodeType())
        {
          Identifier *iterNode = forAll->getIterator();
          Identifier *nodeProp = expr1->getId();
          PropAccess *propIdNode = (PropAccess *)Util::createPropIdNode(iterNode, nodeProp);
          Expression *propIdExpr = Expression::nodeForPropAccess(propIdNode);
          modifiedFilterExpr = (Expression *)Util::createNodeForRelationalExpr(propIdExpr, expr2, filterExpr->getOperatorType());
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
  ifStmt *ifNode = (ifStmt *)Util::createNodeForIfStmt(modifiedFilterExpr,
                                                       forAll->getBody(), NULL);
  blockStatement *newBlock = new blockStatement();
  newBlock->setTypeofNode(NODE_BLOCKSTMT);
  newBlock->addStmtToBlock(ifNode);
  newBlock->setParent(forAll);
  return newBlock;
}

void dsl_cpp_generator::generateCallList(list<formalParam *> paramList, dslCodePad &targetFile)
{
  int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = paramList.size();
  list<formalParam *>::iterator itr;
  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {
    arg_currNo++;
    argumentTotal--;

    Type *type = (*itr)->getType();
    /* targetFile.pushString(convertToCppType(type));
     if(type->isPropType())
     {
         targetFile.pushString("* ");
     }
     else
     {*/
    // targetFile.pushString(" ");
    // targetFile.space();
    //}
    if (type->isPropType())
    {
      targetFile.pushString("d_");
    }

    targetFile.pushString(/*createParamName(*/ (*itr)->getIdentifier()->getIdentifier());

    if (argumentTotal > 0)
      targetFile.pushString(",");

    if (arg_currNo == maximum_arginline)
    {
      targetFile.NewLine();
      arg_currNo = 0;
    }
  }
}

void dsl_cpp_generator::generateParamList(list<formalParam *> paramList, dslCodePad &targetFile)
{
  int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = paramList.size();
  list<formalParam *>::iterator itr;
  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {
    arg_currNo++;
    argumentTotal--;

    Type *type = (*itr)->getType();

    /*if(type->isPropType())
    {
        targetFile.pushString("* ");
    }
    else
    {*/

    // targetFile.space();
    //}
    //~ if(!type->isGraphType()) {
    targetFile.pushString(convertToCppType(type));
    //~ }

    //~ if(type->isGraphType()){
    //~ targetFile.pushString("int* d_meta, int* d_data, int* d_weight");
    //~ }
    targetFile.pushString(" ");

    if (type->isPropType())
    {
      targetFile.pushString("d_");
    }

    //~ if(!type->isGraphType()) // TMP FIX
    targetFile.pushString(/*createParamName(*/ (*itr)->getIdentifier()->getIdentifier());

    if (argumentTotal > 0)
      targetFile.pushString(", ");

    if (arg_currNo == maximum_arginline)
    {
      targetFile.NewLine();
      arg_currNo = 0;
    }
    // if(argumentTotal==0)
  }
}
int cnt_kernels = 1;
void dsl_cpp_generator ::addCudaKernel(forallStmt *forAll)
{
  Identifier* iterator = forAll->getIterator();
  const char *loopVar = iterator->getIdentifier();
  declInForAll[iterator->getIdentifier()]=2;
  globalLoopVar = iterator->getIdentifier();
  char strBuffer[1024];

  //~ Function* currentFunc = getCurrentFunc();
  multigpu::usedVariables usedVars = multigpu::getVarsForAll(forAll);
  list<Identifier *> vars = usedVars.getVariables();

  header.pushString("__global__ void ");
  header.pushString(getCurrentFunc()->getIdentifier()->getIdentifier());
  header.pushString("_kernel");
  header.pushString(to_string(cnt_kernels).c_str());
  cnt_kernels++;
  header.pushString("(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta");
  /*if(currentFunc->getParamList().size()!=0)
    {
      header.pushString(" ,");
      generateParamList(currentFunc->getParamList(), header);
    }*/
  for (Identifier *iden : vars)
  {
    Type *type = iden->getSymbolInfo()->getType();
    if (type->isPropType())
    {
      char strBuffer[1024];
      sprintf(strBuffer, ",%s d_%s", convertToCppType(type), iden->getIdentifier());
      header.pushString(/*createParamName(*/ strBuffer);
      if (iden->getSymbolInfo()->getId()->get_fp_association()){
        sprintf(strBuffer, ",%s d_%s_next", convertToCppType(type), iden->getIdentifier()); 
        header.pushString(/*createParamName(*/ strBuffer); 
      }
       
    }
     if(type->isPrimitiveType()){
         char strBuffer[1024];
      sprintf(strBuffer, ",%s* d_%s", convertToCppType(type), iden->getIdentifier());
      header.pushString(/*createParamName(*/ strBuffer);
      if (iden->getSymbolInfo()->getId()->get_fp_association()){
        sprintf(strBuffer, ",%s* d_%s_next", convertToCppType(type), iden->getIdentifier()); 
        header.pushString(/*createParamName(*/ strBuffer); 
      }
      }
    

  }
  if(FIXEDPOINT){
      bool print = true;
    for(Identifier *iden : vars){
      Type *type = iden->getSymbolInfo()->getType();
      //sprintf(strBuffer,"\n%s %s\n",fixed_pt_var,iden->getIdentifier());
      //header.pushstr_newL(strBuffer);
      if(strcmp(fixed_pt_var,iden->getIdentifier())==0 and (type->isPropType() or type->isPrimitiveType())){
        print = false;
        break;
      }
    }
    if(print){
      sprintf(strBuffer, ",%s* d_%s",fixed_pt_type,fixed_pt_var);  
      header.pushString(/*createParamName(*/ strBuffer); 
    }
  }
  
  header.pushstr_newL("){ // BEGIN KER FUN via ADDKERNEL");

  sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
  // header.pushstr_newL("float num_nodes  = V;");
  header.pushstr_newL(strBuffer);
  header.pushstr_newL("int num_vertices = end-start;");
  sprintf(strBuffer, "if( %s < num_vertices) {",loopVar) ;
  header.pushstr_newL(strBuffer);
  sprintf(strBuffer, "%s+=start;",loopVar) ;
  header.pushstr_newL(strBuffer);
  

  if (forAll->hasFilterExpr())
  {
    blockStatement *changedBody = includeIfToBlock(forAll);
    cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
    forAll->setBody(changedBody);
    // cout<<"FORALL BODY
    // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
  }

  statement *body = forAll->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement *block = (blockStatement *)body;
  list<statement *> statementList = block->returnStatements();


  for (statement *stmt : statementList)
  {
    generateStatement(stmt, false); // false. All these stmts should be inside kernel
                                    //~ if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
  }
  header.pushstr_newL("}");

  header.pushstr_newL("} // end KER FUNC");
}

void dsl_cpp_generator::generateForAll(forallStmt *forAll, bool isMainFile)
{
   Identifier* iterator = forAll->getIterator();
  declInForAll[iterator->getIdentifier()]=2;
  dslCodePad &targetFile = isMainFile ? main : header;
  //~ cout << "inside the forall the value of ismainfile =" << isMainFile;
  proc_callExpr *extractElemFunc = forAll->getExtractElementFunc();
  PropAccess *sourceField = forAll->getPropSource();
  Identifier *sourceId = forAll->getSource();
  // Identifier* extractId;
  Identifier *collectionId;
  if (sourceField != NULL)
  {
    collectionId = sourceField->getIdentifier1();
  }
  if (sourceId != NULL)
  {
    collectionId = sourceId;
  }
  Identifier *iteratorMethodId;
  if (extractElemFunc != NULL)
    iteratorMethodId = extractElemFunc->getMethodId();
  statement *body = forAll->getBody();
  char strBuffer[1024];
  if (forAll->isForall())
  { // IS FORALL

    /*
    if (forAll->hasFilterExpr()) {
      Expression* filterExpr = forAll->getfilterExpr();
      Expression* lhs = filterExpr->getLeft();
      Identifier* filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
      TableEntry* tableEntry = filterId != NULL ? filterId->getSymbolInfo() : NULL;


      if (tableEntry != NULL && tableEntry->getId()->get_fp_association()) {
        generateForAll_header(forAll, false);
      } else {
        generateForAll_header(forAll, false);
      }

    } else {
      //~ std::cout << "\t\tFFOOOOOORRRRR" << '\n';
      generateForAll_header(forAll, isMainFile);
    }
    */
    printf("Entered here for forall \n");

    multigpu::usedVariables usedVars = multigpu::getVarsForAll(forAll);
    list<Identifier *> vars = usedVars.getVariables();

    for (Identifier *iden : vars)
    {
      Type *type = iden->getSymbolInfo()->getType();

      if (type->isPrimitiveType())
        generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
    }
    main.pushString("for(int i=0;i<devicecount;i++)");
    main.NewLine();
    main.pushString("{");
    main.NewLine();
    main.pushString("cudaSetDevice(i);");
    main.NewLine();
    main.pushString(getCurrentFunc()->getIdentifier()->getIdentifier());
    main.pushString("_kernel");
    main.pushString(to_string(cnt_kernels).c_str());
    main.pushString("<<<");
    main.pushString("numBlocksKernel, threadsPerBlock");
    main.pushString(">>>");
    main.push('(');
    main.pushString("h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i]");
    //  if(currentFunc->getParamList().size()!=0)
    // main.pushString(",");
    for (Identifier *iden : vars)
    {
      Type *type = iden->getSymbolInfo()->getType();
      cout << "CHECK IT HERE" << iden->getIdentifier() << endl;
      cout<<"___"<<MODIFIED<<"__"<<iden->getIdentifier()<<endl;
      if(strcmp(MODIFIED,iden->getIdentifier())==0){
        modified_used = true;
      }

      if (type->isPropType() or type->isPrimitiveType())  
      {
        main.pushString(",");
        main.pushString("d_");
        main.pushString(/*createParamName(*/ iden->getIdentifier());
        main.pushString("[i]");

        if (iden->getSymbolInfo()->getId()->get_fp_association())
        {
          sprintf(strBuffer, ",d_%s_next[i]", iden->getIdentifier());
          main.pushString(/*createParamName(*/ strBuffer);
        }
      }

    }
    printf("for all\n");
    cout<<"FIXEDPOINT"<<endl;
    if(FIXEDPOINT){
      bool print = true;
    for(Identifier *iden : vars){

      Type *type = iden->getSymbolInfo()->getType();
      if(strcmp(fixed_pt_var,iden->getIdentifier())==0 and (type->isPropType() or type->isPrimitiveType())){
        print = false;
        break;
      }
    }
    if(print){
      sprintf(strBuffer, ",d_%s[i]", fixed_pt_var);  
      main.pushString(/*createParamName(*/ strBuffer); 
    }
    }
    main.pushString(")");
    main.push(';');
    main.NewLine();


    main.pushstr_newL("}");
    
    main.NewLine();

    main.pushString("for(int i=0;i<devicecount;i++)");
    main.NewLine();
    main.pushString("{");
    main.NewLine();
    main.pushString("cudaSetDevice(i);");
    main.NewLine();
    main.pushString("cudaDeviceSynchronize();");
    main.NewLine();
    main.pushString("}");

    main.NewLine();

    // for (Identifier *iden : vars)
    // {
    //     Type *type = iden->getSymbolInfo()->getType();
    //    if (type->isPrimitiveType())  
    //   {
        
    //     main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    //     main.pushstr_newL("cudaSetDevice(i);");
    //     sprintf(strBuffer,"cudaMemcpyAsync(h_%s[i],d_%s[i],sizeof(%s),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type));
    //     main.pushstr_newL(strBuffer);
    //     main.pushstr_newL("}");
        
    

    //   }
    //   else if(type->isPropType()){
    //     if(iden->getSymbolInfo()->getId()->get_fp_association()){
    //     main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    //     main.pushstr_newL("cudaSetDevice(i);");
    //     sprintf(strBuffer,"cudaMemcpyAsync(h_%s[i],d_%s_next[i],sizeof(%s)*(V+1),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
    //     main.pushstr_newL(strBuffer);
    //     main.pushstr_newL("}");
    //     }
    //     else{
    //     main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    //     main.pushstr_newL("cudaSetDevice(i);");
    //     sprintf(strBuffer,"cudaMemcpyAsync(h_%s[i],d_%s[i],sizeof(%s)*(V+1),cudaMemcpyDeviceToHost);",iden->getIdentifier(),iden->getIdentifier(),convertToCppType(type->getInnerTargetType()));
    //     main.pushstr_newL(strBuffer);
    //     main.pushstr_newL("}");
    //     }
    //   }
    // }
    // main.pushstr_newL("for(int i=0;i<devicecount;i++){");
    // main.pushstr_newL("cudaSetDevice(i);");
    // main.pushstr_newL("cudaDeviceSynchronize();");
    // main.pushstr_newL("}"); 

   
    addCudaKernel(forAll);
    globalLoopVar = NULL;
  }

  else
  { // IS FOR

    generateForAllSignature(forAll, false); // FOR LINE

    if (forAll->hasFilterExpr())
    {
      blockStatement *changedBody = includeIfToBlock(forAll);
      cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
      forAll->setBody(changedBody);
      // cout<<"FORALL BODY
      // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
    }

    if (extractElemFunc != NULL)
    {
      if (neighbourIteration(iteratorMethodId->getIdentifier()))
      { // todo forall neigbour iterion
        cout << "\t ITERATE Neighbour \n";

        //~ char* tmpStr = forAll->getSource()->getIdentifier();
        char *wItr = forAll->getIterator()->getIdentifier(); // w iterator
        std::cout << "src:" << wItr << '\n';
        //~ char* gVar = forAll->getSourceGraph()->getIdentifier();     //g variable
        //~ std::cout<< "G:" << gVar << '\n';
        char *nbrVar;
        // sprintf(strBuffer, "parent is %d", forAll->getParent()->getTypeofNode()) ; 
        // targetFile.pushstr_newL(strBuffer) ; 
        if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
        {
          list<argument *> argList = extractElemFunc->getArgList();
          assert(argList.size() == 1);
          Identifier *nodeNbr = argList.front()->getExpr()->getId();
          //~ targetFile.pushstr_newL("FOR begin | nbr iterate"); // ITERATE BFS
          nbrVar = nodeNbr->getIdentifier();
          //~ std::cout<< "nbr?:" <<  nbrVar<< '\n';

          // sprintf(strBuffer, "here is %s \n", nbrVar) ; 
          // targetFile.pushstr_newL(strBuffer) ; 
          //~ sprintf(strBuffer, "for(unsigned i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
          //~ targetFile.pushstr_newL(strBuffer);

          //~ // HAS ALL THE STMTS IN FOR
          //~ targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.

          //~ sprintf(strBuffer, "unsigned %s = d_data[i];", wItr);
          //~ targetFile.pushstr_newL(strBuffer);

          sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source) {", wItr);
          targetFile.pushstr_newL(strBuffer);
          sprintf(strBuffer, "d_level[%s] = *d_hops_from_source+1;", nbrVar);

          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("*d_finished = false;");

          // sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source + 1) {", wItr);
          // targetFile.pushstr_newL(strBuffer);

          generateBlock((blockStatement *)forAll->getBody(), false, false);

          // targetFile.pushstr_newL("}");

          targetFile.pushstr_newL("}");
          targetFile.pushstr_newL("}");

          // HAS ALL THE STMTS IN FOR
          //~ targetFile.pushstr_newL("{");
          //~ generateStatement(forAll->getBody(), false); //false. All these stmts should be inside kernel
          //~ targetFile.pushstr_newL("}");

          //~ sprintf(strBuffer, "if(bfsDist[%s]==bfsDist[%s]+1)",
          //~ forAll->getIterator()->getIdentifier(),
          //~ nodeNbr->getIdentifier());
          //~ targetFile.pushstr_newL(strBuffer);
          //~ targetFile.pushstr_newL("{");
        }

        /* This can be merged with above condition through or operator but
           separating both now, for any possible individual construct updation.*/

        else if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
        { // ITERATE REV BFS
          char strBuffer[1024];
          list<argument *> argList = extractElemFunc->getArgList();
          assert(argList.size() == 1);
          Identifier *nodeNbr = argList.front()->getExpr()->getId();
          nbrVar = nodeNbr->getIdentifier();
          std::cout << "V?:" << nbrVar << '\n';
          //~ sprintf(strBuffer, "for(int i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
          //~ targetFile.pushstr_newL(strBuffer);

          // HAS ALL THE STMTS IN FOR
          //~ targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.
          //~ sprintf(strBuffer, "int %s = d_data[i];", wItr);
          //~ targetFile.pushstr_newL(strBuffer);
          sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source) {", wItr);
          targetFile.pushstr_newL(strBuffer);
          generateBlock((blockStatement *)forAll->getBody(), false, false);
          targetFile.pushstr_newL("} // end IF  ");
          targetFile.pushstr_newL("} // end FOR");
          // targetFile.pushstr_newL("grid.sync();/ // Device-level syncs across all grids. Ref:https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#grid-synchronization-cg ");

          //~ targetFile.pushstr_newL("FOR begin | nbr iterate");
          //~ if(forAll->isForall() && forAll->hasFilterExpr()){
          //~ std::cout<< "HAS FILTER?" << '\n';
          //~ }
          //~ Identifier* nodeNbr = argList.front()->getExpr()->getId();
          //~ sprintf(strBuffer, "if(bfsDist[%s]==bfsDist[%s]+1)",
          //~ forAll->getIterator()->getIdentifier(),
          //~ nodeNbr->getIdentifier());
          //~ targetFile.pushstr_newL(strBuffer);
          //~ targetFile.pushstr_newL("{");
        }
        else
        {
          //~ std::cout<< "FOR BODY BEGIN" << '\n';
          //~ targetFile.pushstr_newL("{ // FOR BEGIN ITR BEGIN");
          generateStatement(forAll->getBody(), isMainFile);
          targetFile.pushstr_newL("} //  end FOR NBR ITR. TMP FIX!");
          std::cout << "FOR BODY END" << '\n';
        }

        //~ if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS ||
        //~ forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
        //~ targetFile.pushstr_newL("}");
        //~ targetFile.pushstr_newL("}");
      }
      else
      {
        printf("FOR NORML");
        generateStatement(forAll->getBody(), false);
      }

      if (forAll->isForall() && forAll->hasFilterExpr())
      {
        Expression *filterExpr = forAll->getfilterExpr();
        generatefixedpt_filter(filterExpr, false);
      }
    }
    else
    {
      if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN)
      { // FOR SET
        if (body->getTypeofNode() == NODE_BLOCKSTMT)
        {
          targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.
          //~ targetFile.pushstr_newL("//HERE");
          printf("FOR");
          sprintf(strBuffer, "int %s = *itr;", forAll->getIterator()->getIdentifier());
          targetFile.pushstr_newL(strBuffer);
          generateBlock((blockStatement *)body, false); // FOR BODY for
          targetFile.pushstr_newL("}");
        }
        else
          generateStatement(forAll->getBody(), false);
      }
      else
      {
        //~ cout << iteratorMethodId->getIdentifier() << "\n";
        generateStatement(forAll->getBody(), false);
      }

      if (forAll->isForall() && forAll->hasFilterExpr())
      {
        Expression *filterExpr = forAll->getfilterExpr();
        generatefixedpt_filter(filterExpr, false);
      }
    }
  }
}

void dsl_cpp_generator::generatefixedpt_filter(Expression *filterExpr,
                                               bool isMainFile)
{
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  Expression *lhs = filterExpr->getLeft();
  //~ char strBuffer[1024];
  if (lhs->isIdentifierExpr())
  {
    //~ Identifier* filterId = lhs->getId();
    //~ TableEntry* tableEntry = filterId->getSymbolInfo();
    /*
    if(tableEntry->getId()->get_fp_association())
      {
          targetFile.pushstr_newL("#pragma omp for");
          sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
    ","int","v","v",graphId[0]->getIdentifier(),"num_nodes","v");
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_space("{");
          sprintf(strBuffer,"%s[%s] =  %s_nxt[%s]
    ;",filterId->getIdentifier(),"v",filterId->getIdentifier(),"v");
          targetFile.pushstr_newL(strBuffer);
          Expression* initializer =
    filterId->getSymbolInfo()->getId()->get_assignedExpr();
          assert(initializer->isBooleanLiteral());
          sprintf(strBuffer,"%s_nxt[%s] = %s
    ;",filterId->getIdentifier(),"v",initializer->getBooleanConstant()?"true":"false");
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("}");
          targetFile.pushstr_newL("}");

       }
       */
  }
}

void dsl_cpp_generator::castIfRequired(Type *type, Identifier *methodID,
                                       dslCodePad &main)
{
  /* This needs to be made generalized, extended for all predefined function,
     made available by the DSL*/
  string predefinedFunc("num_nodes");
  if (predefinedFunc.compare(methodID->getIdentifier()) == 0)
  {
    if (type->gettypeId() != TYPE_INT)
    {
      char strBuffer[1024];
      sprintf(strBuffer, "(%s)", convertToCppType(type));
      main.pushString(strBuffer);
    }
  }
}

void dsl_cpp_generator::generateVariableDecl(declaration *declStmt,
                                             bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;
    ASTNode* tempstmt = declStmt;
    int isForall = 1;
    while(tempstmt){
      if(tempstmt->getTypeofNode()==18){
        isForall = 2;
        break;
      }
      tempstmt = tempstmt->getParent();
    }
  Identifier* id = declStmt->getdeclId();
  declInForAll[id->getIdentifier()] = isForall;
  Type *type = declStmt->getType();
  char *ch = "V+1";
  if(type->isPropEdgeType()){
    ch="E";
  }
  if (type->isPropType())
  {
    if (type->getInnerTargetType()->isPrimitiveType())
    {
      char strBuffer[1024];
      Type *innerType = type->getInnerTargetType();
      main.pushString(convertToCppType(innerType)); // convertToCppType need to be modified.
      main.pushString("*");
      main.space();
      main.pushString("h_");
      main.pushString(declStmt->getdeclId()->getIdentifier());
      main.pushstr_newL(";");
      sprintf(strBuffer,"h_%s=(%s*)malloc(sizeof(%s)*(%s));",declStmt->getdeclId()->getIdentifier(),convertToCppType(innerType),convertToCppType(innerType),ch);
      main.pushstr_newL(strBuffer);
      main.pushString(convertToCppType(innerType)); // convertToCppType need to be modified.
      main.pushString("**");
      main.space();
      main.pushString("d_");
      main.pushString(declStmt->getdeclId()->getIdentifier());
      main.pushstr_newL(";");
      generateCudaMalloc(type, declStmt->getdeclId()->getIdentifier());
      if(push_map[string(declStmt->getdeclId()->getIdentifier())]){
        sprintf(strBuffer,"%s h_%s_temp1 = (%s)malloc((%s)*(devicecount)*sizeof(%s));",convertToCppType(type),id->getIdentifier(),convertToCppType(type),ch,convertToCppType(type->getInnerTargetType()));
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("cudaSetDevice(0);");
        sprintf(strBuffer,"%s d_%s_temp1;",convertToCppType(type),id->getIdentifier());
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer,"cudaMalloc(&d_%s_temp1,(%s)*(devicecount)*sizeof(%s));",id->getIdentifier(),ch,convertToCppType(type->getInnerTargetType()));
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer,"%s d_%s_temp2;",convertToCppType(type),id->getIdentifier());
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer,"cudaMalloc(&d_%s_temp2,(%s)*(devicecount)*sizeof(%s));",id->getIdentifier(),ch,convertToCppType(type->getInnerTargetType()));
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("\n");  
      }
    }
  }

  // needs to handle carefully for PR code generation
  else if (type->isPrimitiveType())
  {
    char strBuffer[1024];
    // targetFile.pushstr_space(convertToCppType(type));
    const char *varType = convertToCppType(type);
    const char *varName = declStmt->getdeclId()->getIdentifier();
    cout << "varT:" << varType << endl;
    cout << "varN:" << varName << endl;

    if (isMainFile == true)
    { // to fix the PageRank we are doing this
      // sprintf(strBuffer, "__device__ %s %s ", varType, varName);
      // header.pushString(strBuffer);
    }
    sprintf(strBuffer, "%s %s", varType, varName);
    targetFile.pushString(strBuffer);
   
    if (declStmt->isInitialized())
    {

      targetFile.pushString(" = ");
      if (declStmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL)
      {
        proc_callExpr *pExpr = (proc_callExpr *)declStmt->getExpressionAssigned();
        Identifier *methodId = pExpr->getMethodId();
        castIfRequired(type, methodId, main);
      }
      generateExpr(declStmt->getExpressionAssigned(), isMainFile); // PRINTS RHS? YES
      
       
    }

    
    targetFile.pushstr_space("; // asst in .cu");
    targetFile.NewLine();
    int varFound = 0;
    if(allGpuUsedVars.find(varName)!=allGpuUsedVars.end()){
        varFound = 1; 
    }
    main.pushstr_newL("//fixed_pt_var");
    if(strcmp(varName,fixed_pt_var)==0){
      varFound = 1;
    }
    if(isMainFile==1&&varFound==1){
    sprintf(strBuffer, "%s** h_%s;", varType, varName);
    targetFile.pushstr_newL(strBuffer);
    sprintf(strBuffer,"h_%s = (%s**)malloc(sizeof(%s*)*(devicecount+1));",varName,varType,varType);
    targetFile.pushstr_newL(strBuffer);
    targetFile.pushstr_newL("for(int i=0;i<=devicecount;i+=1){");
    sprintf(strBuffer,"h_%s[i] = (%s*)malloc(sizeof(%s));",varName,varType,varType);
    targetFile.pushstr_newL(strBuffer);
    targetFile.pushstr_newL("}");
    targetFile.NewLine() ; 
    sprintf(strBuffer,"%s** d_%s;",varType,varName);
    targetFile.   pushstr_newL(strBuffer);
    sprintf(strBuffer, "d_%s = (%s**)malloc(sizeof(%s*)*devicecount);", varName,varType,varType);
    targetFile.pushstr_newL(strBuffer);
    targetFile.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
    targetFile.pushstr_newL("cudaSetDevice(i);");
    sprintf(strBuffer, "cudaMalloc(&d_%s[i],sizeof(%s));", varName, varType);
    targetFile.pushstr_newL(strBuffer);
    if(declStmt->isInitialized()){
    sprintf(strBuffer,"initKernel<%s> <<<1,1>>>(1,d_%s[i],",varType,varName); 
    targetFile.pushString(strBuffer);
    generateExpr(declStmt->getExpressionAssigned(), isMainFile); // PRINTS RHS? YES
    targetFile.pushstr_newL(");");
    }
    targetFile.pushstr_newL("}");
    targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
    targetFile.pushstr_newL("cudaSetDevice(i);");
    targetFile.pushstr_newL("cudaDeviceSynchronize();");
    targetFile.pushstr_newL("}");        
          
    targetFile.NewLine() ; 
    }
    main.NewLine();
  }

  else if (type->isNodeEdgeType())
  {
    cout<<"enterd"<<endl;
    targetFile.pushstr_space(convertToCppType(type));
    targetFile.pushString(declStmt->getdeclId()->getIdentifier());
    if (declStmt->isInitialized())
    {
      targetFile.pushString(" = ");
      generateExpr(declStmt->getExpressionAssigned(), isMainFile);
      targetFile.pushstr_newL(";");
    }
  }
}

void dsl_cpp_generator::generate_exprLiteral(Expression *expr,
                                             bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  char valBuffer[1024];

  int expr_valType = expr->getExpressionFamily();

  switch (expr_valType)
  {
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

void dsl_cpp_generator::generate_exprInfinity(Expression *expr,
                                              bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  char valBuffer[1024];
  if (expr->getTypeofExpr())
  {
    int typeClass = expr->getTypeofExpr();
    switch (typeClass)
    {
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
  }
  else

  {
    sprintf(valBuffer, "%s", expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
  }

  targetFile.pushString(valBuffer);
}

const char *dsl_cpp_generator::getOperatorString(int operatorId)
{
  switch (operatorId)
  {
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

void dsl_cpp_generator::generate_exprRelational(Expression *expr,
                                                bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  if (expr->hasEnclosedBrackets())
  {
    targetFile.pushString("(");
  }
  generateExpr(expr->getLeft(), isMainFile);

  targetFile.space();
  const char *operatorString = getOperatorString(expr->getOperatorType());
  targetFile.pushstr_space(operatorString);
  generateExpr(expr->getRight(), isMainFile);
  if (expr->hasEnclosedBrackets())
  {
    targetFile.pushString(")");
  }
}

void dsl_cpp_generator::generate_exprIdentifier(Identifier *id,
                                                bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;
  char v[10]="V";
  char e[10]="E";
  declInForAll[v]=2;
  declInForAll[e]=2;
  int isForall = 0;
  for(auto x : declInForAll){
    if(strcmp(x.first,id->getIdentifier())==0){
      isForall = x.second;
    }
  }
  if(isMainFile)
  targetFile.pushString(id->getIdentifier());
  else{
    if(isForall==2){
      targetFile.pushString(id->getIdentifier());
    }
    else{
      targetFile.pushString("d_");
      targetFile.pushString(id->getIdentifier());
      targetFile.pushString("[0]");
    }
  }
}

void dsl_cpp_generator::generateExpr(Expression *expr, bool isMainFile, bool isAtomic)
{ // isAtomic default to false
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  if (expr->isLiteral())
  {
    //~ cout << "INSIDE THIS FOR LITERAL"
    //~ << "\n";
    std::cout<< "------>PROP LIT"  << '\n';
    generate_exprLiteral(expr, isMainFile);
  }
  else if (expr->isInfinity())
  {
    generate_exprInfinity(expr, isMainFile);
  }
  else if (expr->isIdentifierExpr())
  {
  std::cout<< "------>PROP ID"  << '\n';
    generate_exprIdentifier(expr->getId(), isMainFile);
  }
  else if (expr->isPropIdExpr())
  {
    std::cout<< "------>PROP EXP"  << '\n';
    generate_exprPropId(expr->getPropId(), isMainFile);
  }
  else if (expr->isArithmetic() || expr->isLogical())
  {
    std::cout<< "------>PROP AR/LG"  << '\n';
    generate_exprArL(expr, isMainFile, isAtomic);
  }
  else if (expr->isRelational())
  {
    std::cout<< "------>PROP RL"  << '\n';
    generate_exprRelational(expr, isMainFile);
  }
  else if (expr->isProcCallExpr())
  {
    std::cout << "------>PROP PRO CAL" << '\n';
    generate_exprProcCall(expr, isMainFile);
  }
  else if (expr->isUnary())
  {
    std::cout << "------>PROP UNARY" << '\n';
    generate_exprUnary(expr, isMainFile);
  }
  else
  {
    assert(false);
  }
}

void dsl_cpp_generator::generate_exprArL(Expression *expr, bool isMainFile, bool isAtomic)
{ // isAtomic default to false
  dslCodePad &targetFile = isMainFile ? main : header;
  if (expr->hasEnclosedBrackets())
  {
    targetFile.pushString("(");
  }
  if (!isAtomic)
    generateExpr(expr->getLeft(), isMainFile);
  targetFile.space();
  const char *operatorString = getOperatorString(expr->getOperatorType());
  if (!isAtomic)
    targetFile.pushstr_space(operatorString);
  generateExpr(expr->getRight(), isMainFile);
  if (expr->hasEnclosedBrackets())
  {
    targetFile.pushString(")");
  }
}

void dsl_cpp_generator::generate_exprUnary(Expression *expr, bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  if (expr->hasEnclosedBrackets())
  {
    targetFile.pushString("(");
  }

  if (expr->getOperatorType() == OPERATOR_NOT)
  {
    const char *operatorString = getOperatorString(expr->getOperatorType());
    targetFile.pushString(operatorString);
    generateExpr(expr->getUnaryExpr(), isMainFile);
  }

  if (expr->getOperatorType() == OPERATOR_INC ||
      expr->getOperatorType() == OPERATOR_DEC)
  {
    generateExpr(expr->getUnaryExpr(), isMainFile);
    const char *operatorString = getOperatorString(expr->getOperatorType());
    targetFile.pushString(operatorString);
  }

  if (expr->hasEnclosedBrackets())
  {
    targetFile.pushString(")");
  }
}

void dsl_cpp_generator::generate_exprProcCall(Expression *expr,
                                              bool isMainFile)
{
  //~ cout << "inside the expr_proCall ggggggggggggggggggggggggg" << isMainFile;

  dslCodePad &targetFile = isMainFile ? main : header;

  proc_callExpr *proc = (proc_callExpr *)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if (methodId == "get_edge")
  {
    //~ cout << "heloooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";
    targetFile.pushString(
        "edge"); // To be changed..need to check for a neighbour iteration
                 // and then replace by the iterator.
  }
  else if (methodId == "count_outNbrs") // pageRank
  {
    char strBuffer[1024];
    list<argument *> argList = proc->getArgList();
    assert(argList.size() == 1);
    Identifier *nodeId = argList.front()->getExpr()->getId();
    //~ Identifier* objectId = proc->getId1();
    sprintf(strBuffer, "(%s[%s+1]-%s[%s])", "d_meta", nodeId->getIdentifier(), "d_meta", nodeId->getIdentifier());
    targetFile.pushString(strBuffer);
  }
  else if (methodId == "is_an_edge")
  {
    char strBuffer[1024];
    list<argument *> argList = proc->getArgList();
    assert(argList.size() == 2);
    Identifier *srcId = argList.front()->getExpr()->getId();
    Identifier *destId = argList.back()->getExpr()->getId();
    //~ Identifier* objectId = proc->getId1();
    sprintf(strBuffer, "%s(%s, %s, %s, %s)", "findNeighborSorted", srcId->getIdentifier(), destId->getIdentifier(), "d_meta", "d_data");
    targetFile.pushString(strBuffer);
  }
  else
  {
    char strBuffer[1024];
    list<argument *> argList = proc->getArgList();
    if (argList.size() == 0)
    {
      Identifier *objectId = proc->getId1();
      sprintf(strBuffer, "%s.%s( )", objectId->getIdentifier(),
              proc->getMethodId()->getIdentifier());
      targetFile.pushString(strBuffer);
    }
  }
}

void dsl_cpp_generator::generate_exprPropId(
    PropAccess *propId, bool isMainFile) // This needs to be made more generic.
{
  dslCodePad &targetFile = isMainFile ? main : header;

  char strBuffer[1024];
  Identifier *id1 = propId->getIdentifier1();
  Identifier *id2 = propId->getIdentifier2();
  ASTNode *propParent = propId->getParent();
  bool relatedToReduction =
      propParent != NULL ? propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT
                         : false;
  if (id2->getSymbolInfo() != NULL &&
      id2->getSymbolInfo()->getId()->get_fp_association() &&
      relatedToReduction)
  {
    sprintf(strBuffer, "d_%s_next[%s]", id2->getIdentifier(),
            id1->getIdentifier());
  }
  else
  {
    if (!isMainFile)
      sprintf(strBuffer, "d_%s[%s]", id2->getIdentifier(), id1->getIdentifier()); // PREFIX D
    else
      sprintf(strBuffer, "%s[%s]", id2->getIdentifier(), id1->getIdentifier());
  }
  //~ std::cout<< id2->getIdentifier() << id1->getIdentifier() << '\n';
  targetFile.pushString(strBuffer);
}
/* //// ASHWINA's VERSION
void dsl_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct,
                                           bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;
  std::cout<< "FXPT BLK" << '\n';
  //~ std::cout<< "\t var1" <<  << '\n';
  //~ std::cout<< "\t var2" << << '\n';
  char strBuffer[1024];
  Expression* convergeExpr = fixedPointConstruct->getDependentProp();
  Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
  //~ statement* blockStmt = fixedPointConstruct->getBody();
  assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
         convergeExpr->getExpressionFamily() == EXPR_ID);
  targetFile.pushString("while(");
  targetFile.push('!');
  targetFile.pushString(fixedPointId->getIdentifier());
  //~ targetFile.pushString("[0]");  // need to be changed later. Currently done for SSSP
  //~ targetFile.pushstr_newL(" )");
  targetFile.pushstr_newL(") { // WHILE begin FXPT");
  sprintf(strBuffer, "%s = %s;", fixedPointId->getIdentifier(),"true");  // needs to be changed. Currently done for SSSP
  targetFile.pushstr_newL(strBuffer);
  if (fixedPointConstruct->getBody()->getTypeofNode() != NODE_BLOCKSTMT)
    generateStatement(fixedPointConstruct->getBody(), isMainFile);
  else
    generateBlock((blockStatement*)fixedPointConstruct->getBody(), false, isMainFile);
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
  const char *flagVar   = dependentId->getIdentifier();
  const char *fixPointVar = fixedPointId->getIdentifier();
  targetFile.pushstr_newL("// FIXED POINT variables");
  char devicefixPointVar[80] = "d_";
  strcat(devicefixPointVar, fixPointVar);
  generateExtraDeviceVariable("bool",fixPointVar, "1");
  generateExtraDeviceVariable("bool",flagVar, "V");


  if (convergeExpr->getExpressionFamily() == EXPR_ID)
    dependentId = convergeExpr->getId();
  if (dependentId != NULL) {
    if (dependentId->getSymbolInfo()->getType()->isPropType()) {
      if (dependentId->getSymbolInfo()->getType()->isPropNodeType()) {
        //~ Type* type = dependentId->getSymbolInfo()->getType();

        if (graphId.size() > 1) {
          cerr << "GRAPH AMBIGUILTY";
        } else {
          // sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
          // ","int","v","v",graphId[0]->getIdentifier(),"num_nodes","v");
          // targetFile.pushstr_newL(strBuffer);
          // targetFile.pushstr_space("{");
          targetFile.pushstr_newL(
              " initKernel<bool> <<< 1, 1>>>(1, gpu_finished, true);");
        //~ targetFile.pushstr_newL(
            //~ " Compute_SSSP_kernel<<<num_blocks , "
            //~ "block_size>>>(d_meta,d_data, d_weight ,gpu_dist,src, V "
            //~ ",MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);");
        targetFile.pushstr_newL(
            " initKernel<bool><<<num_blocks,block_size>>>(V, "
            "gpu_modified_prev, false);");
        targetFile.pushstr_newL(
            " cudaMemcpy(finished, gpu_finished,  sizeof(bool) *(1), "
            "cudaMemcpyDeviceToHost);");
        sprintf(strBuffer, "%s* %s = %s_nxt ;", "bool", "tempModPtr",
                dependentId->getIdentifier());
        targetFile.pushstr_newL(strBuffer);

        sprintf(strBuffer, "%s_nxt = %s_prev ;", dependentId->getIdentifier(),
                dependentId->getIdentifier());
        targetFile.pushstr_newL(strBuffer);

        sprintf(strBuffer, "%s_prev = %s ;", dependentId->getIdentifier(),
                "tempModPtr");
        targetFile.pushstr_newL(strBuffer);

        Expression* initializer =
            dependentId->getSymbolInfo()->getId()->get_assignedExpr();
        assert(initializer->isBooleanLiteral());
        sprintf(strBuffer, "%s_nxt[%s] = %s ;", dependentId->getIdentifier(),
                "v", initializer->getBooleanConstant() ? "true" : "false");
        targetFile.pushstr_newL(strBuffer);
        // targetFile.pushstr_newL("}");

      //~ if(isNot)
       //~ {
        //~ sprintf(strBuffer,"%s = !%s_fp;",fixedPointId->getIdentifier(),dependentId->getIdentifier());
        //~ main.pushstr_newL(strBuffer);
         //~ }
         //~ else
         //~ {
           //~ sprintf(strBuffer,"%s = %s_fp    ;",fixedPointId->getIdentifier(),dependentId->getIdentifier());
           //~ main.pushString(strBuffer);
         //~ }

       }
      }
    }
  }
  targetFile.pushstr_newL("}");
}
**/
void dsl_cpp_generator::generateFixedPoint(fixedPointStmt *fixedPointConstruct, bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;

  std::cout << "IN FIX PT" << '\n';
  char strBuffer[1024];
  Expression *convergeExpr = fixedPointConstruct->getDependentProp();
  Identifier *fixedPointId = fixedPointConstruct->getFixedPointId();

  //~ statement* blockStmt = fixedPointConstruct->getBody();
  assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
         convergeExpr->getExpressionFamily() == EXPR_ID);

  Identifier *dependentId = NULL;
  //~ bool isNot = false;
  assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
         convergeExpr->getExpressionFamily() == EXPR_ID);
  if (convergeExpr->getExpressionFamily() == EXPR_UNARY)
  {
    if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID)
    {
      dependentId = convergeExpr->getUnaryExpr()->getId();
      //~ isNot = true;
    }
  }
  const char *modifiedVar = dependentId->getIdentifier();
  char *fixPointVar = fixedPointId->getIdentifier();

  const char *modifiedVarType = convertToCppType(dependentId->getSymbolInfo()->getType()->getInnerTargetType()); // BOTH are of type bool
  const char *fixPointVarType = convertToCppType(fixedPointId->getSymbolInfo()->getType());

  targetFile.pushstr_newL("// FIXED POINT variables");
  // char modifiedVarPrev[80] = "d_";
  char modifiedVarNext[80] = "d_";

  // strcat(modifiedVarPrev, modifiedVar);strcat(modifiedVarPrev, "_prev");
  strcat(modifiedVarNext, modifiedVar);
  strcat(modifiedVarNext, "_next");


  
  fixed_pt_var = new char[25];
  fixed_pt_type = new char[25];
  strcpy(fixed_pt_var, fixPointVar); 
  strcpy(fixed_pt_type,fixPointVarType);
  FIXEDPOINT = true;
  // strcpy(fixed_pt_var, fixPointVar); // for use in adding params to 

  // char devicefixPointVar[80] = "d_";
  // strcat(devicefixPointVar, fixPointVar);

  // generateExtraDeviceVariable(fixPointVarType,fixPointVar, "1");

  // generateExtraDeviceVariableNoD(modifiedVarType,modifiedVarPrev, "V");
  // generateExtraDeviceVariableNoD(modifiedVarType,modifiedVarNext, "V");

  // targetFile.NewLine();
  //~ generateExtraDeviceVariable("bool",devicefixPointVar, "1");

  if (convergeExpr->getExpressionFamily() == EXPR_ID)
    dependentId = convergeExpr->getId();
  if (dependentId != NULL)
  {
    if (dependentId->getSymbolInfo()->getType()->isPropType())
    {
      if (dependentId->getSymbolInfo()->getType()->isPropNodeType())
      {
        targetFile.pushstr_newL("//BEGIN FIXED POINT");

       
        sprintf(strBuffer, "for(int i = 0 ; i < devicecount ; i++){");
        targetFile.pushstr_newL(strBuffer);
        sprintf(strBuffer, "cudaSetDevice(i);"); 
        targetFile.pushstr_newL(strBuffer); 

        sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V, %s[i], false);", modifiedVarType, modifiedVarNext);
        targetFile.pushstr_newL(strBuffer);
      
        sprintf(strBuffer, "}") ; 
        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
        targetFile.pushstr_newL("cudaSetDevice(i);");
        targetFile.pushstr_newL("cudaDeviceSynchronize();");
        targetFile.pushstr_newL("}");        
        
        targetFile.NewLine() ; 


        sprintf(strBuffer, "while(!%s) {", fixPointVar);
        targetFile.pushstr_newL(strBuffer);

        main.NewLine();
        // sprintf(strBuffer, "initIndex<%s> <<<1,1>>>(1,%s,0,true);", fixPointVarType, devicefixPointVar);
        sprintf(strBuffer, "%s = %s", fixPointVar, "true");
        targetFile.pushString(strBuffer);
        targetFile.pushstr_newL(";");
        main.pushstr_newL("for(int i=0;i<devicecount;i++){");
        main.pushstr_newL("cudaSetDevice(i);");
        sprintf(strBuffer,"initKernel<%s><<<1,1>>>(1,d_%s[i],(%s)true);",fixPointVarType,fixPointVar,fixPointVarType);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("}");
        main.pushstr_newL("for(int i=0;i<devicecount;i++){");
        main.pushstr_newL("cudaSetDevice(i);");
        main.pushstr_newL("cudaDeviceSynchronize();");
        main.pushstr_newL("}");        
        
        generateCudaMemCpySymbol(fixPointVar, fixPointVarType, true);
        // targetFile.pushstr_newL(strBuffer);

        if (fixedPointConstruct->getBody()->getTypeofNode() != NODE_BLOCKSTMT)
          generateStatement(fixedPointConstruct->getBody(), isMainFile);
        else{
          cout << "-----------------------FROM HERE CHECK TYPES" << endl ; 
          generateBlock((blockStatement *)fixedPointConstruct->getBody(), false, isMainFile);
        }

        // generateCudaMemCpySymbol(fixPointVar, fixPointVarType, false);
        //~ targetFile.pushstr_newL( "Compute_SSSP_kernel<<<num_blocks,block_size>>>(gpu_OA,gpu_edgeList, gpu_edgeLen ,gpu_dist,src, V " ",MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);");
        if(modified_used==true){
          sprintf(strBuffer,"%s* h_%s1;",modifiedVarType,modifiedVar);
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("if(devicecount==1){");
          sprintf(strBuffer,"cudaMemcpy(d_%s[0],d_%s_next[0],sizeof(%s)*(V+1),cudaMemcpyDeviceToDevice);",modifiedVar,modifiedVar,modifiedVarType);
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("}");  
          main.pushstr_newL("if(devicecount>1){");
          if(pullBased){
            main.pushstr_newL("for(int i=0;i<devicecount;i++){");
            main.pushstr_newL("cudaSetDevice(i);");
            sprintf(strBuffer,"cudaMemcpyAsync(h_%s+h_vertex_partition[i],d_%s_next[i]+h_vertex_partition[i],sizeof(%s)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);",modifiedVar,modifiedVar,modifiedVarType);
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("}");
            main.pushstr_newL("for(int i=0;i<devicecount;i++){");
            main.pushstr_newL("cudaSetDevice(i);");
            main.pushstr_newL("cudaDeviceSynchronize();");
            main.pushstr_newL("}");

            
          }
          else{
            sprintf(strBuffer,"h_%s1 = (%s*)malloc((V+1)*sizeof(%s));",modifiedVar,modifiedVarType,modifiedVarType);
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("for(int i=0;i<devicecount;i++){");
            main.pushstr_newL("cudaSetDevice(i);");
            sprintf(strBuffer,"cudaMemcpyAsync(h_%s+i*(V+1),d_%s_next[i],sizeof(%s)*(V+1),cudaMemcpyDeviceToHost);",modifiedVar,modifiedVar,modifiedVarType);
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("}");
            main.pushstr_newL("for(int i=0;i<devicecount;i++){");
            main.pushstr_newL("cudaSetDevice(i);");
            main.pushstr_newL("cudaDeviceSynchronize();");
            main.pushstr_newL("}");
            targetFile.pushstr_newL("cudaSetDevice(0);");
            sprintf(strBuffer,"bool* d_%s_temp;",modifiedVar);
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"bool* d_%s_temp1;",modifiedVar);
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"cudaMalloc(&d_%s_temp,(V+1)*sizeof(%s));",modifiedVar,modifiedVarType);
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"cudaMalloc(&d_%s_temp1,(devicecount)*(V+1)*sizeof(%s));",modifiedVar,modifiedVarType);
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"initKernel<%s><<<numBlocks,threadsPerBlock>>>(V+1,d_%s_temp,false);",modifiedVarType,modifiedVar);
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"for(int i=0;i<devicecount;i++){");
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"cudaMemcpy(d_%s_temp1+i*(V+1),h_%s1+i*(V+1),sizeof(%s)*(V+1),cudaMemcpyHostToDevice);",modifiedVar,modifiedVar,modifiedVarType);
            targetFile.pushstr_newL(strBuffer);
            targetFile.pushstr_newL("}");
            sprintf(strBuffer,"Compute_Or<<<numBlocks,threadsPerBlock>>>(d_%s_temp1,d_%s_temp,V,devicecount);",modifiedVar,modifiedVar);
            targetFile.pushstr_newL(strBuffer);
            sprintf(strBuffer,"cudaMemcpy(h_%s,d_%s_temp,sizeof(%s)*(V+1),cudaMemcpyDeviceToHost);",modifiedVar,modifiedVar,modifiedVarType);
            targetFile.pushstr_newL(strBuffer);
            targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
            targetFile.pushstr_newL("cudaSetDevice(i);");
            targetFile.pushstr_newL("cudaDeviceSynchronize();");
            targetFile.pushstr_newL("}");
          }

          sprintf(strBuffer, "for(int i = 0 ; i < devicecount ; i++){");
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("cudaSetDevice(i);");
          sprintf(strBuffer, "cudaMemcpyAsync(d_%s[i],h_%s,sizeof(%s)*(V+1),cudaMemcpyHostToDevice);", modifiedVar, modifiedVar, fixPointVarType);
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("}");
          targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
          targetFile.pushstr_newL("cudaSetDevice(i);");
          targetFile.pushstr_newL("cudaDeviceSynchronize();");
          targetFile.pushstr_newL("}");
          main.pushstr_newL("}");
          sprintf(strBuffer, "for(int i = 0 ; i < devicecount ; i++){");
          targetFile.pushstr_newL(strBuffer);
          sprintf(strBuffer, "cudaSetDevice(i);"); 
          targetFile.pushstr_newL(strBuffer); 
          sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V, %s[i], false);", modifiedVarType, modifiedVarNext);
          targetFile.pushstr_newL(strBuffer);
          sprintf(strBuffer, "}") ; 
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
          targetFile.pushstr_newL("cudaSetDevice(i);");
          targetFile.pushstr_newL("cudaDeviceSynchronize();");
          targetFile.pushstr_newL("}");        
        }
        // sprintf(strBuffer, "cudaMemcpy(d_%s, %s, sizeof(%s)*V, cudaMemcpyDeviceToDevice)", modifiedVar,
                // modifiedVarNext, fixPointVarType);
        // targetFile.pushString(strBuffer);
        // targetFile.pushstr_newL(";");

        // sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V, %s, false);", fixPointVarType, modifiedVarNext);
        // targetFile.pushstr_newL(strBuffer);

        // generateCudaMemCpyStr("&finished", devicefixPointVar, fixPointVarType, "1", false);

        //~ targetFile.pushstr_newL("cudaMemcpy(finished, finished,  sizeof(bool) *(1), cudaMemcpyDeviceToHost);");
        // sprintf(strBuffer, "%s* %s = %s ; // SWAP next and prev ptrs", modifiedVarType, "tempModPtr",modifiedVarNext);
        // targetFile.pushstr_newL(strBuffer);

        //~ sprintf(strBuffer, "%s* %s = %s_nxt ;", "bool", "tempModPtr",dependentId->getIdentifier());
        //~ targetFile.pushstr_newL(strBuffer);

        /*sprintf(strBuffer, "%s = %s;", modifiedVarNext, modifiedVarPrev);
        targetFile.pushstr_newL(strBuffer);
        sprintf(strBuffer, "%s = %s;", modifiedVarPrev,"tempModPtr");
        targetFile.pushstr_newL(strBuffer);*/
        targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
        targetFile.pushstr_newL("cudaSetDevice(i);");
        sprintf(strBuffer, "cudaMemcpyAsync(h_%s[i], d_%s[i], sizeof(%s)*1, cudaMemcpyDeviceToHost);", fixPointVar, fixPointVar, fixPointVarType);
        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("}");

        targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
        targetFile.pushstr_newL("cudaSetDevice(i);");
        targetFile.pushstr_newL("cudaDeviceSynchronize();");
        targetFile.pushstr_newL("}");

        targetFile.pushstr_newL("for(int i=0;i<devicecount;i++){");
        sprintf(strBuffer,"%s&=h_%s[i][0];", fixPointVar,fixPointVar);
        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("}");
        

        
        targetFile.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){");
        targetFile.pushstr_newL("cudaSetDevice(i);");
        // sprintf(strBuffer, "cudaMalloc(&d_%s[i],sizeof(%s));", fixPointVar, fixPointVarType);
        // targetFile.pushstr_newL(strBuffer);
        sprintf(strBuffer, "initKernel<%s> <<<1,1>>>(1,d_%s[i],true);", fixPointVarType, fixPointVar);
        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("}");

        targetFile.NewLine() ; 
        
        targetFile.pushstr_newL("for(int i = 0 ; i < devicecount ; i++){"); 
        targetFile.pushstr_newL("cudaSetDevice(i);");
        targetFile.pushstr_newL("cudaDeviceSynchronize();"); 
        targetFile.pushstr_newL("}"); 

        Expression *initializer = dependentId->getSymbolInfo()->getId()->get_assignedExpr();
        assert(initializer->isBooleanLiteral());
        //~ sprintf(strBuffer, "%s[%s] = %s ;", modifiedVarNext,"v", initializer->getBooleanConstant() ? "true" : "false");
        //~ targetFile.pushstr_newL(strBuffer);
      }
    }
  }
  targetFile.pushstr_newL("} // END FIXED POINT");
  targetFile.NewLine();
  FIXEDPOINT = false;
  modified_used = false;
}

void dsl_cpp_generator::generateBlock(blockStatement *blockStmt, bool includeBrace, bool isMainFile)
{
  //~ cout << "i am inside generateBlock for the first time and the value of bool="
  //~ << isMainFile;
  dslCodePad &targetFile = isMainFile ? main : header;

  list<statement *> stmtList = blockStmt->returnStatements();
  list<statement *>::iterator itr;
  if (includeBrace)
  {
    targetFile.pushstr_newL("{");
  }
  for (itr = stmtList.begin(); itr != stmtList.end(); itr++)
  {
    statement *stmt = *itr;
    generateStatement(stmt, isMainFile);
  }
  if (includeBrace)
  {
    targetFile.pushstr_newL("}");
  }
}



void dsl_cpp_generator::generateStopTimer()
{
  main.pushstr_newL("cudaEventRecord(stop,0);");
  main.pushstr_newL("cudaEventSynchronize(stop);");
  main.pushstr_newL("cudaEventElapsedTime(&milliseconds, start, stop);");
  main.pushstr_newL("printf(\"GPU Time: %.6f ms\\n\", milliseconds);");
}
void dsl_cpp_generator::generateStartTimer()
{
  //~ cudaEvent_t start, stop; ///TIMER START
  //~ cudaEventCreate(&start);
  //~ cudaEventCreate(&stop);
  //~ float milliseconds = 0;
  //~ cudaEventRecord(start,0);

  main.pushstr_newL("cudaEvent_t start, stop;");
  main.pushstr_newL("cudaEventCreate(&start);");
  main.pushstr_newL("cudaEventCreate(&stop);");
  main.pushstr_newL("float milliseconds = 0;");
  main.pushstr_newL("cudaEventRecord(start,0);");
  main.NewLine();
}

void dsl_cpp_generator::generateCudaMallocParams(list<formalParam *> paramList)
{

  list<formalParam *>::iterator itr;

  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {
    cout << "Hewwo " << (*itr)->getIdentifier()->getIdentifier() << endl; 
    Type *type = (*itr)->getType();
    if (type->isPropType())
    {
      cout << "1" << endl;  
      
      if (type->getInnerTargetType()->isPrimitiveType())
      {
        Type *innerType = type->getInnerTargetType();
        
          main.pushString(convertToCppType(
            innerType)); // convertToCppType need to be modified.
          main.pushString("*");
          main.space();

         char str[80];
        strcpy(str, "h_");
        strcat(str, (*itr)->getIdentifier()->getIdentifier());
        main.pushString(str);
        main.pushstr_newL(";");
        strcpy(str,"h_");
        strcat(str,(*itr)->getIdentifier()->getIdentifier());
        strcat(str,"= (");
        strcat(str, convertToCppType(type->getInnerTargetType()));
         strcat(str,  "*)malloc(sizeof(");
        strcat(str, convertToCppType(type->getInnerTargetType()));
        strcat(str, ")") ; 
        strcat(str, "*") ; 
        strcat(str, "(V+1)"); 
        strcat(str, ");") ; 
        main.pushString(str);
        main.NewLine();
        main.pushString(convertToCppType(
            innerType)); // convertToCppType need to be modified.
        main.pushString("**");
        main.space();
        strcpy(str, "d_");
        strcat(str, (*itr)->getIdentifier()->getIdentifier());
        main.pushString(str);
        main.pushstr_newL(";");

        strcpy(str, "d_") ; 
        strcat(str, (*itr)->getIdentifier()->getIdentifier());
        strcat(str, " = (");
        strcat(str, convertToCppType(type->getInnerTargetType()));
        strcat(str,  "**)malloc(sizeof(");
        strcat(str, convertToCppType(type->getInnerTargetType()));
        strcat(str, "*)") ; 
        strcat(str, "*") ; 
        strcat(str, "devicecount"); 
        strcat(str, ")") ; 
        main.pushString(str); 
        main.pushstr_newL(";");

        strcpy(str, "for (int i = 0; i < devicecount; i++) {");
        main.pushstr_newL(str);
        main.pushstr_newL("cudaSetDevice(i);");
        strcpy(str, "cudaMalloc(&"); 
        strcat(str, "d_") ; 
        strcat(str, (*itr)->getIdentifier()->getIdentifier()); 
        strcat(str, "[i], sizeof("); 
        strcat(str, convertToCppType(type->getInnerTargetType()));
        strcat(str, ")*"); 
        strcat(str, (type->isPropNodeType()) ? "(V+1)" : "E") ; 
        strcat(str, ");"); 
        main.pushstr_newL(str);
        main.pushstr_newL("}");
        main.NewLine(); 
      }
    }
    else if(type->isPrimitiveType()){
      Identifier* iden = (*itr)->getIdentifier();
      char str[80];
      strcpy(str,convertToCppType(type));
      strcat(str,"** ");
      strcat(str, "d_") ; 
      strcat(str, (*itr)->getIdentifier()->getIdentifier()); //
      strcat(str,";");
      main.pushstr_newL(str);

      strcpy(str, "d_") ; 
      strcat(str, (*itr)->getIdentifier()->getIdentifier());//
      strcat(str, " = (");
      strcat(str, convertToCppType(type));
      strcat(str,  "**)malloc(sizeof(");
      strcat(str, convertToCppType(type));
      strcat(str, "*)") ; 
      strcat(str, "*") ; 
      strcat(str, "devicecount"); 
      strcat(str, ")") ; 
      main.pushString(str); 
      main.pushstr_newL(";");

      strcpy(str, "for (int i = 0; i < devicecount; i++) {");
      main.pushstr_newL(str);
      main.pushstr_newL("cudaSetDevice(i);");
      strcpy(str, "cudaMalloc(&"); 
      strcat(str, "d_") ; 
      strcat(str, (*itr)->getIdentifier()->getIdentifier());
      //strcat(str, (*itr)->getIdentifier()->getIdentifier()); 
      
      strcat(str, "[i], sizeof("); 
      strcat(str, convertToCppType(type));
      strcat(str, ")");  
      strcat(str, ");"); 
      main.pushstr_newL(str);
      
      /////initKernel<float> <<<1,1>>>(1,d_num_nodes[i],0);
      strcpy(str,"initKernel<");
      strcat(str,convertToCppType(type));
      strcat(str,"> <<<1,1>>>(1,d_");
      strcat(str,(*itr)->getIdentifier()->getIdentifier());
      strcat(str,"[i],");
      strcat(str,(*itr)->getIdentifier()->getIdentifier());
      strcat(str,");");
      main.pushstr_newL(str);
      main.pushstr_newL("}");
      main.NewLine();

      strcpy(str,convertToCppType(type));
      strcat(str,"** ");
      strcat(str, "h_") ; 
      strcat(str, (*itr)->getIdentifier()->getIdentifier());
      strcat(str, " = (");
      strcat(str, convertToCppType(type));
      strcat(str,  "**)malloc(sizeof(");
      strcat(str, convertToCppType(type));
      strcat(str, "*)") ; 
      strcat(str, "*") ; 
      strcat(str, "(devicecount+1)"); 
      strcat(str, ")") ; 
      main.pushString(str); 
      main.pushstr_newL(";");
      main.pushstr_newL("for(int i=0;i<=devicecount;i++){");
      
      strcpy(str,"h_");
      strcat(str,(*itr)->getIdentifier()->getIdentifier());
      strcat(str,"[i] = (");
      strcat(str, convertToCppType(type));
      strcat(str,  "*)malloc(sizeof(");
      strcat(str, convertToCppType(type));
      strcat(str, ")") ; 
      strcat(str, ");") ; 
      main.pushString(str);
      main.NewLine();
      main.pushstr_newL("}");
    } 
  }
}

void dsl_cpp_generator::generateCudaMemCpyParams(list<formalParam *> paramList)
{

  list<formalParam *>::iterator itr;
  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {

    Type *type = (*itr)->getType();
    if (type->isPropType())
    {
      if (type->getInnerTargetType()->isPrimitiveType())
      {
        //~ Type* innerType = type->getInnerTargetType();

        const char *sizeofProp = NULL;
        if (type->isPropEdgeType())
          sizeofProp = "E";
        else
          sizeofProp = "V";
        const char *temp = "d_";
        char *temp1 = (*itr)->getIdentifier()->getIdentifier();
        char *temp2 = (char *)malloc(1 + strlen(temp) + strlen(temp1));
        strcpy(temp2, temp);
        strcat(temp2, temp1);

        generateCudaMemCpyStr((*itr)->getIdentifier()->getIdentifier(), temp2, convertToCppType(type->getInnerTargetType()), sizeofProp, 0);

      }
    }
  }
}
void dsl_cpp_generator::generateFunc(ASTNode *proc)
{
  // dslCodePad &targetFile = isMainFile ? main : header;

  //~ char strBuffer[1024];
  Function *func = (Function *)proc;
  generateFuncHeader(func, false); //.h or header file | adds prototype of main function and std headers files
  generateFuncHeader(func, true);  // .cu or main file

  // to genearte the function body of the algorithm
  // Note that this we can change later point of time if required

  //~ char outVarName[] = "BC";  //inner type
  //~ char outVarType[] = "double";
  main.pushstr_newL("{");

  generateFuncBody(func, false); // GEnerates CSR ..bool is meaningless
// 
  main.NewLine();

  main.pushstr_newL("//DECLARE DEVICE AND HOST vars in params");
  /* function for generation of cudamalloc for property type params*/
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

  generateBlock(func->getBlockStatement(), false);

  // Assuming one function!
  main.pushstr_newL("//TIMER STOP");
  generateStopTimer();
  main.NewLine();
  generateCudaMemCpyParams(func->getParamList());
  

  main.pushstr_newL("} //end FUN");

  return;
}

const char *dsl_cpp_generator::convertToCppType(Type *type)
{
  if (type->isPrimitiveType())
  {
    int typeId = type->gettypeId();
    switch (typeId)
    {
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
  }
  else if (type->isPropType())
  {
    Type *targetType = type->getInnerTargetType();
    if (targetType->isPrimitiveType())
    {
      int typeId = targetType->gettypeId();
      //~ cout << "TYPEID IN CPP" << typeId << "\n";
      switch (typeId)
      {
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
  }
  else if (type->isNodeEdgeType())
  {
    return "int"; // need to be modified.
  }
  else if (type->isGraphType())
  {
    return "graph&";
  }
  else if (type->isCollectionType())
  {
    int typeId = type->gettypeId();

    switch (typeId)
    {
    case TYPE_SETN:
      return "std::set<int>&";

    default:
      assert(false);
    }
  }

  return "NA";
}

void dsl_cpp_generator::generateCudaMemCpySymbol(char *var, const char *typeStr, bool direction)
{
  char strBuffer[1024];
  // main.pushstr_newL("for(int i=0;i<devicecount;i+=1){");
  // main.pushstr_newL("cudaSetDevice(i);");
  // sprintf(strBuffer,"d_%s[0]=%s",var,var);
  // main.pushstr_newL(strBuffer);
  // main.pushstr_newL("}");
  // main.pushstr_newL("for(int i=0;i<devicecount;i+=1){");
  // main.pushstr_newL("cudaSetDevice(i);");
  // main.pushstr_newL("cudaDeviceSynchronize();");
  // main.pushstr_newL("}");

  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3
  /*
  if(direction)
  {
  sprintf(strBuffer, "cudaMemcpyToSymbol(::%s, &%s, sizeof(%s), 0, cudaMemcpyHostToDevice);", var, var,
          typeStr);  // this assumes PropNode type  IS PROPNODE? V : E //else
                     // might error later
  }
   else
   {
     sprintf(strBuffer, "cudaMemcpyFromSymbol(&%s, ::%s, sizeof(%s), 0, cudaMemcpyDeviceToHost);", var, var,
          typeStr);
   }
  main.pushstr_newL(strBuffer);*/

}

void dsl_cpp_generator::generateCudaMallocStr(const char *dVar,
                                              const char *typeStr,
                                              const char *sizeOfType)
{
  char strBuffer[1024];
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3
  sprintf(strBuffer, "cudaMalloc(&%s, sizeof(%s)*%s);", dVar, typeStr,
          sizeOfType); // this assumes PropNode type  IS PROPNODE? V : E //else
                       // might error later
  main.pushstr_newL(strBuffer);
  //~ main.NewLine();
}

// Oonly for device variables
void dsl_cpp_generator::generateCudaMalloc(Type *type, const char *identifier)
{
  char str[1024];
  //~ Type* targetType = type->getInnerTargetType();
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //
  // d_dist = (int **)malloc(sizeof(int *) * devicecount);
  strcpy(str, "d_");
  strcat(str, identifier);
  strcat(str, " = (");
  strcat(str, convertToCppType(type->getInnerTargetType()));
  strcat(str, "**)malloc(sizeof(");
  strcat(str, convertToCppType(type->getInnerTargetType()));
  strcat(str, "*)");
  strcat(str, "*");
  strcat(str, "devicecount");
  strcat(str, ")");
  main.pushString(str);
  main.pushstr_newL(";");

  // // for (int i = 0; i < devicecount; i++)
  // // {
  // //   cudaSetDevice(i);
  // //   cudaMalloc(&d_dist[i], sizeof(int) * (V + 1));
  // // }

  strcpy(str, "for (int i = 0; i < devicecount; i++) {");
  main.pushstr_newL(str);
  main.pushstr_newL("cudaSetDevice(i);");
  strcpy(str, "cudaMalloc(&");
  strcat(str, "d_");
  strcat(str, identifier);
  strcat(str, "[i], sizeof(");
  strcat(str, convertToCppType(type->getInnerTargetType()));
  strcat(str, ")*");
  strcat(str, (type->isPropNodeType()) ? "(V+1)" : "E");
  strcat(str, ");");
  main.pushstr_newL(str);
  main.pushstr_newL("}");
  main.NewLine();
  // sprintf(strBuffer, "cudaMalloc(&d_%s, sizeof(%s)*(%s));", identifier,
  //         convertToCppType(type->getInnerTargetType()),
  //         (type->isPropNodeType())
  //             ? "V"
  //             : "E"); // this assumes PropNode type  IS PROPNODE? V : E //else
  //                     // might error later

  // main.pushstr_newL(strBuffer);
  // main.NewLine();
}

//~ bool* d_finished;cudaMalloc(&d_finished,sizeof(bool) *(V));
void dsl_cpp_generator::generateExtraDeviceVariable(const char *typeStr, const char *dVar, const char *sizeVal)
{
  // bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  //  1       2                                 2           1      3
  char strBuffer[1024];
  sprintf(strBuffer, "%s* d_%s; cudaMalloc(&d_%s,sizeof(%s)*(%s));", typeStr, dVar, dVar, typeStr, sizeVal);
  main.pushstr_newL(strBuffer);
  //~ main.NewLine();
}

//~ bool* d_finished;cudaMalloc(&d_finished,sizeof(bool) *(V));
void dsl_cpp_generator::generateExtraDeviceVariableNoD(const char *typeStr, const char *dVar, const char *sizeVal)
{
  // bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  //  1       2                                 2           1      3
  char strBuffer[1024];
  sprintf(strBuffer, "%s* %s; cudaMalloc(&%s,sizeof(%s)*(%s));", typeStr, dVar, dVar, typeStr, sizeVal);
  main.pushstr_newL(strBuffer);
  //~ main.NewLine();
}

void dsl_cpp_generator::generateHeaderDeviceVariable(const char *typeStr, const char *dVar)
{
  //__device__ int triCount;
  char strBuffer[1024];
  sprintf(strBuffer, "%s* d_%s ", typeStr, dVar);
  header.pushstr_newL(strBuffer);
  //~ main.NewLine();
}

void dsl_cpp_generator::generateCudaMemcpy(const char *dVar, const char *cVar,
                                           const char *typeStr,
                                           const char *sizeOfType,
                                           bool isMainFile, const char *from)
{
  char strBuffer[1024];
  //~ dslCodePad& targetFile = isMainFile ? main : header;
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3

  sprintf(strBuffer, "cudaMemcpy(&d_%s,%s, sizeof(%s)*%s, %s);", dVar, cVar,
          typeStr, sizeOfType,
          from); // this assumes PropNode type  IS PROPNODE? V : E //else might
                 // error later
  main.pushstr_newL(strBuffer);

  //~ main.NewLine();
}

void dsl_cpp_generator::generateCSRArrays(const char *gId)
{
  char strBuffer[1024];

  sprintf(strBuffer, "int V = %s.num_nodes();",
          gId); // assuming DSL  do not contain variables as V and E
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer, "int E = %s.num_edges();", gId);
  main.pushstr_newL(strBuffer);
  main.NewLine();

  main.pushstr_newL("printf(\"#nodes:%d\\n\",V);");
  main.pushstr_newL("printf(\"#edges:\%d\\n\",E);");
  //~ main.pushstr_newL("printf(\"#srces:\%d\\n\",sourceSet.size()); /// TODO get from var");

  sprintf(strBuffer, "int* edgeLen = %s.getEdgeLen();",
          gId); // assuming DSL  do not contain variables as V and E
  main.pushstr_newL(strBuffer);
  main.NewLine();
  main.pushstr_newL(" FILE* fptr = fopen(\"num_devices.txt\",\"r\"); ");
  main.pushstr_newL("int devicecount;");
  main.pushstr_newL(" if(fptr == NULL){ ");
  main.pushstr_newL(" cudaGetDeviceCount(&devicecount); ");
  main.pushstr_newL(" } ");
  main.pushstr_newL(" else{ ");
  main.pushstr_newL(" fscanf(fptr,\" %d \",&devicecount); ");
  main.pushstr_newL(" fclose(fptr); ");
  main.pushstr_newL("}");
  // Below two lines are same for every multi-GPU backend hence Hardcoded..
  // main.pushstr_newL("int devicecount;");
  // main.pushstr_newL("cudaGetDeviceCount(&devicecount);");

  // These H & D arrays of CSR do not change. Hence hardcoded!
  // main.pushstr_newL("int *h_meta");
  // main.pushstr_newL("int *h_data;");
  // main.pushstr_newL("int *h_src;");

  main.pushstr_newL("int* h_vertex_partition;");
  main.pushstr_newL("int *h_edges;//data");
  main.pushstr_newL("int *h_weight;");
  main.pushstr_newL("int *h_offset;//meta");
  main.pushstr_newL("int* h_src;");
  main.pushstr_newL("int *h_rev_meta;");
  main.pushstr_newL("int h_vertex_per_device;");
  // main.pushstr_newL("int *h_rev_meta;"); // done only to handle PR since other doesn't uses it
  main.NewLine();

  main.pushstr_newL("h_edges = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_weight = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_offset = (int *)malloc( (V+1)*sizeof(int));");
  main.pushstr_newL("h_src = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_vertex_partition = (int*)malloc((devicecount+1)*sizeof(int));");
  main.pushstr_newL("h_rev_meta = (int *)malloc( (V+1)*sizeof(int));");
  main.pushstr_newL("h_vertex_per_device = V/devicecount;");
  main.NewLine();

  main.pushstr_newL("for(int i=0; i<= V; i++) {");
  sprintf(strBuffer, "h_offset[i] = %s.indexofNodes[i];", gId);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer, "h_rev_meta[i] = %s.rev_indexofNodes[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("}");
  main.NewLine();


  main.pushstr_newL("int index = 0;");
  main.pushstr_newL("h_vertex_partition[0]=0;");
  main.pushstr_newL("h_vertex_partition[devicecount]=V;");
  main.pushstr_newL("for(int i=1;i<devicecount;i++){");
  main.pushstr_newL("if(i<=(V%devicecount)){");
  main.pushstr_newL(" index+=(h_vertex_per_device+1);");
  main.pushstr_newL("}");
  main.pushstr_newL("else{");
  main.pushstr_newL(" index+=h_vertex_per_device;");
  main.pushstr_newL("}");
  main.pushstr_newL("h_vertex_partition[i]=index;"); 
  main.pushstr_newL("}"); 
  main.pushstr_newL("for(int i=0; i< E; i++){");
  sprintf(strBuffer, "h_edges[i]= %s.edgeList[i];", gId);
  main.pushstr_newL(strBuffer);
  // main.pushstr_newL("h_data[i] = temp;");
  // sprintf(strBuffer, "temp = %s.srcList[i];", gId);
  // main.pushstr_newL(strBuffer);
  sprintf(strBuffer,"h_src[i]=%s.srcList[i];",gId);
  main.pushstr_newL(strBuffer);
  // main.pushstr_newL("h_src[i] = temp;");
  // main.pushstr_newL("temp = edgeLen[i];");
  main.pushstr_newL("h_weight[i] = edgeLen[i];");
  main.pushstr_newL("}");
  main.NewLine();

  
}

void dsl_cpp_generator::generateFuncBody(Function *proc, bool isMainFile)
{
  //~ dslCodePad& targetFile = isMainFile ? main : header;
  char strBuffer[1024];

  int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = proc->getParamList().size();
  list<formalParam *> paramList = proc->getParamList();
  list<formalParam *>::iterator itr;

  //~ bool genCSR =
  //~ false;  // to generate csr or not in main.cpp file if graph is a parameter
  const char *
      gId; // to generate csr or not in main.cpp file if graph is a parameter
  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {
    arg_currNo++;
    argumentTotal--;

    Type *type = (*itr)->getType();
    // targetFile.pushString(convertToCppType(type));

    // targetFile.pushString(" ");

    // added here
    const char *parName = (*itr)->getIdentifier()->getIdentifier();
    // cout << "param:" <<  parName << endl;
    if (!isMainFile && type->isGraphType())
    {
      genCSR = true;
      gId = parName;
    }

    // targetFile.pushString(/*createParamName(*/(*itr)->getIdentifier()->getIdentifier());
    if (argumentTotal > 0)
      // targetFile.pushString(",");

      if (arg_currNo == maximum_arginline)
      {
        // targetFile.NewLine();
        arg_currNo = 0;
      }
  }

  if (genCSR)
  {
    main.pushstr_newL("// CSR BEGIN");
    generateCSRArrays(gId);

    
    main.NewLine();

    sprintf(strBuffer, "int** d_offset;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "int** d_edges;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "int** d_weight;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "int** d_src;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "int** d_rev_meta;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "d_offset = (int**) malloc(devicecount*sizeof(int*));");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "d_edges = (int**) malloc(devicecount*sizeof(int*));");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "d_weight = (int**) malloc(devicecount*sizeof(int*));");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "d_src = (int**) malloc(devicecount*sizeof(int*));");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "d_rev_meta = (int**) malloc(devicecount*sizeof(int*));");
    main.pushstr_newL(strBuffer);


    // TC values counted on each of the GPU
    // sprintf(strBuffer, "int *triangle_count[devicecount];");
    // main.pushstr_newL(strBuffer);
    // sprintf(strBuffer, "int* d_src;");
    // main.pushstr_newL(strBuffer);
    // sprintf(strBuffer, "int* d_rev_meta;");
    // main.pushstr_newL(strBuffer);
    // sprintf(strBuffer, "bool* d_modified_next;");
    // main.pushstr_newL(strBuffer);
    // main.NewLine();

    // generateCudaMallocStr("d_meta", "int", "(1+V)");
    // generateCudaMallocStr("d_data", "int", "(E)");

    main.NewLine();

    // Below two lines are same for every multi-GPU backend hence Hardcoded..
    // main.pushstr_newL("int perdevicevertices;");
    // main.pushstr_newL("int lastleftvertices;");
    // main.pushstr_newL("perdevicevertices = V / devicecount ;");
    // main.pushstr_newL("lastleftvertices = V \% devicecount;");


    main.pushstr_newL("for(int i=0;i<devicecount;i++)	");
    main.pushstr_newL("{");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("cudaMalloc(&d_offset[i], (V+1)*sizeof(int) );");
    main.pushstr_newL("cudaMalloc(&d_edges[i], (E)*sizeof(int) );");
    main.pushstr_newL("cudaMalloc(&d_weight[i], (E)*sizeof(int) );");
    main.pushstr_newL("cudaMalloc(&d_src[i], (E)*sizeof(int) );");
    main.pushstr_newL("cudaMalloc(&d_rev_meta[i], (V+1)*sizeof(int) );");
    main.pushstr_newL("}");

    main.pushstr_newL("for(int i=0;i<devicecount;i++)	");
    main.pushstr_newL("{");
    main.pushstr_newL("cudaSetDevice(i);");
    // main.pushstr_newL("cudaMalloc(d_offset[i], (V+2)*sizeof(int) );");
    // main.pushstr_newL("cudaMalloc(d_edges[i], (E)*sizeof(int) );");
    main.pushstr_newL("cudaMemcpyAsync(d_offset[i], h_offset, (V+1)*sizeof(int),cudaMemcpyHostToDevice);");
    main.pushstr_newL("cudaMemcpyAsync(d_edges[i], h_edges, (E)*sizeof(int),cudaMemcpyHostToDevice);");
    main.pushstr_newL("cudaMemcpyAsync(d_weight[i], h_weight, (E)*sizeof(int),cudaMemcpyHostToDevice );");
    main.pushstr_newL("cudaMemcpyAsync(d_src[i], h_src, (E)*sizeof(int),cudaMemcpyHostToDevice );");
    main.pushstr_newL("cudaMemcpyAsync(d_rev_meta[i], h_rev_meta, (V+1)*sizeof(int),cudaMemcpyHostToDevice );");
    main.pushstr_newL("}");

    main.pushstr_newL("for(int i=0;i<devicecount;i++)	");
    main.pushstr_newL("{");
    main.pushstr_newL("cudaSetDevice(i);");
    main.pushstr_newL("cudaDeviceSynchronize();");
    main.pushstr_newL("}");

    // generateCudaMemCpyStr("d_src", "h_src", "int", "E");
    // generateCudaMemCpyStr("d_weight", "h_weight", "int", "E");
    // generateCudaMemCpyStr("d_rev_meta", "h_rev_meta", "int", "(V+1)");
    // main.NewLine();

    // main.pushstr_newL("// CSR END");

    // main.pushString("//LAUNCH CONFIG");

    generateLaunchConfig("nodes"); // Most of the time unless we perform edgeBased!

    main.NewLine();

    main.pushstr_newL("// TIMER START");
    generateStartTimer();
  }
}

void dsl_cpp_generator::generateFuncHeader(Function *proc, bool isMainFile)
{
  dslCodePad &targetFile = isMainFile ? main : header;
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
  list<formalParam *> paramList = proc->getParamList();
  list<formalParam *>::iterator itr;

  //~ bool genCSR = false;// to generate csr  main.cpp file if g is a param ! var moved to .h file. May not be need. refractor later!
  //~ char* gId;// to generate csr  main.cpp file if g is a param
  for (itr = paramList.begin(); itr != paramList.end(); itr++)
  {
    arg_currNo++;
    argumentTotal--;

    Type *type = (*itr)->getType();
    targetFile.pushString(convertToCppType(type));
    char *parName = (*itr)->getIdentifier()->getIdentifier();

    targetFile.pushString(" ");
    char str[80];
    strcpy(str, "d_");
    strcat(str, (*itr)->getIdentifier()->getIdentifier());
    //~ strcat (str, '\0');
    // added here
    cout << "param :" << parName << endl;
    cout << "paramD:" << str << endl;

    if (!isMainFile)
    {
      //~ vars *vv = new vars;
      //~ vv->varType="int*";
      //~ vv->varName=str   ;
      //~ vv->result =false ;
      //~ varList.push_back(*vv);
      //~ varList.push_back({"double *", str,false});
      //~ Identifier* id = (*itr)->getIdentifier();
      if (type->isGraphType())
      {
        std::cout << "========== SET TRUE" << '\n';
        genCSR = true;
        //~ gId = parName;
      }

      // varList.push_back(id); cout << "PUSHED =======> " << parName << endl;
      //~ varList.push_back({"int*","d_meta", false})  ;
      //~ varList.push_back({"int*","d_data", false})  ;
      //~ varList.push_back({"int*","d_weight", false});
    }
    //~ if(!type->isGraphType() && isMainFile){
    //~ varList.push_back({convertToCppType(type),parName, false});
    //~ }

    targetFile.pushString(/*createParamName(*/ parName);
    if (argumentTotal > 0)
      targetFile.pushString(",");

    if (arg_currNo == maximum_arginline)
    {
      targetFile.NewLine();
      arg_currNo = 0;
    }
  }

  targetFile.pushString(")");

  if (!isMainFile)
  {
    targetFile.pushString(";");
    targetFile.NewLine();
    targetFile.NewLine();

    for (itr = paramList.begin(); itr != paramList.end(); itr++)
    {
      Type *type = (*itr)->getType();
      char *parName = (*itr)->getIdentifier()->getIdentifier();

      if (type->isPrimitiveType())
      {
        char strBuffer[1024];

        // sprintf(strBuffer, "__device__ %s %s ;", convertToCppType(type), parName);
        // targetFile.pushString(strBuffer);
        targetFile.NewLine();
      }
    }
  }
  //~ if (!isMainFile) {
  //~ targetFile.NewLine();
  //~ targetFile.pushString("{");
  //~ }

  targetFile.NewLine();
  targetFile.NewLine();

  return;
}

bool dsl_cpp_generator::openFileforOutput()
{
  char temp[1024];
  printf("fileName %s\n", fileName);
  sprintf(temp, "%s/%s.h", "../graphcode/generated_multigpu", fileName);
  headerFile = fopen(temp, "w");
  if (headerFile == NULL)
    return false;
  header.setOutputFile(headerFile);

  sprintf(temp, "%s/%s.cu", "../graphcode/generated_multigpu", fileName);
  bodyFile = fopen(temp, "w");
  if (bodyFile == NULL)
    return false;
  main.setOutputFile(bodyFile);

  return true;
}
void dsl_cpp_generator::generation_end()
{
  header.NewLine();
  header.pushstr_newL("#endif");
}

void dsl_cpp_generator::closeOutputFile()
{
  if (headerFile != NULL)
  {
    header.outputToFile();
    fclose(headerFile);
  }
  headerFile = NULL;

  if (bodyFile != NULL)
  {
    main.outputToFile();
    fclose(bodyFile);
  }

  bodyFile = NULL;
}

void dsl_cpp_generator::setCurrentFunc(Function *func)
{
  currentFunc = func;
}

Function *dsl_cpp_generator::getCurrentFunc()
{
  return currentFunc;
}

bool dsl_cpp_generator::generate()
{
  // cout<<"FRONTEND
  // VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();
  // //openFileforOutput();
  if (!openFileforOutput())
    return false;
  generation_begin();

  list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
  {
    setCurrentFunc(func);
    generateFunc(func);
  }

  generation_end();

  closeOutputFile();

  return true;
}

void dsl_cpp_generator::setFileName(
    char *f) // to be changed to make it more universal.
{
  char *token = strtok(f, "/");
  char *prevtoken;

  while (token != NULL)
  {
    prevtoken = token;
    token = strtok(NULL, "/");
  }
  fileName = prevtoken;
}
}

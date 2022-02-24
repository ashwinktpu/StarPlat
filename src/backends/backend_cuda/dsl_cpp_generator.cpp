#include <string.h>
#include <cassert>
#include "dsl_cpp_generator.h"

void dsl_cpp_generator::generateInitkernel(const char* name) {
  char strBuffer[1024];
  header.pushstr_newL("template <typename T>");
  sprintf(strBuffer,
          "__global__ void initKernel(unsigned %s, T* d_array, T initVal) {",
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
void dsl_cpp_generator::generateInitkernelStr(const char* inVarType, const char* inVarName, const char* initVal) {
  char strBuffer[1024];
  sprintf(strBuffer, "initKernel<%s> <<<numBlocks,numThreads>>>(V,%s,%s);", inVarType, inVarName, initVal);
  main.pushstr_newL(strBuffer);
}
void dsl_cpp_generator::generateInitkernel1(
    assignment* assign, bool isMainFile) {  // const char* typ,
  //~ initKernel<double> <<<numBlocks,numThreads>>>(V,d_BC, 0.0);
  char strBuffer[1024];

  Identifier* inId = assign->getId();
  Expression* exprAssigned = assign->getExpr();

  const char* inVarType =
      convertToCppType(inId->getSymbolInfo()->getType()->getInnerTargetType());
  const char* inVarName = inId->getIdentifier();

  sprintf(strBuffer, "initKernel<%s> <<<numBlocks,numThreads>>>(V,d_%s,",
          inVarType, inVarName);
  main.pushString(strBuffer);

  std::cout << "varName:" << inVarName << '\n';
  generateExpr(
      exprAssigned,
      isMainFile);  // asssuming int/float const literal // OUTPUTS INIT VALUE

  main.pushstr_newL(");");
  main.NewLine();
}

void dsl_cpp_generator::generateLaunchConfig(const char* name) {
  //~ const unsigned threadsPerBlock = 512;                                  //
  //~ unsigned numThreads   = V < threadsPerBlock ? V: 512;                  //
  //LAUNCH CONFIG ~ unsigned numBlocks    =
  //(numThreads+threadsPerBlock-1)/threadsPerBlock;//

  char strBuffer[1024];
  main.NewLine();
  const unsigned threadsPerBlock = 512;
  const char* totalThreads = (strcmp(name, "nodes") == 0) ? "V" : "E";
  sprintf(strBuffer, "const unsigned threadsPerBlock = %u;", threadsPerBlock);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer, "unsigned numThreads   = (%s < threadsPerBlock)? %s: %u;",
          totalThreads, totalThreads, threadsPerBlock);
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,
          "unsigned numBlocks    = "
          "(numThreads+threadsPerBlock-1)/threadsPerBlock;");
  main.pushstr_newL(strBuffer);
  main.NewLine();
  // main.pushstr_newL("}");
}

void dsl_cpp_generator::generateCudaMemCpyStr(const char* sVarName,
                                              const char* tVarName,
                                              const char* type,
                                              const char* sizeV,
                                              bool isH2D = true) {
  //~ cudaMemcpy(  d_data,   h_data, sizeof(int)*(   E),
  //cudaMemcpyHostToDevice);
  //                1         2               3       4       5
  char strBuffer[1024];
  sprintf(strBuffer, "cudaMemcpy(%8s, %8s, sizeof(%3s)*(%s), %s);", sVarName,
          tVarName, type, sizeV,
          (isH2D ? "cudaMemcpyHostToDevice" : "cudaMemcpyDeviceToHost"));
  main.pushstr_newL(strBuffer);
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

void dsl_cpp_generator::generation_begin() {
  char temp[1024];
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

  header.pushstr_newL("#include <cooperative_groups.h>");
  //~ header.pushstr_newL("#include <cooperative_groups.h>");
  header.NewLine();

  main.pushString("#include ");
  sprintf(temp, "%s.h", fileName);
  addIncludeToFile(temp, main, false);
  main.NewLine();
}

void add_InitialDeclarations(dslCodePad* main, iterateBFS* bfsAbstraction) {
  char strBuffer[1024];
  char* graphId = bfsAbstraction->getGraphCandidate()->getIdentifier();
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

void add_BFSIterationLoop(dslCodePad* main, iterateBFS* bfsAbstraction) {
  char strBuffer[1024];
  char* iterNode = bfsAbstraction->getIteratorNode()->getIdentifier();
  char* graphId = bfsAbstraction->getGraphCandidate()->getIdentifier();
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
void add_RBFSIterationLoop(dslCodePad* main, iterateBFS* bfsAbstraction) {
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
void dsl_cpp_generator::addCudaRevBFSIterationLoop(iterateBFS* bfsAbstraction) {
  main.NewLine();
  main.pushstr_newL("hops_from_source--;");
  generateCudaMemCpyStr("d_hops_from_source", "&hops_from_source", "int", "1");
  main.NewLine();

  //~ main.pushstr_newL("{");
  //~ main.pushstr_newL("#pragma omp parallel for");
  //~ sprintf(strBuffer,"for( %s %s = %s; %s < levelCount[phase] ;
  //%s++)","int","l","0","l","l"); ~ main.pushstr_newL(strBuffer); ~
  //main.pushstr_newL("{"); ~ sprintf(strBuffer,"int %s = levelNodes[phase][l]
  //;",bfsAbstraction->getIteratorNode()->getIdentifier()); ~
  //main.pushstr_newL(strBuffer);
}
void dsl_cpp_generator::addCudaRevBFSIterKernel(list<statement*>& statementList) {
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
  const char* loopVar = "v";
  //~ const char* nbbrVar = "w";
  char strBuffer[1024];

  //~ statement* body = bfsAbstraction->getBody();
  //~ assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  //~ blockStatement* block = (blockStatement*)body;
  //~ list<statement*> statementList = block->returnStatements();
  sprintf(strBuffer, "__global__ void back_pass(int n, int* d_meta,int* d_data,int* d_weight, double* d_delta, double* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished, double* d_BC) {");
  header.pushstr_newL(strBuffer);

  sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
  header.pushstr_newL(strBuffer);

  sprintf(strBuffer, "if(%s >= n) return;", loopVar);
  header.pushstr_newL(strBuffer);
  header.pushstr_newL("auto grid = cooperative_groups::this_grid();");

  sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source-1) {", loopVar);
  header.pushstr_newL(strBuffer);

  for (statement* stmt : statementList) {
    generateStatement(stmt, false);  //false. All these stmts should be inside kernel
  }

  header.pushstr_newL("} // end if d lvl");
  header.pushstr_newL("} // kernel end");
}
void dsl_cpp_generator::addCudaBFSIterKernel(iterateBFS* bfsAbstraction) {
  //~ var v
  //~ var w
  //~ __global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, double* d_delta, double* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished, double* d_bc) {
  //~ unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  //~ if(v >= n) return;
  //~ if(d_level[v] == *d_hops_from_source) {
  //~ for(unsigned i = d_meta[v], end = d_meta[v+1]; i < end; ++i) {
  //~ unsigned w = d_data[i];
  //~ if(d_level[w] == -1) {
  //~ d_level[w] = *d_hops_from_source + 1;
  //~ *d_finished = false;
  //~ }
  //~ if(d_level[w] == *d_hops_from_source + 1) {
  //~ atomicAdd(&d_sigma[w], d_sigma[v]);             // GENERATES
  //~ }
  //~ }
  //~ }
  //~ }
  const char* loopVar = "v";
  //~ const char* nbbrVar = "w";
  char strBuffer[1024];

  statement* body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement* block = (blockStatement*)body;
  list<statement*> statementList = block->returnStatements();

  header.pushstr_newL("__global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, double* d_delta, double* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished, double* d_BC) {");

  sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
  header.pushstr_newL(strBuffer);

  sprintf(strBuffer, "if(%s >= n) return;", loopVar);
  header.pushstr_newL(strBuffer);
  sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source) {", loopVar);
  header.pushstr_newL(strBuffer);

  for (statement* stmt : statementList) {
    generateStatement(stmt, false);  //false. All these stmts should be inside kernel
                                     //~ if (stmt->getTypeofNode() == NODE_FORALLSTMT) {

    //~ }
    //~ else if (stmt->getTypeofNode() == NODE_ASSIGN) {
    //~ // generateAssignmentStmt((assignment*)stmt,false,true);
    //~ generateDeviceAssignmentStmt((assignment*)stmt,false);
    //~ }
  }

  header.pushstr_newL("} // end if d lvl");
  header.pushstr_newL("} // kernel end");
}
void dsl_cpp_generator::addCudaBFSIterationLoop(iterateBFS* bfsAbstraction) {
  //char strBuffer[1024];
  //~ char* iterNode = bfsAbstraction->getIteratorNode()
  //~ ->getIdentifier();  // v in g.nodes from src
  //~ char* graphId = bfsAbstraction->getGraphCandidate()->getIdentifier();

  //~ generateInitkernel1(

  main.pushstr_newL("finished = true;");  // there vars are BFS specific
  generateCudaMemCpyStr("d_finished", "&finished", "bool", "1");

  //~ main.pushstr_newL("bfsCount = 0 ;");
  //~ main.pushstr_newL("#pragma omp parallel for");
  //~ sprintf(strBuffer,"for( %s %s = %s; %s < prev_count ;
  //%s++)","int","l","0","l","l"); ~ main.pushstr_newL(strBuffer); ~
  //main.pushstr_newL("{"); ~ sprintf(strBuffer,"int %s = levelNodes[phase][%s]
  //;",iterNode,"l"); ~ main.pushstr_newL(strBuffer); ~
  //sprintf(strBuffer,"for(%s %s = %s.%s[%s] ; %s < %s.%s[%s+1] ; %s++)
  //","int","edge",graphId,"indexofNodes",iterNode,"edge",graphId,"indexofNodes",iterNode,"edge");
  //~ main.pushString(strBuffer);
  //~ main.pushstr_newL("{");
  //~ sprintf(strBuffer,"%s %s = %s.%s[%s]
  //;","int","nbr",graphId,"edgeList","edge"); ~ main.pushstr_newL(strBuffer);
  //~ main.pushstr_newL("int dnbr ;");
  //~ main.pushstr_newL("if(bfsDist[nbr]<0)");
  //~ main.pushstr_newL("{");
  //~ sprintf(strBuffer,"dnbr =
  //%s(&bfsDist[nbr],-1,bfsDist[%s]+1);","__sync_val_compare_and_swap",iterNode);
  //~ main.pushstr_newL(strBuffer);
  //~ main.pushstr_newL("if (dnbr < 0)");
  //~ main.pushstr_newL("{");
  //~ sprintf(strBuffer,"%s %s =
  //%s();","int","num_thread","omp_get_thread_num"); ~ //sprintf(strBuffer,"int
  //%s = bfsCount.fetch_add(%s,%s) ;","loc","1","std::memory_order_relaxed"); ~
  //main.pushstr_newL(strBuffer); ~ sprintf(strBuffer,"
  //levelNodes_later[%s].push_back(%s) ;","num_thread","nbr"); ~
  //main.pushstr_newL(strBuffer); ~ main.pushstr_newL("}"); ~
  //main.pushstr_newL("}");
  main.NewLine();
  main.pushstr_newL("//Kernel LAUNCH");
  main.pushstr_newL("fwd_pass<<<numBlocks,numThreads>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished, d_BC); ///TODO from varList");
  main.NewLine();

  addCudaBFSIterKernel(bfsAbstraction);  // KERNEL BODY!!!

  //~ main.pushstr_newL("cudaDeviceSynchronize(); //MUST - rupesh");
  main.pushstr_newL("incrementDeviceVar<<<1,1>>>(d_hops_from_source);");
  main.pushstr_newL("cudaDeviceSynchronize(); //MUST - rupesh");
  main.pushstr_newL("++hops_from_source; // updating the level to process in the next iteration");
  main.pushstr_newL("// k++; //DEBUG");
}

void dsl_cpp_generator::generateRevBFSAbstraction(iterateBFS* bfsAbstraction,
                                                  bool isMainFile) {
  //~ char strBuffer[1024];
  //~ add_InitialDeclarations(&main,bfsAbstraction);
  // printf("BFS ON GRAPH
  // %s",bfsAbstraction->getGraphCandidate()->getIdentifier()); ~
  //add_BFSIterationLoop(&main,bfsAbstraction);

  statement* body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement* block = (blockStatement*)body;
  list<statement*> statementList = block->returnStatements();

  //~ addCudaRevBFSIterationLoop(bfsAbstraction);

  for (statement* stmt : statementList) {
    generateStatement(stmt, isMainFile);  //false. All these stmts should be inside kernel
  }
  //~ main.pushstr_newL("}");
}

/// MAIN CUDA ITER BFS GENERATION!
void dsl_cpp_generator::generateBFSAbstraction(iterateBFS* bfsAbstraction,
                                               bool isMainFile) {
  char strBuffer[1024];
  //~ add_InitialDeclarations(&main,bfsAbstraction);
  // printf("BFS ON GRAPH
  // %s",bfsAbstraction->getGraphCandidate()->getIdentifier()); ~
  //add_BFSIterationLoop(&main,bfsAbstraction);

  statement* body = bfsAbstraction->getBody();
  assert(body->getTypeofNode() == NODE_BLOCKSTMT);
  blockStatement* block = (blockStatement*)body;
  list<statement*> statementList = block->returnStatements();

  //~ bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  //~ int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));
  //~ int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

  //~ generateExtraVariable( "bool","d_finished","1");
  //~ generateExtraVariable( "int","d_hops_from_source","1");
  //~ generateExtraVariable( "int","d_level","V");

  main.NewLine();
  main.pushstr_newL("//EXTRA vars for ITBFS AND REVBFS");  //NOT IN DSL so hardcode is fine
  main.pushstr_newL("bool finished;");
  main.pushstr_newL("int hops_from_source=0;");

  main.pushstr_newL("bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));");
  main.pushstr_newL("int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));  cudaMemset(d_hops_from_source,0,sizeof(int));");
  main.pushstr_newL("int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));");

  main.NewLine();
  main.pushstr_newL("//EXTRA vars INITIALIZATION");

  generateInitkernelStr("int", "d_level", "-1");

  sprintf(strBuffer, "initIndex<int><<<1,1>>>(V,d_level,%s, 0);", bfsAbstraction->getRootNode()->getIdentifier());
  main.pushstr_newL(strBuffer);
  main.NewLine();

  main.pushstr_newL("// long k =0 ;// For DEBUG");
  main.pushstr_newL("do {");

  addCudaBFSIterationLoop(bfsAbstraction);  // ADDS BODY OF ITERBFS + KERNEL LAUNCH

  main.NewLine();

  //~ for (statement* stmt : statementList) {
  //~ generateStatement(stmt, false);
  //~ }

  generateCudaMemCpyStr("&finished", "d_finished", "bool", "1", false);

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

  blockStatement*
      revBlock = (blockStatement*)bfsAbstraction->getRBFS()->getBody();
  list<statement*> revStmtList = revBlock->returnStatements();
  addCudaRevBFSIterationLoop(bfsAbstraction);

  main.pushstr_newL("//BACKWARD PASS");
  main.pushstr_newL("while(hops_from_source > 1) {");

  main.NewLine();
  main.pushstr_newL("//KERNEL Launch");
  main.pushstr_newL("back_pass<<<numBlocks,numThreads>>>(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished, d_BC); ///TODO from varList");  ///TODO get all propnodes from function body and params

  main.NewLine();
  addCudaRevBFSIterKernel(revStmtList);  // KERNEL BODY

  //~ for(statement* stmt:revStmtList) {
  //~ generateStatement(stmt, false);
  //~ }

  main.pushstr_newL("hops_from_source--;");
  generateCudaMemCpyStr("d_hops_from_source", "&hops_from_source", "int", "1", true);

  main.pushstr_newL("}");
  main.pushstr_newL("//accumulate_bc<<<numBlocks,numThreads>>>(V,d_delta, d_BC, d_level, src);");
  //~ main.NewLine();

  //~ main.pushstr_newL("}while(!finished);");
  //~ main.pushstr_newL("}");
  //~ main.pushstr_newL("phase = phase - 1 ;");
  //~ main.pushstr_newL("}");
}

void dsl_cpp_generator::generateStatement(statement* stmt, bool isMainFile) {
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

    //~ generateAssignmentStmt((assignment*)stmt,
    //~ isMainFile);  // Change "true" to later ; currently
    //~ // one asst in forall/kernel
  }

  if (stmt->getTypeofNode() == NODE_WHILESTMT) {
    // generateWhileStmt((whileStmt*) stmt);
  }

  if (stmt->getTypeofNode() == NODE_IFSTMT) {
    //~ cout << "I am insie if block "
    //~ "pppppppppppppppppppppppppppppppppppppppppppppppp="
    //~ << isMainFile;

    generateIfStmt((ifStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_DOWHILESTMT) {
    generateDoWhileStmt((dowhileStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
    std::cout << "STMT: For" << '\n';
    generateForAll((forallStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) {
    generateFixedPoint((fixedPointStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
    //~ cout << "IS REDUCTION STMT HI"
    //~ << "\n";
    generateReductionStmt((reductionCallStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_ITRBFS) {
    generateBFSAbstraction((iterateBFS*)stmt, isMainFile);
  }
  //~ if(stmt->getTypeofNode()==NODE_ITRRBFS) //REVERSE
  //~ {
  //~ cout << "\nI am inside REV BLK
  //pppppppppppppppppppppppppppppppppppppppppppppppp=\n"; ~
  //generateRevBFSAbstraction((iterateBFS*) stmt, isMainFile);
  //~ }
  if (stmt->getTypeofNode() == NODE_PROCCALLSTMT) {
    generateProcCall((proc_callStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
    unary_stmt* unaryStmt = (unary_stmt*)stmt;
    generateExpr(unaryStmt->getUnaryExpr(), isMainFile);
    main.pushstr_newL(";");
  }
}

void dsl_cpp_generator::generateAtomicBlock(bool isMainFile) {
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  main.pushstr_newL("atomicMin(&gpu_dist[nbr] , dist_new);");
  main.pushstr_newL("gpu_modified_next[nbr]=true;");
  main.pushstr_newL("gpu_finished[0] = false;");
}

void dsl_cpp_generator::generateReductionCallStmt(reductionCallStmt* stmt,
                                                  bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  reductionCall* reduceCall = stmt->getReducCall();
  char strBuffer[1024];
  if (reduceCall->getReductionType() == REDUCE_MIN) {
    if (stmt->isListInvolved()) {
      //~ cout << "INSIDE THIS OF LIST PRESENT"
      //~ << "\n";
      list<argument*> argList = reduceCall->getargList();
      list<ASTNode*> leftList = stmt->getLeftList();
      //~ int i = 0;
      list<ASTNode*> rightList = stmt->getRightList();
      printf("LEFT LIST SIZE %lu \n", leftList.size());

      targetFile.space();
      if (stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType()) {
        Type* type = stmt->getAssignedId()->getSymbolInfo()->getType();

        targetFile.pushstr_space(convertToCppType(type->getInnerTargetType()));
      }
      sprintf(strBuffer, "%s_new", stmt->getAssignedId()->getIdentifier());
      targetFile.pushString(strBuffer);
      list<argument*>::iterator argItr;
      argItr = argList.begin();
      argItr++;
      targetFile.pushString(" = ");
      generateExpr((*argItr)->getExpr(), isMainFile);
      targetFile.pushstr_newL(";");
      list<ASTNode*>::iterator itr1;
      list<ASTNode*>::iterator itr2;
      itr2 = rightList.begin();
      itr1 = leftList.begin();
      itr1++;
      for (; itr1 != leftList.end(); itr1++) {
        ASTNode* node = *itr1;
        ASTNode* node1 = *itr2;

        if (node->getTypeofNode() == NODE_ID) {
          targetFile.pushstr_space(convertToCppType(
              ((Identifier*)node)->getSymbolInfo()->getType()));
          sprintf(strBuffer, "%s_new", ((Identifier*)node)->getIdentifier());
          targetFile.pushString(strBuffer);
          targetFile.pushString(" = ");
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

      sprintf(strBuffer, "%s[id]!= MAX_VAL && ",
              stmt->getAssignedId()->getIdentifier());
      targetFile.pushString(strBuffer);
      generate_exprPropId(stmt->getTargetPropId(), isMainFile);

      sprintf(strBuffer, " > %s_new)", stmt->getAssignedId()->getIdentifier());
      targetFile.pushstr_newL(strBuffer);
      targetFile.pushstr_newL("{");  // added for testing then doing atomic min.
      /* storing the old value before doing a atomic operation on the node
       * property */
      if (stmt->isTargetId()) {
        Identifier* targetId = stmt->getTargetId();
        targetFile.pushstr_space(
            convertToCppType(targetId->getSymbolInfo()->getType()));
        targetFile.pushstr_space("oldValue");
        targetFile.pushstr_space("=");
        generate_exprIdentifier(stmt->getTargetId(), isMainFile);
        targetFile.pushstr_newL(";");
      } else {
        PropAccess* targetProp = stmt->getTargetPropId();
        Type* type = targetProp->getIdentifier2()->getSymbolInfo()->getType();
        if (type->isPropType()) {
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

      itr1 = leftList.begin();
      itr1++;
      for (; itr1 != leftList.end(); itr1++) {
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
          sprintf(strBuffer, "%s[0] = %s ;", fpId, "false");
          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("}");  // needs to be removed
          targetFile.pushstr_newL("}");  // needs to be removed
        }
      }
      // targetFile.pushstr_newL("}");
      targetFile.pushstr_newL(
          "}");  // added for testing condition..then atomicmin.
    }
  }
}

void dsl_cpp_generator::generateReductionOpStmt(reductionCallStmt* stmt,
                                                bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  if (stmt->isLeftIdentifier()) {
    Identifier* id = stmt->getLeftId();
    targetFile.pushString(id->getIdentifier());
    targetFile.pushString(" = ");
    targetFile.pushString(id->getIdentifier());
    const char* operatorString = getOperatorString(stmt->reduction_op());
    targetFile.pushstr_space(operatorString);
    generateExpr(stmt->getRightSide(), isMainFile);
    targetFile.pushstr_newL(";");

  } else {
    generate_exprPropId(stmt->getPropAccess(), isMainFile);
    targetFile.pushString(" = ");
    generate_exprPropId(stmt->getPropAccess(), isMainFile);
    const char* operatorString = getOperatorString(stmt->reduction_op());
    targetFile.pushstr_space(operatorString);
    generateExpr(stmt->getRightSide(), isMainFile);
    targetFile.pushstr_newL(";");
  }
}

void dsl_cpp_generator::generateReductionStmt(reductionCallStmt* stmt,
                                              bool isMainFile) {
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  //~ char strBuffer[1024];

  if (stmt->is_reducCall()) {
    generateReductionCallStmt(stmt, isMainFile);
  } else {
    generateReductionOpStmt(stmt, isMainFile);
  }
}

void dsl_cpp_generator::generateDoWhileStmt(dowhileStmt* doWhile,
                                            bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  targetFile.pushstr_newL("do");
  generateStatement(doWhile->getBody(), isMainFile);
  targetFile.pushString("while(");
  generateExpr(doWhile->getCondition(), isMainFile);
  targetFile.pushString(");");
}

void dsl_cpp_generator::generateIfStmt(ifStmt* ifstmt, bool isMainFile) {
  //~ cout
  //~ << "inside the if statment lllllllllllllllllllllllllllllllllllllllllllll="
  //~ << isMainFile;
  dslCodePad& targetFile = isMainFile ? main : header;

  //~ cout << "INSIDE IF STMT"
  //~ << "\n";
  Expression* condition = ifstmt->getCondition();
  targetFile.pushString("if (");
  //~ cout << "TYPE OF IFSTMT" << condition->getTypeofExpr() << "\n";
  generateExpr(condition, isMainFile);
  targetFile.pushString(" )");
  generateStatement(ifstmt->getIfBody(), isMainFile);
  if (ifstmt->getElseBody() == NULL) return;
  targetFile.pushstr_newL("else");
  generateStatement(ifstmt->getElseBody(), isMainFile);
}

void dsl_cpp_generator::findTargetGraph(vector<Identifier*> graphTypes,
                                        Type* type) {
  if (graphTypes.size() > 1) {
    //~ cerr << "TargetGraph can't match";
  } else {
    Identifier* graphId = graphTypes[0];

    type->setTargetGraph(graphId);
  }
}

void dsl_cpp_generator::generateAssignmentStmt(assignment* asmt, bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;
  //~ bool isAtomic = false;
  std::cout << "\tGenerate ASST STMT" << '\n';
  if (asmt->lhs_isIdentifier()) {
    Identifier* id = asmt->getId();
    //~ if(prefixNeed)
    //~ targetFile.pushString("d_");
    targetFile.pushString(id->getIdentifier());
  } else if (asmt->lhs_isProp())  // the check for node and edge property to be
                                  // carried out.
  {
    PropAccess* propId = asmt->getPropId();
    if (asmt->getAtomicSignal()) {
      if (asmt->getParent()->getParent()->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
        if (asmt->getExpr()->isArithmetic() || asmt->getExpr()->isLogical()) {
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
  targetFile.pushString(" = ");
  generateExpr(asmt->getExpr(), isMainFile);
  targetFile.pushstr_newL(";");
}
void dsl_cpp_generator::generateAtomicDeviceAssignmentStmt(assignment* asmt,
                                                           bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;
  bool isAtomic = false;
  bool isResult = false;
  std::cout << "\tASST\n";
  if (asmt->lhs_isIdentifier()) {
    Identifier* id = asmt->getId();

    targetFile.pushString(id->getIdentifier());
  } else if (asmt->lhs_isProp())  // the check for node and edge property to be
                                  // carried out.
  {
    PropAccess* propId = asmt->getPropId();
    if (asmt->isDeviceAssignment()) {
      std::cout << "\t  DEVICE ASST" << '\n';
    }
    if (asmt->getAtomicSignal()) {
      targetFile.pushString("atomicAdd(&");
      isAtomic = true;
      std::cout << "\t  ATOMIC ASST" << '\n';
    }
    if (asmt->isAccumulateKernel()) {
      isResult = true;
      std::cout << "\t  RESULT BC/2.0 ASST" << '\n';
    }
    targetFile.pushString("d_");  /// IMPORTANT
    targetFile.pushString(propId->getIdentifier2()->getIdentifier());
    targetFile.push('[');
    targetFile.pushString(propId->getIdentifier1()->getIdentifier());
    targetFile.push(']');
  }

  if (isAtomic)
    targetFile.pushString(", ");
  else
    targetFile.pushString(" = ");

  //~ std::cout<< "------>BEG EXP"  << '\n';
  generateExpr(asmt->getExpr(), isMainFile, isAtomic);
  //~ std::cout<< "------>END EXP"  << '\n';

  if (isAtomic)
    targetFile.pushstr_newL(");");
  else if (isResult)
    targetFile.pushstr_newL("/2.0;");
  else
    targetFile.pushstr_newL(";");
}
void dsl_cpp_generator::generateDeviceAssignmentStmt(assignment* asmt,
                                                     bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;
  bool isDevice = false;
  std::cout << "\tASST \n";
  char strBuffer[300];
  if (asmt->lhs_isIdentifier()) {
    Identifier* id = asmt->getId();

    targetFile.pushString(id->getIdentifier());
  } else if (asmt->lhs_isProp())  // the check for node and edge property to be
                                  // carried out.
  {
    PropAccess* propId = asmt->getPropId();

    if (asmt->isDeviceAssignment()) {
      isDevice = true;
      //~ src.dist = 0; ===>  initIndex<int><<<1,1>>>(V,d_dist,src, 0);
      //                                  1              2     3   4
      sprintf(strBuffer, "initIndex<double><<<1,1>>>(V,d_%s,%s,",  //get the type from id
              propId->getIdentifier2()->getIdentifier(),
              propId->getIdentifier1()->getIdentifier());
      std::cout << "\tDEVICE ASST" << '\n';

      //~ Type *typeB = propId->getIdentifier2()->getSymbolInfo()->getType();
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
      targetFile.pushString(strBuffer);
    } else {
      targetFile.pushString("d_");  /// IMPORTANT
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

  if (isDevice)
    targetFile.pushstr_newL(");");
  else
    targetFile.pushstr_newL(";");
}

void dsl_cpp_generator::generateProcCall(proc_callStmt* proc_callStmt,
                                         bool isMainFile) {
  //~ dslCodePad& targetFile = isMainFile ? main : header;
  // cout<<"INSIDE PROCCALL OF GENERATION"<<"\n";
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

        /// initKernel<double> <<<numBlocks,numThreads>>>(V,d_BC, 0);
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

void dsl_cpp_generator::generatePropertyDefination(Type* type, char* Id,
                                                   bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  Type* targetType = type->getInnerTargetType();
  if (targetType->gettypeId() == TYPE_INT) {
    targetFile.pushString("=");
    targetFile.pushString(INTALLOCATION);
    targetFile.pushString("[");
    // printf("%d SIZE OF VECTOR",)
    // findTargetGraph(graphId,type);

    if (graphId.size() > 1) {
      cerr << "TargetGraph can't match";
    } else {
      Identifier* id = graphId[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }
  if (targetType->gettypeId() == TYPE_BOOL) {
    targetFile.pushString("=");
    targetFile.pushString(BOOLALLOCATION);
    targetFile.pushString("[");
    // findTargetGraph(graphId,type);
    if (graphId.size() > 1) {
      cerr << "TargetGraph can't match";
    } else {
      Identifier* id = graphId[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }

  if (targetType->gettypeId() == TYPE_FLOAT) {
    targetFile.pushString("=");
    targetFile.pushString(FLOATALLOCATION);
    targetFile.pushString("[");
    // findTargetGraph(graphId,type);
    if (graphId.size() > 1) {
      cerr << "TargetGraph can't match";
    } else {
      Identifier* id = graphId[0];

      type->setTargetGraph(id);
    }
    char strBuffer[100];
    sprintf(strBuffer, "%s.%s()", type->getTargetGraph()->getIdentifier(),
            "num_nodes");
    targetFile.pushString(strBuffer);
    targetFile.pushString("]");
    targetFile.pushstr_newL(";");
  }

  if (targetType->gettypeId() == TYPE_DOUBLE) {
    targetFile.pushString("=");
    targetFile.pushString(DOUBLEALLOCATION);
    targetFile.pushString("[");
    // findTargetGraph(graphId,type);
    if (graphId.size() > 1) {
      cerr << "TargetGraph can't match";
    } else {
      Identifier* id = graphId[0];

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

void dsl_cpp_generator::getDefaultValueforTypes(int type) {
  switch (type) {
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

void dsl_cpp_generator::generateForAll_header(forallStmt* forAll,
                                              bool isMainFile) {
  //~ cout << "the value of the generateForAll_header = " << isMainFile;
  dslCodePad& targetFile = isMainFile ? main : header;
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

void dsl_cpp_generator::generateForAllSignature(forallStmt* forAll,
                                                bool isMainFile) {
  //~ cout << "from generateForAllSignature = " << isMainFile;
  dslCodePad& targetFile = isMainFile ? main : header;

  char strBuffer[1024];
  //~ Identifier* iterator = forAll->getIterator();
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
      // THIS SHOULD NOT BE EXECUTING FOR SIMPLE FOR LOOP BUT IT IS SO .
      // COMMENTED OUT FOR NOW.
      //~ char* graphId=sourceGraph->getIdentifier();
      //~ char* methodId=iteratorMethodId->getIdentifier();
      //~ string s(methodId);
      //~ if(s.compare("neighbors")==0)
      //~ {
      //~ list<argument*>  argList=extractElemFunc->getArgList();
      //~ assert(argList.size()==1);
      //~ Identifier* nodeNbr=argList.front()->getExpr()->getId();
      //~ sprintf(strBuffer,"for (%s %s = %s[%s]; %s < %s[%s+1]; %s ++)
      //","int","edge","gpu_OA","id","edge","gpu_OA","id","edge"); ~
      //targetFile.pushstr_newL(strBuffer); ~ targetFile.pushString("{"); ~
      //sprintf(strBuffer,"%s %s = gpu_%s[%s]
      //;","int",graphId,"edgeList","edge"); //needs to move the addition of ~
      //targetFile.pushstr_newL(strBuffer); ~ } ~ if(s.compare("nodes_to")==0)
      ////for pageRank ~ { ~ list<argument*>
      //argList=extractElemFunc->getArgList();
      //~ assert(argList.size()==1);
      //~ Identifier* nodeNbr=argList.front()->getExpr()->getId();
      //~ sprintf(strBuffer,"for (%s %s = %s[%s]; %s < %s[%s+1]; %s ++)
      //","int","edge","gpu_rev_OA",nodeNbr->getIdentifier(),"edge","gpu_rev_OA",nodeNbr->getIdentifier(),"edge");
      //~ targetFile.pushstr_newL(strBuffer);
      //~ targetFile.pushString("{");
      //~ sprintf(strBuffer,"%s %s = %s[%s]
      //;","int",iterator->getIdentifier(),"srcList","edge"); //needs to move
      //the addition of ~ targetFile.pushstr_newL(strBuffer); ~ } //statement to
      //a different method.
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
        targetFile.pushstr_newL("//FOR SIGNATURE of SET");
        targetFile.pushstr_newL("std::set<int>::iterator itr;");
        sprintf(strBuffer, "for(itr=%s.begin();itr!=%s.end();itr++)",
                sourceId->getIdentifier(), sourceId->getIdentifier());
        targetFile.pushstr_newL(strBuffer);
      }
    }
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
          PropAccess* propIdNode =
              (PropAccess*)Util::createPropIdNode(iterNode, nodeProp);
          Expression* propIdExpr = Expression::nodeForPropAccess(propIdNode);
          modifiedFilterExpr = (Expression*)Util::createNodeForRelationalExpr(
              propIdExpr, expr2, filterExpr->getOperatorType());
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

void dsl_cpp_generator::generateForAll(forallStmt* forAll, bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;
  //~ cout << "inside the forall the value of ismainfile =" << isMainFile;
  proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
  PropAccess* sourceField = forAll->getPropSource();
  Identifier* sourceId = forAll->getSource();
  // Identifier* extractId;
  Identifier* collectionId;
  if (sourceField != NULL) {
    collectionId = sourceField->getIdentifier1();
  }
  if (sourceId != NULL) {
    collectionId = sourceId;
  }
  Identifier* iteratorMethodId;
  if (extractElemFunc != NULL)
    iteratorMethodId = extractElemFunc->getMethodId();
  statement* body = forAll->getBody();
  char strBuffer[1024];
  if (forAll->isForall()) {
    if (forAll->hasFilterExpr()) {
      Expression* filterExpr = forAll->getfilterExpr();
      Expression* lhs = filterExpr->getLeft();
      Identifier* filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
      TableEntry* tableEntry =
          filterId != NULL ? filterId->getSymbolInfo() : NULL;
      if (tableEntry != NULL && tableEntry->getId()->get_fp_association()) {
        generateForAll_header(forAll, false);
      } else {
        generateForAll_header(forAll, false);
      }

    } else {
      //~ std::cout << "\t\tFFOOOOOORRRRR" << '\n';
      generateForAll_header(forAll, isMainFile);
    }
  }

  generateForAllSignature(forAll, true);  // FOR LINE

  if (forAll->hasFilterExpr()) {
    blockStatement* changedBody = includeIfToBlock(forAll);
    cout << "CHANGED BODY \
    TYPE" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
    forAll->setBody(changedBody);
    // cout<<"FORALL BODY
    // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
  }

  if (extractElemFunc != NULL) {
    if (neighbourIteration(iteratorMethodId->getIdentifier())) {
      cout << "\t ITERATE Neighbour \n";

      //~ char* tmpStr = forAll->getSource()->getIdentifier();
      char* wItr = forAll->getIterator()->getIdentifier();  // w iterator
      std::cout << "src:" << wItr << '\n';
      //~ char* gVar = forAll->getSourceGraph()->getIdentifier();     //g variable
      //~ std::cout<< "G:" << gVar << '\n';
      char* nbrVar;

      if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS) {
        list<argument*> argList = extractElemFunc->getArgList();
        assert(argList.size() == 1);
        Identifier* nodeNbr = argList.front()->getExpr()->getId();
        //~ targetFile.pushstr_newL("FOR begin | nbr iterate"); // ITERATE BFS
        nbrVar = nodeNbr->getIdentifier();
        //~ std::cout<< "nbr?:" <<  nbrVar<< '\n';

        sprintf(strBuffer, "for(unsigned i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
        targetFile.pushstr_newL(strBuffer);

        // HAS ALL THE STMTS IN FOR
        targetFile.pushstr_newL("{");

        sprintf(strBuffer, "unsigned %s = d_data[i];", wItr);
        targetFile.pushstr_newL(strBuffer);

        sprintf(strBuffer, "if(d_level[%s] == -1) {", wItr);
        targetFile.pushstr_newL(strBuffer);
        sprintf(strBuffer, "d_level[%s] = *d_hops_from_source + 1;", wItr);

        targetFile.pushstr_newL(strBuffer);
        targetFile.pushstr_newL("*d_finished = false;");
        targetFile.pushstr_newL("}");

        sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source + 1) {", wItr);
        targetFile.pushstr_newL(strBuffer);

        generateBlock((blockStatement*)forAll->getBody(), false, false);

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

      if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {  // ITERATE REV BFS
        char strBuffer[1024];
        list<argument*> argList = extractElemFunc->getArgList();
        assert(argList.size() == 1);
        Identifier* nodeNbr = argList.front()->getExpr()->getId();
        nbrVar = nodeNbr->getIdentifier();
        std::cout << "V?:" << nbrVar << '\n';
        sprintf(strBuffer, "for(unsigned i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
        targetFile.pushstr_newL(strBuffer);

        // HAS ALL THE STMTS IN FOR
        targetFile.pushstr_newL("{");
        sprintf(strBuffer, "unsigned %s = d_data[i];", wItr);
        targetFile.pushstr_newL(strBuffer);
        sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source) {", wItr);
        targetFile.pushstr_newL(strBuffer);
        generateBlock((blockStatement*)forAll->getBody(), false, false);
        targetFile.pushstr_newL("} // end IF  ");
        targetFile.pushstr_newL("} // end FOR");
        targetFile.pushstr_newL("grid.sync(); // Device-level syncs across all grids. Ref:https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#grid-synchronization-cg ");

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

      //~ if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS ||
      //~ forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
      //~ targetFile.pushstr_newL("}");
      //~ targetFile.pushstr_newL("}");
    } else {
      generateStatement(forAll->getBody(), false);
    }

    if (forAll->isForall() && forAll->hasFilterExpr()) {
      Expression* filterExpr = forAll->getfilterExpr();
      generatefixedpt_filter(filterExpr, false);
    }

  } else {
    if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {  //FOR SET
      if (body->getTypeofNode() == NODE_BLOCKSTMT) {
        targetFile.pushstr_newL("{");
        sprintf(strBuffer, "unsigned %s = (unsigned)*itr;", forAll->getIterator()->getIdentifier());
        targetFile.pushstr_newL(strBuffer);
        generateBlock((blockStatement*)body, false);  //FOR BODY for
        targetFile.pushstr_newL("}");
      } else
        generateStatement(forAll->getBody(), false);

    } else {
      //~ cout << iteratorMethodId->getIdentifier() << "\n";
      generateStatement(forAll->getBody(), false);
    }

    if (forAll->isForall() && forAll->hasFilterExpr()) {
      Expression* filterExpr = forAll->getfilterExpr();
      generatefixedpt_filter(filterExpr, false);
    }
  }
}

void dsl_cpp_generator::generatefixedpt_filter(Expression* filterExpr,
                                               bool isMainFile) {
  //~ dslCodePad& targetFile = isMainFile ? main : header;

  Expression* lhs = filterExpr->getLeft();
  //~ char strBuffer[1024];
  if (lhs->isIdentifierExpr()) {
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

void dsl_cpp_generator::generateVariableDecl(declaration* declStmt,
                                             bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  Type* type = declStmt->getType();
  //~ char strBuffer[1024];

  if (type->isPropType()) {
    if (type->getInnerTargetType()->isPrimitiveType()) {
      Type* innerType = type->getInnerTargetType();
      main.pushString(convertToCppType(
          innerType));  // convertToCppType need to be modified.
      main.pushString("*");
      main.space();
      char str[80];
      strcpy(str, "d_");
      strcat(str, declStmt->getdeclId()->getIdentifier());
      //~ variableList.push_back(std::make_pair(str,str));
      //~ vars v({"int*",str,false});
      //~ v.varType="int";
      //~ v.varName="var";
      //~ v.result =false;
      //~ vList.push_back(new vars());
      main.pushString(str);
      main.pushstr_newL(";");
      //~ cout << "B4 adding: " << str << " Size:" << vvList.size() << '\n';
      //~ vars *v = new vars("int *", str,false);
      //~ varList.push_back(*v);
      //~ vList.emplace_back({"int *", str,false});
      //~ string ss="int";
      //~ vvList.push_back(ss);
      //~ varList.push_back("int");
      //~ cout << "adding: " << str << " Size:" << vvList.size() << '\n';

      //~
      //generatePropertyDefination(type,declStmt->getdeclId()->getIdentifier());
      //// does RHS = new int[V]. as most of cuda vars do not need this.

      generateCudaMalloc(type, declStmt->getdeclId()->getIdentifier());

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

  else if (type->isPrimitiveType()) {
    targetFile.pushstr_space(convertToCppType(type));
    targetFile.pushString(declStmt->getdeclId()->getIdentifier());
    if (declStmt->isInitialized()) {
      targetFile.pushString(" = ");
      /* the following if conditions is for cases where the
         predefined functions are used as initializers
         but the variable's type doesnot match*/
      if (declStmt->getExpressionAssigned()->getExpressionFamily() ==
          EXPR_PROCCALL) {
        proc_callExpr* pExpr =
            (proc_callExpr*)declStmt->getExpressionAssigned();
        Identifier* methodId = pExpr->getMethodId();
        castIfRequired(type, methodId, main);
      }
      generateExpr(declStmt->getExpressionAssigned(), isMainFile);
      targetFile.pushstr_newL(";");
    }

    else {
      targetFile.pushString(" = ");
      getDefaultValueforTypes(type->gettypeId());
      targetFile.pushstr_newL(";");
    }

  }

  else if (type->isNodeEdgeType()) {
    targetFile.pushstr_space(convertToCppType(type));
    targetFile.pushString(declStmt->getdeclId()->getIdentifier());
    if (declStmt->isInitialized()) {
      targetFile.pushString(" = ");
      generateExpr(declStmt->getExpressionAssigned(), isMainFile);
      targetFile.pushstr_newL(";");
    }
  }
}

void dsl_cpp_generator::generate_exprLiteral(Expression* expr,
                                             bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

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
                                              bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

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
    sprintf(valBuffer, "%s",
            expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN");
  }

  targetFile.pushString(valBuffer);
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

void dsl_cpp_generator::generate_exprRelational(Expression* expr,
                                                bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

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
                                                bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  targetFile.pushString(id->getIdentifier());
}

void dsl_cpp_generator::generateExpr(Expression* expr, bool isMainFile, bool isAtomic) {  //isAtomic default to false
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
    //~ std::cout<< "------>PROP PRO CAL"  << '\n';
    generate_exprProcCall(expr, isMainFile);
  } else if (expr->isUnary()) {
    //~ std::cout<< "------>PROP UNARY"  << '\n';
    generate_exprUnary(expr, isMainFile);
  } else {
    assert(false);
  }
}

void dsl_cpp_generator::generate_exprArL(Expression* expr, bool isMainFile, bool isAtomic) {  //isAtomic default to false
  dslCodePad& targetFile = isMainFile ? main : header;

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

void dsl_cpp_generator::generate_exprUnary(Expression* expr, bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

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

void dsl_cpp_generator::generate_exprProcCall(Expression* expr,
                                              bool isMainFile) {
  //~ cout << "inside the expr_proCall ggggggggggggggggggggggggg" << isMainFile;

  dslCodePad& targetFile = isMainFile ? main : header;

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
    sprintf(strBuffer, "(%s[%s+1]-%s[%s])", "gpu_OA", nodeId->getIdentifier(),
            "gpu_OA", nodeId->getIdentifier());
    targetFile.pushString(strBuffer);
  } else if (methodId == "is_an_edge") {
    char strBuffer[1024];
    list<argument*> argList = proc->getArgList();
    assert(argList.size() == 2);
    Identifier* srcId = argList.front()->getExpr()->getId();
    Identifier* destId = argList.back()->getExpr()->getId();
    Identifier* objectId = proc->getId1();
    sprintf(strBuffer, "%s.%s(%s, %s)", objectId->getIdentifier(),
            "check_if_nbr", srcId->getIdentifier(), destId->getIdentifier());
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

void dsl_cpp_generator::generate_exprPropId(
    PropAccess* propId, bool isMainFile)  // This needs to be made more generic.
{
  dslCodePad& targetFile = isMainFile ? main : header;

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
    sprintf(strBuffer, "%s_nxt[%s]", id2->getIdentifier(),
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

void dsl_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct,
                                           bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;

  char strBuffer[1024];
  Expression* convergeExpr = fixedPointConstruct->getDependentProp();
  Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
  //~ statement* blockStmt = fixedPointConstruct->getBody();
  assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
         convergeExpr->getExpressionFamily() == EXPR_ID);
  targetFile.pushString("while ( ");
  targetFile.push('!');
  targetFile.pushString(fixedPointId->getIdentifier());
  targetFile.pushString(
      "[0]");  // need to be changed later. Currently done for SSSP
  targetFile.pushstr_newL(" )");
  targetFile.pushstr_newL("{");
  sprintf(strBuffer, "%s[0] = %s;", fixedPointId->getIdentifier(),
          "true");  // needs to be changed. Currently done for SSSP
  targetFile.pushstr_newL(strBuffer);
  if (fixedPointConstruct->getBody()->getTypeofNode() != NODE_BLOCKSTMT)
    generateStatement(fixedPointConstruct->getBody(), isMainFile);
  else
    generateBlock((blockStatement*)fixedPointConstruct->getBody(), isMainFile);
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
  if (convergeExpr->getExpressionFamily() == EXPR_ID)
    dependentId = convergeExpr->getId();
  if (dependentId != NULL) {
    if (dependentId->getSymbolInfo()->getType()->isPropType()) {
      if (dependentId->getSymbolInfo()->getType()->isPropNodeType()) {
        //~ Type* type = dependentId->getSymbolInfo()->getType();

        if (graphId.size() > 1) {
          cerr << "GRAPH AMBIGUILTY";
        } else
          // sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
          // ","int","v","v",graphId[0]->getIdentifier(),"num_nodes","v");
          // targetFile.pushstr_newL(strBuffer);
          // targetFile.pushstr_space("{");
          targetFile.pushstr_newL(
              " initKernel<bool> <<< 1, 1>>>(1, gpu_finished, true);");
        targetFile.pushstr_newL(
            " Compute_SSSP_kernel<<<num_blocks , "
            "block_size>>>(gpu_OA,gpu_edgeList, gpu_edgeLen ,gpu_dist,src, V "
            ",MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);");
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
        /*
      if(isNot)------chopped out.
       {
        sprintf(strBuffer,"%s = !%s_fp
      ;",fixedPointId->getIdentifier(),dependentId->getIdentifier());
        main.pushstr_newL(strBuffer);
         }
         else
         {
           sprintf(strBuffer,"%s = %s_fp
      ;",fixedPointId->getIdentifier(),dependentId->getIdentifier());
           main.pushString(strBuffer);
         }--------chopped out.
         */
      }
    }
  }
  // targetFile.pushstr_newL("}");
}

void dsl_cpp_generator::generateBlock(blockStatement* blockStmt,
                                      bool includeBrace, bool isMainFile) {
  //~ cout << "i am inside generateBlock for the first time and the value of bool="
  //~ << isMainFile;
  dslCodePad& targetFile = isMainFile ? main : header;

  list<statement*> stmtList = blockStmt->returnStatements();
  list<statement*>::iterator itr;
  if (includeBrace) {
    targetFile.pushstr_newL("{");
  }
  for (itr = stmtList.begin(); itr != stmtList.end(); itr++) {
    statement* stmt = *itr;
    generateStatement(stmt, isMainFile);
  }
  if (includeBrace) {
    targetFile.pushstr_newL("}");
  }
}
void dsl_cpp_generator::generateStopTimer() {
  main.pushstr_newL("cudaEventRecord(stop,0);");
  main.pushstr_newL("cudaEventSynchronize(stop);");
  main.pushstr_newL("cudaEventElapsedTime(&milliseconds, start, stop);");
  main.pushstr_newL("printf(\"GPU Time: %.6f ms\\n\", milliseconds);");
}
void dsl_cpp_generator::generateStartTimer() {
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
void dsl_cpp_generator::generateFunc(ASTNode* proc) {
  // dslCodePad &targetFile = isMainFile ? main : header;

  char strBuffer[1024];
  Function* func = (Function*)proc;
  generateFuncHeader(func, false);  //.h or header file | adds prototype of main function and std headers files
  generateFuncHeader(func, true);   // .cu or main file

  // to genearte the function body of the algorithm
  // Note that this we can change later point of time if required

  char outVarName[] = "BC";  //inner type
  char outVarType[] = "double";
  main.pushstr_newL("{");

  generateFuncBody(func, false);  // GEnerates CSR ..bool is meaningless

  main.pushstr_newL("//END CSR ");
  main.NewLine();

  main.pushstr_newL("//DECLAR DEVICE AND HOST vars in params");
  sprintf(strBuffer, "%s* d_%s; cudaMalloc(&d_%s, sizeof(%s)*(V)); ///TODO from func", outVarType, outVarName, outVarName, outVarType);
  main.pushstr_newL(strBuffer);

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
  sprintf(strBuffer, "cudaMemcpy(%s,d_%s , sizeof(%s) * (V), cudaMemcpyDeviceToHost);", outVarName, outVarName, outVarType);
  main.pushstr_newL(strBuffer);

  main.pushstr_newL("} //end FUN");

  return;
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

void dsl_cpp_generator::generateCudaMallocStr(const char* dVar,
                                              const char* typeStr,
                                              const char* sizeOfType) {
  char strBuffer[1024];
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3
  sprintf(strBuffer, "cudaMalloc(&%s, sizeof(%s)*%s);", dVar, typeStr,
          sizeOfType);  // this assumes PropNode type  IS PROPNODE? V : E //else
                        // might error later
  main.pushstr_newL(strBuffer);
  //~ main.NewLine();
}

// Oonly for device variables
void dsl_cpp_generator::generateCudaMalloc(Type* type, const char* identifier) {
  char strBuffer[1024];
  //~ Type* targetType = type->getInnerTargetType();
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3
  sprintf(strBuffer, "cudaMalloc(&d_%s, sizeof(%s)*(%s));", identifier,
          convertToCppType(type->getInnerTargetType()),
          (type->isPropNodeType())
              ? "V"
              : "E");  // this assumes PropNode type  IS PROPNODE? V : E //else
                       // might error later

  main.pushstr_newL(strBuffer);
  main.NewLine();
}

//~ bool* d_finished;cudaMalloc(&d_finished,sizeof(bool) *(V));
void dsl_cpp_generator::generateExtraDeviceVariable(const char* typeStr, const char* dVar, const char* sizeVal) {
  //bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  // 1       2                                 2           1      3
  char strBuffer[1024];
  printf(strBuffer, "%s* %s; cudaMalloc(&%s,sizeof(%s)*(%s));", typeStr, dVar, dVar, typeStr, sizeVal);
  main.pushstr_newL(strBuffer);
  main.NewLine();
}

void dsl_cpp_generator::generateCudaMemcpy(const char* dVar, const char* cVar,
                                           const char* typeStr,
                                           const char* sizeOfType,
                                           bool isMainFile, const char* from) {
  char strBuffer[1024];
  //~ dslCodePad& targetFile = isMainFile ? main : header;
  // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
  //                   1             2      3

  sprintf(strBuffer, "cudaMemcpy(&d_%s,%s, sizeof(%s)*%s, %s);", dVar, cVar,
          typeStr, sizeOfType,
          from);  // this assumes PropNode type  IS PROPNODE? V : E //else might
                  // error later
  main.pushstr_newL(strBuffer);

  //~ main.NewLine();
}

void dsl_cpp_generator::generateCSRArrays(const char* gId) {
  char strBuffer[1024];

  sprintf(strBuffer, "unsigned V = %s.num_nodes();",
          gId);  // assuming DSL  do not contain variables as V and E
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer, "unsigned E = %s.num_edges();", gId);
  main.pushstr_newL(strBuffer);
  main.NewLine();

  main.pushstr_newL("printf(\"#nodes:%d\\n\",V);");
  main.pushstr_newL("printf(\"#edges:\%d\\n\",E);");

  sprintf(strBuffer, "int* edgeLen = %s.getEdgeLen();",
          gId);  // assuming DSL  do not contain variables as V and E
  main.pushstr_newL(strBuffer);
  main.NewLine();

  // These H & D arrays of CSR do not change. Hence hardcoded!
  main.pushstr_newL("int *h_meta;");
  main.pushstr_newL("int *h_data;");
  main.pushstr_newL("int *h_weight;");
  main.NewLine();

  main.pushstr_newL("h_meta = (int *)malloc( (V+1)*sizeof(int));");
  main.pushstr_newL("h_data = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_weight = (int *)malloc( (E)*sizeof(int));");
  main.NewLine();

  main.pushstr_newL("for(int i=0; i<= V; i++) {");
  sprintf(strBuffer, "int temp = %s.indexofNodes[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("h_meta[i] = temp;");
  main.pushstr_newL("}");
  main.NewLine();

  main.pushstr_newL("for(int i=0; i< E; i++) {");
  sprintf(strBuffer, "int temp = %s.edgeList[i];", gId);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("h_data[i] = temp;");
  main.pushstr_newL("temp = edgeLen[i];");
  main.pushstr_newL("h_weight[i] = temp;");
  main.pushstr_newL("}");
  main.NewLine();

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

void dsl_cpp_generator::generateFuncBody(Function* proc, bool isMainFile) {
  //~ dslCodePad& targetFile = isMainFile ? main : header;
  char strBuffer[1024];

  int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = proc->getParamList().size();
  list<formalParam*> paramList = proc->getParamList();
  list<formalParam*>::iterator itr;

  //~ bool genCSR =
  //~ false;  // to generate csr or not in main.cpp file if graph is a parameter
  const char*
      gId;  // to generate csr or not in main.cpp file if graph is a parameter
  for (itr = paramList.begin(); itr != paramList.end(); itr++) {
    arg_currNo++;
    argumentTotal--;

    Type* type = (*itr)->getType();
    // targetFile.pushString(convertToCppType(type));

    // targetFile.pushString(" ");

    // added here
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

    //~ sprintf(strBuffer,"int MAX_VAL = 2147483647 ;");
    //~ main.pushstr_newL(strBuffer);
    //~ sprintf(strBuffer,"int * gpu_edgeList;");
    //~ main.pushstr_newL(strBuffer);
    //~ sprintf(strBuffer," int * gpu_edgeLen;");
    //~ main.pushstr_newL(strBuffer);
    //~ sprintf(strBuffer,"int * gpu_dist;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer," int * gpu_OA;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"bool * gpu_modified_prev;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"bool * gpu_finished;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"int *gpu_rev_OA;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"int *gpu_srcList;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"float  *gpu_node_pr;");
    //~ main.pushstr_newL(strBuffer);

    //~ main.NewLine();

    //~ generateCudaMallocStr("gpu_OA"  ,"int","(1+V)");
    //~ generateCudaMallocStr("gpu_edgeList"  ,"int","(E)" );
    //~ generateCudaMallocStr("gpu_edgeLen","int","(E)");
    //~ generateCudaMallocStr("gpu_dist","int","(V)");
    //~ generateCudaMallocStr("gpu_modified_prev","bool","(V)");
    //~ generateCudaMallocStr("gpu_modified_next","bool","(V)");
    //~ generateCudaMallocStr("gpu_finished","bool","(1)");
    //~ generateCudaMallocStr("gpu_srcList","int","(E)");
    //~ generateCudaMallocStr("gpu_node_pr","flaot","(V)");
    //~ main.NewLine();

    //~ sprintf(strBuffer,"unsigned int block_size;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"unsigned int num_blocks;");
    //~ main.pushstr_newL(strBuffer);

    //~ sprintf(strBuffer,"if( V <= 1024)");
    //~ main.pushstr_newL(strBuffer);
    //~ main.pushstr_newL("{");
    //~ main.pushstr_newL("block_size = V;");
    //~ main.pushstr_newL("num_blocks = 1;");
    //~ main.pushstr_newL("}");
    //~ main.pushstr_newL("else");
    //~ main.pushstr_newL("{");
    //~ main.pushstr_newL("block_size = 1024;");
    //~ main.pushstr_newL("num_blocks = ceil(((float)V) / block_size);");
    //~ main.pushstr_newL("}");

    //~ generateCudaMemcpy("gpu_OA", "OA","int", "(1+V)",false,
    //"cudaMemcpyHostToDevice"); ~ generateCudaMemcpy("gpu_edgeList",
    //"edgeList","int", "E",false, "cudaMemcpyHostToDevice");

    //~ generateCudaMemcpy("gpu_edgeList", "edgeList","int", "E",false,
    //"cudaMemcpyHostToDevice"); ~ generateCudaMemcpy("gpu_edgeList",
    //"edgeList","int", "E",false, "cudaMemcpyHostToDevice"); ~
    //generateCudaMemcpy("gpu_edgeLen", "cpu_edgeLen ","int", "E",false,
    //"cudaMemcpyHostToDevice"); ~ generateCudaMemcpy("gpu_dist", "modified
    //","bool", "V",false, "cudaMemcpyHostToDevice"); ~
    //generateCudaMemcpy("gpu_finished", "finished ","bool", "1",false,
    //"cudaMemcpyHostToDevice"); ~ generateCudaMemcpy("gpu_srcList",
    //"cpu_srcList","int", "(E)",false, "cudaMemcpyHostToDevice");

    main.NewLine();

    sprintf(strBuffer, "int* d_meta;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "int* d_data;");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer, "int* d_weight;");
    main.pushstr_newL(strBuffer);
    main.NewLine();

    generateCudaMallocStr("d_meta", "int", "(1+V)");
    generateCudaMallocStr("d_data", "int", "(E)");
    generateCudaMallocStr("d_weight", "int", "(E)");
    main.NewLine();

    // h_meta h_data h_weight has to be populated!

    generateCudaMemCpyStr("d_meta", "h_meta", "int", "V+1");
    generateCudaMemCpyStr("d_data", "h_data", "int", "E");
    generateCudaMemCpyStr("d_weight", "h_weight", "int", "E");
    main.NewLine();

    main.pushstr_newL("// CSR END");

    main.pushString("//LAUNCH CONFIG");

    generateLaunchConfig("nodes");  // Most of the time unless we perform edgeBased!

    //~ main.pushString(proc->getIdentifier()->getIdentifier());
    //~ main.pushString("_kernel");
    //~ main.pushString("<<<");
    //~ main.pushString("num_blocks, block_size");
    //~ main.pushString(">>>");
    //~ main.push('(');
    //~ main.pushString("gpu_OA, gpu_edgeList, V, E )");
    //~ main.push(';');
    //~ main.NewLine();
    //~ main.pushString("cudaDeviceSynchronize();");
    //~ main.NewLine();

    //~ main.NewLine();

    main.NewLine();

    main.pushstr_newL("// TIMER START");
    generateStartTimer();
  }
}

void dsl_cpp_generator::generateFuncHeader(Function* proc, bool isMainFile) {
  dslCodePad& targetFile = isMainFile ? main : header;
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
      if (type->isGraphType()) {
        std::cout << "========== SET TRUE" << '\n';
        genCSR = true;
        //~ gId = parName;
      }
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

  if (!isMainFile)
    targetFile.pushString(";");
  //~ if (!isMainFile) {
  //~ targetFile.NewLine();
  //~ targetFile.pushString("{");
  //~ }

  targetFile.NewLine();
  targetFile.NewLine();

  return;
}

bool dsl_cpp_generator::openFileforOutput() {
  char temp[1024];
  printf("fileName %s\n", fileName);
  sprintf(temp, "%s/%s.h", "../graphcode/generated_cuda", fileName);
  headerFile = fopen(temp, "w");
  if (headerFile == NULL) return false;
  header.setOutputFile(headerFile);

  sprintf(temp, "%s/%s.cu", "../graphcode/generated_cuda", fileName);
  bodyFile = fopen(temp, "w");
  if (bodyFile == NULL) return false;
  main.setOutputFile(bodyFile);

  return true;
}
void dsl_cpp_generator::generation_end() {
  header.NewLine();
  header.pushstr_newL("#endif");
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
}

bool dsl_cpp_generator::generate() {
  // cout<<"FRONTEND
  // VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();
  // //openFileforOutput();
  if (!openFileforOutput()) return false;
  generation_begin();

  list<Function*> funcList = frontEndContext.getFuncList();
  for (Function* func : funcList) {
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

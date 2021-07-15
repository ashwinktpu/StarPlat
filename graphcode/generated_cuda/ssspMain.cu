/* v1.0
 * TODO: 1 kernel SSSP and CPU function - Thu, 15-Jul-2021, 12:21:20 IST
 * DONE: 1 kernel SSSP and CPU function - Thu, 15-Jul-2021, 22:24:12 IST
 * TODO: DUBUG LEVEL - Thu, 15-Jul-2021, 22:24:12 IST
 * DONE: DUBUG LEVEL - Thu, 15-Jul-2021, 23:01:02 IST
 * TODO: SEQ BF ALGO - Thu, 15-Jul-2021, 23:01:02 IST
 * DONE: SEQ BF ALGO - Thu, 15-Jul-2021, 23:24:02 IST
 * TODO: SEQ DIJKSTRA ALGO - Fri, 16-Jul-2021, 00:04:43 IST
 * DONE: SEQ DIJKSTRA ALGO - Fri, 16-Jul-2021, 00:22:34 IST
 *
 *
 * TODO
 * CLARIFY should wrap into Graph class?
 * CLARIFY should take file as param?
 * IMP should abstract for user
 * IMP should add timers
 */
#include "sssp_dsl.h"

#define DEBUG1 if(DEBUGCODE >= 1)
#define DEBUG2 if(DEBUGCODE >= 2)
unsigned DEBUGCODE = 0;

#define cudaCheckError() {                                             \
 cudaError_t e=cudaGetLastError();                                     \
 if(e!=cudaSuccess) {                                                  \
   printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e)); \
   exit(0);                                                            \
 }                                                                     \
}

template <typename T>
__global__ void initKernel(unsigned nSize,T* dArray,T initVal){
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if(id < nSize){
    dArray[id]=initVal;
  }
}

__global__ void SSSPKernel(unsigned nSize, int* csrMeta, int* csrData,int* csrWeight, int* distance, bool* modified){
  unsigned u = threadIdx.x + blockDim.x*blockIdx.x; // tid
  if(u < nSize && modified[u] ){ //Only modified u's && modified[u]
    for(int ii=csrMeta[u], end = csrMeta[u+1]; ii < end; ++ii){ //PUSH
      unsigned v = csrData[ii];
      int newDistance = distance[u] + csrWeight[ii] /*edgeweight(uv)*/ ;
      if(newDistance < distance[v]){
        atomicMin(&distance[v],newDistance);
        modified[v]=true;
      }
    }
  }
}

unsigned long long int processSSSP(const unsigned nSize, const unsigned mSize,
int*  csrM,
int*  csrD,
int*  csrW,
int*  distance,
bool* modified,
int source = 0)
{
  DEBUG1 std::cout<< "Process SSSP..start" << '\n';
  /**************
   * INITIALISE
   **************/
  // OPT we can do this in kernel
  for(int ii=0; ii< nSize; ++ii){
    distance[ii] = INT_MAX/2;
    modified[ii] = false;
  }

  distance[source] = 0    ;
  modified[source] = true ;

  /**********************
   * ALLOCATE on GPU
   **********************/
  int*  gcsrM;
  int*  gcsrD;
  int*  gcsrW;

  int*  gDistance;
  bool* gModified;

  unsigned nSizeIntPlus1  = sizeof(int) *(nSize+1);
  unsigned nSizeIntBytes  = sizeof(int) *( nSize );
  unsigned mSizeIntBytes  = sizeof(int) *( mSize );
  unsigned nSizeBoolBytes = sizeof(bool)*( nSize );

  cudaMalloc(&gcsrM, nSizeIntPlus1);
  cudaMalloc(&gcsrD, mSizeIntBytes);
  cudaMalloc(&gcsrW, mSizeIntBytes);

  cudaMalloc(&gDistance, nSizeIntBytes );
  cudaMalloc(&gModified, nSizeBoolBytes);

  /***************
   * COPY TO GPU
   ***************/
  cudaMemcpy( gcsrM, csrM, nSizeIntPlus1, cudaMemcpyHostToDevice);
  cudaMemcpy( gcsrD, csrD, mSizeIntBytes, cudaMemcpyHostToDevice);
  cudaMemcpy( gcsrW, csrW, mSizeIntBytes, cudaMemcpyHostToDevice);

  cudaMemcpy( gDistance, distance, nSizeIntBytes , cudaMemcpyHostToDevice);
  cudaMemcpy( gModified, modified, nSizeBoolBytes, cudaMemcpyHostToDevice);

  cudaCheckError()

  /***************
   * LAUNCH CONFIG
   ***************/
  const unsigned numOfThreads = 1024;
  const unsigned numOfBlocks  = (nSize+numOfThreads-1)/numOfThreads; //Ceil(n/#threads) with function call
  bool finished = false;
  //~ cudaSetDevice(1);
  DEBUG1 std::cout<< "FPLOCK..start.." << '\n';
  /***************
   * FIX PT COMPUTATION
   ***************/
   int k=0;
  do{

    SSSPKernel<<<numOfBlocks, numOfThreads>>>(nSize, gcsrM, gcsrD, gcsrW, gDistance, gModified);

    //~ cudaDeviceSynchronize();

    thrust::device_ptr<bool> gModPtr = thrust::device_pointer_cast(gModified);
    finished = (thrust::reduce(gModPtr, gModPtr + nSize, true, thrust::logical_and<bool>())); // does this go into default stream?
    //~ finished = (thrust::reduce(gModPtr, gModPtr + nSize, false, thrust::logical_or<bool>()));
    //~ finished = !finished;
    /*
     * https://stackoverflow.com/questions/33156534/pass-cuda-array-to-thrustinclusive-scan/33156815#33156815
     * https://thrust.github.io/doc/functional_8h_source.html#l00798
     * finished = neg of all ORs   ~(a | b | c | 1) //not working
     * finished = all AND
     */

    cudaDeviceSynchronize();

    //initKernel<bool> <<<numOfBlocks, numOfThreads>>>(nSize, gModified, false);
    //cudaDeviceSynchronize();

    // DEBUG
    ++k;
    DEBUG2 printf("k:%d Finish:%s\n",k, (finished?"True":"False"));
    if(k==nSize){
      DEBUG1 std::cout<< "TERMINATED!";
      break;
    }
  }while(!finished);
  DEBUG1 std::cout<< "FPLOCK..END.." << '\n';

  cudaMemcpy( distance, gDistance, nSizeIntBytes , cudaMemcpyDeviceToHost);

  DEBUG1 std::cout<< "Process SSSP..Completed" << '\n';
  // TODO: TIMER END
  // TODO: Check with CPU

  unsigned long long int sum=0;

  for(int ii=0; ii < nSize; ++ii){
    DEBUG2 printf("d[%d]=%d\n",ii, distance[ii]);
    sum+=distance[ii];
  }
  //~ DEBUG1 printf("HASH:%lld\n",sum);

  cudaCheckError()

  cudaFree(gcsrM);
  cudaFree(gcsrD);
  cudaFree(gcsrW);

  cudaFree(gDistance);
  cudaFree(gModified);

  return sum;
}



void readInput(
unsigned &nSize,
unsigned &mSize,
std::vector < std::vector <int>> &adjList,
std::map< std::pair<int,int>, int> &W){
  DEBUG1 std::cout<< "Reading inputs.." << '\n';

  std::string line, code, dummy;
  unsigned countEdge = 0;

  while(std::getline(std::cin, line) && !line.empty()){
    std::istringstream iStrSteam(line);
    iStrSteam >> code;
    if(code == "c") //Ignore this line
      continue;
    if(code == "p"){
      //~ p sp 264346 733846
      iStrSteam >> dummy >> nSize >> mSize;
      DEBUG2 std::cout<< nSize << " "<< mSize << '\n';
      adjList.resize(nSize);
    }
    else if( code == "a") {
      //~ a 1 2 803
      unsigned u, v;
      int w;

      iStrSteam >> u >> v >> w;
      //~ std::cout<< u << " " << v << " " << w << '\n';
      adjList[u-1].push_back(v-1);    // NOTE MINUS 1
      W[{u-1,v-1}]=w;                 // NOTE MINUS 1
      countEdge++;
    }
  }
  if(countEdge != mSize){
    std::cout<< "WARN: possibly different #edges" << '\n';
  }
  DEBUG1 std::cout<< "Loaded inputs.." << '\n';
}

void computeCSR(const unsigned nSize, const unsigned mSize,
const std::vector < std::vector <int>> &adjList,
const std::map< std::pair<int,int>, int> &W,  // DONT use const and LHS = W[]
int* csrM,
int* csrD,
int* csrW){
  DEBUG1 std::cout<< "Computing CSR.." << '\n';
  DEBUG2 std::cout<< "csr[0]=0" << '\n';
  for(int ii=0; ii < nSize; ++ii) {
    csrM[ii+1]= csrM[ii]+(int) adjList[ii].size() ; // NOTE +1
    DEBUG2 std::cout<< "csr"<<"["<<ii+1<< "]= "<< csrM[ii+1] << '\n';
  }

  int u   = 0;
  int idx = 0;
  DEBUG2 std::cout<< "CSRDATA" << '\n';
  for(auto nodes : adjList){
    // u begins with 0
    DEBUG2 std::cout<< u << ":" << ' ';
    for(int v : nodes){
      csrD[idx] = v ;
      csrW[idx] = W.at({u,v});
      DEBUG2 printf("(%d,%d),",csrD[idx],csrW[idx]);
      ++idx;
    }
    DEBUG2 std::cout<<  '\n';
    ++u; //NOTE
  }
  if( idx != mSize)
    std::cout<< "WARN: possibly different #edges" << '\n';

}

void printAdjList(const std::vector< std::vector<int> > &graph){
	int i = 0;
	for (auto vec : graph){

		std::cout << i << ": ";
		for(auto v : vec){
			std::cout<< v << " ";
		}
		i++;
		std::cout << std::endl;
	}
}

unsigned long long int BellmanFordMoore(
int nSize,
const std::vector< std::vector<int> > &adjList,
const std::map< std::pair<int,int>, int> &W,
int source = 0
) {
  DEBUG1 std::cout<< "SEQ CPU BFord..start" << '\n';
  std::vector<int> minDist(nSize, INT_MAX/2);
	//~ vector<int> parent(N, -1);
	// all parent are -1
	// all minDist are INT_MAX

	minDist[source] =0;
	bool updated = true;
	for (int k =1; k < (nSize-1) && updated; ++k){
		updated = false;
		DEBUG2 std::cout << k <<" of " << nSize-1 << std::endl;
		for(int u=0, endU = nSize; u < endU; u++){
			for(int j=0, endJ = adjList[u].size(); j < endJ; ++j){
				int v = adjList[u][j];
        int w = W.at({u,v}); // edge weight of (u,v)
				int newDist =  minDist[u]+w;

				if(	newDist < minDist[v] ){ // the to perform relax!
					minDist[v] = newDist;
					//~ parent[v] = u;
					updated=true;
				}
			}
		}
	}
  DEBUG1 std::cout<< "SEQ CPU BFord..end" << '\n';
  unsigned long long int sum = 0;
  for (auto &v : minDist)
    sum += v;

  return sum;
}


unsigned long long int Dijkstra( int nSize,
const std::vector< std::vector<int> > &adjList,
const std::map< std::pair<int,int>, int> &W,
int source=0
) {

  DEBUG1 std::cout<< "SEQ CPU Dijkstra..start" << '\n';

  std::vector<int> minDist(nSize, INT_MAX/2);
	//~ vector<int> parent(graph.size() , -1);
	minDist[ source ] = 0;
	std::set< std::pair<int,int> > active_vertices;
	active_vertices.insert( {0,source} );

	while (!active_vertices.empty()) {
		int u = active_vertices.begin()->second;

		active_vertices.erase( active_vertices.begin() );
		for (auto v : adjList[u]) {
			auto newdist = minDist[u] + W.at({u,v});
			if (newdist < minDist[v]) {
				active_vertices.erase( { minDist[v], v } );
				minDist[v] = newdist;
				//~ parent[v] = u;
				active_vertices.insert( { newdist, v } );
			}
		}
	}

  DEBUG1 std::cout<< "SEQ CPU Dijkstra..end" << '\n';
  unsigned long long int sum = 0;
  for (auto &v : minDist)
    sum += v;

  return sum;
}

int main(int argc, char** argv){

  if(argc > 0){
    DEBUGCODE = argc-1;
    DEBUG1 std::cout<< "DEBUG LEVEL:" << DEBUGCODE << '\n';
  }

  unsigned int nSize;
  unsigned int mSize;

  std::map< std::pair<int,int>, int> W;
  std::vector < std::vector <int>> adjList;

  readInput  (nSize, mSize, adjList, W);
  DEBUG2 printAdjList(adjList);

  int* csrM = (int*) malloc(sizeof(int)*(nSize+1)); //NOTE +1
  int* csrD = (int*) malloc(sizeof(int)*( mSize ));
  int* csrW = (int*) malloc(sizeof(int)*( mSize ));

  int*  distance = (int*)  malloc(sizeof(int) * nSize);
  bool* modified = (bool*) malloc(sizeof(bool)* nSize);
  //~ int*  parent= (int*) malloc(sizeof(int)*nSize);

  computeCSR (nSize, mSize, adjList, W, csrM, csrD, csrW);
  //~ initialize ()
  auto gpuHash = processSSSP(nSize, mSize, csrM, csrD, csrW, distance, modified);
  auto cpuHash = Dijkstra(nSize, adjList, W);
  //~ auto cpuHash = BellmanFordMoore(nSize, adjList, W);
  DEBUG1 printf("(G,C): (%lld,%lld)", gpuHash, cpuHash);
  printf(":%s\n", (gpuHash!=cpuHash?"False": "True"));
  //deallocate(csrM, csrD, csrW, distance, modified););
  free(csrM);
  free(csrD);
  free(csrW);
  free(distance);
  free(modified);

  return 0;
}

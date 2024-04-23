// #ifndef GENCPP_SCC_V2_H
// #define GENCPP_SCC_V2_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "graph.hpp"

using namespace sycl;
using namespace std;

void vHong(graph& g)
{
  queue Q(default_selector_v);
  std::cout << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;

  d_meta=malloc_device<int>((1+V), Q);
  d_data=malloc_device<int>((E), Q);
  d_src=malloc_device<int>((E), Q);
  d_weight=malloc_device<int>((E), Q);
  d_rev_meta=malloc_device<int>((V+1), Q);

  Q.submit([&](handler &h)
    { h.memcpy(  d_meta,   h_meta, sizeof(int)*(V+1)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(  d_data,   h_data, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(   d_src,    h_src, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_weight, h_weight, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1))); })
  .wait();

  // CSR END
  //LAUNCH CONFIG
  int NUM_THREADS = 1048576;
  int stride = NUM_THREADS;

  // TIMER START
  std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int* d_modified;
  d_modified=malloc_device<int>(V, Q);

  bool* d_modified_nxt;
  d_modified_nxt=malloc_device<bool>(V, Q);

  int* d_outDeg;
  d_outDeg=malloc_device<int>(V, Q);

  int* d_inDeg;
  d_inDeg=malloc_device<int>(V, Q);

  bool* d_visitFw;
  d_visitFw=malloc_device<bool>(V, Q);

  bool* d_visitBw;
  d_visitBw=malloc_device<bool>(V, Q);

  bool* d_propFw;
  d_propFw=malloc_device<bool>(V, Q);

  bool* d_propBw;
  d_propBw=malloc_device<bool>(V, Q);

  bool* d_isPivot;
  d_isPivot=malloc_device<bool>(V, Q);

  int* d_scc;
  d_scc=malloc_device<int>(V, Q);

  int* d_range;
  d_range=malloc_device<int>(V, Q);

  int* d_pivotField;
  d_pivotField=malloc_device<int>(V, Q);

  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
    for (; i < V; i += stride) d_modified[i] = (int)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
  for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_outDeg[i] = (int)0;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_inDeg[i] = (int)0;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_visitFw[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_visitBw[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_propFw[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_propBw[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_isPivot[i] = (bool)false;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_scc[i] = (int)-1;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_range[i] = (int)0;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_pivotField[i] = (int)-1;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
int nxtSrc = src + 1; // asst in main

d_inDeg[src] = (int)d_inDeg[src] + d_rev_meta[nxtSrc] - d_rev_meta[src]; //InitIndex
d_outDeg[src] = (int)d_inDeg[src] + d_meta[nxtSrc] - d_meta[src]; //InitIndex
}
}); }).wait(); // end KER FUNC

bool *d_fpoint1;
d_fpoint1=malloc_device<bool>(1, Q);
bool fpoint1 = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!fpoint1) {

fpoint1 = true;
Q.submit([&](handler &h)
{ h.memcpy(d_fpoint1, &fpoint1, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int havePar = 0; // asst in main

int haveChild = 0; // asst in main

// for all else part
for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
{int par = d_src[edge] ;
if (d_scc[par] == -1){ // if filter begin 
if (d_range[par] == d_range[src]){ // if filter begin 
havePar = 1;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
// for all else part
for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
int dst = d_data[edge];
if (d_scc[dst] == -1){ // if filter begin 
if (d_range[dst] == d_range[src]){ // if filter begin 
haveChild = 1;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
d_scc[src] = src;
d_isPivot[src] = true;
d_modified_nxt[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_fpoint1 = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&fpoint1, d_fpoint1, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int color = d_range[src]; // asst in main

int index = color - (color / V) * V; // asst in main

int oldSrcValue = -1; // asst in main

int oldSrc = d_pivotField[index]; // asst in main

if (oldSrc >= 0){ // if filter begin 
oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];

} // if filter end
if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){ // if filter begin 
d_pivotField[index] = src;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int color = d_range[src]; // asst in main

int index = color - (color / V) * V; // asst in main

if (d_pivotField[index] == src){ // if filter begin 
d_visitFw[src] = true;
d_visitBw[src] = true;
d_isPivot[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

bool *d_fpoint2;
d_fpoint2=malloc_device<bool>(1, Q);
bool fpoint2 = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!fpoint2) {

fpoint2 = true;
Q.submit([&](handler &h)
{ h.memcpy(d_fpoint2, &fpoint2, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int myrange = d_range[src]; // asst in main

if (d_propFw[src] == false && d_visitFw[src] == true){ // if filter begin 
// for all else part
for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
int dst = d_data[edge];
if (d_scc[dst] == -1){ // if filter begin 
if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
d_visitFw[dst] = true;
d_modified_nxt[src] = true;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
d_propFw[src] = true;

} // if filter end
if (d_propBw[src] == false && d_visitBw[src] == true){ // if filter begin 
// for all else part
for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
{int par = d_src[edge] ;
if (d_scc[par] == -1){ // if filter begin 
if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
d_visitBw[par] = true;
d_modified_nxt[src] = true;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
d_propBw[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_fpoint2 = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&fpoint2, d_fpoint2, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
if (d_visitFw[src] == false || d_visitBw[src] == false){ // if filter begin 
int ext1 = 0; // asst in main

int ext2 = 0; // asst in main

if (d_visitFw[src] == false){ // if filter begin 
ext1 = 1;

} // if filter end
if (d_visitBw[src] == false){ // if filter begin 
ext2 = 1;

} // if filter end
int newRange = 3 * d_range[src] + ext1 + ext2; // asst in main

d_range[src] = newRange;
d_visitFw[src] = false;
d_visitBw[src] = false;
d_propFw[src] = false;
d_propBw[src] = false;

} // if filter end
if (d_visitFw[src] == true && d_visitBw[src] == true){ // if filter begin 
d_scc[src] = src;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

bool *d_fpoint3;
d_fpoint3=malloc_device<bool>(1, Q);
bool fpoint3 = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!fpoint1) {

fpoint1 = true;
Q.submit([&](handler &h)
{ h.memcpy(d_fpoint1, &fpoint1, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int havePar = 0; // asst in main

int haveChild = 0; // asst in main

// for all else part
for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
{int par = d_src[edge] ;
if (d_scc[par] == -1){ // if filter begin 
if (d_range[par] == d_range[src]){ // if filter begin 
havePar = 1;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
// for all else part
for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
int dst = d_data[edge];
if (d_scc[dst] == -1){ // if filter begin 
if (d_range[dst] == d_range[src]){ // if filter begin 
haveChild = 1;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
d_scc[src] = src;
d_isPivot[src] = true;
d_modified_nxt[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_fpoint1 = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&fpoint1, d_fpoint1, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_range[i] = (int)0;});
}).wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
d_range[src] = (int)d_range[src] + src; //InitIndex
}
}); }).wait(); // end KER FUNC

bool *d_fpoint4;
d_fpoint4=malloc_device<bool>(1, Q);
bool fpoint4 = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!fpoint4) {

fpoint4 = true;
Q.submit([&](handler &h)
{ h.memcpy(d_fpoint4, &fpoint4, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
// for all else part
for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
int dst = d_data[edge];
if (d_scc[dst] == -1){ // if filter begin 
if (d_range[dst] < d_range[src]){ // if filter begin 
d_range[src] = d_range[dst];
d_modified_nxt[src] = true;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int myrange = d_range[src]; // asst in main

if ((myrange != src) && (myrange != d_range[myrange])){ // if filter begin 
d_range[src] = d_range[myrange];
d_modified_nxt[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_fpoint4 = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&fpoint4, d_fpoint4, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT

bool *d_fpoint5;
d_fpoint5=malloc_device<bool>(1, Q);
bool fpoint5 = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!fpoint5) {

fpoint5 = true;
Q.submit([&](handler &h)
{ h.memcpy(d_fpoint5, &fpoint5, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int color = d_range[src]; // asst in main

int index = color - (color / V) * V; // asst in main

int oldSrcValue = -1; // asst in main

int oldSrc = d_pivotField[index]; // asst in main

if (oldSrc >= 0){ // if filter begin 
oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];

} // if filter end
if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){ // if filter begin 
d_pivotField[index] = src;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int color = d_range[src]; // asst in main

int index = color - (color / V) * V; // asst in main

if (d_pivotField[index] == src){ // if filter begin 
d_visitFw[src] = true;
d_visitBw[src] = true;
d_isPivot[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

bool *d_fpoint6;
d_fpoint6=malloc_device<bool>(1, Q);
bool fpoint6 = false; // asst in main

// FIXED POINT variables
//BEGIN FIXED POINT

while(!fpoint6) {

fpoint6 = true;
Q.submit([&](handler &h)
{ h.memcpy(d_fpoint6, &fpoint6, 1 * sizeof(bool)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
int myrange = d_range[src]; // asst in main

if (d_propFw[src] == false && d_visitFw[src] == true){ // if filter begin 
// for all else part
for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
int dst = d_data[edge];
if (d_scc[dst] == -1){ // if filter begin 
if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
d_visitFw[dst] = true;
d_modified_nxt[src] = true;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
d_propFw[src] = true;

} // if filter end
if (d_propBw[src] == false && d_visitBw[src] == true){ // if filter begin 
// for all else part
for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
{int par = d_src[edge] ;
if (d_scc[par] == -1){ // if filter begin 
if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
d_visitBw[par] = true;
d_modified_nxt[src] = true;

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
d_propBw[src] = true;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_fpoint6 = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&fpoint6, d_fpoint6, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> src){for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_scc[src] == -1){ // if filter begin 
if (d_visitFw[src] == false || d_visitBw[src] == false){ // if filter begin 
d_modified_nxt[src] = true;
int ext1 = 0; // asst in main

int ext2 = 0; // asst in main

if (d_visitFw[src] == false){ // if filter begin 
ext1 = 1;

} // if filter end
if (d_visitBw[src] == false){ // if filter begin 
ext2 = 1;

} // if filter end
int newRange = 3 * d_range[src] + ext1 + ext2; // asst in main

newRange = newRange - (newRange / V) * V;
d_range[src] = newRange;
d_visitFw[src] = false;
d_visitBw[src] = false;
d_propFw[src] = false;
d_propBw[src] = false;

} // if filter end
if (d_visitFw[src] == true && d_visitBw[src] == true){ // if filter begin 
d_scc[src] = src;

} // if filter end

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified[i] = d_modified_nxt[i];}); }).wait();
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_modified_nxt[i] = (bool)false;});
}).wait();


Q.submit([ & ](handler & h) {
  h.parallel_for(NUM_THREADS, [ = ](id < 1 > i) {
    for(; i < V; i += stride) {
      if(d_modified[i]) *d_fpoint5 = false;    }
  });
}).wait();
Q.submit([&](handler &h)
{ h.memcpy(&fpoint5, d_fpoint5, 1 * sizeof(bool)); })
.wait();

} // END FIXED POINT
    
    int count_scc=0;
    bool *h_scc = (bool *)malloc((V) * sizeof(bool));
	Q.submit([&](handler &h)
			{ h.memcpy(h_scc, d_isPivot, sizeof(bool) * (V)); }).wait();
	// SCC counting in host side.
	 count_scc = 0;
	for (int i = 0; i < V; i++){
		if (h_scc[i]){
			count_scc++;
		}
	}
	std::cout << std::endl;
	std::cout << "No.of strongly connected components in graph: " << count_scc << std::endl;
    


//free up!! all propVars in this BLOCK!
free(d_pivotField, Q);
free(d_range, Q);
free(d_scc, Q);
free(d_isPivot, Q);
free(d_propBw, Q);
free(d_propFw, Q);
free(d_visitBw, Q);
free(d_visitFw, Q);
free(d_inDeg, Q);
free(d_outDeg, Q);
free(d_modified_nxt, Q);
free(d_modified, Q);

//TIMER STOP
std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
std::cout<<"Time required: "<<std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count()<<"[µs]"<<std::endl;

} //end FUN

// #endif
int main(int argc, char** argv)
{
	std::chrono::steady_clock::time_point start_prg = std::chrono::steady_clock::now();
	char* inp = argv[1];
	std::cout << "Taking input from: " << inp << std::endl;
	graph g(inp);
	g.parseGraph();
	vHong(g);
	std::chrono::steady_clock::time_point stop_prg = std::chrono::steady_clock::now();
	std::cout << "Time taken by program is: " <<std::chrono::duration_cast<std::chrono::milliseconds>(stop_prg - start_prg).count();
	std::cout << " milliseconds." << std::endl;
	return 0;
}
#ifndef GENHIP_SSSP_DSLV2_H
#define GENHIP_SSSP_DSLV2_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void Compute_SSSP(
  graph& dG,
  int* dDist,
  int* dWeight,
  int dSrc
);

#endif

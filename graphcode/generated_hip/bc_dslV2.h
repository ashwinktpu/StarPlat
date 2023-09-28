#ifndef GENHIP_BC_DSLV2_H
#define GENHIP_BC_DSLV2_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void Compute_BC(
  graph& dG,
  float* dBC,
  std::set<int>& dSourceSet
);

#endif

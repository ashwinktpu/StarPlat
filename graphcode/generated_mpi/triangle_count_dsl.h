#ifndef GENCPP_TRIANGLE_COUNT_DSL_H
#define GENCPP_TRIANGLE_COUNT_DSL_H
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdbool>
#include <fstream>
#include <sys/time.h>
#include <boost/mpi.hpp>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
#include "graph.hpp"

using namespace std;
namespace mpi = boost::mpi;

void Compute_TC(graph g);

#endif

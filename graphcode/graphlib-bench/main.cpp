#include <stdio.h>

#include "../graph.hpp"

int main(int argc, char* argv[]) {
  graph g = graph("inp.txt");

  printf("start parsing graph\n");
  g.parseGraph();
  printf("done parsing graph\n");
}


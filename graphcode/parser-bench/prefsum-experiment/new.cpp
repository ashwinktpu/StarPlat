#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string.h>
#include <vector>

class edge {
public:
  int32_t source;
  int32_t destination;
  int32_t weight;
  int32_t id; /* -unique Id for each edge.
                 -useful in adding properties to edges. */
  int dir;
};

class graph {
private:
  int32_t nodesTotal;
  int32_t edgesTotal;
  int32_t *edgeLen;
  int32_t *diff_edgeLen;
  int32_t *rev_edgeLen;
  int32_t *diff_rev_edgeLen;
  char *filePath;
  //std::map<int32_t, std::vector<edge>> edges;
  std::vector<edge>* edges;

public:
  int32_t *indexofNodes;     /* stores prefix sum for outneighbours of a node*/
  int32_t *rev_indexofNodes; /* stores prefix sum for inneighbours of a node*/
  int32_t *edgeList;         /*stores destination corresponding to edgeNo.
                               required for iteration over out neighbours */
  int32_t *srcList;          /*stores source corresponding to edgeNo.
                               required for iteration over in neighbours */
  std::vector<edge> graph_edge;
  int32_t *diff_indexofNodes;
  int32_t *diff_edgeList;
  int32_t *diff_rev_indexofNodes;
  int32_t *diff_rev_edgeList;
  int32_t *perNodeCSRSpace;
  int32_t *perNodeRevCSRSpace;
  int32_t *edgeMap;
  std::map<int, int> outDeg;
  std::map<int, int> inDeg;

  graph(char *file) {
    filePath = file;
    nodesTotal = 0;
    edgesTotal = 0;
    diff_edgeList = NULL;
    diff_indexofNodes = NULL;
    diff_rev_indexofNodes = NULL;
    diff_rev_edgeList = NULL;
    rev_edgeLen = NULL;
  }

  void parseEdges() {
    std::ifstream infile;
    infile.open(filePath);
    std::string line;

    while (std::getline(infile, line)) {
      if (line.length() == 0 || line[0] < '0' || line[0] > '9') {
        continue;
      }

      std::stringstream ss(line);
      edgesTotal++;

      edge e;

      int32_t source;
      int32_t destination;
      int32_t weightVal;

      ss >> source;
      if (source > nodesTotal)
        nodesTotal = source;

      ss >> destination;

      nodesTotal = max(nodesTotal, destination);

      e.source = source;
      e.destination = destination;
      e.weight = 1;

      edges[source].push_back(e);
      graph_edge.push_back(e);

      ss >> weightVal; // for edgelists having weight too.
    }

    infile.close();
  }


  void prefSum() {
    indexofNodes = new int32_t[nodesTotal + 2];
    edgeList = new int32_t[edgesTotal];
    edgeLen = new int32_t[edgesTotal];

    int* nbrCount = new int[nodesTotal+2];
    for (int i = 0; i < nodesTotal; i++) {
      nbrCount[i] = edges[i].size();
    }

    for (int i = 1; i <= nodesTotal; i++) {
      indexofNodes[i] = indexofNodes[i-1] + nbrCount[i];
    }

    for (int i = 1; i <= nodesTotal; i++) {
      int edge_count = 0;
      for (auto v: edges[i]) {
        edgeList[edge_count] = v.destination;
        edgeList[edge_count] = v.weight;
      }
      edge_count++;
    }
  }
};

int main() {
  graph g = graph("../inp.txt");
  g.parseEdges();
  g.prefSum();
  printf("done\n");

  printf("SIZES\n");
  printf("vector<edge> %d\n", sizeof(std::vector<edge>));
  printf("edge %d\n", sizeof(edge));
  printf("graph %d\n", sizeof(graph));
  printf("int32_t %d\n", sizeof(int32_t));
  printf("std::map<int32_t, std::vector<edge>> %d\n",
          sizeof(std::map<int32_t, std::vector<edge>>));
}

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>


int readFile(const char* fname) {
  std::ifstream infile;
  infile.open(fname);
  std::string line;

  int largest = 0;

  while (std::getline(infile, line)) {
    if (line.length() == 0 || line[0] < '0' || line[0] > '9')
      continue;

    int source, destination;
    std::stringstream ss(line);
    ss >> source;
    ss >> destination;

    largest = std::max({largest, source, destination});
  }

  return largest;
}

int main() {
  printf("%d\n", readFile("../inp.txt"));
}

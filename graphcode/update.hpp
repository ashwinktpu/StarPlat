#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>

class update
{
public:
  char type;
  int32_t source;
  int32_t destination;
  int32_t weight;
};

std::vector<update> parseUpdateFile(char *updateFile, bool isDirected)
{
  std::vector<update> updates;
  std::ifstream infile;
  infile.open(updateFile);
  std::string line;

  while (std::getline(infile, line))
  {
    std::stringstream ss(line);

    char type;
    int32_t source;
    int32_t destination;

    ss >> type;
    ss >> source;
    ss >> destination;

    update u;
    u.type = type;
    u.source = source;
    u.destination = destination;
    u.weight = 1;
    updates.push_back(u);

    // only required for undirected graph case
    if (isDirected == false)
    {
      update u1;
      u1.type = type;
      u1.source = destination;
      u1.destination = source;
      u1.weight = 1;

      updates.push_back(u1);
    }
  }

  return updates;
}
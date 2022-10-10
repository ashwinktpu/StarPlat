#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> adjList;
int total;

void parseGraph(char* input, char* output, bool isWeighted = false)
{
    ifstream infile;
    infile.open(input);
    
    ofstream outfile;
    outfile.open(output);
    
    string line;

    int nedges = 0;
    int nvert = 0;
    while(getline(infile,line))
    {
      if(line.length()==0 || line[0] < '0' || line[0] >'9') continue;

      stringstream ss(line);
      
      int32_t source, destination, weightVal = 1;
      ss >> source >> destination;
      if(isWeighted) ss >> weightVal;
      
      // Output must be 1 indexing so take care of that
      adjList[source].push_back(destination + 1);
      adjList[destination].push_back(source + 1);
      nedges += 1;
    }
    
    outfile << total << " " << nedges << "\n";
    for(int i = 0; i < total; i++)
    {
        int ln = adjList[i].size();
        for(int j = 0; j < ln; j++){
            outfile << adjList[i][j];
            if(j != ln - 1) outfile << " ";
        }
        outfile << "\n";
    }
}

int  main(int argc, char **argv){
    // TOTAL vertices
    // g++ -std=c++17 to_metis.cpp && ./a.out "<input>.txt" "<output>.graph" "total nodes"
    if(argc < 4){
        cout << "Usage: ./a.out <input_file> <output_file> <left_size>" << endl;
        return 0;
    }
    char* inp = argv[1];
    char* out = argv[2];
    total = atoi(argv[3]);
    adjList = vector<vector<int>>(total);
    parseGraph(inp, out);
    return 0;
}

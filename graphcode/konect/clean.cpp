#include <bits/stdc++.h>
using namespace std;

void parseGraph(char* input, char* output, int leftSize, bool isWeighted = false)
{
    ifstream infile;
    infile.open(input);
    
    ofstream outfile;
    outfile.open(output);
    
    string line;

    while(getline(infile,line))
    {
      if(line.length()==0 || line[0] < '0' || line[0] >'9') continue;

      stringstream ss(line);
      
      int32_t source, destination, weightVal = 1;
      ss >> source >> destination;
      if(isWeighted) ss >> weightVal;
      
      outfile << source-1 << ' ' << destination-1 + leftSize << endl;
    }
}

int  main(int argc, char **argv){
    if(argc < 4){
        cout << "Usage: ./a.out <input_file> <output_file> <left_size>" << endl;
        return 0;
    }
    char* inp = argv[1];
    char* out = argv[2];
    int leftSize = atoi(argv[3]);
    parseGraph(inp, out, leftSize);
    return 0;
}

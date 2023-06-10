//g++ main.cpp bc_dsl_v3.cpp -lOpenCL
// ./a.out inputGraph.txt  output.txt scrList.txt  n -p(optional)

#include "bc_dsl_v3.h"
#include<bits/stdc++.h>
using namespace std;
int main(int argc, char *argv[])
{
    graph g = graph(argv[1]);
    cout<<" Parsing Graph ....";
    g.parseGraph();
    cout<<"done"<<endl;
    int V = g.num_nodes();
    
    FILE *srcfp = fopen(argv[3], "r");
    int n = atoi(argv[4]);
    set<int>sourceList;
    int src;
    for(int i=0; i<min(V,n); i++)
    {
    	fscanf(srcfp, "%d", &src);
        sourceList.insert(src);
    }
    
    double *BC = (double*)malloc(V*sizeof(double));
    Compute_BC(g,BC,sourceList);
    FILE *op = fopen(argv[2],"w");
    for(int i=0; i<min(V,n); i++)
    {
        fprintf(op,"%lf\n", BC[i]);
        if(argc>5)
        {
            printf("BC Value = %lf\n", BC[i]);
        }
    }
    fclose(op);
    return 0;
}

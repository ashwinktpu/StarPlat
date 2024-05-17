#include "myheader.h"
using namespace std;


/* retuns the number of nodes and edges in a graph taking .txt file as a input*/
vector<int> whatsize(char *filepath){
    graph g(filepath);
    g.parseGraph();
    int V=g.num_nodes();
    int E=g.num_edges();
    return {V,E};
}
vector<int> whatRsize(char *filepath){
    graph g(filepath);
    g.parseGraphResidual();
    int Vr=g.num_nodes();
    int Er=g.num_edges();
    return {Vr,Er};
}




/* it initialises the """""CSR ARRAYS""""" of the the original graph and also the residual graph */
void readGraph(char *filepath,int *cpu_index,int *cpu_edgeList,int *cpu_edgeLength,int *cpu_Rindex,int *cpu_RedgeList,int *cpu_RedgeLength,
                int *rev_cpu_index,int* rev_cpu_edgeList,int *simple_revIndex,int *simple_revList)
{
    graph g(filepath),rg(filepath);
    g.parseGraph();
    cout<<"hello \n";
    rg.parseGraphResidual();

    int V=g.num_nodes();
    int E=g.num_edges();
    cout<<V<<"   nodes<---number--->edges    "<<E<<endl;

    int Vr=rg.num_nodes();
    int Er=rg.num_edges();


    cout<<Vr<<"   nodes<---residual number--->edges    "<<Er<<endl;
    

    for(int i=0;i<V;i++){
        cpu_index[i]=g.indexofNodes[i];
        simple_revIndex[i]=g.rev_indexofNodes[i];
        //cout<<g.rev_indexofNodes[i]<<"   ";
    }
    //cout<<endl;
    int *p=g.getEdgeLen();
    for(int i=0;i<E;i++){
        cpu_edgeList[i]=g.edgeList[i];
        cpu_edgeLength[i]=p[i];
        simple_revList[i]=g.srcList[i];
        //cout<<g.srcList[i]<<"  ";
        
    }
    //cout<<endl<<endl<<endl<<endl;

    for(int i=0;i<V;i++) {
        cpu_Rindex[i]=rg.indexofNodes[i];
        rev_cpu_index[i]=rg.rev_indexofNodes[i];
    }


    int *q=rg.getEdgeLen();
    for(int i=0;i<Er;i++){
        cpu_RedgeList[i]=rg.edgeList[i];
        cpu_RedgeLength[i]=q[i];
        rev_cpu_edgeList[i]=rg.srcList[i];
        
    }
    cout<<"reading graph completed\n";
}
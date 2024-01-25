#include "myheader.h"
using namespace std;


/* Change function takes a graph as a input(ofcourse the CSR of the graph) ,V as the number of vertices in the graph ,
   E as the number of edges in the graph, and an edge represents by its two end nodes as "s" and "e" and the value which 
   has to be assigned to that corresponding edge
*/
int change(int V,int E,int s,int e,int *index,int *list,int *length,int value){
    int start=index[s];
    int end=E-1;
    if(s<V-1) end=index[s+1]-1;
    


    int ans=-1;
    //cout<<"called for changing   "<<s<<"   to   "<<e<<"   to   "<<value<<endl;
    int i;
    for(i=start;i<=end;i++){
        if(list[i]==e){
           // cout<<"\ngotch you at  "<<i <<"\n";
            ans=length[i];
            //cout<<"intial flow value from "<<s<<"    to     "<<e<<"is now "<<length[i]<<endl;
            length[i]=value;
           // cout<<"after preflow value from "<<s<<"    to     "<<e<<"is now "<<length[i]<<endl;
            break;
        }
    }
    //cout<<"  changed value is "<<length[i]<<endl;
    return ans;
}

/* 
Preflow function  takes the original network as input graph, and also the residual graph as the input (ofcourse both the graphs are represented in CSR)
it initialiss the height of every node,pushes max possible value from the source to the neighbours of the source ,initialises the excess_flow of 
each node and also initialsies the excess_total variable and also changes the residual network according to the preflow.  
*/
void preflow(int V,int E ,int Er,int source, int sink,int *cpu_index,int *cpu_edgeList,int *cpu_edgeLength,
            int *cpu_Rindex,int *cpu_RedgeList,int *cpu_RedgeLength,int *cpu_height, int *cpu_excess_flow,int *Excess_total ,int *khush)
{
    // initialising height values and excess flow, Excess_total values
    for(int i = 0; i < V; i++)
    {
        cpu_height[i] = 0; 
        cpu_excess_flow[i] = 0;
    }
    
    cpu_height[source] = V;
    *Excess_total = 0;


    int start=cpu_index[source];
    int end=E-1;
    if(source<V-1) end=cpu_index[source+1]-1;


    for(int i=start;i<=end;i++){
        int s=source;
        int e=cpu_edgeList[i];
        int w=cpu_edgeLength[i];
        if(khush[e]==0) continue;
        //cout<<"pushing  preflow "<<w<<"   from    "<<s<<"    "<<e<<endl;
        change(V,Er,source,cpu_edgeList[i],cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,0);
        change(V,Er,cpu_edgeList[i],source,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,w);
        cpu_excess_flow[cpu_edgeList[i]] +=w ;
        *Excess_total += w;
    }


    cout<<"Preflow completed"<<endl;

}
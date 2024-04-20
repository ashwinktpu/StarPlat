#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <queue>

using namespace std;

/* graph as  a input lekar it returns index of edge connecting node 's' and  node 'e' and return the corresponding index in the edgeList and the 
edgeLengh array*/
int findIndex(int V,int E,int s,int e,int *index,int *edgeList,int *edgeLength){
    int start=index[s];
    int end=E-1;
    if(s<V-1) end=index[s+1]-1;
    for(int i=start;i<=end;i++){
        if(edgeList[i]==e) return i;
    }
    return -1;
}


void Akglobal_relabel(int V, int E,int Er,int source, int sink, int *cpu_height, int *cpu_excess_flow, int *Excess_total, bool *mark, bool *scanned,
    int *cpu_index,int *cpu_edgeList,int *cpu_edgeLength,
    int *cpu_Rindex,int *cpu_RedgeList,int *cpu_RedgeLength,
    int *rev_cpu_index,int *rev_cpu_edgeList,int *khush
)
{

    
    for(int u=0;u<V;u++){
        int start=cpu_Rindex[u];
        int end=Er-1;

        if(u<V-1)end=cpu_Rindex[u+1]-1;
        //cout<<"for node "<<u<<"  start an dend indexes are  "<<start<<"    "<<end<<endl;


        for(int j=start;j<=end;j++){
            int v=cpu_RedgeList[j];
            if(!(khush[u] && khush[v])) continue;

            if(cpu_RedgeLength[j]>0 && cpu_excess_flow[u]>0){
                if(cpu_height[u]>cpu_height[v]+1 ){
                //if(cpu_height[u]>cpu_height[v]+1 && cpu_excess_flow[u]>0){
                    int index2=findIndex(V,Er,u,v,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);
                    int index3=findIndex(V,Er,v,u,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);
                    if(index2==-1 || index3==-1){
                        cout<<"hey -1 can't be the index so returning\n";
                        return ;
                    }
                    //cout<<"in global relabellin pushing "<<cpu_RedgeLength[index2]<<"   from  "<<u<<"   to     "<<v<<endl;
                    cout<<"forced the stanford invariant invalid edge from "<<u<<"   to   "<<v<<endl;
                    int d=cpu_RedgeLength[j];
                    cout<<"Stanford weight is "<<d<<endl;
                    

                    cpu_excess_flow[u] = cpu_excess_flow[u] - d;
                    cpu_excess_flow[v] = cpu_excess_flow[v] + d;
                   

                    cpu_RedgeLength[index3]+=d;
                    cpu_RedgeLength[index2]-=d;
                }
            } 
        }
    }
        //printf("Performing backward bfs started\n");
        // performing backwards bfs from sink and assigning height values with each vertex's BFS tree level
        // declaring the Queue 
        std::queue<int> Que;

        // declaring variables to iterate over nodes for the backwards bfs and to store current tree level
        int x,y,current;
        
        // initialisation of the scanned array with false, before performing backwards bfs
        for(int i = 0; i < V; i++)
        {
            scanned[i] = false;
        }

        // Enqueueing the sink and set scan(sink) to true 
        Que.push(sink);
        scanned[sink] = true;
        cpu_height[sink] = 0;
        //bfs routine and assigning of height values with tree level values
        while(!Que.empty() )
        {
            // dequeue
            x = Que.front();
            Que.pop();

            // capture value of current level
            current = cpu_height[x];
            
            // increment current value
            current = current + 1;

            int start=rev_cpu_index[x];
            int end=Er-1;
            if(x<V-1) end=rev_cpu_index[x+1]-1;

            for(int i=start;i<=end;i++){
                int y=rev_cpu_edgeList[i];
                // int index=findIndex(V,2*E,y,x,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);
                int index=findIndex(V,Er,y,x,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);
                if(cpu_RedgeLength[index]>0){
                    if(scanned[y] == false){
                        //assign current as height of y node
                        cpu_height[y] = current;

                        // mark scanned(y) as true
                        scanned[y] = true;

                        // Enqueue y
                        Que.push(y);
                    }
                }
            }
        }

        //printf("Reverse bfs ended\n");
        //printf("Pre check\n");
        // declaring and initialising boolean variable for checking if all nodes are relabeled
        bool if_all_are_relabeled = true;

        for(int i = 0; i < V; i++)
        {
            if(scanned[i] == false && khush[i]==1)
            {
                if_all_are_relabeled = false;
                break;
            }
        }

        // if not all nodes are relabeled
        if(if_all_are_relabeled == false)
        {
            // for all nodes
            for(int i = 0; i < V; i++)
            {
                // if i'th node is not marked or relabeled
               // if( !( (scanned[i] == true) || (mark[i] == true) ) )
               if(scanned[i]==false)
                {
                    // mark i'th node
                    //mark[i] = true;
                    cpu_height[i]=1e8;

                    /* decrement excess flow of i'th node from Excess_total
                     * This shows that i'th node is not scanned now and needs to be marked, thereby no more contributing to Excess_total
                     */
                    if(cpu_excess_flow[i]>0)
                    cout<<" node "<<i<<" not reachable from the source so decreasing its excesstotal by "<<cpu_excess_flow[i]<<endl;

                    *Excess_total = *Excess_total - cpu_excess_flow[i];
                    cpu_excess_flow[i]=0;
                }
            }
        }
       //cout<<"sakshi2\n";        cout<<"atleast cancellation done Sakshi returning \n";
       cout<<"GB done\n";
        return ;

}
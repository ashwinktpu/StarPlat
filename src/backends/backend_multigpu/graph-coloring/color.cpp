#include<bits/stdc++.h>
#include <chrono>

using namespace std;


int main(int argc,char* argv[]){
    int V,E;cin>>V>>E;
    vector<vector<int>> adj;
    adj.resize(V+1);
    int it =0;
    printf("input started\n");
    for(int i=0;i<E;i+=1){
        int src,dst,wt;
        cin>>src>>dst;
        it+=1;
        adj[src].push_back(dst);
        adj[dst].push_back(src);
    }
    printf("%d %d\n",V,E);
    printf("input ended\n");
    printf("clock started\n");
    auto start = std::chrono::high_resolution_clock::now();
    int colored = 0;
    int iter = 0;
    vector<int> color(V+1);
    vector<int> visited(V+1);
    vector<int> visited1(V+1);
    while(colored<V){
        printf("colored in iter %d\n",iter);
        for(int i=0;i<=V;i+=1){
            color[i] = rand();
        }
        for(int i=0;i<=V;i+=1){
            if(!visited[i]){
                int cnt = 0;
                for(auto v : adj[i]){
                    if(visited[v]){
                        cnt+=1;
                    }
                    else if(color[i]>color[v]){
                        cnt+=1;
                    }
                }
                if(cnt==adj[i].size()){
                    printf("%d ",i);
                    visited1[i]=1;
                    colored+=1;
                }
            }
        }
        visited=visited1;
        printf("\n");
        iter+=1;
    }
    printf("%d\n",iter);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    cout << (float)duration.count()/1000 <<" ms"<< endl;

}
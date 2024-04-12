#include <vector>
#include "../graph.hpp"

auto find(int u , int* parent)
{
  int par = parent[u];
  while (u != par ){
    u = par;
    par = parent[u];
  }
  return par;

}
void merge(int u , int v , int* parent , int* rnk
)
{
  u = find(u,parent);
  v = find(v,parent);
  if (rnk[u] > rnk[v] )
    {
    parent[v] = u;
  }
  else
  {
    parent[u] = v;
  }
  if (rnk[u] == rnk[v] )
    {
    int temp = rnk[v];
    rnk[v] = temp + 1;
  }

}
auto qi_seq(std::vector<int> degree , std::vector<int> edges, std::vector<int> &idxs, int n , int* parent , 
  int* rnk)
{
  // removing some previous nonsense.
  std::vector<int> records ;
  records.assign (n*n, -2) ;
  std::vector<int> par;
  par.assign (n, -1) ;
  idxs.assign (n, 0) ;

  int vertex = 0;
  int index = 0;

  while (vertex < n ){
    int deg = degree[vertex];
    while (deg > 0 ){
      int set_u = find(vertex,parent);
      int set_v = find(edges[index],parent);
      if (set_u != set_v )
        {
        merge(set_u,set_v,parent,rnk);

        int dst = edges[index];
        par[dst] = vertex;
      }
      else
      {
        int dst = edges[index];
        records[vertex * n + idxs[vertex]]=dst ;
        idxs[vertex]++ ;

      }
      index++;
      deg--;
    }
    vertex++;
  }
  vertex = 0;
  while (vertex < n ){
    records[vertex * n + idxs[vertex]]=par[vertex] ;
    idxs[vertex]++ ;
    vertex++;
  }
  return records;
}
bool res = false;
auto quicksi(std::vector<int> degree , std::vector<int> records, std::vector<int> idxs, graph& g , int d , int* H , int* F, int n)
{
  if (d >= n )
    {
    return true;
  }
  std::vector<int> temp (idxs[d], 0) ;
  for (int i=0; i < idxs[d]; i++) {
    temp[i] = records[d*n+i] ;
  }
  int par = temp.back();
  // #pragma omp parallel for default(shared) private(H, F)
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    // std::cout<<"HEY\n";
    if (!res && F[v] == 0 && ((d == 0) || (d > 0 && (par == -1 || (par != -1 && g.check_if_nbr(H[par],v))))) )
      {
      if (g.getOutDegree(v) >= degree[d] )
        {
        bool flag = true;
        int index = 0;
        while (index < temp.size() - 1 ){
          int val = temp[index];
          if (flag )
            {
            if (H[val] != -1 && !g.check_if_nbr(v,H[val]) )
              {
              flag = false;
            }
          }
          index++;
        }
        // std::cout<<"HI\n";
        if (flag )
          {
            H[d] = v;
            F[v] = 1;
            if (quicksi(degree,records,idxs,g,d + 1,H,F,n) )
            {
              res = true;
            }
            F[v] = 0;
        }
      }
    }
  }
  return res;

}

int32_t main(int argc, char* argv[]){

    int v, e;
    std::cin >> v >> e;
    std::cout<<v<<" "<<e<<std::endl;
    std::vector<int>degree(v, 0);
    std::vector<int>edges;
    std::vector<std::vector<int>>all_edges(v);
    for(int i = 0; i < e; i++){
      int x, y;
      std::cin>>x>>y;
      all_edges[x].push_back(y);
      degree[x]++;
    }

    for(int i = 0; i < v; i++){
      for(auto j : all_edges[i]){
        edges.push_back(j);
      }
    }

    // All edges represents vector<vector<int> > 

    int* parent = (int*)malloc((v + 1) * sizeof(int));
    int* rnk = (int*)malloc((v + 1) * sizeof(int));
    
    for(int i = 0 ; i < v; i++){
      parent[i] = i;
      rnk[i] = 0;
    }
    std::vector<int> idxs ;
    std::vector<int>records = qi_seq(degree, edges, idxs, v, parent, rnk);

    std::cout<<"Seq Done\n";

    printf ("Examining the record structure\n") ;
    for (int i=0; i<v; i++) {
      for (int j=0; j<v; j++) {
        printf ("%d ", records[i*v+j]) ;
      }
      printf ("\n") ;
    }

    printf ("traversing graph\n") ;
    graph G(argv[1]);
    G.parseGraph();
    std::cout<<G.num_nodes()<<" "<<G.num_edges()<<std::endl;

    int* F = (int*)malloc(G.num_nodes() * sizeof(int));

    int* H = (int*)malloc(G.num_nodes() * sizeof(int));
    for(int i = 0 ; i < G.num_nodes(); i++){
      F[i] = 0;
      H[i] = -1;
    }

    std::cout<<"Start quicksi\n";
    bool res = quicksi(degree, records, idxs, G, 0, H, F, v);
    for(int i = 0 ; i < v; i++){
      std::cout<<H[i]<<" ";
    }
    std::cout<<"\n";
    if(res){
      std::cout<<"YES\n";
    }
    else{
      std::cout<<"NO\n";
    }

    return 0;
}

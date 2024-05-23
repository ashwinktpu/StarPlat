#include"quickSeq.dsl.h"

auto find(int u , int* parent)
{
  int par = parent[u];
  while (u != par ){
    u = par;
    par = parent[u];
  }
  return par;

}
void merge(int u , int v , int* parent , int* rnk)
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
auto qi_seq(std::vector<int> degree, std::vector<int> edges, int n, int* parent, int* rnk)
{
  std::vector<std::vector<int>> records;
  std::vector<int> par;
  int vertex = 0;
  int index = 0;
  while (vertex < n ){
    std::vector<int> temp;
    records.push_back(temp);

    par.push_back(-1);

    vertex++;
  }
  vertex = 0;
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
        records[vertex].push_back(dst);

      }
      index++;
      deg--;
    }
    vertex++;
  }
  vertex = 0;
  while (vertex < n ){
    records[vertex].push_back(par[vertex]);

    vertex++;
  }
  return records;
}

auto quicksi(std::vector<int> degree , std::vector<std::vector<int>> records , Graph& g , int d , int* H , int* F, bool res)
{
  if (d >= records.size() )
  {
    return true;
  }
  std::vector<int> temp;
  temp = records[d];
  int par = temp.back();
  #pragma omp for
  for (int v = g.start_node (); v < g.end_node (); v++) 
  {
    if (!res && F[v] == 0 && ((d == 0) || (d > 0 && (par == -1 || (par != -1 && g.check_if_nbr(H[par],v))))) )
      {
      if (g.num_out_nbrs(v) >= degree[d] )
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
        if (flag )
        {
            int * H1 = new int [g.num_nodes ()] ;
            int * F1 = new int [g.num_nodes ()] ;
            for (int i=0; i<g.num_nodes(); i++) {
              H1[i]=H[i] ;
              F1[i]=F[i] ;
            }
            H1[d] = v;
            F1[v] = 1;
            if (quicksi(degree,records,g,d + 1,H1,F1, res) )
            {
              res = true;
            }
            F1[v] = 0;
        }
      }
    }
  }
  return res;
}

int32_t main(int argc, char* argv[]){

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    std::vector<int>degree;
    std::vector<int>edges;
    int v, e;
    if (world.rank() == 0 ) {
      std::cin >> v >> e;
      degree.assign (v, 0) ;
      std::cout<<v<<" "<<e<<std::endl;
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
    }
    
    boost::mpi::broadcast(world, edges, 0);
    boost::mpi::broadcast(world, degree, 0) ;

    std::cout <<"edges vector made and broadcasted\n" ;
    for (auto &it:edges) {
      std::cout << it << " " ;
    }
    std::cout << std::endl ;

    int* parent = (int*)malloc((v + 1) * sizeof(int));
    int* rnk = (int*)malloc((v + 1) * sizeof(int));

    for(int i = 0 ; i < v; i++){
      parent[i] = i;
      rnk[i] = 0;
    }
    std::vector<std::vector<int>>records = qi_seq(degree, edges, v, parent, rnk);

    std::cout<<"Seq creation successful " << world.rank () << "\n";

    Graph G(argv[1],world, 1);
    std::cout<<G.num_nodes()<<" "<<G.num_edges()<<std::endl;

    int* F = (int*)malloc(G.num_nodes() * sizeof(int));
    int* H = (int*)malloc(G.num_nodes() * sizeof(int));

    for(int i = 0 ; i < G.num_nodes(); i++){
      F[i] = 0;
      H[i] = -1;
    }

    std::cout<<"============================ Start quicksi ==================== \n";
    bool res = quicksi(degree, records, G, 0, H, F, false);
    MPI_Allreduce(&res, &res, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
    std::cout<<"============================ End   quickSi ==================== \n" ;


    /*for(int i = 0 ; i < v; i++){
      std::cout<<H[i]<<" ";
    }*/

    std::cout<<"\n";
    if(res){
      std::cout<<"YES\n";
    }
    else{
      std::cout<<"NO\n";
    }
    return 0;
}

#include"quickSIMPI.dsl.h"

auto find(int u, NodeProperty<int>& parent, boost::mpi::communicator world )
{
  int par = parent.getValue(u);
  while (u != par ){
    u = par;
    par = parent.getValue(u);
  }
  return par;

}
void merge(Graph& g, int u, int v, NodeProperty<int>& parent, 
  NodeProperty<int>& rnk, boost::mpi::communicator world )
{
  u = find(u,parent, world);
  v = find(v,parent, world);
  if (rnk.getValue(u) > rnk.getValue(v) )
  {
    if(world.rank() == g.get_node_owner(v))
    {
      parent.setValue(v,u);
    }
  }
  else
  {
    if(world.rank() == g.get_node_owner(u))
    {
      parent.setValue(u,v);
    }
  }
  if (rnk.getValue(u) == rnk.getValue(v) )
  {
    int temp = rnk.getValue(v);
    if(world.rank() == g.get_node_owner(v))
    {
      rnk.setValue(v,temp + 1);
    }
  }

}
auto qi_seq(Container<int>& degree, Container<int>& edges, Container<int>& idxs, int n, 
  NodeProperty<int>& parent, NodeProperty<int>& rnk, boost::mpi::communicator world )
{
  Container<int> records;
  records.assign(n * n,-2);

  Container<int> par;
  par.assign(n,-1);

  idxs.assign(n,0);

  int vertex = 0;
  int index = 0;
  while (vertex < n ){
    int deg = degree.getValue (vertex);
    while (deg > 0 ){
      int set_u = find(vertex,parent, world);
      int set_v = find(edges.getValue (index),parent, world);
      if (set_u != set_v )
      {
        merge(set_u,set_v,parent,rnk, world);

        int dst = edges.getValue (index);
        par.getValue (dst) = vertex;
      }
      else
      {
        int dst = edges.getValue (index);
        int push_back_idx = idxs.getValue (vertex)++;
        records.getValue (vertex * n + push_back_idx) = dst;
      }
      index++;
      deg--;
    }
    vertex++;
  }
  vertex = 0;
  while (vertex < n ){
    int push_back_idx = idxs.getValue (vertex)++;
    records.getValue (vertex * n + push_back_idx) = par.getValue (vertex);
    vertex++;
  }
  return records;

}
auto quicksi(Container<int>& degree, Container<Container<int>>& records, Graph& g, int d, 
  NodeProperty<int>& H, NodeProperty<int>& F, boost::mpi::communicator world )
{
  if (d >= records.size( ) )
  {
    return true;
  }
  Container<int> temp;
  temp = records.getValue (d);
  bool res = false;
  int par = temp.back( );
  int res_leader_rank = -1 ;

  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (!res && ((d == 0 && F.getValue (v) == 0) || (d > 0 && F.getValue (v) == 0 && (par == -1 || (par != -1 && g.check_if_nbr(H.getValue (par),v))))) )
    {
      if (g.getOutDegree(v) >= degree.getValue (d) )
      {
        bool flag = true;
        int index = 0;
        while (index < temp.size( ) - 1 ){
          int val = temp.getValue (index);
          if (flag )
          {
            if (H.getValue (val) != -1 && !g.check_if_nbr(v,H.getValue (val)) )
            {
              flag = false;
            }
          }
          index++;
        }
      }
      if (flag )
      {
        H.getValue (d) = v;
        F.setValue(v,1);
        if (quicksi(degree,records,g,d + 1,H,F, world) )
        {
          res_leader_rank = world.rank();
          res = true;
        }
        F.setValue(v,0);
      }
    }
  }
  world.barrier();
  int res_leader_rank_temp = res_leader_rank;
  MPI_Allreduce(&res_leader_rank_temp,&res_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&res,1,MPI_C_BOOL,res_leader_rank,MPI_COMM_WORLD);




  return res;

}

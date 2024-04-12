#include"quickSeq.dsl.h"

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
    // if(world.rank() == g.get_node_owner(v))
    // {
      parent.setValue(v,u);
    // }
  }
  else
  {
    // if(world.rank() == g.get_node_owner(u))
    // {
      parent.setValue(u,v);
    // }
  }
  if (rnk.getValue(u) == rnk.getValue(v) )
  {
    // generation issue.
    rnk.atomicAdd (v,1) ;
    // rnk.getValue(v) = rnk.getValue(v)+ 1;
  }

}
auto qi_seq(Graph& queryGraph, Container<int>& degree, Container<int>& idxs, int n, 
  NodeProperty<int>& parent, NodeProperty<int>& rnk, boost::mpi::communicator world )
{
  Container<int> records;
  records.assign(n * n,-2);

  Container<int> par;
  par.assign(n,-1);

  idxs.assign(n,0);

  int vertex = 0;
  int index = 0;
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    for (int v : g.getNeighbors(u)) 
    {
      int set_u = find(u,parent, world);
      int set_v = find(v,parent, world);
      if (set_u != set_v )
      {
        merge(set_u,set_v,parent,rnk, world);

        if(world.rank() == queryGraph.get_node_owner(v))
        {
          parent.setValue(v,u);
        }
      }
      else
      {
        // int push_back_idx = idxs[vertex]++;
        int push_back_idx = idxs.atomicAdd (u, 1) ; // generation issue.
        // change this to set Value.
        records[u * n + push_back_idx] = v;
      }
    }


  }


  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    // int push_back_idx = idxs[vertex]++;
    // change this to set Value.
    int push_back_idx = idxs.atomicAdd (u,1) ; // generation issue.
    records[vertex * n + push_back_idx] = parent.getValue(u);
  }


  return records;

}

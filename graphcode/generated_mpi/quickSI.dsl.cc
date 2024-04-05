#include"quickSI.dsl.h"

auto find(Graph& g, int u, NodeProperty<int>& parent, boost::mpi::communicator world )
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
  if (rnk[u] > rnk[v] )
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
  if (rnk[u] == rnk[v] )
  {
    if(world.rank() == g.get_node_owner(v))
    {
      rnk.atomicAdd(v,1);
    }
  }

}
void merge(Graph& g, int u, int v, NodeProperty<int>& parent, 
  NodeProperty<int>& rnk, boost::mpi::communicator world )
{
  u = find(u,parent, world);
  v = find(v,parent, world);
  int x = u;
  int y = v;
  if(world.rank() == g.get_node_owner(u))
  {
    parent.setValue(u,x);
  }

}

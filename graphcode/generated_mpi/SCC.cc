#include"SCC.h"

void scc(Graph& g, boost::mpi::communicator world )
{
  NodeProperty<bool> vis;
  NodeProperty<int> tim;
  NodeProperty<int> low;
  vis.attachToGraph(&g, (bool)false);
  tim.attachToGraph(&g, (int)-1);
  low.attachToGraph(&g, (int)-1);
  int cur_t = 0;
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    if (tim.getValue(u) == -1 )
    {
      dfs(u,vis,tim,low,g,cur_t, world);

    }
  }


}
auto dfs(int u, NodeProperty<bool>& vis, NodeProperty<int>& tim, NodeProperty<int>& low, 
  Graph& g, int cur_t, boost::mpi::communicator world )
{
  if(world.rank() == g.get_node_owner(u))
  {
    vis.setValue(u,true);
  }
  if(world.rank() == g.get_node_owner(u))
  {
    tim.setValue(u,cur_t);
  }
  if(world.rank() == g.get_node_owner(u))
  {
    low.setValue(u,cur_t);
  }
  cur_t = cur_t + 1;
  for (int v : g.getNeighbors(u)) 
  {
    if (tim.getValue(v) == -1 )
    {
      cur_t = dfs(v,vis,tim,low,g,cur_t, world);
      if (low.getValue(u) > low.getValue(v) )
      {
        if(world.rank() == g.get_node_owner(u))
        {
          low.setValue(u,low.getValue(v));
        }
      }
    }
    else
    if (vis.getValue(v) == true )
    {
      if (tim.getValue(v) < low.getValue(u) )
      {
        if(world.rank() == g.get_node_owner(u))
        {
          low.setValue(u,tim.getValue(v));
        }
      }
    }
  }

  if (low.getValue(u) == tim.getValue(u) )
  {
    dfs1(u,g,vis, world);

  }
  return cur_t;

}
void dfs1(int u, Graph& g, NodeProperty<bool>& vis, boost::mpi::communicator world )
{
  if(world.rank() == g.get_node_owner(u))
  {
    vis.setValue(u,false);
  }
  for (int v : g.getNeighbors(u)) 
  {
    if (vis.getValue(v) == true )
    {
      dfs1(v,g,vis, world);

    }
  }


}

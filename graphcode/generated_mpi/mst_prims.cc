#include"mst_prims.h"

void Prims(Graph& g, boost::mpi::communicator world )
{
  NodeProperty<int> minCost;
  NodeProperty<int> minEdge;
  NodeProperty<bool> visited;
  minCost.attachToGraph(&g, (int)INT_MAX);
  minEdge.attachToGraph(&g, (int)-1);
  visited.attachToGraph(&g, (bool)false);
  EdgeProperty<bool> inMST;
  inMST.attachToGraph(&g, (bool)false);
  int start_v = 0;
  int minCostTemp = INT_MAX;
  Edge minEdgeTemp = -1;
  for (int nbr : g.getNeighbors(start_v)) 
  {
    if (minCost.getValue(nbr) < minCostTemp )
    {
      minCostTemp = minCost.getValue(nbr);
      minEdgeTemp = g.get_edge(start_v, nbr);
    }
  }

  if(world.rank() == g.get_node_owner(start_v))
  {
    minCost.setValue(start_v,minCostTemp);
  }
  if(world.rank() == g.get_node_owner(start_v))
  {
    minEdge.setValue(start_v,minEdgeTemp);
  }
  bool allVisited = false;
  while (!allVisited ){
    int minNodeTemp = -1;
    int minCostTemp = INT_MAX;
    for (int vertex = 0; vertex < g.num_nodes(); vertex ++) 
    {
      if (visited.getValue(vertex) == false )
      {
        if (minCost.getValue(vertex) < minCostTemp )
        {
          minCostTemp = minCost.getValue(vertex);
          minNodeTemp = vertex;
        }
      }
    }

    Edge new_edge = minEdge.getValue(minNodeTemp);
    if(world.rank() == g.get_edge_owner(new_edge))
    {
      inMST.setValue(new_edge,true);
    }
    if(world.rank() == g.get_node_owner(minNodeTemp))
    {
      visited.setValue(minNodeTemp,true);
    }
    for (int vertex : g.getNeighbors(minNodeTemp)) 
    {
      if (visited.getValue(vertex) == false )
      {
        Edge e = g.get_edge(minNodeTemp, vertex);
        if (weight.getValue(e) < minCost.getValue(vertex) )
        {
          if(world.rank() == g.get_node_owner(vertex))
          {
            minCost.setValue(vertex,weight.getValue(e));
          }
          if(world.rank() == g.get_node_owner(vertex))
          {
            minEdge.setValue(vertex,e);
          }
        }
      }
    }

    allVisited = true;
    for (int vertex = 0; vertex < g.num_nodes(); vertex ++) 
    {
      if (visited.getValue(vertex) == false )
      {
        allVisited = false;
      }
    }

  }

}

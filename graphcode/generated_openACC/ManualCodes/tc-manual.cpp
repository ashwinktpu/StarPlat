#include <stdio.h>
#include <iostream>
#include "graph.hpp"


void compute_TC(graph& g)
{
    long triangle_count = 0;


    #pragma acc data copyin(g)
    {
        #pragma acc data copyin( g.indexofNodes[0:g.num_nodes()+1], g.edgeList[0:g.num_edges()] )  copy(triangle_count)
        {
            #pragma acc parallel loop reduction(+ : triangle_count)
            for (int v = 0; v< g.num_nodes(); v++)   //Iterate all the vertices of the graph
            {
                //Iterate all the neighbors of v
                for (int u_ind = g.indexofNodes[v]; u_ind < g.indexofNodes[v+1]; u_ind++)
                {
                    int u = g.edgeList[u_ind];

                    if( u < v )  //only process neighbors (u) of v that are behind v 
                    {
                        //Iterate all the neighbors of v again
                        for (int w_ind = g.indexofNodes[v]; w_ind < g.indexofNodes[v+1]; w_ind++)
                        {
                            int w = g.edgeList[w_ind];
                            
                            if( w > v )     //only process neighbors (w) of v that are in front of v
                            {
                                if(g.check_if_nbr(w, u))    //Check if u and w are connected to form a triangle
                                {
                                    triangle_count = triangle_count + 1;
                                }
                            }
                        }

                    }
                }
            }
        }
    }
    printf("Number of triangles: %ld \n", triangle_count);

}



int main()
{
    graph G("/home/mvk-ubuntu/OpenAcc Practice/GRAPH-SSSP-ATTEMPT-2/GraphSSSP - Parsed Graph/roadNet-CA.txt");   //GermanyRoadud.txt
    G.parseGraph();
    compute_TC(G);
    return 0;
}

#include <iostream>
#include "graph.hpp"
#include <stdio.h>
#include"randomGeneratorUtil.h"
#include <stdlib.h>


using namespace std;

struct return_values 
{
    int numb_nodes_inp;
    int numb_edges_inp;
    edge* edge_array_inp;
    int* offset_array_inp;
};



struct return_values get_input_from_graph ()  //(int& vertex_count, int& edge_count, edge* edge_array, int* offset_array)
{
    graph G("/home/mvk-ubuntu/OpenAcc Practice/GRAPH-SSSP-ATTEMPT-2/GraphSSSP - Parsed Graph/roadNet-CA.txt");
    //graph G("sample_graph.txt");
    //graph G("simple_test_graph.txt");
    G.parseGraph();
    int numb_nodes = G.num_nodes();   //Maximum Node number
    int numb_edges = G.num_edges();   //Total number of edges

    cout << "\nPoint -1 : Graph Parsed\n" << "Number of vertices in graph: " << numb_nodes <<endl;

    //Dynamic array to store all edges of the graph from node-0 to last-node in increasing order. 
    //Array of size [numb_edges+1][2]
    edge* edge_array = new edge[numb_edges];


    map<int,vector<edge>> edge_list = G.getEdges();     //vector<edges> mapped to each node      

    cout << "Point-2: Data structures Allocated\n";

    //Copy edges from edge list to edge_array[][2]
    int edge_i = 0;
    //Traverse nodes
    for (int i=0; i < numb_nodes; i++)    //numb_node is the maximum node number
    { 
        vector<edge>& temp = edge_list[i];

        //Traverse all edges of current node
        for (int j=0; j<temp.size(); j++)
        {
            edge_array[edge_i] = temp[j];

            edge_i++;  
        }

    }

    int* offset_array = G.indexofNodes;   //Copy offset_array pointer


    cout << "Point-3: Data Structure Assigned Values\n";
  /*
  //Print-Test
    //Print first 10 elements of offset_array[]
    for (int i=0; i<10; i++)
    {
      printf("%d  ", offset_array[i]);
    }
    printf("\n");

    //Print first 10 elements of edge_array[]
    for (int i=0; i<28; i++)
    {
      printf("%d  %d  %d\n", edge_array[i].source, edge_array[i].destination, edge_array[i].weight);
    }



    cout << "Number of Nodes: " << numb_nodes << endl;
    cout << "Number of edges: " << numb_edges << endl;
  */

    //Return values:
        // 1)numb_nodes
        // 2)numb_edges
        // 3)offset_array
        // 4)edge_array



    struct return_values input_values = {numb_nodes, numb_edges, edge_array, offset_array};


    return input_values;
}



void compute_BC()              //(set<int>& sourceSet)
{
    struct return_values graph_inputs = get_input_from_graph();
    int vertex_count = graph_inputs.numb_nodes_inp;   //Maximum vertex number
    int edge_count = graph_inputs.numb_edges_inp;     //Total Number of edges
    edge* edge_array = graph_inputs.edge_array_inp;   //Array of size [edge_count][2]
    int* offset = graph_inputs.offset_array_inp;   


    float* BC = new float[vertex_count]();  //Create BC array, initialised wiht 0



   set<int> :: iterator itr;  //Iterator to iterate sourceSet

    //Iterate sourceSet
   //for (itr =sourceSet.begin(); itr!=sourceSet.end(); itr++)   //-------------------------------------CORRECT---
   int src;
   for (src = 0; src<=10; src = src+10)
   {
        //int src = 0;  //Source Vertex------Change as needed

        //Create Required Data Structures
        double* sigma = new double[vertex_count];   
        float* delta = new float[vertex_count];
        int* level = new int[vertex_count];   //level[i] is the level of vertex-i 
        double* bc = new double[vertex_count];
        //int* dist_from_source = new int[vertex_count];

        //Create flags and variables
        int dist_from_source = 0;
        int finished = 0;


        //Initialise data structures
    #pragma acc data copy(delta[0:vertex_count], sigma[0:vertex_count], level[0:vertex_count], bc[0:vertex_count])
    {
        #pragma acc parallel loop
        for (int t=0; t<vertex_count; t++)
        {
            delta[t] = 0;
            sigma[t] = 0; 
            level[t] = -1;
            bc[t] = 0;
        }
    }

        //Initial values for src
        sigma[src] = 1;
        level[src] = 0;
        dist_from_source = 0;

        //printf("Level-0 : %d \n", src);   //--Db

    //-------------------------------------------------------------BFS LOOP----------------------------------------------------------------
    #pragma acc data copy(delta[0:vertex_count], sigma[0:vertex_count], level[0:vertex_count], bc[0:vertex_count]) copyin(vertex_count, edge_count, src, offset[0:vertex_count+1], edge_array[0:edge_count])
    {
        do
        {
            //printf("Level-%d : ", dist_from_source+1);  //--Db--
            finished = 1;
        #pragma acc data copy(finished, dist_from_source)
        {
            #pragma acc parallel loop
            for (int u=0; u<vertex_count; u++)       //Iterate u over all vertices of the graph, (only process vertices at level == dist_from_source)
            {
                if(level[u] == dist_from_source)   //Process only nodes at level dist_from_source
                {
                    //Iterate the neighbors of u
                    for (int edge_index=offset[u]; edge_index < offset[u+1]; edge_index++)
                    {
                        int v = edge_array[edge_index].destination;

                        if ( level[v] == -1 )   //vertex v is unvisited
                        {
                            level[v] = dist_from_source + 1;   //Since this is a neighbor of verte-u and is previouusly unvisited, it belongs to next level
                            finished = 0;  //unvisited neighbor detected
                        }   

                        if ( level[v] == dist_from_source+1 )  //If vertex-v is indeed the neighbor of u. Can be clubbed with above if??
                        {
                            #pragma acc atomic update
                                sigma[v] = sigma[v] + sigma[u];   //----ATOMIC RACE CONDITION-----
                            //printf(" %d ", v);   //--Db
                        }
                    }

                }
            }
        }
            //printf("\n");  //--Db
            dist_from_source++;

        }
        while(!finished);
    //}------------------

        dist_from_source--;  //Correct dist_from_source for RBFS


        //----------------------------------------------------------RBFS LOOP---------------------------------------------------------------------
        //#pragma acc data copy(delta[0:vertex_count], sigma[0:vertex_count], level[0:vertex_count], bc[0:vertex_count]) copyin(vertex_count, edge_count, offset[0:vertex_count+1], edge_array[0:edge_count])
        while (dist_from_source>1)
        {
            //printf("Level-%d : ", dist_from_source);
             
        #pragma acc data copy(dist_from_source)
        {
            #pragma acc parallel loop
            for (int u=0; u<vertex_count; u++)    //Iterate u over all the vertices ( process only vertices at level == dist_from_source-1)
            {
                if( level[u] == dist_from_source-1 )   //Process only nodes at previous-level dist_from_source-1. (Predecessor vertices)
                {
                    double sum = 0.0;   
                    //Iterate the neighbors of u (vertices at level==dist_from_source)
                    for (int edge_index=offset[u]; edge_index < offset[u+1]; edge_index++)
                    {
                        int v = edge_array[edge_index].destination;

                        if ( level[v] == dist_from_source )   //If vertex v is in dist_from_source wave
                        {
                            sum = sum + (1.0 * sigma[u]) / sigma[v] * (1.0 + delta[v]);
                            //printf(" %d ", v);   //--Db
                        }
                    }

                    delta[u] = delta[u] + sum;
                }

            }
        }
            //printf("\n");  //--Db
            dist_from_source--;
        }
    


        //----------------------------------------Accumulate bc[] (Supposed to be part of RBFS loop)-----------------------------------------
        //#pragma acc data copyin(vertex_count) copy(bc[0:vertex_count], delta[0:vertex_count], level[0:vertex_count], src)
        #pragma acc parallel loop
        for (int i = 0; i<vertex_count; i++)
        {
            if( (i != src) && (level[i] != -1) )  //Ensure i is not source vertex or unvisited vertex
            {
                bc[i] =  bc[i] + delta[i]/2.0;
            }
        }
        
    }

        //--------------------------------------------Print bc[] results-----------------------------------------
        for (int i=0; i<vertex_count; i++)
        {
            std::cout << "Vertex-"<< i << " BC: " << bc[i] <<endl; 
        }

    }
}


int main()
{
    
	//int seeds[5];={267649,518682,10978,22382,43958};
	//rand.feedRandomUniqueNum(seeds,20);
	//int nseeds[5];
    
    /*                      ---Removed seeds temporarily
    set<int> seeds;
    randomGeneratorUtil rand;
    rand.feedFromFile(seeds,"../../dataRecords/socJournalSources/sources-USRoad/sources20.txt");
    */
   
    compute_BC();

    return 0;
}








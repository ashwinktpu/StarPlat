#include <stdio.h>
#include <stdlib.h>
#include "graph.hpp"
//#include <atomic>
#include <sys/time.h>
//#include <cuda>
//#include <map>
//#include <vector>

using namespace std;

//#define MAX_SIZE 1000    //Max size of workset[]
//#define vertex_count 7    //5    //7
//#define edge_count 10     //8   //10


struct return_values 
{
    int numb_nodes_inp;
    int numb_edges_inp;
    edge* edge_array_inp;
    int* offset_array_inp;
};



struct return_values get_input_from_graph ()  //(int& vertex_count, int& edge_count, edge* edge_array, int* offset_array)
{
    graph G("./roadNet-CA.txt");
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




void SSSP()
{

    //Parse and get input graph data 
    struct return_values graph_inputs = get_input_from_graph(); //(verttex_count, edge_count, edge_array, offse);

    int vertex_count = graph_inputs.numb_nodes_inp;   //Maximum vertex number
    int edge_count = graph_inputs.numb_edges_inp;     //Total Number of edges
    edge* edge_array = graph_inputs.edge_array_inp;   //Array of size [edge_count][2]
    int* offset = graph_inputs.offset_array_inp;   


    //Create and initialise dist[], modified[] and modified_nxt[]
    int* dist = new int[vertex_count];
    bool* modified = new bool[vertex_count];
    bool* modified_nxt = new bool[vertex_count];
    
    
	#pragma acc data copyout( dist[0:vertex_count] ) // modified[0:vertex_count], modified_nxt[0:vertex_count])
	{
	    #pragma acc parallel loop num_gangs(vertex_count) 
	    for (int i=0; i<vertex_count; i++)
	    {
		dist[i] = __INT_MAX__;   //Initialise all distance to infinity except that of source
	       // modified[i] = false;
	       // modified_nxt[i] = false;
	    }

	}

        

    cout << "Point-4: Distance Values initialised\n";

    //Source assumed as vertex - 0
    int source = 0;   //---------------------------------
    modified[source] = 1;
    dist[source] = 0;



#pragma acc data copyin(g.edge_array[0:edge_count], offset[:vertex_count+1], modified[0:vertex_count], modified_nxt[0:vertex_count]) copy(dist[0:vertex_count]) 
{
    bool finished = false;
    while ( !finished )
    {
        finished = true;

    #pragma acc data copy(finished)
    {   
        #pragma acc parallel loop num_gangs(vertex_count)
        for (int v = 0; v < vertex_count; v++) 
        {
            if ( modified[v] == 1 )
            {
                //Iterate all outgoing edges
                for ( int edge_iter = offset[v]; edge_iter < offset[v+1]; edge_iter++ ) 
                {
                    edge nbr_edge = edge_array[edge_iter];       // g.edgeList[edge_iter] ;
                    int nbr = nbr_edge.destination;
                                                                    //int e = edge_iter;
                    int dist_new = dist[v] + nbr_edge.weight;
                    //bool modified_new = true;
                    
                    if(dist[nbr] > dist_new)
                    {
                        //int oldValue = dist[nbr];

                        #pragma acc atomic write
                            dist[nbr] = dist_new;      //Relax neighbor                                       
                        
                        //#pragma acc atomic write
                        modified_nxt[nbr] = 1;    //Mark neighbor as modified
                        
                        #pragma acc atomic write
                            finished = false ;        //Unfinished
                        
                                                        //atomicMin(&dist[nbr],dist_new);
                    }
                }
            }
        }


        //Swap modified[] and modified_nxt[]
        /*
        bool* temp = modified_nxt;
        modified_nxt = modified;
        modified = temp;
        */


        #pragma acc parallel loop
        for (int v = 0; v < vertex_count; v ++)    //Copy modified_nxt[] to modified[]. Re-initialise all of modified_nxt[] = 0
        { 
            modified[v] =  modified_nxt[v] ;
            modified_nxt[v] = 0 ;
        }

    } 


    }
}

    cout << "FINAL POINT: Algorithm Finished Running\n"; 

    //cout <<  ": Number of iterations:";  cout << iteration << endl;

    /*
   //Print shortest distances to each vertex --
    for (int i=0; i<200; i++)
    {
        printf(" Shortest Path to Vertex %d >>> %d \n",i, dist[i]);
    }*/


    //------------------Write shortest paths to output file
    char *outputfilename = "output-shortest-paths-GPU.txt";
    FILE *outputfilepointer;
    outputfilepointer = fopen(outputfilename, "w");

    for (int i = 0; i <vertex_count; i++)
    {
        fprintf(outputfilepointer, "Vertex %d:  %d\n", i, dist[i]);
    }
    
}



int main()
{
    SSSP();

    return 0;
}

    

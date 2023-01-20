#include <iostream>
#include "graph.hpp"
#include <sys/time.h>


using namespace std;


struct return_values 
{
    // int numb_nodes_inp;
    // int numb_edges_inp;
    // edge* edge_array_inp;
    // int* offset_array_inp;

    int numb_nodes_inp;
    int numb_edges_inp;
    int* source_list;
    int* reverse_offset_arr;
    int* forw_offset_arr;

};



struct return_values get_input_from_graph ()  //(int& vertex_count, int& edge_count, edge* edge_array, int* offset_array)
{
    graph G("./roadNet-CA.txt");
    //graph G("Sample_Small_Graph.txt");
    G.parseGraph();
    int numb_nodes = G.num_nodes();   //Maximum Node number
    int numb_edges = G.num_edges();   //Total number of edges

    cout << "\nPoint -1 : Graph Parsed\n" << "Number of vertices in graph: " << numb_nodes <<endl;

    //Dynamic array to store all out-going edges of the graph from node-0 to last-node in increasing order. 
    //Array of size [numb_edges+1][2]
    //edge* edge_array = new edge[numb_edges];

    

    //map<int,vector<edge>> edge_list = G.getEdges();     //vector<edges> mapped to each node      

    int* src_list = G.srcList;    //Stores souce node corresponding to edge number. Edges to same destination node are grouped togethor.
    int* rev_offset_arr = G.rev_indexofNodes;
    int* forward_offset_arr = G.indexofNodes;

    cout << "Point-2: Data structures Allocated\n";

    /*
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

    //int* offset_array = G.indexofNodes;   //Copy offset_array pointer

    int* rev_offset_arr = G.rev_indexofNodes;

    cout << "Point-3: Data Structure Assigned Values\n"; */
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
    


    struct return_values input_values = {numb_nodes, numb_edges, src_list, rev_offset_arr, forward_offset_arr};


    return input_values;
}




void compute_pagerank(float beta, float delta, int maxIter)
{
    struct return_values graph_input_values = get_input_from_graph();

    int num_nodes = graph_input_values.numb_nodes_inp;
    int num_edges = graph_input_values.numb_edges_inp;

    //REVERSE CSR: INCOMING EDGES TO EACH VERTEX
    int* src_list = graph_input_values.source_list;
    int* reverse_offset_arr = graph_input_values.reverse_offset_arr;
    
    //Forward Offset Array
    int* forw_offset_arr = graph_input_values.forw_offset_arr;


    //START MEASURE TIME
    struct timeval t1, t2;
	double elapsed = 0.0;
	gettimeofday(&t1, NULL);
    //--------------------

    //Create pageRank[] and pageRank_nxt[] arrays

    float* pageRank =   new float[num_nodes];   //(float*)malloc(num_nodes*sizeof(float));  //new float(num_nodes);
    float* pageRank_nxt =   new float[num_nodes]; //(float*)malloc(num_nodes*sizeof(float));  //new float(num_nodes);

    cout << "Pagerank data structures created \n";


    //Initialise pageRank[] array
    #pragma acc data copyout(pageRank[0:num_nodes])
    {
    #pragma acc parallel loop
    for (int i=0; i<num_nodes; i++)
    {
      pageRank[i] = 1.0/num_nodes;     
    }
    }
    
float diff;
//Copy all graph data structures
#pragma acc data copyin( src_list[0:num_edges+1], forw_offset_arr[0:num_nodes+1], reverse_offset_arr[0:num_nodes+1] )  copy(pageRank[0:num_nodes]) copyin(pageRank_nxt[0:num_nodes])  //copy(pageRank[0:num_nodes], pageRank_nxt[0:num_nodes])
{
    int itercount = 0;   //counts the number of while iterations over the entire graph
    //float diff;      //diff is an idication of how much the pagerank[] array has changed since the previous iteration
    do
    {
        diff = 0.0000000000;   
        //Traverse all the nodes of the graph
        #pragma acc data copy(diff)           //copyin( pageRank[0:num_nodes] )    copyout(pageRank_nxt[0:num_nodes])  copy(diff)    //pagerank[]] is used from previous iteration. pagerank_nxt[] is copied to pagerank[] after loop. 
        {
        #pragma acc parallel loop reduction(+:diff)    //num_gangs(num_nodes/1024) //reduction(+:diff)
        for (int v = 0; v < num_nodes; v++)
        {
            float sum = 0.00000000;  //float sum = 0.000000000;

            //Traverse all the in-neighbors of each node, calculate sum
            for (int nbr_index = reverse_offset_arr[v]; nbr_index < reverse_offset_arr[v+1]; nbr_index++)
            {
                int nbr = src_list[nbr_index];
                sum = sum + pageRank[nbr]/(forw_offset_arr[nbr+1] - forw_offset_arr[nbr]) ;    //pageRank[nbr] / num_of_out_neighbors(nbr) 
                
            }

            float val = (1-delta)/num_nodes + delta*sum;   //val = Current pagerank calculated value. pagerank[v] is old value. For now.
            diff =  diff + val - pageRank[v];  
            pageRank_nxt[v] = val;    //Set pagerank_nxt[v] to new current pagerank value
            

        }
    
        //cout << "Iteration " << itercount;   //Debugging Purpose
        //pageRank = pageRank_nxt;    //Shallow copy pagerank_nxt to pagerank   
        
        #pragma acc parallel loop
        for (int node = 0; node < num_nodes; node ++) 
        {
            pageRank[node] = pageRank_nxt[node] ;
        }
        }
            
        itercount++;
    } 
    while((diff>beta) && (itercount<maxIter));
    //cout << "Numbere of iterations: " << itercount << "Diff value: " << diff << "\n";
    //printf("Number of iterations: %d , Diff Value : %0.9lf \n", itercount, diff);

}

    //-------STOP MEASURE TIME-----------
    gettimeofday(&t2, NULL);
	elapsed = (t2.tv_sec - t1.tv_sec) * 1000.0  +  (t2.tv_usec - t1.tv_usec) / 1000.0;  //(in ms)
    cout << "Time Elapsed to Run Algorithm: " << elapsed << "ms" << endl;
    //-----------------------------------

    
    //Write shortest paths to output file
    char* outputfilename = "PAGE-RANK-GPU.txt";
    FILE* outputfilepointer;
    outputfilepointer = fopen(outputfilename, "w");

    for (int i = 0; i < num_nodes; i++)
    {
        fprintf(outputfilepointer, "Vertex %d:  %0.9lf \n", i, pageRank[i]);
    }

}



int main()
{
            //Debugging Purpose

      /*
      struct return_values inputs = get_input_from_graph();

      cout << inputs.numb_edges_inp << endl;
      cout << inputs.numb_nodes_inp << endl;
      
      int* src_list = inputs.source_list;
      int* rev_offset = inputs.reverse_offset_arr;
      int* offset = inputs.forw_offset_arr;

      cout << "Source List: \n";

  */

      /* Print forward offset array
      for (int i=0; i < inputs.numb_nodes_inp; i++)
      {
          cout << offset[i+1] - offset[i] << "  ";
      } 
      */

      /*
            //Print reverse offset array
      for (int i=0; i<inputs.numb_nodes_inp+1; i++)
      {
        cout << rev_offset[i] << "  ";
      }
      */
      
      /* Print source list
      for (int i=0; i < inputs.numb_edges_inp; i++)
      {
          cout << src_list[i] << "  ";
      }
      */

      /*      
      //Print in-neighbors corresponding to each node
      for (int i=0; i < inputs.numb_nodes_inp; i++)
      {
          cout << "Node-" << i << ": ";
          int num_in_neighbors = rev_offset[i+1] -  rev_offset[i];  
          
          //Print sources of in-neighbors of node-i
          for (int j=rev_offset[i]; j < rev_offset[i] + num_in_neighbors; j++)
          {
              cout << src_list[j] << "  ";
          }

          cout << endl;
      }
      */
 


    compute_pagerank(0.001, 0.85, 100);

    return 0;
}


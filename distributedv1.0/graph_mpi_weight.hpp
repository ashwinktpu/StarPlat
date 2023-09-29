#ifndef _GRAPH_

#define _GRAPH_

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>
// #include "omp.h"

// using namespace std;
using namespace std;

class edge
{
public:
    int source;
    int destination;
    int weight;
    int process;
};

class graph
{
private:
    int original_nodesTotal;
    int diff_Total;
    int padded_nodesTotal;
    int edgesTotal;
    int degree_max;
    int np;

    char *filePath;
    std::map<int, std::vector<edge>> edges;
    std::map<int, int> global_to_local;
    std::map<int, int> local_to_global;

public:
    int *indexofNodes;
    int *edgeList;
    int *edgeLen;
    int *rev_indexofNodes; /*stores destination corresponding to edgeNo.
                         required for iteration over out neighbours */
    int *srcList;          /*stores source corresponding to edgeNo.
                                 required for i teration over in neighbours */
    int *process_list;
    explicit graph(char *file)
    {
        filePath = file;
        original_nodesTotal = 0;
        diff_Total = 0;
        edgesTotal = 0;
        degree_max = 0;
        padded_nodesTotal = 0;
        np = 1;
        // std::cerr << "World size: " <<np << std::endl;
    }

    std::vector<edge> getNeighbours(int nodeptr)
    {
        return edges[global_to_local[nodeptr]];
    }

    int get_local(int node_id)
    {
        return global_to_local[node_id];
    }

    std::map<int, int> get_local_to_global()
    {
        return local_to_global;
    }

    std::map<int, int> get_global_to_local()
    {
        return global_to_local;
    }

    int getDiffTotal()
    {
        return diff_Total;
    }

    std::map<int, std::vector<edge>> getEdges()
    {
        return edges;
    }
    bool exists_here(int nodeptr)
    {

        if (edges.find(nodeptr) != edges.end())
        {
            return true;
        }
        return false;
    }
    int *getProcessList()
    {

        return process_list;
    }

    int *getEdgeLen()
    {
        return edgeLen;
    }
    int *getEdgeList()
    {
        return edgeList;
    }
    int *getSrcList()
    {
        return srcList;
    }
    int *getIndexofNodes()
    {
        return indexofNodes;
    }
    int *getRevIndexofNodes()
    {
        return rev_indexofNodes;
    }

    int num_nodes()
    {
        return padded_nodesTotal;
    }
    int ori_num_nodes()
    {
        return original_nodesTotal;
    }
    int num_edges()
    {
        return edgesTotal;
    }
    int max_degree()
    {
        return degree_max;
    }

    void parseGraph()
    {
        std::ifstream infile;
        infile.open(filePath);
        if (!infile)
        {
            std::cerr << "Cannot open the file " << filePath << std::endl;
            exit(0);
        }
        std::string line;
        std::stringstream ss;
        std::cerr << "graph recognized \n";
        int counter = 0;
        while (getline(infile, line))
        {

            if (line[0] < '0' || line[0] > '9')
            {
                int k = 3;
                while (k--)
                {
                    std::cerr << "ignoring " << line << std::endl;
                    getline(infile, line);
                    continue;
                }
            }

            ss.clear();
            ss << line;
            edgesTotal++;

            edge e;
            int global_source;
            int global_destination;
            int local_source;
            int local_destination;
            int weight;
            int process;
            int rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            if (ss >> global_source && ss >> global_destination && ss >> weight && ss >> process)
            {

                // map the global source to the local source with a counter ;
                if (global_to_local.find(global_source) == global_to_local.end())
                {

                    global_to_local[global_source] = counter++;
                    // cerr << "new vertex found " << counter << endl ;
                    diff_Total++;
                }

                local_source = global_to_local[global_source];

                if (global_to_local.find(global_destination) == global_to_local.end())
                {

                    global_to_local[global_destination] = counter++;
                    // cerr << "new vertex found " << counter << endl ;

                    if (rank == process)
                    {
                        diff_Total++;
                    }
                }

                // cerr << "counter_val " << counter <<" " << global_source << " " <<global_destination << " || " ;

                local_destination = global_to_local[global_destination];
                if (local_source > original_nodesTotal)
                    original_nodesTotal = local_source;
                if (local_destination > original_nodesTotal)
                    original_nodesTotal = local_destination;
                e.source = local_source;
                e.destination = local_destination;
                e.weight = weight;
                e.process = process;
                edges[local_source].push_back(e);
            }
        }

        cerr << "inside this rank " << diff_Total << endl;

        // edge list made.
        original_nodesTotal += 1;
        if (original_nodesTotal % np == 0)
        {
            padded_nodesTotal = original_nodesTotal;
        }
        else
        {
            padded_nodesTotal = original_nodesTotal + np - (original_nodesTotal % np);
        }

        for (int i = 0; i < padded_nodesTotal; i++)
        {
            std::vector<edge> edgeOfVertex = edges[i];

            if (edges[i].size() > degree_max)
                degree_max = edges[i].size();

            sort(edgeOfVertex.begin(), edgeOfVertex.end(),
                 [](const edge &e1, const edge &e2)
                 {
                     if (e1.source != e2.source)
                         return e1.source < e2.source;

                     return e1.destination < e2.destination;
                 });
        }

        std::cerr << "Edges : " << ' ' << edgesTotal << "\n Vertices : " << original_nodesTotal << std::endl;
        indexofNodes = new int[padded_nodesTotal + 1];
        edgeList = new int[edgesTotal];
        edgeLen = new int[edgesTotal];
        process_list = new int[edgesTotal];
        rev_indexofNodes = new int[padded_nodesTotal + 1];
        srcList = new int32_t[edgesTotal];

        int edge_no = 0;

        for (int i = 0; i < padded_nodesTotal; i++)
        {
            std::vector<edge> edgeofVertex = edges[i];

            indexofNodes[i] = edge_no;

            std::vector<edge>::iterator itr;

            for (itr = edgeofVertex.begin(); itr != edgeofVertex.end(); itr++)
            {

                edgeList[edge_no] = (*itr).destination;

                edgeLen[edge_no] = (*itr).weight;

                process_list[edge_no] = (*itr).process;
                edge_no++;
            }
        }
        indexofNodes[padded_nodesTotal] = edge_no;
        for (int i = 0; i < padded_nodesTotal; i++)
        {
            std::vector<std::pair<int, int>> vect;
            for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
                vect.push_back(std::make_pair(edgeList[j], edgeLen[j]));
            std::sort(vect.begin(), vect.end());

            for (int j = 0; j < vect.size(); j++)
            {
                edgeList[j + indexofNodes[i]] = vect[j].first;
                edgeLen[j + indexofNodes[i]] = vect[j].second;
            }

            vect.clear();
            std::vector<std::pair<int, int>>().swap(vect);
        }

        cerr << "generating reverse list\n";
        for (int i = 0; i < padded_nodesTotal + 1; i++)
            rev_indexofNodes[i] = 0;

        /* Prefix Sum computation for in neighbours
           Loads rev_indexofNodes and srcList.
        */

        /* count indegrees first */
        int32_t *edge_indexinrevCSR = new int32_t[edgesTotal];

        for (int i = 0; i < padded_nodesTotal; i++)
        {

            for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
            {
                int dest = edgeList[j];
                int temp = __sync_fetch_and_add(&rev_indexofNodes[dest], 1);
                edge_indexinrevCSR[j] = temp;
            }
        }
        /* convert to revCSR */
        int prefix_sum = 0;
        for (int i = 0; i < padded_nodesTotal; i++)
        {
            int temp = prefix_sum;
            prefix_sum = prefix_sum + rev_indexofNodes[i];
            rev_indexofNodes[i] = temp;
        }
        rev_indexofNodes[padded_nodesTotal] = prefix_sum;

        /* store the sources in srcList */
        for (int i = 0; i < padded_nodesTotal; i++)
        {
            for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
            {
                int dest = edgeList[j];
                int index_in_srcList = rev_indexofNodes[dest] + edge_indexinrevCSR[j];
                srcList[index_in_srcList] = i;
            }
        }
        std::cerr << "done\n";
        for (int i = 0; i < padded_nodesTotal; i++)
        {
            std::vector<int> vect;
            vect.insert(vect.begin(), srcList + rev_indexofNodes[i], srcList + rev_indexofNodes[i + 1]);
            std::sort(vect.begin(), vect.end());
            for (int j = 0; j < vect.size(); j++)
                srcList[j + rev_indexofNodes[i]] = vect[j];

            vect.clear();
            std::vector<int>().swap(vect);
        }
        delete[] edge_indexinrevCSR;

        cerr << edges.size() << " this is what I should get\n";
        std::cerr << "okay" << std::endl;
    }
};
#endif

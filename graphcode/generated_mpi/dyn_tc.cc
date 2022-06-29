#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<cmath>
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdbool>
#include <fstream>
#include <sys/time.h>
#include <vector>
#include <map>
#include <utility>
#include <boost/mpi.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
#include "../graph.hpp"

#define btoa(x) ((x)?"true":"false")

using namespace std;
namespace mpi = boost::mpi;

int *local_index,*local_weight,*local_edgeList;

int *local_diff_index=NULL, *local_diff_edgeList=NULL, *local_diff_weight=NULL;
int *local_PNCS, *local_PNDCS;

int totalBatches;

vector <int> getNeighbors(int v)
{
	// v is indexed locally to the process
    vector <int> neighbors;
	if (local_index)
	{
		for (int i=local_index[v]-local_index[0]; i<local_index[v+1]-local_index[0]; i++)
		{
			if (local_edgeList[i] != INF)
			{
				neighbors.push_back(local_edgeList[i]);
			}
		}
	}
	if (local_diff_index)
	{
		for (int i=local_diff_index[v]; i<local_diff_index[v]; i++)
		{
			if (local_diff_edgeList[i] != INF)
			{
				neighbors.push_back(local_edgeList[i]);
			}
		}
	}
    return neighbors;
}


void printArray(int rank, int *arr, int size, int type)
{
	string s = "\t[" + to_string(rank) + "] ";
	if (type==0) s = s + "csr:";
	else s = s + "diff_csr:";
	for (int i=0; i<size; i++)
	{
		s = s + " " + to_string(arr[i]) ;
	}
	s = s + "\n";
	std::cout << s;
}


/*
 * Computes TC sequentially
 */
int COMPUTE_TC(int np,int my_rank,int part_size,int num_nodes,mpi::communicator &world)
{
    // solve sequentially
	
	// x -> y -> z triangle ?
	// forall(v in g.nodes()) {
	// 	forall(u in g.neighbors(v).filter(u < v)) {
	// 		forall(w in g.neighbors(v).filter(w > v)) {
	// 			if (g.is_an_edge(u, w)) {
	// 				triangle_count += 1;
	// 			}
	// 		}
	// 	}
	// }

	std::vector <std::map<int,std::vector<int>>> send_data(np);
	std::vector <std::map<int,std::vector<int>>> receive_data(np);

	// printf("[%d]: Computing sequential TC\n", my_rank);

	// TODO: add diff_csr edges too
	for (int i=0; i<part_size; i++)
	{
		int v = my_rank*(num_nodes/np) + i;
		
		// for (int e1 = local_index[i]-local_index[0]; e1 < local_index[i+1]-local_index[0]; e1++)
		// {
        //     int u = local_edgeList[e1];
		// 	if (u != INF && u < v)
		// 	{
		// 		for (int e2 = local_index[i]-local_index[0]; e2 < local_index[i+1]-local_index[0]; e2++)
		// 		{
        //             int w = local_edgeList[e2];
		// 			if (w != INF && w > v)
		// 			{
		// 				int u_procno = u / (num_nodes/np);
		// 				if (u_procno >= np) u_procno = np-1;
		// 				send_data[u_procno][u].push_back(w);
		// 			}
		// 		}
		// 	}
		// }

		for (int u : getNeighbors(i))
		{
			if (u<v)
			{
				for (int w : getNeighbors(i))
				{
					if (w>v)
					{
						int u_procno = u / (num_nodes/np);
						if (u_procno >= np) u_procno = np-1;
						send_data[u_procno][u].push_back(w);
					}
				}
			}
		}
	}

	// printf("[%d]: Computed send_data \n", my_rank);

	MPI_Barrier(MPI_COMM_WORLD);

	all_to_all(world, send_data, receive_data);

	int local_triangle_count = 0;

	for (int i=0; i<np; i++)
	{
		int v_procno = i;
		std::map <int, std::vector<int>> pairs = receive_data[i];
		
		for (auto &u_and_ws : pairs)
		{
			int u = u_and_ws.first;
			int u_index = u - my_rank*(num_nodes/np);

			for (int &w : u_and_ws.second)
			{
				bool found = false;
				for (int j=local_index[u_index]-local_index[0]; j<local_index[u_index+1]-local_index[0]; j++)
				{
					if (local_edgeList[j] == w) 
					{
						found = true;
                        // printf("Found triangle with u=%d and w=%d\n", u, w);
						break;
					}
				}
				if (!found && local_diff_index)
				{
					for (int j=local_diff_index[u_index]; j<local_diff_index[u_index+1]; j++)
					{
						if (local_diff_edgeList[j] == w) 
						{
							found = true;
							break;
						}
					}
				}
				if (found) 
				{
					local_triangle_count++;
				}
			}
		}
	}

	for (int i=0; i<np; i++)
	{
		send_data[i].clear();
		receive_data[i].clear();
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	int global_triangle_count=0;

	MPI_Reduce(&local_triangle_count, &global_triangle_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
	{
		printf("Global triangle count = %d\n", global_triangle_count);
	}

    return global_triangle_count;
}


int distributeBatchUpdates(int np,int my_rank,int num_nodes,int part_size,int k,int elements,char type,
							int *perProcessSize,int *perProcessIndex,std::vector<update> &updateEdges,
							int **updateMatrixSrc,int **updateMatrixDest,int **updSrcs,int **updDests)
{
	int updSize;
	if (my_rank == 0)
	{
		// printf("\nA new batch update with op=%c k = %d, elements = %d\n",type,k,elements);
		for (int i=0; i<np; i++)
		{
			perProcessSize[i] = 0;
			perProcessIndex[i] = 0;
		}

		int c1 = 0, c2 = 0, c3 = 0;
		for (int i=0; i<elements; i++)
		{
			int pos = k+i;
			update u = updateEdges[pos];
			
			if (u.source>=num_nodes || u.destination>=num_nodes) continue;

			if (u.type == type)
			{
				c1 ++;
				int src_rank = u.source/part_size;
				if (src_rank >= np) src_rank = np-1;
				perProcessSize[src_rank]++;
			}
		}

		for (int i=0; i<np; i++)
		{
			c2 += perProcessSize[i];
			// if (perProcessSize[i] > 0) printf("size demand for i=%d, size=%d\n", i, perProcessSize[i]);
			updateMatrixSrc[i] = new int[perProcessSize[i]];
			updateMatrixDest[i] = new int[perProcessSize[i]];
		}

		for (int i=0; i<elements; i++)
		{
			int pos = k+i;
			update u = updateEdges[pos];

			if (u.source>=num_nodes || u.destination>=num_nodes) continue;

			if (u.type == type)
			{
				c3++;
				int src_rank = u.source/part_size;
				if (src_rank >= np) src_rank = np-1;
				updateMatrixSrc[src_rank][perProcessIndex[src_rank]] = u.source;
				updateMatrixDest[src_rank][perProcessIndex[src_rank]] = u.destination; 
				perProcessIndex[src_rank]++;
			}
		}

		for (int i=1; i<np; i++)
		{
			MPI_Send(&perProcessSize[i],1,MPI_INT,i,0,MPI_COMM_WORLD);
			if (perProcessSize[i] > 0)
			{
				MPI_Send(updateMatrixSrc[i],perProcessSize[i],MPI_INT,i,1,MPI_COMM_WORLD);
				MPI_Send(updateMatrixDest[i],perProcessSize[i],MPI_INT,i,2,MPI_COMM_WORLD);
			}
		}

		updSize=perProcessSize[0];
		// printf("[%d], updSize = %d\n", my_rank, updSize);
		*updSrcs = new int[updSize];
		*updDests = new int[updSize];
		for (int i=0; i<updSize; i++)
		{
			(*updSrcs)[i] = updateMatrixSrc[0][i];
			(*updDests)[i] = updateMatrixDest[0][i];
			// printf("Rank [%d] delete edge between %d and %d\n", my_rank, (*updSrcs)[i], (*updDests)[i]);
		}

		for (int i=0; i<np; i++)
		{
			assert(perProcessIndex[i] == perProcessSize[i]);
			// printf("[%d]: Is index=%d  ==  size=%d ?\n", i, perProcessIndex[i], perProcessSize[i]);
		}

		assert(c1 == c2);
		assert(c2 == c3);

		// printf("Is c1=%d == c2=%d == c3=%d ?\n", c1, c2, c3);

		for (int i=0; i<np; i++)
		{
			delete [] updateMatrixSrc[i];
			delete [] updateMatrixDest[i];
		}
	}
	else
	{
		MPI_Recv (&updSize,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		// printf("[%d], updSize = %d\n", my_rank, updSize);
		if (updSize > 0)
		{
			*updSrcs = new int[updSize];
			*updDests = new int[updSize];
			MPI_Recv (*updSrcs,updSize,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv (*updDests,updSize,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
	}
	return updSize;
}

void deleteCSR(int np,int my_rank,int part_size,int num_nodes,int updSize,int *updSrcs, int *updDests)
{
	// delete edges

	for (int i=0; i<updSize; i++)
	{
		int remote_src = updSrcs[i];
		int remote_dest = updDests[i];

		// 0 indexing here
		int local_src = remote_src - my_rank*(num_nodes/np);

		// printf("Rank [%d]: To be removed: r_src=%d, r_dest=%d, l_src=%d\n", my_rank, remote_src, remote_dest, local_src);

		bool found = false;

		for (int j=local_index[local_src]-local_index[0]; j<local_index[local_src+1]-local_index[0]; j++)
		{
			if (local_edgeList[j] == remote_dest)
			{
				// printf("Rank [%d]: Destination found for src=%d and dest=%d\n", my_rank, remote_src, remote_dest);
				local_edgeList[j] = INF;
				local_PNCS[local_src]++;
				found = true;
			}
		}
		if (!found) // TODO: diff_csr might contain duplicate edge?
		{
			for(int j=local_diff_index[local_src]; j<local_diff_index[local_src+1]; j++)
			{
				int nbr = local_diff_edgeList[j];
				if(nbr == remote_dest)
				{
					// printf("Rank [%d]: Destination found for src=%d and dest=%d (diff_csr)\n", my_rank, remote_src, remote_dest);
					local_PNDCS[local_src]++;   // Nobita: extra book-keeping
					int index = local_diff_index[local_src+1] - local_PNDCS[local_src];
					local_diff_edgeList[j] = local_diff_edgeList[index];
					local_diff_weight[j] = local_diff_weight[index];
					local_diff_edgeList[index] = INF;
					found = true;
				}
			}
		}

		// if (!found)
		// {
		// 	printf("ERROR [%d]: Edge between %d and %d not found\n", my_rank, remote_src, remote_dest); // TODO
		// }

	}
}

void addCSR(int np,int my_rank,int part_size,int num_nodes,int updSize,int *updSrcs, int *updDests,int *local_needed_info,int *local_PNDCS_Total)
{
	// add edges 

	// updSrcs <-> updDests edge formation

	for (int i=0; i<part_size; i++)
	{
		local_needed_info[i] = 0;
	}
	for (int i=0; i<updSize; i++)
	{
		local_needed_info[updSrcs[i] - my_rank*(num_nodes/np)]++;   // TODO: my_rank*num_nodes/np
	} 

	int local_diff_csr_new_add = 0;

	for (int i=0; i<part_size; i++)
	{
		local_PNDCS_Total[i] = local_diff_index[i+1] - local_diff_index[i];

		int avail = local_PNCS[i];
		int diff_avail = local_PNDCS[i];
		int needed = local_needed_info[i];

		int new_addition = needed - avail - diff_avail;

		// printf("Node [%d]: avail=%d, diff_avail=%d, needed=%d, new_addition=%d\n", (num_nodes/np)*my_rank+i, avail, diff_avail, needed, new_addition);

		if (new_addition > 0)
		{
			// after filling both spaces, calculating extra space needed in diff_csr
			local_PNDCS_Total[i] += new_addition;
			local_diff_csr_new_add += new_addition;
		}
	}
	// printf("Rank [%d]: Requirement new diff csr size = %d\n", my_rank, local_diff_index[part_size] - local_diff_index[0] + local_diff_csr_new_add);

	if (local_diff_csr_new_add > 0)
	{
		int local_diff_csr_new_size = local_diff_index[part_size] - local_diff_index[0] + local_diff_csr_new_add;

		int *local_diff_edgeList_new = new int[local_diff_csr_new_size];
		int *local_diff_index_new = new int [part_size+1];
		int *local_diff_weight_new = new int[local_diff_csr_new_size];

		// starting index will be same (actually zero only)
		local_diff_index_new[0] = local_diff_index[0];

		for (int i=0; i<part_size; i++)
		{
			local_diff_index_new[i+1] = local_diff_index_new[i] + local_PNDCS_Total[i];

			int old_start = local_diff_index[i];
			int new_start = local_diff_index_new[i];
			int old_total = local_diff_index[i+1] - local_diff_index[i];
			int new_total = local_PNDCS_Total[i];

			// getting the old diff_csr values
			for (int j=0; j<old_total; j++)
			{
				local_diff_edgeList_new[new_start+j] = local_diff_edgeList[old_start+j];
				local_diff_weight_new[new_start+j] = local_diff_weight[old_start+j];
			}

			// setting the remaining values as INF
			for (int j=old_total; j<new_total; j++)
			{
				local_diff_edgeList_new[new_start+j] = INF;
			}

			local_PNDCS[i] += new_total - old_total;
		}

		free(local_diff_edgeList);
		local_diff_edgeList = local_diff_edgeList_new;

		free(local_diff_index);
		local_diff_index = local_diff_index_new;

		free(local_diff_weight);
		local_diff_weight = local_diff_weight_new;

	}


	for (int i=0; i<updSize; i++)
	{
		int remote_src = updSrcs[i];
		int remote_dest = updDests[i];

		// 0 indexing here
		int local_src = remote_src - my_rank*(num_nodes/np);

		// printf("Rank [%d]: Yet to be added: r_src=%d, r_dest=%d, l_src=%d\n", my_rank, remote_src, remote_dest, local_src);

		bool added = false;

		// if only there is enough space
		if (local_PNCS[local_src] > 0) 
		{
			for (int j=local_index[local_src]-local_index[0]; j<local_index[local_src+1] - local_index[0]; j++)
			{
				if (local_edgeList[j] == INF)
				{
					// printf("Rank [%d]: Empty space found for adding adge between %d and %d\n", my_rank, remote_src, remote_dest);
					local_edgeList[j] = remote_dest;
					// TODO: add weight too
					local_PNCS[local_src]--;
					added = true;
					break;
				}
			}
		}

		if (!added)
		{
			if (local_PNDCS[local_src] <= 0)
			{
				printf("Rank [%d]: ERROR! Not enough space to add edge between %d and %d\n", my_rank, remote_src, remote_dest);
			}

			int indexToAdd = local_diff_index[local_src+1] - local_PNDCS[local_src];
			local_diff_edgeList[indexToAdd] = remote_dest;
			// local_diff_weight[indexToAdd] = weightt;

			local_PNDCS[local_src]--;

		}
	}
}


int COMPUTE_TC_CHANGE(int np,int my_rank,int part_size,int num_nodes,int updSize,int *updSrcs,int *updDests,mpi::communicator &world)
{
	int csr_size = local_index[part_size]-local_index[0];
	int diff_csr_size = local_diff_index[part_size];

	bool *modified = new bool[csr_size];
	bool *modified_diff = new bool[diff_csr_size];

	// printf("[%d]: Total size for modified vector: %d + %d = %d\n",my_rank,csr_size,diff_csr_size,csr_size+diff_csr_size);

	for (int i=0;i<csr_size;i++) modified[i]=false;
	for (int i=0;i<diff_csr_size;i++) modified_diff[i]=false;

	for (int i=0; i<updSize; i++)
	{
		int remote_src = updSrcs[i];
		int remote_dest = updDests[i];
		int local_src = remote_src - my_rank*(num_nodes/np);

		bool found = false;
		for (int j=local_index[local_src]-local_index[0]; j<local_index[local_src+1]-local_index[0]; j++)
		{
			if (local_edgeList[j] == remote_dest)
			{
				found = true;
				modified[j] = true;
				// printf("[%d]: Marking edge %d as modified\n",my_rank,j);
				break;
			}
		}
		if (!found)
		{
			for (int j=local_diff_index[local_src]; j<local_diff_index[local_src+1]; j++)
			{
				if (local_diff_edgeList[j] == remote_dest)
				{
					found = true;
					modified_diff[j] = true;
					// printf("[%d]: Marking diff edge %d as modified\n",my_rank,j);
					break;
				}
			}
		}
	}

	std::vector <std::map<int, std::vector<std::pair<int, bool>>>> send_data(np);
	std::vector <std::map<int, std::vector<std::pair<int, bool>>>> receive_data(np);

	for (int i=0; i<updSize; i++)
	{
		int u = updSrcs[i];
		int v = updDests[i];
		// e1: u-v edge

		int u_index = u - my_rank*(num_nodes/np);

		// printf("[%d]: u=%d u_index=%d\n", my_rank, u, u_index);


		for (int j=local_index[u_index]-local_index[0]; j<local_index[u_index+1]-local_index[0]; j++)
		{
			int w = local_edgeList[j];
			if (w != INF && w != v)
			{
				// e2: u-w edge
				int w_proc = w / (num_nodes/np);
				if (w_proc >= np) w_proc = np-1;
				// printf("[%d]: u=%d u_index=%d v=%d w=%d w_proc=%d\n",my_rank,u,u_index,v,w,w_proc);
				send_data[w_proc][w].push_back({v, modified[j]});
			}
		}

		for (int j=local_diff_index[u_index]; j<local_diff_index[u_index+1]; j++)
		{
			int w = local_diff_edgeList[j];
			if (w != INF && w != v)
			{
				// e2: u-w edge
				int w_proc = w / (num_nodes/np);
				if (w_proc >= np) w_proc = np-1;
				send_data[w_proc][w].push_back({v, modified_diff[j]});
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	all_to_all(world, send_data, receive_data);

	int count1 = 0, count2 = 0, count3 = 0;

	for (int i=0; i<np; i++)
	{
		std::map <int,std::vector <std::pair<int,bool>>> pairs = receive_data[i];
		for (auto &w_and_vs : pairs)
		{
			int w = w_and_vs.first;
			int w_index = w - my_rank*(num_nodes/np);

			// TODO: sort v's before
			for (auto &v : w_and_vs.second)
			{
				for (int j=local_index[w_index]-local_index[0]; j<local_index[w_index+1]-local_index[0]; j++)
				{
					if (local_edgeList[j] == v.first) 
					{
						// printf("[%d]: Found triangle with w=%d v=%d(%d) and modified[%d]=%s\n",my_rank,w,v.first,v.second,j,btoa(modified[j]));
						if (v.second && modified[j]) count3++;
						else if (v.second || modified[j]) count2++;
						else count1++;
						break;
					}
				}
				for (int j=local_diff_index[w_index]; j<local_diff_index[w_index+1]; j++)
				{
					if (local_diff_edgeList[j] == v.first) 
					{
						// printf("[%d]: Found triangle with w=%d v=%d(%d) and modified_diff[%d]=%s\n",my_rank,w,v.first,v.second,j,btoa(modified_diff[j]));
						if (v.second && modified_diff[j]) count3++;
						else if (v.second || modified_diff[j]) count2++;
						else count1++;
						break;
					}
				}
			}
		}
	}

	delete [] modified;
	delete [] modified_diff;

	// printf("Rank[%d]: count1=%d count2=%d count3=%d\n", my_rank, count1, count2, count3);

	int global_count1=0, global_count2=0, global_count3=0;

	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Reduce(&count1, &global_count1, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&count2, &global_count2, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&count3, &global_count3, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	int change = 0;

	if (my_rank==0)
	{
		// printf("Global data: c1=%d c2=%d c3=%d\n", global_count1, global_count2, global_count3);
		change = global_count1/2 + global_count2/4 + global_count3/6;
		printf("Change in triangle count by %d\n", change);
	}

	return change;
}


int dynamicUpdate(graph g, int np, int my_rank,int num_nodes,int part_size,char *percentInput,char *updatefilePath,int bSize,mpi::communicator &world, int TC)
{
    int updateSize, batchSize, elements;
	float percent;
	std::vector<update> updateEdges;

	batchSize=bSize;

	// extracting and broadcasting UPDATE SIZE
	if(my_rank == 0)
	{
		updateEdges = g.parseUpdates(updatefilePath);

		percent = atof(percentInput); 
		printf("PERCENT INPUT = %f\n", percent);

		updateSize = ceil(percent*updateEdges.size());

		printf("UPDATE SIZE = %d\n",updateSize);

		printf("BATCH SIZE = %d\n", batchSize);

		MPI_Bcast (&updateSize,1,MPI_INT,my_rank,MPI_COMM_WORLD);
	}
	else
	{
		MPI_Bcast (&updateSize,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	elements = batchSize;

	// perNodeCSRSpace and perNodeDiffCSRSpace
	local_PNCS = new int[part_size];
	local_PNDCS = new int[part_size];
	local_diff_index = new int [part_size+1];

	local_diff_index[0] = 0;
	for(int i=0; i<part_size; i++)
	{
		local_diff_index[i+1] = 0;
		local_PNCS[i]=0;
		local_PNDCS[i]=0;
	}

    int updSize;
	int *updSrcs=NULL, *updDests=NULL;

	int *local_needed_info = new int [part_size];
	int *local_PNDCS_Total = new int [part_size];

	int ** updateMatrixSrc = new int* [np];
	int ** updateMatrixDest = new int* [np];

	int* perProcessSize, *perProcessIndex;
	if (my_rank == 0)
	{
		perProcessSize = new int[np];
		perProcessIndex = new int[np];
	}

	MPI_Barrier(MPI_COMM_WORLD);

	// printArray(my_rank,local_edgeList,local_index[part_size]-local_index[0],0);
	// printArray(my_rank,local_diff_edgeList,local_diff_index[part_size]-local_diff_index[0],1);

	MPI_Barrier(MPI_COMM_WORLD);

	int changeTC=0;
	totalBatches=0;

	struct timeval start, end;
	long seconds, micros;

	for(int k=0; k<updateSize; k+=batchSize)
	{
		totalBatches++;

		if(my_rank==0) gettimeofday(&start, NULL);

		if(batchSize > updateSize - k) elements = updateSize - k;

		updSize = distributeBatchUpdates(np,my_rank,num_nodes,part_size,k,elements,'d',
							perProcessSize,perProcessIndex,updateEdges,
							updateMatrixSrc,updateMatrixDest,&updSrcs,&updDests);

		
		changeTC = COMPUTE_TC_CHANGE(np,my_rank,part_size,num_nodes,updSize,updSrcs,updDests,world);
		
		if (my_rank==0)
		{
			TC -= changeTC;
			printf("After deletion of %d triangles, TC = %d\n", changeTC, TC);
		}

		// delete edges
		deleteCSR(np,my_rank,part_size,num_nodes,updSize,updSrcs,updDests);

		free(updSrcs);
		free(updDests);
		updSrcs=NULL;
		updDests=NULL;

		MPI_Barrier(MPI_COMM_WORLD);

		/////////////////////////////////// DELETION DONE //////////////////////////////////

		updSize = distributeBatchUpdates(np,my_rank,num_nodes,part_size,k,elements,'a',
							perProcessSize,perProcessIndex,updateEdges,
							updateMatrixSrc,updateMatrixDest,&updSrcs,&updDests);

		// add edges
		addCSR(np,my_rank,part_size,num_nodes,updSize,updSrcs,updDests,local_needed_info,local_PNDCS_Total);

		changeTC = COMPUTE_TC_CHANGE(np,my_rank,part_size,num_nodes,updSize,updSrcs,updDests,world);
		
		free(updSrcs);
		free(updDests);
		updSrcs=NULL;
		updDests=NULL;

		MPI_Barrier(MPI_COMM_WORLD);

		/////////////////////////////////// ADDITION DONE //////////////////////////////////

		if (my_rank==0)
		{
			TC += changeTC;
			printf("After addition of %d triangles, TC = %d\n",changeTC,TC);

			gettimeofday(&end, NULL);

			seconds = end.tv_sec - start.tv_sec;
			micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

			printf("Batch update took %ld seconds\n", seconds);
			printf("Batch update took %ld micros\n", micros);
		}
    }

	return TC;
}

int main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
	mpi::communicator world;
	int my_rank = world.rank();

	MPI_Barrier(MPI_COMM_WORLD);

	int np = world.size();
	if (my_rank == 0) printf("World size = %d\n",np);

	int max_degree,num_nodes, part_size;
	int *index,*weight,*edgeList;
	// int *local_index,*local_weight,*local_edgeList;

	graph g(argv[1]);

	// TODO: Everything just fails when np == 1 ??

	// setting up local dta structures - distributing the ggraph and it's details
	if(my_rank == 0)
	{
		g.parseGraph();

		weight = g.getEdgeLen();
		edgeList = g.edgeList;
		index = g.indexofNodes;

		num_nodes = g.num_nodes();
		part_size = g.num_nodes()/np;
		MPI_Bcast (&num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
		MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
		local_index = new int[part_size+1];
		for(int i=0;i<part_size+1;i++)
			local_index[i] = index[i];

		// Sending out extra nodes left to change the part_size
		int leftout_nodes = num_nodes % np;
		MPI_Send(&leftout_nodes, 1, MPI_INT, world.size()-1, 5, MPI_COMM_WORLD);

		printf("[0]: num_nodes=%d, num_edges=%d, part_size=%d\n", g.num_nodes(), g.num_edges(), part_size);

		int num_ele = local_index[part_size]-local_index[0];
		local_weight = new int[num_ele];
		for(int i=0;i<num_ele;i++)
			local_weight[i] = weight[i];
		local_edgeList = new int[num_ele];
		for(int i=0;i<num_ele;i++)
			local_edgeList[i] = edgeList[i];

		for(int i=1;i<np-1;i++)
		{
			int pos = i*part_size;
			MPI_Send (index+pos,part_size+1,MPI_INT,i,0,MPI_COMM_WORLD);
			int start = index[pos];
			int end = index[pos+part_size];
			int count = end - start;
			MPI_Send (weight+start,count,MPI_INT,i,1,MPI_COMM_WORLD);
			MPI_Send (edgeList+start,count,MPI_INT,i,2,MPI_COMM_WORLD);
		}

		int pos = (np-1)*part_size;
		MPI_Send (index+pos,part_size+1+leftout_nodes,MPI_INT,np-1,0,MPI_COMM_WORLD);
		int start = index[pos];
		int end = index[pos+part_size+leftout_nodes];
		int count = end - start;
		MPI_Send (weight+start,count,MPI_INT,np-1,1,MPI_COMM_WORLD);
		MPI_Send (edgeList+start,count,MPI_INT,np-1,2,MPI_COMM_WORLD);

		delete [] weight;
		delete [] edgeList;
		delete [] index;

		printf("[0]: Done with phase 1. Will have to read updates now!!\n");
	}
	else
	{
		MPI_Bcast (&num_nodes,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);

		if (my_rank == world.size()-1)
		{
			// printf("[%d]: Taking the extra nodes for the team...\n", my_rank);
			int leftout_nodes;
			MPI_Recv(&leftout_nodes, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			part_size += leftout_nodes;
		}

		// printf("[%d]: part_size=%d\n", my_rank, part_size);

		local_index = new int[part_size+1];
		MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		int num_ele = local_index[part_size]-local_index[0];
		local_weight = new int[num_ele];
		MPI_Recv (local_weight,num_ele,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		local_edgeList = new int[num_ele];
		MPI_Recv (local_edgeList,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		

 		printf("[%d]: num_edges=%d\n", my_rank, num_ele);
	}

	MPI_Barrier(MPI_COMM_WORLD);

    int global_triangle_count = COMPUTE_TC(np,my_rank,part_size,num_nodes,world);
	
    MPI_Barrier(MPI_COMM_WORLD);

	struct timeval start, end;
	long seconds,micros;
	
	if (my_rank==0) gettimeofday(&start, NULL);

	int final_triangle_count = dynamicUpdate(g,np,my_rank,num_nodes,part_size,argv[3],argv[4],atoi(argv[5]),world,global_triangle_count);

	if(my_rank==0)
	{
		printf("\nFinal TC = %d\n", final_triangle_count);
		gettimeofday(&end, NULL);

		seconds = end.tv_sec - start.tv_sec;
		micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

		printf("All updates took %ld seconds\n", seconds);
		printf("All updates took %ld micros\n", micros);

		printf("Avg batch update took %ld seconds\n", seconds/totalBatches);
		printf("Avg batch update took %ld micros\n", micros/totalBatches);
	}

	// printf("Done with rank=%d\n", my_rank);

    MPI_Finalize();
    return 0;
}
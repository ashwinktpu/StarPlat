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
#include <boost/mpi.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
#include"../graph.hpp"

// using namespace std;
namespace mpi = boost::mpi;


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
			
			// Nobita TODO: remove
			if (u.source>=num_nodes || u.destination>=num_nodes) 
			{
				continue;
			}

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

			if (u.source>=num_nodes || u.destination>=num_nodes) 
			{
				// printf("Ignoring the update between %d and %d of type %c\n", u.source, u.destination, u.type);
				continue;
			}
			
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
			// printf("Rank [%d] delete edge between %d and %d\n", my_rank, (*updSrcs)[i], (*updDests)[i]);
			(*updSrcs)[i] = updateMatrixSrc[0][i];
			(*updDests)[i] = updateMatrixDest[0][i];
		}

		// printf("updSrc and updDests are set\n");

		for (int i=0; i<np; i++)
		{
			assert(perProcessIndex[i] == perProcessSize[i]);
			// printf("[%d]: Is index=%d  ==  size=%d ?\n", i, perProcessIndex[i], perProcessSize[i]);
		}

		assert(c1 == c2);
		assert(c2 == c3);

		// printf("Assertion checks are clear\n");

		// printf("Is c1=%d == c2=%d == c3=%d ?\n", c1, c2, c3);

		for (int i=0; i<np; i++)
		{
			delete [] updateMatrixSrc[i];
			delete [] updateMatrixDest[i];
		}

		// printf("Deleted matrices\n");
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


int dynamicUpdate(graph g, int np, int my_rank,int num_nodes,int part_size,int *local_index,int *local_weight,int *local_edgeList,char *percentInput,char *updatefilePath,int bSize)
{
	int updateSize, batchSize, elements;
	float percent;
	std::vector<update> updateEdges;

	batchSize = bSize;

	// extracting and broadcasting UPDATE SIZE
	if(my_rank == 0)
	{
		updateEdges = g.parseUpdates(updatefilePath);

		percent = atof(percentInput); 
		printf("PERCENT INPUT = %f\n", percent);

		updateSize = ceil(percent*updateEdges.size());
		// updateSize = 400;
		printf("UPDATE SIZE = %d\n",updateSize);

		printf("BATCH SIZE = %d\n", batchSize);

		MPI_Bcast (&updateSize,1,MPI_INT,my_rank,MPI_COMM_WORLD);
	}
	else
	{
		MPI_Bcast (&updateSize,1,MPI_INT,0,MPI_COMM_WORLD);
	}
	
	elements = batchSize;

	int *local_diff_index, *local_diff_edgeList=NULL, *local_diff_weight=NULL;
	int *local_PNCS, *local_PNDCS;

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

	// printf("Rank[%d]: Successfully created auxiliary csr and per node space data structures\n", my_rank);

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

	struct timeval start, end;
	long seconds, micros;

	int itrCount = 0;

	for(int k=0; k<updateSize; k+=batchSize)
	{
		itrCount++;

		if(k+batchSize > updateSize) elements = updateSize-k;

		if(my_rank==0) gettimeofday(&start, NULL);
		
		/////////////////////////////// DELETION BATCH //////////////////////////////

		updSize = distributeBatchUpdates(np,my_rank,num_nodes,part_size,k,elements,'d',
							perProcessSize,perProcessIndex,updateEdges,
							updateMatrixSrc,updateMatrixDest,&updSrcs,&updDests);

		MPI_Barrier(MPI_COMM_WORLD);

		// if (my_rank==0) 
		// {
		// 	printf("Extracted deletion batch\n");
		// }

		for (int i=0; i<updSize; i++)
		{
			int remote_src = updSrcs[i];
			int remote_dest = updDests[i];

			// 0 indexing here 
			// E.g. consider a given source id 1000 (=remote_src) on 64 processes
			// It will be distributed on process id 1000/64 = 15 (=my_rank)
			// On process id 15, it will indexed at 1000 - 15*64 = 40 (=local_src)
			int local_src = remote_src - my_rank*(num_nodes/np);

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

			// if (!found)   // TODO: Might contain duplicate edge? Need to remove that too
			// {
				for(int j=local_diff_index[local_src]; j<local_diff_index[local_src+1]; j++)
				{
					int nbr = local_diff_edgeList[j];
					if(nbr == remote_dest)
					{
						local_PNDCS[local_src]++;
						int index = local_diff_index[local_src+1] - local_PNDCS[local_src];
						local_diff_edgeList[j] = local_diff_edgeList[index];
						local_diff_weight[j] = local_diff_weight[index];
						local_diff_edgeList[index] = INF;
						found = true;
					}
				}
			// }

			// if (!found)
			// {
        	// 	printf("ERROR [%d]: Edge between %d and %d not found\n", my_rank, remote_src, remote_dest); // TODO
			// }
		}


		free(updSrcs);
		free(updDests);
		updSrcs=NULL;
		updDests=NULL;

		MPI_Barrier(MPI_COMM_WORLD);

		////////////////////////////////// DELETION DONE ////////////////////////////


		///////////////////////////////// ADDITION BATCH //////////////////////////
		
		updSize = distributeBatchUpdates(np,my_rank,num_nodes,part_size,k,elements,'a',
							perProcessSize,perProcessIndex,updateEdges,
							updateMatrixSrc,updateMatrixDest,&updSrcs,&updDests);
		
		MPI_Barrier(MPI_COMM_WORLD);

		// if (my_rank==0) 
		// {
		// 	printf("Extracted addition batch\n");
		// }

		// add edges 
		for (int i=0; i<part_size; i++)
		{
			local_needed_info[i] = 0;
		}
		for (int i=0; i<updSize; i++)
		{
			local_needed_info[updSrcs[i] - my_rank*(num_nodes/np)]++;
		} 

		int local_diff_csr_new_add = 0;
		
		for (int i=0; i<part_size; i++)
		{
			// current diff size for i
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

		// printf("Rank [%d]: Diff csr size should be = %d\n", my_rank, local_diff_index[part_size] - local_diff_index[0] + local_diff_csr_new_add);

		if (local_diff_csr_new_add > 0)
		{
			// increase the size of diff_csr
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


		free(updSrcs);
		free(updDests);
		updSrcs=NULL;
		updDests=NULL;

		MPI_Barrier(MPI_COMM_WORLD);

		////////////////////////////// ADDITION DONE /////////////////////////

		if (my_rank == 0)
		{
			gettimeofday(&end, NULL);

			seconds = end.tv_sec - start.tv_sec;
			micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

			printf("Batch update took %ld seconds\n", seconds);
			printf("Batch update took %ld micros\n", micros);
		}
	}

	delete [] local_needed_info;
	delete [] local_PNDCS_Total;

	// Yahi problem hai !!!
	if (my_rank == 0)
	{
		delete [] updSrcs;
		delete [] updDests;

		delete [] updateMatrixSrc;
		delete [] updateMatrixDest;

		delete [] perProcessSize;
		delete [] perProcessIndex;
	}

	return itrCount;
}


int main(int argc, char** argv)
{
	mpi::environment env(argc, argv);
	mpi::communicator world;
	int my_rank = world.rank();

	struct timeval start, end;
	long seconds, micros;

	MPI_Barrier(MPI_COMM_WORLD);

  	if (my_rank == 0)
	{
		gettimeofday(&start, NULL);
	}

	int np = world.size();
	if (my_rank == 0) printf("World size = %d\n",np);

	int max_degree,num_nodes, part_size;
	int *index,*weight,*edgeList;
	int *local_index,*local_weight,*local_edgeList;

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

  	if (my_rank == 0)
	{
		gettimeofday(&end, NULL);

		seconds = end.tv_sec - start.tv_sec;
		micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

		printf("Graph building took %ld seconds\n", seconds);
		printf("Graph building took %ld micros\n", micros);

		gettimeofday(&start, NULL);
	}


	// NOTE: part_size for last process will mostly be larger than other processes who have equal part_size (=num_nodes/np)

	int totalBatches = dynamicUpdate(g,np,my_rank,num_nodes,part_size,local_index,local_weight,local_edgeList,argv[3],argv[4],atoi(argv[5]));

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) 
	{
		gettimeofday(&end, NULL);

		printf("\nDone with the program\n");

		seconds = end.tv_sec - start.tv_sec;
		micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

		printf("All updates took %ld seconds\n", seconds);
		printf("All updates took %ld micros\n", micros);

		printf("Avg batch update took %ld seconds\n", seconds/totalBatches);
		printf("Avg batch update took %ld micros\n", micros/totalBatches);
	}

	MPI_Finalize();
	return 0;
}
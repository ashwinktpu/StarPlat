#include "mst_prims_new.h"

void reduce_global_min(int *in, int *inout, int *len, MPI_Datatype *dptr)
{
    if(in[2] < inout[2])
    {
        inout[0] = in[0];
        inout[1] = in[1];
        inout[2] = in[2];
    }
}

void Compute_MST_Prims(Graph &g, int start_node, int &num_mst_nodes, long long int &mst_weight, boost::mpi::communicator world)
{
    MPI_Op reduce_edge_op;
    MPI_Op_create((MPI_User_function *)reduce_global_min, true, &reduce_edge_op);

    NodeProperty<bool> inMST;
    inMST.attachToGraph(&g, (bool)false);

    if(world.rank() == g.get_node_owner(start_node))
    {
        inMST.setValue(start_node, true);
        num_mst_nodes++;
    }
    MPI_Bcast(&num_mst_nodes, 1, MPI_INT, g.get_node_owner(start_node), MPI_COMM_WORLD);

    if(world.rank() == 0)
    {
        std::cout << "Starting MST computation" << std::endl;
    }

    int active = 1;
    while(active)
    {
        int min_edge[3] = {-1, -1, INT_MAX};
        for(int i=g.start_node(); i<=g.end_node(); i++)
        {
            if(inMST.getValue(i)) continue;

            for(int j : g.getNeighbors(i))
            {
                if(!inMST.getValue(j)) continue;

                Edge e = g.get_edge(i, j);
                if(g.weights.getValue(e) < min_edge[2])
                {
                    min_edge[0] = i;
                    min_edge[1] = j;
                    min_edge[2] = g.weights.getValue(e);
                }
            }
        }

        int min_edge_global[3] = {-1, -1, INT_MAX};
        MPI_Allreduce(min_edge, min_edge_global, 3, MPI_INT, reduce_edge_op, MPI_COMM_WORLD);

        if(world.rank() == g.get_node_owner(min_edge_global[0]))
        {
            inMST.setValue(min_edge_global[0], true);
            num_mst_nodes++;
            mst_weight += min_edge_global[2];
            std::cout << "Iteration " << num_mst_nodes << " " << "Rank " << world.rank() << std::endl;
            std::cout << "Min Edge is: " << min_edge_global[0] << " " << min_edge_global[1] << " " << min_edge_global[2] << std::endl;

            if(num_mst_nodes == g.num_nodes()) active = 0;
        }
        MPI_Bcast(&num_mst_nodes, 1, MPI_INT, g.get_node_owner(min_edge_global[0]), MPI_COMM_WORLD);
        MPI_Bcast(&mst_weight, 1, MPI_LONG_LONG_INT, g.get_node_owner(min_edge_global[0]), MPI_COMM_WORLD);
        MPI_Bcast(&active, 1, MPI_INT, g.get_node_owner(min_edge_global[0]), MPI_COMM_WORLD);
        world.barrier();
    }
}
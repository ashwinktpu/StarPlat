#include"updates.h"
#include"limits.h"
#include<fstream>
#include<sstream>
#include<string>
#include<cassert>

void readFromUpdatesFile(char* filePath, int32_t &num_updates,std::vector<char> &types,std::vector<int32_t> &srcs,std::vector<int32_t> &dests,std::vector<int32_t> &weights, bool push_reverse, int max_updates, int total_nodes)
{

    std::ifstream infile;
    infile.open(filePath);

    if(!infile.is_open())
    {
        std::cerr<<"could not open updates file "<<filePath<<std::endl;
        exit(-1);
    }
    std::string line;
    num_updates = 0;

    while (max_updates > num_updates && std::getline(infile, line))
    {

        if (line.length() == 0 || (line[0] != 'a' && line[0] != 'd'))
        {
            continue;
        }

        std::stringstream ss(line);

        

        char type;
        int32_t source;
        int32_t destination;
        int32_t weightVal;

        ss>>type;
        ss >> source;
        ss >> destination;
        
        if(!(ss >> weightVal))
          weightVal =1;

        if(source >= total_nodes || destination >= total_nodes)
            continue;

        num_updates++;    
        types.push_back(type);
        srcs.push_back(source);
        dests.push_back(destination);
        weights.push_back(weightVal);
        
        if(push_reverse)
        {
            num_updates++;
            types.push_back(type);
            srcs.push_back(destination);
            dests.push_back(source);
            weights.push_back(weightVal);
        }
    }
    infile.close();
}

Updates::Updates(char* file,boost::mpi::communicator world,Graph * g, float percent_updates)
{
    this->g = g;
    current_batch = 0;
    localBatchSize = INT_MAX;
    num_batches = 1;
    num_local_updates = 0;

    std::vector<char> types;
    std::vector<int32_t> srcs;
    std::vector<int32_t> dests;
    std::vector<int32_t> weights;

    total_updates = 0;
    int max_updates = INT_MAX;
    if(percent_updates != 100)
    {
        max_updates = (int)(percent_updates*g->num_edges());
    }
    if(world.rank()==0)
    {
        readFromUpdatesFile(file,total_updates,types,srcs,dests,weights, g->is_undirected(),max_updates,  g->num_nodes());
    }
    world.barrier();
    boost::mpi::broadcast(world, total_updates, 0);

    std::vector<int32_t> sizes, offsets;
    offsets.push_back(0);
    for (int i = 0; i < world.size(); i++)
    {
      sizes.push_back(total_updates / world.size() + (i < (total_updates % world.size()) ? 1 : 0));
      if(i!= world.size()-1)
        offsets.push_back(offsets[i] + sizes[i]);
    }
    int32_t scatter_size = total_updates / world.size() + (world.rank() < (total_updates % world.size()) ? 1 : 0);

    char * localtypes = new char[scatter_size];
    int32_t * localsrcs = new int32_t[scatter_size];
    int32_t * localdests = new int32_t[scatter_size];
    int32_t * localweights = new int32_t[scatter_size];
    MPI_Scatterv(types.data(), sizes.data(), offsets.data(), MPI_CHAR, localtypes, scatter_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatterv(srcs.data(), sizes.data(), offsets.data(), MPI_INT32_T, localsrcs, scatter_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatterv(dests.data(), sizes.data(), offsets.data(), MPI_INT32_T, localdests, scatter_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatterv(weights.data(), sizes.data(), offsets.data(), MPI_INT32_T, localweights, scatter_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
    world.barrier();
    types.clear();srcs.clear();dests.clear();weights.clear();

    std::vector<std::vector<Update>> updates(world.size());

    for(int i=0;i<scatter_size;i++)
    {   
        Update u;
        u.type = localtypes[i]=='a' ?Addition : Delition;
        u.source = localsrcs[i];u.destination = localdests[i];u.weight = localweights[i];
        updates[g->get_node_owner(u.source)].push_back(u);
    }
    delete []localtypes;delete []localsrcs;delete []localdests;delete []localweights;

    boost::mpi::all_to_all(world,updates,updates);

    for(int i=0;i<world.size();i++)
    {
        for(Update u: updates[i])
            this->updates.push_back(u);
    }
    num_local_updates = this->updates.size();
    
    is_batch = false;
}


void Updates::splitIntoSmallerBatches(int size)
{
    assert(size>0);
    current_batch = 0;
    num_batches = (total_updates+(size-1))/size;
    
    localBatchSize = (num_local_updates+(num_batches-1))/num_batches;

}


bool Updates::nextBatch()
{
    if(current_batch>= num_batches)
        return false;

    std::vector<Update> currentBatch_vec;
    std::vector<Update> currentAddBatch_vec;
    std::vector<Update> currentDelBatch_vec;
    
    for(int i=current_batch*localBatchSize;i<(current_batch+1)*localBatchSize && i<num_local_updates ;i++)
    {
        currentBatch_vec.push_back(updates[i]);
        if(updates[i].type == Addition)
            currentAddBatch_vec.push_back(updates[i]);
        else   
            currentDelBatch_vec.push_back(updates[i]);
    }

    delete currentBatch;
    delete currentAddBatch;
    delete currentDelBatch;

    currentBatch=new Updates(currentBatch_vec);
    currentAddBatch=new Updates(currentAddBatch_vec);
    currentDelBatch=new Updates(currentDelBatch_vec);       
    current_batch++;
    return true;
}

void Updates::updateCsrAdd(Graph * g)
{
    std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>> addUpdates;
    std::vector<std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>> rev_addUpdates_to_process(world.size());
    std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>> rev_addUpdates;
    for(Update u: currentAddBatch->getUpdates())
    {
        addUpdates.push_back(std::make_pair(u.source, std::make_pair(u.destination,u.weight)));
        rev_addUpdates_to_process[g->get_node_owner(u.destination)].push_back(std::make_pair(u.source, std::make_pair(u.destination,u.weight)));
    }

    boost::mpi::all_to_all(world,rev_addUpdates_to_process,rev_addUpdates_to_process);

    for(int i=0;i<world.size();i++)
    {
        for(std::pair<int32_t,std::pair<int32_t,int32_t>> p :rev_addUpdates_to_process[i])
        {
            rev_addUpdates.push_back(p);
        }
    }
    
    g->addEdges_Csr(addUpdates);
    g->addEdges_RevCsr(rev_addUpdates);

}

void Updates::updateCsrDel(Graph * g)
{   
    std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>> delUpdates;
    std::vector<std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>>> rev_delUpdates_to_process(world.size());
    std::vector<std::pair<int32_t,std::pair<int32_t,int32_t>>> rev_delUpdates;
    for(Update u: currentDelBatch->getUpdates())
    {
    
        delUpdates.push_back(std::make_pair(u.source, std::make_pair(u.destination,u.weight)));
        rev_delUpdates_to_process[g->get_node_owner(u.destination)].push_back(std::make_pair(u.source, std::make_pair(u.destination,u.weight)));
    }
    
    boost::mpi::all_to_all(world,rev_delUpdates_to_process,rev_delUpdates_to_process);

    for(int i=0;i<world.size();i++)
    {
        for(std::pair<int32_t,std::pair<int32_t,int32_t>> p :rev_delUpdates_to_process[i])
        {
            rev_delUpdates.push_back(p);
        }
    }
    
    g->delEdges_Csr(delUpdates);
    g->delEdges_RevCsr(rev_delUpdates);

}
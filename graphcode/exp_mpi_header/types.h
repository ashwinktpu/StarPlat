#ifndef MPI_GRAPH_TYPES_HEADER
#define MPI_GRAPH_TYPES_HEADER

#include <unordered_map>
class Edge{
    private:
        bool belongs_to_csr;
        int edge_id;

    public :
        Edge(bool belongs_to_csr, int edge_id)
        {
            this->belongs_to_csr = belongs_to_csr;
            this->edge_id = edge_id;
        }
        bool is_in_csr()
        {
            return belongs_to_csr;
        }
        int get_id()
        {
            return edge_id;
        }   
};

#endif
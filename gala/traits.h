
// stray header, tdlib related need to get rid of this

#ifndef TD_DEFS_NETWORK_FLOW
#define TD_DEFS_NETWORK_FLOW
namespace treedec{

struct Vertex_NF{
    bool visited;
    int predecessor;
};

struct Edge_NF{
    bool path; //true if a path uses the edge
};
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Vertex_NF, Edge_NF> digraph_t;

}
#endif

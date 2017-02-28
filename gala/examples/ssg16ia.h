#include <gala/trace.h>
#include <iostream>
#include <iomanip>
#include <map>
// #include <boost/graph/adjacency_list.hpp>
#include <stx/btree_set.h>
#include <gala/td.h>
#include "alloc.hpp"
#include <gala/digraph.h>

template<class T, class...>
using myset = typename std::set<T, std::less<T>, POOL_ALLOC<T>  >;


template<class G>
struct ssg16ia_config : public gala::graph_cfg_default<G> {
};

typedef gala::graph< myset, std::vector, uint16_t, ssg16ia_config > ssg_16ia;


namespace treedec{
template<>
struct graph_traits<ssg_16ia>{ //
	typedef typename treedec_chooser<ssg_16ia>::type treedec_type;
	typedef typename outedge_set<ssg_16ia>::type outedge_set_type;
	typedef idgwel<ssg_16ia> directed_overlay;
	typedef typename boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> immutable_type;
};

using ssg16ia_traits=graph_traits<ssg_16ia>;

std::pair<typename boost::graph_traits<typename ssg16ia_traits::directed_overlay>::vertex_descriptor,
          typename boost::graph_traits<typename ssg16ia_traits::directed_overlay>::vertex_descriptor>
    make_digraph_with_source_and_sink(ssg_16ia const &G, std::vector<bool> const &disabled,
                 unsigned num_dis,
                 typename graph_traits<ssg_16ia>::directed_overlay& dg,
                 std::vector<typename boost::graph_traits<ssg_16ia>::vertex_descriptor> &idxMap,
                 typename std::set<typename boost::graph_traits<ssg_16ia>::vertex_descriptor> const &SRC,
                 typename std::set<typename boost::graph_traits<ssg_16ia>::vertex_descriptor> const &SNK)
{
	dg = std::move(idgwel<ssg_16ia>(G, disabled, num_dis, idxMap, SRC, SNK));
	return std::make_pair(dg.source(), dg.sink());
}


} // treedec




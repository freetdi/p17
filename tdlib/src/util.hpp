#ifndef TD_UTIL_H
#define TD_UTIL_H

#include "marker_util.hpp"

namespace treedec{

// count number of edges missing in 1-neighborhood of v
template <typename G_t>
inline size_t count_missing_edges(
        const typename boost::graph_traits<G_t>::vertex_descriptor v, G_t const &G)
{
    size_t missing_edges = 0;

    typename boost::graph_traits<G_t>::adjacency_iterator nIt1, nIt2, nEnd;
    for(boost::tie(nIt1, nEnd) = boost::adjacent_vertices(v, G); nIt1 != nEnd; nIt1++){
        nIt2 = nIt1;
        nIt2++;
        for(; nIt2 != nEnd; nIt2++){
            if(!boost::edge(*nIt1, *nIt2, G).second){
                ++missing_edges;
            }
        }
    }
    return missing_edges;
}

template <typename G_t, class MARKER>
inline size_t count_missing_edges(
        const typename boost::graph_traits<G_t>::vertex_descriptor v,
		  MARKER& marker, G_t const &g)
{
	size_t missing_edges = 0;

	auto p=boost::adjacent_vertices(v, g);
	for(; p.first!=p.second; ++p.first){
		trace2("visit", v, *p.first);
		marker.clear();
		mark_neighbours(marker, *p.first, g);

		auto q=adjacent_vertices(v, g);
		for(; q.first!=q.second; ++q.first){
			if(*q.first>=*p.first){
				// skip. TODO: more efficient skip
			}else if(marker.is_marked(*q.first)){
				// done, reachable from *p.first
			}else{
				trace2("found", *p.first, *q.first);
				++missing_edges;
			}
		}
	}
	trace2("counted_missing_edges w/marker", v, missing_edges);
	return missing_edges;
}

}

#endif

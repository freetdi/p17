/* Copyright (C) 2015-2016 Felix Salfelder
 * Author: Felix Salfelder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 *
 */
#include <iostream>
#include <iomanip>
#include <map>
#include <gala/graph.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/adjacency_matrix.hpp>

typedef gala::graph<std::set, std::vector, uint32_t> ssg_32i;

// HACK HACK HACK
#include <tdlib/graph_traits.hpp>
#include <gala/td.h>
#ifndef TD_DEFS_NETWORK_FLOW
#define TD_DEFS_NETWORK_FLOW
namespace treedec{
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Vertex_NF, Edge_NF> digraph_t;

}
#endif

#include <gala/digraph.h>
#include <gala/immutable.h>

namespace treedec{
template<>
struct graph_traits<ssg_32i>{ //
	typedef typename treedec_chooser<ssg_32i>::type treedec_type;
	typedef typename outedge_set<ssg_32i>::type outedge_set_type;
	typedef idgwel<ssg_32i> directed_overlay;
	typedef idgwel<ssg_32i> immutable_directed_type;
#if 1
	typedef immvecgraph<ssg_32i> immutable_type;
	typedef immvecgraph<ssg_32i> immutable_undirected_type;
#else
	typedef typename boost::adjacency_matrix<boost::undirectedS> immutable_type;
#endif
};

using ssg32i_traits=graph_traits<ssg_32i>;

std::pair<typename boost::graph_traits<typename ssg32i_traits::directed_overlay>::vertex_descriptor,
          typename boost::graph_traits<typename ssg32i_traits::directed_overlay>::vertex_descriptor>
    make_digraph_with_source_and_sink(ssg_32i const &G, std::vector<bool> const &disabled,
                 unsigned num_dis,
                 typename graph_traits<ssg_32i>::directed_overlay& dg,
                 std::vector<typename boost::graph_traits<ssg_32i>::vertex_descriptor> &idxMap,
                 typename std::set<typename boost::graph_traits<ssg_32i>::vertex_descriptor> const &SRC,
                 typename std::set<typename boost::graph_traits<ssg_32i>::vertex_descriptor> const &SNK)
{ untested();
	dg = std::move(idgwel<ssg_32i>(G, disabled, num_dis, idxMap, SRC, SNK));
	return std::make_pair(dg.source(), dg.sink());
}

} // treedec

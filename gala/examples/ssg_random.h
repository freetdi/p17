/* Copyright (C) 2016 Felix Salfelder
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
 *  graph with randomized degree buckets
 *
 */

#include <tdlib/graph_traits.hpp>
#include <tdlib/degree_config.hpp>
namespace gala_rand{

template<class G>
struct m_deg_config : public misc::detail::random_deg_config<G> {
	typedef typename boost::graph_traits<G>::vertex_descriptor vd_type;
	typedef typename G::EL bag_type;
	static void alloc_init(size_t)
	{
	}
	static unsigned num_threads()
	{ untested();
		unsigned n=1;
		if(const char* nt=getenv("TD_NUM_DEGWORKER")){ untested();
			n = atoi(nt);
			trace1("num_threads", n);
		}else{ untested();
		}
		return n;
	}
};

template<class T, class...>
using DEX = misc::DEGS<T, m_deg_config<T> >;

template<class G>
struct svbs_config : public gala::graph_cfg_default<G> {
	typedef DEX<G> degs_type;
};
}

template<class X>
using ssg16rset=std::set<X>;

typedef gala::graph<ssg16rset, std::vector, uint16_t, gala_rand::svbs_config> ssg16_random;
typedef gala::graph<ssg16rset, std::vector, uint32_t, gala_rand::svbs_config> ssg32_random;

namespace treedec{
template<>
struct graph_traits<ssg16_random>
// : public graph_traits_base?!
{ //
	typedef typename treedec_chooser<ssg16_random>::type treedec_type;
	typedef typename outedge_set<ssg16_random>::type outedge_set_type;
	typedef idgwel<ssg16_random> directed_overlay;
	typedef idgwel<ssg16_random> immutable_directed_type;
#if 1
	typedef immvecgraph<ssg16_random> immutable_type;
	typedef immvecgraph<ssg16_random> immutable_undirected_type;
#else
	typedef typename boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> immutable_type;
#endif
};
}


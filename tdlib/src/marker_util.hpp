// Felix Salfelder, 2017
//
// (c) 2017 Felix Salfelder
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#ifndef TD_MARKER_UTIL_HPP
#define TD_MARKER_UTIL_HPP

#include "marker.hpp"

namespace treedec{

template<class I, class M>
void mark_range(I i, I e, M& marker)
{
    for(; i!=e; ++i){
		 marker.mark(*i);
    }
}

} // treedec

// mark neighbours of v up to v.
template<class M, typename V, class G>
void mark_neighbours(M& marker, V v, G const& g)
{
    auto pp=boost::adjacent_vertices(v, g);
    for(; pp.first!=pp.second; ++pp.first){
		 marker.mark(*pp.first);
    }
}

template<class M, typename V, class G>
void mark_smaller_neighbours(M& marker, V v, G const& g)
{ untested();
    auto pp=boost::adjacent_vertices(v, g);
    for(; pp.first!=pp.second; ++pp.first){
        if(*pp.first>=v){ untested();
            // break; // need sorting...
            continue;
        }else{ untested();
            marker.mark(*pp.first);
        }
    }
}

// FIXME: obsolete, use without mask, use induced subgraph maybe.
// mark neighbours of v up to v.
template<class M, typename V, class G, class MASK>
size_t mark_smaller_neighbours(M& marker, V v, G const& g, MASK const& m)
{
    size_t cnt=0;
    //std::cerr << "marking for " << v << "\n";
    auto pp=boost::adjacent_vertices(v, g);
    for(; pp.first!=pp.second; ++pp.first){
        assert(*pp.first!=v);
        if(!m[*pp.first]){
            // masked...
        }else if(*pp.first>=v){
            // break; // need sorting...
        }else{
            // std::cerr << "marking " << *pp.first << "\n";
            marker.mark(*pp.first);
            ++cnt;
        }
    }
    return cnt;
}

// BUG: mask is inverted
template<class G, class MARKER, class MASK>
typename boost::graph_traits<G>::vertices_size_type
eliminate_with_marker(
		typename boost::graph_traits<G>::vertex_descriptor v,
		G& g, MARKER& marker, MASK const& mask)
{
    assert(boost::is_directed(g));
    auto nv=boost::num_vertices(g); (void)nv;
    assert(!mask[v]); // must be already gone.

    unsigned actual_degree = 0; // recompute degree of center vertex

    // auto p=boost::adjacent_vertices(elim_vertex, *this);
    auto p=boost::adjacent_vertices(v, g);
    for(; p.first!=p.second; ++p.first){
        if(!mask[*p.first]){
            continue;
        }

        marker.clear();
        // mark_smaller_neighbours(_marker, *p.first, *this); doesntwork
        mark_smaller_neighbours(marker, *p.first, g, mask);

        //        --_degree[*p.first];

        auto q=boost::adjacent_vertices(v, g);
        for(; q.first!=q.second; ++q.first){
            if(!mask[*q.first]){
                continue;
            }
            if(*q.first>=*p.first){
                // skip. TODO: more efficient skip
            }else if(marker.is_marked(*q.first)){
                // done.
            }else{
                //                ++_degree[*p.first];
                //                ++_degree[*q.first];
                treedec::add_edge(*p.first, *q.first, g);

                // treedec graph iface enforces this. (.. should)
                assert(boost::edge(*p.first, *q.first, g).second);
                assert(boost::edge(*q.first, *p.first, g).second);
            }
        }
        ++actual_degree;
    }

    return actual_degree;
} // elim with marker

#endif
// vim:ts=8:sw=4:et

// Felix Salfelder, 2015, 2016, 2017
//
// (c) 2016 Goethe-Universität Frankfurt
//     2017 Felix Salfelder
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
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
//
// graph overlays through views or (partial) copies.
//

#ifndef OVERLAY_H
#define OVERLAY_H

namespace treedec {
namespace draft {

// immutable overlay
// make a vertex range of a graph look like a graph
// this graph is immutable, to allow for efficient storage
// a callback can be used to add even more edges.
template<class G_t, class I_t, class S_t, class IG_t, class M_t, class CB_t>
inline IG_t const& immutable_clone(
     G_t const &G,
     IG_t& ig,
     I_t bbegin,
     I_t bend,
     S_t bag_nv,
    //   URGHS. no default types without c++11.
     M_t* vdMap, /*=NULL*/
     CB_t* cb
     )
{
//    typedef typename graph_traits<G_t>::immutable_type immutable_type;
    typedef typename boost::graph_traits<IG_t>::vertex_descriptor vertex_descriptor_ig;

    BOOST_AUTO(nv, boost::num_vertices(G));
    ig = MOVE(IG_t(bag_nv));

    assert(bag_nv == boost::num_vertices(ig));

    // map ig vertices (positions) to bag elements (= vertices in G)
    M_t local_vd_map;
    // std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> local_vd_map;
    if(vdMap){
        // use that...
    }else{ untested();
        vdMap = &local_vd_map;
    }
    vdMap->resize(bag_nv);
    // map vertex positions in G to vertices in ig
    std::vector<vertex_descriptor_ig> reverse_map(nv);

    BOOST_AUTO(bi, bbegin);
    BOOST_AUTO(be, bend);
    unsigned i=0;
    for(; bi!=be; ++bi){ itested();
        // FIXME: pos, vertex_index?
        assert(i < vdMap->size());
        (*vdMap)[i] = *bi;
        reverse_map[get_pos(*bi, G)] = i;
        ++i;
    }
    assert(i==bag_nv);


    bi = bbegin;
    unsigned s=-1;
    unsigned t=-1;
    unsigned vertices_count;
    for(; bi!=be; ++bi){
        ++vertices_count;
        
        if(!cb){
            BOOST_AUTO(s, get_pos(*bi, G));
            BOOST_AUTO(A, boost::adjacent_vertices(*bi,G));
            for(;A.first!=A.second;++A.first){ itested();
                BOOST_AUTO(t, get_pos(*A.first, G));
                boost::add_edge(reverse_map[s], reverse_map[t], ig);
            }
        }else{
            BOOST_AUTO(vi, bi);
            ++vi; // skip self loop

            for(; vi!=be; ++vi){
                bool edg=false;
                if(boost::edge(*bi, *vi, G).second){
                    edg = true;
                }else if(!cb){ untested();
                }else if((*cb)(*bi, *vi)){
                    edg = true;
                }else{
                    // no edge.
                }

                if(edg){
                    BOOST_AUTO(s, get_pos(*bi, G));
                    BOOST_AUTO(t, get_pos(*vi, G));
                    boost::add_edge(reverse_map[s], reverse_map[t], ig);
                }else if(s==-1u){
                    assert(get_pos(*bi, G)!=-1u);
                    s = get_pos(*bi, G);
                    t = get_pos(*vi, G);
                }else{
                }
            }
        }
    }
    // HACK. not here.
    if(cb && s!=-1u){
        /// let MSVS know about a particular new edge
        cb->a = reverse_map[s];
        cb->b = reverse_map[t];
    }else{
        // assert(is_clique(ig));
    }

    return ig;
}

// FIXME: must be more implicit...
namespace dummy_hack{
template<class VD_t>
class cb{ //
public:
    cb(){unreachable();}
    bool operator()(VD_t, VD_t){unreachable(); return false;}
public: // HACK
    unsigned a, b;
};
}

template<class G_t, class I_t, class S_t, class IG_t, class M_t>
inline IG_t const& immutable_clone(
     G_t const &G,
     IG_t& ig,
     I_t bbegin,
     I_t bend,
     S_t bag_nv,
     M_t* vdMap /*=NULL*/)
{
    typedef typename boost::graph_traits<G_t>::vertex_descriptor vd;
    dummy_hack::cb<vd>* c=NULL;
    return immutable_clone(G, ig, bbegin, bend, bag_nv, vdMap, c);
}

} // draft

} // treedec

namespace boost{

template<class G, class M>
class permute_vertex;
template<class G, class M>
class permute_edge;

} // boost

namespace treedec{

namespace draft{

template<class G, class M=boost::identity_property_map>
class const_mapped_graph{
public:
    const_mapped_graph(G const& g, M const&m) : _g(g), _m(m) {}
    const_mapped_graph(G& g, M const&m) : _g(g), _m(m) {
        incomplete();
    }
private:
    unsigned operator[](typename boost::graph_traits<G>::vertex_descriptor x){ untested();
        return _m[x];
    }
private:
public: // BUG
    G const& _g;
    M const& _m;
public:
    friend class boost::permute_vertex<G, M>;
    friend class boost::permute_edge<G, M>;
};
} // draft

template<class G, class M>
draft::const_mapped_graph<G, M>
make_mapped_graph(G const& g, M const/*?*/&m)
{ untested();
    return draft::const_mapped_graph<G, M>(g, m);
}

#if 0
template<class G, class M>
draft::const_mapped_graph<G, M>
make_mapped_graph(G& g, M const/*?*/&m)
{ incomplete();
    return draft::const_mapped_graph<G, M>(g, m);
}
#endif

} // treedec

namespace boost{

template<class G, class M>
class permute_vertex {
public:
    typedef typename M::value_type value_type;
    typedef treedec::draft::const_mapped_graph<G, M> GM;
    typedef typename boost::graph_traits<G>::vertex_descriptor G_vertex_descriptor;
public:
    permute_vertex(G_vertex_descriptor x, GM const& gm, unsigned yes) : _v(x), _m(gm._m)
    { untested();
    }
    permute_vertex(value_type x, GM const& gm) : _m(gm._m)
    { untested();
        assert(x<num_vertices(gm._g));
        bool found=false;
        (void) found;

        auto v=boost::vertices(gm._g);
        for(;v.first!=v.second; ++v.first){ untested();
            if(gm._m[*v.first] == x){ untested();
                _v = *v.first;
                found=true;
                break;
            }
        }
        assert(found);
    }
public:
    operator value_type( /* m?? */){ untested();
        return _m[_v];
    }
private:
public: // BUG
    G_vertex_descriptor _v;
    M const& _m; // needed?
};

template<class G, class M>
class permute_edge {
public:
    typedef treedec::draft::const_mapped_graph<G, M> GM;
    typedef permute_vertex<G, M> vertex_descriptor;

    typedef typename boost::graph_traits<G>::vertex_descriptor G_vertex_descriptor;
    typedef typename boost::graph_traits<G>::edge_descriptor G_edge_descriptor;
public:
    permute_edge(G_edge_descriptor e, GM const& gm) : _e(e) { untested(); }
    permute_edge(G_edge_descriptor e) : _e(e) { untested();
        assert(e==_e);
        untested();
        assert(_e==e);
        untested();
    }
public:
    template<class GM>
    vertex_descriptor source(GM const& gm){ untested();
        auto gv=boost::source(_e, gm._g);
        return vertex_descriptor(gv, gm, 1u);

    }
    vertex_descriptor target(GM const& gm){ untested();
        auto gv=boost::target(_e, gm._g);
        return vertex_descriptor(gv, gm, 1u);
    }
private:
    G_edge_descriptor _e;
};

template<class G, class M>
struct permute_vertex_iterator
    : public iterator_facade<permute_vertex_iterator<G, M>,
                             permute_vertex<G, M>,
                             bidirectional_traversal_tag,
                             permute_vertex<G, M> const&,
                             permute_vertex<G, M> const*>
{ //

};

template<class G, class M>
struct permute_edge_iterator
    : public iterator_facade<permute_edge_iterator<G, M>,
                             permute_edge<G, M>,
                             bidirectional_traversal_tag,
                             permute_edge<G, M>,
                             permute_edge<G, M>*>
{ //
public:
    typedef typename boost::graph_traits<G>::edge_iterator G_edge_iterator;
    typedef typename boost::graph_traits<G>::vertex_descriptor G_vertex_descriptor;
public:
    permute_edge_iterator(G_edge_iterator e, M const& m)
        : _e(e), _m(m) { untested(); }
private:
    permute_edge<G, M> dereference() const{ untested();
        auto E=*_e;
        return permute_edge<G, M>(E);
    }

    bool equal(const permute_edge_iterator& other) const { untested();
        return _e==other._e;
    }

    void increment() { untested();
        ++_e;
    }
    void decrement() { untested();
        --_e;
    }

    G_edge_iterator _e;
    M const& _m;
public:
    friend class iterator_core_access;
};

template<class G, class M>
struct graph_traits<treedec::draft::const_mapped_graph<G, M> >{
    // staticassert convertibleto(G::vertex_descriptor, M::valuetype)
    typedef permute_edge<G, M> edge_descriptor;
    typedef permute_vertex<G, M> vertex_descriptor;
    typedef typename graph_traits<G>::vertices_size_type vertices_size_type;
    typedef permute_vertex_iterator<G, M> vertex_iterator;
// incomplete    typedef permute_adjacency_iterator<G, M> adjacency_iterator;
    typedef permute_edge_iterator<G, M> edge_iterator;

    typedef typename G::directed_category      directed_category;
    typedef typename G::edge_parallel_category edge_parallel_category;
    typedef typename G::traversal_category     traversal_category;
};

template<class G, class M>
permute_vertex<G, M> source(
        permute_edge<G, M> e,
        treedec::draft::const_mapped_graph<G, M> const& g){ untested();
    return e.source(g);
}

template<class G, class M>
permute_vertex<G, M> target(
        permute_edge<G, M> e,
        treedec::draft::const_mapped_graph<G, M> const& g){ untested();
    return e.target(g);
}

template<class G, class M>
typename graph_traits<G>::vertices_size_type num_vertices(
        treedec::draft::const_mapped_graph<G, M> const& gm){ itested();
    return num_vertices(gm._g);
}

template<class G, class M>
typename graph_traits<G>::edges_size_type
num_edges(treedec::draft::const_mapped_graph<G, M> const& gm)
{ itested();
    if(!num_edges(gm._g)){ untested(); }

    return num_edges(gm._g);
}

template<class G, class M>
typename permute_edge<G, M>::vertex_descriptor vertex(
        unsigned x,
        treedec::draft::const_mapped_graph<G, M> const& gm)
{ untested();
    return permute_vertex<G, M>(x, gm);
}

template<class G, class M>
std::pair<permute_vertex_iterator<G, M>, permute_vertex_iterator<G, M> > vertices(
        treedec::draft::const_mapped_graph<G, M> const& gm)
{ incomplete();
}

template<class G, class M>
std::pair<permute_edge<G, M>, bool> edge(
        typename graph_traits<treedec::draft::const_mapped_graph<G, M> >::vertex_descriptor s,
        typename graph_traits<treedec::draft::const_mapped_graph<G, M> >::vertex_descriptor t,
        treedec::draft::const_mapped_graph<G, M> const& gm)
{ untested();
    typedef typename boost::graph_traits<G>::edge_descriptor GE;
    std::pair<GE, bool> gedge=edge(s._v, t._v, gm._g);

    permute_edge<G, M> pedge(gedge.first, gm);

    return std::make_pair(pedge, gedge.second);
}

template<class G, class M>
std::pair<permute_edge_iterator<G, M>, permute_edge_iterator<G, M> >
edges(treedec::draft::const_mapped_graph<G, M> const& gm)
{ untested();
    typedef permute_edge_iterator<G, M> I;
    auto ge=edges(gm._g);
    if(ge.first==ge.second){ untested();
        assert(I(ge.first, gm._m) == I(ge.first, gm._m) );
    }else{ untested();
        assert(I(ge.first, gm._m) != I(ge.second, gm._m) );
    }
    return std::make_pair(I(ge.first, gm._m),
                          I(ge.second, gm._m));
}

}

#endif // guard

// vim:ts=8:sw=4:et

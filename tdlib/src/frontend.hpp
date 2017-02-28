// Felix Salfelder, 2017
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
//

#ifndef TD_FRONTEND_HPP
#define TD_FRONTEND_HPP

#include <vector>
#include <list>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/variant.hpp>
#include "trace.hpp"
#include "graph.hpp"
#include <boost/tuple/tuple.hpp>
#include "exception.hpp"
#include "bits/any_iterator.hpp"

namespace treedec{

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> TD_graph_t;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> TD_graph_vec_t;
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> TD_graph_directed_t;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> TD_graph_directed_vec_t;

namespace detail{

template<class I, class X=void>
struct fill_help{

    template<class vit>
    static void fill_idmap(vit i, vit e, I& idm, unsigned & /*max*/, unsigned&)
    { untested();
        for(;i!=e; ++i){ untested();
            idm.push_back(*i);
        }
    }

template <typename G_t, class IDM, class it, class it2>
static void fill(G_t &G, IDM const& idmap, it, it, it2 i2, it2 e2, size_t max)
{ untested();
    trace1("fill", max);
    typedef std::map<typename it::value_type, unsigned> idxmap_type;
    // hmm only unsigned?
    idxmap_type idxMap;
    auto bi=boost::vertices(G).first;
    // for(; ii!=ee; ++ii){ untested(); }
    for(auto ii=idmap.begin(); ii!=idmap.end(); ++ ii){ untested();
        idxMap[*ii] = *bi;
        ++bi;
        trace1("map", max);
    }

    for(; i2!=e2; ++i2) { untested();
        auto p=*i2;

        if(idxMap.find(p.first)==idxMap.end()){ untested();
            throw exception_invalid("source invalid");
            // + std::to_string(p.first) + ":" + std::to_string(p.first));
        }else if(idxMap.find(p.second)==idxMap.end()){ untested();
            throw exception_invalid("target invalid");
            // + std::to_string(p.second) + ":" + std::to_string(p.second));
        }else{ untested();
            treedec::add_edge(idxMap[p.first], idxMap[p.second], G);
        }
    }
}
};

// for "graph"
template<>
struct fill_help< boost::typed_identity_property_map<long unsigned int> > {
template<class vit, class I>
static void fill_idmap(vit i, vit e, I&, unsigned & max, unsigned& cnt)
{ untested();
    for(;i!=e; ++i){ untested();
        ++cnt;
//        _idmap.push_back(*i);
        if(unsigned(*i) > max){ untested();
            max = *i;
        }else{ untested();
        }
    }
}

template <typename G_t, class I, class it>
static void fill(G_t &G, I const&, it i2, it e2, it, it, size_t max)
{ untested();
    trace1("fill", max);

    for(; i2!=e2; ++i2) { untested();
        auto p=*i2;

        if(unsigned(p.first)>=max){ untested();
            throw exception_invalid("source invalid "
             + std::to_string(p.first) + ":" + std::to_string(p.first));
        }else if(unsigned(p.second)>=max){ untested();
            throw exception_invalid("target invalid"
             + std::to_string(p.second) + ":" + std::to_string(p.second));
        }else{ untested();
            // takes care of reverse edges in directed graphs.
            treedec::add_edge(p.first, p.second, G);
        }
    }
}
};

//template<class idmap_t>
// "subgraph". essentially the same as Graph...
template<>
struct fill_help<std::vector<unsigned> > {
    //, std::enable_if< std::numeric_limits<idmap_t::value_type >::is_integer >::type > 
    template<class vit, class I>
    static void fill_idmap(vit i, vit e, I& _idmap, unsigned & max, unsigned& cnt)
    { untested();
        for(;i!=e; ++i){ untested();
            ++cnt;
            _idmap.push_back(*i);
            if(unsigned(*i) > max){ untested();
                max = *i;
            }else{ untested();
            }
        }
    }
template <typename G_t, class I, class it, class eit>
static void fill(G_t &G, I const& idmap, it, it, eit i2, eit e2, size_t max)
{ untested();
    trace1("fill", max);
    // hmm only unsigned?
    typedef std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> idxmap_type;
    idxmap_type idxMap(max+1, -1u);
    auto bi=boost::vertices(G).first;
    for(auto iii=idmap.begin(); iii!=idmap.end(); ++iii){ untested();
        trace2("map", *iii, *bi);
        idxMap[*iii] = *bi;
        ++bi;
    }

    for(; i2!=e2; ++i2) { untested();
        auto p=*i2;

        if(idxMap.size() <= p.first || idxMap[p.first]==-1u){ untested();
            trace3("wrong?", p.first, p.second, max);
            throw exception_invalid("source invalid " + std::to_string(p.first)
                    + ":" + std::to_string(p.second));
        }else if(idxMap.size() <= p.first || idxMap[p.second]==-1u){ untested();
            throw exception_invalid("target invalid " + std::to_string(p.first)
                    + ":" + std::to_string(p.second));
        }else{ untested();
            treedec::add_edge(idxMap[p.first], idxMap[p.second], G);
        }
    }
}
};


} // detail


// this is meant to be an undirected loopless simple graph.
// (even if the backend is directed...)
//
// class graph_frontend?
namespace frontend{

template<class T>
struct vd_sel{
    typedef typename T::value_type type;
};

using IteratorTypeErasure::any_iterator;

struct dummy{};

template<class idmap_type=std::vector<unsigned> >
class graph{
public:
    using backend_t=boost::variant<TD_graph_t, TD_graph_vec_t,
                                   TD_graph_directed_t, TD_graph_directed_vec_t>;
    typedef typename vd_sel<idmap_type>::type vertex_descriptor;
    typedef std::pair<vertex_descriptor, vertex_descriptor> edge_descriptor;

    // backend iteration. private?!
    typedef any_iterator<const unsigned,
                         std::input_iterator_tag,
                         const unsigned> raw_vertex_iterator;

    typedef any_iterator<const edge_descriptor,
                         std::input_iterator_tag,
                         const edge_descriptor> edge_iterator;

    enum backend_type{
        b_default=0,
        b_badjl_vvd=4
    };
private: // visitors
    struct name : boost::static_visitor<std::string> {
        std::string operator()(TD_graph_t const&) const { untested();
            return "TD_graph_t";
        }
        std::string operator()(TD_graph_vec_t const&) const { untested();
            return "TD_graph_vec_t";
        }
        std::string operator()(TD_graph_directed_t const&) const { untested();
            return "TD_graph_t";
        }
        std::string operator()(TD_graph_directed_vec_t const&) const { untested();
            return "TD_graph_vec_t";
        }

        template<typename T>
        std::string operator()(T const&) const { throw "Not implemented"; } // TODO proper exception
    };
    struct get_verts;

    struct get_verts
      : boost::static_visitor<
           std::pair<  boost::transform_iterator<get_verts, raw_vertex_iterator> ,
                       boost::transform_iterator<get_verts, raw_vertex_iterator> > > {
          typedef boost::transform_iterator<get_verts, raw_vertex_iterator> vertex_iterator;

          get_verts(idmap_type const& m)
              : _m(m)
          { untested();
          }
          get_verts(get_verts const& o)
              : _m(o._m)
          { untested();
          }
        template<typename T>
        std::pair<vertex_iterator, vertex_iterator>
        operator()(T const& t) const { untested();
            auto x=boost::vertices(t);
            auto b=raw_vertex_iterator(x.first);
            auto e=raw_vertex_iterator(x.second);

            vertex_iterator bt=make_transform_iterator(b, *this);
            vertex_iterator et=make_transform_iterator(e, *this);

            return std::make_pair(bt, et);
        }

        // hack. need proper idmaps, then pass
        // idmap to make_transform_iterator directly.
        typename idmap_type::value_type operator()(unsigned x) const {
            return _m[x];
        }

        idmap_type const& _m;
    };


    struct get_edgs
      : boost::static_visitor<
           std::pair< edge_iterator ,
                      edge_iterator> >  {

          template<class G>
        struct edgpairmap{
              edgpairmap(idmap_type const& m, G const& g)
                  : _m(m), _g(g)
              { untested();
              }
            template<class E>
            std::pair<typename idmap_type::value_type,
                      typename idmap_type::value_type>
                          operator()(E edg) const {
                auto s=boost::source(edg, _g);
                auto t=boost::target(edg, _g);
                return std::make_pair(_m[s], _m[t]);
            }
            idmap_type const& _m;
            G const& _g;
        };
//        typedef boost::transform_iterator<edgpairmap, raw_edge_iterator> edge_iterator;

          get_edgs(idmap_type const& m)
              : _m(m)
          { untested();
          }
          get_edgs(get_edgs const& o)
              : _m(o._m)
          { untested();
          }

        template<typename G>
        std::pair<edge_iterator, edge_iterator>
        operator()(G const& g) const { untested();
            typedef typename boost::graph_traits<G>::edge_iterator geit_t;
            auto x=boost::edges(g);
            geit_t b(x.first);
            geit_t e(x.second);

            // translation
            auto bt=make_transform_iterator(b, edgpairmap<G>(_m, g));
            auto et=make_transform_iterator(e, edgpairmap<G>(_m, g));

            // "cast" to any_iterator
            return std::make_pair(edge_iterator(bt),
                                  edge_iterator(et));
        }

        idmap_type const& _m;
        // G const& _g;
    };
public:
    // typedef boost::transform_iterator<idmap_type, raw_vertex_iterator> vertex_iterator;
    typedef typename get_verts::vertex_iterator vertex_iterator;
//    typedef typename get_edgs::edge_iterator edge_iterator;
private:

    struct num_vert : boost::static_visitor<size_t> {
        template<typename T>
        size_t operator()(T const& t) const { untested();
            return boost::num_vertices(t);
        }
    };
    struct num_edg : boost::static_visitor<size_t> {
        template<typename T>
        size_t operator()(T const& t) const { untested();
            return treedec::num_edges(t);
        }
    };
public: // construct
    graph(std::string const&){ untested();
        incomplete();
    }
    graph(size_t, std::string const&){ untested();
        incomplete();
    }
    graph(size_t cnt=0, unsigned type=0)
    { untested();
        switch(type){ untested();
        case 0:
            _g = TD_graph_t(cnt);
            break;
        case b_badjl_vvd:
            _g = TD_graph_directed_vec_t(cnt);
            break;
        default:
            throw exception_invalid("unknown type");
        }
    }
    template<class eit>
    graph( eit i2, eit e2, unsigned x, unsigned type=0)
    { untested();
        trace2("creating graph from edgiter ", x, type);
        typedef detail::fill_help<idmap_type> fh;
        switch(type){ untested();
        case 0:
            _g = TD_graph_t(x);
            // no. does not work on directed graphs
            // fill(boost::get<TD_graph_t>(_g), i, e, E, max);
            fh::fill(boost::get<TD_graph_t>(_g), _idmap, i2, e2, i2, e2, x);
            break;
        case b_badjl_vvd: untested();
            _g = TD_graph_directed_vec_t(x);
            // fill(boost::get<TD_graph_directed_vec_t>(_g), i, e, E, max);
            fh::fill(boost::get<TD_graph_directed_vec_t>(_g), _idmap, i2, e2, i2, e2, x);
            break;
        default:
            throw exception_invalid("unknown type");
        }
    }

    // legacy stuff. only works with list<int>, list<tuple<int,int>>
    // no type checking...
    // this should create a graph with vertices mapped to what's in the first array
    // passed to it. perhaps the identifiers need to be unique...
    //graph(const boost::python::list& v,
    //         const boost::python::list& edg)
    // turnaround edges first.
    template<class vit, class eit>
    graph(eit i2, eit e2, vit i, vit e, unsigned type=0)
    { untested();
        unsigned int max = 0;
        unsigned int cnt = 0;
        typedef detail::fill_help<idmap_type> fh;
        fh::fill_idmap(i, e, _idmap, max, cnt);

        /// if type=0 etc
        switch(type){ untested();
        case 0:
            _g = TD_graph_t(cnt);
            fh::fill(boost::get<TD_graph_t>(_g), _idmap, i, e, i2, e2, max);
            break;
        case b_badjl_vvd:
            _g = TD_graph_directed_vec_t(cnt);
            fh::fill(boost::get<TD_graph_directed_vec_t>(_g), _idmap, i, e, i2, e2, max);
            break;
        default:
            throw exception_invalid("unknown type");
        }
    }

    graph(std::list<int>const& v, std::list<unsigned> const& e,
            backend_type b=b_default)
    { incomplete();
    }
    graph(std::vector<unsigned> v, std::vector<unsigned> e,
            backend_type b=b_default)
    { incomplete();
        _g = TD_graph_t();
//        make_tdlib_graph(boost::get<TD_graph_t>(_g), v, e);
    }
public:
    graph(const graph& o)
       : _g(o._g)
    { untested();
    }
public: //ops
    graph& operator=(const graph& o){ untested();
        // not used from within python.
        // not even accessible?!
        _g = o._g;
        return *this;
    }
public: // const access
    std::pair<vertex_iterator, vertex_iterator> vertices() const{ untested();
        return boost::apply_visitor(get_verts(_idmap), _g);
    }
    std::pair<edge_iterator, edge_iterator> edges() const{ untested();
        return boost::apply_visitor(get_edgs(_idmap), _g);
    }
    size_t num_vertices() const{ untested();
        return boost::apply_visitor(num_vert(), _g);
    }
    size_t num_edges() const{ untested();
        return boost::apply_visitor(num_edg(), _g);
    }
    std::string backend_typename() const{ untested();
        return boost::apply_visitor(name(), _g);
    }
public: // graph algorithms. maybe wrong place.
    template<class tdt>
    size_t preprocessing(tdt&);
    void min_degree();
    void exact_cutset();
    void exact_ta();
public: // hack
    template<class U>
    vertex_descriptor maphack(U x) const{ untested();
        assert(x<num_vertices());
        return _idmap[x];
    }
private:
    backend_t _g;
    idmap_type _idmap;
    // numbering?
    // order?
    // bags?? (use for undirected?)
};
}

// typedef frontend::graph<> anygraph;
} // treedec

#endif // guard
// vim:ts=8:sw=4:et

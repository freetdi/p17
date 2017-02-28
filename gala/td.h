// Felix Salfelder 2016-2017
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
//
//
// treedec routines specialized for gala backend.
//
#ifndef GALA_TREEDEC_H
#define GALA_TREEDEC_H

#include "boost.h"
#include "sfinae.h"
#include <tdlib/graph_traits.hpp>
//#include <tdlib/treedec_traits.hpp>
#include <boost/graph/iteration_macros.hpp>
//hack
#include <stx/btree_set.h>
#include <unordered_set>

#include "sethack.h"
//#include "parallel.h"
// #include "degs.h"

#define q_in_cb

#if 0 // now in tdlib/graph_traits.hpp
// HACK HACK HACK
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
#endif

namespace treedec { // here??

template<class CB, class G>
class callback_proxy{ //
public:
	typedef typename boost::graph_traits<G>::vertex_descriptor vertex_descriptor;
public:
	callback_proxy(vertex_descriptor v, CB* cb, G const& g)
	    : _done(false), _v(v), _cb(cb), _g(g)
	{ untested();
		assert( g.is_valid(v) );
	}
	void operator()(vertex_descriptor w)
	{ untested();
//		BUG:: :w
		if(!_cb){ itested();
		}else if(_v<w){ itested();
			trace2("", _v, w);
//			assert(boost::edge(_v,w,_g).second); //not necessarily
			(*_cb)(_v, w);
#ifdef q_in_cb
			{ itested();
				(*_cb)(_v);
				(*_cb)(w);
			}
#endif
			_done=true;
		}else{ untested();
		}
	}
	bool done() const
	{ untested();
		return _done;
	}
private:
	bool _done;
	vertex_descriptor _v;
	CB* _cb;
	const G& _g;
};

template<galaPARMS, class CB,
         typename=void /*required for sfinae*/ >
struct sghelp_hack{
static size_t mcah(
		typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor c,
		gala::graph<SGARGS>& g,
// 		typename treedec::graph_traits< gala::graph<SGARGS> >::bag_type& bag,
		 typename treedec::treedec_traits<
		   typename treedec::treedec_chooser<  gala::graph<SGARGS>  >::type>::bag_type& bag,
		CB* cb)
{ untested();
	typedef gala::graph<SGARGS> G;
	typedef typename boost::graph_traits<G>::vertex_descriptor vertex_descriptor;
	typedef typename boost::graph_traits<G>::adjacency_iterator Iter;
	typedef typename boost::graph_traits<G>::vertex_descriptor VD;
#ifndef NDEBUG
	treedec::check(g);
#endif
	VD x; (void) x;
	Iter I, J, K, E;
	boost::tie(I, E) = boost::adjacent_vertices(c, g);
	long n=0;

	for(J=boost::adjacent_vertices(c, g).first; J!=E; ++J){ untested();
		vertex_descriptor N=*J;
#ifndef q_in_cb
		if(cb){untested();
			(*cb)(N);
		}
#endif

		assert(*J!=c);
		assert(g.is_valid(N));
		unsigned m=g.out_edges(N).erase(c);
		assert(m==1); (void) m;
		n += -g.out_edges(N).size();
		auto II(sethack::make_skip_it(I, E, N));

		callback_proxy<CB, G> cbp(N, cb, g);

		// merge II .. E into out_edges(N)
		// call back on newly inserted out edges.
		//  ... before inserting them.
		zipfwdb4(g.out_edges(N), II, E, &cbp);
		if(cbp.done()){ untested();
			assert(cb);
		// 	(*cb)(N); not yet.
		}else{ untested();
		}
		n += g.out_edges(N).size();
	}

	assert(!(n%2));
	n /= 2; // edges added.
//	typedef typename outedge_set< gala::graph<SGARGS> >::type bag_type;
	assert(!bag.size());
	bag = std::move(g.out_edges(c));

	// FIXME: this might be completely unnecessary.
	g._num_edges += n;
	g._num_edges -= bag.size();
	g.out_edges(c).clear();
	return n;
}

/*----------------------------------*/
// contract edge. it better exists
// FIXME: pass EDGE, not pair of vertices.
typedef typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor vertex_descriptor;
	static void ce(vertex_descriptor vd, vertex_descriptor into,
	                   gala::graph<SGARGS> &g,
							 bool erase=true,
	                   treedec::vertex_callback<typename gala::graph<SGARGS>::vertex_type >* cb=NULL)
	{ itested();
		typedef gala::graph<SGARGS> G;
		(void) erase;
		assert(vd!=into);
		{ /// 	g.contract(vd,into);

			auto x=g.out_edges(vd).find(into);
			if(x!=g.out_edges(vd).end()){ itested();
				// delete edge in between
				g.out_edges(vd).erase(x); // can we avoid it? we will vd.clear() later on.
				--g._num_edges; // hack!!
				x = g.out_edges(into).find(vd);
				assert(x!=g.out_edges(into).end());
				g.out_edges(into).erase(x);
			}else{ untested();
				assert(false); // no edge to contract.
			}

			// merge outnodes.
			auto E = g.out_edges(vd).end();
#define OLD // slightly(!) slower. sometimes.
#ifdef OLD
			auto I = g.out_edges(vd).begin();
			zipfwdb4(g.out_edges(into), I, E);
#endif

			for(typename G::EL::iterator iI=g.out_edges(vd).begin();
			    iI!=E; ++iI){
				typename G::vertex_type I = *iI;
				assert(I != into);
				bool inserted_new = g.out_edges(I).insert(into).second;
				if(inserted_new){ itested();
					// HERE: insert the other. with hint?
#ifndef OLD
					inserted_new = g.out_edges(into).insert(I).second;
					assert(inserted_new);
#endif
					// edge has moved.
				}else{itested();
					// there are two, one of which will be gone.
					if(cb){
						(*cb)(I);
					}
					--g._num_edges; // hack!!
				}

				{itested();
				assert(g.out_edges(I).end() != g.out_edges(I).find(vd));
				g.out_edges(I).erase(vd);
				}
			} // for

			// urgs. might be unnecessary
			// case of erase=true (next function)
			g.out_edges(vd).clear();
		}
	}
}; // sghelp_hack

template<galaPARMS, class CB>
struct sghelp_hack< ECT, VCT, VDP, CFG, CB,
	typename gala::sfinae::is_vec_tpl<ECT>::type > { //
static size_t mcah(
		typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor,
		gala::graph<SGARGS>&,
// 		typename treedec::graph_traits< gala::graph<SGARGS> >::bag_type& bag,
		 typename treedec::treedec_traits<
		   typename treedec::treedec_chooser<  gala::graph<SGARGS>  >::type>::bag_type&,
		CB*)
{ untested();
	incomplete();
	assert(0);
	// BOOST_STATIC_ASSERT(0);
	return 0;
}
typedef typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor vertex_descriptor;
	static void ce(vertex_descriptor vd, vertex_descriptor into,
	                   gala::graph<SGARGS> &g,
							 bool erase=true,
	                   treedec::vertex_callback<typename gala::graph<SGARGS>::vertex_type >* cb=NULL)
{ untested();
	incomplete();
	assert(0);
	// BOOST_STATIC_ASSERT(0);
}
};

} //treedec

namespace treedec { //
	template<typename G>
	using vertex_iterator = typename boost::graph_traits<G>::vertex_iterator;
	template<typename G>
	using vertex_descriptor = typename boost::graph_traits<G>::vertex_descriptor;
	template<typename G>
	using adjacency_iterator = typename boost::graph_traits<G>::adjacency_iterator;

	VCTtemplate
	void remove_vertex(typename boost::graph_traits<gala::graph<SGARGS> >::vertex_iterator u,
	                   gala::graph<SGARGS> &g)
	{ // untested();
		g.remove_vertex(u.value());
	}

	VCTtemplate
	bool is_valid(typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
	              gala::graph<SGARGS> const& g)
	{
		return g.is_valid(v);
	}

//	template<class G>
//	bool is_valid(typename boost::graph_traits<G>::vertex_descriptor,
//	              G const&)
//	{ untested();
//		return false;
//	}

	VCTtemplate
	void contract_edge(vertex_descriptor<gala::graph<SGARGS> > vd,
	                   vertex_descriptor<gala::graph<SGARGS> > into,
	                   gala::graph<SGARGS> &g,
							 bool erase=true,
	                   vertex_callback<typename gala::graph<SGARGS>::vertex_type >* cb=NULL)
	{ itested();
//		typedef typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor vertex_descriptor;
		typedef vertex_callback<typename gala::graph<SGARGS>::vertex_type> CB;
//		typedef gala::graph<SGARGS> G;
		(void) erase;
		assert(vd!=into);
		treedec::sghelp_hack<ECT, VCT, VDP, CFG, CB>::ce(vd, into, g, erase, cb);
	}

	// weird wrapper. maybe irrelevant.
	VCTtemplate
	void contract_edge(vertex_iterator<gala::graph<SGARGS> > v,
			vertex_descriptor<gala::graph<SGARGS> > into,
			gala::graph<SGARGS> &g,
			bool erase=true,
			vertex_callback<typename gala::graph<SGARGS>::vertex_type >* cb=NULL)
	{ untested();
		if(erase){ untested();
		}
		contract_edge(*v,into,g, erase, cb);

		//		boost::clear_vertex(*v, g);
		if(erase){ incomplete();
			// broken for vector!
			remove_vertex(v, g);
			// *v = into; does not work. for some reason
		}else{ untested();
			assert(g.degree(*v)==0);
			//			(*v)->n.clear();
		}
	}
} // treedec

namespace treedec { // BUG in graphviz?
	// required for _new algo...?
	VCTtemplate
	static inline void add_edge(typename gala::graph<SGARGS>::vertex v,
			typename gala::graph<SGARGS>::vertex w,
			gala::graph<SGARGS> &G){ untested();
		 G.add_edge(v, w);
	}

}
namespace treedec{

	// get a vertex identifier.
	VCTtemplate
	vertex_descriptor<gala::graph<SGARGS> > get_vd(const gala::graph<SGARGS>&,
			const vertex_descriptor<gala::graph<SGARGS> >& v )
	{ itested();
		return v;
	}

	VCTtemplate
	void insert_neighbours
	   (typename outedge_set< gala::graph<SGARGS> >::type &b,
	    typename gala::graph<SGARGS>::vertex_type v,
	    gala::graph<SGARGS> const &g)
	{ untested();
		zipfwdb4(b, g.out_edges(v));
	}

	VCTtemplate
	void assign_neighbours
	   (typename outedge_set< gala::graph<SGARGS> >::type &b,
	    typename gala::graph<SGARGS>::vertex_type v,
	    gala::graph<SGARGS> const &g)
	{ itested();
		b = g.out_edges(v);
	}

	VCTtemplate
	void assign_neighbours
	   (typename outedge_set< gala::graph<SGARGS> >::type &b,
	    typename gala::graph<SGARGS>::vertex_type v,
	    typename gala::graph<SGARGS>::vertex_type w,
	    typename gala::graph<SGARGS>::vertex_type x,
	    gala::graph<SGARGS> const &g)
	{ untested();
		// not efficient yet...
		b = g.out_edges(v);
		insert_neighbours(b, w, g);
		insert_neighbours(b, x, g);
	}

// FIXME: always hijack like this, if
// :outedge_set<gala::graph<SGARGS> >::type is treedec_chooser::bag_type?
	VCTtemplate
	typename outedge_set<gala::graph<SGARGS> >::type detach_neighborhood(
			  typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor& c,
			  gala::graph<SGARGS> & g,
			  typename outedge_set<gala::graph<SGARGS> >::type* N)
	{ untested();
		assert(N==nullptr); // for now..
		incomplete(); //	unreachable...
		 typename treedec::outedge_set<gala::graph<SGARGS> >::type bag;
		 typename boost::graph_traits<gala::graph<SGARGS> >::adjacency_iterator nIt1, nIt2, nEnd;
		 // inefficient.
		 for(boost::tie(nIt1, nEnd) = boost::adjacent_vertices(c, g);
					nIt1 != nEnd; nIt1++){ // untested();
			  bag.insert(treedec::get_vd(g, *nIt1));
		 }
		 return bag;
	}

	template<galaPARMS>
	size_t make_clique_and_detach(
			typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor c,
			gala::graph<SGARGS>& g,
			typename outedge_set< gala::graph<SGARGS> >::type& bag,
			typename treedec::graph_callback<gala::graph<SGARGS> >* cb=NULL)
	{ untested();
		typedef typename treedec::graph_callback<gala::graph<SGARGS> > CB;
		return treedec::sghelp_hack<ECT, VCT, VDP, CFG, CB>::mcah(c, g, bag, cb);
	}

	// theres no Vertex. use the position instead
	template< template<class T, class... > class ECT, class VDP >
	unsigned get_id(const gala::graph<ECT,std::vector, gala::vertex_ptr_tag>& g,
			const vertex_descriptor<gala::graph<ECT,std::vector, gala::vertex_ptr_tag> >& v )
	{ untested();
		unsigned p=g.position(v);
		assert(p<g.num_vertices());
		assert(0); // for now (this is dangerous).
		return p;
	}

	VCTtemplate
	unsigned get_id(const gala::graph<SGARGS>& /*g*/,
			const vertex_descriptor<gala::graph<SGARGS> >& /*v*/ )
	{untested();
		incomplete();
		return 0;
	}

	VCTtemplate
	struct outedge_set< gala::graph<SGARGS> >{
		typedef typename gala::graph<SGARGS>::EL type;
	};

	VCTtemplate
	struct treedec_chooser< gala::graph<SGARGS> >{
		typedef vertex_descriptor< gala::graph<SGARGS> > value_type;
		typedef typename outedge_set< gala::graph<SGARGS> >::type bag_type;
		typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, bag_type> type;
	};

#if 0
VCTtemplate
struct treedec_traits< treedec_chooser<gala::graph<SGARGS> > >{ //
	typedef vertex_descriptor<gala::graph<SGARGS> > vd_type;
	typedef typename outedge_set< gala::graph<SGARGS> >::type bag_type;
};
#endif

//VCTtemplate
// FIXME: more sgs.
template<>
struct treedec_traits< boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
	std::set<gala::graph<std::set, std::vector, gala::vertex_ptr_tag>::vertex_*> > >{ //
	typedef typename gala::graph<std::set, std::vector, gala::vertex_ptr_tag>::vertex_* vd_type;
 	typedef typename outedge_set< gala::graph<std::set, std::vector, gala::vertex_ptr_tag> >::type bag_type;
};

#if 0
/// something like that...
template<class T>
struct treedec_traits { //
	typedef typename T::vertex_property_type::value_type;
 	typedef typename outedge_set< G::vertex_property_type >::type bag_type;
};
///
template<class U>
struct treedec_traits< typename boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
	 std::set< typename gala::graph<std::set, std::vector, U>::vertex_*> > >{ //
	typedef typename gala::graph<std::set, std::vector, U>::vertex_* vd_type;
 	typedef typename outedge_set< gala::graph<std::set, std::vector, U> >::type bag_type;
};
#endif

VCTtemplate
inline typename treedec_traits<gala::graph<SGARGS> >::bag_type&
   bag(gala::graph<SGARGS>& /*t*/,
	const vertex_descriptor<gala::graph<SGARGS> >& /*v*/)
{ untested();
	incomplete();
   // return g[v].bag;
}


using boost::setS;
using boost::vecS;
using boost::undirectedS;

typedef  boost::adjacency_list<setS, vecS, undirectedS, \
	 std::set< gala::graph<std::set, std::vector, gala::vertex_ptr_tag>::vertex_*> > TDT;

//VCTtemplate //not yet.
inline typename treedec_traits<TDT>::bag_type&
   bag(TDT& t,
	const typename boost::graph_traits<TDT>::vertex_descriptor& v)
{ itested();
   return t[v];
}

template<class G>
size_t degree(G const& g)
{untested();
	size_t ret=0;
	auto i = boost::vertices(g).first;
	auto e = i;

	for(boost::tie(i,e) = boost::vertices(g); i!=e; ++i){untested();
		size_t d=boost::out_degree(*i,g);
		if(ret<d) ret = d;
	}
	return ret;
}

VCTtemplate
size_t degree(gala::graph<SGARGS> const& g)
{ untested();
	return g.degree();
}

VCTtemplate
void check(gala::graph<SGARGS> const& g)
{itested();
	typedef gala::graph<SGARGS> G;
	typedef typename boost::graph_traits<G>::vertex_descriptor vertex_descriptor;
	using adjacency_iterator = typename boost::graph_traits<G>::adjacency_iterator;

	unsigned edges=0;
	auto i = boost::vertices(g).first;
	auto e = i;
#ifdef DEBUG
	auto I = i;
	auto E = i;
#endif

#ifdef MORE_DEBUG
	for(boost::tie(i,e) = boost::vertices(g); i!=e; ++i){untested();
//		assert(treedec::is_valid(iter::deref(i),g));
	}
#endif

	for(boost::tie(i,e) = boost::vertices(g); i!=e; ++i){
	//	assert(treedec::is_valid(*i,g));
		auto j = boost::out_edges(*i,g).first;
		auto f = j;
		for(boost::tie(j,f) = boost::out_edges(*i,g); j!=f; ++j){
			++edges;
		}
	}
	assert(g.is_directed() || !(edges%2));

	if(g.is_directed()){
	}else{
		edges/=2;
	}

//	trace3("check edgecount", edges, boost::num_edges(g), g.is_directed() );
	if (edges > boost::num_edges(g)){untested();
		assert(false);
	}else if (edges < boost::num_edges(g)){untested();
		assert(false);
	}

	adjacency_iterator aI, aE;

	std::set<vertex_descriptor> X;
	for(boost::tie(i,e) = boost::vertices(g); i!=e; ++i){ itested();
		X.clear();
		for(boost::tie(aI,aE) = boost::adjacent_vertices(*i,g); aI!=aE; ++aI){ itested();
			assert(*aI!=*i);
			X.insert(*aI);

#ifdef DEBUG
			assert(boost::edge(*i,*aI,g).second);
			if(!g.is_directed()){ untested();
				assert(boost::edge(*aI,*i,g).second);
			}
#endif
		}
		// if !multiedge
		assert(X.size() == boost::out_degree(*i, g));
#ifdef DEBUG
		if(!g.is_directed()){ untested();
			for(boost::tie(I,E) = boost::vertices(g); I!=E; ++I){ itested();
				assert(boost::edge(*I,*i,g).second == boost::edge(*i,*I,g).second);
			}
		}
#endif
	}
}

	VCTtemplate
	struct deg_chooser<gala::graph<SGARGS> >{
		typedef gala::graph<SGARGS> G;
		typedef CFG<G> cfg;
		typedef typename cfg::degs_type type;
		typedef typename G::vertex_type vd_type;

#if __cplusplus < 201103L
		typedef std::set<vd_type> bag_type;
#else
		typedef std::unordered_set<vd_type> bag_type;
#endif
	};
}

namespace treedec{ //

VCTtemplate
inline size_t count_missing_edges(
		const typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
		gala::graph<SGARGS> const &g)
{ itested();

	size_t missing_edges=0;

	auto V=v;
	auto e=g.out_edges(V);
	for(auto nIt1 = e.begin(); nIt1!=e.end(); ++nIt1){ untested();
		auto nIt2 = nIt1;
		++nIt2;
		auto x=*nIt1; // hack for svu. something about const.
		missing_edges += count_missing(g.out_edges(x), nIt2, e.end());
	}
	return missing_edges;
}

namespace detail{ //
	// iterate over edges adjacent to both v and s
	// implementation: iterate over outedges(v), skip non-outedges(s).
	template<galaPARMS>
	class shared_adj_iter<gala::graph<SGARGS> >
	    : public intersection_iterator<typename gala::graph<SGARGS>::EL,
		                                typename gala::graph<SGARGS>::EL> { //
	public: // types
		typedef typename gala::graph<SGARGS> G;
		typedef typename gala::graph<SGARGS>::EL S;
		typedef intersection_iterator<S, S> parent;
		typedef typename boost::graph_traits<G>::adjacency_iterator adjacency_iterator;
		typedef typename boost::graph_traits<G>::vertex_descriptor vertex_descriptor;
	public: // construct
		shared_adj_iter(vertex_descriptor v,
		                vertex_descriptor w, G const& g)
		    : parent(g.out_edges(v), g.out_edges(w))
		{untested();
		}
		shared_adj_iter(adjacency_iterator v, adjacency_iterator ve,
		                vertex_descriptor w, G const& g)
		    : parent(v.base(), ve.base(), g.out_edges(w))
		{
		}
	};
} // detail

VCTtemplate
bool check_twins(typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
                 typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor w,
                 const gala::graph<SGARGS>& g)
{ itested();
    return(g.out_edges(v) == g.out_edges(w));
}

VCTtemplate
inline typename boost::graph_traits<gala::graph<SGARGS> >::vertices_size_type
   get_pos(typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
			const gala::graph<SGARGS>& G)
{ itested();
    return boost::get(boost::vertex_index, G, v);
}

} // treedec


// should not be necessary.
// remove. and remove Vertex_NF/Edge_NF declarations.
namespace treedec{
VCTtemplate
struct graph_traits<gala::graph<SGARGS> >{
	using G_t = typename gala::graph<SGARGS>;

	typedef typename treedec_chooser<G_t>::type treedec_type;
	typedef typename outedge_set<G_t>::type outedge_set_type;
	typedef typename boost::adjacency_list<boost::vecS, boost::vecS,
	    boost::bidirectionalS, Vertex_NF, Edge_NF> directed_overlay;

	typedef typename boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> immutable_type;
	//                                 for now. ^^^^^
	typedef typename gala::graph<SGARGS>::directed_type directed_type;
};
} // treedec

#endif

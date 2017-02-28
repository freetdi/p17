// Felix Salfelder, 2016
//
// (c) 2016 Felix Salfelder
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
// a simple graph with edge labels overlay for an immutable graph.
//
//
#ifndef G_IMMUTABLE_H
#define G_IMMUTABLE_H
#include <boost/graph/directed_graph.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <tdlib/graph_traits.hpp> // hmmm

// almost sure...
#define GETPOS(a,b) boost::get(boost::vertex_index,b,a)

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

#if 0
class mybool{ //
public:
	mybool() : value_(bool())
	{ untested();
	}
	/* explicit */ mybool(bool const& t): value_(t) {}
	/* explicit */ operator bool&() { return value_; }
	/* explicit */ operator bool() const { return value_; }
private:
	bool value_;
};
#endif

// inspired by boost::container::flat_set
// std::find on random access iterator should be fine...
namespace boost_dissect{ //
   template <class RanIt, class key_type, typename size_type=size_t>
   inline RanIt priv_lower_bound(RanIt first, const RanIt last,
                          const key_type & key)
   {
      size_type len = (last - first);
      RanIt middle;

      while (len) {
         size_type step = len >> 1;
         middle = first;
         middle += step;

         if (*middle < key) {
            first = ++middle;
            len -= step + 1;
         }else{
            len = step;
         }
      }
      return first;
   }

	template <class RanIt, class key_type>
	inline RanIt find(RanIt first, const RanIt last,
			const key_type& k)
	{
#ifndef NDEBUG
		RanIt a = first;
		while(true){
			if(a==last){ untested();
			  	break;
			}else{
			}
			auto p=*a;
			++a;
			if(a==last){
			  	break;
			}
			assert(p < *a);
		}
#endif

		RanIt i = priv_lower_bound(first, last, k);
		if (i != last && k < *i){
			i = last;
		}else{
		}
		return i;
	}
} // boost_dissect

// hmm, template argument only needed for construction/assignment?
template<class G>
class immvecgraph{ //
public: // types
	typedef typename boost::graph_traits<G>::vertices_size_type vertex_descriptor;
	typedef typename boost::graph_traits<G>::vertices_size_type vertex_index_t;
	typedef typename boost::graph_traits<G>::vertices_size_type vertices_size_type;
	typedef typename std::vector<vertex_descriptor> edgelist;
	typedef typename edgelist::const_iterator internal_out_edge_iterator;
	typedef internal_out_edge_iterator adjacency_iterator;
	typedef boost::counting_iterator<vertex_index_t> vertex_iterator;
	typedef std::pair<vertex_descriptor, internal_out_edge_iterator> edge_descriptor;

	typedef void directed_category;
	typedef void edge_parallel_category;
	typedef void traversal_category;

	class edge_iterator
      : public boost::iterator_facade<edge_iterator,
                                      edge_descriptor,
                                      boost::forward_traversal_tag,
                                      edge_descriptor> {
		public: // types
			typedef edge_descriptor value_type;
//       typedef edge_descriptor* difference_type;
//       typedef edge_descriptor reference;
//       typedef edge_descriptor* pointer;
//       typedef std::input_iterator_tag iterator_category;
	public: // cons
		edge_iterator()
		{
		}
		edge_iterator(vertex_descriptor v, immvecgraph const& g)
		    : _s(v), _t(g._vertices[v]), _g(&g)
		{
			skip();
		}
		edge_iterator(vertex_descriptor v, internal_out_edge_iterator w, immvecgraph const& g)
		    : _s(v), _t(w), _g(&g)
		{
		}
	public: // op
		bool operator==(edge_iterator const& p) const
		{ untested();
			assert( _s!=p._s || p._t==_t);
			return p._t==_t;
		}
		bool operator!=(edge_iterator const& p) const
		{
			assert( _s!=p._s || p._t==_t);
			return p._t!=_t;
		}
		void increment()
		{
			++_t;
			skip();
		}
		void skip()
		{
			while( _t != _g->_vertices.back()){

				if(*_t>_s){
					++_s;
					_t = _g->_vertices[_s];
					if(_t == _g->_vertices.back()){ untested();
						break;
					}else{
					}
				}else if(_g->_vertices[_s+1] == _t){
					++_s;
				}else{
					break;
				}
			}
		}
		value_type operator*() const
		{
			return std::make_pair(_s, _t);
		}
		vertex_descriptor source() const
		{ untested();
			return _s;
		}
		vertex_descriptor target() const
		{ untested();
			return *_t;
		}
	private:
		vertex_descriptor _s;
		internal_out_edge_iterator _t;
		immvecgraph const* _g;
	};

	class out_edge_iterator
      : public boost::iterator_facade<out_edge_iterator,
                                      edge_descriptor,
                                      boost::forward_traversal_tag,
                                      const edge_descriptor&> { //
	public: // cons
		out_edge_iterator()
		{
		}
		out_edge_iterator(vertex_descriptor v, immvecgraph const& g)
		    : _s(v), _t(g._vertices[v])
		{ untested();
		}
		out_edge_iterator(vertex_descriptor v, internal_out_edge_iterator w)
		    : _s(v), _t(w)
		{
		}
	public: // op
		bool operator==(out_edge_iterator const& p) const
		{ untested();
			assert(_s==p._s);
			return p._t == _t;
		}
		bool operator!=(out_edge_iterator const& p) const
		{
			assert(_s==p._s);
			return p._t != _t;
		}
		void increment()
		{
			++_t;
		}
		edge_descriptor operator*() const
		{
			return std::make_pair(_s, _t);
		}
	private:
		vertex_descriptor _s;
		internal_out_edge_iterator _t;
	};
public: // construct
	immvecgraph(const G& g) : _g(&g)
	{
	}
 	immvecgraph() : _g(NULL)
 	{ untested();
 	}
	template<class S>
	immvecgraph(const G& g, std::vector<bool> const & disabled,
			vertex_index_t num_dis,
			std::vector<typename boost::graph_traits<G>::vertex_descriptor> &idxMap,
			S const& SRC, S const& SINK)
		: _vertices(boost::num_vertices(g)+1),
		  _edges(NULL), _g(&g)
	{ //
#ifndef NDEBUG
		unsigned c=0;
		for(auto x : disabled){ untested();
			if(x) ++c;
		}
		assert(num_dis==c);
#endif
		assign(g, disabled, num_dis, idxMap, SRC, SINK);
	}
public: // assign
	immvecgraph& operator=(immvecgraph&& p)
	{ untested();
		assert(p._g == _g || !_g);
		_vertices = std::move(p._vertices);
		_edges = std::move(p._edges);
		return *this;
	}

	template<class S>
	void assign(const G& g, std::vector<bool> const & disabled,
			vertex_index_t num_dis,
			std::vector<typename boost::graph_traits<G>::vertex_descriptor> &idxMap,
			S const& SRC, S const& SNK);

	std::pair<edge_iterator, edge_iterator> edges() const
	{
		edge_iterator begin(0, *this);
		edge_iterator end(num_vertices(), _vertices.back(), *this);
		return std::make_pair(begin, end);
	}
	std::pair<out_edge_iterator, out_edge_iterator>
	out_edges(vertex_descriptor v) const
	{
		assert(unsigned(v+1)<_vertices.size());
		assert(_vertices[v] <= _vertices[v+1]);
		out_edge_iterator begin(v, _vertices[v]);
		out_edge_iterator end(v, _vertices[v+1]);
		return std::make_pair(begin, end);
	}
public:
	void clear()
	{
		_vertices.resize(0);
		_edges.resize(0);
	}
	void reserve_edges(size_t t)
	{
		_edges.reserve(t);
	}

	std::pair<internal_out_edge_iterator, internal_out_edge_iterator>
	internal_out_edges(vertex_descriptor v) const
	{
		assert(unsigned(v+1)<_vertices.size());
		return std::make_pair(_vertices[v], _vertices[v+1]);
	}
	std::pair<vertex_iterator, vertex_iterator> vertices() const
	{
		assert(_vertices.size());
		return std::make_pair(vertex_iterator(0), vertex_iterator(_vertices.size()-1));
	}
	std::pair<edge_descriptor, bool> edge(vertex_descriptor s,
	                                      vertex_descriptor t) const
	{
		auto range=internal_out_edges(s);
		auto f=boost_dissect::find(range.first, range.second, t);
		if(f==range.second){
			return std::make_pair(edge_descriptor(), false);
		}else{
			edge_descriptor e(s,f);
			return std::make_pair(e, true);
		}
	}
public: // boost interface
	unsigned num_edges() const
	{
		return _edges.size() / 2;
	}
	unsigned degree(vertex_descriptor v) const
	{
		auto range=internal_out_edges(v);
		return range.second-range.first;
	}
	std::pair<adjacency_iterator, adjacency_iterator>
		adjacent_vertices(vertex_descriptor v) const
	{
		auto range=internal_out_edges(v);
		return range;
	}
	unsigned num_vertices() const
	{
		if(_vertices.size()){
			return _vertices.size()-1;
		}else{
			return 0;
		}
	}
	unsigned add_vertex()
	{
		_vertices.push_back(_edges.end());
		return _vertices.size()-1;
	}
public: // backend hacks.
	void push_edge(vertex_descriptor v)
	{
		_edges.push_back(v);
	}
private: // implementation
	unsigned edge_pos(internal_out_edge_iterator e) const
	{ untested();
		unsigned p = std::distance(_edges.begin(), e);
		return p;
	}
	unsigned edge_pos(edge_descriptor e) const
	{ untested();
		return edge_pos(e.second);
	}
private: // data
#ifndef NDEBUG
public:
#endif
	typename std::vector<typename edgelist::const_iterator> _vertices;
	edgelist _edges;
	// G const& _g;
	G const* _g;

	// map vertices in _g to nodes here.
	std::vector<vertex_descriptor> _idxInverseMap;
};

namespace boost {

	template<class G>
	struct graph_traits<immvecgraph<G> >{
		typedef typename immvecgraph<G>::vertex_descriptor vertex_descriptor;
		typedef typename immvecgraph<G>::adjacency_iterator adjacency_iterator;
		typedef typename immvecgraph<G>::edge_iterator edge_iterator;
		typedef typename immvecgraph<G>::out_edge_iterator out_edge_iterator;
		typedef boost::counting_iterator<vertex_descriptor> vertex_iterator;
		typedef typename immvecgraph<G>::edge_descriptor edge_descriptor;

		typedef typename graph_traits<G>::vertices_size_type vertices_size_type;
		typedef typename graph_traits<G>::edges_size_type edges_size_type;
		typedef typename graph_traits<G>::directed_category directed_category;
		typedef typename graph_traits<G>::edge_parallel_category edge_parallel_category;
		typedef typename graph_traits<G>::traversal_category traversal_category;
		typedef typename graph_traits<G>::degree_size_type degree_size_type;
	};
	template<class G>
		unsigned /*FIXME*/
	get(boost::vertex_index_t, const immvecgraph<G>&,
			typename immvecgraph<G>::vertex_descriptor v)
	{
		return v;
	}
	template<class G>
	std::pair<typename immvecgraph<G>::adjacency_iterator,
	          typename immvecgraph<G>::adjacency_iterator>
	    adjacent_vertices(typename immvecgraph<G>::vertex_descriptor v,
	          immvecgraph<G> const& g)
	{
		return g.adjacent_vertices(v);
	}
	template<class G>
	unsigned num_edges(immvecgraph<G> const& g)
	{
		return g.num_edges();
	}
	template<class G>
	unsigned num_vertices(immvecgraph<G> const& g)
	{
		return g.num_vertices();
	}
	template<class G>
	unsigned degree(typename immvecgraph<G>::vertex_descriptor v, immvecgraph<G> const& g)
	{
		return g.degree(v);
	}
	template<class G>
	unsigned out_degree(typename immvecgraph<G>::vertex_descriptor v, immvecgraph<G> const& g)
	{
		return g.degree(v);
	}
	template<class G>
	inline std::pair<typename immvecgraph<G>::edge_iterator,
	                 typename immvecgraph<G>::edge_iterator>
	    edges(const immvecgraph<G>& g)
	{
		return g.edges();
	}
	template<class G>
	inline typename immvecgraph<G>::vertex_descriptor source(
			const typename immvecgraph<G>::edge_descriptor e,
	      immvecgraph<G> const&)
	{
		return e.first;
	}
	template<class G>
	inline typename immvecgraph<G>::vertex_descriptor target(
			const typename immvecgraph<G>::edge_descriptor e,
	      immvecgraph<G> const&)
	{
		return *e.second;
	}
	template<class G>
	inline std::pair<typename immvecgraph<G>::out_edge_iterator,
	                 typename immvecgraph<G>::out_edge_iterator>
	    out_edges(typename immvecgraph<G>::vertex_descriptor v, const immvecgraph<G>& g)
	{
		return g.out_edges(v);
	}
	template<class G>
	bool get(bool treedec::Edge_NF::*, const immvecgraph<G>& g,
	    typename immvecgraph<G>::edge_descriptor e)
	{ untested();
		return g.path(e);
	}
	template<class G>
	bool& get(bool treedec::Edge_NF::*, immvecgraph<G>& g,
	    typename immvecgraph<G>::edge_descriptor e)
	{ untested();
		return g.path(e);
	}
	template<class G>
	bool get(bool treedec::Vertex_NF::*, const immvecgraph<G>& g,
	         typename immvecgraph<G>::vertex_descriptor v)
	{ untested();
		return g.visited(v);
	}
	template<class G>
	bool& get(bool treedec::Vertex_NF::*, immvecgraph<G>& g, typename immvecgraph<G>::vertex_descriptor v)
	{ untested();
		return g.visited(v);
	}
	template<class G>
	typename immvecgraph<G>::vertex_descriptor
  	get(int treedec::Vertex_NF::*, const immvecgraph<G>& g, typename immvecgraph<G>::vertex_descriptor v)
	{ untested();
		return g.predecessor(v);
	}
	template<class G>
	typename immvecgraph<G>::vertex_descriptor&
	get(int treedec::Vertex_NF::*, immvecgraph<G>& g, typename immvecgraph<G>::vertex_descriptor v)
	{ untested();
		return g.predecessor(v);
	}
	template<class G>
	typename immvecgraph<G>::vertex_descriptor vertex(
			size_t t, immvecgraph<G> const&)
	{
		return t;
	}
	template<class G>
	std::pair<typename immvecgraph<G>::edge_descriptor, bool> edge(
			typename immvecgraph<G>::vertex_descriptor s,
			typename immvecgraph<G>::vertex_descriptor t,
			immvecgraph<G> const& g)
	{
		return g.edge(s,t);
	}
	// template<class G>
	// inline std::pair<typename immvecgraph<G>::edge_iterator, bool>
	//     edge(unsigned a, unsigned b, const immvecgraph<G>& g)
	// { incomplete();
	// }
	template<class G>
	std::pair<typename immvecgraph<G>::vertex_iterator,
	          typename immvecgraph<G>::vertex_iterator> vertices(const immvecgraph<G>& g){
		return g.vertices();
	}

	class imm_vid_map : public put_get_helper<unsigned, imm_vid_map> {
		public:
			typedef readable_property_map_tag category;
			typedef unsigned value_type;
			typedef unsigned reference;
			typedef unsigned key_type;
			imm_vid_map(){}
			template <class T>
				long operator[](T x) const { return x; }
	};
	template <class G>
	inline imm_vid_map
	get(vertex_index_t, const immvecgraph<G>&)
	{
    return imm_vid_map();
	}

} // boost

template<class G>
	template<class S>
	void immvecgraph<G>::assign(const G& g, std::vector<bool> const & disabled,
			vertex_index_t num_dis,
			std::vector<typename boost::graph_traits<G>::vertex_descriptor> &idxMap,
			S const& SRC, S const& /*SNK*/)
	{ untested();
		unsigned nv=boost::num_vertices(g)-num_dis;
		_vertices.resize(nv+1); // one extra for end.

		_idxInverseMap.resize(boost::num_vertices(g));

		idxMap.resize(nv); // hmmm
		unsigned ne=boost::num_edges(g)*2;
		_edges.resize(0);
		_edges.reserve(ne);

		BOOST_AUTO(V, boost::vertices(g));
		BOOST_AUTO(v, V.first);
		BOOST_AUTO(vend, V.second);

		unsigned vn=0;
		for(;v!=vend;++v){ untested();
			auto vpos=boost::get(boost::vertex_index, *v, *_g);
			if(disabled[vpos]){ untested();
			}else{ untested();
				idxMap[vn] = *v;
				_idxInverseMap[*v] = vn;
				_vertices[vn] = _edges.end();
				BOOST_AUTO(E, boost::adjacent_vertices(*v, g));
				BOOST_AUTO(e, E.first);
				BOOST_AUTO(eend, E.second);
				for(;e!=eend;++e){ untested();
					if(!disabled[boost::vertex_index(boost::vertex_index, *_g, *e)]){ untested();
						_edges.push_back(*e);
					}else{ untested();
					}
				}

				++vn;
			}
		}
		_vertices[vn] = _edges.end();

		for(auto& e : _edges){ untested();
			e = _idxInverseMap[e];
		}

		for(auto s : SRC){ untested();
			auto p=boost::get(boost::vertex_index, *_g, s);
			assert(p<boost::num_vertices(*_g));
			assert(!disabled[p]);
			assert(_idxInverseMap[p] < vn);

			_edges.push_back(_idxInverseMap[p]);
		}

		++vn;
		_vertices[vn] = _edges.end();

		++vn;
		_vertices[vn] = _edges.end();
		assert(vn+1==_vertices.size());

#ifndef NDEBUG
		{ untested();
			vertex_iterator i=vertices().first;
			unsigned c=0;
			for(;i!=vertices().second; ++i){ untested();
				++c;
				auto O=out_edges(*i);
				auto Oi=O.first;
				auto Oe=O.second;
				for(;Oi!=Oe;++Oi){ untested();
					assert(
							boost::edge(boost::target(*Oi, *this),
								boost::source(*Oi, *this), *this).second);
				}
			}
			assert(c+1==_vertices.size());
		}
#endif
	}

namespace treedec {

#if 0
template<typename G_t>
inline unsigned int get_pos(typename immvecgraph<G_t>::vertex_descriptor v, const immvecgraph<G_t>& G)
{
    return boost::get(boost::vertex_index, G, v);
}
#endif

namespace draft {

// immutable overlay specialization for graphs without add_edge.
template<class G_t, class I_t, class S_t, class M_t, class CB_t>
inline immvecgraph<G_t> const& immutable_clone(
     G_t const &G,
     immvecgraph<G_t>& ig,
     I_t bbegin,
     I_t bend,
     S_t bag_nv,
    //   URGHS. no default types without c++11.
     M_t* vdMap, /*=NULL*/
     CB_t* cb
     )
{
	typedef typename graph_traits<G_t>::immutable_type IG_t;
	typedef typename boost::graph_traits<IG_t>::vertex_descriptor vertex_descriptor_ig;

	assert(bbegin!=bend);
	assert(bag_nv);

	BOOST_AUTO(nv, boost::num_vertices(G));

	if(bag_nv != boost::num_vertices(ig)){
		// drop a new one... (for now?)
		// FIXME: just resize
//		ig = MOVE(IG_t(0));
		ig.clear();
	}else{ untested();
		ig.clear();
	}

	assert(ig._edges.size()==0);
	assert(ig._vertices.size()==0);
	ig.reserve_edges(bag_nv * (bag_nv-1));

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
	//
	// FIXME: don't alloc here.
	std::vector<vertex_descriptor_ig> reverse_map(nv);

	BOOST_AUTO(bi, bbegin);
	BOOST_AUTO(be, bend);
	unsigned i=0;
	auto prevpos=0; (void)prevpos;
	for(; bi!=be; ++bi){
		// FIXME: pos, vertex_index?
		assert(i < vdMap->size());
		(*vdMap)[i] = *bi;
		auto pos=GETPOS(*bi, G);
		assert(!pos || pos>prevpos);
		reverse_map[pos] = i;
		++i;
		prevpos = GETPOS(*bi, G);
	}
	assert(i==bag_nv);

	bi = bbegin;
	unsigned s=-1;
	unsigned t=-1;

	 // apparently inefficient...
	for(; bi!=be; ++bi){
		unsigned new_vertex=ig.add_vertex();
		assert(new_vertex<bag_nv);
		BOOST_AUTO(N, GETPOS(*bi, G)); (void)N;
		assert(reverse_map[N] == new_vertex);

		BOOST_AUTO(vi, bbegin);
		for(; vi!=be; ++vi){
			char edg = 0;
			if(*vi==*bi){
				continue;
				// skip self loop
			}else if(*vi<*bi){
				// egde if the inverse edge exists
				// inefficient?! yes.
				BOOST_AUTO(s, GETPOS(*vi, G)); (void) s;
				BOOST_AUTO(t, GETPOS(*bi, G)); (void) t;
				assert(s<t);
				assert(reverse_map[s] < new_vertex);
				auto rs=boost::vertex(reverse_map[s], ig);
				edg = boost::edge(rs, new_vertex, ig).second;
			}else if( boost::edge(*bi, *vi, G).second){
				edg = 2;
			}else if(!cb){
			}else if((*cb)(*bi, *vi)){ untested();
				edg = 3;
			}else{ untested();
				// no edge.
			}

			if(edg){
				BOOST_AUTO(s, GETPOS(*bi, G)); (void)s;
				BOOST_AUTO(t, GETPOS(*vi, G));
				assert(ig._vertices.size()==unsigned(reverse_map[s]+1));
				ig.push_edge(reverse_map[t]);
			}else if(s==-1u){ // ..  && t>s)
				assert(GETPOS(*bi, G)!=-1);
				s = GETPOS(*bi, G);
				t = GETPOS(*vi, G);
				assert(s!=t);
			}else{
			}
		}
	}
	ig.add_vertex(); // actually not adding vertex, just end.
	assert(ig.num_vertices()); // not sure what happens with empty bags...
	assert(ig.num_vertices() == bag_nv);

#ifndef NDEBUG
		{
			auto i = ig.vertices().first;
			unsigned c=0;
			for(;i!=ig.vertices().second; ++i){
				++c;
				auto O=ig.out_edges(*i);
				auto Oi=O.first;
				auto Oe=O.second;
				for(;Oi!=Oe;++Oi){
					assert(
							boost::edge(boost::target(*Oi, ig),
								boost::source(*Oi, ig), ig).second);
				}
			}
			assert(c+1==ig._vertices.size());
		}
#endif

		if(cb && s!=-1u){ untested();
			cb->a = reverse_map[s];
			cb->b = reverse_map[t];
		}else{
			// assert(is_clique(ig));
		}

	return ig;
}

} // draft
} // treedec

#undef GETPOS

#endif // guard

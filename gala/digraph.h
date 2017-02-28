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
#ifndef DIGRAPH_H
#define DIGRAPH_H

#include <boost/graph/graph_traits.hpp>

#ifndef HAVE_MYBOOL
#define HAVE_MYBOOL
class mybool{ //
public:
	mybool() : value_(bool())
	{ untested();
	}
	/* explicit */ mybool(bool const& t): value_(t) {}
	// /* explicit */ operator bool&() { return value_; }
	/* explicit */ operator bool() const { return value_; }
private:
	char value_;
};
#endif

// a digraph with edge labels overlay for an immutable graph.
//
//
#include <boost/graph/directed_graph.hpp>
#include <boost/iterator/counting_iterator.hpp>

// #include <tdlib/graph.hpp>

#include "immutable.h"

// hmm. implement graph with edge_id?
// better: generic edge properties...
template<class G>
class idgwel{ //
public: // types
	typedef typename boost::graph_traits<G>::vertex_descriptor vertex_descriptor;
	typedef typename boost::graph_traits<G>::vertices_size_type vertices_size_type;
	typedef typename boost::graph_traits<G>::vertex_descriptor vertex_index_t;
	typedef typename std::vector<vertex_descriptor> edgelist;
	typedef typename edgelist::const_iterator internal_out_edge_iterator;
	typedef boost::counting_iterator<vertex_index_t> vertex_iterator;

	typedef std::pair<vertex_descriptor, internal_out_edge_iterator> edge_descriptor;
	class out_edge_iterator{ //
	public: // cons
		out_edge_iterator()
		{ untested();
		}
		out_edge_iterator(vertex_descriptor v, idgwel const& g)
		    : _s(v), _t(g._vertices[v])
		{untested();
		}
		out_edge_iterator(vertex_descriptor v, internal_out_edge_iterator w)
		    : _s(v), _t(w)
		{ itested();
		}
	public: // op
		bool operator==(out_edge_iterator const& p) const
		{ untested();
			assert(_s==p._s);
			return p._t == _t;
		}
		bool operator!=(out_edge_iterator const& p) const
		{ itested();
			assert(_s==p._s);
			return p._t != _t;
		}
		out_edge_iterator& operator++()
		{ untested();
			++_t;
			return *this;
		}
		edge_descriptor operator*() const
		{ itested();
			return std::make_pair(_s, _t);
		}
	private:
		vertex_descriptor _s;
		internal_out_edge_iterator _t;
	};
public: // construct
	idgwel(const G& g) : _g(&g)
	{untested();
	}
 	idgwel() : _g(NULL)
 	{ untested();
 	}
	template<class S>
	idgwel(const G& g, std::vector<bool> const & disabled,
			vertex_index_t num_dis,
			std::vector<typename boost::graph_traits<G>::vertex_descriptor> &idxMap,
			S const& SRC, S const& SINK)
		: _vertices(boost::num_vertices(g)+1),
		  _visited(boost::num_vertices(g)-num_dis),
		  _predecessor(boost::num_vertices(g)-num_dis),
		  _edges(0), _path(0),
		  _g(&g)
	{ untested();
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
	idgwel& operator=(idgwel&& p)
	{ untested();
		assert(p._g == _g || !_g);
		_vertices = std::move(p._vertices);
		_edges = std::move(p._edges);
		_path = std::move(p._path);
		_predecessor = std::move(p._predecessor);
		_visited = std::move(p._visited);
		return *this;
	}

	template<class S>
	void assign(const G& g, std::vector<bool> const & disabled,
			vertex_index_t num_dis,
			std::vector<typename boost::graph_traits<G>::vertex_descriptor> &idxMap,
			S const& SRC, S const& SNK);
	std::pair<out_edge_iterator, out_edge_iterator>
	out_edges(vertex_descriptor v) const
	{ itested();
		//std::cerr << "v" << v;
		assert(unsigned(v+1)<_vertices.size());
		assert(_vertices[v] <= _vertices[v+1]);
		out_edge_iterator begin(v, _vertices[v]);
		out_edge_iterator end(v, _vertices[v+1]);
		return std::make_pair(begin, end);
	}
	std::pair<internal_out_edge_iterator, internal_out_edge_iterator>
	internal_out_edges(vertex_descriptor v) const
	{ untested();
		assert(unsigned(v+1)<_vertices.size());
		return std::make_pair(_vertices[v], _vertices[v+1]);
	}
	std::pair<vertex_iterator, vertex_iterator> vertices() const
	{ untested();
		assert(_vertices.size());
		return std::make_pair(vertex_iterator(0), vertex_iterator(_vertices.size()-1));
	}
	bool& path(edge_descriptor e)
	{ untested();
		assert(edge_pos(e)<_path.size());
		return _path[edge_pos(e)];
	}
	bool path(edge_descriptor e) const
	{ untested();
		return _path[edge_pos(e)];
	}
	bool& visited(vertex_descriptor v)
	{ untested();
		return _visited[v];
	}
	bool visited(vertex_descriptor v) const
	{ untested();
		return _visited[v];
	}
	vertex_descriptor& predecessor(vertex_descriptor v)
	{ untested();
		return _predecessor[v];
	}
	vertex_descriptor predecessor(vertex_descriptor v) const
	{ untested();
		return _predecessor[v];
	}
	vertex_descriptor source() const
	{ untested();
		return _vertices.size()-3;
	}
	vertex_descriptor sink() const
	{ untested();
		return _vertices.size()-2;
	}

	std::pair<edge_descriptor, bool> edge(vertex_descriptor s,
	                                      vertex_descriptor t) const
	{ untested();
		auto range=internal_out_edges(s);
		auto f=boost_dissect::find(range.first, range.second, t);
		if(f==range.second){ untested();
			return std::make_pair(edge_descriptor(), false);
		}else{ untested();
			edge_descriptor e(s,f);
			assert(edge_pos(e) < _path.size());
			return std::make_pair(e, true);
		}
	}
private: // implementation
	unsigned edge_pos(internal_out_edge_iterator e) const
	{ untested();
		unsigned p = std::distance(_edges.begin(), e);
		assert(p<_path.size());
		return p;
	}
	unsigned edge_pos(edge_descriptor e) const
	{ untested();
		return edge_pos(e.second);
	}
private: // data
	typename std::vector<typename edgelist::const_iterator> _vertices;
	typename std::vector<mybool> _visited;
	typename std::vector<vertex_descriptor> _predecessor;

	edgelist _edges;
	typename std::vector<mybool> _path;

	// G const& _g;
	G const* _g;

	// map vertices in _g to nodes here.
	std::vector<vertex_descriptor> _idxInverseMap;
};

namespace boost{ //
	template<class G>
	struct graph_traits<idgwel<G> >{ //
		typedef typename idgwel<G>::vertex_descriptor vertex_descriptor;
		typedef void* adjacency_iterator;
		typedef typename idgwel<G>::out_edge_iterator out_edge_iterator;
		typedef boost::counting_iterator<vertex_descriptor> vertex_iterator;
	};

	template<class G>
	inline std::pair<
    typename idgwel<G>::out_edge_iterator,
    typename idgwel<G>::out_edge_iterator>
		 out_edges(typename idgwel<G>::vertex_descriptor v, const idgwel<G>& g)
	 { untested();
		 return g.out_edges(v);
	 }

	template<class G>
	inline typename idgwel<G>::vertex_descriptor
	   source(typename idgwel<G>::edge_descriptor e, const idgwel<G>&)
	{ itested();
		return e.first;
	}
	template<class G>
	inline typename idgwel<G>::vertex_descriptor
	   target(typename idgwel<G>::edge_descriptor e, const idgwel<G>&)
	{ itested();
		return *(e.second);
	}

	template<class G>
	bool get(bool treedec::Edge_NF::*, const idgwel<G>& g,
			typename idgwel<G>::edge_descriptor e)
	{ untested();
		return g.path(e);
	}
	template<class G>
	bool& get(bool treedec::Edge_NF::*, idgwel<G>& g,
			typename idgwel<G>::edge_descriptor e)
	{ untested();
		return g.path(e);
	}
	template<class G>
	bool get(bool treedec::Vertex_NF::*, const idgwel<G>& g, typename idgwel<G>::vertex_descriptor v)
	{ untested();
		return g.visited(v);
	}
	template<class G>
	bool& get(bool treedec::Vertex_NF::*, idgwel<G>& g, typename idgwel<G>::vertex_descriptor v)
	{ untested();
		return g.visited(v);
	}
	template<class G>
	typename idgwel<G>::vertex_descriptor
  	get(int treedec::Vertex_NF::*, const idgwel<G>& g, typename idgwel<G>::vertex_descriptor v)
	{ untested();
		return g.predecessor(v);
	}
	template<class G>
	typename idgwel<G>::vertex_descriptor&
	get(int treedec::Vertex_NF::*, idgwel<G>& g, typename idgwel<G>::vertex_descriptor v)
	{ untested();
		return g.predecessor(v);
	}
	template<class G>
	std::pair<typename idgwel<G>::edge_descriptor, bool> edge(
			typename idgwel<G>::vertex_descriptor s,
			typename idgwel<G>::vertex_descriptor t, idgwel<G> const& g)
	{ untested();
		return g.edge(s,t);
	}
	template<class G>
	std::pair<typename idgwel<G>::vertex_iterator,
	          typename idgwel<G>::vertex_iterator> vertices(const idgwel<G>& g){ untested();
		return g.vertices();
	}
} // boost


template<class G>
	template<class S>
	void idgwel<G>::assign(const G& g, std::vector<bool> const & disabled,
			vertex_index_t num_dis,
			std::vector<typename boost::graph_traits<G>::vertex_descriptor> &idxMap,
			S const& SRC, S const& SNK)
	{ untested();
		// 0, 1 .... SRC SNK
		vertex_descriptor vsource=boost::num_vertices(g)-num_dis; (void)vsource;
		vertex_descriptor vsink=boost::num_vertices(g)-num_dis+1;
		unsigned nv=boost::num_vertices(g)-num_dis+2;
		_vertices.resize(nv+1); // one extra for end.

		_idxInverseMap.resize(boost::num_vertices(g)+1); // one extra for sink
		vertices_size_type tmpsink=boost::num_vertices(g);
		_idxInverseMap.back() = vsink;
		assert(_idxInverseMap[tmpsink] == vsink);

		idxMap.resize(nv); // hmmm
		_visited.resize(nv);
		_predecessor.resize(nv);
		unsigned ne=boost::num_edges(g)*2 + SRC.size() + SNK.size();
		_edges.resize(0);
		_edges.reserve(ne);

		_path.resize(0);
		_path.reserve(ne);

		BOOST_AUTO(V, boost::vertices(g));
		BOOST_AUTO(v, V.first);
		BOOST_AUTO(vend, V.second);

		unsigned vn=0;
		for(;v!=vend;++v){ untested();
			auto vpos=boost::get(boost::vertex_index, *_g, *v);
			if(disabled[vpos]){ untested();
			}else{ untested();
				idxMap[vn] = *v;
				_idxInverseMap[*v] = vn;
				_vertices[vn] = _edges.end();
				_visited[vn] = false;
				_predecessor[vn] = vn;
				BOOST_AUTO(E, boost::adjacent_vertices(*v, g));
				BOOST_AUTO(e, E.first);
				BOOST_AUTO(eend, E.second);
				for(;e!=eend;++e){ itested();
					if(!disabled[boost::get(boost::vertex_index, *_g, *v)]){ untested();
						//std::cerr << "push " << vn << "(" << *v << ")" << " -> " << *e << "\n";
						_edges.push_back(*e); // later: translate to diG vertex
						_path.push_back(false);
					}else{ untested();
					}
				}

				// connect to sink vertex if it is in SNK.
				// inefficient!
				if(SNK.find(*v)!=SNK.end()){ untested();
						_edges.push_back(tmpsink);
						//std::cerr << "pushsink " << vn << "(" << *v << ")" << " -> " << tmpsink << "\n";
						_path.push_back(false);
				}else{ untested();
				}
				++vn;
			}
		}
		_vertices[vn] = _edges.end();
		_visited[vn] = false;
		_predecessor[vn] = vn;
		assert(vn==vsource);
		assert(vn==source());

		for(auto& e : _edges){ untested();
			e = _idxInverseMap[e];
		}

		for(auto s : SRC){ untested();
			auto p=boost::get(boost::vertex_index, *_g, s);
			assert(p<boost::num_vertices(*_g));
			assert(!disabled[p]);
			assert(_idxInverseMap[p] < vn);

			_edges.push_back(_idxInverseMap[p]);
			_path.push_back(false);
		}

		++vn;
		assert(vn==vsink);
		assert(vn==sink());
		_vertices[vn] = _edges.end();
		_visited[vn] = false;
		_predecessor[vn] = vn;

		++vn;
		_vertices[vn] = _edges.end();
		assert(vn+1==_vertices.size());


#ifndef NDEBUG
		{ untested();
			vertex_iterator i=vertices().first;
			unsigned c=0;
			for(;i!=vertices().second; ++i){ untested();
				//std::cerr << *i << ": ";
				++c;
				auto O=out_edges(*i);
				auto Oi=O.first;
				auto Oe=O.second;
				for(;Oi!=Oe;++Oi){ untested();
					//std::cerr << boost::source(*Oi, *this) << " ";
					//std::cerr << boost::target(*Oi, *this) << " ";
					assert(boost::source(*Oi, *this) != sink());
					assert(boost::target(*Oi, *this) != source());
					assert(boost::source(*Oi, *this) == *i);
					assert(boost::edge(boost::source(*Oi, *this),
								            boost::target(*Oi, *this), *this).second);
					assert( *i == source()
							|| boost::target(*Oi, *this) == sink()
							|| boost::edge(boost::target(*Oi, *this),
								            boost::source(*Oi, *this), *this).second);
				}
				//std::cerr << "\n";
			}
			assert(c+1==_vertices.size());
		}
#endif
	}

#endif // guard

#pragma once

#include <boost/graph/graph_traits.hpp>
// inspired by boost::mindegree
// (but more graph-centric)

namespace treedec{

namespace draft{

template<class G_t>
class NUMBERING_1 {
public:
    typedef typename boost::property_map<G_t, boost::vertex_index_t>::type idmap_type;
    typedef boost::graph_traits<G_t> GraphTraits;
	 typedef typename GraphTraits::vertices_size_type value_type;
	 typedef typename GraphTraits::vertex_descriptor vertex_descriptor;
	 typedef std::vector<value_type> container_type;
private: // don't use.
	 NUMBERING_1(){ unreachable(); }
	 NUMBERING_1( const NUMBERING_1&){ unreachable(); }
public:
	NUMBERING_1(G_t const& g)
		: _current(0), _data(boost::num_vertices(g)), _idmap(idmap_type() ) {
			increment();
		}
#if 0 // later?
	Numbering(G_t const& g, idmap_type i)
		: _current(1), _data(boost::num_vertices(g)), _idmap(idmap_type() ) {}
#endif

	void put(vertex_descriptor v) { _data[get(_idmap, v)] = _current; }
	void increment(value_type i = 1) { _current -= i; }
	bool is_numbered(vertex_descriptor v) const {
		return _data[get(_idmap, v)] != 0;
	}
	bool is_not_numbered(vertex_descriptor v) const {
		return !is_numbered(v);
	}
	bool is_before(vertex_descriptor v, vertex_descriptor w) const {
		return _data[get(_idmap, v)] > _data[get(_idmap, w)];
	}
private:
	value_type _current;
	container_type _data;
	idmap_type _idmap;
};

} // draft

} // treedec

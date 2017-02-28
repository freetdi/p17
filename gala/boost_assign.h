

// FIX: only use include traits, not adj list!
#include <boost/graph/graph_traits.hpp>
// does not work if vertex_descriptor is not contiguous
// should be obsolete with boost::copy_graph
VCTtemplate template<class H>
void gala::graph<SGARGS>::assign(H const& g)
{ untested();
//	size_t nv = boost::graph_traits<H>::num_vertices(g);
	typedef gala::graph<SGARGS> G;
	size_t nv = boost::num_vertices(g);
	size_t ne = boost::num_edges(g);

	typedef typename boost::graph_traits<gala::graph<SGARGS > >::vertices_size_type st;
	size_t L = std::numeric_limits<st>::max();
	assert(boost::num_vertices(g) <= L); (void) L;

	if(_v.size()){ untested();
		_v.resize(0);
	}else{ untested();
	}
	_v.resize(nv);
	vertex_type map[nv];
	size_t i=0;
	_num_edges = 0;

	// not necessary for vector...
//	vertex_iterator v=begin();
	// for(unsigned i=0; i<nv; ++i){ //  v=begin(); v!=end(); ++v) //}
	for(iterator v=begin(); v!=end(); ++v) { untested();
		// for(auto& v : vertices())
		map[i] = G::iter::deref(v);
		++i;
	}
#ifndef NDEBUG
 	if(i!=nv){ untested();
		std::cerr << "oops2 " << i << " " << nv << "\n";
	}
	assert(i==nv);
#endif

	typename boost::graph_traits<H>::edge_iterator eIt, eEnd;
	for(boost::tie(eIt, eEnd) = boost::edges(g); eIt != eEnd; ++eIt){ // untested();
		if(boost::source(*eIt, g) >= num_vertices()){ untested();
			assert(false);
			// unreachable?
		}else if(boost::target(*eIt, g) >= num_vertices()) { untested();
			assert(false);
			// unreachable?
		}else{ untested();
// -Wno-uninitialized!?
			add_edge(map[boost::source(*eIt, g)],
						map[boost::target(*eIt, g)]);
		}
		// ++_num_edges;
	}
	if(_num_edges == 2*ne){ untested();
		_num_edges = ne; // ??1
	}else if(_num_edges < ne){ untested();
//		possibly assigning directed graph.
//		getting fewer edges
		assert(_num_edges<ne);
	}else{ untested();
		assert(_num_edges == ne);
	}
	assert(num_vertices() == nv);


#if 0 // not yet
	for(vertex_& i : vertices()){ untested();
		assert(is_valid(&i));
		for(auto& j : vertices()){ untested();
			assert(is_edge(&j,&i) == is_edge(&i,&j));
		}
	}
#endif
}

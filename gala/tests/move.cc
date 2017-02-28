
#include <assert.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/adjacency_matrix.hpp>
#include "../boost.h"
#include <boost/graph/graph_traits.hpp>

template<class G>
struct dvv_config : public gala::graph_cfg_default<G> {
	 typedef boost::mpl::true_ is_directed_t;
};

using namespace std;

int main(int , char* [])
{
	using namespace boost;

	gala::graph<> g;

	add_vertex(g);
	add_vertex(g);
	auto V = vertices(g);
	auto next=V.first; ++next;
	bool added = add_edge(*V.first, *next, g).second;
	assert(added);
	assert(degree(*V.first, g)==1);

	std::cerr << "force rewire 1st edg " << *V.first;
	for(unsigned i=0; i<100; ++i){
		add_vertex(g);
	}
	V = vertices(g);
	assert(degree(*V.first, g)==1);
	trace2("", *adjacent_vertices(*V.first, g).first, *V.first);

	V = vertices(g);
	next = V.first; ++next;
	std::cerr << "before move " << *next << " "
		       << *adjacent_vertices(*V.first, g).first << "\n";
	assert(*adjacent_vertices(*V.first, g).first == *next);

	assert(num_vertices(g)==102);
	next = V.first;
	add_edge(*V.first, *(++next), g);
	add_edge(*(++V.first), *(++next), g);
	add_edge(*(++V.first), *(++next), g);

	std::cerr << "force rewire " << *V.first;
	for(unsigned i=0; i<100; ++i){
		if (i%5) std::cerr << ".";
		add_vertex(g);
	}
	std::cerr << "\n";
	V = vertices(g);
	std::cerr << "after rewire " << *V.first;

	gala::graph<> h(std::move(g));

	V = vertices(h);
	std::cerr << "after move " << *V.first;
	next = V.first;
	added = add_edge(*V.first, *(++next), h).second;
	assert(!added);
	added = add_edge(*V.first, *(++next), h).second;
	assert(added);

	g = std::move(h);
	V = vertices(g);
	std::cerr << "moved back " << *V.first;

}


#include <assert.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/adjacency_matrix.hpp>
#include "../boost.h"
#include <boost/graph/graph_traits.hpp>

template<class G>
struct dvv_config : public gala::graph_cfg_default<G> {
	 typedef boost::mpl::true_ is_directed_t;
};

typedef gala::graph<> svbs;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t> SSG_16i;
typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::directedS> BALD_t;


using namespace std;

int main(int , char* [])
{
	using namespace boost;

	svbs G(5);
	std::vector<svbs::vertex_type> v(5);

	unsigned i=0;
	for( auto& vv: G ){
		v[i++] = &vv;
	}

	G.add_edge(v[0], v[1]);
	G.add_edge(v[1], v[0]); // dup
	G.add_edge(v[1], v[2]);
	G.add_edge(v[1], v[4]);
	G.add_edge(v[4], v[2]);
	G.add_edge(v[1], v[2]); // dup
	G.add_edge(v[2], v[0]);
	trace1("G", (G.num_edges()));

	assert(G.num_edges()==5);
	assert(G.directed_view().num_edges()==10);
	assert(G.directed_view().directed_view().num_edges()==10);
}


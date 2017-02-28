
#include <assert.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/adjacency_matrix.hpp>
#include "../boost.h"
#include "../boost_copy.h"
#include <boost/graph/graph_traits.hpp>

template<class G>
struct dvv_config : public gala::graph_cfg_default<G> {
	static constexpr bool is_directed=true;
};

typedef gala::graph<std::vector, std::vector, uint16_t, dvv_config> sg_dvv16;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t> SSG_16i;
typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::directedS> BALD_t;


using namespace std;

int main(int , char* [])
{
	using namespace boost;

	BALD_t G;

	add_vertex(G);
	add_vertex(G);
	add_vertex(G);
	add_vertex(G);

	add_edge(0, 1, G);
	add_edge(1, 0, G);
	add_edge(1, 2, G);

	auto E=boost::edges(G);
	sg_dvv16 G2(5);

	add_edge(0, 1, G2);
	add_edge(1, 0, G2);
	add_edge(1, 2, G2);
	add_edge(2, 0, G2);
	add_edge(4, 0, G2);
	assert(boost::num_vertices(G2)==5);
	assert(boost::num_edges(G2)==5);

	sg_dvv16 G2b;
	assert(G2.is_directed());

	sg_dvu G3(G2);
	assert(!G3.is_directed());
	assert(boost::num_edges(G3)==4);
	assert(boost::num_vertices(G3)==5);

	boost::copy_graph(G, G2);
	assert(boost::num_vertices(G)==boost::num_vertices(G2));
	assert(boost::num_edges(G)==boost::num_edges(G2));
	boost::copy_graph(G2, G2b); // uses copy constructor
//	boost::copy_graph(std::move(G2), G2b); // does not "work"
//
//
	assert(boost::num_edges(G3)==4);
	{
		boost::copy_graph(G3, G2);

		assert(boost::num_vertices(G3)==boost::num_vertices(G2));
		assert(boost::num_edges(G3)*2==boost::num_edges(G2));

		assert(boost::num_vertices(G3)==5);
	}
	{
		boost::copy_graph(G3,G2);
		assert(boost::num_edges(G2)==8);
		assert(boost::num_vertices(G3)==boost::num_vertices(G2));
	}
}


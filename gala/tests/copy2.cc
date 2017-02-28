
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
typedef gala::graph<std::vector, std::vector, uint32_t, dvv_config> sg_dvv32;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t> SSG_16i;
typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::directedS> BALD_t;
typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS> BALUVEC_t;
typedef boost::adjacency_list <boost::setS, boost::vecS, boost::undirectedS> BALU_t;


using namespace std;

int main(int , char* [])
{
	using namespace boost;

	BALD_t GD;
	BALU_t GU;

	sg_dvv16 G2(5);

	add_edge(0, 1, G2);
	add_edge(1, 0, G2);
	add_edge(1, 2, G2);
	add_edge(2, 0, G2);
	assert(boost::num_vertices(G2)==5);
	assert(boost::num_edges(G2)==4);

	boost::copy_graph(G2, GD);

	assert(boost::num_vertices(GD)==5);
	assert(boost::num_edges(GD)==4);

	boost::copy_graph(G2, GU);

	assert(boost::num_vertices(GU)==5);
	assert(boost::num_edges(GU)==3);

	sg_dvu GU2;

	boost::copy_graph(G2, GU2);

	assert(boost::num_vertices(GU2)==5);
	assert(boost::num_edges(GU2)==3);

	{
		BALUVEC_t GU3;
		boost::copy_graph(G2, GU3);

		add_edge(2, 0, G2);
		add_edge(2, 0, G2);
		add_edge(2, 0, G2);
		add_edge(2, 0, G2);
		assert(boost::num_vertices(GU3)==5);
		assert(boost::num_edges(GU3)==4); // double edge 1-0 and 0-1. no multiple 2-0...

	}
	{
		sg_dvv16 Gsame(G2);
		assert(boost::num_vertices(Gsame)==5);
		assert(boost::num_edges(Gsame)==4);
	}
	{
		sg_dvv32 Gsimilar(G2);
		assert(boost::num_vertices(Gsimilar)==5);
		assert(boost::num_edges(Gsimilar)==4);
	}

}

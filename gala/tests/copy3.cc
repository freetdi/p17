
#include <assert.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/adjacency_matrix.hpp>
#include "../boost.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/copy.hpp>
#include "../boost_copy.h"
#include <gala/examples/ssg16i.h>

template<class G>
struct dvv_config : public gala::graph_cfg_default<G> {
	static constexpr bool is_directed=true;
};

typedef gala::graph<std::vector, std::vector, uint16_t, dvv_config> sg_dvv16;
typedef gala::graph<std::vector, std::vector, uint32_t, dvv_config> sg_dvv32;
typedef gala::graph<std::set, std::vector, uint16_t> ssg16;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t> SSG_16i;


using namespace std;

int main(int , char* [])
{
	using namespace boost;

	sg_dvv16 G(5);

	add_edge(0, 1, G);
	add_edge(1, 0, G);
	add_edge(1, 2, G);
	add_edge(2, 0, G);
	add_edge(4, 0, G);
	assert(boost::num_vertices(G)==5);
	assert(boost::num_edges(G)==5);

	{
		ssg16 Gtest(G);

		assert(boost::num_vertices(Gtest)==5);
		trace1("", boost::num_edges(Gtest));
		assert(boost::num_edges(Gtest)==4);
	}

	{
		sg_dvu Gtest(G);

		assert(boost::num_vertices(Gtest)==5);
		trace1("",boost::num_edges(Gtest));
		assert(boost::num_edges(Gtest)==4);
	}

	{
		sg_dvv32 Gtest(G);
		assert(boost::num_vertices(Gtest)==5);
		trace1("",boost::num_edges(Gtest));
		assert(boost::num_edges(Gtest)==5);

		Gtest.clear();

		boost::copy_graph(G, Gtest);
		assert(boost::num_vertices(Gtest)==5);
		trace1("",boost::num_edges(Gtest));
		assert(boost::num_edges(Gtest)==5);
	}

}

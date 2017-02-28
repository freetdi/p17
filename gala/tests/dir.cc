
#include <assert.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/adjacency_matrix.hpp>
#include "../boost.h"
#include <boost/graph/graph_traits.hpp>

template<class G>
struct dvv_config : public gala::graph_cfg_default<G> {
	static constexpr bool is_directed=true;
};
typedef gala::graph<std::vector, std::vector, uint16_t, dvv_config> sg_dvv16;

typedef gala::graph<> svbs;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t> SSG_16i;
typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::directedS> BALD_t;


using namespace std;

int main(int , char* [])
{
	svbs U;
	svbs G;

	assert(!G.is_directed());
	assert(!boost::is_directed(G));

	assert(sg_dvv16().is_directed());
	assert(boost::is_directed(sg_dvv16()));
}

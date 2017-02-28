
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

BOOST_CONCEPT_ASSERT(( boost::AdjacencyGraphConcept<svbs> ));
BOOST_CONCEPT_ASSERT(( boost::AdjacencyGraphConcept<sg_dvu> ));
BOOST_CONCEPT_ASSERT(( boost::AdjacencyGraphConcept<SSG_16i> ));

int main(int , char* [])
{
	auto x=std::is_convertible< boost::graph_traits<svbs>::directed_category,
			 boost::undirected_tag>::value;
	assert(x);
	x = std::is_convertible< boost::graph_traits<svbs>::directed_category,
			  boost::directed_tag>::value;
	assert(!x);

}

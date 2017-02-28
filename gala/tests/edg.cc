
#include <iostream>
#include <stdlib.h>
#include <gala/graph.h>
#include <gala/trace.h>
#include <gala/boost.h>
#include <boost/graph/copy.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/minimum_degree_ordering.hpp>

template<class G>
struct dvv_config : public gala::graph_cfg_default<G> {
	static constexpr bool is_directed=true;
};

typedef uint32_t unsignedType;

typedef gala::graph<std::vector, std::vector, unsignedType, dvv_config> sg_dvv;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t, dvv_config> sg_dvvn;

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> bald_t;
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> balu_t;

// typedef bald_t parsegr_t;

int main(int , char**)
{
	using namespace boost;
	sg_dvv G(5);

	assert(!G.is_ordered());
	add_edge(3, 1, G);
//	add_edge(1, 0, G);
	add_edge(1, 2, G);
	add_edge(1, 0, G);
	add_edge(2, 0, G);

	auto E=boost::edges(G);

	unsigned i=0;
	for(;E.first!=E.second;++E.first){
		++i;
		std::cout << boost::source(*E.first,G) << " ";
		std::cout << boost::target(*E.first ,G) << "\n";
	}

	assert(i==4);

	assert(!edge(0,1,G).second);
	assert(edge(1,0,G).second);
	assert(edge(1,2,G).second);
	assert(edge(2,0,G).second);
	assert(!edge(2,1,G).second);
	assert(!edge(0,2,G).second);

	assert(!edge(0,3,G).second);
	assert(!edge(1,3,G).second);
	assert(!edge(2,3,G).second);
	assert(!edge(3,0,G).second);
	assert(edge(3,1,G).second);
	assert(!edge(3,2,G).second);


}


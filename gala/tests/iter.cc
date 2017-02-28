
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
//	typedef gala::default_DEGS<G> degs_type;
	static constexpr bool is_directed=true;
};

typedef uint32_t unsignedType;

typedef gala::graph<std::vector, std::vector, unsignedType, dvv_config> sg_dvv;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t, dvv_config> sg_dvvn;

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> bald_t;
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> balu_t;

// typedef bald_t parsegr_t;

template<class T>
void countcheck(size_t size)
{

	size_t ne=size*size/4;

	boost::mt19937 rng;
	T g(size);
	g.reshape(0);
	boost::generate_random_graph(g, size, ne, rng);
	size_t e=boost::num_edges(g);
	size_t n=boost::num_vertices(g);
	std::cout << "generated " << e << " edges, " << n << " vertices\n";
	//g.make_symmetric(false);
	e = boost::num_edges(g);

	e=boost::num_edges(g);



	unsigned i=0;
	auto E=boost::edges(g);
	for(;E.first!=E.second; ++E.first){

		if(i++<10){
		std::cout << boost::source(*E.first, g) << " -- " <<
			          boost::target(*E.first, g) << "\n";
		}

	}
	std::cout << "found " << i << " edges\n";

	assert(i==e);

}


int main(int argc, char** argv)
{
	size_t size=16;
	if(argc>1) size=1<<atoi(argv[1]);

	countcheck<sg_dvv>(size);
	countcheck<sg_dvu>(size);
}

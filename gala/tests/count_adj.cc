
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
struct dir_conf : public gala::graph_cfg_default<G> {
	static constexpr bool is_directed=true;
};

typedef uint32_t unsignedType;

typedef gala::graph<std::vector, std::vector, unsignedType, dir_conf> sg_dvv;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvu;
typedef gala::graph<std::set, std::vector, uint16_t, dir_conf> sg_dvvn;

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> bald_t;
typedef gala::graph<std::set, std::vector, uint16_t, dir_conf> svud;
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

	auto somev=boost::vertices(g).first;
	++somev; ++somev; ++somev;

	unsigned i=0;
	auto E=boost::adjacent_vertices(*somev, g);
	for(;E.first!=E.second; ++E.first){

		if(i++<3){
			std::cout << *E.first << "\n";
		}

	}
	std::cout << "found " << i << " outedges\n";

	assert(i==boost::degree(*somev, g));

}


int main(int argc, char** argv)
{
	size_t size=16;
	if(argc>1) size=1<<atoi(argv[1]);

	std::cout << "counting directed set\n";
	assert(svud().is_directed());
	assert(svud().is_ordered());
	countcheck<svud>(size);

	std::cout << "counting default undirected set\n";
	assert(!gala::graph<>().is_directed());
	assert(gala::graph<>().is_ordered());
	countcheck<gala::graph< > >(size);

	std::cout << "counting directed unordered\n";
	assert(sg_dvv().is_directed());
	assert(!sg_dvv().is_ordered());
	countcheck<sg_dvv>(size);

	std::cout << "counting undirected unordered vertex\n";
	assert(!sg_dvu().is_directed());
	assert(!sg_dvu().is_ordered());
	countcheck<sg_dvu>(size);
}


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

typedef uint16_t unsignedType;

#if 1
typedef gala::graph<std::vector, std::vector, unsignedType, dvv_config> sg_dvv;
#else
typedef gala::graph<std::set, std::vector, uint16_t, dvv_config> sg_dvv;
#endif

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> bald_t;
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> balu_t;

// typedef bald_t parsegr_t;

int main(int argc, char** argv)
{
#if 0
	PARSE* p;
	try{ untested();
		p = new PARSE(std::cin);
	}catch(...){ untested();
		std::cout << "uuh\n";
		exit(2);
	}
#endif

	size_t size=1<<9;
	if(argc>1) size=1<<atoi(argv[1]);
	size_t ne=size*size/8;

	boost::mt19937 rng;

#if USE_BOOST_GRAPH
//	balu_t h;
//	boost::generate_random_graph(h, size, ne, rng);
	bald_t g;
	boost::generate_random_graph(g, size, ne, rng);
//	boost::copy_graph(h,g);
	size_t e=boost::num_edges(g);
	size_t n=boost::num_vertices(g);
	std::cout << "generated " << e << " edges, " << n << " vertices\n";
	auto EE=boost::edges(g);
 	for(;EE.first!=EE.second; ++EE.first){ untested();
 		auto s=boost::source(*EE.first, g);
 		auto t=boost::target(*EE.first, g);
 		if(!boost::edge(t,s,g).second){ untested();
			boost::add_edge(t,s,g);
		}
 	}
#else//  gala
	sg_dvv g(size);
	g.reshape(0);
	boost::generate_random_graph(g, size, ne, rng);
	size_t e=boost::num_edges(g);
	size_t n=boost::num_vertices(g);
	std::cout << "generated " << e << " edges, " << n << " vertices\n";
	g.make_symmetric(false);
	e = boost::num_edges(g);
#endif


  // boost::add_edge(0,1,g);
	e=boost::num_edges(g);

	std::cout << "symmetric " << e << " edges\n";


	unsigned i=0;
	auto E=boost::edges(g);
	for(;E.first!=E.second; ++E.first){
		++i;

		std::cout << boost::source(*E.first, g) << " -- " <<
			          boost::target(*E.first, g) << "\n";

		if(i==5) break;

	}


    i = 0;
    // boost md does not like cliques.
    trace2("clique check", n, e);
    if((n*(n-1u)) == boost::num_edges(g)){ untested();
		 std::cerr << "clique";
			 exit(0);
    }else{
        itested();
    }

    std::vector<int> inverse_perm(n, 0);
    std::vector<int> supernode_sizes(n, 1);
    auto id = boost::get(boost::vertex_index, g);
    std::vector<int> degree(n, 0);
    std::vector<int> iO(n, 0);
    std::vector<int> O(n, 0);

    /*
     * (Graph& g,
     *  DegreeMap degree,
     *  InversePermutationMap inverse_perm,
     *  PermutationMap perm,
     *  SuperNodeMap supernode_size,
     *  int delta,
     *  VertexIndexMap vertex_index_map)
     */

#ifdef HAVE_MINDEGREE_FORK
    int w =
#endif
    boost::minimum_degree_ordering
             (g,
              boost::make_iterator_property_map(&degree[0], id, degree[0]),
              &iO[0],
              &O[0],
              boost::make_iterator_property_map(&supernode_sizes[0], id, supernode_sizes[0]),
              0,
              id
#ifdef HAVE_MINDEGREE_FORK
              , ub
#endif
              );

#ifdef HAVE_MINDEGREE_FORK
	 std::cout << "done, " << w << "\n";
#endif


}

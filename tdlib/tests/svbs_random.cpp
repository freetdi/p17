#include <iostream>

#ifdef HAVE_GALA
#include <gala/examples/svbs_random.h>
#endif

#include <tdlib/elimination_orderings.hpp>

int main()
{
#ifndef HAVE_GALA
	return 77;
#else
	using namespace treedec;
	unsigned n=5;
	svbs_random g(n);
	typedef svbs_random G;
	
	typedef typename boost::graph_traits<G>::vertex_descriptor vd;
	auto VR=boost::vertices(g);
	std::vector<vd> V;
	for(;VR.first!=VR.second;++VR.first){
		V.push_back(*VR.first);
	}

	boost::add_edge(V[0],V[1],g);
	boost::add_edge(V[0],V[2],g);
	boost::add_edge(V[0],V[3],g);
	boost::add_edge(V[0],V[4],g);
	boost::add_edge(V[1],V[0],g);
	boost::add_edge(V[1],V[2],g);
	boost::add_edge(V[1],V[4],g);

	typename treedec::graph_traits<G>::treedec_type T;
	treedec::minDegree_decomp(g, T);

	std::cout << get_bagsize(T) << "\n";
#endif
}

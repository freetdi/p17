#ifdef HAVE_GALA
#include <boost/graph/graph_traits.hpp>
#include <gala/boost.h>
#include <gala/td.h>
#include <tdlib/elimination_orderings.hpp>
typedef gala::graph<> GG;
#endif

int main()
{
#ifndef HAVE_GALA
	return 77;
#else
	std::cout << "hello world\n";

	GG gg(50);

	auto V=boost::vertices(gg);

	auto next=V.first;
	++next;
	for(;next!=V.second;){
		boost::add_edge(*V.first, *next, gg);
		V.first = next++;
	}

	treedec::graph_traits<GG>::treedec_type t;
	treedec::minDegree_decomp(gg,t);
	unsigned bagsize=treedec::get_bagsize(t);

	std::cout << "a path has tree width " << bagsize - 1 << "\n";

	assert(boost::num_edges(gg)==0);

	V = boost::vertices(gg);
	next=V.first;
	for(;next!=V.second;){
		++next;
		for(;next!=V.second;){
			boost::add_edge(*V.first, *next, gg);
			++next;
		}
		++V.first;
		next = V.first;
	}
	std::cout << "a this one has " << boost::num_edges(gg) << " edges\n";

	treedec::minDegree_decomp(gg, t);
	bagsize=treedec::get_bagsize(t);
	std::cout << "tree width lower bounded by " << bagsize - 1 << "\n";

#endif
}

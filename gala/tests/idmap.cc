
#include <iostream>
#include <stdlib.h>
#include <gala/graph.h>
#include <gala/trace.h>
#include <gala/boost.h>
#include <boost/graph/copy.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/properties.hpp>

typedef gala::graph<std::set, std::vector, gala::vertex_ptr_tag> G;

int main(int, char**)
{
	G g(5);

	auto m=boost::get(boost::vertex_index, g);

	auto VR=boost::vertices(g);
	for(;VR.first!=VR.second; ++VR.first){
		std::cout << *VR.first << " " << m[*VR.first] << "\n";
	}
}

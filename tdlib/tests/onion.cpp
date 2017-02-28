#include <tdlib/graph.hpp>
#include <tdlib/iter.hpp>
#include <iostream>

#ifndef TESTGRAPH_T
#include <boost/graph/adjacency_list.hpp>
#define TESTGRAPH_T boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>
#endif

int main()
{
	using namespace boost;
	using namespace treedec;

	TESTGRAPH_T g(15);

	// layer1 2 3 4
	add_edge(0,1,g);
	add_edge(0,2,g);
	add_edge(0,3,g);
	add_edge(0,4,g);
	add_edge(1,0,g);
	add_edge(1,2,g);
	add_edge(1,4,g);

	// layer2 7 5 6
	add_edge(2,7,g);
	add_edge(4,6,g);
	add_edge(4,6,g);
	add_edge(5,3,g);

	// layer3 11 14
	add_edge(5,11,g);
	add_edge(6,14,g);

	std::vector<unsigned> vertices;

	vertices.push_back(0);
	vertices.push_back(1);

	{
		std::vector<bool> mask(num_vertices(g));
		mask[0] = true;
		mask[1] = true;
		// onion_iter(range_type v, G const& g, vis_t* vis, scratch_type* c=NULL)
		auto OLR=make_onion_range(vertices.begin(), vertices.end(), g, &mask);

		std::vector<unsigned> check={ 3,3,2 };
		unsigned layercount=0;
		for(;OLR.first!=OLR.second;++OLR.first){
			unsigned count=0;
			auto R=*OLR.first;
			for(; R.first!=R.second; ++R.first){
				std::cout << *R.first << "\n";
				++count;
			}
			std::cout << count << " elements" << "\n";
			assert(count==check[layercount]);
			++layercount;
		}
		std::cout << layercount << " layers" << "\n";
		assert(layercount=3);
	}
	std::cout << "--------\n";
// testing a bfs hack
	{
		std::vector<bool> mask(num_vertices(g));
		mask[0] = true;
		mask[1] = true;
		auto R=make_bfs_range(vertices, g, &mask);

		std::vector<unsigned> check={ 2,3,4 };
		unsigned count=0;
		for(;R.first!=R.second;++R.first){
			std::cout << *R.first << " " << R.first.is_neighbour() << "\n";
			assert(R.first.is_neighbour() || count>=3);
			assert(!R.first.is_neighbour() || count<3);
		//	assert(*R.first==check[count]);
			++count;
		}
		// assert(count=check.size());
	}
}

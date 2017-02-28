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

	TESTGRAPH_T g(5);

	add_edge(0,1,g);
	add_edge(0,2,g);
	add_edge(0,3,g);
	add_edge(0,4,g);

	add_edge(1,0,g);
	add_edge(1,2,g);
	add_edge(1,4,g);

	std::vector<unsigned> vertices;
	std::vector<bool> mask(num_vertices(g));

	vertices.push_back(0);
	vertices.push_back(1);

	{
		auto R=make_bfs_range(vertices, g, &mask);

		std::vector<unsigned> check={ 1,2,3,4,0 };
		unsigned count=0;
		for(;R.first!=R.second;++R.first){
			std::cout << *R.first << "\n";
			assert(*R.first==check[count]);
			++count;
		}
		assert(count=check.size());
	}
	{
		mask[2] = false;
		mask[3] = false;
		mask[4] = false;
		auto R=make_bfs_range(vertices, g, &mask);

		std::vector<unsigned> check={ 2,3,4 };
		unsigned count=0;
		for(;R.first!=R.second;++R.first){
			std::cout << *R.first << "\n";
			assert(*R.first==check[count]);
			++count;
		}
		assert(count=check.size());
	}

}

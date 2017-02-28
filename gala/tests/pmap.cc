
#include <assert.h>
#include <boost/functional/hash.hpp> // BUG?!
#include <boost/graph/properties.hpp>
#include "../boost.h"
#include <boost/graph/graph_traits.hpp>

typedef gala::graph<> G;

using namespace std;

int main(int , char* [])
{
	char L[] = { 't', 'e', 's', 't', 'p', 'm', 'a', 'p', ' ' };
	G g(8);
	typedef boost::property_map<G, boost::vertex_index_t>::type M;
	typedef typename boost::graph_traits<G>::vertex_descriptor vertex_descriptor;
	M v_id = get(boost::vertex_index_t(), g);

	// typedef boost::property_map<G, boost::id_tag>::type some_Pmap;
	boost::iterator_property_map<char*, M, char, char&>
	      labels_pa(L, v_id);

	auto v=boost::add_vertex(g);
	auto p=boost::vertices(g);
	assert(g.position(v) == 8);
	boost::get(labels_pa, v)='!';

	string T;
	for(;p.first!=p.second;++p.first){
		vertex_descriptor d=*p.first;
		cout << d << " ";
		auto x = boost::get(v_id, d);
		assert(x==g.position(d));
		auto y = v_id[d];
		assert(x==y);
		auto z = boost::get(labels_pa, d);
		cout << x << " " << z << "\n";
		T+=z;
	}
	assert(T=="testpmap!");

}

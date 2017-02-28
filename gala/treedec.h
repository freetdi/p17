#ifndef GALA_TREEDEC_H
#define GALA_TREEDEC_H

#include <tdlib/graph_traits.hpp>

namespace treedec{
	VCTtemplate
	struct treedec_chooser< gala::graph<SGARGS> >{ //
		typedef typename boost::graph_traits< gala::graph<SGARGS> >::vertex_descriptor value_type;
		typedef typename outedge_set< gala::graph<SGARGS> >::type bag_type;
		typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, bag_type> type;
	};
}

#endif

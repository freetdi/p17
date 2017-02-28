#ifndef GALA_BOOST_COPY_H
#define GALA_BOOST_COPY_H
#include "boost.h"

namespace boost {

namespace detail {
	template<class SRC, galaPARMS, class X=void>
	struct copy_helper{
		template<class GG, class BEI=typename graph_traits<SRC>::edge_iterator >
		class edge_iw : public BEI{ //
			GG const& _g;

		public:
			edge_iw(BEI& b, const GG& g ) : BEI(b), _g(g) {}
			std::pair<size_t, size_t> operator*()
			{
				typedef typename graph_traits<SRC>::edge_descriptor edge_descriptor;
				typedef typename graph_traits<SRC>::vertex_descriptor vertex_descriptor;
				edge_descriptor e=BEI::operator*();
				vertex_descriptor s=source(e,_g);
				vertex_descriptor t=target(e,_g);
				return std::pair<size_t, size_t>(s,t);
			}
		};

		static void boost_to_gala(const SRC& src, gala::graph<SGARGS>& tgt)
		{
			typedef typename graph_traits<SRC>::edge_iterator BEI;
			typedef edge_iw<SRC, BEI> bw_t;
			tgt.reshape(boost::num_vertices(src), boost::num_edges(src), boost::is_directed(src));
			auto E=edges(src);

			if(boost::is_directed(src)){
				if(tgt.is_multiedge()){
					// probably more cases...
					incomplete();
				}
			}

			if(boost::is_multigraph<SRC>()){
				tgt.fill_in_edges(bw_t(E.first, src), bw_t(E.second, src), true);
			}else if(boost::is_directed(src)){
				// edge counter ...
				tgt.fill_in_edges(bw_t(E.first, src), bw_t(E.second, src), true);
			}else{
				tgt.fill_in_edges(bw_t(E.first, src), bw_t(E.second, src), false);
				incomplete();
			}
		}
		static void gala_to_boost(const gala::graph<SGARGS>& src, SRC& tgt)
		{
			auto nv=src.num_vertices();
			if(num_vertices(tgt)==0){
				// good idea?
				tgt=std::move(SRC(nv));
			}else{
				tgt.clear(); // HACK, not supported by boost!
				while(num_vertices(tgt)<nv){
					add_vertex(tgt);
				}
				while(num_vertices(tgt)>nv){ untested();
					remove_vertex(num_vertices(tgt)-1,tgt);
				}
			}

			// uuh, directed vs undirected?
			auto V=edges(src);
			for(;V.first!=V.second;++V.first){
				auto v=source(*V.first, src);
				auto w=target(*V.first, src);
				auto vp=src.position(v);
				auto wp=src.position(w);
				add_edge(vp,wp,tgt);
			}
		}
	};

}

// assign boost graph
// (how to pick properly?
   template<class SRC, galaPARMS>
	inline void copy_graph(const SRC& g, gala::graph<SGARGS>& h,
			typename boost::graph_traits<SRC>::vertex_descriptor=boost::graph_traits<SRC>::null_vertex() )
	{
		detail::copy_helper<SRC, SGARGS>::boost_to_gala(g, h);
	}

   template<class SRC, galaPARMS>
	inline 	typename
	std::enable_if<is_convertible< typename
	  graph_traits<SRC>::traversal_category , edge_list_graph_tag>::value, void >::type
	 copy_graph(const gala::graph<SGARGS>& g, SRC& h)
	{
		detail::copy_helper<SRC, SGARGS>::gala_to_boost(g, h);
	}

	/// collides with...
	// VCTtemplate
	// inline typename graph_traits<gala::graph<SGARGS> >::vertices_size_type
	// num_vertices(const gala::graph<SGARGS>& g)
	// { untested();
	// 	return g.num_vertices();
	// }

}// boost

#endif

// Felix Salfelder 2016
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// exact decomposition, common denominator
//

// does not work (yet?) because the size is not known in advance.
// ... immutable_clone needs multipass iterator (?)
// #define NEWRANGE

#ifndef TD_EXACT_H
#define TD_EXACT_H

#include "lower_bounds.hpp"
// #include "misc.hpp"
#include "preprocessing.hpp"
#include "overlay.hpp"
#include "treedec_misc.hpp"
#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <boost/graph/bandwidth.hpp>

namespace treedec{


namespace draft{

#if 1 // MOVE! (or use directly...)
template<class G, class O>
void print_matrix(G const& g, O& o)
{ itested();
	unsigned n=boost::num_vertices(g);

	for(unsigned i=0; i<n; ++i){ itested();
//		auto a=boost::adjacent_vertices(g, i);
//		for(;a.first!=a.second; ++a.first){
//			std::cout << "\n";
//
//		}
		for(unsigned j=0; j<n; ++j){ itested();
			if(boost::edge(i,j,g).second){ itested();
				o << "1";
			}else{ itested();
				o << "0";
			}
		}
		
		o << "\n";
	}
}

template<class G, class M, class MI>
unsigned rcmk_(G const& g, M& perm, MI& inv_perm )
{

	typedef typename boost::graph_traits<G>::vertices_size_type size_type;

	typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;
//	std::vector<size_type> inv_perm(boost::num_vertices(g));

        unsigned n=boost::num_vertices(g);

        std::vector<int> colors(n, 0);
	//reverse cuthill_mckee_ordering
	boost::cuthill_mckee_ordering(g, inv_perm.rbegin(),
			// boost::get(boost::vertex_color, g),
			 &colors[0],
			boost::make_degree_map(g));

	std::cerr << "Reverse Cuthill-McKee ordering:" << std::endl;
	std::cerr << "  ";

	auto index_map = boost::get(boost::vertex_index, g);

	for (typename std::vector<Vertex>::const_iterator i=inv_perm.begin();
			i != inv_perm.end(); ++i){
		std::cerr << "(" << *i << ") ";
        }
	std::cerr << std::endl;

	if(boost::num_vertices(g)){
		for (size_type c = 0; c != inv_perm.size(); ++c){
			perm[index_map[inv_perm[c]]] = c;
		}
		std::cout << "c  bandwidth: ";
                auto bw=boost::bandwidth(g, boost::make_iterator_property_map(&perm[0], index_map, perm[0]));
		std::cout << bw << "\n";
                
//		print_matrix(g, inv_perm, std::cout);

                return bw;
	}
        return 0;
}
#endif

template<typename G_t,
	template<class G_> class config,
	template<class X, template<class G__> class Y> class kernel>
class exact_decomposition{
public:
  typedef kernel<G_t, config> kern_t;
public:
  template<class GraphType>
  struct cfg16 : config<GraphType>{
      static constexpr unsigned max_vertex_index=15;
      // chunksize?!
  };
  template<class GraphType>
  struct cfg32 : config<GraphType>{
      static constexpr unsigned max_vertex_index=31;
      // chunksize?!
  };
  template<class GraphType>
  struct cfg64 : config<GraphType>{
      static constexpr unsigned max_vertex_index=63;
  };
  template<class GraphType>
  struct cfg128 : config<GraphType>{
      static constexpr unsigned max_vertex_index=127;
  };
  template<class GraphType>
  struct cfg256 : config<GraphType>{
      static constexpr unsigned max_vertex_index=255;
  };
  template<class GraphType>
  struct cfg512 : config<GraphType>{
      static constexpr unsigned max_vertex_index=511;
  };
  template<class GraphType>
  struct cfg1024 : config<GraphType>{
      static constexpr unsigned max_vertex_index=1023;
  };
public:
    exact_decomposition(G_t &G)
        : _g(G) {
    }

    template<class T>
    void try_it(T&, unsigned lb_bs);
    template<class G, class T>
    void run_kernel(G const&, T&, unsigned& lb_bs);
private:
    template<class T_t>
    void do_components(T_t&, unsigned lb_bs);
private:
    G_t& _g;
};


template<typename G_t,
	template<class G_> class config,
	template<class X, template<class G__> class Y> class kernel>
template<class G_t_, class T_t>
inline void exact_decomposition<G_t, config, kernel>::run_kernel(
        G_t_ const& G, T_t& T, unsigned& lb_bs)
{ untested();

    auto numv=boost::num_vertices(G);
    // std::cout << "c kernel " << numv << "\n";
    if(numv<=32){ untested();
        kernel<G_t_, cfg32> kern(G);
        kern.do_it(T, lb_bs);
    }else if(numv<=64){ untested();
        kernel<G_t_, cfg64> kern(G);
        kern.do_it(T, lb_bs);
    }else if(numv<=128){ untested();
        kernel<G_t_, cfg128> kern(G);
        kern.do_it(T, lb_bs);
    }else{ incomplete();
        kernel<G_t_, cfg1024> kern(G);
        kern.do_it(T, lb_bs);
    }
}

template<typename G_t,
	template<class G_> class config,
	template<class X, template<class G__> class Y> class kernel>
template<class T_t>
inline void exact_decomposition<G_t, config, kernel>::do_components(
        T_t& t, unsigned lb_bs)
{

    // Compute a tree decomposition for each connected component of G and glue
    // the decompositions together.
#ifndef NEWRANGE
    typedef std::vector<std::set<typename boost::graph_traits<G_t>::vertex_descriptor> > components_t;
    components_t components;
    treedec::get_components(_g, components);
#else
    auto R=boost::vertices(_g);
    std::vector<EXCUT_BOOL> visited(n);
    auto cmps_range = make_components_range(
                R.first, R.second,
                _g, &visited, NULL, EXCUT_BOOL());
#endif

    typename boost::graph_traits<T_t>::vertex_descriptor root = boost::add_vertex(t);
    typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> vdMap;
#ifndef NEWRANGE
    typename components_t::iterator i = components.begin();
    for(; i!=components.end(); ++i)
#else
    for(; cmps_range.first != cmps_range.second; ++cmps_range.first)
#endif
    {
        //Ignore isolated vertices (already included in 'bags').
#ifndef NEWRANGE
        trace2("found component ", i->size(), components.size());
        if(i->size() == 1){
            continue;
            auto nv=boost::add_vertex(t);
            auto& B=bag(nv, t);
            treedec::push(B, *(*i).begin());
            trace2("isolated node ", nv,  *(*i).begin());
            if(nv!=0){ // uuh hack
                boost::add_edge(nv, nv-1, t);
            }

        }
#endif

        typedef typename graph_traits<G_t>::immutable_type immutable_type;

#ifndef NEWRANGE
        unsigned compsize = i->size();
        auto comp_range = *i;
#else
        unsigned compsize = n; // don't know...
        auto comp_range = *(cmps_range.first);
#endif
        immutable_type H(compsize);
        const immutable_type& G_=draft::immutable_clone(_g, H,
                std::begin(comp_range), std::end(comp_range), compsize,
                &vdMap);

#ifdef NEWRANGE
        if(boost::num_vertices(H)==1){
            incomplete();
        }
#endif
        trace3("excut comp", lb_bs, boost::num_vertices(G_), boost::num_edges(G_));

        assert_connected(G_);

//        std::cout << "raw.\n";
//        print_matrix(G_, boost::identity_property_map(), std::cout);

#ifdef USE_RCMK
	typedef typename boost::graph_traits<immutable_type>::vertex_descriptor Vertex;
	typedef std::vector<Vertex> M;
	auto nv=boost::num_vertices(G_);

	auto ne2=boost::num_edges(G_);
	auto ne3=boost::num_edges(_g);

	M m(nv);
	M minv(nv);
        unsigned bw=rcmk_(G_, m, minv);

        typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> balu;
        balu OG;

//        using boost::vertices;
//        boost::VertexListGraphConcept<immutable_type> x;
//        boost::copy_graph(G_, OG);
//        std::cout << "c cmkwidth" << get_width_of_elimination_ordering(OG, minv) << "\n";

        auto G_cmk=treedec::make_mapped_graph(G_, m);
	auto ne1=boost::num_edges(G_cmk);
        assert(ne1==ne2);
#endif

        // assert(ne2==ne3);
        // assert(vdMap.size()==nv);

        T_t T_;
        // std::cout << "kernel\n";
        // print_matrix(G_cmk, boost::identity_property_map(), std::cout);
        // std::cout << "now, run.\n";
#ifdef USE_RCMK
        run_kernel(G_cmk, T_, lb_bs);

        // permute_vector(vdMap, m); // hmm
        concat_maps(minv, vdMap);
        draft::append_decomposition(t, std::move(T_), G_, minv);
#else
        run_kernel(G_, T_, lb_bs);

#ifndef NDEBUG
        unsigned tn=boost::num_vertices(t);
        unsigned tn_=boost::num_vertices(T_);
#endif
        draft::append_decomposition(t, std::move(T_), G_, vdMap);
        assert( tn+tn_ == boost::num_vertices(t));
#endif
    }
} // do_it

template<typename G_t,
	template<class G_> class config,
	template<class X, template<class Y> class c> class kernel>
template<class T_t>
void exact_decomposition<G_t, config, kernel>::try_it(T_t& T, unsigned lb_bs)
{
    int lb_tw=lb_bs-1;

    auto n=boost::num_vertices(_g);
    auto e=boost::num_edges(_g);
    trace3("exact_decomposition", lb_tw, n, e);
    if(n==0){ untested();
        boost::add_vertex(T);
        return;
    }else{
    }

    //Preprocessing.
    // this is really inefficient...
    int low = -1;

    std::vector<boost::tuple<
        typename treedec_traits<typename treedec_chooser<G_t>::type>::vd_type,
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type
         > > bags;

    // if config.preprocessing?
    treedec::preprocessing(_g, bags, low);

    if(boost::num_edges(_g) == 0){ untested();
        treedec::glue_bags(bags, T);
        return;
    }else{
    }

    std::cout << "c PP said tw" << low << "\n";

    //Lower bound on the treewidth of the reduced instance of G.
    G_t H(_g);
    int tw_lb_deltaC = treedec::lb::deltaC_least_c(H);

    std::cout << "c deltaC said " << tw_lb_deltaC << "\n";

    trace3("excut comb", lb_tw, low, tw_lb_deltaC);
    if(low > lb_tw){
        lb_tw = low;
    }else{ untested();
    }

    if (tw_lb_deltaC > lb_tw){ untested();
        lb_tw = tw_lb_deltaC;
    }else{
    }

    trace3("excut comb", lb_tw, boost::num_vertices(_g), boost::num_edges(_g));

    do_components(T, lb_tw+1);

    treedec::glue_bags(bags, T);
} // try_it

}// draft

}// treedec

#endif
// vim:ts=8:sw=4:et

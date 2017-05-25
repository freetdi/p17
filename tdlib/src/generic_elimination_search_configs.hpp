// Lukas Larisch, 2014 - 2017
//
// (c) 2014-2016 Goethe-Universität Frankfurt
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
//

#ifndef TD_GENERIC_ELIM_SEARCH_CONFIGS
#define TD_GENERIC_ELIM_SEARCH_CONFIGS

#include <vector>
#include <limits.h>

#include "lower_bounds.hpp"
#include "elimination_orderings.hpp"
#include "postprocessing.hpp"
#include "generic_elimination_search.hpp"

#include <boost/graph/copy.hpp>
#include <gala/boost_copy.h>

#include <iostream>

// "virtual overloads" for algos derived from gen_search_base.

namespace treedec{
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> TD_graph_t;


namespace gen_search{

namespace configs{
using treedec::gen_search::generic_elimination_search_DFS;
template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_1;
template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_2;
template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_3;

template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_p17;
template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_p17_2;




/*
    -initial_lb_algo = deltaC_least_c
    -initial_ub_algo = NONE
    -lb_algo = NONE
    -next = all nodes "from left to right"
    -refiner = NONE
*/

template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_1 : generic_elimination_search_DFS<G_t, CFG_DFS_1<G_t, cfg>, cfg> {
    typedef generic_elimination_search_DFS<G_t, CFG_DFS_1<G_t, cfg>, cfg> baseclass;
    CFG_DFS_1(G_t const& G) : baseclass(G)
    {}

    typedef typename boost::graph_traits<G_t>::vertex_descriptor vd;

    static bool is_jumper(){
        return false;
    };

    static const std::string name()
    {
        return "CFG_DFS_1";
    }

    // return bagsize!
    static unsigned initial_lb_algo(const G_t &G) {
        G_t H(G);
        return treedec::lb::deltaC_least_c(H)+1;
    }

    static unsigned initial_ub_algo(const G_t &G, std::vector<vd> &O)
    {
        for(unsigned i = 0; i < boost::num_vertices(G); ++i){
            O[i] = i;
        }
        return boost::num_vertices(G);
    }


    static unsigned lb_algo(G_t &){ //aka no lb algo
        return 0;
    }

    static bool next(vd& n, const G_t & /*G*/, const
            std::vector<BOOL> &active, unsigned &idx, const std::vector<vd>
            &/*elim_ordering*/, unsigned /*depth*/)
    {
        for(; idx < active.size(); ++idx){
            if(active[idx]){
                n = idx++;
                return true;
            }
        }

        return false;
    }
#ifndef NDEBUG
    template<class G>
    struct dvv_config : public gala::graph_cfg_default<G> {
        static constexpr bool is_directed=true;
    };
#endif

    static unsigned refiner(const G_t &G, std::vector<vd> &orig_elim, std::vector<vd> &new_elim) //aka no refiner
    {
        G_t H(G);
        treedec::minimalChordal(H, orig_elim, new_elim);

#ifdef USE_GALA
#ifndef NDEBUG
        typedef gala::graph<std::vector, std::vector, uint32_t> TT;
        typedef gala::graph<std::vector, std::vector, uint32_t, dvv_config> gdvv;
        TT tst;
        gdvv X(std::move(tst));
        gdvv Htest(boost::num_vertices(G));
        auto E=boost::edges(G);
        for(; E.first!=E.second; ++E.first){
            auto s=boost::source(*E.first, G);
            auto t=boost::target(*E.first, G);
            boost::add_edge(s,t,Htest);
            if(!is_directed(G)){
                boost::add_edge(t,s,Htest);
            }else{
            }
        }
        auto T=treedec::get_bagsize_of_elimination_ordering(Htest, new_elim);
#endif
#endif

        G_t H2(G);
        auto ret=treedec::get_bagsize_of_elimination_ordering(H2, new_elim); //not necessary

#ifdef USE_GALA
        assert(ret==T);
#endif
        return ret;
    }

}; // CFG_DFS_1

/*
    -initial_lb_algo = deltaC_least_c
    -initial_ub_algo = minDegree
    -lb_algo = NONE
    -next = all nodes "from left to right"
*/
template <typename G_t, template<class G, class ...> class CFGT>
struct CFG_DFS_2 : generic_elimination_search_DFS<G_t, CFG_DFS_2<G_t, CFGT>, CFGT> {
    typedef generic_elimination_search_DFS<G_t, CFG_DFS_2<G_t, CFGT>, CFGT> baseclass;
    CFG_DFS_2(G_t const& G) : baseclass(G)
    {
    }

    CFG_DFS_2(G_t const& G, unsigned m, unsigned n) : baseclass(G, m, n)
    {}

    typedef typename boost::graph_traits<G_t>::vertex_descriptor vd;

    static bool is_jumper(){
        return false;
    };

    static const std::string name()
    {
        return "CFG_DFS_2";
    }

    static unsigned initial_lb_algo(const G_t &G) {
        G_t H(G);
        return treedec::lb::deltaC_least_c(H)+1;
    }

    static unsigned initial_ub_algo(const G_t &G, std::vector<vd> &O)
    {
        G_t H(G);
        return treedec::minDegree_ordering(H, O)+1;
    }

    static unsigned lb_algo(const G_t &){
        // no lb algo
        return 0;
    }

    static bool next(vd& n, const G_t & /*G*/, const std::vector<BOOL> &active,
            unsigned &idx, const std::vector<vd> &/*elim_ordering*/,
            unsigned /*depth*/)
    {
        for(; idx < active.size(); ++idx){
            if(active[idx]){
                n = idx;
                idx++;
                return true;
            }
        }

        return false;
    }

    static unsigned refiner(const G_t &G, std::vector<vd> &orig_elim, std::vector<vd> &new_elim)
    {
        G_t H(G);
        treedec::minimalChordal(H, orig_elim, new_elim);
        G_t H2(G);
        return treedec::get_bagsize_of_elimination_ordering(H2, new_elim); //not necessary
    }
}; // CFG_DFS_2

/*
    -initial_lb_algo = deltaC_least_c
    -initial_ub_algo = fillIn
    -lb_algo = NONE
    -next = all nodes "from left to right"
*/
template <typename G_t, template<class G, class ...> class cfg>
struct CFG_DFS_3 : generic_elimination_search_DFS<G_t, CFG_DFS_3<G_t, cfg>, cfg> {
    typedef generic_elimination_search_DFS<G_t, CFG_DFS_3<G_t, cfg>, cfg> baseclass;
    typedef typename boost::graph_traits<G_t>::vertex_descriptor vd;
    CFG_DFS_3(G_t const& G) : baseclass(G)
    {}

    static bool is_jumper(){
        return false;
    };

    static const std::string name()
    {
        return "CFG_DFS_3";
    }

    static unsigned initial_lb_algo(const G_t &G)
    {
        G_t H(G);
        return treedec::lb::deltaC_least_c(H)+1;
    }

    static unsigned initial_ub_algo(const G_t &G, std::vector<vd> &O)
    {
        G_t H(G);
        return treedec::fillIn_ordering(H, O)+1;
    }

    static unsigned lb_algo(const G_t &){
        // no lb algo
        return 0;
    }

    static bool next(vd& n, const G_t & /*G*/, const std::vector<BOOL> &active,
            unsigned &idx, const std::vector<vd> &/*elim_ordering*/,
            unsigned /*depth*/)
    {
        for(; idx < active.size(); ++idx){
            if(active[idx]){
                n = idx;
                idx++;
                return true;
            }
        }

        return false;
    }

    static unsigned refiner(const G_t &G, std::vector<vd> &orig_elim, std::vector<vd> &new_elim) //aka no refiner
    {
        G_t H(G);
        treedec::minimalChordal(H, orig_elim, new_elim);
        G_t H2(G);
        return treedec::get_bagsize_of_elimination_ordering(H2, new_elim); //TODO: get from minimalChordal
    }
}; // CFG_DFS_3


/* PACE 2017 config
    -initial_lb_algo = deltaC_least_c
    -initial_ub_algo = minDegree. no, FI
    -lb_algo = deltac_least_c
    -next = all nodes "from left to right"
*/
template <typename G_t, template<class G, class ...> class CFGT>
struct CFG_DFS_p17 : generic_elimination_search_DFS<G_t, CFG_DFS_p17<G_t, CFGT>, CFGT> {
    typedef generic_elimination_search_DFS<G_t, CFG_DFS_p17<G_t, CFGT>, CFGT> baseclass;
    typedef typename boost::graph_traits<G_t>::vertices_size_type vertices_size_type;
    typedef CFGT<G_t> CFG;
    CFG_DFS_p17(G_t const& G) : baseclass(G)
    {
    }

    CFG_DFS_p17(G_t const& G, unsigned m, unsigned n) : baseclass(G, m, n)
    {
        // ???
        // impl::preprocessing<G_t> PP(G);
        // PP.do_it();
    }

    typedef typename boost::graph_traits<G_t>::vertex_descriptor vd;

    static bool is_jumper(){ untested();
//        return true;
        return false;
    };

    static const std::string name()
    {
        return "CFG_DFS_p17";
    }

    static unsigned initial_lb_algo(const G_t &G) { untested();
        G_t H(G);
        return treedec::lb::deltaC_least_c(H)+1;
//        return treedec::lb::LBPC_deltaC(H)+1;
    }

    // returns bagsize
    // p17::
    static unsigned initial_ub_algo(const G_t &G, std::vector<vd> &O)
    {
        vertices_size_type best;
        {
            for(unsigned i = 0; i < boost::num_vertices(G); ++i){
                O[i] = i;
            }
            G_t H2(G);
            best=treedec::get_bagsize_of_elimination_ordering(H2, O);
            CFG::message(bDEBUG, "initial ub n %d\n", best);
        }
#if 0
        // static_assert(sizeof(vd)==sizeof(int)); no.
        std::vector<int> O2(O.begin(), O.end()); // bmd wants vector<int>, why?!
        impl::bmdo<G_t> A(H, O2);
        A.do_it();
        auto bs=A.bagsize();
        O.assign(O2.begin(), O2.end());
        return bs;
#else
        G_t H(G);
        auto fi_bs=treedec::fillIn_ordering(H, O)+1;
        CFG::message(bDEBUG, "initial ub f %d\n", fi_bs);

        if(best<fi_bs){
            for(unsigned i = 0; i < boost::num_vertices(G); ++i){
                O[i] = i;
            }
        }else{
            best = fi_bs;
        }
        // why not try bmd as well?!
        return best;
#endif
    }


    static unsigned lb_algo(G_t &G){
//        return 0;
        G_t H(G);
        //TD_graph_t H; // BUG
        // boost::copy_graph(G,H);

        return treedec::lb::deltaC_least_c(H)+1;
    }

    static bool next(vd& n, const G_t & /*G*/, const std::vector<BOOL> &active,
            unsigned &idx, const std::vector<vd> &/*elim_ordering*/,
            unsigned /*depth*/)
    {
        for(; idx < active.size(); ++idx){
            if(active[idx]){
                n = idx;
                idx++;
                return true;
            }
        }

        return false;
    }

    static unsigned refiner(const G_t &G, std::vector<vd> &orig_elim, std::vector<vd> &new_elim)
    { untested();
#if 0
        incomplete();
        G_t H(G);
#else
        TD_graph_t H;
         boost::copy_graph(G,H);
#endif
         incomplete(); // need interruption
        treedec::minimalChordal(H, orig_elim, new_elim);
        G_t H2(G);
//        if(treedec::is_directed(H2)){ untested(); }
//        //bug.
        if(boost::is_directed(H2)){ untested();
        }else{ untested();
        }
        // hmmm H2 is not directed?
        return treedec::get_bagsize_of_elimination_ordering(H2, new_elim); //not necessary
    }
}; // p17

/* PACE 2017 config 2
    -initial_lb_algo = deltaC_least_c
    -initial_ub_algo = minDegree
    -lb_algo = NONE
    -next = all nodes "from left to right"
*/
template <typename G_t, template<class G, class ...> class CFGT>
struct CFG_DFS_p17_2 : generic_elimination_search_DFS<G_t, CFG_DFS_p17_2<G_t, CFGT>, CFGT> {
    typedef generic_elimination_search_DFS<G_t, CFG_DFS_p17_2<G_t, CFGT>, CFGT> baseclass;
    CFG_DFS_p17_2(G_t const& G) : baseclass(G)
    {}

    CFG_DFS_p17_2(G_t const& G, unsigned m, unsigned n) : baseclass(G, m, n)
    {}

    typedef typename boost::graph_traits<G_t>::vertex_descriptor vd;

    static bool is_jumper(){ untested();
        return true;
    };

    static const std::string name()
    {
        return "CFG_DFS_p17_2";
    }

    static unsigned initial_lb_algo(const G_t &)
    {
        return 0;
    }

    static unsigned initial_ub_algo(const G_t &, std::vector<vd> &)
    {
        return UINT_MAX;
    }

    static unsigned lb_algo(const G_t&){
        // no lb algo
        return 0;
    }

    static bool next(vd& n, const G_t & /*G*/, const std::vector<BOOL> &active, unsigned
            &idx, const std::vector<vd> &elim_ordering, unsigned depth)
    {
        if(idx == 0 && active[elim_ordering[depth]]){
            ++idx;
            n = elim_ordering[depth];
            return true;
        }

        for(; idx < active.size(); ++idx){
            if(active[idx]){
                n = idx;
                idx++;
                return true;
            }
        }

        return false;
    }

    static unsigned refiner(const G_t &G, std::vector<vd> &orig_elim, std::vector<vd> &new_elim)
    {
        G_t H(G);
        treedec::minimalChordal(H, orig_elim, new_elim);
        G_t H2(G);
        incomplete();
        return treedec::get_bagsize_of_elimination_ordering(H2, new_elim); //not necessary
    }
};

} //namespace configs

} //namespace gen_search

} //namespace treedec

#endif //TD_GENERIC_ELIM_SEARCH
// vim:ts=8:sw=4:et

// Lukas Larisch, 2014 - 2016
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

/*
 * Offers some recommended combinations of the algorithms.
 *
 * These functions are most likely to be interesting for outside use:
 *
 * - void PP_MD(G_t &G, T_t &T)
 * - void PP_MD(G_t &G, T_t &T, int &low)
 * - void PP_FI(G_t &G, T_t &T)
 * - void PP_FI(G_t &G, T_t &T, int &low)
 * - void PP_FI_TM(G_t &G, T_t &T)
 * - void PP_FI_TM(G_t &G, T_t &T, int &low)
 *
 * - void exact_decomposition_cutset(G_t &G, T_t &T)
 * - void exact_decomposition_cutset(G_t &G, T_t &T, int low)
 * - void exact_decomposition_cutset_decision(G_t &G, T_t &T, int k)
 * - void exact_decomposition_dynamic(G_t &G, T_t &T)
 * - void exact_decomposition_dynamic(G_t &G, T_t &T, int low)
 *
 * - void separator_algorithm_MSVS(G_t &G, T_t &T)
 * - void separator_algorithm_TM(G_t &G, T_t &T)
 * - void MSVS_trivial(G_t &G, T_t &T)
 *
 */

#ifndef TD_COMBINATIONS
#define TD_COMBINATIONS

#include <set>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional.hpp>

#include "algo.hpp"
#include "dynamicCR.hpp"
#include "elimination_orderings.hpp"
#include "exact_cutset.hpp"
#include "exact_ta.hpp"
#include "lower_bounds.hpp"
#include "misc.hpp"
#include "postprocessing.hpp"
#include "preprocessing.hpp"
#include "overlay.hpp"
#include "separator_algorithm.hpp"

namespace treedec{



//Recursively applies preprocessing rules and glues corresponding bags with
//current tree decomposition this version applies the minDegree-heuristic on
//not fully preprocessable graph instances.
template <typename G_t, typename T_t>
void PP_MD(G_t &G, T_t &T, int &low){
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }else{
    }

    std::vector<boost::tuple<
        typename treedec_traits<typename treedec_chooser<G_t>::type>::vd_type,
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type
         > > bags;

    treedec::preprocessing(G, bags, low);
    if(boost::num_edges(G) > 0){
        treedec::minDegree_decomp( G, T,
         (typename std::vector<typename treedec_chooser<G_t>::value_type>*)NULL,
         UINT_MAX, true /*ignore_isolated_vertices*/);
    }else{
        untested();
    }
    treedec::glue_bags(bags, T);
}

//Recursively apply preprocessing rules and glues corresponding bags with
//current tree decomposition this version applies the fillIn-heuristic on
//not fully preprocessable graph instances.
template <typename G_t, typename T_t>
void PP_FI(G_t &G, T_t &T, int &low_tw)
{
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }else{
    }

#ifndef NOBAGS
    std::vector<boost::tuple<
        typename treedec_traits<typename treedec_chooser<G_t>::type>::vd_type,
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type
         > > bags;

    treedec::preprocessing(G, bags, low_tw);
#else
    impl::preprocessing<G_t> A(G);
    A.set_treewidth(low_tw, -1u);
    A.do_it();
    low_tw = A.get_treewidth();
    // A.get_bags(bags); // we don't need them!
    A.get_graph(G);
#endif

    if(boost::num_edges(G) > 0){
        unsigned low2=-1;
        treedec::impl::fillIn_decomp(G, T, low2, true); //ignore_isolated
        low_tw = low2;
    }else{ untested();
    }
#ifndef NOBAGS
    treedec::glue_bags(bags, T);
#else
    skeleton<...> S(...)
    S.do_it();
#endif
}


//Recursively apply preprocessing rules and glue corresponding bags with
//current tree decomposition. This version applies the fillIn-heuristic followed
//by triangulation minimization on not fully preprocessable graph instances.
template <typename G_t, typename T_t>
void PP_FI_TM(G_t &G, T_t &T, int &low)
{
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }

    std::vector<boost::tuple<
        typename treedec_traits<typename treedec_chooser<G_t>::type>::vd_type,
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type
         > > bags;

    treedec::preprocessing(G, bags, low);

    if(boost::num_edges(G) > 0){
        typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> old_elim_ordering;
        typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> new_elim_ordering;

        G_t H(G);
        //true = ignore isolated vertices
        treedec::fillIn_ordering(G, old_elim_ordering, true); //ignore isolated
        G = H; // reset

#ifdef DEBUG
        for( auto i : old_elim_ordering){
            assert(is_valid(i,G));
        }
#endif
        treedec::minimalChordal(G, old_elim_ordering, new_elim_ordering);

        typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> new_elim_ordering_(old_elim_ordering.size());
        unsigned c = 0;
        for(unsigned i = 0; i < new_elim_ordering.size(); i++){
            if(boost::out_degree(new_elim_ordering[i], G) > 0){
                new_elim_ordering_[c++] = new_elim_ordering[i];
            }
        }

        treedec::ordering_to_treedec(G, new_elim_ordering_, T);
    }

    treedec::glue_bags(bags, T);
}

template <typename G_t, typename T_t>
void exact_decomposition_ta(G_t &G, T_t &T, int lb_tw)
{
    using draft::exact_decomposition;
#if 1
    auto alg=exact_decomposition<G_t, algo::default_config, exact_ta>(G);
                                   // really^?
#else
    auto alg=exact_decomposition<G_t, algo::default_config, draft::exact_cutset>(G);
#endif
    return alg.try_it(T, lb_tw+1);
}

template <typename G_t, typename T_t>
void exact_decomposition_cutset(G_t &G, T_t &T, int lb_tw)
{
    using draft::exact_decomposition;
    using draft::exact_cutset;

    auto alg=exact_decomposition<G_t, algo::default_config, exact_cutset>(G);
    return alg.try_it(T, lb_tw+1);
}

// TODO: use class.
template <typename G_t, typename T_t>
bool exact_decomposition_cutset_decision(G_t &G, T_t &T, int k){
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);

        if(k >= -1){ return true; }
        else{ return false; }
    }

    //Preprocessing.
    int low_tw = -1;

    std::vector<boost::tuple<
        typename treedec_traits<typename treedec_chooser<G_t>::type>::vd_type,
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type
         > > bags;

    treedec::preprocessing(G, bags, low_tw);

    if(boost::num_edges(G) == 0){
        treedec::glue_bags(bags, T);
        if(low_tw <= k){
            return true;
        }
        return false;
    }

    //Lower bound on the treewidth of the reduced instance of G.
    G_t H(G);
    int lb_deltaC = treedec::lb::deltaC_least_c(H);

    int lb = low_tw;
    if(lb_deltaC > lb){
        lb = lb_deltaC;
    }else{
    }

    if(lb > k){ untested();
        return false;
    }else{
    }

    //Compute a treedecomposition for each connected component of G and glue the decompositions together.
    std::vector<std::set<typename boost::graph_traits<G_t>::vertex_descriptor> > components;
    treedec::get_components(G, components);

    // root
    boost::add_vertex(T);

    for(unsigned int i = 0; i < components.size(); i++){
        //Ignore isolated vertices (already included in 'bags').
        if(components[i].size() == 1){
            continue;
        }

        G_t G_;
        typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> vdMap;
        treedec::induced_subgraph(G_, G, components[i], vdMap);
        T_t T_;

        while(!treedec::exact_cutset(G_, T_, lb)){
            lb++;
            if(lb > k){
                return false;
            }
        }
    }

    return true;
}



template <typename G_t, typename T_t>
void exact_decomposition_dynamic(G_t &G, T_t &T, int lb){
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }

    //preprocessing
    int low = -1;
    std::vector<boost::tuple<
        typename treedec_traits<typename treedec_chooser<G_t>::type>::vd_type,
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type
         > > bags;

    treedec::preprocessing(G, bags, low);
    if(boost::num_edges(G) == 0){
        treedec::glue_bags(bags, T);
        return;
    }

    lb = (low > lb)? low : lb;

    //Compute a treedecomposition for each connected component of G and glue the decompositions together.
    std::vector<std::set<typename boost::graph_traits<G_t>::vertex_descriptor> > components;
    treedec::get_components(G, components);

    if(components.size() == 1){
        treedec::CR_dynamic_decomp(G, T, lb);

        treedec::glue_bags(bags, T);
        return;
    }

    // root
    boost::add_vertex(T);

    for(unsigned int i = 0; i < components.size(); i++){ untested();
        //Ignore isolated vertices (already included in 'bags').
        if(components[i].size() == 1){
            continue;
        }

        G_t G_;
        typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> vdMap;
        treedec::induced_subgraph(G_, G, components[i], vdMap);
        T_t T_;

        treedec::CR_dynamic_decomp(G_, T_, lb);

        draft::append_decomposition(T, std::move(T_), G_, vdMap);
    }

    // uuh use append_decomposition?
    treedec::glue_bags(bags, T);
}


template <typename G_t, typename T_t>
void exact_decomposition_chordal(G_t &G, T_t &T){
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }

    typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> elim_ordering;
    treedec::LEX_M_minimal_ordering(G, elim_ordering);
    treedec::ordering_to_treedec(G, elim_ordering, T);
}

template <typename G_t, typename T_t>
void separator_algorithm_MSVS(G_t &G, T_t &T){
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }

    treedec::separator_algorithm(G, T);
    treedec::MSVS(G, T);
}

template <typename G_t, typename T_t>
void separator_algorithm_TM(G_t &G, T_t &T){
    if(boost::num_vertices(G) == 0){
        boost::add_vertex(T);
        return;
    }

    treedec::separator_algorithm(G, T);
    typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> old_elim_ordering;
    typename std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> new_elim_ordering;
    treedec::treedec_to_ordering<G_t, T_t>(T, old_elim_ordering);
    treedec::minimalChordal(G, old_elim_ordering, new_elim_ordering);
    T.clear();
    treedec::ordering_to_treedec(G, new_elim_ordering, T);
}

template <typename G_t, typename T_t>
void MSVS_trivial(G_t &G, T_t &T)
{
    if(boost::num_vertices(G) == 0){ untested();
        boost::add_vertex(T);
        return;
    }else{untested();
    }

    treedec::trivial_decomposition(G, T);
    treedec::MSVS(G, T);
}

template <typename G_t, typename T_t>
void PP_MD(G_t &G, T_t &T){ untested();
    int low = -1;
    PP_MD(G, T, low);
}

template <typename G_t, typename T_t>
void PP_FI(G_t &G, T_t &T){
    int low = -1;
    PP_FI(G, T, low);
}

template <typename G_t, typename T_t>
void PP_FI_TM(G_t &G, T_t &T){
    int low = -1;
    PP_FI_TM(G, T, low);
}

template <typename G_t, typename T_t>
void exact_decomposition_dynamic(G_t &G, T_t &T){
    int lb = -1;
    exact_decomposition_dynamic(G, T, lb);
}

template <typename G_t, typename T_t>
void exact_decomposition_cutset(G_t &G, T_t &T)
{ untested();
    int lb = -1;
    exact_decomposition_cutset(G, T, lb);
}

template <typename G_t, typename T_t>
void MD_MSVS(G_t &G, T_t &T){
    int low = -1;
    G_t H(G);
    treedec::minDegree_decomp(G, T);
    treedec::MSVS(H, T);
}

template <typename G_t, typename T_t>
void FI_MSVS(G_t &G, T_t &T)
{ untested();
    int low = -1;
    G_t H(G);
    treedec::fillIn_decomp(G, T);
    treedec::MSVS(H, T);
}

} //namespace treedec

#endif //TD_COMBINATIONS

// vim:ts=8:sw=4:et

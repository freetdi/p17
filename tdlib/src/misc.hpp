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
 * Offers miscellaneous algorithms about computing tree decompositions.
 *
 * These functions are most likely to be interesting for outside use:
 *
 * - bool is_valid_decomposition(G_t const& G, T_t const& T)
 * - void trivial_decomposition(G_t &G, T_t &T)
 * - int get_width(T_t &T)
 * - float get_average_bag_size(T_t &T)
 * - void make_small(T_t &T)
 * - void glue_decompositions(T_t &T1, T_t &T2)
 * - void glue_bag(typename treedec_traits<T_t>::bag_type &bag,
 *                 typename treedec_traits<T_t>::vd_type preprocessed_node, T_t &T)
 * - void glue_bags(std::vector< boost::tuple<
 *                   typename treedec_traits<T_t>::vd_type,
 *                   typename treedec_traits<T_t>::bag_type
 *                  > > &bags, T_t &T)
 *
 */

#ifndef TD_MISC
#define TD_MISC

#include <stack>
#include <queue>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "container.hpp"
#include "graph_traits.hpp"
#include "simple_graph_algos.hpp"
#include "platform.hpp"
#include "trace.hpp"
#include "treedec_traits.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

namespace treedec{

// Find a root of an acyclic graph T.
// Complexity: Linear in the number of vertices of T.
template <typename T_t>
typename boost::graph_traits<T_t>::vertex_descriptor find_root(T_t &T){
    typename boost::graph_traits<T_t>::vertex_descriptor t = *(boost::vertices(T).first);
    typename boost::graph_traits<T_t>::in_edge_iterator e, e_end;
    std::vector<BOOL> visited(boost::num_vertices(T), false);

    for(boost::tie(e, e_end)=boost::in_edges(t, T); e!=e_end;
        boost::tie(e, e_end)=boost::in_edges(t, T)){
        if(!visited[boost::source(*e, T)]){
            t = boost::source(*e, T);
            visited[t] = true;
        }
        else{
            //T is undirected
            return t;
        }
    }

    return t;
}

// TODO: deduplicate (see sethack.h)
template<class S, class T>
inline bool set_intersect(const S& s, const T& t)
{
    typename S::const_iterator it1 = s.begin();
    typename T::const_iterator it2 = t.begin();

    for(; it1!=s.end() && it2!=t.end();){
        if(*it1 == *it2){
            return true;
        }
        else if(*it1 < *it2){
            auto previous=*it1;
            it1++;
            assert(it1==s.end() || previous<*it1);
            (void)previous;
        }
        else{
            auto previous=*it2;
            it2++;
            assert(it2==t.end() || previous<*it2);
            (void)previous;
        }
    }
    return false;
}


template <typename T_t>
void postorder_traversal(T_t &T, std::stack<typename boost::graph_traits<T_t>::vertex_descriptor> &S)
{
    std::stack<typename boost::graph_traits<T_t>::vertex_descriptor> S_tmp;

    std::vector<BOOL> visited(boost::num_vertices(T), false);

    //The root can be chosen freely.
    typename boost::graph_traits<T_t>::vertex_descriptor root = treedec::find_root(T);
    S_tmp.push(root);
    visited[root] = true;

    while(!S_tmp.empty()){
        typename boost::graph_traits<T_t>::vertex_descriptor v = S_tmp.top();
        S_tmp.pop();
        S.push(v);

        typename boost::graph_traits<T_t>::adjacency_iterator nIt, nEnd;
        for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(v, T); nIt != nEnd; nIt++){
            if(!visited[*nIt]){
                S_tmp.push(*nIt);
                visited[*nIt] = true;
            }
        }
    }
}

template <typename T_t>
bool validate_connectivity(T_t &T){
    //Compute a postorder traversal.
    std::stack<typename boost::graph_traits<T_t>::vertex_descriptor> S;
    treedec::postorder_traversal(T, S);

    std::vector<BOOL> visited(boost::num_vertices(T), false);
    typename treedec_traits<T_t>::bag_type forgotten;

    while(true){
        typename boost::graph_traits<T_t>::vertex_descriptor cur = S.top();
        S.pop();
        visited[cur] = true;

        //Get the parent of cur.
        typename boost::graph_traits<T_t>::vertex_descriptor parent;
        if(!S.empty()){
            //This works because T is a tree (first check in caller func).
            typename boost::graph_traits<T_t>::in_edge_iterator eIt, eEnd;
            for(boost::tie(eIt, eEnd) = boost::in_edges(cur, T); eIt != eEnd; eIt++){
                if(!visited[boost::source(*eIt, T)]){
                    parent = boost::source(*eIt, T);
                }
            }
        }


        if(set_intersect(forgotten, bag(cur, T))){
#ifndef NDEBUG
            typename treedec_traits<T_t>::bag_type I;
            std::set_intersection(forgotten.begin(), forgotten.end(), bag(cur, T).begin(), bag(cur, T).end(), std::inserter(I, I.begin()));
            std::cerr << "[is_valid_treedecomposition]: vertices are not connected: " << std::endl;
            for(typename treedec_traits<T_t>::bag_type::iterator it = I.begin(); it != I.end(); it++){
                std::cerr << *it << std::endl;
            }
#endif
            return false;
        }
        else if(S.empty()){
            return true;
        }

        std::set_difference(bag(cur, T).begin(),
                            bag(cur, T).end(),
                            bag(parent, T).begin(),
                            bag(parent, T).end(),
                            std::inserter(forgotten, forgotten.begin()));
    }
}

namespace HACK{
// hmm, inefficient. should sort first
// but then, need mutable bags :/
// (e.g. in check_treedec below)
template <class X>
inline bool contains(std::vector<X> const& c, X const& v)
{
	typedef typename std::vector<X>::const_iterator it;

	for(it i=c.begin(); i!=c.end(); ++i){
		if(v == *i) return true;
	}
	return false;
}
template <class C>
inline bool contains(C const& c, typename C::value_type const& v)
{
	return(c.find(v) != c.end());
}
} // HACK


}
namespace treedec{

template <typename G_t>
bool is_tree(G_t const& G)
{
    auto nv=boost::num_vertices(G);
    auto ne=boost::num_edges(G);
    if(ne+1!=nv){
        return false;
    }else{
    }
    //This is a root if G is a tree.
    typename boost::graph_traits<G_t>::vertex_descriptor root = find_root(G);
    //This will not be exhaustive if 'root' is not a root.
    std::vector<BOOL> visited(nv, false);
    std::vector<std::set<typename boost::graph_traits<G_t>::vertex_descriptor> > components;
    components.resize(1);
    t_search_components(G, root, visited, components, 0);

    //root itself is not contained in components[0].
    return (components[0].size()+1 == nv);
}


/* Check if a tree decomposition is valid with respect to G.
 *
 *  0 = valid
 * -1 = (at least) not a tree
 * -2 = (at least) not all vertices covered (but a tree)
 * -3 = (at least) not all edges covered (but a tree and all vertices covered)
 * -4 = there exist vertices, coded in the bags of T, that are not connected in T
 *                           (but T is a tree and all edges/vertices are covered)
 * -5 = There's no vertex in T. (Also required if G is empty, by convention.)
 */

template <typename G_t, typename T_t>
int check_treedec(G_t const& G, T_t const& T)
{
    if(boost::num_vertices(T) == 0){
        //The empty graph has a treedecomposition with 1 vertex and an empty bag.
        return -5;
    }else{
    }

    if(!is_tree(T)){
#ifndef NDEBUG
        std::cerr << "[is_valid_treedecomposition]: treedecomposition is not a tree" << std::endl;
#endif
        return -1;
    }else{
    }

    //Checks if exactly the vertices of G are covered.
    std::set<typename treedec_traits<T_t>::bag_type::value_type> coded_vertices;
    typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
    for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){ itested();
        coded_vertices.insert(bag(*tIt, T).begin(),
                              bag(*tIt, T).end());
    }

    std::set<typename treedec_traits<T_t>::bag_type::value_type> vertices;
    typename boost::graph_traits<G_t>::vertex_iterator vIt, vEnd;
    for(boost::tie(vIt, vEnd) = boost::vertices(G); vIt != vEnd; vIt++){ itested();
        typename treedec_traits<T_t>::bag_type::value_type v;
        v = *vIt;
        insert(vertices, v);
    }

    if(coded_vertices == vertices){
    }else{
#ifndef NDEBUG
        std::cerr << "[is_valid_treedecomposition]: invalid vertices or not all vertices coded: " << std::endl;
        typename treedec_traits<T_t>::bag_type sym_diff;
        std::set_symmetric_difference(coded_vertices.begin(), coded_vertices.end(),
                                      vertices.begin(), vertices.end(),
                                      std::inserter(sym_diff, sym_diff.begin()));
        for(typename treedec_traits<T_t>::bag_type::iterator sIt=sym_diff.begin(); sIt != sym_diff.end(); sIt++){
            std::cerr << *sIt << " ";
        } std::cerr << std::endl;
#endif

        return -2;
    }

    //Check if all edges are covered.
    typename std::vector<typename treedec_traits<T_t>::bag_type> edges(boost::num_edges(G));
    for(boost::tie(vIt, vEnd) = boost::vertices(G); vIt != vEnd; vIt++){ itested();
        typename boost::graph_traits<G_t>::adjacency_iterator nIt, nEnd;
        for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(*vIt, G); nIt != nEnd; nIt++){
            if(*vIt >= *nIt){
                continue;
            }

            bool is_contained = false;
            for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
                typedef typename treedec_traits<T_t>::bag_type::value_type vd_type;
                vd_type v(*vIt);
                vd_type n(*nIt);

                if(HACK::contains(bag(*tIt, T), v) &&
                   HACK::contains(bag(*tIt, T), n)){
                    is_contained = true;
                    break;
                }
            }

            if(is_contained){
                // TODO: need tests, really!
            }else{
#ifndef NDEBUG
                std::cerr << "[is_valid_treedecomposition]: not all edges covered: " << std::endl;
                std::cerr << "    " << *vIt << "--" << *nIt << std::endl;
#endif
                return -3; //Not all edges are covered.
            }
        }
    }

    if(!validate_connectivity(T)){
        return -4;
    }else{
    }

    return 0;
}

/* Check if a tree decomposition is valid with respect to G. */
template <typename G_t, typename T_t>
bool is_valid_treedec(G_t const& G, T_t const& T)
{ itested();
#ifdef DEBUG
  auto e=boost::edges(T);
  std::cerr << "----\n";
  std::cerr << "nv " << boost::num_vertices(T) << "\n";
  for(;e.first!=e.second;++e.first){
	  std::cerr << boost::source(*e.first, T);
	  std::cerr << ":" <<  boost::target(*e.first, T) << "\n";
  }
#endif
    return !validate_treedecomposition(G, T);
}

template <typename G_t, typename T_t>
int validate_treedecomposition(G_t const& G, T_t const& T)
{
    return check_treedec(G, T);
}

template <typename G_t, typename T_t>
bool is_valid_treedecomposition(G_t const& G, T_t const& T){
    return (validate_treedecomposition(G, T) == 0);
}

template <typename G_t, typename T_t>
void trivial_decomposition(G_t const &G, T_t &T){
    typename boost::graph_traits<T_t>::vertex_descriptor t = boost::add_vertex(T);

    typename boost::graph_traits<G_t>::vertex_iterator vIt, vEnd;
    for(boost::tie(vIt, vEnd) = boost::vertices(G); vIt != vEnd; vIt++){
        typename treedec_traits<T_t>::bag_type::value_type v = *vIt;
        insert(bag(t, T), v);
    }
}


template <typename T_t>
inline size_t get_bagsize(T_t const &T){
    size_t max = 0;
    typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
    for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
        size_t bag_size = bag(*tIt, T).size();
        if(bag_size > max){
            max = bag_size;
        }
    }

    return (max);
}

// this is really... stupid.
template <typename T_t>
int get_width(T_t const &T){
    return get_bagsize(T)-1;
}

template <typename T_t>
float get_average_bag_size(T_t &T){
    float avg = 0.0;
    typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
    for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
        avg += bag(*tIt, T).size();
    }

    return (boost::num_vertices(T) > 0)? avg/boost::num_vertices(T) : 0.0;
}

template <typename T_t>
void make_small(T_t &T){
    while(true){
        bool modified = false;
        std::vector<typename boost::graph_traits<T_t>::vertex_descriptor > N;
        typename boost::graph_traits<T_t>::vertex_descriptor child, parent;

        typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
        for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
            typename boost::graph_traits<T_t>::adjacency_iterator nIt, nEnd;
            for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(*tIt, T); nIt != nEnd; nIt++){
                if(*tIt == *nIt){
                    continue;
                }
                if(std::includes(bag(*nIt, T).begin(), bag(*nIt, T).end(),
                                 bag(*tIt, T).begin(), bag(*tIt, T).end())){
                    child = *tIt;
                    parent = *nIt;

                    N.resize(boost::out_degree(*tIt, T)-1);
                    unsigned int c = 0;
                    typename boost::graph_traits<T_t>::adjacency_iterator nIt2, nEnd2;
                    for(boost::tie(nIt2, nEnd2) = boost::adjacent_vertices(*tIt, T); nIt2 != nEnd2; nIt2++){
                        if(*nIt2 != parent){
                            N[c++] = *nIt2;
                        }
                    }

                    modified = true;
                    break;
                }
            }
            if(modified){
                for(unsigned int i = 0; i < N.size(); i++){
                    boost::add_edge(parent, N[i], T);
                }

                boost::clear_vertex(child, T);
                boost::remove_vertex(child, T);
                break;
            }
        }
        if(!modified){
            return;
        }
    }
}


//Glues two "disjoint" decompositions (e.g. decompositions of two components of a graph)
template <typename T_t>
void glue_decompositions(T_t &T1, T_t const&T2){
    typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;

    //Copy T2 to T1 and add an edge from root to an arbitrary vertex of T2.
    std::vector<typename boost::graph_traits<T_t>::vertex_descriptor> idxMap(boost::num_vertices(T2));
    std::map<typename boost::graph_traits<T_t>::vertex_descriptor, unsigned int> vertex_map;
    unsigned int id = 0;
    for(boost::tie(tIt, tEnd) = boost::vertices(T2); tIt != tEnd; tIt++){
        idxMap[id] = boost::add_vertex(T1);
        vertex_map.insert(std::pair<typename boost::graph_traits<T_t>::vertex_descriptor, unsigned int>(*tIt, id));
        bag(idxMap[id++], T1) = bag(*tIt, T2);
    }

    typename boost::graph_traits<T_t>::edge_iterator eIt, eEnd;
    for(boost::tie(eIt, eEnd) = boost::edges(T2); eIt != eEnd; eIt++){
        typename std::map<typename boost::graph_traits<T_t>::vertex_descriptor, unsigned int>::iterator v, w;
        v = vertex_map.find(boost::source(*eIt, T2));
        w = vertex_map.find(boost::target(*eIt, T2));

        boost::add_edge(idxMap[v->second], idxMap[w->second], T1);
    }

    typename boost::graph_traits<T_t>::vertex_iterator tIt2, tEnd2;

    boost::tie(tIt, tEnd) = boost::vertices(T1);

    boost::add_edge(*tIt, idxMap[0], T1);
}


template <typename T_t>
void make_thick(T_t &T){
    unsigned int maxsize = (unsigned int) get_bagsize(T);
    bool modified = true;

    //Fill bags such that they all have size 'maxsize'.
    while(modified){
        modified = false;

        typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
        for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
            if((int)bag(*tIt, T).size() == maxsize){
                typename boost::graph_traits<T_t>::adjacency_iterator nIt, nEnd;
                for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(*tIt, T); nIt != nEnd; nIt++){
                    typename treedec_traits<T_t>::bag_type::iterator bIt = bag(*tIt, T).begin();
                    while(bag(*nIt, T).size() < maxsize){
                        bag(*nIt, T).insert(*(bIt++));
                        modified = true;
                    }
                }
            }
        }
    }

    modified = true;

    //Remove duplicated bags.
    while(modified){
        modified = false;

        typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
        for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
            typename boost::graph_traits<T_t>::adjacency_iterator nIt, nEnd;
            for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(*tIt, T); nIt != nEnd; nIt++){
                if(bag(*tIt, T) == bag(*nIt, T)){
                    typename boost::graph_traits<T_t>::adjacency_iterator nIt2, nEnd2;
                    for(boost::tie(nIt2, nEnd2) = boost::adjacent_vertices(*tIt, T); nIt2 != nEnd2; nIt2++){
                        if(*nIt2 != *nIt){
                            boost::add_edge(*nIt2, *nIt, T);
                        }
                    }
                    boost::clear_vertex(*tIt, T);
                    boost::remove_vertex(*tIt, T);
                    modified = true;

                    goto NEXT_ITER1;
                }
            }
        }
        NEXT_ITER1: ;
    }

    //Adjacent bags B1, B2 must fulfill |(B1 ^ B2)| = maxwidth-1.
    while(modified){
        modified = false;

        typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
        for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
            typename boost::graph_traits<T_t>::adjacency_iterator nIt, nEnd;
            for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(*tIt, T); nIt != nEnd; nIt++){
                typename treedec_traits<T_t>::bag_type intersection;
                std::set_intersection(bag(*tIt, T).begin(), bag(*tIt, T).end(),
                                      bag(*nIt, T).begin(), bag(*nIt, T).end(),
                                      std::inserter(intersection, intersection.begin()));

                if(intersection.size() != maxsize-1){
                    typename boost::graph_traits<T_t>::vertex_descriptor new_vertex = boost::add_vertex(T);
                    bag(new_vertex, T) = intersection;

                    typename treedec_traits<T_t>::bag_type::iterator bIt = bag(*tIt, T).begin();
                    while(bag(new_vertex, T).size() < maxsize){
                        bag(new_vertex, T).insert(*(bIt++));
                    }

                    boost::remove_edge(*tIt, *nIt, T);
                    boost::add_edge(*tIt, new_vertex, T);
                    boost::add_edge(new_vertex, *nIt, T);

                    modified = true;
                    goto NEXT_ITER2;
                }
            }
        }
        NEXT_ITER2: ;
    }
}

namespace detail{

//Converts a tree decomposition to a binary tree decomposition (all vertices have degree <= 2).
//Complexity: Linear in the number of vertices of T.
template <class T_t>
void make_binary(T_t &T, typename boost::graph_traits<T_t>::vertex_descriptor t, std::vector<BOOL> &visited){
    typename boost::graph_traits<T_t>::adjacency_iterator c, c_end;
    typename boost::graph_traits<T_t>::vertex_descriptor c0, c1;

    unsigned int cnt = 0;
    visited[t] = true;

    for(boost::tie(c, c_end) = boost::adjacent_vertices(t, T); c != c_end; c++){
        if(!visited[*c]){ cnt++; };
    }

    boost::tie(c, c_end) = boost::adjacent_vertices(t, T);

    switch(cnt){
        case 0:
            return;
        case 1:
            while(visited[*c]){ c++; }
            make_binary(T, *c, visited);
            return;
        case 2:
            break;
        default:
            while(visited[*c]){ c++; }
            c0 = *c++;
            while(visited[*c]){ c++; }
            c1 = *c;

            typename boost::graph_traits<T_t>::vertex_descriptor d = boost::add_vertex(T);
            boost::add_edge(d, c0, T);
            boost::add_edge(d, c1, T);

            boost::remove_edge(t, c0, T);
            boost::remove_edge(t, c1, T);

            bag(d, T) = bag(t, T);
            boost::add_edge(t, d, T);

            make_binary(T, t, visited);
            return;
    }

    while(visited[*c]){ c++; }
    c0 = *c++;
    while(visited[*c]){ c++; }
    c1 = *c;

    make_binary(T, c0, visited);

    if(bag(t, T) != bag(c0, T)){
        typename boost::graph_traits<T_t>::vertex_descriptor d = boost::add_vertex(T);
        boost::add_edge(d, c0, T);
        boost::add_edge(t, d, T);
        boost::remove_edge(t, c0, T);
        bag(d, T) = bag(t, T);
    }

    make_binary(T, c1, visited);

    if(bag(t, T) != bag(c1, T)){
        typename boost::graph_traits<T_t>::vertex_descriptor d = boost::add_vertex(T);
        boost::add_edge(d, c1, T);
        boost::add_edge(t, d, T);
        boost::remove_edge(t, c1, T);
        bag(d, T) = bag(t, T);
    }
}

} //namespace detail

template <typename T_t>
void make_binary(T_t &T){
    std::vector<BOOL> visited(boost::num_vertices(T), false);
    detail::make_binary(T, *boost::vertices(T).first, visited);
}

namespace detail{

//Complexity: O(|V(T)|)
template <typename T_undir_t, typename T_dir_t>
void make_rooted(T_undir_t &T, T_dir_t &T_,
                 typename boost::graph_traits<T_undir_t>::vertex_descriptor t,
                 std::vector<BOOL> &visited)
{
    visited[t] = true;
    typename boost::graph_traits<T_undir_t>::adjacency_iterator nIt, nEnd;
    for(boost::tie(nIt, nEnd) = boost::adjacent_vertices(t, T); nIt != nEnd; nIt++){
        if(!visited[*nIt]){
            boost::add_edge(t, *nIt, T_);
            make_rooted(T, T_, *nIt, visited);
        }
    }
}

template <typename T_undir_t, typename T_dir_t>
void make_rooted(T_undir_t &T, T_dir_t &T_,
                 typename boost::graph_traits<T_undir_t>::vertex_descriptor t)
{
    for(unsigned int i = 0; i < boost::num_vertices(T); i++){
        typename boost::graph_traits<T_undir_t>::vertex_descriptor new_vertex = boost::add_vertex(T_);
        bag(new_vertex, T_) = bag(i, T);
    }

    std::vector<BOOL> visited(boost::num_vertices(T), false);
    make_rooted(T, T_, t, visited);
}

} //namespace detail

template <typename T_undir_t, typename T_dir_t>
void make_rooted(T_undir_t &T, T_dir_t &T_)
{
    typename boost::graph_traits<T_undir_t>::vertex_descriptor t;
    t = *boost::vertices(T).first;

    detail::make_rooted(T, T_, t);
}

//Glues a single bag with the current tree decomposition T according to subset relation.
//Version used for preprocessing.
//
// elim_vertex is not an element of b
//
// BUG: this is inefficient. there's already a better implementation
// (where?)
template<typename T_t>
void glue_bag(
        typename treedec_traits<T_t>::bag_type &b,
        typename treedec_traits<T_t>::vd_type elim_vertex,
        T_t &T)
{
    // BOOST_STATIC_ASSERT(something_is_a_set);
    typedef typename boost::graph_traits<T_t>::vertex_descriptor t_vertex_descriptor;
    typename boost::graph_traits<T_t>::vertex_iterator vIt, vEnd;

    for(boost::tie(vIt, vEnd) = boost::vertices(T); vIt != vEnd; vIt++){
        if(std::includes(bag(*vIt, T).begin(),
                         bag(*vIt, T).end(),
                         b.begin(), b.end()))
        {
            // BUG, inefficient for vectors.
            if(!contains(bag(*vIt, T), elim_vertex)){
                push(b, elim_vertex);
                t_vertex_descriptor t_dec_node=boost::add_vertex(T);
                bag(t_dec_node, T) = MOVE(b);

                boost::add_edge(*vIt, t_dec_node, T);
            }else{ untested();
            }
            return;
        }else{ itested();
        }
    }

    //Case for a disconnected graph.
    t_vertex_descriptor t_dec_node = boost::add_vertex(T);
    push(b, elim_vertex);
    bag(t_dec_node, T) = MOVE(b);

    if(boost::num_vertices(T) > 1){
        boost::tie(vIt, vEnd) = boost::vertices(T);
        boost::add_edge(*vIt, t_dec_node, T);
    }
}

template <typename T_t>
void glue_two_bags(T_t &T,
      typename treedec_traits<T_t>::bag_type &bag1,
      typename treedec_traits<T_t>::bag_type &bag2)
{
    typename boost::graph_traits<T_t>::vertex_iterator vIt1, vIt2, vEnd;
    typename boost::graph_traits<T_t>::vertex_descriptor b1, b2;

    for(boost::tie(vIt1, vEnd) = boost::vertices(T); vIt1 != vEnd; vIt1++){
        if(bag(*vIt1, T) == bag1){
            b1 = *vIt1;
            break;
        }
    }

    for(boost::tie(vIt2, vEnd) = boost::vertices(T); vIt2 != vEnd; vIt2++){
        if(bag(*vIt2, T) == bag2){
            b2 = *vIt2;
            break;
        }
    }

    if(vIt1 != vEnd && vIt2 != vEnd){
        return;
    }

    if(vIt1 == vEnd){
        b1 = boost::add_vertex(T);
        bag(b1, T) = bag1;
    }

    if(vIt2 == vEnd){
        b2 = boost::add_vertex(T);
        bag(b2, T) = bag2;
    }

    boost::add_edge(b1, b2, T);
}

//Glues bags with the current tree decomposition.
// destroys bags
// BUG: what is V_t?
template<typename V_t, typename T_t>
void glue_bags(V_t& bags, T_t &T)
{
    for(unsigned int i = bags.size(); i > 0; i--){
        typename treedec_traits<T_t>::vd_type first = boost::get<0>(bags[i-1]);
        typename treedec_traits<T_t>::bag_type& second = boost::get<1>(bags[i-1]);
/*
        if(ignore_isolated_vertices && second.empty()){
            continue;
        }
*/
        glue_bag(second, first, T);
    }
}


namespace detail{

// S_ = "vdmap[S]"
// first argument: set of vertices in H.
// second argument: set of vdMap values...
template <typename G_t, typename VDSET, typename M_t>
void map_descriptors(std::set<typename boost::graph_traits<G_t>::vertex_descriptor> const &S,
                     VDSET &S_, G_t &H, M_t& vdMap)
{
    for(auto sIt : S) {
        unsigned int pos = get_pos(sIt, H);
        S_.insert(vdMap[pos]);
    }
}

template <typename G_t, typename S_t, typename B_t>
void map_descriptors_to_bags(S_t const &S, B_t &B)
{
    for(typename std::set<typename boost::graph_traits<G_t>::vertex_descriptor>::iterator sIt =
                  S.begin(); sIt != S.end(); sIt++)
    {
        typename treedec_traits<typename treedec_chooser<G_t>::type>::bag_type::value_type vd = *sIt;
        push(B, vd);
    }
}

} // detail

// transition
using detail::map_descriptors;
using detail::map_descriptors_to_bags;

static std::set<unsigned> emptyset___;

template <typename G_t, typename T_t, typename M_t>
void apply_map_on_treedec(T_t &T, G_t &G, M_t &vdMap,
        std::set<unsigned> = emptyset___)
{
    typename boost::graph_traits<T_t>::vertex_iterator tIt, tEnd;
    for(boost::tie(tIt, tEnd) = boost::vertices(T); tIt != tEnd; tIt++){
        typename treedec_traits<T_t>::bag_type bag_old, bag_new;
        bag_old = bag(*tIt, T);
        for(auto x : bag_old) {
            unsigned int pos = get_pos(x, G);
            bag_new.insert(vdMap[pos]);
        }
        bag(*tIt, T) = MOVE(bag_new);
    }
}

//Collect all subsets of [i,e) of size 'k' and store in 'subs'.
//pass "sub" to use custom stack space.
template <typename S, typename O>
void subsets(S i, S e, int k, O &subs,
        std::vector<typename S::value_type> *sub=NULL)
{
    if(!sub){
        sub = new std::vector<typename S::value_type>();
    }
    if(k==0){
        typename O::value_type subS;
        // check: is sub already ordered?!
        subS.insert(sub->begin(), sub->end());
        subs.push_back(subS);
        return;
    }

    for(; i!=e;){
        sub->push_back(*i);
        ++i;
        subsets(i, e, k-1, subs, sub);
        sub->pop_back();
    }
}

// wrapper...
template <typename S>
void subsets(S const &X, int size, int k, int idx,
        std::vector<S> &subs,
        std::vector<typename S::value_type> *sub=NULL)
{
    assert(idx==0);
    assert(unsigned(size)==X.size());

    return subsets(X.begin(), X.end(), k, subs, sub);
}

//wrapper. don't use.
template <typename S>
void subsets(S const &X, int size, int k, int idx,
        std::vector<typename S::value_type> &sub,
        std::vector<S> &subs)
{
    assert(!sub.size());
    return subsets(X, size, k, idx, subs, &sub);
}

#if 0 //TODO: not here.
template<class G, class B>
inline void detach_neighborhood(
        typename boost::graph_traits<G>::vertex_descriptor& c,
        G& g, B& bag)
{
    assert(boost::is_undirected(g));

    typename boost::graph_traits<G>::adjacency_iterator nIt1, nIt2, nEnd;
    // inefficient.

    for(boost::tie(nIt1, nEnd) = boost::adjacent_vertices(c, g); nIt1 != nEnd; nIt1++)
    {
        bag.insert(get_vd(g, *nIt1));
    }
    if(boost::is_directed(g)){ untested();
//        hack, do it differnetly.
        unreachable();
    }else{
        for(boost::tie(nIt1, nEnd) = boost::adjacent_vertices(c, g); nIt1 != nEnd; nIt1++)
        {
            bag.insert(get_vd(g, *nIt1));
        }
        boost::clear_vertex(c, g);
    }
}

#endif
} // namespace treedec

#endif //TD_MISC

// vim:ts=8:sw=4:et

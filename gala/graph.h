/* Copyright (C) 2015-2016 Felix Salfelder
 * Author: Felix Salfelder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * graph abstraction with low-level access
 */
#ifndef GALA_GRAPH_H
#define GALA_GRAPH_H

#include "trace.h"

#include <cstdlib>
#include <vector>
#include <map>
#include <forward_list>
#include <stx/btree_set.h>
#include <type_traits>
		
#include <boost/serialization/static_warning.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/tuple/tuple.hpp>

#include "degs.h"
#include "sfinae.h"
#include <assert.h>
#include <boost/mpl/bool.hpp>

/*--------------------------------------------------------------------------*/
#ifndef GALA_DEFAULT_OVERRIDE
# include <set>
# include <vector>
# define GALA_DEFAULT_SET std::set
# define GALA_DEFAULT_VECTOR std::vector
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace gala{
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct vertex_ptr_tag {}; // not in use yet.
/*--------------------------------------------------------------------------*/
#define galaPARMS    template<class T, typename... > class ECT, \
                     template<class T, typename... > class VCT, \
                     class VDP, \
                     template<class GG> class CFG
#define VCTtemplate template< galaPARMS >
#define SGARGS ECT, VCT, VDP, CFG
/*--------------------------------------------------------------------------*/
#define VCTtemplateP template< \
                     template<class T, typename... > class ECT, \
                     template<class T, typename... > class VCT \
                     template<class G> class CFG \
                     >
#define SGARGSP ECT, VCT, CFG
/*--------------------------------------------------------------------------*/
#define STPARMS    template<class T, typename... > class ECT, \
                   template<class T, typename... > class VCT, \
                   class VDP
#define STtemplate template< STPARMS >
#define STARGS ECT, VCT, VDP
/*--------------------------------------------------------------------------*/
struct directed_tag { };
struct undirected_tag { };
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<typename T>
struct tovoid { typedef void type; };
/*--------------------------------------------------------------------------*/
namespace bits{ //
/*--------------------------------------------------------------------------*/
template<class C, class E>
void my_push_back(C& c, E e)
{ untested();
	c.insert(e);
}
template<class E>
void my_push_back(std::vector<unsigned>& c, E e)
{ untested();
	// it's better not there yet (up to you).
	c.push_back(e);
}
template<class E>
void my_push_back(std::vector<short unsigned>& c, E e)
{ untested();
	c.push_back(e);
}
/*--------------------------------------------------------------------------*/
// TODO: move to bits header
template<class S, class X=void>
struct container_helper {
	template<class C, class E>
	static bool exists(C& c, E e) { itested();
		return c.find(e) != c.end();
	}
	template<class C, class E>
	static void add(C& c, E e) {
		bool done=c.insert(e).second;
		assert(done); (void)done;
	}
	template<class C, class E>
	static bool insert(C& c, E e) {
		bool done=c.insert(e).second;
		return done;
	}
	template<class C, class E>
	static bool remove(C& c, E e) { untested();
		return c.erase(e);
	}
};
template<class S>
struct container_helper<S, typename sfinae::is_vector<S>::type > {
	template<class C, class E>
	static bool exists(C&, E) { incomplete();
		return false;
	}
	template<class C, class E>
	static void add(C& c, E e){
		// not really, just TDLIB
		// assert(std::find(c.begin(), c.end(), e)==c.end());
		c.push_back(e);
	}
	template<class C, class E>
	static bool insert(C& c, E e){ itested();
		if(std::find(c.begin(), c.end(), e)==c.end()){ itested();
			c.push_back(e);
			return true;
		}else{
			return false;
		}
	}
	template<class C, class E>
	static bool remove(C& c, E e) {
		BOOST_STATIC_WARNING(false); // inefficient.
		auto what=std::find(c.begin(), c.end(), e);
		if(what==c.end()){
			return false;
		}else{
			c.erase(what);
			return true;
		}
	}
};
template<class S>
struct container_helper<S, typename sfinae::is_hash<S>::type > {
	template<class C, class E>
	static bool exists(C&, E)
	{ incomplete();
		return false;
	}
};
/*--------------------------------------------------------------------------*/
template<class C, class E>
bool edge_insert(C& c, E e)
{
	return container_helper<C>::insert(c, e);
}
#if 0
template<class E, class C>
bool edge_insert(C& c, E e, typename std::enable_if< sfinae::is_vector<C>::value, bool>::type=false)
{ untested();
	if(std::find(c.begin(), c.end(), e)==c.end()){ untested();
		c.push_back(e);
		return true;
	}else{ untested();
		return false;
	}
}
#endif
/*--------------------------------------------------------------------------*/
template<class C, class E>
void edge_add(C& c, E e)
{
	container_helper<C>::add(c, e);
}
#if 0
template<class E, class C>
bool edge_add(C& c, E e, typename std::enable_if< sfinae::is_vector<C>::value, bool>::type=false)
{ untested();
	c.push_back(e);
	return true;
}
#endif
/*--------------------------------------------------------------------------*/
// stub. maybe merge into some storage helper.
template<bool s,
         template<class T, typename... > class ECT,
         class X=void>
struct order_helper {
	template<class V>
	static void do_it(V&){
		// nop
	}
};
template<template<class T, typename... > class ECT>
struct order_helper<true, ECT,
	typename tovoid < typename std::enable_if<
	 sfinae::is_seq_tpl<ECT>::value
	>::type >::type > { //
	template<class V>
	static void do_it(V& v){
		for( auto& i : v){
			std::sort(i.begin(), i.end());
		}
	}
};
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class S, class X=void>
struct outedge_helper {
	template<class C, class V>
	static void erase(C& s, V w){ untested();
		s.erase(w);
	}
	template<class C, class E>
	bool exists(C& c, E e)
	{ itested();
		return c.find(e) != c.end();
	}
};
template<template<class T, typename... > class ECT>
struct outedge_helper<ECT, typename sfinae::is_vec_tpl<ECT>::type > {
	//this is inefficient. probably not what you need.
	//just for backwards compatibility
	template<class C, class V>
	static void erase(C& s, V w){ untested();
		for(auto& i: s){ untested();
			if(i==w){ untested();
				i=s.back();
				s.pop_back();
			}
		}
	}
	template<class C, class E>
	bool exists(C&, E)
	{ incomplete();
		return false;
	}
};
/*--------------------------------------------------------------------------*/
template<class VDP>
struct vertex_helper{ //
	template<class T, class V, class VC>
	static bool contains(T& v, V& w, VC*)
	{ itested();
		return container_helper<VC>::exists(v, w);
	}
	template<class T, class V, class VC>
	static void add(T& v, V& w, VC*)
	{
		// BUG? insert vs add.
		return container_helper<VC>::add(v, w);
	}
	template<class T, class V, class VC>
	static void insert(T& v, V& w, VC*)
	{ itested();
		edge_insert(v, w);
	}
	template<class T, class V, class VC>
	static bool remove(T& v, V& w, VC*)
	{ itested();
		return container_helper<VC>::remove(v, w);
	}
	template<class VL>
	static bool is_valid(VDP const& v, VL const& _v)
	{ itested();
		return v<_v.size();
	}
	template<class E, class S>
	static void rebase(E& e, S const& s, intptr_t /*delta*/)
	{ itested();
		e=s;
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<>
struct vertex_helper<vertex_ptr_tag>{ //
	template<class T, class V, class VC>
	static bool contains(T& v, V&, VC* wp)
	{ itested();
		incomplete();
		// return contains(v.n, wp);
		return container_helper<VC>::exists(v.n, wp);
	}
	template<class T, class V, class VC>
	static void insert(T& v, V&, VC* wp)
	{ itested();
		v.n.insert(wp);
	}
	template<class T, class V, class VC>
	static void add(T& v, V&, VC* wp)
	{ untested();
		container_helper<VC>::add(v.n, wp);
	}
	template<class T, class V, class VC>
	static bool remove(T&, V&, VC*)
	{ untested();
		incomplete();
		return false;
	}
	template<class VL>
	static bool is_valid(typename VL::value_type const* v, VL const& _v)
	{ itested();
		intptr_t B= intptr_t(v) - intptr_t(&*_v.begin());
		if(!std::is_same<std::random_access_iterator_tag,
		                 typename VL::iterator::iterator_category > :: value) { untested();
			return true; // don't know...
		}else if(B<0){untested();
			return false;
		}else if(B>=intptr_t(&*_v.end())) {untested();
			return false;
		}else if(B%sizeof(typename VL::value_type)) { untested();
			return false;
		}else{ itested();
			return true;
		}
	}
	template<class T, class S>
	static void rebase(T& tgt, S const& src, intptr_t delta)
	{ untested();
		tgt.clear();
		for(auto i : src) { untested();
#ifdef NOHINT_REBASE
			tgt.insert( typename T::value_type((intptr_t(i)+delta)));
#else // faster. check applicability
			auto e=tgt.end();
			tgt.insert(e, (typename T::value_type)((intptr_t(i)+delta)));
#endif
		}
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class c_iter_tag, class VDP, bool is_directed, bool is_multiedge>
struct iter_helper{ //

	template<class iter, class VL>
	static size_t fill_pos(iter first, iter last, VL& _v, bool dir=false,
			bool dups=true)
	{
		unsigned all=0;
		if(is_multiedge){ untested();
			if(is_directed){ untested();
			}else{ untested();
				if(dups){ untested();
				}else{ untested();
				}
			}
		}else{
			if(is_directed){ untested();
			}else{
			}
		}
		auto nv=_v.size(); (void)nv;
		trace1("fill_pos", nv);
		assert(!dir); (void) dir;
		size_t c=0;
		for(;first!=last; ++first){
			++all;
			unsigned v=(*first).first;
			unsigned w=(*first).second;
			trace2("fill_pos", v, w);
			assert(v<nv);
			assert(w<nv);
			// FIXME: use add_edge...
			bool doit;
			if(is_multiedge){ untested();
				// use template arg!
				doit=true;
			}else if(!dups){
				doit=true;
			}else if(vertex_helper<VDP>::contains(_v[v], w, &_v[w])){
				doit=false;
			}else{
				doit=true;
			}

			if(doit){
				vertex_helper<VDP>::add(_v[v], w, &_v[w]);
				vertex_helper<VDP>::add(_v[w], v, &_v[v]);
				++c;
			}else{
			}
		}
		return c;
	}

};
/*--------------------------------------------------------------------------*/
inline size_t source(std::pair<size_t, size_t> const& p){
	return p.first;
}
inline size_t target(std::pair<size_t, size_t> const& p){
	return p.second;
}
/*--------------------------------------------------------------------------*/
template<class c_iter_tag, class VDP, bool is_multiedge>
struct iter_helper<c_iter_tag, VDP, /*directed*/ true, is_multiedge> {

	template<class iter, class VL>
	static size_t fill_pos(iter first, iter last, VL& _v, bool dir=false, bool dups=true)
	{
		if(dups){
		}else{ untested();
		}
		assert(dir); (void) dir;
		auto nv = _v.size();
		size_t c=0;
		for(;first!=last; ++first){ itested();
			unsigned v=source(*first);
			unsigned w=target(*first);
			assert(v<nv);
			assert(w<nv);
			// FIXME: use add_edge...
			vertex_helper<VDP>::insert(_v[v], w, &_v[w]);
			++c;
		}
		return c;
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class VDP, bool is_directed, bool is_multiedge>
struct iter_helper<std::bidirectional_iterator_tag, VDP, is_directed, is_multiedge>{ //

template<class iter, class VL>
static size_t fill_pos(iter first, iter last, /* size_t nv, */ VL& _v, bool dir, bool dup=false)
{ untested();
	if(dup){incomplete();
	}
	if(dir){ incomplete();
	}
	typedef typename VL::value_type v_t;
	size_t nv=_v.size();
	if(nv){ untested();
	}else{ untested();
	}
	GALA_DEFAULT_VECTOR<v_t*> vmap(_v.size());
	size_t index=0;
	for(auto& i : _v){ untested();
		vmap[index++] = &i;
	}
	assert(index==_v.size());
	size_t c=0;
	for(;first!=last; ++first){ untested();
		unsigned v=first->first;
		unsigned w=first->second;
		assert(v<nv);
		assert(w<nv);
		v_t* _V = vmap[v];
		v_t* _W = vmap[w];
		v_t& V = *_V;
		v_t& W = *_W;
		vertex_helper<VDP>::insert(V, W, _W);
		if(is_directed){ untested();
			// hmm.
		}else{ untested();
			vertex_helper<VDP>::insert(W, V, _V);
		}
		++c;
	}
	return c;
}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// select vertex type depending on 3rd template parameter
template<template<class T, typename... > class ECT, class VDP>
struct vertex_selector{ //
	typedef VDP type;
	typedef const VDP const_type;
	typedef ECT<type> stype;
	typedef VDP vertices_size_type;
	typedef size_t edges_size_type; // TODO.
	typedef unsigned vertex_index_type; // INCOMPLETE
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template< template<class T, typename... > class ECT >
struct vertex_selector<ECT, vertex_ptr_tag>{ //
	typedef struct vertex_{ //
		ECT<vertex_*> n;
		size_t size() const { return n.size(); }
	} vertex_;
	typedef vertex_* type;
	typedef vertex_ const* const_type;
	typedef vertex_ stype; // storage type
	typedef uintptr_t vertices_size_type;
	typedef size_t edges_size_type; // TODO.
	typedef unsigned vertex_index_type; // INCOMPLETE
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// part of storage? not sure yet.
STtemplate
struct rewire_helper{
	template<class A, class B>
	static void rewire_nodes(A& new_vl, B offset)
	{
		// specialize... (will be expensive)
		assert( sfinae::is_vec_tpl< VCT >::value );
		trace1("rewire", offset);
		for(auto& node : new_vl){
	//		node.n.insert(NULL); ??
			auto& N = node.n;
			for(auto& neigh : N) {
				*(const_cast<typename A::value_type**>(&neigh)) =
						(typename A::value_type*) ( uintptr_t(neigh) + uintptr_t(offset));
			}
		}
	}
};
/*--------------------------------------------------------------------------*/
STtemplate
struct storage_base{ //
	typedef vertex_selector<ECT,VDP> vs;
	typedef typename vs::type vertex_type;
	typedef typename vs::stype vertex_;
	typedef VCT<vertex_> container_type;
	typedef typename vs::vertex_index_type vertex_index_type;
	static vertex_index_type num_edges(vertex_index_type n, container_type const&)
	{ untested();
		return n;
	}
};
/*--------------------------------------------------------------------------*/
template<class vertex_type>
struct edge_type_ : std::pair<vertex_type, vertex_type>{
	typedef std::pair<vertex_type, vertex_type> base;
	template<class vt, class vt2>
	edge_type_(vt a, vt2 b) : base(a,b) {
	}
	edge_type_() : base() {
	}
};
/*--------------------------------------------------------------------------*/
STtemplate
struct storage : storage_base<STARGS>{ //
	using typename storage_base<STARGS>::vs;
	using typename storage_base<STARGS>::vertex_;
	using typename storage_base<STARGS>::vertex_type;
	using typename storage_base<STARGS>::container_type;
	typedef typename vs::vertex_index_type vertex_index_type;
	typedef typename vs::const_type const_vertex_type;
	typedef edge_type_<vertex_type> edge_type;
	typedef ECT<vertex_type> edge_container_type; // -> storage base?
	typedef VCT<vertex_> VL;
	static bool need_rewire()
	{ untested();
		return false;
	}
	static vertex_type new_node(container_type& _v)
	{
		vertex_ n;
		auto s=_v.size();
		_v.push_back(n);
		return s;
	}
	static VDP degree(const VDP v, container_type const& _v)
	{ itested();
		return _v[v].size();
	}
	static VDP degree(container_type const& _v)
	{ untested();
		vertex_index_type ret=0;
		for(auto const& e : _v){ untested();
			vertex_index_type d = e.size();
			if(d>ret) ret=d;
		}
		return ret;
	}
	static void clear_vertex(const VDP v, container_type& _v)
	{ untested();
		_v[v].clear();
	}
	static edge_container_type& out_edges(vertex_type& v, container_type& _v)
	{ itested();
		return _v[v];
	}
	static edge_container_type& out_edges(const_vertex_type& v, container_type& _v)
	{
		return _v[v];
	}
	static const edge_container_type& out_edges(const_vertex_type& v, const container_type& _v)
	{ itested();
		return _v[v];
	}
	static void remove_edge_single(vertex_index_type v, vertex_index_type w,
	                         container_type& _v)
	{ untested();
		outedge_helper<ECT>::erase(out_edges(v, _v), w);
	}
	static void add_pos_edge(vertex_index_type v, vertex_index_type w,
	                         container_type& _v)
	{ untested();
		assert(v<w); // for now
		_v[v].insert(w);
		_v[w].insert(v);
	}
	template<class E, class S>
	static void rebase(E& e, S const& s, intptr_t /*delta*/)
	{ incomplete();
		e = s;
	}

}; // storage<VDP>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class ECT,
         template<class T, typename... > class VCT>
struct storage<ECT, VCT, vertex_ptr_tag> : public storage_base<ECT, VCT, vertex_ptr_tag>{ //
	typedef vertex_ptr_tag VDP;
	typedef typename bits::vertex_selector<ECT,VDP>::stype vertex_;
	typedef typename bits::vertex_selector<ECT,VDP>::type vertex_type;
	typedef typename bits::vertex_selector<ECT,VDP>::vertex_index_type vertex_index_type;
	typedef typename bits::vertex_selector<ECT,VDP>::const_type const_vertex_type;
	typedef edge_type_<vertex_type> edge_type;
	typedef VCT<vertex_> container_type;
	typedef ECT<vertex_type> edge_container_type;
	typedef VCT<vertex_> VL;
	static bool need_rewire()
	{ untested();
		return true;
	}
	static vertex_type new_node(container_type& _v)
	{ itested();
		unsigned int s = _v.size();
		auto old_begin=_v.begin();
		_v.resize(s+1);
		vertex_type offset = (vertex_type) (uintptr_t(&*_v.begin()) - uintptr_t(&*old_begin));
		if(!offset){
		}else{
			// std::cerr << "add rewire " << _v.size() << "\n";
			rewire_helper<STARGS>::rewire_nodes(_v, offset);
		}
		return &_v.back();
	}
	static vertex_index_type degree(const vertex_type& v, container_type const&){ // ??
		return v->n.size();
	}
	static vertex_index_type degree(const_vertex_type& v, container_type const&){
		return v->n.size();
	}
	static vertex_index_type degree(container_type const& _v)
	{ untested();
		vertex_index_type ret=0;
		typename container_type::const_iterator v = _v.begin();
		typename container_type::const_iterator e = _v.end();
		for(; v!=e ; ++v){ untested();
			vertex_index_type d = v->n.size();
			if(d>ret) ret=d;
		}
		return ret;
	}
	static void clear_vertex(const vertex_type v, container_type&)
	{ untested();
		v->n.clear();
	}
	static edge_container_type& out_edges(vertex_& v, container_type&)
	{
		return v.n;
	}
	static const edge_container_type& out_edges(vertex_& v, const container_type&)
	{ untested();
		return v.n;
	}
	static edge_container_type& out_edges(vertex_type& v, container_type&)
	{
		return v->n;
	}
	static const edge_container_type& out_edges(const_vertex_type& v, const container_type&)
	{ untested();
		return v->n;
	}
public:
	static void remove_edge_single(vertex_type v, vertex_type w,
	                         container_type& _v)
	{ untested();
		out_edges(v, _v).erase(w);
	}
	static void add_pos_edge(vertex_index_type v, vertex_index_type w,
	                         container_type& _v)
	{ untested();
		assert(v<w); // for now
		_v[v].n.insert(&_v[w]);
		_v[w].n.insert(&_v[v]);
	}
	// copy over outedges.
	template<class T, class S>
	static void rebase(T& tgt, S const& src, intptr_t delta)
	{ incomplete();
		tgt.clear();
		auto e=tgt.end();
		for(auto i : src) { untested();
			tgt.insert( (vertex_*) (intptr_t(i)+delta), e);
		}
	}
}; //storage<vertex_ptr_tag>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<STPARMS, bool is_directed, bool is_simple>
struct edge_helper : public storage<STARGS> {
};
/*--------------------------------------------------------------------------*/
template<STPARMS>
struct edge_helper<STARGS, false, false> : public storage<STARGS>{
	// undirected implementation.
	using typename storage<STARGS>::edge_type;
	using typename storage<STARGS>::vertex_type;
	using storage<STARGS>::out_edges;
	// to storage base?!
	template<class N, class VC>
	static std::pair<edge_type, bool> add_edge(vertex_type a, vertex_type b,
	                                           N& num_edges, VC& vc)
	{ untested();
		// trace0("undiredted add_edge");
		vertex_type* A=&a;
		vertex_type* B=&b;
		// don't attempt to avoid dups
		edge_add(out_edges(*B, vc), *A);
		edge_add(out_edges(*A, vc), *B);
		++num_edges;
		return std::make_pair(edge_type(*A, *B), true);
	}

	typedef typename storage<STARGS>::container_type vertex_container_type;
	template<class E>
	static void add_reverse_edges(vertex_container_type& /*_v*/, E& )
	{ unreachable();
	}
};
/*--------------------------------------------------------------------------*/
template<STPARMS>
struct edge_helper<STARGS, false, true> : public storage<STARGS>{
	// undirected implementation.
	using typename storage<STARGS>::edge_type;
	using typename storage<STARGS>::vertex_type;
	using storage<STARGS>::out_edges;
	// to storage base?!
	template<class N, class VC>
	static std::pair<edge_type, bool> add_edge(vertex_type a, vertex_type b,
	                                           N& num_edges, VC& vc)
	{

		// trace0("undiredted add_edge");
		vertex_type* A=&a;
		vertex_type* B=&b;
		size_t s = out_edges(*A, vc).size();
		edge_insert(out_edges(*A, vc), (*B));
		// since the graph is undirected,
		// if (a, b) already exists, then (b, a) does too
		bool added=false;
		if(s == out_edges(*A, vc).size()){
		}else{
			edge_add(out_edges(*B, vc), *A);
			++num_edges;
			added = true;
		}
		return std::make_pair(edge_type(*A, *B), added);
	}

	typedef typename storage<STARGS>::container_type vertex_container_type;
	template<class E>
	static void add_reverse_edges(vertex_container_type& /*_v*/, E& )
	{ unreachable();
	}
};
/*--------------------------------------------------------------------------*/
template<STPARMS, bool is_simple>
struct edge_helper<STARGS, true, is_simple>
	// directed implementation.
    : public storage<STARGS>{ //
	typedef typename storage<STARGS>::container_type vertex_container_type;
	using typename storage<STARGS>::edge_type;
	using typename storage<STARGS>::vertex_type;
	using storage<STARGS>::out_edges;
	template<class N, class VC>
	static std::pair<edge_type, bool> add_edge(vertex_type a, vertex_type b,
															 N& num_edges, VC& vc)
	{ itested();
		vertex_type* A=&a;
		vertex_type* B=&b;
		size_t s = out_edges(*A, vc).size();
		edge_insert(out_edges(*A, vc), *B);

		bool added=false;
		if(s == out_edges(*A, vc).size()){ itested();
		}else{ itested();
			++num_edges; // BUG, multiplier?
			added = true;
		}
		return std::make_pair(edge_type(*A, *B), added);
	}
};
/*--------------------------------------------------------------------------*/
template<STPARMS, class X=void>
struct reverse_helper : public storage<STARGS> { //

// 	typedef typename storage<STARGS>::container_type vertex_container_type;
// 	using typename storage<STARGS>::edge_type;
// 	using typename storage<STARGS>::vertex_type;

	//typedef foo incomplete;

//	template<class E>
//	static void make_symmetric(vertex_container_type& , E& , bool /*oriented*/)
//	{ untested();
//		// base case. hmm better don't use yet.
//
//		assert(false);
//		incomplete();
//	}
};
/*--------------------------------------------------------------------------*/
#if 0 // see below
template< template<class T, typename... > class ECT,
          template<class T, typename... > class VCT>
struct reverse_helper<ECT, VCT, vertex_ptr_tag,
	typename sfinae::is_set_tpl<ECT>::type >
	: public storage<ECT, VCT, vertex_ptr_tag> { //
};
#endif
/// hmm better constexpr flags for
// - directed
// - oriented
// - loopless?
template< template<class T, typename... > class ECT,
          template<class T, typename... > class VCT, class VDP>
struct reverse_helper<ECT, VCT, VDP, typename tovoid < typename sfinae::is_set_tpl<ECT>::type > :: type  >
	: public storage<ECT, VCT, VDP
//	typename std::enable_if< std::is_unsigned< VDP >::value, VDP >::type 
	> { //
	typedef typename storage<ECT,VCT,VDP>::container_type vertex_container_type;
	using typename storage<ECT,VCT,VDP>::edge_type;
	using typename storage<ECT,VCT,VDP>::vertex_type;

//	typename sfinae::is_set_tpl<ECT>::type BLA;
	typedef typename sfinae::is_set< ECT<sfinae::any> >::type hmm;

	template<class E>
	static void make_symmetric(vertex_container_type& _v, E& e, bool /*oriented*/)
	{ itested();
		unsigned ii=0;
		for(auto & i : _v){ itested();
			for(auto & j : i){ itested();
				bool ins=_v[j].insert(ii).second;
//				trace2("",e,ins);
				e+=ins;
			}
			++ii;
		}
	}
};
/*--------------------------------------------------------------------------*/
template<STPARMS>
struct reverse_helper<ECT, VCT, VDP,
//	typename std::enable_if< std::is_unsigned< VDP >::value, ECT<VDP> >::type,
	typename sfinae::is_seq_tpl< ECT >::type >
	: public storage<ECT, VCT, VDP > { //

	typedef typename sfinae::is_seq< ECT<sfinae::any> >::type hmm;
	typedef typename std::enable_if< std::is_unsigned< VDP >::value, VDP>::type hmm2;

	typedef typename storage<ECT,VCT,VDP>::container_type vertex_container_type;
	using typename storage<ECT,VCT,VDP>::edge_type;
	using typename storage<ECT,VCT,VDP>::vertex_type;

	template<class E>
	static void make_symmetric(vertex_container_type& _v, E& e, bool oriented)
	{
		// oriented: "there is only one edge {u,v}"
		trace3("make_symmetric", oriented, e, _v.size());
#ifndef NDEBUG
		auto ebefore=e;
#endif

		std::vector<vertex_type> howmany(_v.size());
		vertex_type i=0;
		unsigned checksum=0;
		for(auto& vertex : _v){
			howmany[i++] = vertex.size();
			checksum+= vertex.size();
		}
		trace4("make_symmetric", oriented, e, checksum, _v.size());
		assert(checksum==e || !oriented);
		// ssg only. for now.
		if(oriented){ untested();
			vertex_type vpos=0;
			for(unsigned j=0; j<_v.size(); ++j){ itested();
				auto K = _v[j].begin();
				for(unsigned i=0; i<howmany[j]; ++i ){ itested();
					my_push_back(_v[*K],(j));
					++K;
					++e;
				}
				++vpos;
			}
		}else{
//		BUG: this is inefficient if
			vertex_type vpos=0;
			for(unsigned j=0; j<_v.size(); ++j){
				auto K = _v[j].begin();
				for(unsigned i=0; i<howmany[j]; ++i ){
					e += edge_insert(_v[*K], j);
					++K;
				}
				++vpos;
			}

		}

#ifndef NDEBUG
		trace3("make_symm", oriented, ebefore, e);
		if(oriented){
			assert(ebefore*2==e);
		}else{
		}
#endif
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class ECT,
         template<class T, typename... > class VCT >
struct reverse_helper<ECT, VCT, vertex_ptr_tag,
  typename sfinae::is_set_tpl< ECT >::type
>
	: public storage<ECT, VCT, vertex_ptr_tag > { //
		typedef typename sfinae::is_set< ECT<sfinae::any> >::type hmm;
	typedef typename storage<ECT, VCT, vertex_ptr_tag>::container_type vertex_container_type;
	using typename storage<ECT, VCT, vertex_ptr_tag>::edge_type;
	using typename storage<ECT, VCT, vertex_ptr_tag>::vertex_type;

	template<class E>
	static void make_symmetric(vertex_container_type& _v, E& e, bool /*oriented*/)
	{ untested();

		// concept check?
		auto B=_v[0];
		auto I=B.n.begin();

		for(auto & i : _v){ untested();
			for(auto & j : i.n){ itested();
				bool ins=j->n.insert(&i).second;
				e+=ins;
			}
		}
	}
};
/*--------------------------------------------------------------------------*/
// vector outedge set ...?
template<template<class T, typename... > class ECT,
         template<class T, typename... > class VCT>
struct reverse_helper<ECT, VCT, vertex_ptr_tag,
  typename sfinae::is_seq< ECT<sfinae::any> >::type
	> : public storage<ECT, VCT, vertex_ptr_tag > { //
	typedef vertex_ptr_tag VDP;
	typedef typename storage<ECT,VCT,VDP>::container_type vertex_container_type;
	using typename storage<ECT,VCT,VDP>::edge_type;
	using typename storage<ECT,VCT,VDP>::vertex_type;

	template<class E>
	static void make_symmetric(vertex_container_type& /*_v*/, E& , bool /*oriented*/)
	{ incomplete();
//		typename VDP::dbg foo;
///		typedef typename
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
STtemplate
struct iter{ //
	typedef boost::counting_iterator<VDP> vertex_iterator;
	typedef boost::counting_iterator<VDP> const_vertex_iterator; // ?
	static vertex_iterator vbegin(typename storage<STARGS>::VL&)
	{
		return vertex_iterator(0);
	}
	static const_vertex_iterator vbegin(const typename storage<STARGS>::VL&)
	{
		return vertex_iterator(0);
	}
	static vertex_iterator vend(typename storage<STARGS>::VL& v)
	{
		return vertex_iterator(v.size());
	}
	static const vertex_iterator vend(const typename storage<STARGS>::VL& v)
	{
		return vertex_iterator(v.size());
	}
	static VDP deref(vertex_iterator v)
	{ itested();
		return *v;
	}
	// FIXME: is this "iter"?
	typedef typename bits::vertex_selector<ECT,VDP>::type vertex_type;
	typedef typename bits::vertex_selector<ECT,VDP>::stype vertex_;
	typedef VCT<vertex_> container_type;
	static unsigned pos(const vertex_type v, container_type const&)
	{
		return v;
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class ECT,
         template<class T, typename... > class VCT>
struct iter<ECT, VCT, vertex_ptr_tag>{ //
	typedef vertex_ptr_tag VDP;
	typedef typename storage<STARGS>::VL::iterator vertex_iterator;
	typedef typename storage<STARGS>::VL::const_iterator const_vertex_iterator;
	typedef typename vertex_selector<ECT,VDP>::type vertex_type;
	typedef typename vertex_selector<ECT,VDP>::const_type const_vertex;

	static const_vertex_iterator vbegin(typename storage<STARGS>::VL const& v)
	{ untested();
		return v.begin();
	}
	static vertex_iterator vbegin(typename storage<STARGS>::VL& v)
	{
		return v.begin();
	}
	static const_vertex_iterator vend(typename storage<STARGS>::VL const& v)
	{
		return v.end();
	}
	static vertex_iterator vend(typename storage<STARGS>::VL& v)
	{
		return v.end();
	}
	static vertex_type deref(vertex_iterator& v)
	{
		return &*v;
	}
	// needed?
	static const vertex_type deref(const vertex_iterator& v)
	{
		return &*v;
	}
	static const_vertex deref(const_vertex_iterator& v)
	{ untested();
		return &*v;
	}

// FIXME: is this "iter"?
// BUG:: type?
// fixme: VDP!=vector?!
	typedef typename bits::vertex_selector<ECT,VDP>::stype vertex_;

	typedef VCT<vertex_> container_type;
	static unsigned /*hmmm*/ pos(const vertex_type v, container_type const& _v)
	{
		vertex_ const* B = &*_v.begin();
		vertex_ * const V = v;
		intptr_t delta = intptr_t(V) - intptr_t(B);
		assert(delta>=0); // fails for sl. (incomplete).
		unsigned r=delta/sizeof(vertex_);
		assert(r<_v.size());
		return r;
	}
}; // iter<vertex_ptr_tag>
}// bits
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class C>
void prealloc(C const&, size_t /*howmany*/)
{
}
/*--------------------------------------------------------------------------*/
// typedef vertex_ptr_tag use_pointers;
/*--------------------------------------------------------------------------*/
template<class G>
struct graph_cfg_default;
/*--------------------------------------------------------------------------*/
namespace detail{
template<class CFG, class X=void>
struct is_directed_select {
#if 0
	typedef boost::mpl::false_ type; // obsolete. don't use.
#endif
	static constexpr bool value=false;
	operator bool() const {return false;}
};
template<class CFG>
struct is_directed_select<CFG,
	typename tovoid < typename std::enable_if< CFG::is_directed >::type >::type >
{ //
	static constexpr bool value=CFG::is_directed;
};
/*--------------------------------------------------------------------------*/
template<class CFG, template<class x, class ...> class ECT, class X=void>
struct is_ordered_select {
	// "sets" are ordered for now.
	// careful with hashsets...
	static constexpr bool value=sfinae::is_set_tpl<ECT>::value;
	operator bool() const {return value;}
};
template<class CFG, template<class x, class ...> class ECT>
struct is_ordered_select<CFG, ECT,
	typename tovoid < typename std::enable_if< CFG::force_ordering >::type >::type >
{ //
	static constexpr bool value=
		CFG::force_ordering || sfinae::is_set_tpl<ECT>::value;
};
/*--------------------------------------------------------------------------*/
template<class CFG, template<class x, class ...> class ECT, class X=void>
struct is_simple_select {
	// static constexpr bool value=sfinae::is_set_tpl<ECT>::value; FUTURE DEFAULT
	static constexpr bool value=true;
	operator bool() const {return value;}
};
template<class CFG, template<class x, class ...> class ECT>
struct is_simple_select<CFG, ECT,
	typename tovoid < typename std::enable_if<CFG::force_simple> >::type >
{ //
	static constexpr bool value=
		CFG::force_simple || sfinae::is_set_tpl<ECT>::value;
	operator bool() const {return value;}
};
/*--------------------------------------------------------------------------*/
template<class CFG, class X=void>
struct is_symmetric_select {
	static constexpr bool value=false;
	operator bool() const {return value;}
};
template<class CFG>
struct is_symmetric_select<CFG,
	typename tovoid < typename std::enable_if< CFG::force_symmetric >::type >::type >
{ //
	static constexpr bool value=CFG::force_symmetric;
};
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class ECT,
         class X=void>
struct is_multiedge_select {
	// BUG, contradicts add_edge...
	static constexpr bool value=true;
};
template<template<class T, typename... > class ECT>
struct is_multiedge_select<ECT,
	typename tovoid<typename sfinae::is_set_tpl<ECT>::type>::type>
{ //
	static constexpr bool value=false;
};
/*--------------------------------------------------------------------------*/
// temporary hack. use proper maps later
template<STPARMS, class X=void>
struct is_nn {
	static constexpr bool value=sfinae::is_vec_tpl<VCT>::value;
};
template<template<class T, typename... > class ECT,
         template<class T, typename... > class VCT>
struct is_nn<ECT, VCT, vertex_ptr_tag> {
	// incomplete: this is not what it is meant to be...
	static constexpr bool value=false;
};
#if 0
template<STPARMS>
struct is_nn<STARGS,
	typename std::enable_if<sfinae::is_vec_tpl<VCT>::value >::type >
{ //
	static constexpr bool value=true;
};
#endif
// struct gala::detail::is_nn<uset, std::vector, gala::vertex_ptr_tag, void>
/*--------------------------------------------------------------------------*/
template<class Gsrc, class Gtgt, bool srcDir, bool tgtDir,
	bool srcCont=false, bool tgtCont=false,
	bool srcOrd=false, bool tgtOrd=false>
struct copy_helper{
	static void assign(Gsrc const&, Gtgt&);
};
/*--------------------------------------------------------------------------*/
template<class Gsrc, class Gtgt, class X=void>
struct move_helper{
	static void move(Gsrc const&& s, Gtgt& t){ untested();

		t._v=std::move(s._v);

		if(!s.is_symmetric() && t.is_symmetric()){ untested();
			if(t.is_simple){ untested();
				t._num_edges=0;
				symmetrify(t);
			}else{ incomplete();
			}
		}else{untested();
			t._num_edges = s._num_edges;
		}
	}
	static void symmetrify(Gtgt& g){
		// add (b,a) for (a,b)
		// then check if all edges are simple,
		// sets? hmm x is not ordered...
		size_t i=0;
		for(auto& x : g._v){ itested();
			for(auto y : x){ itested();
				// HACK
				assert(i!=y); // no self loops.
				trace2("rev", y, i);
				g._v[y].push_back(i);
			}
			++i;
		}
		for(auto& x : g._v){ itested();
			std::sort(x.begin(), x.end()); // for now.
			x.erase( unique( x.begin(), x.end() ), x.end() );
			g._num_edges += x.size();
			trace1("", x.size());
		}

		if(g.is_directed()){
		}else{untested();
			trace1("correcting edgecount", g._num_edges);
			g._num_edges/=2;
		}
	}
};
/*--------------------------------------------------------------------------*/
} // namespace detail
/*--------------------------------------------------------------------------*/
template< template<class T, typename... > class ECT=GALA_DEFAULT_SET,
          template<class T, typename... > class VCT=GALA_DEFAULT_VECTOR,
          typename VDP=vertex_ptr_tag, // use_pointers,
          template<class G> class CFG=graph_cfg_default>
class graph{ //
	BOOST_STATIC_ASSERT( (std::numeric_limits<VDP>::is_integer
	                 && !std::numeric_limits<VDP>::is_signed )
						  || std::is_same<VDP, vertex_ptr_tag>::value );
public: // types
	using this_type = graph<SGARGS>;

	template<class T>
	using vertex_container_t = VCT<T>;

	template<class T>
	using edgecontainer = ECT<T>;
	
	using vs = bits::vertex_selector<ECT,VDP>;

public: // BUG. private & helper friends..
	typedef CFG<this_type> myCFG;
	static constexpr bool is_directed_v=detail::is_directed_select<myCFG>::value;
	static constexpr bool is_symmetric_v=!is_directed_v || detail::is_symmetric_select<myCFG>::value;
	static constexpr bool is_ordered_v=detail::is_ordered_select<myCFG, ECT>::value;
	static constexpr bool is_simple_v=detail::is_simple_select<myCFG, ECT>::value;
	static constexpr bool is_multiedge_v=!detail::is_simple_select<myCFG, ECT>::value;

public:
	static constexpr bool is_directed() {
		return is_directed_v;
	}
	// indicate that edge iterators are ordered.
	static constexpr bool is_ordered() {
		return is_ordered_v;
	}
	// indicate that there are no parallel edges
	static constexpr bool is_simple() { //
		return is_simple_v;
	}
	static constexpr bool is_symmetric() { //
		return is_symmetric_v;
	}
	// is_multiGRAPH?!
	static constexpr bool is_multiedge() { //
		return is_multiedge_v;
	}

	static constexpr bool is_nn_v=detail::is_nn<ECT, VCT, VDP>::value;

	typedef typename vs::type vertex_type;
	typedef typename vs::const_type const_vertex_type;
	typedef typename vs::stype vertex_;
	typedef typename vs::vertices_size_type vertices_size_type;
#if 0
	typedef typename vs::edges_size_type edges_size_type;
#else
	typedef size_t edges_size_type; // ??
#endif
	typedef typename vs::vertex_index_type vertex_index_type;
// private: hmm not yet.
	using storage=bits::storage<STARGS>;
	using edge_helper=bits::edge_helper<STARGS, is_directed_v, is_simple_v>;

	typedef typename bits::reverse_helper<STARGS> reverse_helper;
	typedef typename bits::iter<STARGS> iter;
	typedef edgecontainer<vertex_type> EL;

public: // reconfig
	template<template<class G> class new_config>
	struct reconfig{
		typedef graph<ECT, VCT, VDP, new_config> type;
	};
	template<class GG>
	struct my_undirected_config : public CFG<GG> {
		static constexpr bool is_directed=false;
	};
	template<class GG>
	struct my_symmetric_config : public CFG<GG> {
		static constexpr bool force_symmetric=true;
	};
	template<class GG>
	struct my_unsymmetric_config : public CFG<GG> {
		static constexpr bool force_symmetric=false;
	};

	typedef typename reconfig<my_undirected_config>::type undirected_type;
	typedef typename reconfig<my_symmetric_config>::type symmetric_type;
	typedef typename reconfig<my_unsymmetric_config>::type unsymmetric_type;
public:
	typedef typename storage::container_type VL;
	typedef typename storage::container_type vertex_container_type;
	typedef typename storage::edge_container_type edge_container_type;

	typedef typename EL::iterator out_vertex_iterator;
	typedef typename EL::const_iterator out_vertex_const_iterator;

	typedef typename iter::vertex_iterator vertex_iterator_type;
	typedef typename iter::const_vertex_iterator const_vertex_iterator_type;

public: // range-based loops support aliases
	typedef vertex_iterator_type iterator;
	typedef const_vertex_iterator_type const_iterator;

	//typedef std::pair<vertex_type, vertex_type> edge; // FIXME: remove.
	typedef typename storage::edge_type edge_type;

	typedef std::pair<iterator, out_vertex_iterator> edge_iterator;
private: // inacessible sfinae
	struct pdummy{};
public: // construct
	graph(const graph& x) : _num_edges(0)
	{
		assign_same(x); // FIXME op=?
	}
   template<template<class T, typename... > class ECT2, \
            template<class T, typename... > class VCT2, \
            class VDP2, \
            template<class G> class CFG2>
	graph(graph<ECT2, VCT2, VDP2, CFG2> const& x)
	: _num_edges(0)
	{

		detail::copy_helper<graph<ECT2,VCT2,VDP2,CFG2>, graph,
			  graph<ECT2,VCT2,VDP2,CFG2>::is_directed_v, is_directed_v
			>::assign(x, *this);
		assert(num_vertices()==x.num_vertices());

		if( !is_directed() && x.is_directed() ){
			// anything.
		}else{
			assert(_num_edges==x._num_edges);
		}
#ifndef NDEBUG
		for(auto i = begin(); i!=end(); ++i){
			assert(is_valid(iter::deref(i)));
			//   		for(auto& j : vertices()){ untested();
			//   //			assert(is_edge(&j,&i) == is_edge(&i,&j));
			//   		}
		}
#endif
	}
public: // move
#if 0
   template<template<class G> class CFG2>
	graph( graph<ECT, VCT, VDP, CFG2> const&& )
	{
		incomplete();
	}
#endif

   template<template<class G> class CFG2>
	graph( graph<ECT, VCT, VDP, CFG2> const&&x,
			typename std::enable_if<
			    !std::is_same< graph<ECT, VCT, VDP, CFG2>, this_type>::value
			  && this_type::is_directed_v
			  && this_type::is_simple_v
			  && this_type::is_symmetric_v
			//  &&!graph<ECT, VCT, VDP, CFG2>::is_simple_v
			  && graph<ECT, VCT, VDP, CFG2>::is_directed_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_symmetric_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_ordered_v,
				 pdummy>::type=pdummy())
	    : _v(std::move(x._v)),
	      _num_edges(0)
	{
		detail::move_helper<graph<ECT, VCT, VDP, CFG2>, this_type >::symmetrify(*this);
	}
#if 1
   template<template<class G> class CFG2>
	graph( graph<ECT, VCT, VDP, CFG2> const&&x,
			typename std::enable_if<
			     this_type::is_simple_v
			  && this_type::is_directed_v
			  && this_type::is_symmetric_v
			  && graph<ECT, VCT, VDP, CFG2>::is_simple_v
			  && graph<ECT, VCT, VDP, CFG2>::is_directed_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_symmetric_v
			  && graph<ECT, VCT, VDP, CFG2>::is_ordered_v,
				 pdummy>::type=pdummy())
	    : _v(std::move(x._v)),
	      _num_edges(x._num_edges)
	{ itested();
		BOOST_STATIC_ASSERT(sfinae::is_set_tpl<ECT>::value);
		size_t i=0;
		for(auto& x : _v){ itested();
			for(auto y : x){ itested();
				// HACK
				assert(i!=y); // no self loops.
				trace2("rev", y, i);
				_num_edges+=_v[y].insert(i).second;
			}
			++i;
		}

		if(is_directed()){ itested();
		}else{untested();
			_num_edges/=2;
		}
	}
#endif
   template<template<class G> class CFG2>
	graph( graph<ECT, VCT, VDP, CFG2> const&&x,
			typename std::enable_if<
			    !std::is_same< graph<ECT, VCT, VDP, CFG2>, this_type>::value
			  &&!this_type::is_symmetric_v
			  && graph<ECT, VCT, VDP, CFG2>::is_directed_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_simple_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_symmetric_v,
				 pdummy>::type=pdummy())
	    : _v(std::move(x._v)),
	      _num_edges(x._num_edges)
	{ untested();
		incomplete();
	}
   template<template<class G> class CFG2>
	graph( graph<ECT, VCT, VDP, CFG2> const&&x,
			typename std::enable_if<
			     graph<ECT, VCT, VDP, CFG2>::is_simple_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_directed_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_symmetric_v,
				 pdummy>::type=pdummy())
	    : _v(std::move(x._v)),
	      _num_edges(x._num_edges)
	{ untested();
		unreachable(); // undirected graphs are symmetric.
	}
   template<template<class G> class CFG2>
	graph( graph<ECT, VCT, VDP, CFG2> const&& x,
			typename std::enable_if<
			    !std::is_same< graph<ECT, VCT, VDP, CFG2>, this_type>::value
			  && graph<ECT, VCT, VDP, CFG2>::is_simple_v
			  &&!graph<ECT, VCT, VDP, CFG2>::is_directed_v
			  && graph<ECT, VCT, VDP, CFG2>::is_symmetric_v,
				 pdummy>::type=pdummy())
	    : _v(std::move(x._v)),
	      _num_edges(x._num_edges)
	{
		if(is_directed()){
			_num_edges*=2;
		}else{untested();
		}
	}
	// move self.
	graph(graph&& x)
	    : _v(std::move(x._v)),
	      _num_edges(x._num_edges)
	{

//		assert(nonvoid)
		// assert(num_vertices()==x.num_vertices()); no. _v has gone ...
		for(auto i = begin(); i!=end(); ++i){
			assert(is_valid(iter::deref(i)));
#ifdef MORE_DEBUG
			for(auto& j : vertices()){ untested();
				assert(is_edge(&j,&i) == is_edge(&i,&j));
			}
#endif
		}
	}
	graph(vertices_size_type n=0, edges_size_type m=0)
	    : _v(n), _num_edges(0)
	{
		if(m){
			EL a;
			prealloc(a, 2*m);
		}
	}
public: // Required by Iterator Constructible Graph
	template <class EdgeIterator>
	graph(EdgeIterator first, EdgeIterator last,
	      vertices_size_type nv, edges_size_type ne=0);

	template <class EI1, class EI2>
	void fill_in_edges(EI1 first, EI2 last, bool checkdups);
	template <class RANGE>
	void fill_in_edges(const RANGE& r)
	{ untested();
		return fill_in_edges(r.first, r.second, true);
	}
public: //assign
   template<template<class T, typename... > class ECT2, \
            template<class T, typename... > class VCT2, \
            class VDP2, \
            template<class G> class CFG2>
	graph& operator=(graph<ECT2,VCT2,VDP2,CFG2> const&);

	// somehow... required.
	graph& operator=(graph<SGARGS> const& x);

	// does not work. same structure often sufficient...
	graph& assign_same(graph<SGARGS> const& x);

public: // move assign
	graph& operator=(graph&& x) {
		trace2("move assign_same", size_t(num_vertices()), size_t(num_edges()));
		trace2("move assign", size_t(x.num_vertices()), size_t(x.num_edges()));
		trace2("move assign", is_directed(), x.is_directed());
#ifndef NDEBUG
		unsigned c=0;
		for(auto& i : x._v){ itested();
			c += i.size();
		}
		if(x.is_directed()){ untested();
			assert(x.num_edges() == c);
		}else{
			assert(2*x.num_edges() == c);
		}
#endif
		if (!x.is_ordered() && is_ordered()){ unreachable();
		}else{
		}
		if (&x!=this){
			_num_edges = x._num_edges;
			_v = std::move(x._v);
//			x._v.clear(); // hmm, does not work for sl
			x._num_edges = 0;
		}else{ untested();
		}
		trace2("move assign", size_t(num_vertices()), size_t(num_edges()));
#ifndef NDEBUG
		c=0;
		for(auto& i : _v){
			c += i.size();
		}
		// assert(2*num_edges() == c);
#endif
		return *this;
	}
   template<template<class T, typename... > class ECT2, \
            template<class T, typename... > class VCT2, \
            class VDP2, \
            template<class G> class CFG2>
	graph& operator=(graph<ECT2,VCT2,VDP2,CFG2> const&&);
public: // construct
// private:??
	template <typename G_t>
	void assign(G_t const& G);
public: // iterators
	iterator begin() {
		return iter::vbegin(_v);
	}
	iterator end() {
		return iter::vend(_v);
	}
	const_iterator cbegin() const {
		return iter::vbegin(_v);
	}
	const_iterator cend() const {
		return iter::vend(_v);
	}
	const_iterator begin() const {
		return iter::vbegin(_v);
	}
	const_iterator end() const {
		return iter::vend(_v);
	}
	// will not work for non-vector VCT...
	// only for sv*
	unsigned position(const vertex_type v) const
	{
		return iter::pos(v, _v);
	}
public: // BUG: should not expose this...?
	vertex_container_type& vertices() { untested();
		// incomplete(); later
		return _v;
	}
	const vertex_container_type& vertices() const { untested();
	  	// incomplete(); later
		return _v;
	}
public:
	void clear() {
		// inefficient (maybe not, with proper allocator...)
		_v.resize(0);
		_num_edges = 0;
	}
	// "reserve" maybe?!
	void reshape(size_t nv, size_t ne=0, bool directed_edges=false) {
		(void)directed_edges;
		if(ne) {
			// inefficient.
		}
		_v.resize(nv);
		for(iterator v=begin(); v!=end(); ++v){
			vertex_type w=iter::deref(v);
			out_edges(w).clear();
		}
		_num_edges = 0;
	}
	bool is_edge(const_vertex_type a, const_vertex_type b) const { untested();
		return a->n.find(const_cast<vertex_type>(b)) != a->n.end();
	}
private:
	//O(log d), where d is the degree of a
	void remove_edge_single(vertex_type a, vertex_type b) { untested();
		assert(a!=b);
		storage::remove_edge_single(a, b, _v);
		--_num_edges;
	}
private: // required by contruct from iterator
	// add edge from vertex positions \in [0, nv)
	void add_pos_edge(vertex_index_type a, vertex_index_type b) { untested();
		storage::add_pos_edge(a, b, _v);
	}
	// same, but from strings.
	void add_pos_edge(const char* a, const char* b) { untested();
		storage::add_pos_edge(a, b, _v);
	}
public:
	//O(n) due to (possible) vector resizing
	//amortized O(log n), possibly.
	vertex_type new_node();
	//O(1)
	EL& out_edges(vertex_type& v);
	EL& out_edges(vertex_type& v) const;
	EL& out_edges(const_vertex_type& /*v*/);
	const EL& out_edges(const_vertex_type& /*v*/) const;
	//O(1)
	vertex_index_type num_vertices() const {
		return _v.size();
	}
	//O(1)
	edges_size_type num_edges() const;
// private: ... friends
	void set_num_edges(edges_size_type);
public:
	size_t num_edges_debug(){
#ifdef DEBUG
		auto ne=0;
		for(auto& v:_v){ itested();
			for(auto const& x : v){ itested();
				(void)x;
				++ne;
			}
		}
		trace3("ned", ne, _num_edges, _v.size());
		return ne;
#endif
		return 0;
	}
public:
	// for each edge, add a reverse edge,
	// if it does not exist.
	//
	// set oriented, if the graph is oriented.
	// ( FIXME: this should be a tag/property)
	//
	//O(num_edges+num_vertices)
	void make_symmetric(bool oriented=false)
	{
		if(!is_directed()){
		}else if(is_symmetric()){ itested();
		}else{
			reverse_helper::make_symmetric(_v, _num_edges, oriented);
		}
		num_edges_debug(); // will notice if you were lying.
	}
	void add_reverse_edges(bool oriented=true)
	{ unreachable(); // will go!
		// use make_symmetric() instead!
		return make_symmetric(oriented);
	}
	//O(log max{d_1, d_2}), where d_1 is the degree of a and d_2 is the degree of b
	std::pair<edge_type, bool> add_edge(vertex_type a, vertex_type b) {
		assert(is_valid(a));
		assert(is_valid(b));
		return edge_helper::add_edge(a, b, _num_edges, _v);
	}
	// O(?)
	template<class PRED>
	void remove_out_edge_if(vertex_type a, PRED & p)
	{ // vector only
		auto& oa=out_edges(a);
		assert(oa.size()<=_num_edges);
		while(oa.size()){
			edge_type e(a, oa.back());
			if(p(e)){
				--_num_edges;
				oa.pop_back();
			}else{
				break;
			}
		}
		if(oa.size()==0){
			return;
		}else{
		}

		auto ii=oa.begin();
		auto nn=oa.begin();
		if(nn!=oa.end())
		for(++nn; nn!=oa.end();ii=nn++) {
			edge_type e(a, *ii);
			if(p(e)){ itested();
				--_num_edges;
				*ii=oa.back();
				oa.pop_back();

				while(nn!=oa.end()){
					if(p(edge_type(a, oa.back()))){
						--_num_edges;
						oa.pop_back();
					}else{
						break;
					}
				}
				if(nn==oa.end()){
					break;
				}else{
				}
			}else{
			}
		}
	}
	// O(log max{d_1, d_2}), where d_1 is the degree of a and d_2 is the degree of b
	// check=false: "i am sure this edge exists"
	void remove_edge(vertex_type a, vertex_type b, bool check=true)
	{
		if(check){
			// lets see..
		}else{ untested();
			assert(bits::vertex_helper<VDP>::contains(_v[a], b, &_v[b]));
		}
		bool done=bits::vertex_helper<VDP>::remove(_v[a], b, &_v[b]);

		if(is_directed()){
			// only one edge involved.
			if(check){
				if(done){
					--_num_edges;
				}else{
				}
			}else{ untested();
				--_num_edges;
			}
		}else{ // undirected.
			if(check){ untested();
				// if there was an edge, we need to take care of the back edge
				if(done){ untested();
					assert(bits::vertex_helper<VDP>::contains(_v[b], a, &_v[a]));
					bool done2=bits::vertex_helper<VDP>::remove(_v[b], a, &_v[a]);
					assert(done2); (void)done2;
					--_num_edges;
				}else{ untested();
				}
			}else{ untested();
				assert(bits::vertex_helper<VDP>::contains(_v[b], a, &_v[a]));
				bool done2=bits::vertex_helper<VDP>::remove(_v[b], a, &_v[a]);
				assert(done2==done); (void)done2;
				--_num_edges;
			}
		}
	}
	void contract(vertex_type& vd, vertex_type into);
	vertex_index_type degree(const_vertex_type who) const
	{ itested();
		return storage::degree(who, _v);
	}
	vertex_index_type out_degree(const_vertex_type who) const
	{ untested();
		return storage::degree(who, _v);
	}
	// degree of the graph, O(V)
	vertex_index_type degree() const
	{ untested();
		return storage::degree(_v);
	}
public:
	// BUG. don't use
	void hacksort(){
		incomplete();
		for( auto v : *this){
			std::sort(_v[v].begin(), _v[v].end());
		}
	}
public:
	// O(n + deg(v)*log(D)), where D is the maximum of the degrees of the
	// neighbours of v
	void clear_vertex(vertex_type v)
	{ untested();
		assert(is_valid(v));
		unsigned c = 0;
		for(auto& nIt : out_edges(v)){ untested();
			remove_edge_single(nIt, v);
			++c;
		}
		assert(c==out_edges(v).size());
		storage::clear_vertex(v, _v);
	}
	void remove_vertex(vertex_type)
	{ incomplete();
//			_v.erase(*v);
	}
	bool is_valid(const_vertex_type v) const
	{ itested();
		return bits::vertex_helper<VDP>::is_valid(v,_v);
	}
	void remove_vertex(iterator v);
	// O(log d), where d is the degree of v
	bool exists_edge(vertex_type v, vertex_type w)
	{ untested();
		assert((out_edges(v).find(w) == out_edges(v).end())
				== (out_edges(w).find(v) == out_edges(w).end()));
		return (out_edges(v).find(w) != out_edges(v).end());
	}

		/*
			template<class S>
			void print(S& s){ untested();
			for(unsigned int i = 0; i < num_vertices(); i++){ untested();
			for(edgecontainer<size_t>::iterator nIt = _v[i].begin(); nIt != _v[i].end(); nIt++) { untested();
			s << i << " -- " << *nIt << " ";
			}
			s << std::endl;
			}
			}
			*/
	bool graphviz_parse() const;
private:
//	void rewire_nodes(vertex_container_type& new_vl, vertex_type offset);
public: // BUG: private. use "friend"...
	vertex_container_type _v;
	size_t _num_edges;

public: // directions
	template<class GG>
	struct my_directed_config : public CFG<GG> { //
		static constexpr bool is_directed=true;
	};
public: // experimental...?

	// does this really work? directed/undirected are binary incompatible...
	template<class GG>
	struct my_directed_self_config : public CFG<GG> { //
		// hmm, this must proxy all supported types
//		typedef boost::mpl::true_ is_directed_t;
		static constexpr bool is_directed=true;

		typedef graph base_type;
//		typedef typename GG::edges_size_type edges_size_type;
		typedef size_t edges_size_type;
		static edges_size_type num_edges(GG const &g)
		{
			incomplete(); // staged for removal.
			return 2*g._num_edges; // BUG
			return 2*reinterpret_cast<base_type const&>(g).num_edges();
		}
		static void set_num_edges(edges_size_type e, GG &g)
		{ untested();
			incomplete(); // staged for removal.
			assert(! (e%2) );
			g._num_edges = e/2; // BUG/incomplete
//			reinterpret_cast<base_type const&>(g).set_num_edges();
		}
	};

	template<class self, bool dir>
	struct directed_self{
		typedef typename reconfig<my_directed_self_config>::type type;
	};
	template<class self>
	struct directed_self<self, true>{
		typedef self type;
	};
	typedef typename directed_self<this_type, is_directed_v>::type directed_self_type;
	typedef typename reconfig<my_directed_config>::type directed_type;

	directed_self_type const& directed_view() const
	{
		return reinterpret_cast<directed_self_type const&>(*this);
	}
	// more dangerous...
	// this is OBSOLETE, don't use.
	directed_self_type& directed_view() {
		unreachable();
		return reinterpret_cast<directed_self_type&>(*this);
	}
}; // class graph
/*--------------------------------------------------------------------------*/
template<class G>
struct graph_cfg_default {
	typedef default_DEGS<G> degs_type; // BUG
	typedef typename G::edges_size_type edges_size_type;
//	typedef size_t edges_size_type; // uuh
	static edges_size_type num_edges(G const& g);

	// private&friends...
	static void set_num_edges(edges_size_type, G& g);
};
/*--------------------------------------------------------------------------*/
VCTtemplate
typename graph<SGARGS>::edges_size_type
graph<SGARGS>::num_edges() const
{ itested();
	return CFG<graph<SGARGS> >::num_edges(*this);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
void
graph<SGARGS>::set_num_edges(typename graph<SGARGS>::edges_size_type e)
{ untested();
	return CFG<graph<SGARGS> >::set_num_edges(e, *this);
}
/*--------------------------------------------------------------------------*/
// construct from iterator...
VCTtemplate
template <class EI1, class EI2>
void graph<SGARGS>::fill_in_edges(EI1 first, EI2 last, bool possible_duplicates)
{
	if(possible_duplicates){ untested();
		//incomplete();
		// need to take more care of edge count
	}else{
	}

	{
		typedef typename iter::vertex_iterator::iterator_category iterator_category;
		trace1("calling fillpos", is_directed());
		_num_edges = bits::iter_helper<iterator_category, VDP, is_directed_v,
					 is_multiedge_v >::
			fill_pos(first, last, _v, /* bug? o. */ is_directed(),
					possible_duplicates/* template arg?! */ );
		trace1("fillpos done", _num_edges);
	}
}
/*--------------------------------------------------------------------------*/
VCTtemplate
template <class EdgeIterator>
graph<SGARGS>::graph(EdgeIterator first, EdgeIterator last,
                     vertices_size_type nv, edges_size_type ne)
    : graph(nv, ne)
{
	_num_edges=0;

	assert(_v.size()==nv);
	fill_in_edges(first, last, false);
	assert(!ne || ne==_num_edges); // unique edges? for now.

#ifndef NDEBUG
	unsigned c = 0;
	for(auto& i : _v){ itested();
		c += i.size();
	}
	assert(is_directed() || 2*num_edges() == c);
#endif
}
/*--------------------------------------------------------------------------*/
VCTtemplate
typename graph<SGARGS>::EL const&
    graph<SGARGS>::out_edges(const_vertex_type& v) const
{ itested();
	assert(is_valid(v));
	return storage::out_edges(v, _v);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
typename graph<SGARGS>::EL&
	graph<SGARGS>::out_edges(const_vertex_type& v)
{
	assert(is_valid(v));
	return storage::out_edges(v, _v);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
typename graph<SGARGS>::EL&
	graph<SGARGS>::out_edges(vertex_type& v)
{ itested();
	return storage::out_edges(v, _v);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
typename graph<SGARGS>::EL& // const?!
	graph<SGARGS>::out_edges(vertex_type& v) const
{ itested();
	assert(is_valid(v));
	auto&_V = const_cast<graph<SGARGS>*>(this)->_v;
	return storage::out_edges(v, _V);
}
/*--------------------------------------------------------------------------*/
// VCTtemplate
// const typename graph<SGARGS>::EL&
// 	graph<SGARGS>::out_edges(const typename graph<SGARGS>::vertex_type& v) const
// { untested();
// 	return storage::out_edges(v);
// }
/*--------------------------------------------------------------------------*/
VCTtemplate
   template<template<class T, typename... > class ECT2, \
            template<class T, typename... > class VCT2, \
            class VDP2, \
            template<class G> class CFG2>
graph<SGARGS>& graph<SGARGS>::operator=(graph<ECT2,VCT2,VDP2,CFG2> const& x)
{
	typedef graph<ECT2,VCT2,VDP2,CFG2> Gsrc;
	if((void*)&x==(void*)this){
		return *this;
	}else{
	}

	trace2("op=&", Gsrc::is_directed(), is_directed_v);

	detail::copy_helper<Gsrc, graph,
		 Gsrc::is_directed_v, is_directed_v,
		 Gsrc::is_nn_v, is_nn_v,
	    Gsrc::is_ordered_v, is_ordered() >::assign(x, *this);

	constexpr bool sortneeded=!x.is_ordered() && is_ordered();
	trace1("op=", sortneeded);
	bits::order_helper<sortneeded, ECT>::do_it(_v);
	return *this;
}
/*--------------------------------------------------------------------------*/
VCTtemplate
   template<template<class T, typename... > class ECT2, \
            template<class T, typename... > class VCT2, \
            class VDP2, \
            template<class G> class CFG2>
graph<SGARGS>& graph<SGARGS>::operator=(graph<ECT2,VCT2,VDP2,CFG2> const&& x)
{
	typedef graph<ECT2,VCT2,VDP2,CFG2> Gsrc;
	if((void*)&x==(void*)this){ untested();
		return *this;
	}else{
	}

	trace2("op=&&", Gsrc::is_directed(), is_directed_v);

	detail::move_helper<Gsrc, graph>::move(std::move(x), *this);

	constexpr bool needed=!x.is_ordered() && is_ordered();
	bits::order_helper<needed, ECT>::do_it(_v);
	return *this;
}
/*--------------------------------------------------------------------------*/
VCTtemplate
graph<SGARGS>& graph<SGARGS>::operator=(graph<SGARGS> const& x)
{
// 
// detail::copy_helper<graph<SGARGS>, graph<SGARGS> >::merge(x, *this, IGNOREDUPS)
// return *this
	return assign_same(x);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
graph<SGARGS>& graph<SGARGS>::assign_same(graph<SGARGS> const& x)
{
	typedef graph<ECT, VCT, VDP, CFG> oG;
	typedef typename oG::const_iterator other_const_iterator;
	typedef typename oG::const_vertex_type other_const_vertex_type;

	if (intptr_t(&x) == intptr_t(this)) { untested();
	}else if (num_vertices()==0){
		detail::copy_helper<graph, graph,
			is_directed_v, is_directed_v,
			is_nn_v, is_nn_v>::assign(x, *this);
	}else if (num_vertices()!=x.num_vertices()){ incomplete();
	// }else if( .. incomplete){ untested();
	}else{ untested();
// 		dead?. should not get here.
		// why not assign_?
		const_iterator b = begin();
		other_const_iterator s = x.begin();
		intptr_t delta = intptr_t(&*b) - intptr_t(&*s);
//		trace1("op=", delta);
		iterator v = begin();
		iterator e = end();
		_num_edges = x._num_edges;
		for(; v!=e ; ++v){ untested();
			vertex_type vd = iter::deref(v);
			other_const_vertex_type sd = oG::iter::deref(s);
			EL& E = out_edges(vd); // ?!
			EL const& S = x.out_edges(sd); // ?!
			bits::vertex_helper<VDP>::rebase(E, S, delta);
#if 0
			incomplete();// does not work for stx

			EL& MS = const_cast<EL&>(S); // HACK!?
			E = MS;
			//for(vertex& h : out_edges(vd))
			// works for std::set only...
			for(typename EL::iterator i=vd->n.begin(); i!=vd->n.end(); ++i){ untested();
				vertex* j= const_cast<vertex*>(&(*i));
				assert(*j >= &*x.begin());
				assert(*j < &*x.end());
				assert(*i >= &*x.begin());
				assert(*i < &*x.end());

				*j = vertex(intptr_t(*j) + delta);

				assert(*j >= &*begin());
				assert(*j < &*end());
			}
			for(typename EL::iterator i=vd->n.begin(); i!=vd->n.end(); ++i){ untested();
				assert(*i >= &*begin());
				assert(*i < &*end());
			}
#endif
			++s;
		}
	}
	return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace detail{
/*--------------------------------------------------------------------------*/
template<class oG, class G, bool X, bool Y, bool srcCont, bool tgtCont,
bool srcOrd, bool tgtOrd>
void copy_helper<oG, G, X, Y,
                 srcCont, tgtCont,
					  srcOrd, tgtOrd>::assign(oG const& src, G& tgt)
{
	auto& g=src;
//	typedef graph<ECT2, VCT2, VDP2, CFG2> oG; // source graph
	size_t nv = g.num_vertices();
	size_t ne = g.num_edges();
	trace6("assign_", nv, ne, tgt.num_vertices(), tgt.num_edges(), srcCont, tgtCont);
	assert(!tgt.is_directed() || src.is_directed()); // use other helper
//	auto psize=tgt._v.size();
	tgt._v.resize(nv);

	// BUG. inefficient.
	for(auto vi = tgt.begin(); vi!=tgt.end(); ++vi){
		typename G::vertex_type v=G::iter::deref(vi);
		tgt.out_edges(v).clear();
	}

	assert(tgt._v.size()==nv);
	typename G::vertex_type map[nv];
	std::map<typename oG::const_vertex_type, size_t> reverse_map;
	size_t i=0;
	tgt._num_edges = 0;

	// not necessary for vector...
	for(auto v=tgt.begin(); v!=tgt.end(); ++v){ itested();
		// for(auto& v : vertices())
		map[i] = G::iter::deref(v);
		++i;
		assert(i<=nv);
	}
//	trace2("resize v", i, nv);
	assert(i==nv);

	oG* GG = const_cast<oG*>(&g);
	i=0;
	// FIXME: use const_iter
	for(auto v=GG->cbegin(); v!=GG->cend(); ++v){ itested();
		typename oG::const_vertex_type ov=oG::iter::deref(v);
		reverse_map[ov] = i;
		++i;
	}

	assert(tgt._num_edges == 0);
	if(i!=nv){ untested();
		std::cerr << i << " " << nv << "\n";
		assert(false);
	}

	assert(tgt._num_edges == 0);
	auto num_og_edges=0;
	for(typename oG::iterator V=GG->begin(); V!=GG->end(); ++V){ itested();
		typename oG::vertex_type v=oG::iter::deref(V);
		for(typename oG::vertex_type w : g.out_edges(v)){ itested();
			assert(w!=v);
			assert(reverse_map[v]<tgt.num_vertices());
			assert(reverse_map[w]<tgt.num_vertices());
			tgt.add_edge(map[reverse_map[v]],
			             map[reverse_map[w]]);
//			++_num_edges;
			++num_og_edges;
		}
	}
//	trace3("debug", tgt.is_directed, src.is_directed, num_og_edges);
	if(tgt._num_edges == 2*ne){ untested();
	}else if(tgt.num_edges() == ne){
	}else if(tgt.is_directed() && !g.is_directed() ){ untested();
		trace2("bug", tgt._num_edges, ne );
		tgt._num_edges = ne;
		assert(tgt.num_edges() == 2*g.num_edges());
		incomplete();
	}else if(tgt.is_directed() && g.is_directed() ){

//		trace5("assign..1 1",nv, ne, tgt.num_vertices(), tgt.num_edges(), GG->num_edges());
//		trace2("assign...",GG->_num_edges, tgt._num_edges);
//		tgt._num_edges = 1; // HACK
		// this is dangerous?!
		if(tgt._num_edges !=  tgt.num_edges()){
		}else{ untested();
		}

		// OUCH. this does not work. must amend edgecount later.
		// assert(tgt.num_edges() == g.num_edges() || tgt.num_edges() == 2*g.num_edges());

	}else if( src.is_directed() && !tgt.is_directed()){
		
	}else{ untested();
		unreachable();
		std::cerr << "assign_ oops " << tgt._num_edges << ":" << ne << "\n";
	}
//	assert(tgt._num_edges == ne || tgt._num_edges == 2*ne);
	assert(tgt.num_vertices() == nv);



#ifndef NDEBUG
	// incomplete...
	for(auto i = tgt.begin(); i!=tgt.end(); ++i){
		assert(tgt.is_valid(G::iter::deref(i)));
		//   		for(auto& j : vertices()){ untested();
		//   //			assert(is_edge(&j,&i) == is_edge(&i,&j));
		//   		}
	}
#endif
} // copy_helper
//
#if 0 // use default
template<class Gsrc, class Gtgt>
struct copy_helper<Gsrc, Gtgt,  boost::mpl::true_,  boost::mpl::false_ >
{ untested();
}; // copy_helper
#endif
template<class Gsrc, class Gtgt>
struct copy_helper<Gsrc, Gtgt, false, true > {
	static void assign(Gsrc const& src, Gtgt& tgt)
	{
		trace2("assigning undirected to directed", src.num_edges(), tgt.num_edges());
		tgt = src.directed_view();
		trace2("done undirected to directed", src.num_edges(), tgt.num_edges());
		
//		HACK HACK
      if(tgt._num_edges == tgt.num_edges()){
			// incomplete(); better cross fingers!
			// tgt._num_edges = src.num_edges()*2;
		}else{ untested();
			incomplete();
		}
	}
}; // copy_helper

template<class Gsrc, class Gtgt>
struct copy_helper<Gsrc, Gtgt, false, true, true, true> {
	static void assign(Gsrc const& src, Gtgt& tgt)
	{
		// for now
		return copy_helper<Gsrc, Gtgt, false, true, false, false>::assign(src, tgt);
	}
};
/*--------------------------------------------------------------------------*/

namespace detail{
	template<class S, class T, class X=void>
	struct set_hlp{
		static void copy_set(S const& src, T& tgt)
		{
			for(auto w : src){
				tgt.push_back(w);
			}
		}
	};
	template<class S, class T>
	struct set_hlp<S, T, typename tovoid<typename sfinae::is_set<T>::type>::type> {

		typedef typename sfinae::is_set<T>::type hmm;

		typedef T hmm2;

		static void copy_set(S const& src, T& tgt)
		{ untested();
			for(auto w : src){ untested();
				tgt.insert(w); // FIXME: hint, if ordered.
			}
		}
	};
	template<class S>
	struct set_hlp<S, S> {
		static void copy_set(S const& src, S& tgt)
		{
			tgt = src;
			// applymap...?
		}
	};
}//detail

template<class Gsrc, class Gtgt>
struct copy_helper<Gsrc, Gtgt, true, false > {
	static void assign(Gsrc const& src, Gtgt& tgt)
	{
//		must weed out double edges?
		trace2("assigning directed to undirected", src.num_edges(), tgt.num_edges());
		tgt.directed_view() = src;
		assert( bool(tgt._num_edges) == bool(src._num_edges));
		trace1("assigned directed to undirected", src.num_edges());
		trace3("assigned directed to undirected", tgt._num_edges, tgt.num_edges(), tgt.directed_view().num_edges());
		// HACK tgt.directed_view() is inconsistent...
		tgt.directed_view().make_symmetric();
		trace2("symmetrified", tgt._num_edges, tgt.num_edges());
		tgt._num_edges /= 2;
	}
}; // copy_helper

template<class Gsrc, class Gtgt>
struct copy_helper<Gsrc, Gtgt, true, true, true, true > {
	static void assign(Gsrc const& src, Gtgt& tgt)
	{
		auto& g=src;
	//	typedef graph<ECT2, VCT2, VDP2, CFG2> oG; // source graph
		size_t nv = g.num_vertices();
		size_t ne = g.num_edges();
		trace4("assign_",nv, ne, tgt.num_vertices(), tgt.num_edges());
		assert(!tgt.is_directed() || g.is_directed()); // use other helper
	//	auto psize=tgt._v.size();
		tgt._v.resize(nv);

		if(tgt.num_edges()){
			// BUG. inefficient.
			for(auto vi = tgt.begin(); vi!=tgt.end(); ++vi){
				typename Gtgt::vertex_type v=Gtgt::iter::deref(vi);
				tgt.out_edges(v).clear();
			}
		}
		typename Gtgt::edges_size_type new_edges=0;
		for(typename Gsrc::iterator V=src.begin(); V!=src.end(); ++V){ itested();
			typename Gsrc::vertex_type v=Gsrc::iter::deref(V);

			detail::set_hlp<typename Gsrc::edge_container_type,
				             typename Gtgt::edge_container_type>
				::copy_set(g.out_edges(v), tgt.out_edges(v) /*, map...*/ );
			new_edges += g.out_edges(v).size();
		}
		tgt._num_edges = new_edges; // HACK!
		// tgt.set_num_edges(new_edges);
	}
}; // copy_helper

#if 0
template<class Gsrc, class Gtgt>
struct copy_helper<Gsrc, Gtgt, boost::mpl::true_, boost::mpl::true_>
{ untested();
	static void assign(Gsrc const&, Gtgt&)
	{ untested();
		incomplete();
	}
}; // copy_helper
#endif
/*--------------------------------------------------------------------------*/
} // detail
/*--------------------------------------------------------------------------*/
// TODO: runtime info?
VCTtemplate
void graph<SGARGS>::contract(vertex_type& vd, vertex_type into)
{ untested();
	assert(vd);
	assert(into);

	auto x=vd->n.find(into);
	if(x!=vd->n.end()){ untested();
		// delete edge in between
		vd->n.erase(x); // can we avoid it? we will vd.clear() later on.
		--_num_edges;
		x = into->n.find(vd);
		assert(x!=into->n.end());
		into->n.erase(x);
	}else{ untested();
	}

	// merge outnodes.
	auto I = vd->n.begin();
	auto E = vd->n.end();
	into->n.insert(I,E);

	for(auto& I : vd->n){ untested();
		assert(&*I != &*into);
		bool inserted_new = I->n.insert(into).second;
		if(inserted_new){ untested();
			// edge has moved.
		}else{ untested();
			// there are two, one of which will be gone.
			--_num_edges;
		}

		x = I->n.find(vd);
		assert(x!=I->n.end());
		I->n.erase(x);
	}

	// urgs. could be unnecessary
	// case of erase=true (next function)
	vd->n.clear();
}
/*--------------------------------------------------------------------------*/
VCTtemplate
inline void graph<SGARGS>::remove_vertex(iterator v)
{ untested();
	if(storage::need_rewire()){ untested();
		incomplete();
	}
	_v.erase(v);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
inline typename graph<SGARGS>::vertex_type graph<SGARGS>::new_node()
{
	return storage::new_node(_v);
}
/*--------------------------------------------------------------------------*/
#if 0
VCTtemplate
static inline size_t num_edges(graph<SGARGS> &G)
{ untested();
    return G.num_edges();
}
/*--------------------------------------------------------------------------*/
VCTtemplate
static inline size_t num_edges(graph<SGARGS> const &G)
{ untested();
    return G.num_edges();
}
/*--------------------------------------------------------------------------*/
// VCTtemplate
// static inline void remove_edge(vertex_type v,
// 		typename graph<SGARGS>::vertex w, graph<SGARGS> &G)
// { untested();
//     G.remove_edge(v, w);
// }
/*--------------------------------------------------------------------------*/
// VCTtemplate
// static inline size_t out_degree(const /*_*/ vertex_type v, graph<SGARGS> const& G)
// { untested();
//     return G.degree(v);
// }
#endif
/*--------------------------------------------------------------------------*/
VCTtemplate
static inline boost::tuple<typename graph<SGARGS>::out_iterator,
                           typename graph<SGARGS>::out_iterator>
adjacent_vertices(typename graph<SGARGS>::vertex v, graph<SGARGS> &G)
{ untested();
	typedef typename graph<SGARGS>::out_iterator Iter;
	typedef boost::tuple<Iter, Iter> P;
	auto& o=G.out_edges(v);
	return P(o.begin(), o.end());
}
/*--------------------------------------------------------------------------*/
VCTtemplate
static inline void clear_vertex(typename graph<SGARGS>::vertex v, graph<SGARGS> &G)
{ untested();
    G.clear_vertex(v);
}
/*--------------------------------------------------------------------------*/
VCTtemplate
static inline std::pair<bool, bool> edge(typename graph<SGARGS>::vertex v,
		typename graph<SGARGS>::vertex w, graph<SGARGS> &G)
{ untested();
    return std::pair<bool, bool>(true, G.exists_edge(v, w));
}
/*--------------------------------------------------------------------------*/
namespace bits{
template<>
inline void storage<GALA_DEFAULT_SET, GALA_DEFAULT_VECTOR, vertex_ptr_tag>::add_pos_edge(
      typename storage<GALA_DEFAULT_SET, GALA_DEFAULT_VECTOR, vertex_ptr_tag>::vertex_index_type v,
      typename storage<GALA_DEFAULT_SET, GALA_DEFAULT_VECTOR, vertex_ptr_tag>::vertex_index_type w,
      typename storage<GALA_DEFAULT_SET, GALA_DEFAULT_VECTOR, vertex_ptr_tag>::container_type& _v)
{ untested();
	trace2("pos edge", v, w);
	assert(v<w); // for now
	_v[v].n.insert(&_v[w]);
	_v[w].n.insert(&_v[v]);
}
/*--------------------------------------------------------------------------*/
// FIXME: implement for all ECTs...
// template< template<class T, class... > class ECT >
// inline void storage<ECT, std::vector, vertex_ptr_tag>::add_pos_edge(
//       typename storage<ECT, std::vector, vertex_ptr_tag>::vertex_index_type v,
//       typename storage<ECT, std::vector, vertex_ptr_tag>::vertex_index_type w,
//       typename storage<ECT, std::vector, vertex_ptr_tag>::container_type& _v)
// { untested();
// 	_v[v].insert(&_v[w]);
// 	_v[w].insert(&_v[v]);
// }
/*--------------------------------------------------------------------------*/
} // bits
/*--------------------------------------------------------------------------*/
template<class G>
typename G::edges_size_type graph_cfg_default<G>::num_edges(G const& g)
{ itested();
	return g._num_edges;
}
template<class G>
void graph_cfg_default<G>::set_num_edges(typename G::edges_size_type x, G& g)
{ untested();
	g._num_edges = x;
}
/*--------------------------------------------------------------------------*/
namespace detail{
/*--------------------------------------------------------------------------*/
template<class SRC, class TGT>
struct move_helper<SRC, TGT,
                   typename tovoid < typename std::enable_if<
                        TGT::is_ordered() && !SRC::is_ordered()
           &&   TGT::is_simple() && !SRC::is_simple()
                    >::type >::type
                  >
{ //
	static void move(SRC const&& src, TGT& tgt){ untested();
		incomplete(); // need to remove dups!
		if ((void*)&tgt==(void*)&src){ untested();
		}else{ itested();
			tgt._num_edges = src._num_edges;
			tgt._v = std::move(src._v);
//			x._v.clear(); // hmm, does not work for sl
//			tgt._num_edges = 0;
			tgt.hacksort();
		}
	}
	static void symmetrify(TGT& g){ untested();
		incomplete();
	}
}; // move_helper
/*--------------------------------------------------------------------------*/
template<class SRC, class TGT>
struct move_helper<SRC, TGT,
                   typename tovoid < typename std::enable_if<
                        TGT::is_ordered() && !SRC::is_ordered()
           && (  TGT::is_simple() == SRC::is_simple() )
                    >::type >::type
                  >
{ //
	static void move(SRC const&& src, TGT& tgt){
		// BUG: check the other flags?!
		if ((void*)&tgt==(void*)&src){ untested();
		}else{
			tgt._num_edges = src._num_edges;
			tgt._v = std::move(src._v);
//			x._v.clear(); // hmm, does not work for sl
//			tgt._num_edges = 0;
			tgt.hacksort();
		}
	}
	static void symmetrify(TGT& x){ untested();
		incomplete();
	}
};
} // detail
/*--------------------------------------------------------------------------*/
} // gala
/*--------------------------------------------------------------------------*/
#endif

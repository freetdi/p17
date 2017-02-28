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
 * boost interface for gala graph
 */
#ifndef GALA_BOOST_H
#define GALA_BOOST_H

#include <gala/graph.h>
#include <boost/graph/graph_traits.hpp> // bidirectional_graph_tag
#include <boost/graph/properties.hpp>

// hrm. doesn't work
// #include "boost_copy.h"


namespace boost { //
	struct gala_graph_traversal_category :
		public virtual bidirectional_graph_tag, // ?!
		public virtual adjacency_graph_tag,
		public virtual edge_list_graph_tag,
		public virtual vertex_list_graph_tag { };

	template<class VDP>
	struct myVDP{
		typedef VDP type;
	};

	template<>
	struct myVDP<gala::vertex_ptr_tag>{
		typedef uintptr_t type;
	};

	// => boost/detail?
	template< STPARMS, bool is_directed , bool is_ordered, bool is_loopless=true>
	struct edge_helper{ //
		// needed?
		using storage=gala::bits::storage<STARGS>;
		using iter=gala::bits::iter<STARGS>;

		template<class B, class VC>
		static void skip_edge(B& base, VC& vc)
		{
			auto &f=base.first;
			auto &s=base.second;
			auto vend=iter::vend(vc);

			if(f==vend){
			}else if( is_directed ){
			}else if( !is_ordered ){
			}else if(*s > iter::deref(f)){
				increment_edge(base, vc);
			}else{ untested();
			}
		}

		template<class B, class VC>
		static void increment_edge(B& base, VC& vc)
		{
			auto &f=base.first;
			auto &s=base.second;
			auto e = storage::out_edges(*f, vc).end();
			++s;

			auto vend=iter::vend(vc);

			while(true){
				if(f==vend){ untested();
					break;
				}else if(s==storage::out_edges(*f, vc).end()) {
					++f;
					if(f==vend){
						break;
					}
					e = storage::out_edges(*f, vc).end();
					s = storage::out_edges(*f, vc).begin();
				}else if(!is_loopless && *s == iter::deref(f)){ untested();
					incomplete();
				}else if( is_ordered && !is_directed && iter::deref(f) < *s){
					++f;
					e = storage::out_edges(*f, vc).end();
					s = storage::out_edges(*f, vc).begin();
				}else if( !is_ordered && !is_directed && iter::deref(f) > *s){
					++s;
					continue;
				}else if(s!=storage::out_edges(*f, vc).end()){
					break;
				}else{ untested();
					// next row.
					++f;
					e = storage::out_edges(*f, vc).end();
					s = storage::out_edges(*f, vc).begin();
				}
			}
		}
	}; // edge_helper

#if 0 // use constexpr bool for diversion
	template< STPARMS, bool is_ordered >
	struct edge_helper<STARGS,/*directed*/ true, is_ordered >{ //
		using storage=gala::bits::storage<STARGS>;
		using iter=gala::bits::iter<STARGS>;

		template<class B, class VC>
		static void increment_edge(B& base, VC& vc)
		{ untested();
			auto &f=base.first;
			auto &s=base.second;
			auto e = storage::out_edges(*f, vc).end();
			++s;

			if(e==s){ untested();
			}

			auto vend=iter::vend(vc);

			while(true){ untested();
				if(f==vend){ untested();
					break;
				}else if(s==storage::out_edges(*f, vc).end()) { untested();
					++f;
					if(f==vend){ untested();
						break;
					}
					e = storage::out_edges(*f, vc).end();
					s = storage::out_edges(*f, vc).begin();
				}else if(*s == iter::deref(f)){ untested();
					unreachable(); // self loop
					assert(false);
				}else if(!is_directed && iter::deref(f) < *s){ untested();
					++f;
					e = storage::out_edges(*f, vc).end();
					s = storage::out_edges(*f, vc).begin();
				}else if(s!=storage::out_edges(*f, vc).end()){ untested();
					break;
				}else{ untested();
					// next row.
					++f;
					e = storage::out_edges(*f, vc).end();
					s = storage::out_edges(*f, vc).begin();
				}
			}
		}
	}; // edge_helper
#endif
	namespace util{
		template<class C>
		bool is_in_edgeset(C const& c, typename C::value_type v, bool s)
		{
			if(s){
				return std::binary_search(c.begin(), c.end(), v);
			}else{
				return std::find(c.begin(), c.end(), v)!=c.end();
			}
		}

		// incomplete: other sets? use sfinae.h...
		template<class E>
		bool is_in_edgeset(std::set<E> const& c, E v, bool s)
		{ itested();
			assert(s);
			return c.find(v)!=c.end();
		}
	}
	namespace detail{
	template<bool X>
	struct galaboost_dir {
		typedef undirected_tag type;
	};
	template<>
	struct galaboost_dir<true> {
		typedef directed_tag type;
	};
	template<bool X>
	struct galaboost_par {
		typedef disallow_parallel_edge_tag type;
	};
	template<>
	struct galaboost_par<true> {
		typedef allow_parallel_edge_tag type;
	};
	}

	VCTtemplate
	struct graph_traits<gala::graph<SGARGS> > { //
		using G=gala::graph<SGARGS>;
		typedef typename myVDP<VDP>::type vertices_size_type;
		typedef size_t edges_size_type; // FIXME: use v^2
		typedef typename myVDP<VDP>::type degree_size_type;

		typedef typename gala::graph<SGARGS>::vertex_type vertex_descriptor;
		typedef typename gala::graph<SGARGS>::edge_type edge_descriptor;

		typedef typename detail::galaboost_dir<G::is_directed()>::type directed_category;
		typedef gala_graph_traversal_category traversal_category;

		typedef typename detail::galaboost_par<G::is_multiedge()>::type edge_parallel_category;

		class vertex_iterator
		    : public iterator_facade<
		        vertex_iterator, //  <= Derived
		        typename gala::graph<SGARGS>::iterator, // <= value_type
		        typename gala::graph<SGARGS>::iterator::iterator_category,
		        typename gala::graph<SGARGS>::vertex_type, // <= value&
		        ptrdiff_t /*Difference*/
		    >{ //
		public:
			typedef gala::graph<SGARGS> G;
			typedef typename gala::graph<SGARGS>::vertex_type reference;
			typedef typename gala::graph<SGARGS>::iterator base_type;
			typedef ptrdiff_t difference_type; // in iterator_facade?

			void advance(size_t n)
			{
			// base+=n;
				std::advance(base,n);
			}
			vertex_iterator(typename gala::graph<SGARGS>::iterator
			    e=typename gala::graph<SGARGS>::iterator(),
			    const gala::graph<SGARGS>*g=NULL)
			        : base(e)
			{
				(void)g;
			}
			vertex_iterator(const vertex_iterator& p)
			    : base(p.base) /*,g(p.g)*/
			{
			}

			typename gala::graph<SGARGS>::iterator value(){return base;}

#if 0 // ?!
				vertex_iterator&& operator+(size_t n)
				{ untested();
					vertex_iterator a=*this;
					a.advance(n);
					return std::move(a);
				}
#else
			vertex_iterator operator+(size_t n)
			{ untested();
				vertex_iterator a=*this;
				a.advance(n);
				return a;
			}
#endif

		private:
			// reference
			reference dereference()
			{ untested();
				return &*base;
			}
			const reference dereference() const
			{ // untested();
				// BUG. don't use detail...
				return G::iter::deref(base);
			}

			bool equal(const vertex_iterator& other) const
			{
				return base == other.base;
			}
			void increment()
			{
				++base;
			}
			void decrement()
			{ untested();
				--base;
			}
		private:
			//value_type
			base_type base;
#ifdef EXTRA_G
			const ::gala::graph<SGARGS>* _g; //needed for vertex descriptor??
#endif
			friend class iterator_core_access;
		};

		class adjacency_iterator
		    : public iterator_facade<
		        adjacency_iterator,
		        typename gala::graph<SGARGS>::out_vertex_iterator, // value_type
//		        bidirectional_traversal_tag,
				  std::input_iterator_tag, // for now.
		        /*const*/ typename ::gala::graph<SGARGS>::vertex_type, // <= reference
		        typename ::gala::graph<SGARGS>::vertex_type* // difference_type
		    >{ //
		public:
			typedef typename gala::graph<SGARGS>::out_vertex_iterator value_type;
			typedef typename gala::graph<SGARGS>::out_vertex_const_iterator const_value_type;
			typedef typename gala::graph<SGARGS>::vertex_type reference;
//		   typedef typename gala::graph<SGARGS>::vertex_type* difference_type; ??
		   typedef intptr_t difference_type; // why?
		public:
			adjacency_iterator(typename ::gala::graph<SGARGS>::out_vertex_iterator
			    e=typename gala::graph<SGARGS>::out_vertex_iterator()) : _base(e)
			{
			}
			bool operator==(const_value_type other) const
			{ untested();
				return const_value_type(_base) == other;
			}
			bool operator==(const adjacency_iterator& other) const
			{
				return _base == other._base;
			}
			bool operator!=(const adjacency_iterator& other) const
			{
				return _base != other._base;
			}
		private: // reference
			reference dereference() const
			{
				return *_base;
			}

			bool equal(const adjacency_iterator& other) const
			{ untested();
				return _base == other._base;
			}
			void increment()
			{
				++_base;
			}
			void decrement()
			{ untested();
				--_base;
			}
		public: // bug? use friends...
			const value_type base(){return _base;}
		private:
			value_type _base;
			friend class iterator_core_access;
		};

		class edge_iterator
			: public iterator_facade<
			             edge_iterator,
			             typename gala::graph<SGARGS>::edge_iterator, // value_type
			             bidirectional_traversal_tag,
			             const typename gala::graph<SGARGS>::edge_type, // reference (?)
			             const typename gala::graph<SGARGS>::out_vertex_iterator*>
		{ //
		public: // types
			typedef gala::graph<SGARGS> G;
			typedef typename G::edge_iterator base_type;
			typedef typename G::edge_type reference;
			typedef ptrdiff_t difference_type; // is this correct?
			typedef std::input_iterator_tag iterator_category; // makes sense!?
		public:
			edge_iterator() : base()
			{ itested();
			}
			edge_iterator(typename gala::graph<SGARGS>::iterator v,
			              typename gala::graph<SGARGS>::out_vertex_iterator e,
					const ::gala::graph<SGARGS>* g)
				: base(std::make_pair(v,e)), _g(g)
			{
				typedef typename ::gala::graph<SGARGS> G;
				G& gg = *const_cast<gala::graph<SGARGS>*>(_g); // HACK
				typename G::vertex_container_type& VC=gg._v;

				edge_helper< STARGS, G::is_directed(), G::is_ordered() >::skip_edge
					(base, VC);

			}

		private:
			const reference dereference() const
			{
				typename G::iterator i = base.first;
				typename G::vertex_type s = G::iter::deref(i);
				typename G::vertex_type t = *base.second;

				return reference(s,t);
			}
			bool equal(const edge_iterator& other) const
			{
				if(base.first==_g->end() &&
					    other.base.first==_g->end()){
					return true;
				}else {
					return base.first == other.base.first
					    && base.second == other.base.second;
				}
			}
			void increment()
			{
				G& gg = *const_cast<gala::graph<SGARGS>*>(_g); // HACK
				typename G::vertex_container_type& VC=gg._v;

				edge_helper< STARGS, G::is_directed(), G::is_ordered() >::increment_edge
					(base, VC);
			}

			void decrement() { untested();
				if(base.second == base.first.begin()) { untested();
					--(base.first);
					incomplete();
					base.second = base.first.begin();
				}
				--(base.second);
			}

		private:
			base_type base;
			const gala::graph<SGARGS>* _g;
			friend class iterator_core_access;
		};

		class out_edge_iterator
			: public iterator_facade<
			   out_edge_iterator,
			   std::pair<typename gala::graph<SGARGS>::vertex_type,
			             typename gala::graph<SGARGS>::out_vertex_iterator>,
			   // bidirectional_traversal_tag, // breaks InputIterator (why?)
				std::input_iterator_tag,
			   const typename gala::graph<SGARGS>::edge_type, // <= reference
			   const typename gala::graph<SGARGS>::out_vertex_iterator* // difference_type
			>{ //
		public: // types
			typedef typename std::pair<
			    typename gala::graph<SGARGS>::vertex_type,
			    typename gala::graph<SGARGS>::out_vertex_iterator> value_type;
		   typedef intptr_t difference_type; // why?
			typedef typename gala::graph<SGARGS>::edge_type reference;
			typedef typename gala::graph<SGARGS>::edge_type edge_type;

		public: // construct
			out_edge_iterator()
			{
			}
			out_edge_iterator(
			    typename boost::graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
			    typename gala::graph<SGARGS>::out_vertex_iterator w)
			{
				base.first = v;
				base.second = w;
			}
		private:
			reference dereference() const
			{
				return edge_type(base.first, *base.second);
			}
			bool equal(const out_edge_iterator& other) const
			{
				return base.second == other.base.second;
			}
			void increment()
			{
				++(base.second);
			}
			void decrement()
			{ untested();
				--(base.second);
			}
//			bool operator==(const out_edge_iterator& other) const
//			{ incomplete();
//				return false;
//			}
//			bool operator!=(const out_edge_iterator& other) const
//			{ incomplete();
//				return false;
//			}

		private:
			value_type base;
			friend class iterator_core_access;
		};

	}; // graph_traits<sgVCT>
	// =============================================================================//

	VCTtemplate
	typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor
	    add_vertex(gala::graph<SGARGS>& g)
	{
		return g.new_node();
	}

	VCTtemplate
	inline std::pair<typename graph_traits<::gala::graph<SGARGS> >::adjacency_iterator,
	                 typename graph_traits<::gala::graph<SGARGS> >::adjacency_iterator>
	      adjacent_vertices(typename graph_traits<::gala::graph<SGARGS> >::vertex_descriptor u,
	                        const gala::graph<SGARGS>& g)
	{ itested();
		typedef typename graph_traits<::gala::graph<SGARGS> >::adjacency_iterator Iter;
		auto& o = g.out_edges(u);
		return std::make_pair( Iter(o.begin()), Iter(o.end()));
	}

	VCTtemplate
	inline std::pair<typename graph_traits<::gala::graph<SGARGS> >::adjacency_iterator,
	                 typename graph_traits<::gala::graph<SGARGS> >::adjacency_iterator>
	      adjacent_vertices(typename graph_traits<::gala::graph<SGARGS> >::vertex_descriptor u,
	                        gala::graph<SGARGS>& g)
	{
		typedef typename graph_traits<::gala::graph<SGARGS> >::adjacency_iterator Iter;
		return std::make_pair( Iter(g.out_edges(u).begin()),
		                       Iter(g.out_edges(u).end()));
	}

	VCTtemplate
	std::pair<typename graph_traits<::gala::graph<SGARGS> >::edge_descriptor,
	    bool> add_edge(
         typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor u,
         typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
         gala::graph<SGARGS>& g)
	{
		// if loopless ...
		assert(v!=u); // for now

		return g.add_edge(u, v);
//		return std::make_pair(n, /*FIXME*/ true);
	}

	VCTtemplate
	void remove_edge(
    typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor u,
    typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor v,
    gala::graph<SGARGS>& g)
	{
		g.remove_edge(u, v);
	}

	template< galaPARMS, class PRED >
	void remove_out_edge_if(
			typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor u, PRED& p, gala::graph<SGARGS>& g)
	{
		// wrap P?
		g.remove_out_edge_if(u, p);
	}

	VCTtemplate
	void clear_vertex(typename graph_traits<::gala::graph<SGARGS>
			>::vertex_descriptor u, ::gala::graph<SGARGS> &g)
	{ itested();
		g.clear_vertex(u);
	}

	VCTtemplate
	void remove_vertex(typename graph_traits< ::gala::graph<SGARGS>
			>::vertex_descriptor u, ::gala::graph<SGARGS> &g)
	{ untested();
		g.remove_vertex(u);
	}

	// doesnt work
//	VCTtemplate
//	void* operator[]( ::gala::graph<SGARGS>&,
//			const typename ::gala::graph<SGARGS>::vertex& v) { // untested();
//		incomplete();
//		return NULL;
//	}

	VCTtemplate
   std::pair<typename graph_traits<::gala::graph<SGARGS> >::edge_descriptor, bool>
	   edge(/*const*/ typename graph_traits< ::gala::graph<SGARGS> >::vertex_descriptor u,
	        /*const*/ typename graph_traits< ::gala::graph<SGARGS> >::vertex_descriptor v,
		     const ::gala::graph<SGARGS>& g)
	{
		assert(g.is_valid(v));
		assert(g.is_valid(u));

		bool is_edge;
		is_edge = util::is_in_edgeset(g.out_edges(u), v, g.is_ordered());

		assert((!is_edge) || (u!=v));
		
		//i = g.out_edges(u).find(v);
		//assert(is_edge == (i!=g.out_edges(u).end()));

		if(is_edge){
			auto e = typename graph_traits<gala::graph<SGARGS> >::edge_descriptor(u,v);
			return std::make_pair(e, is_edge);
		}else{
			auto e = typename graph_traits<gala::graph<SGARGS> >::edge_descriptor();
			return std::make_pair(e, is_edge);
		}
	}

	VCTtemplate
	typename graph_traits<::gala::graph<SGARGS> >::degree_size_type
	out_degree(typename graph_traits< ::gala::graph<SGARGS> >::vertex_descriptor u,
			const ::gala::graph<SGARGS>&g)
	{
	   return g.degree(u);
	}

	VCTtemplate
	typename graph_traits<::gala::graph<SGARGS> >::degree_size_type
	degree(typename graph_traits< ::gala::graph<SGARGS> >::vertex_descriptor u,
			const ::gala::graph<SGARGS>&g)
	{
		return boost::out_degree(u,g);
	}

#if 0
	VCTtemplate
	typename graph_traits< ::gala::graph<SGARGS> >::vertex_descriptor source(
	  typename graph_traits< ::gala::graph<SGARGS> >::edge_descriptor e,
	  const ::gala::graph<SGARGS>&)
	{ untested();
		return e.first;
	}

	VCTtemplate
	typename graph_traits<gala::graph<SGARGS> >::vertex_descriptor target(
	  typename graph_traits<gala::graph<SGARGS> >::edge_descriptor e,
	  const ::gala::graph<SGARGS>&)
	{ untested();
		return e.second;
	}
#else
	using std::source;
	using std::target;
#endif

	VCTtemplate
	inline std::pair<typename graph_traits<gala::graph<SGARGS> >::edge_iterator,
	                 typename graph_traits<gala::graph<SGARGS> >::edge_iterator>
	             edges(const ::gala::graph<SGARGS>& g)
	{
		typedef gala::graph<SGARGS> G;
		typedef typename graph_traits<G>::edge_iterator Iter;
		typedef typename G::iterator vi;
		typedef typename G::vertex_type vertex_type;
		typedef typename G::out_vertex_iterator ovi;
		gala::graph<SGARGS>* gg = const_cast<G*>(&g); // HACK
		vi u = gg->begin();
		vi e = gg->end();

		while(u!=e){
			vertex_type U=G::iter::deref(u);
			if(!gg->out_edges(U).empty()){
				break;
			}else{
			}
			++u;
		}
		auto deref=G::iter::deref(u);

		if(u!=e){
			ovi U = gg->out_edges(deref).begin();
			assert(!gg->is_ordered() || (deref<*U) || u==e);

			return std::make_pair(Iter(u, U, &g),
			                      Iter(e, U, &g));
		}else{
			// mpty
			ovi E;
			return std::make_pair(Iter(e, E, &g),
			                      Iter(e, E, &g));
		}

	}

	VCTtemplate
	inline std::pair<typename graph_traits<gala::graph<SGARGS> >::vertex_iterator,
	                 typename graph_traits<gala::graph<SGARGS> >::vertex_iterator>
	             vertices(gala::graph<SGARGS> const& g)
	{
		typedef typename graph_traits<gala::graph<SGARGS> >::vertex_iterator Iter;
		::gala::graph<SGARGS>* G = const_cast<::gala::graph<SGARGS>*>(&g); // HACK
		return std::make_pair( Iter(G->begin()), Iter(G->end()));
	}

	VCTtemplate
	inline std::pair<typename graph_traits<gala::graph<SGARGS> >::vertex_iterator,
	                 typename graph_traits<gala::graph<SGARGS> >::vertex_iterator >
						vertices(gala::graph<SGARGS>& g)
	{ // untested();
		typedef typename graph_traits<gala::graph<SGARGS> >::vertex_iterator Iter;
		return std::make_pair( Iter(g.begin(),&g), Iter(g.end(),&g) );
	}

	VCTtemplate
	typename graph_traits<::gala::graph<SGARGS> >::edges_size_type
	num_edges(const ::gala::graph<SGARGS>& g)
	{
#ifndef NDEBUG
		if(!g.is_directed()){
			size_t c=0;
			auto p=vertices(g);
			for(; p.first!=p.second; ++p.first){ itested();
				c+=degree(*p.first, g);
			}
			if(g.is_multiedge()){
			}else{
			}
			if(c!=g.num_edges()*2){
				std::cerr << c << " vs " << g.num_edges() << "\n";
			}
			assert(c==g.num_edges()*2);
		}
#endif
		return g.num_edges();
	}


// internal assignment
	template<galaPARMS,
	template<class T, typename... > class ECT2,
	template<class T, typename... > class VCT2,
	class VDP2,
	template<class G> class CFG2>
   inline void copy_graph(const gala::graph<SGARGS>& g, gala::graph<ECT2, VCT2, VDP2, CFG2>& h)
	{
		h = g;
	}
}

// TODO: cleanup;
#include "boost_detail.h"

namespace boost {

	VCTtemplate
	typename graph_traits<::gala::graph<SGARGS> >::vertices_size_type
	num_vertices(const gala::graph<SGARGS>& g) { // untested();
		return g.num_vertices();
	}

//	VCTtemplate
//	struct vertex_property_type<::gala::graph<SGARGS> >{
//		typedef void type;
//	};
//	VCTtemplate
//	struct edge_property_type<::gala::graph<SGARGS> >{
//		typedef void type;
//	};

	// map stuff
	VCTtemplate
	class gala_graph_index_map
	    : public put_get_helper<
	        typename gala::bits::vertex_selector<ECT, VDP>::vertex_index_type,
	        gala_graph_index_map<SGARGS> >
	{ //
		public:
			typedef typename gala::bits::vertex_selector<ECT, VDP>::vertex_index_type vertex_index_type;
			typedef typename gala::graph<SGARGS>::vertex_type vertex_descriptor;
			typedef readable_property_map_tag category;
			typedef vertex_index_type value_type;
			typedef vertex_index_type reference;
			typedef vertex_descriptor key_type;
			gala_graph_index_map(gala::graph<SGARGS>const & g)
			    : _g(g)
			{
			}
			template <class T>
			value_type operator[](T x) const
			{
//				assert(x == _g.position(x));
				return _g.position(x);
			}
			gala_graph_index_map& operator=(const gala_graph_index_map& s)
			{
				assert(&s._g==&_g); (void)s;
				return *this;
			}
		private:
			gala::graph<SGARGS> const&_g;
	};

	// map stuff
	class gala_graph_eid_map
		: public put_get_helper<int, gala_graph_eid_map>
	{ //
		public:
			typedef readable_property_map_tag category;
			typedef int value_type;
			typedef int reference;
			typedef std::pair<unsigned long, unsigned long> key_type; // ouch
			gala_graph_eid_map() { incomplete(); }
			template <class T>
			long operator[](T x) const { untested(); return x->id(); }
	};

	// property stuff.
	template <class Tag>
	struct gala_graph_property_map { };

	template <>
	struct gala_graph_property_map<vertex_index_t> {
		VCTtemplate
		struct bind_ {
			typedef gala_graph_index_map<SGARGS> type;
			typedef gala_graph_index_map<SGARGS> const_type;
		};
	};

	template <>
	struct gala_graph_property_map<edge_index_t> {
		VCTtemplate
		struct bind_ {
			typedef gala_graph_eid_map type;
			typedef gala_graph_eid_map const_type;
		};
	};

	template <class Data, class DataRef, class GraphPtr>
	class gala_graph_data_map
	    : public put_get_helper<
	                  DataRef,
	                  gala_graph_data_map<Data, DataRef, GraphPtr> > { //
		public:
			typedef Data value_type;
			typedef DataRef reference;
			typedef unsigned long key_type; // ouch
			typedef lvalue_property_map_tag category;
			gala_graph_data_map(GraphPtr g) : m_g(g) { incomplete(); }
			template <class NodeOrEdge>
				DataRef operator[](NodeOrEdge x) const { incomplete(); return (*m_g)[x]; }
		protected:
			GraphPtr m_g;
	};

	template <>
	struct gala_graph_property_map<vertex_all_t> {
		VCTtemplate
		struct bind_ {
			typedef typename gala::graph<SGARGS>::vertex_type vtype;
			typedef gala_graph_data_map<vtype, const vtype&, gala::graph<SGARGS>*> type;
			typedef gala_graph_data_map<vtype, const vtype&,
					  const gala::graph<SGARGS>*> const_type;
		};
	};

	template <>
	struct gala_graph_property_map<edge_all_t> {
		VCTtemplate
		struct bind_ {
			typedef typename gala::graph<SGARGS>::edge_type etype;
			typedef gala_graph_data_map<etype, etype&, gala::graph<SGARGS>*> type;
			typedef gala_graph_data_map<etype, const etype&,
					  const gala::graph<SGARGS>*> const_type;
		};
	};

  // g++ 'enumeral_type' in template unification not implemented workaround
	template<galaPARMS, class Tag>
	struct property_map<gala::graph<SGARGS>, Tag> {
		typedef typename gala_graph_property_map<Tag>::template bind_<SGARGS> map_gen;
		typedef typename map_gen::type type;
		typedef typename map_gen::const_type const_type;
	};

	VCTtemplate
	inline typename property_map<gala::graph<SGARGS>, edge_all_t>::type
	get(edge_all_t, gala::graph<SGARGS> & g) { incomplete();
		typedef typename property_map<gala::graph<SGARGS>, edge_all_t>::type
			pmap_type;
		return pmap_type(&g);
	}

	VCTtemplate
	inline typename property_map<gala::graph<SGARGS>, edge_all_t>::const_type
	get(edge_all_t, const gala::graph<SGARGS>& g) { incomplete();
		typedef typename property_map<::gala::graph<SGARGS>, edge_all_t>::const_type
			pmap_type;
		incomplete();
		return pmap_type(&g);
	}

	VCTtemplate
	inline typename property_map<gala::graph<SGARGS>, vertex_all_t>::type
	get(vertex_all_t, gala::graph<SGARGS> & g) { incomplete();
		typedef typename property_map<::gala::graph<SGARGS>, vertex_all_t>::type
			pmap_type;
		return pmap_type(&g);
	}

	VCTtemplate
	inline typename property_map<::gala::graph<SGARGS>, vertex_all_t>::const_type
	get(vertex_all_t, const gala::graph<SGARGS>& g) { incomplete();
		typedef typename property_map<::gala::graph<SGARGS>, vertex_all_t>::const_type
			pmap_type;
		incomplete();
		return pmap_type(&g);
	}

#if 1
	VCTtemplate
	struct property_map<gala::graph<SGARGS>, vertex_index_t>{
		typedef gala_graph_index_map<SGARGS> type;
		typedef type const_type;
	};

	VCTtemplate
	inline typename property_map<gala::graph<SGARGS>, vertex_index_t>::const_type
	get(vertex_index_t, const gala::graph<SGARGS> &g)
	{
//		return pmap_type(&g);
		return gala_graph_index_map<SGARGS>(g);
	}
#else
	VCTtemplate
	struct property_map<gala::graph<SGARGS>, vertex_index_t>{
		typedef identity_property_map type;
		typedef type const_type;
	};

	VCTtemplate
	identity_property_map
	get(vertex_index_t, const gala::graph<SGARGS>&)
	{
		return identity_property_map();
	}
#endif

	template < galaPARMS, class PropertyTag, class Key>
	inline typename boost::property_traits<
	typename boost::property_map<gala::graph<SGARGS>, PropertyTag>::const_type
	>::value_type
	get(PropertyTag p, const gala::graph<SGARGS>& g, const Key& key) {
		return get(get(p, g), key);
	}

	VCTtemplate
	typename boost::graph_traits<gala::graph<SGARGS > >::vertices_size_type
	get(vertex_index_t t, const gala::graph<SGARGS>& g,
			typename boost::graph_traits< gala::graph<SGARGS> >::vertex_descriptor v)
	{ itested();
		return get(get(t, g), v);
	}

	VCTtemplate
	inline std::pair<typename boost::graph_traits< gala::graph<SGARGS> >::out_edge_iterator,
						  typename boost::graph_traits< gala::graph<SGARGS> >::out_edge_iterator >
						out_edges(typename boost::graph_traits< gala::graph<SGARGS> >::vertex_descriptor v,
								gala::graph<SGARGS> const& g)
	{
		typedef typename boost::graph_traits<gala::graph<SGARGS> >::out_edge_iterator Iter;
		::gala::graph<SGARGS>* G = const_cast<::gala::graph<SGARGS>*>(&g); // HACK
		return std::make_pair(Iter(v, G->out_edges(v).begin()), Iter(v, G->out_edges(v).end()));
	}
	VCTtemplate
	bool is_directed(gala::graph<SGARGS> const& g)
	{
		return g.is_directed();
	}
	VCTtemplate
	bool is_undirected(gala::graph<SGARGS> const& g)
	{ untested();
		return !g.is_directed();
	}

} // namespace boost


// HERE? requires boost... (bug?)
// FIX: only use include traits, not adj list!
#include <boost/graph/adjacency_list.hpp>
// does not work if vertex_descriptor is not contiguous
// should be obsolete with boost::copy_graph
VCTtemplate template<class H>
void gala::graph<SGARGS>::assign(H const& g)
{ untested();
//	size_t nv = boost::graph_traits<H>::num_vertices(g);
	typedef gala::graph<SGARGS> G;
	size_t nv = boost::num_vertices(g);
	size_t ne = boost::num_edges(g);

	typedef typename boost::graph_traits<gala::graph<SGARGS > >::vertices_size_type st;
	size_t L = std::numeric_limits<st>::max();
	assert(boost::num_vertices(g) <= L); (void) L;

	if(_v.size()){ untested();
		_v.resize(0);
	}else{ untested();
	}
	_v.resize(nv);
	vertex_type map[nv];
	size_t i=0;
	_num_edges = 0;

	// not necessary for vector...
//	vertex_iterator v=begin();
	// for(unsigned i=0; i<nv; ++i){ //  v=begin(); v!=end(); ++v)
	for(iterator v=begin(); v!=end(); ++v) { untested();
		// for(auto& v : vertices())
		map[i] = G::iter::deref(v);
		++i;
	}
#ifndef NDEBUG
 	if(i!=nv){ untested();
		std::cerr << "oops2 " << i << " " << nv << "\n";
	}
	assert(i==nv);
#endif

	typename boost::graph_traits<H>::edge_iterator eIt, eEnd;
	for(boost::tie(eIt, eEnd) = boost::edges(g); eIt != eEnd; ++eIt){ // untested();
		if(boost::source(*eIt, g) >= num_vertices()){ untested();
			assert(false);
			// unreachable?
		}else if(boost::target(*eIt, g) >= num_vertices()) { untested();
			assert(false);
			// unreachable?
		}else{ untested();
// -Wno-uninitialized!?
			add_edge(map[boost::source(*eIt, g)],
						map[boost::target(*eIt, g)]);
		}
		// ++_num_edges;
	}
	if(_num_edges == 2*ne){ untested();
		_num_edges = ne; // ??1
	}else if(_num_edges < ne){ untested();
//		possibly assigning directed graph.
//		getting fewer edges
		assert(_num_edges<ne);
	}else{ untested();
		assert(_num_edges == ne);
	}
	assert(num_vertices() == nv);


#if 0 // not yet
	for(vertex_& i : vertices()){ untested();
		assert(is_valid(&i));
		for(auto& j : vertices()){ untested();
			assert(is_edge(&j,&i) == is_edge(&i,&j));
		}
	}
#endif
}


#endif

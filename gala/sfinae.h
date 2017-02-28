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
 * identify container types ins template specialization.
 *
 * TODO: inherited types.
 * http://stackoverflow.com/questions/15616730/cannot-trait-an-inherited-class-as-base-with-sfinae
 *
 */

#ifndef GALA_SFINAE_H
#define GALA_SFINAE_H

#include <deque>
#include <set>
// #ifdef HAVE_STX_BTREE_SET
#include <stx/btree_set.h>
// #endif
#include <boost/container/flat_set.hpp>
#include <vector>
#include <unordered_set>
#include "trace.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace gala{
namespace sfinae{
/*--------------------------------------------------------------------------*/
struct any{
	int dummy; // required for size!=0?
	bool operator==(const any&)const{ unreachable(); return true; }
	typedef int value_type; // dummy.
};
/*--------------------------------------------------------------------------*/
}
}
/*--------------------------------------------------------------------------*/
namespace std {
template<>
struct hash<gala::sfinae::any>{
	size_t operator()(const gala::sfinae::any&)const{ unreachable(); return 0; }
};
} // std
/*--------------------------------------------------------------------------*/
namespace gala{
namespace sfinae{
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class A, class B=any, class T=void, class...>
struct is_set{
	static constexpr bool value = false;
};
/*--------------------------------------------------------------------------*/
template<class S, class T>
struct is_set<S, typename std::enable_if < std::is_same<
std::set<typename S::value_type, typename S::key_compare, typename S::allocator_type >, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;

};
//#ifdef HAVE_BTREE_SET...
template<class S, class T>
struct is_set<S, typename std::enable_if < std::is_same<
stx::btree_set<typename S::value_type, typename S::key_compare, typename S::allocator_type >, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;
};
//#endif
template<class S, class T>
struct is_set<S, typename std::enable_if < std::is_same<
boost::container::flat_set<typename S::value_type, typename S::key_compare, typename S::allocator_type >, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class A, class B=any, class T=void>
struct is_seq{
	static constexpr bool value = false;
};
/*--------------------------------------------------------------------------*/
template<class S, class T>
struct is_seq<S, typename std::enable_if < std::is_same<
std::vector<any, typename S::allocator_type >, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;
};
template<class S, class T>
struct is_seq<S, typename std::enable_if < std::is_same<
std::deque<any, typename S::allocator_type >, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class A, class B=any, class T=void>
struct is_hash{
	static constexpr bool value = false;
};
/*--------------------------------------------------------------------------*/
template<class S, class T>
struct is_hash<S, typename std::enable_if < std::is_same<
std::hash<typename S::value_type>, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class A, class B=any, class T=void>
struct is_vector{
	static constexpr bool value = false;
};
/*--------------------------------------------------------------------------*/
template<class S, class T>
struct is_vector<S, typename std::enable_if < std::is_same<
std::vector<typename S::value_type, typename S::allocator_type >, S
>::value, any >::type , T>{ //

	typedef T type;
	static constexpr bool value = true;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// convenience wrappers for non-determined types
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class C>
struct is_set_tpl : is_set<C<any> > {};
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class C>
struct is_seq_tpl : is_seq<C<any> > {};
/*--------------------------------------------------------------------------*/
template<template<class T, typename... > class C>
struct is_vec_tpl : is_vector<C<any> > {};
/*--------------------------------------------------------------------------*/

}//sfinae
} // namespace gala

#endif

// Felix Salfelder 2016
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option) any
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
// basig tracing. inspired by gnucap and incomplete.
//
// define
//    TRACE_UNTESTED to trace untested calls
//    TRACE_ITESTED to trace interactively tested calls
//    DO_TRACE to trace program flow
//
//
#include <iostream>
#include <vector>

#ifndef HP_hp
#define HP_hp
class hp{
	intptr_t p;
	public:
	hp(const void* x){ //
		p = (intptr_t)x %30011;
	}
	operator int(){ //
		return static_cast<int>(p);
	}
};
#endif

#ifndef incomplete
#define incomplete() \
	std::cout << "incomplete " << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n"
#endif

#ifndef unreachable
#define unreachable() \
	std::cerr << "unreachable " << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n"
#endif

// interactively tested
#undef itested
#ifdef TRACE_ITESTED
#include <iostream>
#define itested() ( std::cerr << "@@#\n@@@:" \
     << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )
#else
#define itested()
#endif

#undef untested
#ifdef TRACE_UNTESTED
#include <iostream>
#define untested() ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
          <<":" << __func__ << "\n" )
#else
#define untested()
#endif

#ifndef USE
#define USE(x) (1)?(void)(0):(void)(x)
#endif

#undef trace0
#undef trace1
#undef trace2
#undef trace3
#undef trace4
#undef trace5
#undef trace6

#ifdef DO_TRACE
#define trace0(s) ( std::cerr << "@#@" << (s) << "\n")
#define trace1(s,x) ( \
		std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << std::endl )
#define trace2(s,x,y) ( \
		std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << std::endl )
#define trace3(s,x,y,z) ( \
		std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << std::endl )
#define trace4(s,x,y,z,w) ( \
		std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "  " << #w << "=" << (w)  \
		     << std::endl )
#define trace5(s,x,y,z,w,u) ( \
		std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "  " << #w << "=" << (w)  \
		     << "  " << #u << "=" << (u)  \
		     << std::endl )
#define trace6(s,x,y,z,w,u,t) ( \
		std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "  " << #w << "=" << (w)  \
		     << "  " << #u << "=" << (u)  \
		     << "  " << #t << "=" << (t)  \
		     << std::endl )
#else
#define trace0(s) (USE(s))
#define trace1(s,x) (USE(s),USE(x))
#define trace2(s,x,y) USE(s);USE(x);USE(y)
#define trace3(s,x,y,z) USE(s);USE(x);USE(y);USE(z)
#define trace4(s,x,y,z,w) USE(s);USE(x);USE(y);USE(z);USE(w)
#define trace5(s,x,y,z,w,u) USE(s);USE(x);USE(y);USE(z);USE(w);USE(u)
#define trace6(s,x,y,z,w,u,t) USE(s);USE(x);USE(y);USE(z);USE(w);USE(u);USE(t)
#endif

#ifndef TRACE_H
#define TRACE_H
# ifndef NDEBUG
template <class C>
inline bool contains(C const& c, typename C::value_type const& v)
{ untested();
	return(c.find(v) != c.end());
}
template <class X>
inline bool contains(std::vector<X> const& c, X const& v)
{ untested();
	typedef typename std::vector<X>::const_iterator it;

	for(it i=c.begin(); i!=c.end(); ++i){ untested();
		if(v == *i) return true;
	}
	return false;
}
# endif
#endif

/*
 * Copyright (C) 2017 Felix Salfelder
 * Authors: Felix Salfelder
 *
 * This file is part of "freetdi", the free tree decomposition intiative
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
 *
 * bitvector based set implementation.
 */
#pragma once

#include <tdlib/trace.hpp>
#include "ta_hash.h" // BUG

/*--------------------------------------------------------------------------*/
// TODO: size/storage control
// - first-set-bit?
// - fixed-K
// - more stl compliance
// - rename

#define BSDarg unsigned W, \
  typename CHUNK_T, \
  typename HMT, \
  typename OST, \
  typename SCT
#define BSDt  template<BSDarg>
#define BSDto template<BSDarg ## 2>
#define BSDa  W, CHUNK_T, HMT, OST, SCT
#define BSDa1 W, CHUNK_T, HMT, OST, SCT1
#define BSDa2 W, CHUNK_T, HMT, OST, SCT2
#undef lassert
#define lassert(x) assert(x)

#ifdef NDEBUG
//#undef unreachable
//#define unreachable() __builtin_unreachable()
#endif

namespace cbset{

namespace bits{
// fixme: upper end missing?
template<class CT>
static inline void shiftBy(CT* s, unsigned dist, unsigned howmany=-1u)
{ untested();
  assert(dist>0);
  assert(dist);
  if(howmany==-1u){ untested();
#ifndef NDEBUG
    incomplete();
#endif
  }else{ untested();
    howmany += dist;
  }

  assert(howmany>0);
  for(unsigned x=howmany-1; x+1 != dist; --x) { untested();
    unsigned tgt=x;
    if(tgt>howmany) break;
    trace3("shift", x, x-dist, int(s[x-dist]));
    s[x] = s[x-dist];
  }

  // lower pad;
  for(unsigned x=0; x<dist; ++x) { untested();
    s[x] = 0;
  }
}
} // bits

/*--------------------------------------------------------------------------*/
//template<class S>
//static inline bool contains(S const& s, typename S::value_type i);
template<class S>
static inline bool contains(S const& s, typename S::value_type i)
{
  assert(i<::n);
  return s.contains(i);
}
/*--------------------------------------------------------------------------*/
template<class T>
unsigned numberofones(T x)
{ untested();
#if 1 // GCC
  return __builtin_popcount(x);
#else
  unsigned r=0;
  while(x){ untested();
    r+=x&1;
    x=x>>1;
  }
  return r;
#endif
}
/*--------------------------------------------------------------------------*/
template<>
unsigned numberofones(uint64_t x)
{ untested();
#if 1 // GCC
  return __builtin_popcountl(x);
#else
  unsigned r=0;
  while(x){ untested();
    r+=x&1;
    x=x>>1;
  }
  return r;
#endif
}
/*--------------------------------------------------------------------------*/
struct nosize_t{
  nosize_t(int){ untested();
  }
  bool operator!=(const nosize_t&)const{ untested();
    return false;
  }
};
/*--------------------------------------------------------------------------*/
struct nooffset_t{
  nooffset_t(int){ untested();
  }
  operator unsigned() const { return 0; }
};
/*--------------------------------------------------------------------------*/
struct nohowmany_t{
  nohowmany_t(){}
  nohowmany_t(int){ untested();
  }
  bool operator==(unsigned)const{ untested();
    return false;
  }
};
/*--------------------------------------------------------------------------*/
template<unsigned W,
         typename CHUNK_T=uint8_t,
         typename OST=uint8_t,
         typename HMT=uint8_t,
	 typename SCT=unsigned>
struct BSET_DYNAMIC;
/*--------------------------------------------------------------------------*/
namespace detail{
/*--------------------------------------------------------------------------*/
BSDt
struct cbshelp{
  static unsigned get_size(BSET_DYNAMIC<BSDa> const&, unsigned size)
  { itested();
    return size;
  }
  static void set_size(SCT& x, unsigned size)
  { untested();
    x = size;
  }
};
/*--------------------------------------------------------------------------*/
} // detail
/*--------------------------------------------------------------------------*/
namespace detail{
/*--------------------------------------------------------------------------*/
template<class OST>
struct ofhelp{
  typedef OST type;
  template<class H>
  static const H& get(H const& h, unsigned W){ untested();
    return h;
  }
  template<class H>
  static void set(type& h, H x){ untested();
    h=x;
  }
  static constexpr bool active=true;
};
/*--------------------------------------------------------------------------*/
template<>
struct ofhelp<nooffset_t>{
  typedef unsigned type;
  template<class W>
  static W get(nooffset_t h, W w){ untested();
    return w;
  }
  template<class H>
  static void set(nooffset_t&, H){ untested();
  }
  static constexpr bool active=false;
};
/*--------------------------------------------------------------------------*/
template<class HMT>
struct hmhelp{
  typedef HMT type;
  typedef HMT const& ref;
  template<class H>
  static const H& get(H const& h, unsigned){ untested();
    return h;
  }
  template<class H>
  static type& set(type& h, H x){ untested();
    h = x;
    return h;
  }
  template<class D>
  static void trim(HMT& h, D const* d){ untested();
    for(;h && !d[h-1]; --h);
  }
  template<class D>
  static void trim_(HMT& h, D const* d){ untested();
    for(;!d[h-1]; --h);
  }
  template<class X, class D>
  static void pad(type& h, X const& x, D* d){ untested();
    for(; h<=x; ++h){ untested();
      d[h] = 0;
    }
  }
  template<class D>
  static void clear(D*, unsigned){
  }
  static constexpr bool active=true;
};
/*--------------------------------------------------------------------------*/
template<>
struct hmhelp<nohowmany_t>{
  typedef unsigned type;
  typedef unsigned ref;
  template<class W>
  static W get(nohowmany_t h, W w){ untested();
    return w;
  }
  template<class H>
  static void set(nohowmany_t&, H){ untested();
  }
  template<class D>
  static void trim(nohowmany_t&, D*){ untested();
  }
  template<class D>
  static void trim_(nohowmany_t&, D*){ untested();
  }
  template<class X, class D>
  static void pad(nohowmany_t& h, X x, D* d){ untested();
  }
  template<class D>
  static void clear(D* d, unsigned n){
    for(unsigned i=0; i<n; ++i){
      d[i]=0;
    }
  }
  static constexpr bool active=false;
};
/*--------------------------------------------------------------------------*/
} // detail
/*--------------------------------------------------------------------------*/
template<class S1, class S2>
class lazy_intersection;
/*--------------------------------------------------------------------------*/
BSDt
struct BSET_DYNAMIC{ //
public:
  constexpr static unsigned CHUNKBITS=8*sizeof(CHUNK_T);
  constexpr static unsigned max_element=8*sizeof(CHUNK_T)*W-1;
  typedef unsigned value_type;
  typedef OST offset_type;
  typedef enum{setminus} _setminus_t;
  typedef enum{sqcup} _sqcup_t;
  typedef enum{cup} _cup_t;
  typedef enum{cap} _cap_t;
public:
  static bool use_offset(){ return detail::ofhelp<OST>::active; }
  static bool use_howmany(){ return detail::hmhelp<HMT>::active; }
public:
  BSET_DYNAMIC()
    : _howmany(0),
      _offset(0),
      _size(0) { untested();
  }
  BSET_DYNAMIC(BSET_DYNAMIC const& o) :
      _howmany(o._howmany), _offset(o._offset), _size(o._size) { untested();
    trace2("copying from", o, unsigned(o.howmany()));
    trace1("", uintptr_t(o.next())-uintptr_t(&o));
    memcpy(_d, o._d, howmany()*sizeof(CHUNK_T));
    trace2("copied.", *this, o.howmany());
    assert(howmany()<=W); // for now.
    assert(size()==o.size()); // for now.
    assert(size()<=::n); // for now.
  }
public:
  template<class SCT2>
  BSET_DYNAMIC(const BSET_DYNAMIC& s, const BSET_DYNAMIC<BSDa2>& t, _setminus_t);
  template<class SCT2>
  BSET_DYNAMIC(const BSET_DYNAMIC& s, const BSET_DYNAMIC<BSDa2>& t, _cup_t);
  template<class SCT2>
  BSET_DYNAMIC(const BSET_DYNAMIC& s, const BSET_DYNAMIC<BSDa2>& t, _sqcup_t);
  template<class SCT2>
  BSET_DYNAMIC(const BSET_DYNAMIC& s, const BSET_DYNAMIC<BSDa2>& t, _cap_t);
public:
  class const_iterator{
  public:
    const_iterator(unsigned x, BSET_DYNAMIC const& s) :
      _i(x), _s(s)
    { untested();
      assert(x>=CHUNKBITS*s.offset());

      if(_i==-1u){ untested();
//      }else if(_i>=CHUNKBITS*(s._offset+s._howmany)){ untested();
//	unreachable();
//        _i = -1u; // CHUNKBITS*(s._offset+s._howmany);
//	_c = 0;
      }else if(s.howmany()==0){ untested();
	// skip to end...
	_i=-1u;
      }else{ untested();
	// assert(_i<CHUNKBITS*(s._offset+s._howmany));
	_c = _s._d[(x/CHUNKBITS)-_s.offset()] >> (x%CHUNKBITS);
	if(_c){ untested();
	  // newonb is not trimmed...
	  // need to iterate anyway.
	  unsigned ctz;
	  if(CHUNKBITS>32){
	    ctz = __builtin_ctzl(_c);
	  }else{
	    ctz = __builtin_ctz(_c);
	  }
	  _i += ctz;
	  _c = _c >> ctz;
	}else{ untested();
	}
	skip();
      }
      // trace2("ci", x, int(_s._howmany));
      // trace2("ci", n, int(_s._offset));
      // trace2("ci", int(_c), int(_s._d[0]));
    }

    bool operator==(const_iterator const& o) const{return _i==o._i;}
    bool operator!=(const_iterator const& o) const{return _i!=o._i;}
    unsigned operator*() const{return _i;}
    const_iterator& operator++(){ itested();
      assert(_i!=-1u);
      _c = _c+0;
      inc();
      skip();
      // trace1("++skipped to", _i);
      // trace1("++skipped to", (int)_c);
      return *this;
    }

private:
    unsigned _i;
    CHUNK_T _c;
private:
    void /*const_iterator::*/inc(){ itested();
      ++_i;
//      assert(_i <= CHUNKBITS*(_s._howmany+_s.offset()));
      if(_i >= CHUNKBITS*(_s.howmany()+_s.offset())){ untested();
	_i = -1u; // CHUNKBITS*(W+_s.offset());
      }else if(_i%CHUNKBITS){ itested();
        _c = _c >> 1;
      }else{ itested();
        assert(_i/CHUNKBITS>=_s.offset());
        _c = _s._d[_i/CHUNKBITS-_s.offset()];
	if(_c){ untested();
	  unsigned ctz;
	  if(CHUNKBITS>32){
	    ctz = __builtin_ctzl(_c);
	  }else{
	    ctz = __builtin_ctz(_c);
	  }
	  _i += ctz;
	  _c = _c >> ctz;
	}
      }
    }
    void /*const_iterator::*/skip(){ itested();
      while(_i<CHUNKBITS*(_s.howmany()+_s.offset())){ itested();
        if (_c & 1){ itested();
          break;
        }else{ itested();
        }
	_c = _c+0;
        inc();
      }
      //trace3("doneskip", _i, int(_s.howmany()), int(_s.offset()));
    }
    BSET_DYNAMIC const& _s;
  };
  class intersection_iterator{ //
  public:
    intersection_iterator(BSET_DYNAMIC const& s, BSET_DYNAMIC const& t, unsigned)
      : _i(CHUNKBITS*std::min(s.howmany()+s.offset(), t.howmany()+t.offset()))
    { untested();
      trace1("ints end", _i);
    }
    intersection_iterator(BSET_DYNAMIC const& s, BSET_DYNAMIC const& t)
      : _s((s.offset()<=t.offset())?&t:&s),
	_t((s.offset()<=t.offset())?&s:&t),
	_delta(_s->offset()-_t->offset())
    { untested();
      _i = _s->offset();
      unsigned M=std::min(_s->howmany()+_s->offset(), _t->howmany()+_t->offset());
      if(_i>M){ untested();
	_i=M;
      }
      _i *= CHUNKBITS;
      trace1("start", _i);
#ifndef NDEBUG
      if(!_s->howmany()){ untested();
	incomplete();
      }else if(!t.howmany()){ untested();
	incomplete();
      }
#endif

      _c = _s->_d[0];
      if(_delta<_t->howmany()){ untested();
	_c &= _t->_d[_delta];
      }else{ untested();
      }

      skip();
    }

    bool operator==(intersection_iterator const& o) const{return _i==o._i;}
    bool operator!=(intersection_iterator const& o) const{return _i!=o._i;}
    unsigned operator*() const{return _i;}
    intersection_iterator& operator++(){ untested();
      assert(_s);
      assert(_t);
      unsigned M=std::min(_s->howmany()+_s->offset(), _t->howmany()+_t->offset());
      assert(_i<CHUNKBITS*M);
      _c = _c+0;
      inc();
      skip();
      return *this;
    }

  private:
    void inc(){ untested();
      ++_i;
      assert(_i <= CHUNKBITS*(_s->howmany()+_s->offset())
           ||_i <= CHUNKBITS*(_t->howmany()+_t->offset()));

      if(_i%CHUNKBITS){ untested();
        _c = _c >> 1;
      }else{ untested();
       	if(_i/CHUNKBITS-_s->offset() < _s->howmany()){ untested();
	  _c = _s->_d[_i/CHUNKBITS-_s->offset()];
	}else{ untested();
	  _c = 0;
	}
       	if(_i/CHUNKBITS-_t->offset() < _t->howmany()){ untested();
	   _c &= _t->_d[_i/CHUNKBITS-_t->offset()];
	}else{ untested();
	}
      }
    }
    void skip(){ untested();
      unsigned M=std::min(_s->howmany()+_s->offset(), _t->howmany()+_t->offset());
      while(_i<CHUNKBITS*M){ untested();
        if (_c & 1){ untested();
          break;
        }else{ untested();
        }
        inc();
      }
    }
    unsigned _i;
    CHUNK_T _c;
    BSET_DYNAMIC const* _s;
    BSET_DYNAMIC const* _t;
    unsigned _delta;
  }; // intersection_iterator
  class union_iterator{
  public:
    union_iterator(BSET_DYNAMIC const& s, BSET_DYNAMIC const& t, unsigned)
      : _i(CHUNKBITS*std::max(s.offset()+s.howmany(), t.offset()+t.howmany()))
    { untested();
    }
    union_iterator(BSET_DYNAMIC const& s, BSET_DYNAMIC const& t)
      : _s((s.offset()<=t.offset())?&s:&t),
	_t((s.offset()<=t.offset())?&t:&s),
	_delta(_t->offset()-_s->offset())
    { untested();
	_i=(_s->offset()*CHUNKBITS);
      if(!s.howmany()){ untested();
	_c = 0;
      }else if(!t.howmany()){ untested();
	_c = 0;
      }else{ untested();
	_c = _s->_d[0] | _t->_d[_delta];
      }

      skip();
    }

    bool operator==(union_iterator const& o) const{return _i==o._i;}
    bool operator!=(union_iterator const& o) const{return _i!=o._i;}
    unsigned operator*() const{return _i;}
    union_iterator& operator++(){ untested();
      assert(_s);
      assert(_t);
      unsigned M=std::max(_s->howmany()+_s->offset(), _t->howmany()+_t->offset());
      assert(_i<CHUNKBITS*M);
      inc();
      skip();
      return *this;
    }

  private:
    void inc(){ untested();
      ++_i;
      assert(_i <= CHUNKBITS*(_s->howmany()+_s->offset())
           ||_i <= CHUNKBITS*(_t->howmany()+_t->offset()));

      assert(_i!=-1u);
      if(_i%CHUNKBITS){ untested();
        _c = _c >> 1u;
      }else{ untested();
       	if(_i/CHUNKBITS-_s->offset() < _s->howmany()){ untested();
	  _c = _s->_d[_i/CHUNKBITS-_s->offset()];
	}else{ untested();
	  _c = 0;
	}

       	if(_i/CHUNKBITS-_t->offset() < _t->howmany()){ untested();
	   _c |= _t->_d[_i/CHUNKBITS-_t->offset()];
	}else{ untested();
	}
      }
    }
    void skip(){ untested();
      unsigned M=std::max(_s->howmany()+_s->offset(), _t->howmany()+_t->offset());
      while(_i<CHUNKBITS*M){ itested();
        if (_c & 1){ itested();
          break;
        }else{ itested();
        }
        inc();
      }
    }
    unsigned _i;
    CHUNK_T _c;
    BSET_DYNAMIC const* _s;
    BSET_DYNAMIC const* _t;
    unsigned _delta;
  };
  bool contains(value_type i) const;
  bool operator==(BSET_DYNAMIC const& t) const;
  bool operator!=(BSET_DYNAMIC const& t) const{ untested();
    return !(*this == t);
  }
  bool is_subset_of(BSET_DYNAMIC const& t) const;
  unsigned size() const { untested();
    return detail::cbshelp<BSDa>::get_size(*this, _size);
  }
  unsigned empty() const { untested();
    return !size();
  }
  unsigned recount() const;
  unsigned long hash() const;
  template<class SCT2>
  bool intersects(BSET_DYNAMIC<BSDa2> const& t) const;
public: // modify
  void insert(value_type i);
  void add(value_type i);
  template<class S>
  void add_sorted_sequence(S const& s);
  template<class S>
  void remove_sorted_sequence(S const& s);
  void intersect(BSET_DYNAMIC const& t);
  void carve(const BSET_DYNAMIC& t);
  void subtract(BSET_DYNAMIC const& t);
  template<class SCT2>
  void merge(BSET_DYNAMIC<BSDa2> const& t);
  value_type front() const;
  value_type back() const;
  void clear(){ untested();
    _howmany = 0;
    _offset = 0;
    _size = 0;
    detail::hmhelp<HMT>::clear(_d, W);
  }
public: // dangerous
  void erase(value_type i);
  void trim_below();
public: // protect and friends?
  void set_offset(offset_type x){ untested();
    _offset=x;
  }
  void set_size(unsigned s){ untested();
    detail::cbshelp<BSDa>::set_size(_size, s);
  }
public: // iter
  const_iterator begin() const{ itested();
    assert(offset()==offset());
    auto I=const_iterator(CHUNKBITS*offset(), *this);
    return I;
  }
  const_iterator end() const{ untested();
    return const_iterator(-1u, *this);
    return const_iterator(CHUNKBITS*(offset()+W), *this);
    return const_iterator(CHUNKBITS*(offset()+howmany()), *this);
  }
  OST offset()const { untested();
    if(_offset<W){ untested();
      // ok
    }else{ untested();
    }
    return _offset;
  }
public: // howmany stuff
  typename detail::hmhelp<HMT>::ref const howmany()const { untested();
    return detail::hmhelp<HMT>::get(_howmany, W);
  }
private:
  void trim_tail(){ untested();
    detail::hmhelp<HMT>::trim(_howmany, _d);
  }
  void set_howmany(unsigned h){ untested();
    detail::hmhelp<HMT>::set(_howmany, h);
  }
public:
  void* next()const { untested();
    trace4("next", sizeof(HMT), sizeof(OST), sizeof(SCT), sizeof(CHUNK_T));
    return (void*)(uintptr_t(this)
	+ sizeof(HMT) + sizeof(OST) + sizeof(SCT)
	+ howmany()*sizeof(CHUNK_T));
  }
  void check() const{ untested();
#ifndef NDEBUG
    if(howmany()){ untested();
      assert(_d[0] == _d[0]);
      assert(_d[howmany()-1] == _d[howmany()-1]);
    }
    assert(_offset<=W);
    assert(size() == recount());
    for(auto const& v: *this){ untested();
      assert(contains(v));
    }
#endif
  }
private:
  template<class SCT1, class SCT2>
  void merge(BSET_DYNAMIC<BSDa1> const&,
      BSET_DYNAMIC<BSDa2> const&, unsigned disjoint=0);
private:
  HMT _howmany;
  OST _offset;
  SCT _size;
  CHUNK_T _d[W];
public:
  template<unsigned W_, typename CHUNK_T_, typename H_, typename O_, class SCT_>
  friend struct BSET_DYNAMIC;
  template<class A, class B>
  friend class lazy_intersection; // too many?!
}; // BSET_DYNAMIC
/*--------------------------------------------------------------------------*/
namespace detail{
/*--------------------------------------------------------------------------*/
template<unsigned W, typename CHUNK_T, typename HMT, typename OST>
struct cbshelp<W, CHUNK_T, HMT, OST, nosize_t>{
  static unsigned get_size(
      BSET_DYNAMIC<W, CHUNK_T, HMT, OST, nosize_t> const& n, nosize_t)
  { untested();
    return n.recount();
  }
  static void set_size(nosize_t x, unsigned size)
  { untested();
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class window_t, class chunk_t>
struct cnt{
  static constexpr unsigned size=sizeof(window_t);
  static constexpr unsigned chunksize=sizeof(chunk_t);

  template<class D, class W, class H>
  static unsigned count_bits(D const* dd, W& w, H h){
    unsigned k=0;

    if(sizeof(chunk_t)<=size){ untested();
      for (; w+(size/chunksize)<=h;){ untested();
	window_t d;
	memcpy(&d, dd+w, size);
	assert(k<=::n);
	k += numberofones(d);
	assert(k<=::n);

	w += size/chunksize;
      }
      trace3("done64", w, h, k);
    }
    return k;
  }
/*--------------------------------------------------------------------------*/
}; // cnt
/*--------------------------------------------------------------------------*/
} // detail
/*--------------------------------------------------------------------------*/
BSDt
inline unsigned BSET_DYNAMIC<BSDa>::recount() const
{ untested();
// no, carve does not trim...
//   if(_howmany){ untested();
//     assert(_d[0]);
//     assert(_d[_howmany-1]);
//   }
  assert(sizeof(long)==8);
  assert(sizeof(unsigned)==4);
  assert(sizeof(short)==2);

  unsigned k = 0;
  unsigned w = 0;

  k += detail::cnt<unsigned long, CHUNK_T>::count_bits(_d, w, howmany());
  k += detail::cnt<unsigned, CHUNK_T>::count_bits(_d, w, howmany());
  k += detail::cnt<unsigned short, CHUNK_T>::count_bits(_d, w, howmany());
  k += detail::cnt<unsigned char, CHUNK_T>::count_bits(_d, w, howmany());

  assert(k<=::n);
  assert(k<=max_element+1);
  return k;
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void subtract(BSET_DYNAMIC<BSDa>& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  return s.subtract(t);
}
/*--------------------------------------------------------------------------*/
BSDt
inline void BSET_DYNAMIC<BSDa>::subtract(BSET_DYNAMIC const& t)
{ untested();

  unsigned i=std::max(offset(), t.offset());
  for (; i<offset()+howmany() && i<t.offset()+t.howmany(); i++) { untested();
    _d[i-offset()] &= ~t._d[i-t.offset()];
  }
}
/*--------------------------------------------------------------------------*/
template<BSDarg, typename SCT2>
static inline BSET_DYNAMIC<BSDa> diff(BSET_DYNAMIC<BSDa> const& s,
    BSET_DYNAMIC<BSDa2> const& t)
{ untested();
  s.check();
  t.check();
  trace1("", s);
  trace1("", t);
  return BSET_DYNAMIC<BSDa>(s, t, BSET_DYNAMIC<BSDa>::setminus);
}
/*--------------------------------------------------------------------------*/
// construct s\t
BSDt
template<class SCT2>
BSET_DYNAMIC<BSDa>::BSET_DYNAMIC(const BSET_DYNAMIC& s,
    const BSET_DYNAMIC<BSDa2>& t, BSET_DYNAMIC::_setminus_t)
  : _offset(s._offset), _size(s._size)
{ untested();
#ifndef NDEBUG
  set_howmany(0);
  for(unsigned i=0; i<W; ++i){
    _d[i] = 0;
  }
#endif

    assert(s.size()==s.recount());
    assert(t.size()==t.recount());
  int delta=s._offset-t._offset;
  if(delta>=0){ untested();
  }else{ untested();
  }

  int i=0;
  // not reached from t.
  for (; int(i)<-delta && unsigned(i)<s.howmany(); i++) { untested();
    _d[i] = s._d[i];
  }

  for (; unsigned(i)<s.howmany() && (i+delta)<int(t.howmany()); i++) { untested();
    assert(i+delta>=0);
    set_size(size() - numberofones(s._d[i] & t._d[i+delta]));
    _d[i] = s._d[i] & ~t._d[i+delta];
  }

  assert(i>=0);

  for (; unsigned(i)<s.howmany(); i++) { untested();
    assert(i>=0);
    assert(i+delta>=0);
    _d[i] = s._d[i];
  }
  set_howmany(i);

  trim_tail();

  assert(howmany()<=W); // for now.
  trim_below(); // inefficient... (but works)

  assert(size()==recount());
  check();
}
/*--------------------------------------------------------------------------*/
BSDt
void BSET_DYNAMIC<BSDa>::carve(const BSET_DYNAMIC& t)
{ untested();
  assert(size() == recount());
  assert(t.size() == t.recount());
  int delta=_offset-t._offset;

  unsigned i;
  // not reached from t.
#if 1
  i = 0;
  for (; int(i)<-delta && i<howmany(); i++);
#else

  if(delta>=0){ untested();
    i = 0;
  }else if(-delta<int(howmany())){ untested();
    i = -delta;
  }else{ untested();
    i = howmany();
  }
#endif

  for (; i<howmany() && i+delta<t.howmany() ; i++) { untested();
    assert(i>=0);
    assert(i+delta>=0);
    set_size(size() - numberofones(_d[i] & t._d[i+delta]));
    //trace4(" ", unsigned(_d[i]), unsigned(t._d[i+delta]), _size, unsigned(_d[i] & t._d[i+delta]));
    _d[i] &= ~t._d[i+delta];
  }

  for (; i<howmany(); i++) { untested();
    assert(i>=0);
    assert(i+delta>=0);
  }
  assert(size()==recount());

#if 0
  for(; _howmany && !_d[_howmany-1];){ untested();
    --_howmany;
  }
#else
  trim_tail();
#endif
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void unionWith(BSET_DYNAMIC<BSDa>& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  trace1("uw", s);
  trace1("uw", t);
  assert(s.size()==s.recount());
  assert(t.size()==t.recount());
  s.merge(t);
  assert(s.size()==s.recount());
}
/*--------------------------------------------------------------------------*/
BSDt
std::ostream& operator<<(std::ostream& o, BSET_DYNAMIC<BSDa> const& s);
// union.
BSDt
template<class SCT2>
BSET_DYNAMIC<BSDa>::BSET_DYNAMIC(const BSET_DYNAMIC& s,
    const BSET_DYNAMIC<BSDa2>& t, BSET_DYNAMIC::_cup_t)
  : _offset(t._offset), _size(0)
{ untested();
#ifndef NDEBUG
  set_howmany(0);
  for(unsigned i=0; i<W; ++i){
    _d[i] = 0;
  }
#endif
  merge(s, t);
  assert(size()==recount());
}
/*--------------------------------------------------------------------------*/
BSDt
std::ostream& operator<<(std::ostream& o, BSET_DYNAMIC<BSDa> const& s);
/*--------------------------------------------------------------------------*/
// union.
BSDt
template<class SCT2>
BSET_DYNAMIC<BSDa>::BSET_DYNAMIC(const BSET_DYNAMIC& s,
    const BSET_DYNAMIC<BSDa2>& t, BSET_DYNAMIC::_sqcup_t)
  : _offset(t._offset), _size(0)
{ untested();
#ifndef NDEBUG
  set_howmany(0);
  for(unsigned i=0; i<W; ++i){
    _d[i] = 0;
  }
#endif
  merge(s, t, 1);
  assert(_size==recount());
}
/*--------------------------------------------------------------------------*/
BSDt
template<class SCT2>
inline void BSET_DYNAMIC<BSDa>::merge(BSET_DYNAMIC<BSDa2> const& t)
{ untested();
  assert(size()==recount());
  assert(t.size()==t.recount());
#ifndef NDEBUG
  for(auto const& v: *this){ untested();
    assert(contains(v));
  }
#endif
  BSET_DYNAMIC<BSDa> bak(*this);
  trace1("merge", bak);
#ifndef NDEBUG
  for(auto const& v: *this){ untested();
    assert(v<::n);
  }
#endif
  int delta=_offset-t._offset;

  unsigned char newhowmany=0;
  if(t.size()==0)
#if 0
  if(!t._howmany)
#endif
  { untested();
    return;
  }else if(_offset>t._offset){ itested();
      unsigned delta=_offset-t._offset;

      newhowmany = t.howmany();
      unsigned x=t.howmany()-1;
      for(; x>howmany()+unsigned(delta-1); --x) { untested();
	set_size(size() + numberofones(t._d[x]));
        _d[x] = t._d[x];
      }

      // pure shift
      x = howmany()+unsigned(delta-1);
      if(howmany()+delta>newhowmany){ untested();
        newhowmany = howmany()+delta;
      }else{ untested();
      }
      CHUNK_T* dd=(CHUNK_T*)(_d-delta);
      for(; x>=unsigned(delta) && x>=t.howmany(); --x) { untested();
        _d[x] = dd[x];
      }

      for(; x>=t.howmany(); --x) { untested();
        _d[x] = 0;
      }

      // overlap
      assert(t.howmany());
      x = t.howmany()-1;
      if(x>unsigned(howmany()+delta-1)){ untested();
        x = howmany()+delta-1;
      }else{ untested();
      }
      for(; x+1 && unsigned(x+1)>unsigned(delta); --x){ untested();
	set_size(size() + numberofones((~dd[x]) & t._d[x]));
        _d[x] = _d[x-delta] | t._d[x];
      }

      // rest is t.
      for(; x+1; --x) { untested();
	set_size(size() + numberofones(t._d[x]));
        _d[x] = t._d[x];
      }
    _offset = t._offset;

  }else{ itested();
    // _offset<=t._offset
    assert(delta<=0);
    assert(size()==recount());

    unsigned i=howmany();
    for (; i<unsigned(-delta); ++i) { untested();
      _d[i] = 0;
    }

    CHUNK_T* td=(CHUNK_T*)(t._d+delta);
    assert(i+delta>=t.howmany() || i>=howmany());

    i = -delta;

    // merge.
    for (; i<unsigned(t.howmany()-delta) && i<howmany(); ++i) { untested();
      set_size(size() + numberofones((~_d[i]) & td[i]));
      _d[i] |= td[i];
    }

    // tail.
    for (; i+delta<t.howmany(); ++i) { untested();
      _d[i] = td[i];
      set_size(size() + numberofones(td[i]));
    }

    if(newhowmany<i){ untested();
      newhowmany = i;
    }
    if(newhowmany<howmany()){ untested();
      newhowmany = howmany();
    }
    assert(newhowmany>=howmany());
  }
  assert(newhowmany<=W); // for now.

  set_howmany(newhowmany);

#ifndef NDEBUG
  for(auto const& v: bak){ untested();
    assert(contains(v));
  }
  for(auto const& v: t){ untested();
    assert(contains(v));
  }
#endif
  assert(size()==recount());
}

BSDt
template<class SCT1, class SCT2>
void BSET_DYNAMIC<BSDa>::merge(
    const BSET_DYNAMIC<BSDa1>& s,
    const BSET_DYNAMIC<BSDa2>& t, unsigned disjoint)
{ untested();
  if(s._offset==t._offset){ untested();
    unsigned i=0;

    // overlap
    assert(_offset==t._offset);
#if 0
    for(; i<std::min((t.howmany()), unsigned(s.howmany())); ++i) { // }
#else
    for(; i<t.howmany() && i<s.howmany(); ++i)
#endif
    { untested();
      assert(i>=0);
      _d[i] = s._d[i] | t._d[i];
    }

    // s is longer.
    for (; i<s.howmany(); ++i){ untested();
      _d[i] = s._d[i];
    }

    // t is longer.
    for (; i<t.howmany(); ++i){ untested();
      _d[i] = t._d[i];
    }

    set_howmany(i);
  }else if(s._offset<t._offset){ untested();
    _offset=s.offset();
    return merge(t, s, disjoint);
  }else{ untested();

  unsigned delta=s._offset-t._offset;
  unsigned i=0;

  // first chunk from t
  trace0("first t");
#if 0
  for(; i<std::min(delta, unsigned(t.howmany())); i++){ untested();
#else
  for(; i<t.howmany() && i<delta; ++i){ // }
#endif
    assert(i>=0);
    _d[i] = t._d[i];
  }

  CHUNK_T* sb=(CHUNK_T*)(s._d-delta);

  // overlap
  assert(_offset==t._offset);
#if 0
  for(; i<std::min(unsigned(t.howmany()), delta+unsigned(s.howmany())); ++i)
#else
  for(; i<t.howmany() && i<delta+s.howmany(); ++i)
#endif
  { untested();
    assert(i>=0);
    assert(i+delta>=0);
    _d[i] = sb[i] | t._d[i];
  }
  // no overlap?
  // s.offset>=t.offset ... delta>=0
  for(;i<delta; ++i){ untested();
    _d[i] = 0;
  }

  // s is longer.
#if 1
  for (; i<s.howmany()+delta; ++i){ untested();
    _d[i] = sb[i];
  }
#else // slightly slower?
//  if(i<s._howmany+delta){ untested();
    memcpy(_d+i, s._d+i-delta, s.howmany()+delta-i);
    i=s.howmany()+delta;
//  }
#endif

  // t is longer.
  for (; i<t.howmany(); ++i){ untested();
    _d[i] = t._d[i];
  }
  set_howmany(i);
  assert(howmany()<=W);


  }
  // inefficient
  if(disjoint){ untested();
    assert(!s.intersects(t));
    detail::cbshelp<BSDa>::set_size(_size, s.size()+t.size());
  }else{ untested();
    detail::cbshelp<BSDa>::set_size(_size, recount());
  }
  assert(size()==recount());

#ifdef EXTRADEBUG
  trace2("cup check", (int)howmany(), (int)_offset);
  std::cout << "@!s" << s << "\n";
  std::cout << "@!t" << t << "\n";
  std::cout << "@!u" << *this << "\n";
  for (auto const&v : s) { untested();
    trace1("cup check s", v);
    assert(contains(v));
  }
  for (auto const&v : t) { untested();
    trace1("cup check t", v);
    assert(contains(v));
  }
#endif
  assert(size()==recount());
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void remove_(BSET_DYNAMIC<BSDa>& s,
    typename BSET_DYNAMIC<BSDa>::value_type i)
{ untested();
  s.erase(i);
  assert(s.size()==s.recount());
}
/*--------------------------------------------------------------------------*/
BSDt
template<class S>
inline void BSET_DYNAMIC<BSDa>::remove_sorted_sequence(S const& s)
{ untested();
  set_size(size() - s.size());
  if(size()==0){ untested();
    set_howmany(0);
    return;
  }else{ untested();
  }
  auto i=s.begin();
  for(;i!=s.end();++i){ untested();
    assert(contains(*i));
    unsigned w = *i / CHUNKBITS;
    unsigned j = *i % CHUNKBITS;

    assert(w-_offset>=0);
    assert(w-_offset<howmany());
    _d[w-_offset] &= ~(1llu << j);
  }

#if 1
  detail::hmhelp<HMT>::trim_(_howmany, _d);
#else
  while(!_d[howmany()-1]){ untested();
    --_howmany;
  }
#endif
  if(use_howmany()){
    assert(!size() == !howmany());
  }
}
/*--------------------------------------------------------------------------*/
BSDt
inline void BSET_DYNAMIC<BSDa>::erase(value_type i)
{ untested();
  assert(contains(i));
  assert(_howmany);
  assert(size());
  assert(i<=max_element);
  unsigned w = i / CHUNKBITS;
  unsigned j = i % CHUNKBITS;

  set_size(size()-1);

  assert(w-offset()>=0);
  _d[w-offset()] &= ~(1llu << j);

  if(_d[w-offset()]){ untested();
    // no trim
  }else if(unsigned(w-offset()) == unsigned(_howmany-1)){ untested();

    while(!_d[_howmany-1]){ untested();
      --_howmany;
    }

  }else if (w==offset()){ untested();
    // trim later.
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
BSDt
inline void BSET_DYNAMIC<BSDa>::trim_below()
{ untested();
  unsigned shift=0;

  // if !howmany, access uninitialized memory d[0]
  // (doesn't matter)
  for(; !_d[shift] && shift<howmany(); ++shift){ untested();
  }

  if(shift){ untested();
    CHUNK_T* dd=(CHUNK_T*)(_d+shift);

    for(unsigned j=0; j<howmany()-shift; ++j) { untested();
      _d[j] = dd[j];
    }

#if 0
    _howmany -= shift;
    _offset += shift;
#else
    set_howmany(howmany() - shift);
    set_offset(offset() + shift);
#endif
  }else{ untested();

  }
}
/*--------------------------------------------------------------------------*/
BSDt
inline void trim(BSET_DYNAMIC<BSDa>& s)
{ untested();
  s.trim_below();
}
/*--------------------------------------------------------------------------*/
BSDt
inline bool BSET_DYNAMIC<BSDa>::contains(value_type i) const
{ untested();
  assert(howmany()<=W);
  unsigned w=i / CHUNKBITS; // block that i is in.
  assert(offset()<=W);
  if(_howmany==0){ untested();
    return false;
  }else if(w<offset()){ untested();
    return false;
  }else{ untested();
    w-=offset();
  }

  if(w>=howmany()){ untested();
    return false;
  }else{ untested();
  }

  unsigned j = i % CHUNKBITS;
  assert(w<howmany());
  assert(howmany()<=W);
  return (_d[w] & (1llu << j));
}
/*--------------------------------------------------------------------------*/
BSDt
static inline unsigned long hash(BSET_DYNAMIC<BSDa> const& s)
{ untested();
  return s.hash();
}
/*--------------------------------------------------------------------------*/
BSDt
inline unsigned long BSET_DYNAMIC<BSDa>::hash() const
{ untested();
  if(howmany()){ untested();
    assert(_d[0]);
    assert(_d[howmany()-1]);
  }

#ifdef stdhash
  char* setbase = (char*)this;
  setbase+=sizeof(howmany());
  unsigned datasize=sizeof(_offset)+sizeof(_size)+howmany();
  std::string b(setbase, datasize);
  return std::hash<std::string>()(b) % nHash;
#endif

  assert(nHash);
  unsigned long altret;
  assert(sizeof(long)==8);
  if(1){}else
  if(W<=sizeof(unsigned)){ untested();
    unsigned ret=0;
    memcpy(((char*)(&ret))+offset(), _d, howmany());

//     if(howmany()!=W){ untested();
//       ret &= (1l << (8*howmany())) -1;
//       ret = ret << _offset*CHUNKBITS;
//     }

    return ret % nHash;
  }else if(W<=sizeof(unsigned long)){ untested();
    assert(CHUNKBITS==8);
    // 64 bit...
    unsigned long ret=0;

    // not optimal?
    memcpy(((char*)(&ret))+offset(), _d, howmany());

//     if(howmany()!=W){ untested();
//       ret &= (1l << (8*howmany())) -1;
//       ret = ret << offset()*CHUNKBITS;
//     }

    return ret % nHash;

  }else if(false && W<=16){ untested();

    assert(CHUNKBITS==8);
    // 64 = 8x8 bit...
    __uint128_t ret=0;

    memcpy(&ret, _d, sizeof(__uint128_t));;

    ret &= (1ll << (8*howmany())) -1;
    ret = ret << offset()*CHUNKBITS;

    altret = ret % nHash;
    return altret;
  }else{ untested();
  }
  unsigned long h=0;

  unsigned a=offset();
  assert(offset()<=W);
  unsigned i=0;
  for(; i<unsigned(howmany()); i++){ untested();
    unsigned long x=_d[i];
    h += x << CHUNKBITS*a;
    h %= nHash;
    ++a;
    if(a==CHUNKBITS*W){ untested();
      a = 0;
    }
  }
  return h % nHash;
}
/*--------------------------------------------------------------------------*/
BSDt
static inline bool isSubset(BSET_DYNAMIC<BSDa> const& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  return s.is_subset_of(t);
}
/*--------------------------------------------------------------------------*/
template<BSDarg, typename SCT2>
inline BSET_DYNAMIC<BSDa> union_(BSET_DYNAMIC<BSDa> const& s,
    BSET_DYNAMIC<BSDa2> const& t)
{ untested();
  assert(t.size()==t.recount());
  assert(s.size()==s.recount());
  return BSET_DYNAMIC<BSDa>(s, t, BSET_DYNAMIC<BSDa>::cup);
}
/*--------------------------------------------------------------------------*/
BSDt
inline void BSET_DYNAMIC<BSDa>::intersect(BSET_DYNAMIC const& t)
{ untested();
  if(howmany()){ untested();
    assert(_d[0]);
    assert(_d[howmany()-1]);
  }
  if(t.howmany()){ untested();
    assert(t._d[0]);
    assert(t._d[t.howmany()-1]);
  }
  assert(t.size()==t.recount());
  assert(size()==recount());
  int delta_=offset()-t.offset();
  if(delta_<0){ untested();
    unsigned delta=-delta_;
    for(unsigned i=0; i<delta+howmany(); ++i){ untested();
//      _size -= numberofones(_d[i]);
      _d[i] = _d[i+delta];
    }
    set_offset(offset() + delta);
    set_howmany(howmany() - delta);
    delta_ = 0;
  }else{ untested();
  }
  assert(delta_>=0);
  unsigned delta=delta_;

  CHUNK_T* td=(CHUNK_T*)(t._d+delta);
  unsigned i=0;
  for (; i<howmany() && i+delta<t.howmany(); ++i) { untested();
//    _size -= numberofones(_d[i] & ~(td[i]));
    _d[i] &= td[i];
  }

  set_howmany(i);

  trim_tail();
///  for(; _howmany && !_d[_howmany-1];){ untested();
//    --_howmany;
//  }


  trim_below(); // inefficient.

#ifndef NDEBUG
  for(auto const& v: t){ untested();
    (void)v;
    assert(t.contains(v));
  }
  for(auto const& v: *this){ untested();
    (void)v;
    assert(t.contains(v));
  }
#endif
  set_size(recount());
  assert(t.size()==t.recount());
  assert(size()==recount());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BSDt
static inline void intersection(BSET_DYNAMIC<BSDa>& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  s.intersect(t);
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void clear(BSET_DYNAMIC<BSDa>& s)
{ untested();
  s.clear();
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void fullSet(BSET_DYNAMIC<BSDa>& s, unsigned n)
{ untested();
  clear(s);
  for (unsigned v=0; v<n; v++) { untested();
    add(s, v);
  }
}
/*--------------------------------------------------------------------------*/
// check if all elements are also in t.
BSDt
bool BSET_DYNAMIC<BSDa>::is_subset_of(BSET_DYNAMIC const& t) const
{ untested();
  if(size()>t.size()){ itested();
    assert(size()==recount());
    assert(t.size()==t.recount());
    return false;
  }else{ untested();
  }
  int delta=t.offset()-offset();

  unsigned i=0;
//  if(t.offset()>offset()){ untested();
//    i=t.offset()-offset();
//  }

  //for (; i<std::min(unsigned(delta),unsigned(_howmany)); ++i) { // }
  for (; int(i)<delta && i<howmany(); ++i){ untested();
    if (_d[i]) { untested();
      return false;
    }
  }

  // overlap
  for (; i<unsigned(howmany()) && int(i)<t.howmany()+delta; i++) { untested();
    if (_d[i] & ~t._d[i-delta]) { untested();
      return false;
    }
  }

  // tail empty?
  for (; i<howmany(); i++) { untested();
    if (_d[i]) { untested();
      return false;
    }
  }
#ifndef NDEBUG
  for(auto const& v: *this){ untested();
    if(!contains(v)){ untested();
      trace1("hmm ", v);
      assert(false);
    }
    if(!t.contains(v)){ untested();
      trace1("hmm ", v);
      trace1("set ", *this);
      trace1("superset", t);
      assert(false);
    }
  }
#else
#endif
  return true;
//FF:
#ifndef NDEBUG
  trace1("set ", *this);
  trace1("superset", t);
  for(auto const& v: *this){ untested();
    if(!t.contains(v)){ untested();
      return false;
    }
  }
  std::cerr<<*this<<"\n";
  std::cerr<<t<<"\n";
  assert(false);
#endif
  return false;
}

#ifndef BLANK_CHAR

#ifdef DEBUGDIFF
#define BLANK_CHAR '0'
#else
#define BLANK_CHAR '-'
#endif

#endif

BSDt
std::ostream& operator<<(std::ostream& o, BSET_DYNAMIC<BSDa> const& s)
{ // itested();
  typedef BSET_DYNAMIC<BSDa> B;
  unsigned i=0;
  for(; i<B::CHUNKBITS*s.offset() && i<n; ++i){ untested();
    o << BLANK_CHAR;
  }

  for(; i<n && i<B::CHUNKBITS*(s.howmany()+s.offset()); ++i){ untested();
//    trace3("<<", i, int(s.howmany()), int(s.offset()));
    if(contains(s, i)) { untested();
      o << '1';
    }else{ untested();
      o << '0';
    }
  }
  for(; i<n; ++i){ itested();
    o << BLANK_CHAR;
  }
  return o;
}
/*--------------------------------------------------------------------------*/
BSDt
inline void BSET_DYNAMIC<BSDa>::insert(value_type i)
{ untested();
  if(!contains(i)){ untested();
    add(i);
  }else{ untested();
  }
  assert(contains(i));
}
/*--------------------------------------------------------------------------*/
BSDt
template<class S>
inline void BSET_DYNAMIC<BSDa>::add_sorted_sequence(S const& s)
{ untested();
  unsigned ns=size()+s.size();
  auto i=s.begin();
  if(i!=s.end()){ untested();
    add(*i);
    ++i;
  }
  for(;i!=s.end();++i){ untested();
    unsigned chunk = *i/CHUNKBITS - offset();
    unsigned j = *i % CHUNKBITS;
    // shift does the padding below.
    //
    // pad above if chunk is too the right
    // for(;_howmany <= chunk; ++_howmany){ untested();
    //   //    trace2("PAD", int(_howmany), chunk);
    //   _d[_howmany]=0;
    // }
    detail::hmhelp<HMT>::pad(_howmany, chunk, _d);

    assert(chunk>=0);
    assert(chunk<W);
    _d[chunk] |= 1llu << j;
    assert(contains(*i));
  }
  set_size(ns);
}
/*--------------------------------------------------------------------------*/
BSDt
inline void BSET_DYNAMIC<BSDa>::add(value_type i)
{ untested();
  assert(!contains(i));
  assert(howmany()<=W);
  assert(offset()<W);
  assert(i<W*CHUNKBITS);

  unsigned chunk;
  unsigned chunksize=CHUNKBITS;
  assert(offset()<=W);

  if(howmany()==0){ untested();
    chunk = 0;
    set_offset(i/chunksize);
  }else if(i/chunksize < offset()){ untested();
    unsigned delta = offset() - i/chunksize;
    bits::shiftBy(_d, delta, howmany());
    set_offset(offset() - delta);
    set_howmany(howmany() + delta);
    assert(offset()<W);
    chunk = 0;
  }else{ untested();
    chunk = i/CHUNKBITS - offset();
  }
  unsigned j = i % CHUNKBITS;
  // shift does the padding below.
  //
  // pad above if chunk it too the right
  detail::hmhelp<HMT>::pad(_howmany, chunk, _d);

  assert(chunk>=0);
  assert(chunk<W);
  _d[chunk] |= 1llu << j;
  // std::cerr<<"did " << *this << "\n";
  assert(contains(i));
  // trace2("inserted", *this, i);
  set_size(size() + 1);
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void add(BSET_DYNAMIC<BSDa> &s, int i)
{ untested();
  s.add(i);
}
/*--------------------------------------------------------------------------*/
BSDt
static inline void insert(BSET_DYNAMIC<BSDa> &s, int i)
{ untested();
  s.insert(i);
}
/*--------------------------------------------------------------------------*/
BSDt
static inline int firstSetBit(BSET_DYNAMIC<BSDa> const& s)
{ untested();
  return s.front();
}
/*--------------------------------------------------------------------------*/
BSDt
inline typename BSET_DYNAMIC<BSDa>::value_type BSET_DYNAMIC<BSDa>::front() const
{ untested();
  auto& s=*this;
  for (unsigned k = 0; k < s.howmany(); ++k) { untested();
    if (s._d[k]) { untested();
      CHUNK_T mask = 1;
      for (unsigned i=0; i<CHUNKBITS; i++) { untested();
	if ((s._d[k] & mask)) { untested();
	  unsigned f=(k+s.offset()) * CHUNKBITS + i;
	  return f;
	}
	mask = mask << 1;
      }
    }
  }
  std::cerr << "-";
  return -1;
}
/*--------------------------------------------------------------------------*/
BSDt
inline typename BSET_DYNAMIC<BSDa>::value_type BSET_DYNAMIC<BSDa>::back() const
{ untested();
  assert(howmany());
  assert(size());
  for (unsigned k=howmany(); k;){ untested();
    --k;
    if (CHUNK_T xx=_d[k]) { untested();
      CHUNK_T mask=1ul << (CHUNKBITS-1);
      for (unsigned i = CHUNKBITS; i; ) { untested();
	--i;
	if ((xx & mask) != 0) { untested();
	  return (k+offset()) * CHUNKBITS + i;
	}
	mask = mask >> 1;
      }
    }else{ untested();
      untested();
    }
  }
  return -1;
}
/*--------------------------------------------------------------------------*/
BSDt
bool BSET_DYNAMIC<BSDa>::operator==(BSET_DYNAMIC const& t) const
{ untested();
  if(!use_offset()){
  }else if(howmany()){ untested();
    assert(_d[0]);
    assert(_d[howmany()-1]);
  }
  if(!t.use_offset() || !t.use_howmany() ){ untested();
  }else if(t.howmany()){ untested();
    assert(t._d[0]);
    assert(t._d[t.howmany()-1]);
  }

  unsigned i=0;
  if(size()!=t.size()){ untested();
    return false;
  }else if(howmany()!=t.howmany()){ untested();
    return(false);
  }else if(offset()!=t.offset()){ untested();
    return(false);
  }else if(offset()==t.offset()){ untested();
    // probably faster...
    // strcmp?
    for(; i<howmany(); ++i){ untested();
      if (_d[i] != t._d[i]) { untested();
	return false;
      }else{ untested();
      }
    }
  }

#ifndef NDEBUG
  for(auto const& v: *this){ untested();
    assert(t.contains(v));
  }
  for(auto const& v: t){ untested();
    assert(this->contains(v));
  }
#endif

  return true;
}
/*--------------------------------------------------------------------------*/
BSDt
static inline bool intersects(BSET_DYNAMIC<BSDa> const& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  return s.intersects(t);
}
/*--------------------------------------------------------------------------*/
BSDt
template<class SCT2>
bool BSET_DYNAMIC<BSDa>::intersects(BSET_DYNAMIC<BSDa2> const& t) const
{ untested();
  int delta=offset()-t.offset();
  // unsigned M=std::max(offset(), t.offset());
  // unsigned MM=std::min(howmany(), t.howmany());
  if(delta>=0){ untested();
    unsigned d=delta;
    for (unsigned i=0; i<howmany() && i+d<t.howmany(); ++i) { untested();
      if ((_d[i] & t._d[i+d])){ untested();
        return true;
      }else{ itested();
      }
    }
    return false;
  }else{ untested();
    return t.intersects(*this);
  }
}
/*--------------------------------------------------------------------------*/
BSDt
static inline bool empty(BSET_DYNAMIC<BSDa> const& s)
{ untested();
  return s.empty();
}
/*--------------------------------------------------------------------------*/
BSDt
static inline unsigned size(BSET_DYNAMIC<BSDa> const& s)
{ untested();
  return s.size();
}
/*--------------------------------------------------------------------------*/
BSDt
static inline bool equals(BSET_DYNAMIC<BSDa> const& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  return s == t;
}
/*--------------------------------------------------------------------------*/
BSDt
inline BSET_DYNAMIC<BSDa> dis_union_(BSET_DYNAMIC<BSDa> const& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  assert(t.size()==t.recount());
  assert(s.size()==s.recount());
  return BSET_DYNAMIC<BSDa>(s, t, BSET_DYNAMIC<BSDa>::sqcup);
}
/*--------------------------------------------------------------------------*/
BSDt
inline BSET_DYNAMIC<BSDa> intersection_(BSET_DYNAMIC<BSDa> const& s, BSET_DYNAMIC<BSDa> const& t)
{ untested();
  if(s.offset()>=t.offset()){ untested();
    BSET_DYNAMIC<BSDa> r=s;
    r.intersect(t);
    r.trim_below(); // incomplete
    return r;
  }else{ untested();
    BSET_DYNAMIC<BSDa> r=t;
    r.intersect(s);
    r.trim_below(); // incomplete
    return r;
  }
}
/*--------------------------------------------------------------------------*/
#include "setutil.h" // CLEANUP!
template<class S1, class S2>// variadic?
lazy_union<S1, S2> make_lazy_union(S1 const& s1, S2 const& s2)
{ untested();
	return lazy_union<S1, S2>(s1, s2);
}
/*--------------------------------------------------------------------------*/
template<class S1, class S2>
class lazy_intersection{
public:// types
  typedef unsigned value_type;
//  typedef typename T::CHUNKBITS CHUNKBITS;
public:
  lazy_intersection(S1 const& s, S1 const& t)
      : _s((s.offset()<=t.offset())?t:s),
	_t((s.offset()<=t.offset())?s:t),
	_delta(_s.offset()-_t.offset()){ untested();

#ifndef NDEBUG
    if(!_s.howmany()){ untested();
      incomplete();
    }else if(!t.howmany()){ untested();
      incomplete();
    }
#endif

  }

public:
  value_type const* begin() const{ untested();
    // pretend/incomplete. the intersection must be nonempty
    unsigned i=0;
    _scratch.set_offset(_s.offset());
    _scratch._d[0] = _s._d[i] & _t._d[i+_delta];
    while(!_scratch._d[0]){ untested();
      ++i;
      _scratch.set_offset(_scratch.offset()+1);
      _scratch._d[0] = _s._d[i] & _t._d[i+_delta];
    }

    trace2("", unsigned(_s.offset()), unsigned(_scratch.offset()));
    _hack = __builtin_ctz(_scratch._d[0]) + S1::CHUNKBITS*_scratch.offset();
    assert(_s.contains(_hack));
    assert(_t.contains(_hack));
    return &_hack;
  }
  operator S1 const&() const{ untested();
    unsigned i=_scratch.offset()-_s.offset();
    unsigned j=_scratch.offset()-_t.offset();

    assert(_scratch._d[0] == (_s._d[i] & _t._d[j]));
    _scratch.set_size(_scratch.size() + numberofones(_scratch._d[0]));
    _scratch.set_howmany(1);

    for(unsigned k=1; i+k < _s.howmany() && j+k < _t.howmany(); ++k){ untested();
      _scratch._d[k] = _s._d[i+k] & _t._d[j+k];
      _scratch.set_size(_scratch.size() + numberofones(_scratch._d[k]));
      _scratch.set_howmany(_scratch.howmany()+1);
    }
    while(!_scratch._d[_scratch.howmany()-1]) { untested();
      assert(_scratch.howmany()); // only works for nonempty intersections.
      _scratch.set_howmany(_scratch.howmany()-1);
    }
    return _scratch;
  }
  unsigned size() const{ untested();
    if(_s.size()==1u){ untested();
      return 1u;
    }else{ untested();
      // incomplete.
      return -1u;
    }
  }
  bool contains(value_type a) const{ untested();
//    hmm incomplete, use _scratch if it's there...
    return _s.contains(a) && _t.contains(a);
  }
  bool is(S1 const* x) const{ return &_s==x; }
private:
  S1 const& _s;
  S2 const& _t;
  unsigned _delta;
  mutable S1 _scratch;
  mutable value_type _hack;
};
/*--------------------------------------------------------------------------*/
template<class S1, class S2>// variadic?
lazy_intersection<S1, S2> make_lazy_intersection(S1 const& s1, S2 const& s2)
{ untested();
  return lazy_intersection<S1, S2>(s1, s2);
}
/*--------------------------------------------------------------------------*/
template<BSDarg, class S>
inline void add_sorted_sequence(BSET_DYNAMIC<BSDa>& b, S const& s)
{ untested();
  return b.add_sorted_sequence(s);
}
/*--------------------------------------------------------------------------*/
template<BSDarg, class S>
inline void remove_sorted_sequence(BSET_DYNAMIC<BSDa>& b, S const& s)
{ untested();
  return b.remove_sorted_sequence(s);
}
/*--------------------------------------------------------------------------*/

} // cbset

// vim:ts=8:sw=2:noet

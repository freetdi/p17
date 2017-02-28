/* Copyright (C) 2017 Felix Salfelder
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
 * graph decomposition algorithms, traits.
 */

#ifndef TD_ALGO_HPP
#define TD_ALGO_HPP

#include <limits>
#include "timer.hpp"
#include "graph_traits.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace treedec{

namespace algo{

template<class T>
class CLOSED_RANGE {
public:
    // default constructor produces empty object.
    // consistent with STL containers.
    CLOSED_RANGE(T b=0, T e=-1)
        : _b(b), _e(e+1)
    { untested();
    }
public: // set
    void set_begin(unsigned b){ untested();
        _b = b;
    }
    void set_front(unsigned b){ untested();
        _b = b;
    }
    void set_end(unsigned e){ untested();
        _e = e;
    }
    void set_back(unsigned e){ untested();
        _e = e+1;
    }
public: // get
    bool is_empty() const{
        return _b==_e;
    }
    T first(){ return _b; }
    T last(){ return _e-1; }
    T front(){ return _b; }
    T back(){ return _e-1; }
    T min(){ return _b; }
    T max(){ return _e-1; }
//    unsigned begin(){ return counting_iterator(_b); }
//    unsigned end(){ return counting_iterator(_e); }
public: // compute
    CLOSED_RANGE& intersect(CLOSED_RANGE& o){ untested();
        _b = max(_b, o._b);
        _e = min(_e, o._e);
        return *this;
    }
    CLOSED_RANGE& lower_bound(T l){ untested();
        _b = std::max(_b, l);
        return *this;
    }
    CLOSED_RANGE& upper_bound(T u){ untested();
        _e = std::min(_e, u+1);
        return *this;
    }
private:
    T _b, _e;
};

struct config_base{
    static void interruption_point(){
        // compile-time disabled signalling
    }
    static void commit_lb(unsigned){ untested();
        // 
    }
    static void commit_ub(unsigned){ untested();
        // 
    }
    // "C" style seems to be most practical.
    static void message(unsigned badness, const char* fmt, ...){
        // compile-time disabled messaging
        (void) badness; (void) fmt;
    }
};


template<class GraphType, class ... rest>
struct default_config : config_base{
    using vst=typename boost::graph_traits<GraphType>::vertices_size_type;
    static constexpr unsigned max_vertex_index=std::numeric_limits<vst>::max();
};

namespace draft{

template<class O>
O* clone(O const* o)
{
    if(o){ untested();
        return o->clone();
    }else{
        return NULL;
    }
}

class algo1{
protected:
	algo1(const algo1& o)
#ifdef TIMER
		: _timer(clone(o._timer))
#endif
		{(void)o;}
public:
	algo1(std::string label)
#ifdef TIMER
	   : _timer(NULL) {
			_timer=new DOUBLE_TIMER("raw" + label);
		}
#else
	{(void)label;}
#endif
	virtual ~algo1(){
#ifdef TIMER
		if(_timer){
		//	std::cout << _label << ": ";
			std::cout << *_timer << "\n";
		}
#endif
	}

	virtual void do_it() = 0;

//for now
public:
	double get_runtime(){
#ifdef TIMER
            if(_timer){
                return _timer->total();
            }
#endif
            return -1;
        }

protected:
	void timer_on(){
#ifdef TIMER
		if(_timer){
			_timer->start();
		}
#endif
	}
	void timer_off(){
#ifdef TIMER
		if(_timer){
			_timer->stop();
		}
#endif
	}

private:
//	std::string _label; incomplete.
#ifdef TIMER
	TIMER_BASE* _timer;
#endif
};

} // draft

} // algo1

} // treedec

#endif // guard

// vim:ts=8:sw=4:et

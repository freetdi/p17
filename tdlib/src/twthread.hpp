/* Copyright (C) 2016 Felix Salfelder
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
 * graph decomposition thread
 */

#ifndef TD_THREAD_HPP
#define TD_THREAD_HPP

#include <boost/thread.hpp>
#include <boost/graph/graph_traits.hpp>
#include <gala/trace.h>
#include <tdlib/printer.hpp>
#include <tdlib/message.hpp>
#include <tdlib/elimination_orderings.hpp>
#include <mutex>
#include <atomic>
#include <condition_variable>
		

// TODO use tdgrprinter
#include "dumptd.h"

#ifdef NDEBUG
#define assert_permutation(P)
#else
#define assert_permutation(P) \
{ \
	std::vector<bool> check(P.size()); \
		for(unsigned i : P){ \
		    check[i]=true; \
		} \
		for(auto i : check){ \
		    assert(i); \
		} \
}
#endif


//hacks. must cleanup...
// should be static in _BASE, nmaybe
typedef unsigned result_t;
extern std::atomic<unsigned> global_result;

/*
 * upon finding a solution, a worker must
 * - acquire lock
 * - write to TWTHREAD<G>::_result
 * - release lock
 */

/*
 * worker finding an optimum solution should
 * - store solution
 * - acquire lock
 * - write to TWTHREAD<G>::_result
 *           i.e. call commit_result
 * - release lock
 * - signal TERM (hack? yes, but works for pace)
 */

/*
 * worker giving up should
 * - acquire lock
 * - write to TWTHREAD<G>::_result
 *           i.e. call commit_result
 * - store solution (required if if commit_result().second)
 * - release lock
 * - signal TERM (hack? yes.)
 */

/*
 * a commit is binding. after committing a result,
 * the thread must be prepared to
 * - receiving an interrupt
 * - and immediately unlock results in worker
 * - to handle TWTHREAD::print_results call from main thread
 */

#include "algo.hpp"
namespace treedec{

namespace draft{

class TWTHREAD_BASE : public boost::thread{
public:
	TWTHREAD_BASE() { itested();
	}
	virtual ~TWTHREAD_BASE() {}

	virtual void print_results(std::ostream&o) = 0;
	virtual unsigned get_result() const = 0;
	virtual std::pair<unsigned, bool> commit_result(result_t){
		unreachable();
		return std::make_pair(0u, false);
	}

protected:
	unsigned& running(){return _running;}
public: // should be private... careful.
	static unsigned _running;
public:
	static std::map<std::string, TWTHREAD_BASE*> _global_maphack;
	void global_resulthack(unsigned x){
		_result = x;
		if(unsigned(global_result)<x){ untested();
		}else{
			global_result = x;
		}
	}
protected:
	unsigned _result; // better private?
};

} //draft

#define TWTt template<class G, template<class G_, class ...> class CFGT>
#define TWTa G, CFGT

template<class G, template<class G_, class ...> class CFGT=algo::default_config>
class TWTHREAD : public draft::TWTHREAD_BASE{
public: // types
	typedef CFGT<G> CFG;
	typedef std::lock_guard<std::mutex> scoped_lock;
	typedef draft::TWTHREAD_BASE base;
protected: // run time config
	 template<class GG, class ... rest>
	 struct cfgt : CFGT<GG, rest ...> {
		static void interruption_point(){ itested();
			boost::this_thread::interruption_point();
		}
		static void commit_ub(unsigned x, std::string reason=""){ itested();
			// TODO: cleaner.
			// how to get rid of dictionary??
			if(reason!=""){
				reason=", "+reason;
			}else{
			}
			CFG::message(bLOG, "thread received upper bound %d%s\n", x, reason.c_str());
			std::stringstream ss;
			ss << boost::this_thread::get_id();
			auto twt=TWTHREAD_BASE::_global_maphack[ss.str()];
			assert(twt);
			twt->global_resulthack(x);
		}
	 };
protected:
	 TWTHREAD(){untested();}
	 TWTHREAD(const TWTHREAD&){untested();}
	 TWTHREAD(TWTHREAD&&){untested();}
	 TWTHREAD(const TWTHREAD&&){untested();}
public:
	TWTHREAD(G const& g, std::string const& name, unsigned)
		 : _g(g), _name(name)
	{ untested();
		_result=-1u;
		 // create idle thread..
		trace2("TWTHREAD idle", &g, &_g);
		trace2("TWTHREAD idle", boost::num_vertices(_g), boost::num_edges(_g));
		CFG::message(bLOG, "starting idle twthread %s, %p\n", _name.c_str(), this);
		_result_lock = new std::lock_guard<std::mutex>(_tw_mutex);
	}
	TWTHREAD(G const& g, std::string const& name)
		 : _g(g), _name(name)
	{ untested();
		_result=-1u;
		CFG::message(bLOG, "starting twthread %s, %p\n", _name.c_str(), this);
		_result_lock = new std::lock_guard<std::mutex>(_tw_mutex);
		go(); // hmm should wait for child class constructor?
	}
	virtual ~TWTHREAD() { untested();
		trace1("shutting down", _name);
	}
private:
	TWTHREAD& operator=(const TWTHREAD&&p) {
	  	incomplete();
		unreachable();
	}
public:
	virtual void run() = 0;
	void print_results(std::ostream&o);
	unsigned get_result() const {
		return _result;
	}
	template<class T>
	T const& get_tree_decomp() const {
		incomplete();
		// depends, maybe, we have only an ordering.
		// need some do_ overrides (similar to "print_")
	}
	template<class T>
	T const& get_elim_ordering() const {
		incomplete();
		// depends, maybe, there's only a tree
		// need some do_ overrides (similar to "print_")
	}
	virtual void do_print_results(std::ostream&o)
	{ untested();
		// you should override this function!
		o << _result <<"\n";
	}
	// commit a result.
	// commit_result(..).second indicates that the master is interested.
	// (if not, the worker may discard it)
	std::pair<unsigned, bool> commit_result(result_t x);

protected:
	G const& input_graph() const {
		return _g;
	}
	void go() { untested();
		trace2("TWTHREAD go", boost::num_vertices(_g), boost::num_edges(_g));
		++base::running();
		boost::thread::operator=(
				std::move(boost::thread(bind(&TWTHREAD::run, this)))
		);
		std::stringstream ss;
		ss << boost::thread::get_id();
		CFG::message(0, "started %s %s\n", _name.c_str(), ss.str().c_str());
		_global_maphack[ss.str()] = this;
	}
	void unlock_results()
	{
		assert(_result_lock);
		delete _result_lock;
	}
	void just_wait();


protected:
	template<class order_t>
	void print_results_order(std::ostream& o, order_t const& ord, result_t bagsize=-1);
	template<class T>
	void print_results_tree(std::ostream& o, T const& t, G const* g_override=NULL);
protected:
	G const& _g;
	const std::string& _name;
	std::mutex _tw_mutex;
	std::lock_guard<std::mutex>* _result_lock;
};

TWTt
inline std::pair<unsigned, bool> TWTHREAD<TWTa>::commit_result(result_t x)
{
	// FIXME: where to put the guards? here?
	// (need the same guard for the solution...?)
	unsigned backup=global_result;
	bool is_better = true;
	assert(x<=_result);
	if(unsigned(global_result)<x){ untested();
		x = global_result;
		is_better = false;
	}else{
		global_result = x;
		// HACK do the printing here, for now.
		// (lacks a mutex!)
	//	std::cout << "c status " << x << " " << status_ms() << " " << _name << "\n";
	}
#if 0 // incomplete
	else if(global_result == x){ untested();
		x = global_result;
		is_better = false;
	}else{ untested();
		_result = x;
		// result
		is_better = true;
	}
#endif

	_result = x;
	return std::make_pair(backup, is_better);
}

TWTt
void TWTHREAD<TWTa>::just_wait()
{
	trace1("just waiting", _name);
	// sort of a hack, just wait until the program stops. maybe should wait
	// for release by main tread (e.g. triggered after print).
	std::mutex end;
	std::unique_lock<std::mutex> lock(end);
	std::condition_variable cv;
	while(true) {
		cv.wait(lock);
	}
}

TWTt
void TWTHREAD<TWTa>::print_results(std::ostream&o)
{ untested();
	scoped_lock lk(_tw_mutex);
	trace2("print", _name, _result);
	do_print_results(o);
}

TWTt
template<class order_t>
void TWTHREAD<TWTa>::print_results_order(
		std::ostream& o, order_t const& ord, result_t bagsize)
{ untested();
	if(bagsize!=-1u){ untested();
	}else{ untested();
	}
	assert(bagsize==-1u || bagsize == _result);
	treedec::grtdprinter<G> P(o, _g);
	size_t numbags = boost::num_vertices(_g);
	P.head(numbags, _result);
	assert(ord.size()==numbags);
	assert_permutation(ord);
	treedec::draft::vec_ordering_to_tree(_g, ord, P );
}

// FIXME: use printer...
template<class T, class G>
static void outdata(T const &t, G const& g, std::ostream&) // incomplete
{
    dumptd(t, g);
}

TWTt
template<class T>
void TWTHREAD<TWTa>::print_results_tree(std::ostream& o, T const& t, G const* g_override)
{
	if(g_override){
		// HACK/workaround. dont use.
		outdata(t, *g_override, o);
	}else{
		outdata(t, _g, o);
	}
}

std::map<std::string, draft::TWTHREAD_BASE*> draft::TWTHREAD_BASE::_global_maphack;

} // treedec


#undef TWTt
#undef TWTa

#endif // guard

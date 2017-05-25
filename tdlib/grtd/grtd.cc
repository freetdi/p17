// Lukas Larisch, 2016
// Felix Salfelder, 2016
//
// (c) 2016 Goethe-Universität Frankfurt
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
// tw-heuristic etc.
//

#include "config.h"

#ifdef USE_GALA
// #define USE_RANDOM_MD // 1
// #define USE_FI // 2 hmmm..
// #define USE_MSVS_TRIVIAL // 4
#define USE_BMD // 8
// #define USE_SOME // maybe
#define USE_EX // 32
#define USE_TA // 64
// pace17 heuristics
//
#endif

// this one works without gala
#define USE_P17 // 128

#ifdef DEBUG
#undef NDEBUG
#endif


#include <algorithm>
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


#define assert_symmetric(g) { \
    unsigned i=0; \
    auto E=boost::edges(g); \
    for(;E.first!=E.second; ++E.first){ \
        ++i;\
        assert( boost::edge(boost::source(*E.first, g), \
                    boost::target(*E.first, g),g).second); \
        assert( boost::edge(boost::target(*E.first, g), \
                    boost::source(*E.first, g),g).second); \
    }\
    trace1("symmetric", i); \
}

unsigned long status_ms(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long msSinceEpoch = (unsigned long long)(tv.tv_sec) * 1000
       + (unsigned long long)(tv.tv_usec) / 1000;
    return msSinceEpoch;
}

/// old...
enum thread_n{
    nMD = 0,
    nFI = 1,
    nMSVS = 2,
    nBMD = 3,
    nSOME = 4,
    nEX = 5,
    nTA = 6,
    nP17 = 7,
    nTOTAL = 8
};

// TODO: cleanup, threads now have names.
const std::string names[] = {"MD", "FI", "MSVS", "BMD", "SOME", "EX", "TA", "P17", "NONE"};

std::mutex best_mutex;
std::condition_variable cv;
static std::atomic<unsigned> global_result;
bool trace=false;

#include <boost/graph/graph_traits.hpp>

#ifdef USE_SOME
#include "bp.h"
#endif

//
#ifdef USE_GALA
#include <gala/boost.h>
#include <tdlib/directed_view.hpp>
#ifdef USE_RANDOM_MD
#include <gala/examples/ssg_random.h>
#endif
// #include <boost/graph/minimum_degree_ordering.hpp>
#include <gala/examples/ssg32i.h>
#include <gala/examples/ssg16i.h>
#include <gala/examples/ssg16ia.h>
#include <gala/examples/svbs.h>
#include <gala/examples/svbs_random.h>
#include <gala/td.h>
#include <gala/immutable.h>
#include <gala/boost_copy.h>
#else
#warning no gala. does not fully work
#endif


#include <tdlib/combinations.hpp>
#include <tdlib/elimination_orderings.hpp>
#include <tdlib/printer.hpp>
#include <tdlib/twthread.hpp>

using treedec::TWTHREAD;
using treedec::draft::TWTHREAD_BASE;

unsigned TWTHREAD_BASE::_running;

#include "grparse.h"
#include "dumptd.h"

// bug, still tdlib

////// why is this necessary? //////
using boost::out_edges;
using boost::out_degree;
using boost::degree;
using boost::source;
using boost::target;
////////////////////////////////////

#ifdef USE_GALA
BOOST_CONCEPT_ASSERT(( boost::IncidenceGraphConcept<ssg_16i> ));
typedef typename treedec::graph_traits<ssg_16i>::immutable_type check_type;
// boost::iterator_traits<check_type::out_edge_iterator>::value_type A;
std::iterator_traits<check_type::out_edge_iterator>::value_type A;
BOOST_CONCEPT_ASSERT(( boost::IncidenceGraphConcept<check_type> ));
#endif

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> balu_t;
typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> bald_t;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> balvvd_t;

template<class G>
using decomp_t = typename treedec::graph_traits<G>::treedec_type;

enum mag_t{
    Munknown=0,
    M6=6, // <= 64 verts
    M7=7, // <= 128 verts
    M8=8,
    M16=16,
    M15=15,
    M31=31,
    M32=32,
    M64=64
};

// fixme: move to other compilation unit
#include <stdarg.h>
#include <stdio.h>
int errorlevel=bLOG;
template<class X=balu_t, class ... rest>
struct grtd_algo_config : treedec::algo::default_config<X, rest...>{
    // print with "c "
    static void message(int badness, const char* fmt, ...)
    {
        if (badness >= errorlevel){
            char buffer[2048] = "";
            va_list arg_ptr;
            va_start(arg_ptr,fmt);
            vsprintf(buffer,fmt,arg_ptr);
            va_end(arg_ptr);
            std::cout << "c " << buffer;
        }else{
        }
    }
};
/*--------------------------------------------------------------------------*/


#ifdef USE_GALA
#define FIgraph_t ssg_16i
#define MDgraph_t ssg16_random
#else
#define FIgraph_t balu_t
#define MDgraph_t balu_t
#endif

#ifdef USE_GALA
typedef simplegraph_vector_bs svbs;

template<class T, class...>
using tdDEGS = misc::DEGS<T>;

template<class G>
struct dvv_config : gala::graph_cfg_default<G> {
    static constexpr bool is_directed=false;
    static constexpr bool force_simple=true;
    static constexpr bool force_loopless=true; // not used yet?
    static constexpr bool force_symmetric=true; // not used yet?
    typedef tdDEGS<G> degs_type;
};

template<class G>
struct dpvv_config : dvv_config<G> {
    static constexpr bool force_simple=false;
    static constexpr bool force_symmetric=false;
    static constexpr bool is_directed=true;
};

typedef gala::graph<std::vector, std::vector, uint16_t, dpvv_config> sg_dpvv16;
typedef gala::graph<std::vector, std::vector, uint32_t, dpvv_config> sg_dpvv32;

typedef gala::graph<std::vector, std::vector, uint16_t, dvv_config> sg_dvv16;
typedef gala::graph<std::vector, std::vector, uint32_t, dvv_config> sg_dvv32;

//typedef gala::graph<std::set, std::vector, uint16_t> ssg16;
#endif

#ifdef USE_GALA
#ifdef USE_BMD
#include "bmd.h"
#endif
#if defined USE_SOME
#include "some.h"
#endif
#endif

struct sigaction sa;

sig_atomic_t int_received;
sig_atomic_t term_received;
bool quiet=false;
volatile unsigned finished;

static void nohandler(int, siginfo_t *, void *)
{
}
static void term_handler(int signum, siginfo_t *, void *)
{
    if(signum==SIGTERM){
        fprintf(stderr, "TERM handler\n");
        sigset_t mask;
        sigemptyset (&mask);
        sigaddset (&mask, signum);
        if (sigprocmask(SIG_BLOCK, &mask, NULL)!=0) { untested();
            printf("cannot reinstall TERM handler");
            exit(3);
        }else{
            untested();
        }
        term_received = 1;
    }else{
        unreachable();
    }
}
static void int_handler(int signum, siginfo_t *, void *)
{
    if(signum==SIGUSR1 || signum==SIGINT){
        fprintf(stderr, "USR1 handler\n");
        int_received = 1;
    }
}


static void fin(volatile unsigned & finished)
{
    std::unique_lock<std::mutex> scoped_lock(best_mutex);
    /// mutex running?!
    if(trace) std::cerr << "countdown" << finished << " " << TWTHREAD_BASE::_running << "\n";
    ++finished;
    if(finished==TWTHREAD_BASE::_running){
        if(trace) std::cerr << "signalling end\n";
//        scoped_lock.wait(cv);
        scoped_lock.unlock();
        kill(getpid(), SIGINT); // pass by "wait" in mainloop??
        cv.notify_all(); // needed after interrupt. main thread might be waiting for us.
    }
}

#ifdef USE_RANDOM_MD
template<class G>
class MD_THREAD : public TWTHREAD<ssg16_random> {
public:
    typedef TWTHREAD<ssg16_random> base;

    MD_THREAD( G g /* create local copy*/, const std::string& name )
        : TWTHREAD<ssg16_random>(_g /*bug, not initialized yet??*/, name, 0), _g(g) // <= stored here
    { untested();
        trace2("MD_THREAD", &_g, boost::num_vertices(_g));
        base::go();
    }

    void do_print_results(std::ostream& o)
    { untested();
        trace2("md dpr", boost::num_vertices(_g), &_g);
        return base::print_results_tree(o, _t, &_g); /// HACK passing &_g, for now.
    }

    // BUG: use config.
    void cb(unsigned x)
    { itested();
        if(x){ untested();
            trace1("md cb", x);
            base::commit_result(x);
        }else{ untested();
            // calls 0 to just interrupt.
        }
        boost::this_thread::interruption_point();
    }


    void run()
    {
//        auto const & tg=TWTHREAD<G>::_g;
//        trace2("run", base::_name, boost::num_vertices(tg));
        trace1("MD", boost::num_vertices(_g));
        //G_work X;
        auto F=bind(&MD_THREAD::cb, this, _1);
        try{
            treedec::impl::endless_minDegree_decomp(_g, _t, &F);
        }catch(...){ // BUG catch what?!
            trace1("caught interrupt", boost::num_vertices(_g));
            trace2("caught interrupt", boost::num_vertices(_t), treedec::get_bagsize(_t));

            base::unlock_results();
            base::just_wait();
        }
        unreachable();

        unsigned r;
        r = treedec::get_bagsize(_t);
        assert(boost::num_vertices(_g) || r==0);
        base::commit_result(r);
        base::unlock_results();

        //  g /* const ref! */, boost::ref(OSOME), boost::ref(wSOME));
    }
private:
//   result_t &_result;
#ifdef USE_GALA
   decomp_t<ssg16_random> _t;
   ssg16_random _g;
#else
   decomp_t<G> _t;
   G _g;
#endif
}; // MD_THREAD
#endif

#ifdef USE_GALA
#ifdef USE_TA
#include "ta_thread.h"
#endif
#endif

#ifdef USE_GALA
#ifdef USE_FI
#include "fi.h"
#endif

#ifdef USE_EX
#include "ex.h"
#endif
#endif

#ifdef USE_MSVS_TRIVIAL
#include "msvs.h"
#endif

#ifdef USE_P17
#include "p17.h"
#endif


static bool sig_received()
{
    return term_received || int_received;
}

void twh(PARSE*p, mag_t, unsigned);

int main(int argc, char * const * argv)
{
    trace1("main", getpid());
    global_result = -1u;
    finished = 0;
    trace=false;

    sa.sa_sigaction = term_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_RESETHAND;
    if (0
//     || sigaction(SIGINT,  &sa, NULL) == -1
     || sigaction(SIGTERM, &sa, NULL) == -1
     ) {
        std::cerr << "error installing sighandler\n";
        exit(1);
    }
    sa.sa_sigaction = int_handler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        std::cerr << "error installing sighandler\n";
        exit(1);
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        std::cerr << "error installing sighandler\n";
        exit(1);
    }
    mag_t m=Munknown;
    unsigned mask=-1;
    unsigned mask_in=0;

    int i=1;
    while(i<argc){
        if(!strncmp("-q", argv[i], 2)){ untested();
            quiet=true;
        }else if(!strncmp("-L", argv[i], 2)){ untested();
            errorlevel=bLOG;
        }else if(!strncmp("-D", argv[i], 2)){ untested();
            errorlevel=bDEBUG;
        }else if(!strncmp("-N", argv[i], 2)){ untested();
            errorlevel=bNOERROR;
        }else if(!strncmp("-T", argv[i], 2)){ untested();
            trace=true;
            errorlevel=bTRACE;
        }else if(!strncmp("-t", argv[i], 2)){ untested();
            trace=true;
            errorlevel=bTRACE;
            std::cerr << "tracing ON\n";
        }else if(!strncmp("-w", argv[i], 2)){ untested();
            m=M32;
            std::cerr << "wide ON\n";
        }else if(!strncmp("-m", argv[i], 2)){
            ++i; // incomplete: range check?
            mask_in = atoi(argv[i]);
            trace2("mask arg", argv[i], mask_in);
        }else if(!strncmp("-s", argv[i], 2)){ untested();
            // not yet
            ++i;
        }else{ untested();
        }
        ++i;
    }
    if(mask_in){
        mask = mask_in;
        trace1("mask set", mask);
    }else{
        untested();
    }

    PARSE* p;

    try{ untested();
        p = new PARSE(std::cin, oUPPER);
    }catch(...){ untested();
        // BUG catch what?!
        std::cerr << "error parsing header\n";
        exit(2);
    }
    global_result = p->num_vertices();
    std::cout << "c status " << p->num_vertices() << " " << status_ms() << " initial\n";

    if(m){ untested();
    }else if(p->num_vertices() >= (1l<<31)-1){ untested();
        m = M32;
    }else if(p->num_vertices() >= (1<<16)-1){ untested();
        m = M31;
    }else if(p->num_vertices() >= (1<<15)-1){ untested();
        m = M16;
    }else{ untested();
        m = M15;
    }

    twh(p, m, mask);

    return(0);
}

template<class T>
void join_and_cleanup(T* t)
{
    if(t){ untested();
        t->join();
        delete t;
        t=NULL;
    }else{ untested();
    }
}
template<class T>
void shutdown_gently(T* t)
{
    if(t){ untested();
        t->interrupt();
        t->join();
    }else{ untested();
    }
}


template<class T>
void cleanup(T const& t)
{
#ifndef GENTLE_SHUTDOWN
    (void)t;
    if(trace){
        std::cerr << "forced shutdown\n";
    }
    {
        sa.sa_sigaction = nohandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_SIGINFO;
        if (sigaction(SIGSEGV, &sa, NULL) == -1){
            exit(3);
        }
    }
#else // requires interruption points
// does not really work, as threads do not come back...
    if(trace){
        std::cerr << "slowly shutting down.";
    }
    for(auto ti : t){
        std::cerr << ".";
        shutdown_gently(ti);
    }
    if(trace){
        std::cerr << "done..\n";
    }
#endif
}

void mainloop()
{
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, SIGTERM);

    while(true){
        if(term_received){
            if(trace) std::cerr << "term received\n";
            if (sigprocmask(SIG_BLOCK, &mask, NULL)<0) { untested();
                std::cerr << "error blocking SIGTERM\n";
                exit(99);
            }
            return;
        }else if(int_received){ untested();
            std::lock_guard<std::mutex> scoped_lock(best_mutex);
            std::cout << "c " << global_result << "\n";
        }else{
        }
        int_received=0;
        if(!TWTHREAD_BASE::_running){ untested();
            // all threads masked?
            // all threads completed (that was quick!)
            break;
        }else if(!sig_received()){
            trace0("pause");
            pause();
        }
        // get here after the sighandler has been executed
    }
}

template<class V>
unsigned find_best(V const& v)
{
    unsigned best=-1;
    unsigned bestindex=nTOTAL;

    for(unsigned i=0; i<v.size(); ++i){
        if(!v[i]){
            continue;
        }
        auto result=v[i]->get_result();
        if (result < best){
            bestindex = i;
            best = result;
        }
    }
    return bestindex;
}

void twh(PARSE*p, mag_t m, unsigned mask)
{

#ifdef USE_GALA
//    typedef gala::graph<std::vector, std::vector, uint32_t, dvv_config> sg_dvv32;

    typedef sg_dvv16 G16;
    typedef sg_dvv32 G32;

    typedef sg_dpvv16 G16p;
    typedef sg_dpvv32 G32p;

    G16 g16;
    G32 g32;

    if(m>M15){ untested();
        G32p pg32(p->begin(), p->end(), p->num_vertices(), p->num_edges());
        g32 = std::move(pg32);
        assert(boost::num_edges(g32)==p->num_edges()); // usually.
    }else{
        trace2("", p->num_vertices(), p->num_edges());
        G16p pg16(p->begin(), p->end(), p->num_vertices(), p->num_edges());
        trace1("", boost::num_edges(pg16));
        g16 = std::move(pg16);
        trace1("", boost::num_edges(g16));
        assert(boost::num_edges(g16)==p->num_edges()); // usually.
    }
#else
    incomplete();
    typedef balu_t G;
    typedef balu_t G16;
    typedef balu_t G32;
    G g(p->begin(), p->end(), p->num_vertices(), p->num_edges());
    G& g16(g);
    G& g32(g);
#endif


    size_t n=p->num_vertices();
    size_t e=p->num_edges();

    std::cout << "c n: " << n << ", e: " << e << std::endl;
#ifdef USE_GALA
    std::cout << "c gala on" << std::endl;
#endif

    std::vector<TWTHREAD_BASE*> threads(nTOTAL);

    if(trace){
        std::cerr << "starting threads for " << m << " bit mode\n";
    }

    TWTHREAD_BASE::_running=1;
/*--------------------------------------------------------------------------*/
#ifdef USE_RANDOM_MD
    if(!(mask & ( 1 << nMD ))) {
    }else if( m <= M16){ untested();
        threads[nMD] = new MD_THREAD<G16>(g16, "MD16");
    }else{ untested();
        threads[nMD] = new MD_THREAD<G32>(g32, "MD32");
    }
#endif
/*--------------------------------------------------------------------------*/
#ifdef USE_MSVS_TRIVIAL
    if(!(mask & ( 1 << nMSVS ))){
    }else if( m <= M16){
        threads[nMSVS] = new MSVS_THREAD<ssg_16i>(g16, "MSVS16");
    }else{
        threads[nMSVS] = new MSVS_THREAD<ssg_32i>(g32, "MSVS32");
    }
#endif
/*--------------------------------------------------------------------------*/
#ifdef USE_FI
    if(! ( mask & ( 1 << nFI ))) { untested();
    }else if( m <= M16){ untested();
        threads[nFI] = new FI_THREAD<G16>(g16, "FI16");
    }else{ untested();
//        incomplete
//        threads[nFI] = new FI_THREAD<G32>(g32, "FI32");
    }
#endif
/*--------------------------------------------------------------------------*/
#ifdef USE_P17
    if(! ( mask & ( 1 << nP17 ))) {
    }else if( m < M16){ untested();
        // need "less than M16", due to bucket sorter quirk
        threads[nP17] = new P17_THREAD<G16, grtd_algo_config>(g16, "P17_16");
    }else{ untested();
        threads[nP17] = new P17_THREAD<G32, grtd_algo_config>(g32, "P17_32");
    }
#endif
/*--------------------------------------------------------------------------*/
#ifdef USE_SOME // 16 bit
    if(!(mask & ( 1 << nSOME ))) { untested();
    }else if( m <= M16){
        threads[nSOME] = new SOME_THREAD<G16>(g16, "SOME");
    }
#endif
/*--------------------------------------------------------------------------*/
#ifdef USE_BMD
    if(! ( mask & ( 1 << nBMD ))) {
    }else if(m <= M16){
        threads[nBMD] = new BMD_THREAD<G16, grtd_algo_config>(g16, "BMD16", m);
    }else if(m <= M32){ untested();
        threads[nBMD] = new BMD_THREAD<G32, grtd_algo_config>(g32, "BMD32", m);
    }else{
        incomplete();
        // need 64 bit BMD...
        // perhaps gala<vec,vec,void*,directed>...?
        // (distribute?)
//        threads[nBMD] = new BMD_THREAD<G64>(g32, results[nBMD]);
    }
#endif
/*--------------------------------------------------------------------------*/
#if defined(USE_TA) && defined(USE_GALA)
    if(! ( mask & ( 1 << nTA ))) { untested();
    }else if(m > M15){ untested();
        // does this even make sense?
        // maybe for very sparse graphs...
        // threads[nTA] = new TA_THREAD<G32>(boost::ref(g32), "TA32");
        threads[nTA] = new TA_THREAD<G32>(g32, "TA32");
    }else{
        // threads[nTA] = new TA_THREAD<G16>(boost::ref(g16), "TA16");
        threads[nTA] = new TA_THREAD<G16>(g16, "TA16");
    }
#endif
/*--------------------------------------------------------------------------*/
#if defined(USE_EX) && defined(USE_GALA)
    if(! ( mask & ( 1 << nEX ))) {
    }else if(m > M16){ untested();
        // does this even make sense?
        // maybe for very sparse graphs...
        threads[nEX] = new EX_THREAD<G32>(g32, "EX32");
    }else{
        threads[nEX] = new EX_THREAD<G16>(g16, "EX16");
    }
#endif
/*--------------------------------------------------------------------------*/

    --TWTHREAD_BASE::_running;

//     while(running!=finished){ untested();
//         std::unique_lock<std::mutex> lk(best_mutex);
//         cv.wait(lk);
//     }


    mainloop();
    trace0("exited mainloop");

    if(trace){
        unsigned x=1;
        for(unsigned i=0; i<nTOTAL; ++i){
            if(! (x&mask) ){
            }else if(!threads[i]){
            }else
                //if(threads[i]->get_result()!=-1u)
               {
                std::cout << "c " << names[i] << ": "
                    << threads[i]->get_result() << "\n";
           // }else{
            }
            x*=2;
        }
    }

    trace0("acquiring lock");
    std::lock_guard<std::mutex> scoped_lock(best_mutex); // needed?


    unsigned best_tid=find_best(threads);
    trace2("found best", best_tid, names[best_tid]);
    grtd_algo_config<balu_t>::message(bDEBUG, "best: %s\n", names[best_tid].c_str());

    if(quiet){ untested();
    }else{

        switch(best_tid){
        case nSOME:
        case nBMD:
#ifdef USE_GALA // tmp hack
            // g.make_symmetric(true);
#endif
        default: itested();
            threads[best_tid]->interrupt(); // uuh, wait here until copy is finished?!
            grtd_algo_config<balu_t>::message(bDEBUG, "printing result\n");
            threads[best_tid]->print_results(std::cout);
            break;
        case nTOTAL:
            std::cout << "no result\n";
        } // switch

        std::cout << "c done\n";

    }
    cleanup(threads);
}

// vim:ts=8:sw=4:et:

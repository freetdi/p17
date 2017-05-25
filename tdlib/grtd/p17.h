#ifndef P17_THREAD_H
#define P17_THREAD_H

#include <tdlib/algo.hpp>
#include <tdlib/generic_elimination_search_configs.hpp>
#include <tdlib/message.hpp>

template<class G, template<class H, class ... > class cfgt=treedec::algo::default_config>
class P17_THREAD : public TWTHREAD<G, cfgt> {
public:
    using vertex_descriptor=typename boost::graph_traits<G>::vertex_descriptor;
    using vertices_size_type=typename boost::graph_traits<G>::vertices_size_type;
    typedef TWTHREAD<G, cfgt> base;
    typedef cfgt<G> CFG;
    typedef std::vector<vertex_descriptor> iorder_t;
    typedef treedec::gen_search::configs::CFG_DFS_p17<G,
            base::template cfgt > algo_type;
private:
    using base::_g;

public:
    P17_THREAD(G const& g, const std::string& name)
        : base(g, name, 0)
        // _input_reference(g)
    { untested();
        long long m=std::numeric_limits<vertices_size_type>::max();
        CFG::message(bDEBUG, "running with maxnodes %lld\n", m);

        auto nv=boost::num_vertices(g);
        assert(nv<=m);

        // TODO: should not need _input_reference.
        // then, use base(g,name) instead.
        base::go();
    }

    void do_print_results(std::ostream& o) { untested();
        // TODO: base_g should be treedec-style already
        // _input_reference.make_symmetric(true);
        base::print_results_order(o, _elimord);
    }

    void run() { untested();
        CFG::message(0, "run %p\n", this);

        _work = base::_g;
//        _work.make_symmetric(true); // TODO...

        G g2;
        std::vector<size_t> m;
        treedec::impl::preprocessing<G> PP(_work);
        _elimord.resize(boost::num_vertices(_g));
        auto ie=_elimord.begin();
        { // HACK. bug.
            PP.do_it();
            PP.get_subgraph_copy(g2, m);
            PP.paste_elims(ie, _elimord.end());
        }

        algo_type generic_elim_DFS_test(g2);

        try{
            generic_elim_DFS_test.do_it();
            CFG::message(bDEBUG, "finished\n");
        }catch(boost::thread_interrupted){
            CFG::message(bDEBUG, "interrupt\n");
        }

        generic_elim_DFS_test.paste_ordering(ie, _elimord.end(), m);
        size_t A=generic_elim_DFS_test.global_upper_bound_bagsize(); // does not work

#if 1 // BUG, but where?!
        _work=base::_g;
        // _work.make_symmetric(true);
        assert_permutation(_elimord);
        size_t B=treedec::get_bagsize_of_elimination_ordering(_work, _elimord);
        if(A!=B){ unreachable();
            CFG::message(bWARNING, "bug? %d %d\n", A, B);
        }else{
        }
#endif

        base::commit_result(B);
        base::unlock_results();
        // IF ALONE && noTERMyet
        // kill(getpid(), SIGTERM);
        base::just_wait();

    }
private:
//    result_t _result;
    G _work;
//    G const& _input_reference; // needed for output // HACK
    // why?
    //iorder_t _elimord;
    iorder_t _elimord;
};

#endif

// vim:ts=8:sw=4:et

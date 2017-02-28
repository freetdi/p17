#ifndef FITHREAD_H
#define FITHREAD_H

//typedef sg_dvv16 GWORKFI;
typedef ssg16 GWORKFI;

template<class G>
class FI_THREAD : public TWTHREAD<GWORKFI> {
public:
    typedef TWTHREAD<GWORKFI> base;
    FI_THREAD( G g /* create local copy*/, const std::string& name )
        : base(g, name), _work(g) // <= stored here
    { untested();
// #ifdef USE_GALA
//         h = g;
// #endif
//        base::go();
    }

    void do_print_results(std::ostream& o)
    { untested();
        // auto &g=TWTHREAD<G>::_g;
        return base::print_results_tree(o, _t, &_work); /// HACK passing &_g, for now.
    }

    void run()
    { untested();
#if 1
        treedec::fillIn_decomp(_work, _t);
#else
        typedef typename boost::graph_traits<sg_dvv16>::vertex_descriptor vertex_descriptor;
        std::vector<vertex_descriptor> O;
        treedec::impl::endless_fillIn_ordering(_work, O);
#endif
        unsigned r = treedec::get_bagsize(_t);
        assert(boost::num_vertices(_work) || r==0);
        base::commit_result(r);
        base::unlock_results();
        base::just_wait();

//    NOT YET
//     G = H;
//     treedec::MSVS(G, T);
//     w = treedec::get_bagsize(T);
//     update_best_bagsize(w, "FI+MSVS");
//
    }
private:
   result_t _result;
#ifdef USE_GALA
   // work on svbs
   // INCOMPLETE, hardwire 16bit!
   GWORKFI _work;
   decomp_t<GWORKFI> _t;
#else
   incomplete
   decomp_t<G> _t;
   G _work;
#endif
   // why?
   //iorder_t _elimord;
};

#endif

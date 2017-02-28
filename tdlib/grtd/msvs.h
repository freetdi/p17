
template<class G>
class MSVS_THREAD : public TWTHREAD<G> {
public:
    typedef decomp_t<G> T;
    typedef typename boost::graph_traits<G>::vertices_size_type vertices_size_type;
private:
    class MSVS : public treedec::impl::MSVS<G, T> {
        typedef treedec::impl::MSVS<G, T> base;
    public:
        MSVS(G const& g, T& t, MSVS_THREAD& m)
           : base(g, t), _thr(m)
        { untested();
        }
        ~MSVS()
        { untested();
        }
    private:
        void callback(vertices_size_type x)
        { untested();
            _thr.cb(x);
        }
    private:
        MSVS_THREAD& _thr;
    };
public:
    typedef TWTHREAD<G> base;

    MSVS_THREAD( G g /* create local copy*/, const std::string& name )
        : TWTHREAD<G>(_g /*bug, not initialized yet??*/, name, 0), _g(g) // <= stored here
    { untested();
        trace2("MSVS_THREAD", &_g, boost::num_vertices(_g));

        treedec::trivial_decomposition(_g, _t);

        base::go();
    }

    void do_print_results(std::ostream& o)
    { untested();
        trace2("md dpr", boost::num_vertices(_g), &_g);
        return base::print_results_tree(o, _t, &_g); /// HACK passing &_g, for now.
    }

    void cb(unsigned x)
    { itested();
        if(x){ untested();
            trace1("msvs cb", x);
            base::commit_result(x);
        }else{ untested();
            trace0("msvs interrupt");
        }
        boost::this_thread::interruption_point();
    }

    void run()
    {
        // what's this for?!
        std::vector<boost::tuple<
            typename treedec::treedec_traits<typename treedec::treedec_chooser<G>::type>::vd_type,
            typename treedec::treedec_traits<typename treedec::treedec_chooser<G>::type>::bag_type
                > > bags;

     //   treedec::preprocessing(_g, bags);

        MSVS A(_g, _t, *this); // constructor?
        try{ untested();
            A.do_it();
        }catch(.../*FIXME*/){ untested();
            //base::unlock_results();
            //base::just_wait();
        }
        //    treedec::separator_algorithm(g, T);
        unsigned w = treedec::get_bagsize(_t);
        assert(w);
        base::commit_result(w);
        if(trace){
            std::cerr << "MSVS: " << w << "\n";
        }
        base::unlock_results();
        base::just_wait();
    }
private:
//   result_t &_result;
#ifdef USE_GALA
   decomp_t<G> _t;
   G _g;
#else
   decomp_t<G> _t;
   G _g;
#endif
}; // MSVS_THREAD

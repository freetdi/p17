// Lukas Larisch, 2014 - 2016
// Felix Salfelder, 2016
//
// (c) 2014-2016 Goethe-Universität Frankfurt
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
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
//
// greedy heuristics

#ifndef TD_IMPL_GREEDY_HPP
#define TD_IMPL_GREEDY_HPP

#ifndef TD_ELIMINATION_ORDERINGS
#error "not intended to be used like that."
#endif

// yuck. FIXME
#include "../algo.hpp"
#include "../induced_subgraph.hpp"
#include "../marker_util.hpp"

#include "greedy_base.hpp"

namespace treedec{ //

namespace impl{ //

// obsolete. used by minDegree only
template <typename G_t, typename T_t, typename O_t, template<class G, class...> class CFGT_t=algo::default_config>
class greedy_heuristic_base : public ::treedec::algo::draft::algo1{
public:
    typedef typename boost::graph_traits<G_t>::vertex_descriptor vertex_descriptor;
    typedef typename boost::graph_traits<G_t>::vertices_size_type vertices_size_type;
    typedef typename boost::graph_traits<G_t>::adjacency_iterator adjacency_iterator;
    typedef typename std::vector<vertex_descriptor> bag_t;

    greedy_heuristic_base(G_t &G, T_t *T, O_t *O, unsigned ub, bool ignore_isolated_vertices=false)
      : algo1("."), _g(G), _t(T), _o(O), _own_o(!O), _ub_in(ub), _iiv(ignore_isolated_vertices), _i(0),
        _min(0), _ub(0), _current_N(NULL), _num_vert(boost::num_vertices(_g))
    {
        if(_own_o){
            _o = new O_t;
        }else{
        }

        //the following seems to be unnecessary
        if(_t){
            _bags.resize(_num_vert);
        }else{
        }

        _o->resize(_num_vert);
    }


    ~greedy_heuristic_base(){
        if(_own_o){
            delete _o;
        }else{
        }
    }

    void tree_decomposition(){
        assert(_t);
        assert(_o->size()<=_bags.size()); // this is obsolete anyway
        assert(_o->size()==_num_vert); // this is relevant.

        // yuck... will be obsolete with FI rework
        typename std::vector<
            std::pair<vertex_descriptor, bag_t>
                > bags(_num_vert);
        typename std::vector<unsigned> io(_num_vert);

        // stuff center and friends into "skeleton"
        // _num_vert can be less than order/bags size
        for(unsigned i = 0; i < _num_vert; i++){
            bags[i].first = (*_o)[i];
            bags[i].second = _bags[i];
            // io[ (*_o)[i] ] = i;
        }

        treedec::detail::skeleton_to_treedec(_g, *_t, bags, *_o, _i);
    }

    vertices_size_type get_bagsize(){
        return _ub+1;
    }

    O_t& elimination_ordering() {
        return *_o;
    }

    virtual void initialize() = 0;
    virtual void next(vertex_descriptor &c) = 0; // FIXME: switch to template override
    virtual void eliminate(vertex_descriptor v) = 0;
    virtual void postprocessing() = 0;

    void do_it(){
        trace2("fillin::do_it()", boost::num_vertices(_g), boost::num_edges(_g));
        timer_on();

        if(!_num_vert){
            timer_off();
            return;
        }

        assert(_o);
        O_t& elim_vertices = *_o;

#ifndef NDEBUG
        check(_g);
#endif

        initialize();

        _o->resize(_num_vert);

        assert(elim_vertices.size() == _num_vert);

        while(boost::num_edges(_g) > 0){
            vertex_descriptor c;

            next(c);

            //Abort if the width of this decomposition would be larger than 'ub'.
            if(_min >= _ub_in){ untested();
                assert(_t); // ouch?
                _t->clear(); //could be also not the case
                throw exception_unsuccessful();
            }

            elim_vertices[_i] = get_vd(_g, c);

            if(_t){
                _current_N = &_bags[_i];
            }else{
            }

            _ub = (boost::out_degree(c, _g)>_ub)?boost::out_degree(c, _g):_ub;

            // assert(bags_i);?!

            eliminate(c);

            if(!_current_N){
            } else if(!_t){ untested();
                _current_N->clear();
            }else{
            }

            ++_i;
        }

        postprocessing();

        timer_off();

    } // do_it


protected:
    G_t &_g;
    T_t* _t;
    O_t* _o;
    bool _own_o;

    vertices_size_type _ub_in;
    bool _iiv;
    size_t _i;
    unsigned _min;

    std::vector<bag_t> _bags;

    vertices_size_type _ub;

    bag_t* _current_N;

    unsigned _num_vert;

}; // greedy_heuristic_base


template <typename G_t, typename T_t, typename O_t>
// BUG: use greedy_base
class minDegree : public greedy_heuristic_base<G_t, T_t, O_t>{
public:
    typedef greedy_heuristic_base<G_t, T_t, O_t> baseclass; //BUG

    typedef typename deg_chooser<G_t>::type degs_type;

    minDegree(G_t &g, T_t *t, O_t *o,
                    unsigned ub=UINT_MAX, bool ignore_isolated_vertices=false)
        : baseclass(g, t, o, ub, ignore_isolated_vertices),
         _degs(baseclass::_g)
    {
    }

    minDegree(G_t &G, O_t& o, bool ignore_isolated_vertices)
        : baseclass(G, NULL, &o, -1u, ignore_isolated_vertices),
          _degs(baseclass::_g)
    {
    }

    void initialize(){
        auto zerodegbag1=MOVE(_degs.detach_bag(0));
        BOOST_AUTO(it, zerodegbag1.begin());

        if(!baseclass::_iiv){
            for(; it!=zerodegbag1.end(); ++it){
                (*baseclass::_o)[baseclass::_i++] = get_vd(baseclass::_g, *it);
            }
        }
        else{
            baseclass::_num_vert -= zerodegbag1.size();
        }

        baseclass::_min = 1;
    }

    void next(typename baseclass::vertex_descriptor &c){
        if(baseclass::_min>1){
            --baseclass::_min;
        }else{
        }

        auto p=_degs.pick_min(baseclass::_min, baseclass::_num_vert);
        c = p.first;
        baseclass::_min = p.second;
    }

    // minDegree::
    void eliminate(typename baseclass::vertex_descriptor v){ untested();
        incomplete(); // vectors?!
        auto I=boost::adjacent_vertices(v, baseclass::_g);
        for(; I.first!=I.second; ++I.first){
            assert(*I.first!=v); // no self loops...
            _degs.unlink(*I.first);
        }

        if(baseclass::_current_N){
            baseclass::_current_N->resize(boost::out_degree(v, baseclass::_g));
            make_clique_and_detach(v, baseclass::_g, *baseclass::_current_N);
            redegree(NULL, baseclass::_g, *baseclass::_current_N, _degs);
        }else{
            // BUG
            typename baseclass::bag_t tmp(boost::out_degree(v, baseclass::_g));
            make_clique_and_detach(v, baseclass::_g, tmp);
            redegree(NULL, baseclass::_g, tmp, _degs);
        }
        _degs.unlink(v, baseclass::_min);
        _degs.flush();
    }

    void postprocessing(){
        auto zerodegbag=MOVE(_degs.detach_bag(0));
        BOOST_AUTO(it, zerodegbag.begin());

        for(; it!=zerodegbag.end(); ++it){
            (*baseclass::_o)[baseclass::_i++] = get_vd(baseclass::_g, *it);
        }
    }

    void reset(){
        // 9d228f34fa8?!
        incomplete();
    }


private:
    degs_type _degs;

}; // minDegree

// the fillIn heuristic.
template<typename G_t,
         template<class GG, class ...> class CFGT=algo::default_config>
class fillIn : public greedy_base<
               G_t,
               std::vector<typename boost::graph_traits<G_t>::vertex_descriptor>,
               CFGT>{
public: //types
    typedef std::vector<typename boost::graph_traits<G_t>::vertex_descriptor> O_t;
    typedef typename directed_view_select<G_t>::type D_t;
    typedef typename boost::graph_traits<D_t>::vertices_size_type vertices_size_type;
    typedef greedy_base<G_t, O_t, CFGT> baseclass;
    typedef typename baseclass::vertex_descriptor vertex_descriptor;
    typedef typename fill_chooser<typename baseclass::subgraph_type>::type fill_type;

    struct fill_update_cb : public graph_callback<typename baseclass::subgraph_type>{
        typedef typename baseclass::subgraph_type G;

        fill_update_cb(fill_type* d, G const& g) :
            _fill(d), _g(g)
        {
        }

        void operator()(vertex_descriptor v){ untested();
            unreachable();
            _fill->q_eval(v);
        }
        // q_decrement nodes that are incident to both endpoints.
        void operator()(vertex_descriptor, vertex_descriptor) { unreachable();
        }
    private:
        fill_type* _fill;
        G const& _g;
    }; // update_cb

public: // construct
    fillIn(G_t &g,// T_t *t, O_t *o,
                    unsigned ub=UINT_MAX, bool ignore_isolated_vertices=false)
        : baseclass(g, ub, ignore_isolated_vertices),
          _fill(baseclass::_subgraph, boost::num_vertices(g))
          // _cb(fill_update_cb(&_fill, baseclass::_subgraph))
    {
    }

    fillIn(G_t &g, // O_t& o,
            bool ignore_isolated_vertices, unsigned ub=-1u)
        : baseclass(g, ub, ignore_isolated_vertices),
          _fill(baseclass::_subgraph, boost::num_vertices(g))
          // _cb(fill_update_cb(&_fill, baseclass::_subgraph))
    {
    }

public: // implementation

    bool next(typename baseclass::vertex_descriptor &c){
        trace1("next", baseclass::_num_edges);
        if(baseclass::_num_edges){
            _fill.check();
            auto p=_fill.pick_min(0, -1u, true);
            c = p.first;
            baseclass::_min = p.second;
            _fill.check();
            return true;
        }else{
            return false;
        }

    }

    // TODO more useful specialisation?
    // ... finish minDegree, then lets see.
    void eliminate(typename baseclass::vertex_descriptor v){
        trace3("elim", v, baseclass::_num_edges, baseclass::_degree[v]);

        _fill.mark_neighbors(v, baseclass::_min); // relevant in q_decrement
                                                  // different marker

        baseclass::_numbering.put(v);
        baseclass::_numbering.increment();

        { // make clique
            // for new edges, call cb.
#if 0
            make_clique_and_detach(v, baseclass::_g, *baseclass::_current_N, &_cb);
#else
            assert(baseclass::_num_edges >= baseclass::_degreemap[v]);
            baseclass::_num_edges -= baseclass::_degreemap[v];
            auto p=boost::adjacent_vertices(v, baseclass::_subgraph);
            for(; p.first!=p.second; ++p.first){
                _fill.q_eval(*p.first);

                baseclass::_marker.clear();
                mark_neighbours(baseclass::_marker, *p.first, baseclass::_subgraph);
                auto q=adjacent_vertices(v, baseclass::_subgraph);
                for(; q.first!=q.second; ++q.first){
                    if(*q.first>=*p.first){
                        // skip. TODO: more efficient skip
                    }else if(baseclass::_marker.is_marked(*q.first)){
                        // done, reachable from *p.first
                    }else{ itested();
                        ++baseclass::_degreemap[*p.first];
                        ++baseclass::_degreemap[*q.first];
                        ++baseclass::_num_edges;
                        trace2("addedge", *p.first, *q.first);
                        // why does it default to boost::??!
                        assert(*p.first != *q.first);
                        treedec::add_edge(*p.first, *q.first, baseclass::_g);
                        assert(boost::edge(*p.first, *q.first, baseclass::_g).second);
                        assert(boost::edge(*q.first, *p.first, baseclass::_g).second);

                        // q_decrement nodes that are incident to both endpoints.
                        // _cb(*p.first, *q.first);
                        auto r=adjacent_vertices(*q.first, baseclass::_subgraph);
                        for(; r.first!=r.second; ++r.first){
                            if(baseclass::_marker.is_marked(*r.first)){
                                // this is/was a neigh of p.
                                trace1("qdec", *r.first);
                                _fill.q_decrement(*r.first);
                            }else{
                            }
                        }

                    }
                }
                // disconnect center.
                --baseclass::_degreemap[*p.first];
            }

#endif
        }

        trace2("elimd", v, baseclass::_num_edges);
    }

    void postprocessing(){
        if(baseclass::_i == baseclass::_num_vert){
            // no nodes at all?!
        }else{
            // one missing
            ++baseclass::_i;
            auto v = _fill.pick_min(0, 0, true).first;
            assert(baseclass::_i == baseclass::_o->size());
            baseclass::_o->back() = v;
            baseclass::_numbering.put(v);
        }
        assert(baseclass::_i == baseclass::_num_vert);
    }

private:
    fill_type _fill;
//    fill_update_cb _cb;
}; // fillIn

} // namespace impl

} // namespace treedec

#endif // guard

// vim:ts=8:sw=4:et

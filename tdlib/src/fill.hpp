// Felix Salfelder 2016
// Lukas Larisch 2016
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

#ifndef TD_FILL_HPP
#define TD_FILL_HPP

// use local copy
#include "bucket_sorter_bits.hpp"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

#include <assert.h>

#include "directed_view.hpp"
#include "graph.hpp"
#include "trace.hpp"
#include "util.hpp"

#if __cplusplus >= 201103L
# include <unordered_set>
#endif

static bool const lazy_init=true;
static bool const catch_zeroes_in_decrement=true;

namespace misc { //

namespace detail { //
template<class G>
struct fill_config{ //
    typedef typename boost::graph_traits<G>::vertex_descriptor vd_type;
#if __cplusplus < 201103L
    typedef std::set<vd_type> bag_type;
#else
    typedef std::unordered_set<vd_type> bag_type;
#endif
    static void alloc_init(size_t)
    { untested();
    }
    static unsigned num_threads(){return 1;}
};
} // detail
} // misc


namespace misc{ // treedec::misc? hmmm.
 
template<class G_t, class CFG=detail::fill_config<G_t> >
class FILL{
    // FILL(const FILL&) : _degree(n, 0), _fill(n_ + 1, n_, _degree, id),
    // { untested();
    // }
public: // types
    typedef typename boost::graph_traits<G_t>::vertices_size_type vertices_size_type;
    typedef vertices_size_type size_type; //??
    typedef treedec::draft::sMARKER<vertices_size_type, vertices_size_type> marker_type;
private: // types
    class status_t{
    public:
        typedef size_type value_type;
    public:
        status_t() : _value(0), queued(false) {}
    public:
        size_type _value; // fill value, -1==unknown.
        bool queued; // it is not in a bucket right now.
                     // !queued means it's in bucket _fill[value].
        void operator=(size_type x)
        {
            _value = x;
        }
        operator size_type&()
        {
            return _value;
        }
    public:
        void set_unknown()
        { untested();
            _value = size_type(-1);
        }
        bool is_unknown() const
        {
            return(_value==size_type(-1));
        }
        size_t get_value() const
        {
            assert(!is_unknown());
            return _value;
        }
        void set_value(size_t v)
        {
            _value = v;
        }
//        bool operator==(size_t v) const{return _value==v;}
    };
public: // types
    typedef typename boost::graph_traits<G_t>::vertex_descriptor vertex_descriptor;
    typedef typename boost::graph_traits<G_t>::vertex_iterator vertex_iterator;
    typedef typename CFG::bag_type bag_type;
    typedef typename bag_type::iterator bag_iterator;
    typedef typename boost::property_map<G_t, boost::vertex_index_t>::const_type idmap_type;
    // FIXME: use vertex_size_type?
private:
    typedef boost::bucket_sorter<size_type, vertex_descriptor,
           boost::iterator_property_map<status_t*, idmap_type, size_type, size_type&>,
           idmap_type >
        container_type;
    // typedef typename container_type::iterator iterator;
    // typedef typename container_type::const_iterator const_iterator;
    typedef typename boost::graph_traits<G_t>::vertices_size_type fill_t;
private:
    size_t max_missing_edges() const
    { itested();
        size_t nv=_vals.size();
        // - isolated nodes?
//        size_t nv=boost::num_vertices(_g);
        return (nv-1)*(nv-2) / 2;
    }

public: // construct
    FILL(const G_t& g, unsigned nv /*bug*/)
       : _g(g),
         //_degree(boost::num_vertices(g), 0),
         _vi(boost::get(boost::vertex_index, g)),
         _vals(nv),
         _fill(nv, // length
               1+max_missing_edges(),     // max_bucket // "1+" BUG?
               boost::make_iterator_property_map(&_vals[0], _vi, size_type()),
               _vi),
         _neigh_marker(nv)
    {
        trace2("FILL", nv, max_missing_edges());

        _init = true;
        bool foundzero=false;
        auto p=boost::vertices(g);
        unsigned checksum=0;
        trace2("FILL", nv, p.first!=p.second);
        for(; p.first!=p.second; ++p.first){
            ++checksum;
            auto m=boost::get(boost::vertex_index, _g);
            auto pos=boost::get(m, *p.first);
            trace2("init", *p.first, pos);
            assert(_vals.size()>pos);
            (void) pos;
            auto deg=boost::out_degree(*p.first, g);

            if(deg){
                size_t missing_edges=-1;

                if(foundzero){
                    // bypass init. already found a nt. clique
                    q_eval(*p.first); //later.
                    assert(_vals[pos].is_unknown());
                    assert(contains(_eval_queue, *p.first));
                }else{
                    missing_edges = treedec::count_missing_edges(*p.first, _neigh_marker, _g);
                    trace2("reg", *p.first, missing_edges);
                    reg(*p.first, missing_edges);
                    assert(_vals[pos]==missing_edges);
                }

                if (!missing_edges){
                    trace2("found isol", *p.first, lazy_init);
                    // faster by a few percent. sometimes?
                    foundzero = lazy_init;
                }else{
                }
            }else{
                trace1("isolated node", *p.first);
            }
        }
        trace1("fill initialized", _eval_queue.size());
        assert(nv==checksum);
        _init = false;
    }
public: // check
    void check() {
#ifdef OLD_DEBUG // does no lnger work...
        for(auto p : _fill) { untested();
            assert(treedec::count_missing_edges(p.second, _neigh_marker, _g) == p.first);
        }
#endif
    }

public: // queueing
    void unlink(const vertex_descriptor& v, size_t f)
    { untested();
        assert(f>=0);
//        assert(it's there); // how?
        _fill.remove(v);

        unsigned int pos = boost::get(boost::get(boost::vertex_index, _g), v);
        _vals[pos].set_unknown();
        _vals[pos].queued = false;
    }
    void unlink(const vertex_descriptor& v)
    {
        assert(treedec::is_valid(v,_g));
        /// unsigned int pos = boost::get(boost::get(boost::vertex_index, _g), v);
        // unlink(v, _vals[pos]);
        _fill.remove(v);
    }

public:
    void reg(const vertex_descriptor& v, size_t missing_edges)
    {
        assert(treedec::is_valid(v,_g));
//        bool n=_fill.insert(std::make_pair(missing_edges,v)).second;
//        assert(n);
//        (void)n;

        unsigned int pos = boost::get(boost::get(boost::vertex_index, _g), v);
        _vals[pos] = missing_edges;
//        trace2("reg", v, pos);
        _fill.push(v); // updates reverse map. (necessary?!)
        _vals[pos].queued = false;
        assert(!_vals[pos].is_unknown() || _init);
    }
public:
    void reg(const vertex_descriptor v)
    { untested();
        size_t missing_edges=treedec::count_missing_edges(v, _neigh_marker, _g);
        reg(v, missing_edges);
    }
    void q_decrement(const vertex_descriptor v)
    {
        auto m=boost::get(boost::vertex_index, _g);
        auto pos=boost::get(m, v);
        if(_neigh_marker.is_marked(pos)){
            // it's a neighbour. don't touch.
            return;
        }else if(_vals[pos].is_unknown()){ untested();
            // unknown. don't touch.
            assert(_vals[pos].queued);
            assert(contains(_eval_queue, v));
            return;
        }else{
            // getting here, because v is a common neighbor of a new edge.
            // and not a neighbor...
            assert(_vals[pos].get_value()>0);
            q_eval(v, _vals[pos].get_value()-1);

            if(catch_zeroes_in_decrement && _vals[pos]==0){
                // stash for fill, in bucket #0
                reg(v, 0);
                // extra hack: leave in queue, but mark unqueued.
                // (it's not possible to remove from queue)
                _vals[pos].queued = false;
            }

            assert(contains(_eval_queue, v));
            return;
        }
    }

    // queue for later evaluation.
    // default value may be specified in case evalution turns out to be
    // not necessary
    void q_eval(const vertex_descriptor v, int def=-1)
    {
        auto m=boost::get(boost::vertex_index, _g);
        auto pos=boost::get(m, v);
        assert(def>=-1);

        if(def==-1 && _vals[pos].is_unknown()){
            assert(_init || contains(_eval_queue, v));
            return;
        }else if(!_vals[pos].queued){
            unlink(v);
            _eval_queue.push_back(v);
            _vals[pos].queued = true;
            assert(contains(_eval_queue, v));
        }else{
            // hmm queued w/default already, update default...
            assert(contains(_eval_queue, v));
        }
        _vals[pos] = def;
    }

public: // O(1) neighbor stuff.
    // for n \in neigbors(c):
    //   |X| = deg(n)-deg(c)
    //   queue new_fill(n) = old_fill(n) - old_fill(c) - |X|
    // (override in case n is incdent to a newly inserted edge)
    void mark_neighbors(vertex_descriptor c, size_t cfill)
    {
        _neigh_marker.clear();
        auto m=boost::get(boost::vertex_index, _g);
        auto posc=boost::get(m, c);
        (void) posc;

        // TODO:: need degree
        auto degc=boost::out_degree(c, _g);
        typename boost::graph_traits<G_t>::adjacency_iterator n, nEnd;

        auto p=boost::adjacent_vertices(c, _g);
        for(; p.first!=p.second; ++p.first){
            auto m=boost::get(boost::vertex_index, _g);
            auto const& n=p.first;
            auto pos=boost::get(m, *n);
            _neigh_marker.mark(pos);

            if(_vals[pos].is_unknown()){
                // neighbor fill unknown. leave it like that.
                assert(_vals[pos].queued);
                assert(contains(_eval_queue, *n));
                continue;
            }
            auto old_fill=_vals[pos].get_value();
            assert(old_fill>=0);

            auto degn=boost::out_degree(*n, _g);
            if(degn>=degc){
                long X = degn - degc;
                long new_fill = old_fill - cfill - X;
                if(new_fill < 0){
                    // new fill is wrong.
                    // there must be edges to fill adjacent to n...
                    q_eval(*n);
                }else{
                    q_eval(*n, new_fill);
                }
            }else{
                q_eval(*n);
            }
        }
    }

public: // picking
    // vertex_descriptor pick(unsigned fill)
    // { untested();
    //     return *_fill[fill].begin();
    // }
    // pick a minimum fill vertex within fill range [lower, upper]
    std::pair<vertex_descriptor, fill_t> pick_min(unsigned lower=0,
            unsigned upper=-1u, bool erase=false)
    {
        trace4("pickmin", erase, max_missing_edges(), lower, upper);
        if(upper!=-1u){
        }else{
        }

        if(upper>max_missing_edges()){
            upper = max_missing_edges();
        }else{
        }

        unsigned min_fill=0;
        auto list_min_fill = _fill[min_fill];
        if (list_min_fill.empty() && min_fill < upper) {
            ++min_fill;
            list_min_fill = _fill[min_fill];
        }

//        if(_fill.empty() || fp->first)

        if(list_min_fill.empty() || min_fill){

#if 1 // slightly slower...
        trace1("", _eval_queue.size());
        auto qi = _eval_queue.begin();
//        assert(qe!=qi || !_fill.empty());
        for(; qi!=_eval_queue.end(); ++qi){
            auto m=boost::get(boost::vertex_index, _g);
            auto pos=boost::get(m, *qi);
            size_type missing_edges;

            if(!_vals[pos].queued){
                // taken out of queue, because fill==0.
                assert(_vals[pos].get_value()==0);
                // ignore
                continue;
            }else{
            }

            if(_vals[pos].is_unknown()){
                missing_edges = treedec::count_missing_edges(*qi, _neigh_marker, _g);
            }else{
                missing_edges = _vals[pos]._value;
#ifndef NDEBUG
                auto mecheck=treedec::count_missing_edges(*qi, _neigh_marker, _g);
                assert(missing_edges == mecheck);
#endif
            }
            if(!missing_edges && erase){
                 // shortcut...?
                 // see below...
            }
            assert(missing_edges>=0);
            reg(*qi, missing_edges);
            assert(_vals[pos] == missing_edges);
            // assert(!contains(_eval_queue, *qi));
        }
        _eval_queue.clear();
#else // faster...? (try fifo?!)
        while(!_eval_queue.empty()){ untested();
            vertex_descriptor v=_eval_queue.back();
            _eval_queue.pop_back();
            unsigned pos = boost::get(boost::get(boost::vertex_index, _g), v);
            unsigned missing_edges = _vals[pos].value;

            if(!_vals[pos].queued){ untested();
                // taken out of queue, because fill==0.
                assert(missing_edges==0);
                // ignore
                continue;
            }

            if(missing_edges == -1u){ untested();
                // unknown...
                missing_edges = treedec::count_missing_edges(v, _g);
            }else{ untested();
                assert(missing_edges == treedec::count_missing_edges(v, _g));
            }
            if(missing_edges){ untested();
            }else if(erase){ untested();
                 // shortcut...
                _vals[pos].queued = false;
                return std::make_pair(v, 0);
            }
            reg(v, missing_edges);
        }
#endif
        }
        else{
            // no need to process q. it's already there
        }

//        assert(!_fill.empty());

        assert(lower==0); // for now.

        min_fill=0;
        list_min_fill = _fill[min_fill];
        while (list_min_fill.empty()) { itested();
            assert(min_fill<max_missing_edges());
            ++min_fill;
            list_min_fill = _fill[min_fill];
        }

//        BOOST_AUTO(b, _fill.begin());
        auto b=list_min_fill.top();
//         list_min_fill.pop();
        assert(treedec::is_valid(b, _g));

        auto pos = boost::get(boost::get(boost::vertex_index, _g), b);
        (void)pos;
        assert(!_vals[pos].is_unknown());
//        assert(_vals[pos]==b->first);

//        BOOST_AUTO(p, std::make_pair(b->second, b->first));
//        assert(treedec::is_valid(p.first, _g));
        if(erase){
            unlink(b);
            unsigned int pos = boost::get(boost::get(boost::vertex_index, _g), b);
            _vals[pos].set_value(0);
            assert(!_vals[pos].queued);
        }else{ untested();
        }

        // not really. why?!
        // assert(treedec::count_missing_edges(p.first,_g) == p.second);
        return std::make_pair(b, min_fill);
    }

#if 0
    size_t num_nodes() const
    { untested();
        unsigned N=0;
        for(const_iterator i=_fill.begin(); i!=_fill.end(); ++i) { itested();
            N+=i->size();
        }
        return N;
    }
#endif

#if 0
    bag_type const& operator[](size_t x) const
    { untested();
        return _fill[x];
    }
    size_t size() const
    { untested();
        return _fill.size();
    }
#endif

#if 0
public:
    bag_type& operator[](size_t x)
    { untested();
        return _fill[x];
    }
#endif

private:
private:

    bool _init; // initializing.
    const G_t& _g;
    idmap_type _vi;
//private: // later.
    std::vector<status_t> _vals;
    container_type _fill;

//    mutable std::set<vertex_descriptor> _eval_queue;
    typedef std::vector<vertex_descriptor> eq_t;
    mutable eq_t _eval_queue;

    marker_type _neigh_marker; // used in pick_min and in eliminate
}; // FILL

} //namespace misc

namespace treedec{ //

template<class G_t>
struct fill_chooser{ //
    typedef typename misc::FILL<G_t> type;
};

}

//transition, don't use.
namespace noboost{ //
    using treedec::fill_chooser;
}

#endif //guard

// vim:ts=8:sw=4:et

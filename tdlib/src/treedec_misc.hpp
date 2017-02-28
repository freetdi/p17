// Felix Salfelder 2016
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
// tree decomposition operations

#ifndef TREEDEC_MISC_H
#define TREEDEC_MISC_H

namespace treedec{
namespace draft{

    // do I need a class here?!
template<class G, class M>
class applyGmap{//
public:
    applyGmap(G const& g, M const& m) : _g(g), _m(m)
    {
    }

    typedef unsigned result_type;

    unsigned operator()(unsigned x) const
    { itested();
        if(get_pos(x,_g) == x){
            // simple numbering perhaps
        }else{ untested();
        }
        return _m[get_pos(x,_g)];
    }
private:
    G const& _g;
    M const& _m;
};

// append one decomposition to another.
// (take care of node mapping, mainly)
template <typename T_t, class S_t, class G_t, class M_t>
void append_decomposition(T_t &tgt, S_t const&& src, G_t const& /*GR*/, M_t const& map)
{
    auto op=applyGmap<S_t, M_t>(src, map);
//    assert(undirected_tree)...
    unsigned offset=boost::num_vertices(tgt);
    auto SR=boost::vertices(src);
    auto TR=boost::vertices(tgt);
    trace3("append", SR.second-SR.first, TR.second-TR.first, map.size());

    if(SR.first==SR.second){ untested();
        // no bags to fetch. done
    }else{
        unsigned new_tv;
        auto next=SR.first;
        for(; SR.first!=SR.second; SR.first=next){
            trace1("appendfound", *SR.first);
            ++next;
            new_tv=boost::add_vertex(tgt);
            auto& B=bag(new_tv, tgt);
            auto const& SB=bag(*SR.first, src);

            assert(boost::degree(*SR.first, src)>0);
            assert(boost::out_degree(*SR.first, src)>0);
            auto target_in_copy = *boost::adjacent_vertices(*SR.first, src).first;
            if(target_in_copy<*SR.first){
                add_edge(new_tv, target_in_copy+offset , tgt );
            }

#if 0 // vector version (how to select?)
            B = std::move(bag(*SR.first, src));
            // now apply map
            //
            for(auto & x: B){ untested();
                x = map[get_pos(x,src)];
            }
#else

            using draft::applyGmap;
            using boost::bind1st;
            using boost::make_transform_iterator;
            push(B, make_transform_iterator(SB.begin(), op ),
                    make_transform_iterator(SB.end(),   op ));
#endif
        }

        if(offset){
            // connect new stuff to existing.
            boost::add_edge(new_tv, *boost::vertices(tgt).first, tgt);
        }else{ untested();
        }
    }
}

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

template<class V, class P>
void permute_vector(V& vec, P &perm)
{
	assert_permutation(perm);
	unsigned seek=0;
	unsigned n=vec.size();

	for(; seek<n;){ untested();

		if(perm[seek]<=seek){ untested();
			// already done.
		}else{
			typename V::value_type tmp = vec[seek];
			unsigned i = perm[seek];
			while(i!=seek){ untested();
				std::swap(tmp, vec[i]);
				perm[i]=seek;
				i=perm[i];
			}
			vec[i]=tmp;
		}
		++seek;
	}
}

template<class V, class P>
void concat_maps(V& first, P const&second){
	unsigned n=first.size();
	for(unsigned i=0; i<n; ++i){
		first[i]=second[first[i]];
	}
}

} // draft

} // treedec

#endif

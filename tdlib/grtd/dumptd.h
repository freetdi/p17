// Lukas Larisch, 201?
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
//
// this is obsolete. use tdlib/printer.hpp in new code
//
#ifndef DUMPTD_H
#define DUMPTD_H

template <typename T_t, typename G_t>
inline void dumptd(T_t const &T, G_t const& G)
{
    unsigned n_graph=boost::num_vertices(G);
    unsigned int n_decomp = boost::num_vertices(T);
    int width = treedec::get_width(T)+1; //size of maximum bag and NOT the width (see specification)

    //solutions line
    std::cout << "s " << "td " << n_decomp << " " << width << " " << n_graph << std::endl;

    typename boost::graph_traits<T_t>::vertex_iterator vIt, vEnd;
    for(boost::tie(vIt, vEnd) = boost::vertices(T); vIt != vEnd; vIt++){
        std::cout << "b " << treedec::get_pos(*vIt, T)+1;

        for(auto const& e : treedec::bag(*vIt, T)){
            std::cout << " " << treedec::get_pos(e, G)+1;
        }
        std::cout << std::endl;
    }
    typename boost::graph_traits<T_t>::edge_iterator eIt, eEnd;
    for(boost::tie(eIt, eEnd) = boost::edges(T); eIt != eEnd; eIt++){
        auto s=boost::source(*eIt, T);
        auto t=boost::target(*eIt, T);
        auto ps=treedec::get_pos(s, T);
        auto pt=treedec::get_pos(t, T);
        if(1){
            std::cout << ps+1 << " " << pt+1 << std::endl;
        }else{
            std::cout << pt+1 << " " << ps+1 << std::endl;
        }
    }
}

#endif
// vim:ts=8:sw=4:et:

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

#define TD_DYNAMICCR
namespace treedec{
template<class G, class T, class lb>
  void CR_dynamic_decomp(G, T, lb);
}

#include <iostream>
#include <stdlib.h>

#include <gala/boost.h>
#include <tdlib/directed_view.hpp>
//#include <tdlib/treedec.hpp>
// #include <gala/examples/ssg16i.h>
#include <gala/examples/svbs.h>
#include <gala/td.h>
#include <gala/immutable.h>
#include <gala/boost_copy.h>
#include <tdlib/treedec.hpp>
#include <tdlib/tuple_td.hpp>
#include <tdlib/combinations.hpp>
#include <tdlib/elimination_orderings.hpp>
#include <tdlib/printer.hpp>
#include <tdlib/twthread.hpp>

#include <tdlib/combinations.hpp>
//#include <tdlib/treedec.hpp>

#include "grparse.h"
#include "dumptd.h"

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS> balu_t;
typedef gala::graph<std::vector, std::vector, uint16_t> sg_dvv16;

#if 1 // slow preprocessing
//typedef ssg_16i graph_t;
typedef balu_t graph_t;
#else // broken preprocessing
typedef sg_dvv16 graph_t;
#endif

typedef typename treedec::graph_traits<graph_t>::treedec_type decomp_t;
//typedef typename treedec::graph_traits<graph_t>::immutable_type immutable_type;

int main(int argc, char * const * argv)
{
    bool quiet=false;
    bool trace=false; (void)trace;
    unsigned i=1;
    unsigned decide=0;
    while(i<(unsigned)argc){
        if(!strncmp("-q", argv[i], 2)){
            quiet=true;
        }else if(!strncmp("-t", argv[i], 2)){
            trace=true;
        }else if(!strncmp("-d", argv[i], 2)){
            ++i;
            decide=atoi(argv[i]);
        }else if(!strncmp("-s", argv[i], 2)){
            // not yet
            ++i;
        }
        ++i;
    }

    PARSE* p;

    try{
        p = new PARSE(std::cin);
    }catch(...){
        std::cerr << "error parsing header\n";
        exit(2);
    }

    graph_t g(p->begin(), p->end(), p->num_vertices(), p->num_edges());
    decomp_t T;

    if(decide){
        treedec::exact_cutset(g, T, decide-1);
    }else{
        treedec::exact_decomposition_cutset(g, T);
    }
    trace1("cutset decomposition done", treedec::get_width(T));

    if(quiet){
        std::cout << treedec::get_bagsize(T)<< "\n";
    }else{
        dumptd(T, g);
    }
}
// vim:ts=8:sw=4:et:

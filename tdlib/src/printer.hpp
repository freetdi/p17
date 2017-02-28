// Felix Salfelder, 2016
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

#ifndef TD_PRINTER_HPP
#define TD_PRINTER_HPP

#include <ostream>
#include <string>
#include "graph_traits.hpp"
#include <boost/graph/graph_traits.hpp>

namespace treedec{

	// print a tree decomposition in gr format.
	template<class G>
	class grtdprinter{ //
	public:
		grtdprinter(std::ostream& s, G const& g, std::string reason="td")
			: _num_vertices(0),_s(s),_g(g),_reason(reason)
		{
			_offset=1;
			_nva=0;
		}
		~grtdprinter()
		{
			std::cout << "\n";
		}

		virtual void head(size_t numbags=0, size_t bagsize=0, size_t numvert=0)
		{
			auto ngv=boost::num_vertices(_g);
			if(numbags==0 && bagsize==0 && ngv){ incomplete();
				// need to cache results... not now.
			}
			std::cout << "s " << _reason << " " << numbags
			          << " " << bagsize << " " << ngv;
			_num_vertices=numvert;
		}

		void add_vertex()
		{ itested();
		//	std::cout << "\n" << ++_nva;
		}
		void edge(size_t x, size_t y)
		{ itested();
			std::cout << "\n" << x+_offset << " " << y+_offset;
		}
		void announce_bag(size_t x)
		{
			assert(_nva==x); // for now
			std::cout << "\nb " << ++_nva;
		}
		void push_back(size_t x)
		{ itested();
			std::cout << " " << x+_offset;
		}
		// kludge: just tell something has been added.
		// (which is not completely wrong)
		std::pair<bool, bool> insert(size_t x)
		{ itested();
			std::cout << " " << x+_offset;
			return std::make_pair(true, true);
		}

	private:
		unsigned _nva;
		size_t _num_vertices;
		unsigned _offset;
		std::ostream& _s;
		G const& _g;
		std::string _reason;
	};

	// fill bags. one at a time.
	template<class G>
	grtdprinter<G>& bag(size_t i, grtdprinter<G>& g)
	{
		g.announce_bag(i);
		return g;	
	}

	template<class T>
	struct treedec_traits<grtdprinter<T> >{
	};

} // treedec

namespace boost{

	// uuh, really pass vertex_descriptors
	// (it was late and i was tired)
	template<class G>
	void add_vertex( treedec::grtdprinter<G>& p)
	{
		p.add_vertex();
	}
	template<class G>
	void add_edge(size_t x, size_t y,  treedec::grtdprinter<G>& p)
	{
		p.edge(x,y);
	}

}// boost

#endif

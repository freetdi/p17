#include <iostream>
#include <gala/sfinae.h>
#include <gala/trace.h>
#include <vector>
#include <set>
#include <list>

// keep forgetting, how this works.
// here's the test case.

using namespace gala;

template<class S, class X=void>
struct something{
	static const std::string what() {return "base case";};
	static int check() {return 0;};
};

template<class S>
struct something<S, typename sfinae::is_set<S>::type >{
	static const std::string what() {return "set specialisation";};
	static int check() {return 1;};
};

template<class S>
struct something<S, typename sfinae::is_vector<S>::type >{
	static const std::string what() {return "vector specialisation";};
	static int check() {return 2;};
};

template<template<class T, typename... > class S, class X=void>
struct something_else{
	static const std::string what() {return "base template case";};
	static int check() {return 0;};
};

template<template<class T, typename... > class S>
struct something_else<S, typename sfinae::is_set_tpl<S>::type >{
	static const std::string what() {return "set template specialisation";};
	static int check() {return 1;};
};

template<template<class T, typename... > class S>
struct something_else<S, typename sfinae::is_vec_tpl<S>::type >{
	static const std::string what() {return "vector template specialisation";};
	static int check() {return 2;};
};


int main(){

	typedef std::set<unsigned> S;

	// prints 1
	std::cerr << std::is_same<
		std::set<typename S::value_type, typename S::key_compare, typename S::allocator_type >, S
		>::value << "\n";

	std::cerr << something< unsigned >::what() << "\n";
	assert(      something< unsigned >::check()==0);

	std::cerr << something< float >::what() << "\n";
	assert(      something< float >::check()==0);

	std::cerr << something< std::set<sfinae::any> >::what() << "\n";
	assert(      something< std::set<sfinae::any> >::check()==1);

	std::cerr << something< std::set<int> >::what() << "\n";
	assert(      something< std::set<int> >::check()==1);

	std::cerr << something< std::set<unsigned> >::what() << "\n";
	assert(      something< std::set<unsigned> >::check()==1);

	std::cerr << something< std::set<double> >::what() << "\n";
	assert(      something< std::set<double> >::check()==1);

	std::cerr << something< std::vector<int> >::what() << "\n";
	assert(      something< std::vector<int> >::check()==2);

	std::cerr << something< std::vector<std::set<int> > >::what() << "\n";
	assert(      something< std::vector<std::set<int> > >::check()==2);


	std::cerr << something_else< std::list >::what() << "\n";
	assert(      something_else< std::list >::check()==0);

	std::cerr << something_else< std::set >::what() << "\n";
	assert(      something_else< std::set >::check()==1);

	std::cerr << something_else< std::vector >::what() << "\n";
	assert(      something_else< std::vector >::check()==2);

}

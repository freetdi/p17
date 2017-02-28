#pragma once

#include <exception>

class exception_invalid : public std::exception {
public:
	exception_invalid(const exception_invalid&) = default;
	exception_invalid(std::string m)
            : _m(m){ }
	const char *what() const throw() { itested();
            return _m.c_str();
	}
private:
	std::string _m;
};

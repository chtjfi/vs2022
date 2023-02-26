#ifndef _SLOPS_EXCEPTION_H_
#define _SLOPS_EXCEPTION_H_

#include <exception>
#include <string>

namespace slOps {
	class Exception : public std::exception {
	public:
		Exception(void) { };
		Exception(const std::string& strWhat) : _strWhat(strWhat) { };
		~Exception(void) { };
		void setWhat(const std::string& strWhat) { _strWhat = strWhat; }
		virtual const char* what() const throw() { return _strWhat.c_str(); }
	private:
		std::string _strWhat;
	};
}

#endif
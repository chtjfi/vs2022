#pragma once

#include <exception>
#include <string>

namespace cyclOps {
	class ExceptionUnableGetCountryFromDN :
	public std::exception {
	public:

		ExceptionUnableGetCountryFromDN(std::string strWhat)
		{
			_strWhat = strWhat;
		}

		~ExceptionUnableGetCountryFromDN(void)
		{
		}
		virtual const char* what() const throw() { return _strWhat.c_str(); }
	private:
		std::string _strWhat;
	};
}
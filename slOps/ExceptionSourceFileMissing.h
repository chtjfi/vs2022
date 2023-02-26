#pragma once
#include <exception>
#include <string>

namespace slOps {
	class ExceptionSourceFileMissing :
		public std::exception
	{
	public:

		ExceptionSourceFileMissing(const std::string& strWhat) : _strWhat(strWhat)
		{
		}

		~ExceptionSourceFileMissing(void)
		{
		}

		virtual const char* what() const throw() { return _strWhat.c_str(); }
	private:
		std::string _strWhat;
	};
}
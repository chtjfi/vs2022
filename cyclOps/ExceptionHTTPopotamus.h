#pragma once

#include <exception>
#include <string>

#include "Exception.h"

namespace cyclOps {
	class ExceptionHTTPopotamus :
		public cyclOps::Exception
	{
	public:

		ExceptionHTTPopotamus(const std::string& strWhat, const std::string& strFile, const std::string& strFunction, int iLine) 
			: cyclOps::Exception(strWhat, strFile, strFunction, iLine)	{	}

		~ExceptionHTTPopotamus(void)
		{
		}
	};
}
#pragma once
#include "exception.h"

namespace cyclOps {
	class ExceptionXMLigator :
		public cyclOps::Exception
	{
	public:

		ExceptionXMLigator
				(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
				: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
				{ } 

		~ExceptionXMLigator(void)
		{
		}
	};
}
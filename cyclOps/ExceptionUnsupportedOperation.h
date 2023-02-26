#pragma once
#include <string>
#include <exception>

#include "Exception.h"

namespace cyclOps {
	class ExceptionUnsupportedOperation :
		public cyclOps::Exception
	{
	public:

		ExceptionUnsupportedOperation (const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine) 
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) { }

		~ExceptionUnsupportedOperation(void)
		{
		}
	};
}
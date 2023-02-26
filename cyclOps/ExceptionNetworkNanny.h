#pragma once
#include "exception.h"

namespace cyclOps {
	class ExceptionNetworkNanny :
		public cyclOps::Exception
	{
	public:
		ExceptionNetworkNanny
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
			{ } 

		~ExceptionNetworkNanny(void)
		{
		}
	};
}
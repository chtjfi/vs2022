#pragma once
#include <string>
#include <exception>

#include "Exception.h"

namespace cyclOps {
	class ExceptionNoSuchNode :
		public cyclOps::Exception
	{
	public:

		ExceptionNoSuchNode (const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine) 
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) { }

		~ExceptionNoSuchNode(void)
		{
		}

	};
}
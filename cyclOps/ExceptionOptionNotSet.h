#pragma once

#include "Exception.h"

namespace cyclOps {
	class ExceptionOptionNotSet :
		public cyclOps::Exception
	{
	public:

		ExceptionOptionNotSet
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
		{ }


		~ExceptionOptionNotSet(void)
		{
		}
	};
}

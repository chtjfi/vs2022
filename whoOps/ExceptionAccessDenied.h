#pragma once
#include "..\cyclops\exception.h"

namespace whoOps {
	class ExceptionAccessDenied :
		public cyclOps::Exception
	{
	public:

		ExceptionAccessDenied
				(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
				: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
		{
		}

		~ExceptionAccessDenied(void)
		{
		}
	};

}
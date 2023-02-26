#pragma once

#include "Exception.h"

namespace cyclOps {

	class ExceptionSMTPecanPie :
		public cyclOps::Exception
	{
	public:

		ExceptionSMTPecanPie
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError) 

		{
		}

		~ExceptionSMTPecanPie(void)
		{
		}
	};

}
#pragma once

#include "..\cyclOps\Exception.h"

namespace cyclOps {
	class ExceptionInvalidCommandLineOptions :
		public cyclOps::Exception
	{
	public:

		ExceptionInvalidCommandLineOptions
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
		{
		}

		~ExceptionInvalidCommandLineOptions(void)
		{
		}
	};

}
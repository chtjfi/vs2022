#pragma once

#include "..\cyclops\exception.h"

namespace whoOps {
	class ExceptionUnableToOpenFile :
		public cyclOps::Exception
	{
	public:

		ExceptionUnableToOpenFile
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
		{
		}

		~ExceptionUnableToOpenFile(void)
		{
		}
	};
}
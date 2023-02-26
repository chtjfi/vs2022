#pragma once
#include "..\cyclops\exception.h"
namespace whoOps {
	class ExceptionUnsupportedSessionType :
		public cyclOps::Exception
	{
	public:

		ExceptionUnsupportedSessionType (const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine) 
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) { }

		~ExceptionUnsupportedSessionType(void)
		{
		}
	};

}
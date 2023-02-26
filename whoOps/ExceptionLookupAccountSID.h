#pragma once
#include "..\cyclops\exception.h"

namespace whoOps {
	class ExceptionLookupAccountSID :
		public cyclOps::Exception
	{
	public:

		ExceptionLookupAccountSID
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0,
			cyclOps::Exception::TYPE type = TYPE_WIN32)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type) 
		{
		}

		~ExceptionLookupAccountSID(void)
		{
		}
	};

}
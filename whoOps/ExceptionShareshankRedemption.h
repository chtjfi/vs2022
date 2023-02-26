#pragma once

#include <exception>
#include <string>

#include "..\cyclOps\Exception.h"

namespace whoOps {
	class ExceptionShareshankRedemption :
		public cyclOps::Exception
	{
	public:
		ExceptionShareshankRedemption
			/*(const string& strMessage, const string& strFile, const string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) */
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0,
			cyclOps::Exception::TYPE type = TYPE_WIN32)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type) 

			{ } 

		~ExceptionShareshankRedemption(void)
		{
		}
	private:
	};

}
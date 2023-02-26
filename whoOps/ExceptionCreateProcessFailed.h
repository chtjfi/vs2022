#pragma once
#include <exception>
#include <string>
using std::string;
namespace whoOps {
	class ExceptionCreateProcessFailed :
		public cyclOps::Exception
	{
	public:

		ExceptionCreateProcessFailed
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0, cyclOps::Exception::TYPE type = cyclOps::Exception::TYPE_UNKNOWN)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type) 
			{ } 

		~ExceptionCreateProcessFailed(void)
		{
		}
	};
}
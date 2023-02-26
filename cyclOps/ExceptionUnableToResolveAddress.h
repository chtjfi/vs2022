#pragma once
#include <string>
#include "Exception.h"
namespace cyclOps {
class ExceptionUnableToResolveAddress :
	public cyclOps::Exception
{
public:

	ExceptionUnableToResolveAddress
		(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0, cyclOps::Exception::TYPE type = cyclOps::Exception::TYPE_UNKNOWN)
		: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type)

	{
	}

	~ExceptionUnableToResolveAddress()
	{
	}
};

}
#pragma once
#include "..\cyclOps\Exception.h"
namespace drOps {
class ExceptionFailedDeployment :
	public cyclOps::Exception
{
public:

	ExceptionFailedDeployment
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0, cyclOps::Exception::TYPE type = cyclOps::Exception::TYPE_UNKNOWN)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type) 

	{
	}

	~ExceptionFailedDeployment()
	{
	}
};

}
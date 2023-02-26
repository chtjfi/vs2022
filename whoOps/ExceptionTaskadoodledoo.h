#pragma once
#include "..\cyclOps\Exception.h"

namespace whoOps {
class ExceptionTaskadoodledoo :
	public cyclOps::Exception
{
public:

	ExceptionTaskadoodledoo
		(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0, cyclOps::Exception::TYPE type = cyclOps::Exception::TYPE_UNKNOWN)
		: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type) 
	{
	}

	~ExceptionTaskadoodledoo()
	{
	}
};

}
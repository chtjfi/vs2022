#pragma once
#include "..\cyclOps\Exception.h"
namespace whoOps {
class ExceptionUnableToCopyFile :
	public cyclOps::Exception
{
public:

	ExceptionUnableToCopyFile
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError) 
	{
	}

	~ExceptionUnableToCopyFile()
	{
	}
};

}
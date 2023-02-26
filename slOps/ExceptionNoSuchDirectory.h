#pragma once
#include "..\cyclops\Exception.h"
namespace slOps {
class ExceptionNoSuchDirectory :
	public cyclOps::Exception
{
public:

	ExceptionNoSuchDirectory(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
	{
	}

	~ExceptionNoSuchDirectory(void)
	{
	}
};

}
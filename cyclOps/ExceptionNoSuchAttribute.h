#pragma once
#include "Exception.h"
namespace cyclOps {
class ExceptionNoSuchAttribute :
	public cyclOps::Exception
{
public:

	ExceptionNoSuchAttribute
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 

	{
	}

	~ExceptionNoSuchAttribute(void)
	{
	}
};

}
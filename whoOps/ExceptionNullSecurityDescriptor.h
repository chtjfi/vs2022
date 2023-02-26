#pragma once
#include "..\cyclops\Exception.h"

namespace whoOps {
	class ExceptionNullSecurityDescriptor :
		public cyclOps::Exception
	{
	public:

		ExceptionNullSecurityDescriptor
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) 
			{ } 

		~ExceptionNullSecurityDescriptor(void)
		{
		}
	};

}
#pragma once
#include "..\cyclops\exception.h"
namespace whoOps {
	class ExceptionNetSession :
		public cyclOps::Exception
	{
	public:

		ExceptionNetSession (const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine) 
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) { }

		~ExceptionNetSession(void)
		{
		}
	};

}
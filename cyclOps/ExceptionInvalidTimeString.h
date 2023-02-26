#pragma once
#include "exception.h"
namespace cyclOps {
	class ExceptionInvalidTimeString :
		public cyclOps::Exception
	{
	public:

		ExceptionInvalidTimeString(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine)
		{
		}

		~ExceptionInvalidTimeString(void)
		{
		}
	};

}
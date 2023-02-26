#pragma once

#include "Exception.h"

namespace cyclOps {
	class ExceptionPathiosocic :
		public cyclOps::Exception
	{
	public:

		ExceptionPathiosocic(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) { }

		~ExceptionPathiosocic(void)
		{
		}
	};
}
#pragma once
#include "..\cyclops\Exception.h"
namespace slOps {
	class ExceptionNoMatchingFiles :
		public cyclOps::Exception
	{
	public:

		ExceptionNoMatchingFiles(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine) 
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine)
		{
		}

		~ExceptionNoMatchingFiles(void)
		{
		}
	};

}
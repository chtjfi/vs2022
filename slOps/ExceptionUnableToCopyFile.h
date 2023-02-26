#pragma once
#include "..\cyclOps\Exception.h"
namespace slOps {
	class ExceptionUnableToCopyFile :
		public cyclOps::Exception
	{
	public:

		ExceptionUnableToCopyFile(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine) { } 

		~ExceptionUnableToCopyFile(void)
		{
		}
	};

}
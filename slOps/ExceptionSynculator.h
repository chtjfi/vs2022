#pragma once
#include "..\cyclops\exception.h"
namespace slOps {
	class ExceptionSynculator :
		public cyclOps::Exception
	{
	public:

		ExceptionSynculator(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine)
		{
		}

		~ExceptionSynculator(void)
		{
		}
	};

}
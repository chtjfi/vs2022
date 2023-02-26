#pragma once

#include "Exception.h"

namespace slOps {
	class ExceptionInvalidArguments :
		public slOps::Exception
	{
	public:

		ExceptionInvalidArguments(const std::string& strWhat) { this->setWhat(strWhat); }

		~ExceptionInvalidArguments(void)
		{
		}
	};

}
#pragma once

#include <exception>

namespace cyclOps {
	class ExceptionNoActiveEntries :
		public std::exception
	{
	public:
		ExceptionNoActiveEntries(void) { };
		~ExceptionNoActiveEntries(void) { };
	};
}
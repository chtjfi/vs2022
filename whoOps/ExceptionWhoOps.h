#pragma once

#include <exception>
#include "..\cyclOps\cyclOps.h"

namespace whoOps {
	class ExceptionWhoOps :	public std::exception {
		public:
			enum Code {
				UNSPECIFIED,
				UNABLE_TO_GET_SERVICE_HANDLE
			};
			ExceptionWhoOps(void);
			~ExceptionWhoOps(void);
			// virtual const char* what() const throw() { return m_szMessage; }
			// void setWhat(char* szFormat, ...);
			static void throwMe(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
			static void throwMeII(Code iCode, char* szFile, char* szFunction, int iLine, char* szFormat, ...);
			void setWhat(char* szFormat, ...);
			virtual const char* what() const throw() { return m_szMessage; }
			Code getCode() { return _code; }
			void setCode(Code code) { _code = code; }
		private:
			char m_szMessage[5000];
			Code _code;

	};
}


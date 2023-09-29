#include "StdAfx.h"
#include "ExceptionWhoOps.h"


whoOps::ExceptionWhoOps::ExceptionWhoOps(void)
{
}


whoOps::ExceptionWhoOps::~ExceptionWhoOps(void)
{
}

void whoOps::ExceptionWhoOps::setWhat(const char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	_vsnprintf_s(m_szMessage, CYCLOPSSIZEOF(m_szMessage), _TRUNCATE, szFormat, vaArguments);
}

void whoOps::ExceptionWhoOps::throwMe(const char* szFile, const char* szFunction, int iLine, const char* szFormat, ...) {
	char szMessage[5000];
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	_vsnprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE, szFormat, vaArguments);
	whoOps::ExceptionWhoOps::throwMeII(UNSPECIFIED, szFile, szFunction, iLine, szMessage);
}

void whoOps::ExceptionWhoOps::throwMeII(Code code, const char* szFile, const char* szFunction, int iLine, const char* szFormat, ...) {
	char szMessage[5000];
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	_vsnprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE, szFormat, vaArguments);
	whoOps::ExceptionWhoOps e;
	e.setWhat("%s [%s %s(%d)]", szMessage, szFile, szFunction, iLine);
	e.setCode(code);
	throw e;
}

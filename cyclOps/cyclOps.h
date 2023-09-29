#ifndef __CYCLOPS_H__
#define __CYCLOPS_H__

/*

WinSock2.h must come before Windows.h

From http://stackoverflow.com/questions/11726958/cant-include-winsock2-h-in-msvc-2010

To elaborate on this, winsock2.h is meant to be a replacement for winsock.h, they are not meant to be used together. 
If winsock.h is included first, winsock2.h fails to compile because it redeclares everything that winsock.h already declared. 
If winsock2.h is included first, it disables winsock.h for you. windows.h includes winsock2.h if _WIN32_WINNT >= 0x0400, 
otherwise it includes winsock.h instead.

*/

#include <WinSock2.h>

#include <Windows.h>
#include <stdio.h>
#include <string>

#include "ExceptionInvalidCommandLineOptions.h"
#include "ExceptionOptionNotSet.h"
#include "ExceptionInvalidTimeString.h"
#include "ExceptionNoSuchNode.h"
#include "ExceptionUnsupportedOperation.h"
#include "ExceptionPathiosocic.h"
#include "ExceptionSMTPecanPie.h"
#include "ExceptionXMLigator.h"
#include "ExceptionUnableToResolveAddress.h"
#include "ExceptionNoSuchAttribute.h"
#include "ExceptionNoSuchEntry.h"
#include "ExceptionHTTPopotamus.h"
#include "ExceptionFunctionNotImplemented.h"
#include "ExceptionUnableToOpenFileII.h"
#include "ExceptionHTTPStatusCodeNotSuccess.h"
#include "ExceptionInvalidArgumentsII.h"

#include "Logger.h"
#include "Pathiosocic.h"
#include "StringEmUp.h"
#include "XMLigator.h"
#include "ODBCulator.h"
#include "ODBCResultSet.h"
#include "ODBCColumn.h"
#include "RegExtravaganza.h"
#include "TimeTiger.h"
#include "NetworkNanny.h"
#include "SMTPecanPie.h"
#include "JNIRequest.h"
#include "JNIResponse.h"
#include "CommandLinebacker.h"
#include "Strung.h"
#include "HTTPopotamus.h"
#include "Automail.h"
#include "Encryptigator.h"
#include "LDAPadillo.h"
#include "STLadocious.h"

#define SMALLBUF 100
#define BIGBUF 1000
#define HUGEBUF 10000
#define MONSTERBUF 100000

/* Found on Stackoverflow but can't refind it!  Can't give credit. :( 
   These all get defined as class members.
/* enumToString() will be defined in the containing class or namespace.  So use it
   like this objectContainingEnum.enumToString(ENUMBY); */
#define CYCLOPS_ENUM(name, ...)															\
	enum name { __VA_ARGS__ };															\
	std::vector<std::string> name##Map = cyclOps::StringEmUp::split(#__VA_ARGS__, ',');	\
    std::string enumToString(const name v) { std::string s = name##Map.at(v); cyclOps::StringEmUp::trim(s); return s; }




#define CYCLOPSSIZEOF(sz) (sizeof(sz) / sizeof(sz[0])) - 1
#define CYCLOPSSIZEOFII(sz) (sizeof(sz) / sizeof(sz[0])) 
#define CYCLOPSDEBUGENABLE() \
	g_boCyclOpsDebugPrevious = g_boCyclOpsDebug; \
	::cyclOpsLogToConsoleQND("DEBUG", __FILE__, __FUNCTION__, __LINE__, "Enabling debugging."); \
	g_boCyclOpsDebug = true;

#define CYCLOPSDEBUG_II(logger, ...) \
	if (g_boCyclOpsDebug) \
				logger.debug(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CYCLOPSDEBUG(...)	\
	if (g_boCyclOpsDebug) \
		{	::cyclOpsLogToConsoleQND("DEBUG", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }
#define CYCLOPSDEBUGW(...)	if (g_boCyclOpsDebug) {	::cyclOpsLogToConsoleQNDW(L"DEBUG", \
	__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }
#define CYCLOPSHELLO() if (g_boCyclOpsDebug) { \
	::cyclOpsLogToConsoleQND("DEBUG", __FILE__, __FUNCTION__, __LINE__, "Hello."); \
}
#define CYCLOPSGOODBYE() if (g_boCyclOpsDebug) { \
	::cyclOpsLogToConsoleQND("DEBUG", __FILE__, __FUNCTION__, __LINE__, "Goodbye."); \
}

#define CYCLOPSERROR_II(logger, ...) \
				logger.error(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CYCLOPSERROR(...)   \
				::cyclOpsLogToConsoleQND("ERROR", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CYCLOPSFATAL(...)   \
				::cyclOpsLogToConsoleQND("FATAL", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CYCLOPSWARNING(...) \
				::cyclOpsLogToConsoleQND("WARNING", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CYCLOPSINFO(...)    \
			::cyclOpsLogToConsoleQND("INFO", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CYCLOPSVAR(var, format)			\
	if (g_boCyclOpsDebug) {	\
		::cyclOpsLogToConsoleQND("DEBUG", __FILE__, __FUNCTION__, __LINE__, \
			"%s = " format, #var, var); \
	}

/* #define CYCLOPS_THROW_EXCEPTION(...)   cyclOps::ExceptionCyclOps::throwMe(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); */


#define CYCLOPS_THROW_EXCEPTION_III(classs, what) \
	throw classs(what, __FILE__, __FUNCTION__, __LINE__)
#define CYCLOPS_THROW_EXCEPTION_IV(classs, ...) \
	std::string strWhat = cyclOps::StringEmUp::format(__VA_ARGS__); \
	throw classs(strWhat, __FILE__, __FUNCTION__, __LINE__)

#define CYCLOPS_THROW_EXCEPTION_V(classs, dwLastError, type, ...) \
	std::string strWhat = cyclOps::StringEmUp::format(__VA_ARGS__); \
	throw classs(strWhat, __FILE__, __FUNCTION__, __LINE__, dwLastError, type)

#define CYCLOPS_THROW_EXCEPTION_VI(classs, cause, ...) \
	std::string strWhat = cyclOps::StringEmUp::format(__VA_ARGS__); \
	throw classs(strWhat, __FILE__, __FUNCTION__, __LINE__, lCauseCode)

#define CYCLOPS_THROW_EXCEPTION_NOT_IMPLEMENTED() \
	throw cyclOps::ExceptionFunctionNotImplemented("Function not implemented.", __FILE__, __FUNCTION__, __LINE__);

#define CYCLOPS_SNPRINTF_S(szTarget, szFormat, ...) \
	_snprintf_s(szTarget, sizeof(szTarget) / sizeof(szTarget[0]), _TRUNCATE, szFormat, __VA_ARGS__);

#define CYCLOPS_SNWPRINTF_S(wszTarget, wszFormat, ...) \
	_snwprintf_s(wszTarget, sizeof(wszTarget) / sizeof(wszTarget[0]), _TRUNCATE, wszFormat, __VA_ARGS__);


extern bool g_boCyclOpsDebug;
extern bool g_boCyclOpsDebugPrevious;
extern char* g_szCyclOpsDebugFile;
extern char* g_szCyclOpsDebugFunction;


void			cyclOpsLogToConsoleQND(const char* szLevel, const char* szFile, const char* szFunction, int iLine, const char* szFormat, ...);
void			cyclOpsLogToConsoleQNDW(const wchar_t* wszLevel, const char* szFile, const char* szFunction, int iLine, const wchar_t* wszFormat, ...);
std::string		cyclOpsGetCommandLineOptionII(int argc, char* argv[], char* pchOption);
bool			cyclOpsGetCommandLineOption(int argc, char* argv[], char* pchOption, char* pchValue, int iValueSize);
bool			cyclOpsGetCmdlineOptionClassic(char szOption[], char** ppchValue);
char *			cyclOpsReplace(char const * const original,  char const * const pattern,  char const * const replacement);
void			cyclOpsUnixTimeToISO(time_t t, char* szTime, DWORD dwBufSize);
void			cyclOpsGetISOTime(char* szISOTime, int iSize);
BOOL			cyclOpsGetISOTime2(char* szISOTime, int iSize, char chTimeDelimiter = '.');
std::string		cyclOpsGetISOTimeIII();
BOOL			cyclOpsSendAutomail(char szAutomailShare[], char szRecipients[], char szSubject[], char szAutomailMessage[], char szAttachment[], BOOL boVerbose, char szOriginatingProgram[]);
std::string		cyclOpsGetCommandLineOptionIII(int argc, char* argv[], char* pchOption);

#endif
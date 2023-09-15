#ifndef __CROPS_PRIVATE_H__
#define __CROPS_PRIVATE_H__

//#include <xercesc\dom\DOM.hpp>
#include <stdio.h>
#include <string>
#include "..\cyclOps\Logger.h"

extern cyclOps::Logger g_crOpsLogger;


#define CROPSDEBUG(...)		if (g_boCrOpsDebug) { g_crOpsLogger.debug(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }
#define CROPSERROR(...)		g_crOpsLogger.error(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__) 
/* Having CROPSFATAL log an ERROR is easy at error collection time. */
#define CROPSFATAL(...)		g_crOpsLogger.error(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__) 
#define CROPSWARNING(...)	g_crOpsLogger.warn(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CROPSINFO(...)		g_crOpsLogger.info(__VA_ARGS__); 
#define CROPSVARII(var, format)		if (g_boCrOpsDebug) { g_crOpsLogger.debug(__FILE__, __FUNCTION__, __LINE__, "%s == " format , #var, var); }
/*
	{ printf(cyclOps::TimeTiger::getTimeString().c_str()); \
	printf(" DEBUG: %s %s() line %d - ", __FILE__, __FUNCTION__, __LINE__); \
	printf("%s == " format "\n", #var, var); }
*/
#define CROPSSIZEOF(sz)		(sizeof(sz) / sizeof(sz[0])) - 1
#define CROPS_THROW_FATAL(...)   crOpsThrowFatalException(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define CROPS_THROW_NON_FATAL(...)   crOpsThrowNONFatalException(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

unsigned int crOpsPushApplications(const char *szCrOpsConfigFile, const char* szPushTarget, bool boDebug, 
	bool boListOnly, const char* szApplication, std::string* pstrResultXML, 
	std::string strCrOpsLog);
void crOpsThrowFatalException(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
void crOpsThrowNONFatalException(char* szFile, char* szFunction, int iLine, char* szFormat, ...);

extern bool g_boCrOpsDebug;

#endif
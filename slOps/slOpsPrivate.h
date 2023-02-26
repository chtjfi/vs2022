#ifndef __SLOPSPRIVATE_H__
#define __SLOPSPRIVATE_H__

#include "..\cyclOps\Logger.h"

#include "ExceptionInvalidArguments.h"
#include "ExceptionNoMatchingFiles.h"
#include "ExceptionUnableToCopyFile.h"
#include "ExceptionSynculator.h"
#include "ExceptionNoSuchDirectory.h"

#define SLOPSDEBUG CYCLOPSDEBUG
	/* if (g_boSlOpsDebug) { g_slOpsLogger.debug(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }  */
#define SLOPSINFO CYCLOPSINFO 
	/* (...)		g_slOpsLogger.info(__VA_ARGS__); */
#define SLOPSERROR CYCLOPSERROR
	/*(...)		g_slOpsLogger.error(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__) */
#define SLOPSSIZEOF(sz) sizeof(sz) / sizeof(sz[0])

#define	SLOPS_MAX_PATH	30000

extern bool g_boSlOpsDebug;
extern cyclOps::Logger g_slOpsLogger;

bool slOpsCreateDirectory(const char *szDirectory);
bool slOpsPurgeDirectory(const char *szTargetDir, const char *szSourceDir);
bool slOpsCopyFile(const char *szTargetFile, const char *szSourceFile);
bool slOpsRemoveDirectory(const char* szDirectory, bool boDeleteReadOnly);
bool slOpsCopyDirectory(const char *szSourceDir, const char *szTargetDir, bool boIncludeSubs, bool boPurge);

#endif
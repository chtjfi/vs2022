#ifndef __SLOPS_H__
#define __SLOPS_H__

extern "C" __declspec(dllexport) bool dllexportSlOpsCopyDirectory(
	const char *szSourceDir, 
	const char *szTargetDir, 
	bool boIncludeSubs, 
	bool boPurge, 
	bool boDebug, 
	const char* szLogFile,
	bool boContinueOnCopyFailure = false);

#endif
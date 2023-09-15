#ifndef __CROPS_H__
#define __CROPS_H__

extern "C" __declspec(dllexport) unsigned int crOpsPushApplications(
	const char *szCrOpsConfigFile, 
	const char* szPushTarget, 
	bool boDebug, 
	bool boListOnly, 
	const char* szApplication);

#endif
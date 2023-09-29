// This file only for external users of whoOps, NO PRIVATE FUNCTIONS!
#ifndef _WHOOPS_H_
#define _WHOOPS_H_

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include <Windows.h> 
#include <lm.h>
#include <ole2.h>

typedef enum {
	/* When you add a new code be sure to update dllexportWhoOpsGetMessageForResult(). */
	WHOOPS_NO_STATUS,
	WHOOPS_SUCCESS,
	WHOOPS_ERROR_UNSPECIFIED,
	WHOOPS_EXCEPTION_STD,
	WHOOPS_EXCEPTION_UNKNOWN,
	WHOOPS_ERROR_NOT_IMPLEMENTED
	/* When you add a new code be sure to update dllexportWhoOpsGetMessageForResult(). */
} WHOOPS_RESULT;

typedef enum {
	WHOOPS_CLIENT,
	WHOOPS_USER
} WHOOPS_SESSION_TYPE;

extern "C" __declspec(dllexport) void whoOpsSetDebug(bool boDebug);
extern "C" __declspec(dllexport) void whoOpsRegistryGetREG_SZ(LPCSTR szServer, LPCSTR szKey, LPCSTR szValue, char* szReturn, int iSize);
extern "C" __declspec(dllexport) void whoOpsServiceQueryServiceStatus(LPCWSTR wszServer, LPCWSTR wszService, char* szStatus, size_t iSize);
extern "C" __declspec(dllexport) void whoOpsGetFileVersionInfo(const char* szFile, char* szVersion, size_t iSize);
extern "C" __declspec(dllexport) boolean dllexportWhoOpsNetLocalGroupAddMembers(const char* szHost, const char* szLocalGroup, const char* szMember);
extern "C" __declspec(dllexport) boolean whoOpsNetLocalGroupAddMembers(const wchar_t* wszHost, const wchar_t* wszLocalGroup, const wchar_t* wszMember);
extern "C" __declspec(dllexport) NET_API_STATUS whoOpsNetLocalGroupAddMembersII(const wchar_t* wszHost, const wchar_t* wszLocalGroup, const wchar_t* wszMember);
extern "C" __declspec(dllexport) boolean whoOpsNetLocalGroupDelMembers(wchar_t* wszHost, wchar_t* wszLocalGroup, wchar_t* wszMember);
extern "C" __declspec(dllexport) boolean dllexportWhoOpsNetLocalGroupDelMembers(const char* szHost, const char* szLocalGroup, const char* szMember);
extern "C" __declspec(dllexport) boolean whoOpsServiceRestart(LPCWSTR wszServer, LPCWSTR wszService, int iSeconds);
extern "C" __declspec(dllexport) boolean whoOpsOSGetVersion(OSVERSIONINFO* pOSVersionInfo);
extern "C" __declspec(dllexport) boolean whoOpsNetFileEnum(LPWSTR wszHost, void (*callbackFunction)(FILE_INFO_3* pFI3, LPWSTR wszHost));
extern "C" __declspec(dllexport) boolean whoOpsNetLocalGroupGetMembers(const wchar_t* wszHost, const wchar_t* wszGroup, void (*callbackFunction)(LOCALGROUP_MEMBERS_INFO_3* pLMI3, LPWSTR wszHost, LPWSTR wszGroup));
extern "C" __declspec(dllexport) boolean whoOpsNetFileClose(wchar_t* wszHost, DWORD dwFileID);
extern "C" __declspec(dllexport) UINT __stdcall whoOpsTaskNewWorkItem(	LPCWSTR wszServer,	LPCWSTR wszTaskName,	LPCWSTR wszExecutable,	LPCWSTR wszParameters,	LPCWSTR wszUser,	LPCWSTR wszPassword);
extern "C" __declspec(dllexport) boolean dllexportWhoOpsTaskRun(char* szServer, char* szTask);
extern "C" __declspec(dllexport) boolean dllexportWhoOpsTaskGetExitCode(char* szServer, char* szTask, DWORD* pdwExitCode);
extern "C" __declspec(dllexport) const char* dllexportWhoOpsTaskGetStatus(char* szServer, char* szTask);
extern "C" __declspec(dllexport) int dllexportWhoOpsACLAddACEToDACL(char* szPath, char* szSecurityPrinciple, char*szPermission);
extern "C" __declspec(dllexport) const char* dllexportWhoOpsErrorFormatMessage(DWORD dwError);
extern "C" __declspec(dllexport) boolean dllexportWhoOpsServiceRestart(LPCWSTR wszServer, LPCWSTR wszService, int iSeconds);
extern "C" __declspec(dllexport) const char* dllexportWhoOpsTaskTerminate(char* szServer, char* szTask);
extern "C" __declspec(dllexport) int dllexportWhoOpsRegSetValueEx(char* szServer, char* szKey, char* szValue, char* szData);
extern "C" __declspec(dllexport) int dllexportWhoOpsShutdownReboot(char* szServer);
extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsServiceStop(const char* szServer, const char* szService, bool boWait, int iTimeout, char* szMessage, int iSize);
extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsServiceStart(const char* szServer, const char* szService, bool boWait, int iTimeout, char* szMessage, int iSize);
extern "C" __declspec(dllexport) const char* dllexportWhoOpsGetMessageForResult(WHOOPS_RESULT);
extern "C" __declspec(dllexport) void dllexportWhoOpsSetDebug(bool boDebug);
extern "C" __declspec(dllexport) void dllexportWhoOpsServiceQueryServiceStatus(const char* szServer, const char* szService, char* szStatus, size_t iSize);
extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsServiceGetBinaryPathName(const char* szServer, const char* szService, char* szBinaryPathName, size_t iSize); 
extern "C" __declspec(dllexport) bool dllexportWhoOpsServiceIsServiceStopped(const char* szServer, const char* szService);
extern "C" __declspec(dllexport) bool dllexportWhoOpsRegOpenKeyEx(const char * szServer, const char * szKey);
extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsCreateProcessQnD(const char* szCommandLine);
extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsNetSessionDel(const char* szHost, const char* szTarget, const WHOOPS_SESSION_TYPE sessionType);
extern "C" __declspec(dllexport) void dllexportNetLocalGroupGetMembers(const wchar_t* wszHost, const wchar_t* wszGroup);

#endif
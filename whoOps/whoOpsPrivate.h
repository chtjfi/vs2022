#ifndef _WHOOPS_PRIVATE_H_
#define _WHOOPS_PRIVATE_H_

#include <mstask.h>
#include <string>
#include "whoOps.h"


#define WHOOPS_THROW_EXCEPTION(...)   whoOps::ExceptionWhoOps::throwMe(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define WHOOPS_THROW_EXCEPTION_II(code, ...)   whoOps::ExceptionWhoOps::throwMeII(code, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

enum whoOpsWhatToDo { RUN_TASK, GET_LAST_RUN_TIME, GET_EVERYTHING, STOP_TASK, SCHEDULE_TASK };

using std::string;

extern bool g_boCyclOpsDebug;

ITaskScheduler* whoOpsTaskGetTaskScheduler(LPCWSTR wszServer);
ITask* whoOpsTaskActivate(ITaskScheduler* pScheduler, LPCWSTR wszTaskName);
void whoOpsServiceStop(LPCWSTR wszServer, LPCWSTR wszService);
void whoOpsServiceStart(LPCWSTR wszServer, LPCWSTR wszService);
void whoOpsServiceWaitUntil(LPCWSTR wszServer, LPCWSTR wszService, int iSeconds,
							DWORD dwStatue);
void  whoOpsServiceWaitUntilII(const std::wstring& wstrServer, const std::wstring& wstrService, int iSeconds, DWORD dwState);
SC_HANDLE whoOpsServiceGetHandle(LPCWSTR wszServer, LPCWSTR wszService);
void whoOpsPrivateTaskNewWorkItem(
	LPCWSTR wszServer,
	LPCWSTR wszTaskName,
	LPCWSTR wszExecutable,
	LPCWSTR wszParameters,
	LPCWSTR wszUser,
	LPCWSTR wszPassword);

void whoOpsTaskRun(
	LPCWSTR wszServer,
	LPCWSTR wszTaskName);

void whoOpsTaskTerminate(
	LPCWSTR wszServer,
	LPCWSTR wszTaskName);

void whoOpsTaskGetExitCode(
	LPCWSTR wszServer,
	LPCWSTR wszTaskName,
	DWORD* pdwExitCode);

void whoOpsTaskGetExitCode(	LPCWSTR wszServer,	LPCWSTR wszTaskName,	DWORD* pdwExitCode,
	HRESULT* hResult);

ITask* whoOpsTaskGetTask(LPCWSTR wszServer, LPCWSTR wszTaskName);
const char* whoOpsTaskGetStatus(LPCWSTR wszServer,	LPCWSTR wszTaskName);
const char* whoOpsTaskConvertStatusToString(HRESULT lStatus);
const char* whoOpsErrorFormatMessage(DWORD hResult);
void whoOpsTaskTerminate(char* szServer, char* szTaskName);
const char* whoOpsErrorGetLastMessage(void);

typedef enum {
	WHOOPS_START,
	WHOOPS_STOP
} WHOOPS_SERVICE_ACTION;

WHOOPS_RESULT whoOpsServiceStopOrStart(WHOOPS_SERVICE_ACTION action, const char* szServer, const char* szService, bool boWait, int iTimeout, char* szResult, int iSize); 

string whoOpsServiceGetBinaryPathName(const char* szServer, const char* szService); 

SC_HANDLE whoOpsServiceGetHandle(const char* szServer, const char* szService);
QUERY_SERVICE_CONFIG* whoOpsServiceQueryServiceConfig(const char* szServer, const char* szService);
void whoOpsServiceQueryServiceStatusII(LPCWSTR wszServer, LPCWSTR wszService, SERVICE_STATUS* pServiceStatus);
DWORD whoOpsServiceGetCurrentState(const wchar_t* szServer, const wchar_t* szService);
void whoOpsServiceStopStopStopStopStop(const wchar_t* wszServer, const wchar_t* wszService);
bool whoOpsServiceIsServiceStopped(const wchar_t* wszServer, const wchar_t* wszService);
void whoOpsAddBackslashesToServerIfNecessary(wchar_t* wszServerWithBackslashes, size_t iSize, const wchar_t* wszServerOriginal);
#endif
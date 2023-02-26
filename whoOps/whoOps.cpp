// whoOps.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

/*

WinSock2.h must come before Windows.h

From http://stackoverflow.com/questions/11726958/cant-include-winsock2-h-in-msvc-2010

To elaborate on this, winsock2.h is meant to be a replacement for winsock.h, they are not meant to be used together. If winsock.h is included first, winsock2.h fails to compile because it redeclares everything that winsock.h already declared. If winsock2.h is included first, it disables winsock.h for you. windows.h includes winsock2.h if _WIN32_WINNT >= 0x0400, otherwise it includes winsock.h instead.

*/

#include <WinSock2.h>

#include <sstream>
#include <windows.h>
#include <initguid.h>
#include <ole2.h>
#include <mstask.h>
#include <msterr.h>
#include <objidl.h>
#include <wchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <lm.h>
#include "whoOpsPrivate.h"
#include "whoOps.h"
#include "jni.h"
#include "whoOps_TaskScheduler.h"
#include "whoOps_ServiceMangler.h"
#include "whoOps_RegistryRaptor.h"
#include "whoOps_FileVersionFlunky.h"
#include "whoOps_ProcessOpotamus.h"
#include "whoOps_ShareshankRedemption.h"
#include "FileVersionInfo.h"
#include "ExceptionGeneric.h"
#include "ACLAmigo.h"
#include "../cyclOps/cyclOps.h"
// ProcessPrincess.cpp/h have been moved to whoOps.
#include "ProcessPrincess.h"
#include "../cyclOps/JNIJunkie.h"
#include "../cyclOps/TimeTiger.h"
#include "../cyclOps/JNIRequest.h"
#include "../cyclOps/JNIResponse.h"
#include "ExceptionWhoOps.h"
#include "RegistryRhinoceros.h"
#include <sstream>
#include "ShutdownShark.h"
#include "whoOps_ShutdownShark.h"
#include <algorithm>
#include "RegistryRhinoceros.h"
#include "ShareshankRedemption.h"
#include "JNIResponseShareshankRedemption.h"
#include "SessionSuckah.h"
#include "ExceptionServiceManagement.h"

#define WHOOPSLOG(...)      printf(__VA_ARGS__); printf(" [%s line %d]\n", __FILE__, __LINE__)

using std::wstring;
using std::stringstream;

char* whoOpsErrorFormatMessage(DWORD hResult)
{
   char* szMessage = NULL;
   char* szReturn = NULL;
   if (::FormatMessageA(
	   FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
	   NULL, 
	   hResult, 
	   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
	   reinterpret_cast<char*>(&szMessage), 
	   0, NULL) && (szMessage != NULL))
   {
	   //remove newline from message
	   szReturn = cyclOpsReplace(szMessage, "\r\n", "");
   }
   else
   {
      szReturn = "unknown error";
   }

   return szReturn;
}
void whoOpsTaskDelete(LPCWSTR wszServerOriginal, LPCWSTR wszTaskName) {

	wchar_t wszServerWithBackslashes[1000];
	::whoOpsAddBackslashesToServerIfNecessary(wszServerWithBackslashes, CYCLOPSSIZEOF(wszServerWithBackslashes), wszServerOriginal);
	WHOOPSLOG("DEBUG: %s%s", "whoOpsNewWorkItem", "()");
	ITaskScheduler *pITS;
	try {
		pITS = whoOpsTaskGetTaskScheduler(wszServerWithBackslashes);
	} catch (int iException) {
		WHOOPSLOG("ERROR: exception %d thrown by whoOpsGetTaskScheduler()", iException);
		WHOOPS_THROW_EXCEPTION("ERROR: exception %d thrown by whoOpsGetTaskScheduler()", iException);
	} 
/*
	LPWSTR pwszTemp;
	HRESULT hr = pITS->GetTargetComputer(&pwszTemp);
	if (FAILED(hr)) {
		CoUninitialize();
		WHOOPS_THROW_EXCEPTION("GetTargetComputer(%S) failed.", pwszTemp);
	}

  WHOOPSLOG("DEBUG: target computer is %S", pwszTemp);
*/
  HRESULT hr = pITS->Delete(wszTaskName);
  if (FAILED(hr)) {
	  printf("Last message: %s\n", ::whoOpsErrorGetLastMessage());
	  WHOOPSLOG("WARNING: Delete() returned error 0x%x", hr);
	  throw 25;
  }
}
void whoOpsPrivateTaskNewWorkItem(	LPCWSTR wszServer,	LPCWSTR wszTaskName,	LPCWSTR wszExecutable,	LPCWSTR wszParameters,	LPCWSTR wszUser,	LPCWSTR wszPassword)
{

	WHOOPSLOG("DEBUG: %s%s", "whoOpsPrivateNewWorkItem", "()");
	ITaskScheduler *pITS;
	try {
		pITS = whoOpsTaskGetTaskScheduler(wszServer);
	} catch (int iException) {
		WHOOPSLOG("ERROR: exception %d thrown by whoOpsGetTaskScheduler()", iException);
		WHOOPS_THROW_EXCEPTION("ERROR: exception %d thrown by whoOpsGetTaskScheduler()", iException);
	}
	LPWSTR pwszTemp;
	HRESULT hr = pITS->GetTargetComputer(&pwszTemp);
	if (FAILED(hr)) {
		CoUninitialize();
		WHOOPSLOG("ERROR: Failed calling GetTargetComputer(), error = 0x%x", hr);
		throw 2;
	}

  WHOOPSLOG("DEBUG: target computer is %S", pwszTemp);

  /////////////////////////////////////////////////////////////////
  // Delete any task with the same name as the one we are creating
  /////////////////////////////////////////////////////////////////

  hr = pITS->Delete(wszTaskName);
  if (FAILED(hr)) {
	  WHOOPSLOG("WARNING: Delete() returned error 0x%x when trying to delete task before creating it.  This is usually harmless.", hr);
  }
  /////////////////////////////////////////////////////////////////
  // Call ITaskScheduler::NewWorkItem to create new task.
  /////////////////////////////////////////////////////////////////
  ITask *pITask;
  IPersistFile *pIPersistFile;
  hr = pITS->NewWorkItem(wszTaskName,         // Name of task
                         CLSID_CTask,          // Class identifier 
                         IID_ITask,            // Interface identifier
                         (IUnknown**)&pITask); // Address of task 
  pITS->Release();                               // Release object
  if (FAILED(hr))
  {
     CoUninitialize();
	 char* szError = whoOpsErrorFormatMessage(hr);
	 WHOOPSLOG("ERROR: Failed calling NewWorkItem, error = %s", szError);
     throw 3;
  }
  pITask->SetApplicationName(wszExecutable);
  pITask->SetParameters(wszParameters);
  pITask->SetAccountInformation(wszUser, wszPassword);
  /////////////////////////////////////////////////////////////////
  // Call IUnknown::QueryInterface to get a pointer to 
  // IPersistFile and IPersistFile::Save to save 
  // the new task to disk.
  /////////////////////////////////////////////////////////////////
  hr = pITask->QueryInterface(IID_IPersistFile,
                              (void **)&pIPersistFile);
  
  pITask->Release();
  if (FAILED(hr))
  {
     CoUninitialize();
	 WHOOPSLOG("ERROR: Failed calling QueryInterface, error = 0x%x",hr);
     throw 4;
  }
  
  
  hr = pIPersistFile->Save(NULL, TRUE);
  pIPersistFile->Release();
  if (FAILED(hr))
  {
     CoUninitialize();
	 WHOOPSLOG("ERROR: Failed calling Save, error = 0x%x",hr);
     throw 5;
  }
  
  
  CoUninitialize();
  WHOOPSLOG("INFO: Created task.");
}

extern "C" __declspec(dllexport) char* dllexportWhoOpsTaskGetStatus(char* szServer, char* szTask) {
	CYCLOPSVAR(szServer, "%s");	CYCLOPSVAR(szTask, "%s");
	wchar_t wszServer[1000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszTask[1000];
	_snwprintf_s(wszTask, CYCLOPSSIZEOF(wszTask), _TRUNCATE, L"%S", szTask);
	CYCLOPSVAR(wszServer, "%S"); CYCLOPSVAR(wszTask, "%S");
	try {
		char* szStatus = ::whoOpsTaskGetStatus(wszServer, wszTask);
		return szStatus;
	} catch (int iException) {
		CYCLOPSERROR("Exception thrown by ::whoOpsTaskGetStatus(). %d", iException);
		return "false";
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		return "false";
	}
}

char* whoOpsTaskGetStatus(LPCWSTR wszServer,	LPCWSTR wszTaskName) {
	ITask *pITask = whoOpsTaskGetTask(wszServer, wszTaskName);
	HRESULT lStatus;
	HRESULT hr = pITask->GetStatus(&lStatus);
	if (FAILED(hr)) {
		char* szError = ::whoOpsErrorFormatMessage(hr);
		CYCLOPSERROR("GetStatus failed [%s]", szError);
		throw 42;
	}
	CYCLOPSVAR(lStatus, "%d");
	char* szStatus = whoOpsTaskConvertStatusToString(lStatus);
	return szStatus;
}

void whoOpsTaskGetMostRecentRunTime(LPCWSTR wszServer,	LPCWSTR wszTaskName, char* szISO8601, int iLength) {
	ITask *pITask = whoOpsTaskGetTask(wszServer, wszTaskName);
	SYSTEMTIME systimeMostRecentRunTime;
	HRESULT hr = pITask->GetMostRecentRunTime(&systimeMostRecentRunTime);
	if (FAILED(hr)) {
		char* szError = ::whoOpsErrorFormatMessage(hr);
		CYCLOPSERROR("GetMostRecentRunTime() failed [%s]", szError);
		throw 73;
	}
	cyclOps::TimeTiger tiger;
	tiger.systemtimeToISO8601(systimeMostRecentRunTime, szISO8601, iLength);
}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_getExitCodeNative(JNIEnv*  env, jobject obj, jstring input) {
	char szReturn[5000];
	try {
		cyclOps::JNIJunkie jniJunkie(env, obj, input);
		// This second dimension must match the declaration of getArgumentArray(). See http://www.cplusplus.com/forum/general/11627/
		const int iNumberOfArguments = 2;
		wchar_t wszArgumentArray[iNumberOfArguments][5000];
		jniJunkie.getArgumentArray(wszArgumentArray, iNumberOfArguments);
		DWORD iExitCode;
		HRESULT hResult;
		::whoOpsTaskGetExitCode(wszArgumentArray[0], wszArgumentArray[1], &iExitCode, &hResult);
		// Format of szReturn is like this:
		// 0;EXIT_CODE_CONSTANT;Friendly message if there is one.;0;HRESULT_CONSTANT;Friendly message if there is one.
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, 
			"%d; This is the exit code message.;xxx;HRESULT message.", iExitCode);
	} catch (...) {
		CYCLOPSERROR("Exception caught.");
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "ERROR: Exception occurred.");
	}
	return env->NewStringUTF(szReturn);
}


JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_getMostRecentRunTimeNative(JNIEnv*  env, jobject obj, jstring input) {
	char szReturn[5000];
	try {
		cyclOps::JNIJunkie jniJunkie(env, obj, input);
		// This second dimension must match the declaration of getArgumentArray(). See http://www.cplusplus.com/forum/general/11627/
		const int iNumberOfArguments = 2;
		wchar_t wszArgumentArray[iNumberOfArguments][5000];
		jniJunkie.getArgumentArray(wszArgumentArray, iNumberOfArguments);
		::whoOpsTaskGetMostRecentRunTime(wszArgumentArray[0], wszArgumentArray[1], szReturn, CYCLOPSSIZEOF(szReturn));
	} catch (...) {
		CYCLOPSERROR("Exception caught.");
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "ERROR: Exception occurred.");
	}
	return env->NewStringUTF(szReturn);
}


void whoOpsTaskRun(	LPCWSTR wszServer,	LPCWSTR wszTaskName)
{
	ITask *pITask = whoOpsTaskGetTask(wszServer, wszTaskName);
	HRESULT hr = pITask->Run();
	pITask->Release();
	if (FAILED(hr))  {
	  WHOOPSLOG("ERROR: Failed calling ITask::Run, error = 0x%x",hr);
	  throw 7;
	}
 }

void whoOpsTaskTerminate(char* szServer, char* szTaskName) {
	CYCLOPSVAR(szServer, "%s");	CYCLOPSVAR(szTaskName, "%s");
	wchar_t wszServer[1000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszTask[1000];
	_snwprintf_s(wszTask, CYCLOPSSIZEOF(wszTask), _TRUNCATE, L"%S", szTaskName);
	CYCLOPSVAR(wszServer, "%S");	CYCLOPSVAR(wszTask, "%S");
	::whoOpsTaskTerminate(wszServer, wszTask);
}


void whoOpsTaskTerminate(LPCWSTR wszServer,	LPCWSTR wszTaskName)
{
	CYCLOPSVAR(wszServer, "%S"); CYCLOPSVAR(wszTaskName, "%S");
	ITask *pITask = whoOpsTaskGetTask(wszServer, wszTaskName);
	CYCLOPSVAR(pITask, "%p");
	HRESULT hr = pITask->Terminate();
	CYCLOPSVAR(hr, "%0x");
	pITask->Release();
	CYCLOPSDEBUG("Release() complete.");
	if (FAILED(hr))  {
		CYCLOPSERROR("Terminate() failed.");
		WHOOPS_THROW_EXCEPTION("ERROR: Failed calling ITask::Terminate(), error = 0x%x", hr);
	}
}


ITask* whoOpsTaskGetTask(LPCWSTR wszServer, LPCWSTR wszTaskName) {
	ITaskScheduler *pITS;
	try {
		pITS = whoOpsTaskGetTaskScheduler(wszServer);
	} catch (int iException) {
		WHOOPSLOG("ERROR: exception %d thrown by whoOpsGetTaskScheduler()", iException);
		WHOOPS_THROW_EXCEPTION("whoOpsTaskGetTaskScheduler(%S) threw %d", wszServer, iException);
	}
	WHOOPSLOG("debug: pITS = %p", pITS);
	try {
		ITask* pITask = whoOpsTaskActivate(pITS, wszTaskName);
		return pITask;
	} catch (int iException) {
		WHOOPSLOG("ERROR: whoOpsTaskActivate threw error %d", iException);
		pITS->Release();
		CoUninitialize();
		WHOOPS_THROW_EXCEPTION("whoOpsTaskActivate() threw %d.", iException);
	}
	pITS->Release();
}

void whoOpsTaskGetExitCode(	LPCWSTR wszServer,	LPCWSTR wszTaskName,	DWORD* pdwExitCode) {
	whoOpsTaskGetExitCode(wszServer, wszTaskName, pdwExitCode, NULL);
}

void whoOpsTaskGetExitCode(	LPCWSTR wszServer,	LPCWSTR wszTaskName,	DWORD* pdwExitCode,
	HRESULT* phResult) {
	// DONT YOU DARE ADD ANY LOOKUP OF WHAT THE EXIT CODE OR HRESULT MEAN IN
	// IN THIS METHOD.  That should go in another method.
	ITaskScheduler *pITS;
	ITask* pTask;
	try {
		pITS = whoOpsTaskGetTaskScheduler(wszServer);
		pTask = whoOpsTaskActivate(pITS, wszTaskName);
	} catch (int iException) {
		WHOOPSLOG("ERROR: exception %d thrown during whoOpsTaskGetExitCode()", iException);
		WHOOPS_THROW_EXCEPTION("ERROR: exception %d thrown during whoOpsTaskGetExitCode()", iException);
	} 
	HRESULT hr = pTask->GetExitCode(pdwExitCode);
	if (FAILED(hr)) {
		WHOOPSLOG("ERROR: GetExitCode() failed with error 0x%x", hr);
		CoUninitialize();
		throw 9;
	}
	if (phResult != NULL) {
		*phResult = hr;
	}
}
ITaskScheduler* whoOpsTaskGetTaskScheduler(LPCWSTR wszServer) {
  HRESULT hr = S_OK;
  ITaskScheduler *pITS;
  ///////////////////////////////////////////////////////////////////
  // Call CoInitialize to initialize the COM library and then
  // call CoCreateInstance to get the Task Scheduler object.
  ///////////////////////////////////////////////////////////////////
  hr = CoInitialize(NULL);
  if (SUCCEEDED(hr))
  {
     hr = CoCreateInstance(CLSID_CTaskScheduler,
						   NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_ITaskScheduler,
                           (void **) &pITS);
     if (FAILED(hr))
     {
        CoUninitialize();
        throw 1;
     }
  }
  else
  {
     throw 2;
  }

    /////////////////////////////////////////////////////////////////
  // Set target computer
  /////////////////////////////////////////////////////////////////

  hr = pITS->SetTargetComputer(wszServer);
  if (FAILED(hr))
  {
     CoUninitialize();
	 WHOOPSLOG("ERROR: Failed calling SetTargetComputer(), error = 0x%x", hr);
     throw 3;
  }
  return pITS;
}
ITask* whoOpsTaskActivate(ITaskScheduler* pScheduler, LPCWSTR wszTaskName) {
	ITask *pITask;
	CYCLOPSVAR(wszTaskName, "%S");
	HRESULT hr = pScheduler->Activate(wszTaskName,
				  IID_ITask,
				  (IUnknown**) &pITask);
	if (FAILED(hr))	{
		char* szError = ::whoOpsErrorFormatMessage(hr);
		CYCLOPSERROR("ITaskScheduler::Activate() returned 0x%x [%s]", hr, szError);
		throw 5;
	}
	return pITask;
}
DWORD whoOpsErrorPrintLast(char* szFunctionName) {
	DWORD dwError = GetLastError();
	char* szError = whoOpsErrorFormatMessage(dwError);
	WHOOPSLOG("ERROR: %s() failed with %d \"%s\"", szFunctionName, dwError, szError);
	return dwError;
}

// begin whoOpsService functions...
void      whoOpsServiceGetError(DWORD dwCode, char* szStatus, size_t iSize) {
	struct returnCode { DWORD dwCode; char* szString; };
	returnCode returnCodeArray[] = { 
		{ERROR_ACCESS_DENIED, "ERROR_ACCESS_DENIED"},
		{ERROR_DATABASE_DOES_NOT_EXIST, "ERROR_DATABASE_DOES_NOT_EXIST"},
		{ERROR_INVALID_HANDLE, "ERROR_INVALID_HANDLE"},
		{ERROR_INVALID_NAME, "ERROR_INVALID_NAME"},
		{ERROR_SERVICE_DOES_NOT_EXIST, "ERROR_SERVICE_DOES_NOT_EXIST"},
		{7623845, "14_REEBACK_DRIVE"}
	};

	for (int i = 0; returnCodeArray[i].dwCode != 7623845; ++i) {
		if (dwCode == returnCodeArray[i].dwCode) {
			char* szString = returnCodeArray[i].szString;
			strncpy_s(szStatus, iSize, szString, _TRUNCATE);
			return;
		}
	}
	strncpy_s(szStatus, iSize, "WHOOPS_UNKNOWN_ERROR", _TRUNCATE);
}

struct whoOpsServiceReturnCode { DWORD dwCode; char* szString; };

whoOpsServiceReturnCode whoOpsServiceReturnCodeArray[] = { 
		{ SERVICE_CONTINUE_PENDING, "SERVICE_CONTINUE_PENDING" },
		{ SERVICE_PAUSE_PENDING, "SERVICE_PAUSE_PENDING" },
		{ SERVICE_PAUSED, "SERVICE_PAUSED" },
		{ SERVICE_RUNNING, "SERVICE_RUNNING" },
		{ SERVICE_START_PENDING, "SERVICE_START_PENDING" },
		{ SERVICE_STOP_PENDING, "SERVICE_STOP_PENDING" },
		{ SERVICE_STOPPED, "SERVICE_STOPPED" },
		{ 7623845, "14_REEBACK_DRIVE"}
};

string whoOpsServiceGetStringForStatusCode(DWORD dwCode) {
	for (int i = 0; whoOpsServiceReturnCodeArray[i].dwCode != 7623845; ++i) {
		if (dwCode == whoOpsServiceReturnCodeArray[i].dwCode) {
			return whoOpsServiceReturnCodeArray[i].szString;
		}
	}
	return "WHOOPS_STATUS_UNKNOWN";
}

void      whoOpsServiceGetStatus(DWORD dwCode, char* szStatus, size_t iSize) {
	for (int i = 0; whoOpsServiceReturnCodeArray[i].dwCode != 7623845; ++i) {
		if (dwCode == whoOpsServiceReturnCodeArray[i].dwCode) {
			char* szString = whoOpsServiceReturnCodeArray[i].szString;
			strncpy_s(szStatus, iSize, szString, _TRUNCATE);
			return;
		}
	}
	strncpy_s(szStatus, iSize, "WHOOPS_STATUS_UNKNOWN", _TRUNCATE);
}

void  whoOpsServiceWaitUntil(LPCWSTR wszServer, LPCWSTR wszService, int iSeconds, DWORD dwState) {
	SC_HANDLE hService = whoOpsServiceGetHandle(wszServer, wszService);
	SERVICE_STATUS serviceStatus; 
	bool boReady = false;
	int i = 0;
	CYCLOPSINFO("Waiting for service");
	while (!boReady && i < iSeconds) {
		if (!QueryServiceStatus(hService, &serviceStatus)) {
			whoOpsErrorPrintLast("QueryServiceStatus");
			CloseServiceHandle(hService);
			throw 4;
		}
		CYCLOPSDEBUG("dwCurrentState = %d; dwState = %d", serviceStatus.dwCurrentState, dwState);
		if (serviceStatus.dwCurrentState == dwState) {
			boReady = true;
		} else {
			printf(".");
			Sleep(1000);
			i += 1;
		}
		CYCLOPSINFO("boReady = %d; i = %d", boReady, i);
	}
	printf("\n");
}

void  whoOpsServiceWaitUntilII(const wstring& wstrServer, const wstring& wstrService, int iSeconds, DWORD dwState) {
	SC_HANDLE hService = whoOpsServiceGetHandle(wstrServer.c_str(), wstrService.c_str());
	SERVICE_STATUS serviceStatus; 
	bool boReady = false;
	int i = 0; CYCLOPSDEBUG("Waiting for service");
	while (!boReady && i < iSeconds) {
		if ( ! QueryServiceStatus(hService, &serviceStatus)) {
			DWORD dwLastError = ::GetLastError();
			CloseServiceHandle(hService);
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionServiceManagement, dwLastError, 
				cyclOps::Exception::TYPE_WIN32, 
				"QueryServiceStatus() failed for %S/%S", wstrServer.c_str(), wstrService.c_str());
		} CYCLOPSDEBUG("dwCurrentState = %d; dwState = %d", serviceStatus.dwCurrentState, dwState);
		if (serviceStatus.dwCurrentState == dwState) {
			boReady = true;
		} else {
			Sleep(1000);
			i += 1;
		}
	}
	CloseServiceHandle(hService);
	if ( ! boReady) {
		CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionServiceManagement, 
			"Timed out waiting for %S/%S to achive status %s", wstrServer.c_str(), wstrService.c_str(),
			::whoOpsServiceGetStringForStatusCode(dwState).c_str());
	}
}


void whoOpsServiceWaitUntil(const char* szServer, const char* szService, int iSeconds, DWORD dwState) {
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	::whoOpsServiceWaitUntil(wszServer, wszService, iSeconds, dwState);
}

SC_HANDLE whoOpsServiceGetHandle(LPCWSTR wszServer, LPCWSTR wszService) {
	SC_HANDLE hSCM = OpenSCManager(wszServer, NULL, SC_MANAGER_ALL_ACCESS);
	char* szFailedFunction = NULL;
	if (hSCM != NULL) {
		SC_HANDLE hService = OpenService(hSCM, wszService, SC_MANAGER_ALL_ACCESS);
		CloseServiceHandle(hSCM);
		if (hService != NULL) {
				return hService;
		} else {
			szFailedFunction = "OpenService";
		}
	} else {
		szFailedFunction = "OpenSCManager";
	}
	char* szError = whoOpsErrorGetLastMessage();
	WHOOPS_THROW_EXCEPTION_II(whoOps::ExceptionWhoOps::UNABLE_TO_GET_SERVICE_HANDLE,
		"%s() returned NULL.  Error was '%s'.", szFailedFunction, szError);
}

SC_HANDLE whoOpsServiceGetHandle(const char* szServer, const char* szService) {
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	return ::whoOpsServiceGetHandle(wszServer, wszService);
}


void whoOpsServiceControlService(const wchar_t* wszServer, const wchar_t* wszService, DWORD dwControl) {
	CYCLOPSDEBUG("Hello.");
	SC_HANDLE hService = whoOpsServiceGetHandle(wszServer, wszService);
	SERVICE_STATUS serviceStatus;
	if (!ControlService(hService, dwControl, &serviceStatus)) {
		/* What should I do? */
		char* szMessage = whoOpsErrorGetLastMessage();
		CYCLOPSERROR("ControlService() failed: '%s'.", szMessage);
	}
}

void whoOpsServiceControlService(const char* szServer, const char* szService, DWORD dwControl) { CYCLOPSDEBUG("Hello.");
	CYCLOPSDEBUG("Hello.");
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	::whoOpsServiceControlService(wszServer, wszService, dwControl);
}

void whoOpsServiceStop(LPCWSTR wszServer, LPCWSTR wszService) { CYCLOPSDEBUG("Hello.");
	SC_HANDLE hService = whoOpsServiceGetHandle(wszServer, wszService);
	SERVICE_STATUS serviceStatus;
	if ( ! ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
		DWORD dwError = ::GetLastError();
		// error 1062 means service is already stopped 
		if (dwError != 1062) {
			CloseServiceHandle(hService);
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionServiceManagement, dwError, 
				cyclOps::Exception::TYPE_WIN32, "ControlService() failed for %S / %S", wszServer, wszService);

		}
	}
	CloseServiceHandle(hService);
}

void whoOpsServiceStop(const char* szServer, const char* szService) { CYCLOPSDEBUG("Hello.");
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	::whoOpsServiceStop(wszServer, wszService);
}

void whoOpsServiceStart(LPCWSTR wszServer, LPCWSTR wszService) { CYCLOPSDEBUG("Hello.");
	SC_HANDLE hService = whoOpsServiceGetHandle(wszServer, wszService);
	if (!StartService(hService, 0, NULL)) {
		DWORD dwLastError = GetLastError();
		if (dwLastError == ERROR_SERVICE_ALREADY_RUNNING) {
			CYCLOPSDEBUG("The service is already running.");
		} else {
			char* szLastError = ::whoOpsErrorFormatMessage(dwLastError);
			CloseServiceHandle(hService);
			WHOOPS_THROW_EXCEPTION("StartService() failed with the following error: '%s'.", szLastError);
		}
	}
	CloseServiceHandle(hService);
}

void whoOpsServiceStart(const char* szServer, const char* szService) {
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	::whoOpsServiceStart(wszServer, wszService);
}

extern "C" __declspec(dllexport) void whoOpsServiceQueryServiceStatus(LPCWSTR wszServer, LPCWSTR wszService, char* szStatus, size_t iSize) {

	try {
		SERVICE_STATUS serviceStatus;
		::whoOpsServiceQueryServiceStatusII(wszServer, wszService, &serviceStatus);
		::whoOpsServiceGetStatus(serviceStatus.dwCurrentState, szStatus, iSize);
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
	}
}

void whoOpsServiceQueryServiceStatusII(LPCWSTR wszServer, LPCWSTR wszService, SERVICE_STATUS* pServiceStatus) {
		SC_HANDLE hService = whoOpsServiceGetHandle(wszServer, wszService);
		if (!::QueryServiceStatus(hService, pServiceStatus)) {
			char* szLastError = whoOpsErrorGetLastMessage();
			WHOOPS_THROW_EXCEPTION("QueryServiceStatus() failed: %s", szLastError);
		}
}

void whoOpsServiceQueryServiceStatusII(const char* szServer, const char* szService, SERVICE_STATUS* pServiceStatus) {
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	::whoOpsServiceQueryServiceStatusII(wszServer, wszService, pServiceStatus);
}

extern "C" __declspec(dllexport) boolean dllexportWhoOpsServiceRestart(LPCWSTR wszServer, LPCWSTR wszService, int iSeconds) {
	// This is really a duplicate function that just allows the dllexport prefix to be easily identified.
	try {
		boolean boReturn = whoOpsServiceRestart(wszServer, wszService, iSeconds);
		return boReturn;
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		return false;
	}
}

extern "C" __declspec(dllexport) boolean whoOpsServiceRestart(LPCWSTR wszServer, LPCWSTR wszService, int iSeconds) {
	try {
		whoOpsServiceStop(wszServer, wszService);
		whoOpsServiceWaitUntil(wszServer, wszService, iSeconds, SERVICE_STOPPED);
		whoOpsServiceStart(wszServer, wszService);
	} catch (int iException) {
		WHOOPSLOG("ERROR: exception %d thrown during whoOpsServiceRestart()", iException);
	} catch (const std::exception& e) {
		WHOOPSLOG("Exception %s thrown. %s", typeid(e).name(), e.what());
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
	}
	return false;
}
// ...end whoOpsService functions

// begin whoOps file functions...
extern "C" __declspec(dllexport) void whoOpsGetFileVersionInfo(const char* szFile, char* szVersion, size_t iSize) 
{
	try {
		whoOps::FileVersionInfo::getFileVersionInfo(szFile, szVersion, iSize);
	} catch (whoOps::ExceptionGeneric e) {
		CYCLOPSERROR("ExceptionGeneric caught '%s'", e.getMessage());
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
	}
}
// ...end whoOps file functions

extern "C" __declspec(dllexport) void whoOpsSetDebug(bool boDebug) {
	::g_boCyclOpsDebug = boDebug;
}

// begin whoOpsRegistry functions...
extern "C" __declspec(dllexport) void whoOpsRegistryGetREG_SZ(LPCSTR szServer, LPCSTR szKey, 
	LPCSTR szValue, char* szReturn, int iSize) 
{
	try {
		CYCLOPSDEBUG("Hello.");
		HKEY hKey;
		LONG lReturn = ::RegConnectRegistryA(
			szServer, 
			HKEY_LOCAL_MACHINE, 
			&hKey);
		CYCLOPSVAR(lReturn, "%d");
		if (lReturn != ERROR_SUCCESS) {
			CYCLOPSERROR("RegConnectRegistryA() failed.");
			char* szError = ::whoOpsErrorFormatMessage(lReturn);
			WHOOPSLOG("DEBUG: szError is %s", szError);
			_snprintf_s(szReturn, iSize - 1, _TRUNCATE, "ERROR: %s", szError);
			return;
		}
	
		DWORD dwType;
		char szBuffer[1024];
		DWORD dwSize = (sizeof(szBuffer) / sizeof(szBuffer[0])) - 1;
		lReturn = ::RegGetValueA(hKey, szKey, szValue, RRF_RT_REG_SZ, &dwType, (PVOID) szBuffer, &dwSize);
		if (lReturn != ERROR_SUCCESS) {
			CYCLOPSERROR("RegGetValueA() failed.");
			char* szError = ::whoOpsErrorFormatMessage(lReturn);
			CYCLOPSVAR(szError, "%s");
			_snprintf_s(szReturn, iSize - 1, _TRUNCATE, "ERROR: %s", szError);
			CYCLOPSVAR(szReturn, "%s");
			return;
		} 
		if (dwType != REG_SZ) {
			CYCLOPSERROR("Returned type was not a REG_SZ.");
			_snprintf_s(szReturn, iSize - 1, _TRUNCATE, "ERROR: Value is not a REG_SZ");
			return;
		}
		strncpy_s(szReturn, iSize -1, szBuffer, _TRUNCATE);
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
	}
}
// ...end whoOpsRegistry functions

extern "C" __declspec(dllexport) boolean dllexportWhoOpsNetLocalGroupAddMembers(
	const char* szHost, const char* szLocalGroup, const char* szMember) 
{
	wchar_t wszHost[5000];
	_snwprintf_s(wszHost, sizeof(wszHost) / sizeof(wszHost[0]), _TRUNCATE, L"%S", szHost);
	wchar_t wszLocalGroup[5000];
	_snwprintf_s(wszLocalGroup, sizeof(wszLocalGroup) / sizeof(wszLocalGroup[0]), _TRUNCATE, L"%S", szLocalGroup);
	wchar_t wszMember[5000];
	_snwprintf_s(wszMember, sizeof(wszMember) / sizeof(wszMember[0]), _TRUNCATE, L"%S", szMember);
	return ::whoOpsNetLocalGroupAddMembers(wszHost, wszLocalGroup, wszMember);
}

extern "C" __declspec(dllexport) boolean whoOpsNetLocalGroupAddMembers(const wchar_t* wszHost, 
	const wchar_t* wszLocalGroup, const wchar_t* wszMember) 
{
	NET_API_STATUS status = ::whoOpsNetLocalGroupAddMembersII(wszHost, wszLocalGroup, wszMember);
	return status == NERR_Success;
}

extern "C" __declspec(dllexport) NET_API_STATUS whoOpsNetLocalGroupAddMembersII(const wchar_t* wszHost, 
	const wchar_t* wszLocalGroup, const wchar_t* wszMember) 
{
	LOCALGROUP_MEMBERS_INFO_3 lmi3;
	wchar_t wszMemberNonConst[5000];
	::_snwprintf_s(wszMemberNonConst, sizeof(wszMemberNonConst) / sizeof(wszMemberNonConst[0]), _TRUNCATE, L"%s", wszMember);
	lmi3.lgrmi3_domainandname = wszMemberNonConst;
	NET_API_STATUS dwStatus = NetLocalGroupAddMembers(wszHost, wszLocalGroup, 3, (LPBYTE) &lmi3, 1);
	if (dwStatus != NERR_Success) {
		WHOOPSLOG("ERROR: NetLocalGroupAddMembers() returned %d", dwStatus);
		char* szError = whoOpsErrorFormatMessage(dwStatus);
		WHOOPSLOG("ERROR: which means \"%s\"", szError);
	} else {
		WHOOPSLOG("INFO: success!");
	}
	return dwStatus;
}

extern "C" __declspec(dllexport) boolean dllexportWhoOpsNetLocalGroupDelMembers(const char* szHost, const char* szLocalGroup, const char* szMember) {
	wchar_t wszHost[5000];
	_snwprintf_s(wszHost, sizeof(wszHost) / sizeof(wszHost[0]), _TRUNCATE, L"%S", szHost);
	wchar_t wszLocalGroup[5000];
	_snwprintf_s(wszLocalGroup, sizeof(wszLocalGroup) / sizeof(wszLocalGroup[0]), _TRUNCATE, L"%S", szLocalGroup);
	wchar_t wszMember[5000];
	_snwprintf_s(wszMember, sizeof(wszMember) / sizeof(wszMember[0]), _TRUNCATE, L"%S", szMember);
	return ::whoOpsNetLocalGroupDelMembers(wszHost, wszLocalGroup, wszMember);
}

extern "C" __declspec(dllexport) boolean whoOpsNetLocalGroupDelMembers(wchar_t* wszHost, wchar_t* wszLocalGroup, wchar_t* wszMember) {
	LOCALGROUP_MEMBERS_INFO_3 lmi3;
	lmi3.lgrmi3_domainandname = wszMember;
	NET_API_STATUS dwStatus = NetLocalGroupDelMembers(wszHost, wszLocalGroup, 3, (LPBYTE) &lmi3, 1);
	if (dwStatus != NERR_Success) {
		WHOOPSLOG("ERROR: NetLocalGroupDelMembers() returned %d", dwStatus);
		char* szError = whoOpsErrorFormatMessage(dwStatus);
		
		WHOOPSLOG("ERROR: which means \"%s\"", szError);
		return false;
	}
	WHOOPSLOG("INFO: success!");
	return true;
}
extern "C" __declspec(dllexport) boolean whoOpsOSGetVersion_DEPRECATED(OSVERSIONINFO* pOSVersionInfo) {
/*	pOSVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL boReturn = ::GetVersionEx(pOSVersionInfo);
	if (!boReturn) {
		::whoOpsErrorPrintLast("whoOpsOSGetVersion()");
		return false;
	}
	return true;
*/
		return false;
}
extern "C" __declspec(dllexport) boolean whoOpsNetFileEnum(LPWSTR wszHost, void (*callbackFunction)(FILE_INFO_3* pFI3, LPWSTR wszHost)) {
	try {
		printf("debug: whoOpsNetFileEnum()\n");
		FILE_INFO_3 *buf, *cur;
		DWORD read, total, rc, i;
		DWORD_PTR resumeh;
		const int IMAX = 256;
		wchar_t server[IMAX];
		_snwprintf_s(server, IMAX - 1, _TRUNCATE, L"%s", wszHost);
		printf("debug: server is %S\n", server);
		resumeh = 0;
		do	{
			buf = NULL;
			rc = NetFileEnum( server, NULL, NULL, 3,
				(BYTE **) &buf, 2048, &read, &total, &resumeh );
			if ( rc != ERROR_MORE_DATA && rc != ERROR_SUCCESS ) {
				printf("NetFileEnum returned %d.\n", rc);
				return false;
			}
			for ( i = 0, cur = buf; i < read; ++ i, ++ cur )
			{
				// Note: the capital S in the format string will expect Unicode
				// strings, as this is a program written/compiled for ANSI.
				callbackFunction(cur, server);
			}

			if ( buf != NULL )
				NetApiBufferFree( buf );

		} while ( rc == ERROR_MORE_DATA );
		return true;
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		return false;
	}
}
extern "C" __declspec(dllexport) boolean whoOpsNetFileClose(wchar_t* wszHost, DWORD dwFileID) {
	NET_API_STATUS nasReturn = ::NetFileClose(wszHost, dwFileID);
	if (nasReturn != NERR_Success) {
		WHOOPSLOG("ERROR: NetFileClose() returned %d", nasReturn);
		char* szError = whoOpsErrorFormatMessage(nasReturn);
		WHOOPSLOG("ERROR: which means \"%s\"", szError);
		return false;
	}
	return true;
}
extern "C" __declspec(dllexport) boolean whoOpsNetLocalGroupGetMembers(const wchar_t* wszHost, const wchar_t* wszGroup, 
	void (*callbackFunction)(LOCALGROUP_MEMBERS_INFO_3* pLMI3, LPWSTR wszHost, LPWSTR wszGroup)) 
{
	LOCALGROUP_MEMBERS_INFO_3 *pLMI3Buffer, * pLMI3Current;
	DWORD dwRead, dwTotal, dwReturn;
	DWORD_PTR dwResumeHandle;
	const int IMAX = 256;
	wchar_t server[IMAX], group[IMAX];

	_snwprintf_s(group, IMAX - 1, _TRUNCATE, L"%s", wszGroup);
	_snwprintf_s(server, IMAX - 1, _TRUNCATE, L"%s", wszHost);
	dwResumeHandle = 0;
	do {
		pLMI3Buffer = NULL;
		dwReturn = NetLocalGroupGetMembers( server, group, 3, (BYTE **) &pLMI3Buffer, 2048, &dwRead, 
			&dwTotal, &dwResumeHandle);
		if ( dwReturn != ERROR_MORE_DATA && dwReturn != ERROR_SUCCESS ) {
			return false;
		}
		DWORD i;
		for ( i = 0, pLMI3Current = pLMI3Buffer ; i < dwRead; ++ i, ++ pLMI3Current)	{
			callbackFunction(pLMI3Current, server, group);
		}
		if ( pLMI3Buffer != NULL ) {
			NetApiBufferFree( pLMI3Buffer );
		}
	} while ( dwReturn == ERROR_MORE_DATA );
	return true;
}

void whoOpsPrintGroup(LOCALGROUP_MEMBERS_INFO_3* pLMI3, LPWSTR wszHost, LPWSTR wszGroup)  {
	printf("%S\n", pLMI3->lgrmi3_domainandname);
}

extern "C" __declspec(dllexport) void dllexportNetLocalGroupGetMembers(const wchar_t* wszHost, const wchar_t* wszGroup) {
	::whoOpsNetLocalGroupGetMembers(wszHost, wszGroup, whoOpsPrintGroup);
}

extern "C" __declspec(dllexport) UINT __stdcall whoOpsTaskNewWorkItem(	LPCWSTR wszServer,	LPCWSTR wszTaskName,	LPCWSTR wszExecutable,	LPCWSTR wszParameters,	LPCWSTR wszUser,	LPCWSTR wszPassword) {
	try {
		whoOpsPrivateTaskNewWorkItem(wszServer, wszTaskName, wszExecutable,	wszParameters, wszUser, wszPassword);
		return true;
	} catch (int iException) {
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);
		return false;
	} catch (const std::exception& e) {
		WHOOPSLOG("%s - %s", typeid(e).name(), e.what());
		return false;
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		return false;
	}
}
JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_scheduleAndRunNative(JNIEnv *env, jobject obj, jstring input)
{
	const char *str = env->GetStringUTFChars(input, NULL);
	if (str == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	const int i = 6;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", str);
	char * superfluous;
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		WHOOPSLOG("DEBUG: wargv[%d] is %S", j, wargv[j]);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	char szMessage[5000];
	try {
		whoOpsPrivateTaskNewWorkItem(wargv[0], wargv[1], wargv[2], wargv[3], wargv[4], wargv[5]);
		whoOpsTaskRun(wargv[0], wargv[1]);
	} catch (int iException) {
		_snprintf_s(szMessage, sizeof(szMessage) / sizeof(szMessage[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		CYCLOPSERROR(szMessage);
	} catch (const std::exception& e) {
		_snprintf_s(szMessage, sizeof(szMessage) / sizeof(szMessage[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		CYCLOPSERROR(szMessage);
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		_snprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE, "ERROR: Unknown exception caught.");
	}
	env->ReleaseStringUTFChars(input, str);
	_snprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE, "SUCCESS: All is good.");
	return env->NewStringUTF(szMessage);
}
JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_runNative(JNIEnv *env, jobject obj, jstring input)
{
	const char *str = env->GetStringUTFChars(input, NULL);
	if (str == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	const int i = 2;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", str);
	char * superfluous;
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	try {
		WHOOPSLOG("debug: About to call whoOpsTaskRun().");
		whoOpsTaskRun(wargv[0], wargv[1]);
		WHOOPSLOG("debug: whoOpsTaskRun() done.");
	} catch (int iException) {
		WHOOPSLOG("debug: %d caught.", iException);
		env->ReleaseStringUTFChars(input, str);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, 
			"ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (const std::exception& e) {
		env->ReleaseStringUTFChars(input, str);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}

	WHOOPSLOG("debug: About to ReleaseStringUTFChars().");
	env->ReleaseStringUTFChars(input, str);
	char* buf = "SUCCESS: all is good.";
	return env->NewStringUTF(buf);
}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_terminateNative(JNIEnv *env, jobject obj, jstring input)
{
	char szReturn[5000];
	try {
		cyclOps::JNIJunkie jniJunkie(env, obj, input);
		// This second dimension must match the declaration of getArgumentArray(). See http://www.cplusplus.com/forum/general/11627/
		wchar_t wszArgumentArray[2][5000];
		jniJunkie.getArgumentArray(wszArgumentArray, 2);
		::whoOpsTaskTerminate(wszArgumentArray[0], wszArgumentArray[1]);
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "SUCCESS");
	} catch (std::exception& e) {
		CYCLOPSERROR("Exception was caught. %s", e.what());
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "ERROR: Exception occurred. %s", e.what());
		CYCLOPSVAR(szReturn, "%s");
	} catch (...) {
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "ERROR: Unknown exception occurred.");
	}

	return env->NewStringUTF(szReturn);
}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_getStatusNative(JNIEnv *env, jobject obj, jstring input) {
	char* szReturn;
	try {
		cyclOps::JNIJunkie jniJunkie(env, obj, input);
		// This second dimension must match the declaration of getArgumentArray(). See http://www.cplusplus.com/forum/general/11627/
		wchar_t wszArgumentArray[2][5000];
		jniJunkie.getArgumentArray(wszArgumentArray, 2);
		szReturn = ::whoOpsTaskGetStatus(wszArgumentArray[0], wszArgumentArray[1]);
	} catch (...) {
		CYCLOPSERROR("Exception caught.");
		szReturn = "ERROR: Exception occurred.";
	}
	return env->NewStringUTF(szReturn);
}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_deleteNative(JNIEnv *env, jobject obj, jstring input)
{
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	// number of arguments expected
	const int i = 2;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	try {
		whoOpsTaskDelete(wargv[0], wargv[1]);
	} catch (int iException) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);

		return env->NewStringUTF(szError);
	} catch (const std::exception& e) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (...) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: Unknown exception caught.");
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	env->ReleaseStringUTFChars(input, szInput);
	char* buf = "SUCCESS: all is good.";
	return env->NewStringUTF(buf);
}
JNIEXPORT jstring JNICALL Java_whoOps_ServiceMangler_restartNative(JNIEnv *env, jobject obj, jstring input) {
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	// number of arguments expected
	const int i = 3;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	try {
		whoOpsServiceStop(wargv[0], wargv[1]);
		int iWait = _wtoi(wargv[2]);
		whoOpsServiceWaitUntil(wargv[0], wargv[1], iWait, SERVICE_STOPPED);
		WHOOPSLOG("INFO: service stopped");
		whoOpsServiceStart(wargv[0], wargv[1]);
		whoOpsServiceWaitUntil(wargv[0], wargv[1], iWait, SERVICE_RUNNING);
	} catch (int iException) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (const std::exception& e) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	env->ReleaseStringUTFChars(input, szInput);
	char* buf = "SUCCESS: all is good.";
	return env->NewStringUTF(buf);
}
JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_scheduleNative(JNIEnv *env, jobject obj, jstring input)
{
	const char *str = env->GetStringUTFChars(input, NULL);
	if (str == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	const int i = 6;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", str);
	char * superfluous;
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		WHOOPSLOG("DEBUG: wargv[%d] is %S", j, wargv[j]);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	try {
		whoOpsPrivateTaskNewWorkItem(wargv[0], wargv[1], wargv[2], wargv[3], wargv[4], wargv[5]);
	} catch (int iException) {
		env->ReleaseStringUTFChars(input, str);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (const std::exception& e) {
		env->ReleaseStringUTFChars(input, str);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	env->ReleaseStringUTFChars(input, str);
	char* buf = "SUCCESS: all is good.";
	return env->NewStringUTF(buf);
}
JNIEXPORT jstring JNICALL Java_whoOps_ServiceMangler_getServiceStatusNative(JNIEnv *env, jobject obj, jstring input) {
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	// number of arguments expected
	const int i = 2;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	// DANGEROUS!!!!!!
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	char szStatus[500];
	int iSize = sizeof(szStatus) / sizeof(szStatus[0]) - 1;
	::whoOpsServiceQueryServiceStatus(wargv[0], wargv[1], szStatus, iSize);
	env->ReleaseStringUTFChars(input, szInput);
	return env->NewStringUTF(szStatus);
}
JNIEXPORT jstring JNICALL Java_whoOps_ServiceMangler_stopServiceNative(JNIEnv *env, jobject obj, jstring input) {
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	// number of arguments expected
	// 0 = server
	// 1 = service
	// 2 = seconds to wait
	const int i = 3;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	// DANGEROUS!!!!!!
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	try {
		::whoOpsServiceStopStopStopStopStop(wargv[0], wargv[1]);
		int iSeconds = _wtoi(wargv[2]);
		::whoOpsServiceWaitUntil(wargv[0], wargv[1], iSeconds, SERVICE_STOPPED);

	} catch (int iException) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (const std::exception& e) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (...) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: Unknown exception thrown.");
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	env->ReleaseStringUTFChars(input, szInput);
	char* buf = "SUCCESS: Service stopped.";
	return env->NewStringUTF(buf);
}
JNIEXPORT jstring JNICALL Java_whoOps_ServiceMangler_startServiceNative(JNIEnv *env, jobject obj, jstring input) {
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	// number of arguments expected
	// 0 = server
	// 1 = service
	// 2 = seconds to wait
	const int i = 3;
	wchar_t wargv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	// DANGEROUS!!!!!!
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snwprintf_s(wargv[j], sizeof(wargv[j]) / sizeof(wargv[j][0]), _TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	try {
		::whoOpsServiceStart(wargv[0], wargv[1]);
		int iSeconds = _wtoi(wargv[2]);
		::whoOpsServiceWaitUntil(wargv[0], wargv[1], iSeconds, SERVICE_RUNNING);
	} catch (int iException) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %d thrown.", iException);
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	} catch (const std::exception& e) {
		env->ReleaseStringUTFChars(input, szInput);
		char szError[500]; 
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "ERROR: exception %s thrown. %s", typeid(e).name(), e.what());
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	env->ReleaseStringUTFChars(input, szInput);
	char* buf = "SUCCESS: Service running.";
	return env->NewStringUTF(buf);
}
JNIEXPORT jstring JNICALL Java_whoOps_RegistryRaptor_getREG_1SZNative(JNIEnv * env, jobject obj, jstring input) {
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	// number of arguments expected
	const int i = 3;
	char argv[i][500];
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	// DANGEROUS!!!!!!
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int j = 0; j < i && pch != NULL; ++j) {
		_snprintf_s(argv[j], sizeof(argv[j]) / sizeof(argv[j][0]), _TRUNCATE, "%s", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
	char szReturn[500];
	int iSize = sizeof(szReturn) / sizeof(szReturn[0]) - 1;
	::whoOpsRegistryGetREG_SZ(argv[0], argv[1], argv[2], szReturn, iSize);
	env->ReleaseStringUTFChars(input, szInput);
	return env->NewStringUTF(szReturn);

}
JNIEXPORT jstring JNICALL Java_whoOps_FileVersionFlunky_getFileVersionNative(JNIEnv * env, jobject obj, jstring input) {
	const char *szInput = env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	char szReturn[5000];
	try {
		whoOps::FileVersionInfo::getFileVersionInfo(szInput, szReturn, 
			CYCLOPSSIZEOF(szReturn), true);
	} catch (whoOps::ExceptionGeneric e) {
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, 
			"ERROR: whoOps::ExceptionGeneric caught - %s", e.getMessage());
		CYCLOPSERROR("%s", szReturn);
	}
	env->ReleaseStringUTFChars(input, szInput);
	return env->NewStringUTF(szReturn);
}
JNIEXPORT jstring JNICALL Java_whoOps_ProcessOpotamus_createProcessNative(JNIEnv * env, jobject obj, jstring input) {
	char *szInput = (char*) env->GetStringUTFChars(input, NULL);
	if (szInput == NULL) {
		char* szError = "ERROR: GetStringUTFChars returned NULL";
		WHOOPSLOG(szError);
		return env->NewStringUTF(szError);
	}
	char szReturn[5000];
	try {
		// ProcessPrincess.cpp/h have been moved to whoOps.
		DWORD dwExitCode = whoOps::ProcessPrincess::CreateProcessII(szInput);
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "%d", dwExitCode);
	} catch (...) {
		_snprintf_s(szReturn, CYCLOPSSIZEOF(szReturn), _TRUNCATE, "ERROR: Unspecified exception caught.");
		CYCLOPSERROR("%s", szReturn);
	}
	env->ReleaseStringUTFChars(input, szInput);
	return env->NewStringUTF(szReturn);

}

extern "C" __declspec(dllexport) char* dllexportWhoOpsTaskTerminate(char* szServer, char* szTask) {
	CYCLOPSVAR(szServer, "%s"); CYCLOPSVAR(szTask, "%s");
	try {
		::whoOpsTaskTerminate(szServer, szTask);
		return "SUCCESS";
	} catch (const std::exception& e) {
		CYCLOPSERROR("Exception '%s'", e.what());
		return "ERROR";
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		return "ERROR";
	}
}

extern "C" __declspec(dllexport) boolean dllexportWhoOpsTaskRun(char* szServer, char* szTask) {
	CYCLOPSVAR(szServer, "%s");	CYCLOPSVAR(szTask, "%s");
	wchar_t wszServer[1000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszTask[1000];
	_snwprintf_s(wszTask, CYCLOPSSIZEOF(wszTask), _TRUNCATE, L"%S", szTask);
	CYCLOPSVAR(wszServer, "%S");	CYCLOPSVAR(wszTask, "%S");
	try {
		::whoOpsTaskRun(wszServer, wszTask);
		return true;
	} catch (int iException) {
		CYCLOPSERROR("Exception thrown by ::whoOpsTaskRun(). %d", iException);
		return false;
	} catch (const std::exception& e) {
		CYCLOPSERROR("%s thrown by whoOpsTaskRun(). %s", typeid(e).name(), e.what());
		return false;
	}
}

extern "C" __declspec(dllexport) boolean dllexportWhoOpsTaskGetExitCode(char* szServer, char* szTask, DWORD* pdwExitCode) {
	CYCLOPSVAR(szServer, "%s");	CYCLOPSVAR(szTask, "%s");
	wchar_t wszServer[1000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszTask[1000];
	_snwprintf_s(wszTask, CYCLOPSSIZEOF(wszTask), _TRUNCATE, L"%S", szTask);
	CYCLOPSVAR(wszServer, "%S");	CYCLOPSVAR(wszTask, "%S");
	try {
		::whoOpsTaskGetExitCode(wszServer, wszTask, pdwExitCode);
		return true;
	} catch (int iException) {
		CYCLOPSERROR("Exception thrown by ::whoOpsTaskRun(). %d", iException);
		return false;
	} catch (const std::exception& e) {
		CYCLOPSERROR("%s thrown by whoOpsTaskRun() - %s", typeid(e).name(), e.what());
		return false;
	}
}

void whoOpsTaskFillJNIResponse(const whoOps::Task& task, cyclOps::JNIResponse& response) {
	response.putProperty("lastRunTime", task.getLastRunTime());
	response.putProperty("nextRunTime", task.getNextRunTime());
	response.putProperty("taskName", task.getTaskName());
	response.putProperty("userID", task.getUserID());
	response.putProperty("server", task.getServer());
	response.putProperty("state", task.getState());
	response.putProperty("lastResult", task.getLastResult());
	response.putProperty("lastResultMessage", task.getLastResultMessage());
	response.putProperty("path", task.getPath());
	response.putProperty("arguments", task.getArguments());
}

void whoOpsTaskCreateTaskFromJNIRequest(const cyclOps::JNIRequest& request, whoOps::Task& task) {
	task.setServer(request.getProperty("server"));
	task.setTaskName(request.getProperty("taskName"));
	task.setPath(request.getProperty("path"));
	task.setArguments(request.getProperty("arguments"));
	task.setUserID(request.getProperty("userID"));
	task.setPassword(request.getProperty("password"));
}

void whoOpsTaskDoSomethingWithIt(const cyclOps::JNIRequest& request, cyclOps::JNIResponse& response, const whoOpsWhatToDo& whatToDo) {
	whoOps::Taskadoodledoo taskadoo(request.getProperty("server"));
	if (whatToDo == whoOpsWhatToDo::RUN_TASK || whatToDo == whoOpsWhatToDo::STOP_TASK) {
		taskadoo.runOrStop(request.getProperty("task"), whatToDo);
		response.setMessage("The request to run or stop the task was sent.");
	} else if (whatToDo == whoOpsWhatToDo::GET_LAST_RUN_TIME) {
		string lastRunTime = taskadoo.getLastRunTime(request.getProperty("task"));
		response.putProperty("lastRunTime", "%s", lastRunTime.c_str());
		stringstream ss; ss << "Last run time was " << lastRunTime;
		response.setMessage(ss.str());
	} else if (whatToDo == whoOpsWhatToDo::GET_EVERYTHING) {
		whoOps::Task task;
		taskadoo.getAllInformation(request.getProperty("task"), task);
		whoOpsTaskFillJNIResponse(task, response);
		response.setMessage("Task information successfully retrieved.");
	} else if (whatToDo == whoOpsWhatToDo::SCHEDULE_TASK) {
		whoOps::Task task;
		::whoOpsTaskCreateTaskFromJNIRequest(request, task);
		taskadoo.schedule(task);
		stringstream message; 
		message << "The task '" << task.getTaskName() << "' was scheduled on '" << task.getServer() << "'.";
		response.setMessage(message.str());
	}
}

string whoOpsTaskDoSomethingWithIt(const cyclOps::JNIRequest& request, const whoOpsWhatToDo& whatToDo) {
	cyclOps::JNIResponse response;
	response.setStatus("UNKNOWN");
	response.setMessage("The results are as yet unknown.");
	try {
		g_boCyclOpsDebug = request.getDebug();
		::whoOpsTaskDoSomethingWithIt(request, response, whatToDo);
		response.setStatus("SUCCESS");
	} catch (const cyclOps::Exception& e) {
		CYCLOPSERROR("%s caught with a code of %lu (%lx) .", typeid(e).name(), e.getLastError(), e.getLastError());
		if (e.getType() == cyclOps::Exception::TYPE_HRESULT) {
			HRESULT hr = e.getLastError();
			if (whoOps::ErroroneousMonk::isFileNotFound(hr)) {
				response.setStatus("NO_SUCH_TASK");
				stringstream ss; ss << typeid(e).name() << " - " << e.what() << " - " << e.formatMessage();
				response.setMessage(ss.str());
			}
		} else {
			response.setStatus("EXCEPTION");
			stringstream ss; ss << typeid(e).name() << " - " << e.what() << " - " << e.formatMessage();
			response.setMessage(ss.str());
		}
	} catch (const std::exception& e) {
		CYCLOPSDEBUG("%s caught.", typeid(e).name());
		response.setStatus("EXCEPTION");
		stringstream ss; ss << typeid(e).name() << " - " << e.what();
		response.setMessage(ss.str());
	} catch (...) {
		response.setStatus("EXCEPTION");
		response.setMessage("An unknown exception occurred.");
	}
	return response.getPropertyMapAsXMLString();

}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_runNativeII(JNIEnv *env, jobject obj, jstring input) {
	// All the II methods will use the new Task Scheduler API.  The non-II methods use the old API.
	cyclOps::JNIRequest request(env, obj, input);
	string strResponse = whoOpsTaskDoSomethingWithIt(request, whoOpsWhatToDo::RUN_TASK);
	return env->NewStringUTF(strResponse.c_str());
}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_scheduleNativeII(JNIEnv *env, jobject obj, jstring input) {
	// All the II methods will use the new Task Scheduler API.  The non-II methods use the old API.
	cyclOps::JNIRequest request(env, obj, input);
	string strResponse = whoOpsTaskDoSomethingWithIt(request, whoOpsWhatToDo::SCHEDULE_TASK);
	return env->NewStringUTF(strResponse.c_str());
}



JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_stopNativeII(JNIEnv *env, jobject obj, jstring input) {
	// All the II methods will use the new Task Scheduler API.  The non-II methods use the old API.
	cyclOps::JNIRequest request(env, obj, input);
	string strResponse = whoOpsTaskDoSomethingWithIt(request, whoOpsWhatToDo::STOP_TASK);
	return env->NewStringUTF(strResponse.c_str());
}


JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_getLastRunTimeNativeII(JNIEnv *env, jobject obj, jstring input) {
	// All the II methods will use the new Task Scheduler API.  The non-II methods use the old API.
	cyclOps::JNIRequest request(env, obj, input);
	string strResponse = whoOpsTaskDoSomethingWithIt(request, whoOpsWhatToDo::GET_LAST_RUN_TIME);
	return env->NewStringUTF(strResponse.c_str());

}

JNIEXPORT jstring JNICALL Java_whoOps_TaskScheduler_getAllInformationNativeII(JNIEnv *env, jobject obj, jstring input) {
	// All the II methods will use the new Task Scheduler API.  The non-II methods use the old API.
	cyclOps::JNIRequest request(env, obj, input);
	string strResponse = whoOpsTaskDoSomethingWithIt(request, whoOpsWhatToDo::GET_EVERYTHING);
	return env->NewStringUTF(strResponse.c_str());
}


char* whoOpsTaskConvertStatusToString(HRESULT lStatus) {
	switch(lStatus) {
		case SCHED_S_TASK_READY:
			return "SCHED_S_TASK_READY";
			break;
		case SCHED_S_TASK_RUNNING:
			return "SCHED_S_TASK_RUNNING";
			break;
		case SCHED_S_TASK_DISABLED:
			return "SCHED_S_TASK_DISABLED";
			break;
		case SCHED_S_TASK_HAS_NOT_RUN:
			return "SCHED_S_TASK_HAS_NOT_RUN";
			break;
		case SCHED_S_TASK_NOT_SCHEDULED:
			return "SCHED_S_TASK_NOT_SCHEDULED";
			break;
		case SCHED_S_TASK_NO_MORE_RUNS:
			return "SCHED_S_TASK_NO_MORE_RUNS";
			break;
		case SCHED_S_TASK_NO_VALID_TRIGGERS:
			return "SCHED_S_TASK_NO_VALID_TRIGGERS";
			break;
		default:
			return "UNKNOWN"; 
		}
}

extern "C" __declspec(dllexport) int dllexportWhoOpsACLAddACEToDACL(
	char* szPath, char* szSecurityPrinciple, char*szPermission) {
		try {
			whoOps::ACLAmigo aclAmigo;
			aclAmigo.addPermissions(szPath, szSecurityPrinciple, szPermission);
			return 0;
		} catch (...) {
			return 1;
		}
}

extern "C" __declspec(dllexport) char* dllexportWhoOpsErrorFormatMessage(DWORD dwError) {
	return ::whoOpsErrorFormatMessage(dwError);
}

extern "C" __declspec(dllexport) int dllexportWhoOpsRegSetValueEx(char* szServer, char* szKey, char* szValue, char* szData) {
	whoOps::RegistryRhinoceros rhino;
	try {
		rhino.RegSetValue_REG_SZ(szServer, szKey, szValue, szData);
		return 0;
	} catch (const std::exception& e) {
		printf("EXCEPTION: %s\n", e.what());
		return 1;
	} catch (...) {
		return 1;
	}
}

JNIEXPORT jstring JNICALL Java_whoOps_RegistryRaptor_setREG_1SZNative(JNIEnv * env, jobject obj, jstring input) {
	std::stringstream ss;	
	try {
		cyclOps::JNIJunkie jniJunkie(env, obj, input);								CYCLOPSVAR(&jniJunkie, "%p");
		std::vector<std::string> argumentVector = jniJunkie.getArgumentVector();	CYCLOPSVAR(&argumentVector, "%p");
		whoOps::RegistryRhinoceros rhino;											CYCLOPSVAR(&rhino, "%p");
		rhino.RegSetValue_REG_SZ(argumentVector[0], argumentVector[1], 
			argumentVector[2], argumentVector[3]);									CYCLOPSDEBUG("Nearly done.");
		ss << "SUCCESS";
		CYCLOPSINFO("Registry value changed.");
	} catch (const std::exception& e) {
		ss << "ERROR: " << e.what();
	} catch (...) {
		ss << "ERROR: Unknown exception caught.";
	}
	return env->NewStringUTF(ss.str().c_str());
}

extern "C" __declspec(dllexport) int dllexportWhoOpsShutdownReboot(char* szServer) {
	try {
		whoOps::ShutdownShark shark;
		shark.reboot(szServer);
		return 0;
	} catch (...) {
		CYCLOPSERROR("Error during reboot.");
		return 1;
	}
}

JNIEXPORT jstring JNICALL Java_whoOps_ShutdownShark_rebootNative(JNIEnv * env, jobject obj, jstring input) {
	std::stringstream ss;
	try {
		cyclOps::JNIJunkie jniJunkie(env, obj, input);								CYCLOPSVAR(&jniJunkie, "%p");
		std::vector<std::string> argumentVector = jniJunkie.getArgumentVector();	CYCLOPSVAR(&argumentVector, "%p");
		whoOps::ShutdownShark shark;
		shark.reboot(argumentVector[0]);
		ss << "SUCCESS";
		CYCLOPSINFO("%s rebooted.", argumentVector[0].c_str());
	} catch (...) {
		CYCLOPSERROR("Exception caught.");
		ss << "ERROR";
	}
	return env->NewStringUTF(ss.str().c_str());
}

extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsServiceStop(const char* szServer, const char* szService, bool boWait, int iTimeout, char* szResult, int iSize) {
	/* I have handled all possible exceptions in the following method, no need for try/catch here. */
	return whoOpsServiceStopOrStart(WHOOPS_STOP, szServer, szService, boWait, iTimeout, szResult, iSize);
}

WHOOPS_RESULT whoOpsServiceStopOrStart(WHOOPS_SERVICE_ACTION action, const char* szServer, const char* szService, bool boWait, int iTimeout, char* szResult, int iSize) { CYCLOPSDEBUG("Hello.");
	std::stringstream ss;
	WHOOPS_RESULT result = WHOOPS_NO_STATUS;
	try {
		DWORD dwWaitState;
		switch (action) {
			case WHOOPS_STOP: 
				::whoOpsServiceStop(szServer, szService); 
				dwWaitState = SERVICE_STOPPED;
				break;
			case WHOOPS_START: 
				::whoOpsServiceStart(szServer, szService); 
				dwWaitState = SERVICE_RUNNING;
				break;
			default: 
				WHOOPS_THROW_EXCEPTION("The specified service action '%d' is not recognized.", action);
		}
		if (boWait) {
			::whoOpsServiceWaitUntil(szServer, szService, iTimeout, dwWaitState);
		}
		result = WHOOPS_SUCCESS;
	} catch (const std::exception& e) { 
		ss << "An exception occured: [" << typeid(e).name() << " - " << e.what() << "]";
		result = WHOOPS_EXCEPTION_STD;
	} catch (...) {
		ss << "An unknown exception occurred.";
		result = WHOOPS_EXCEPTION_UNKNOWN;
	}
	if (szResult != NULL) {
		std::string strMessage = ss.str();
		_snprintf_s(szResult, iSize, _TRUNCATE, "%s", strMessage.c_str());
	}
	return result;
}

extern "C" __declspec(dllexport) char* dllexportWhoOpsGetMessageForResult(WHOOPS_RESULT result) {
	switch (result) {
	/* This is what a return result should be initialized to. */
	case WHOOPS_NO_STATUS:				return "No return status was set (programming error)."; break; 
	case WHOOPS_SUCCESS:				return "The command completed succesfully."; break;
	case WHOOPS_ERROR_UNSPECIFIED:		return "An unspecified error occurred."; break;
	case WHOOPS_EXCEPTION_STD:			return "An std::exception was caught."; break;
	case WHOOPS_EXCEPTION_UNKNOWN:		return "An unknown exception occurred."; break;
	case WHOOPS_ERROR_NOT_IMPLEMENTED:	return "A called function or method is not implemented."; break;
	default:							return "Result code not found in message list.";
	}
}

extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsServiceStart(const char* szServer, const char* szService, bool boWait, int iTimeout, char* szMessage, int iSize) {
	/* I have handled all possible exceptions in the following method, no need for try/catch here. */
	return whoOpsServiceStopOrStart(WHOOPS_START, szServer, szService, boWait, iTimeout, szMessage, iSize);
}

extern "C" __declspec(dllexport) void dllexportWhoOpsSetDebug(bool boDebug) {
	::g_boCyclOpsDebug = boDebug;
}

char* whoOpsErrorGetLastMessage(void) {
	DWORD dwLastError = ::GetLastError();
	return ::whoOpsErrorFormatMessage(dwLastError);
}

extern "C" __declspec(dllexport) void dllexportWhoOpsServiceQueryServiceStatus(const char* szServer, const char* szService, char* szStatus, size_t iSize) {
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	try {
		::whoOpsServiceQueryServiceStatus(wszServer, wszService, szStatus, iSize);	
	} catch (const std::exception& e) {
		CYCLOPSERROR("Exception while getting service status: %s - %s", typeid(e).name(), e.what());
	} catch (...) {
		CYCLOPSERROR("Unknown exception while getting service status.");
	}
}

extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsServiceGetBinaryPathName(
	const char* szServer, const char* szService, char* szBinaryPathName, size_t iSize) 
{
	try {
		string strPath = whoOpsServiceGetBinaryPathName(szServer, szService);
		_snprintf_s(szBinaryPathName, iSize, _TRUNCATE, "%s", strPath.c_str());
		return WHOOPS_SUCCESS;
	} catch (const std::exception e) {
		CYCLOPSERROR("Exception caught: %s - %s", typeid(e).name(), e.what());
		return WHOOPS_EXCEPTION_STD;
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
		return WHOOPS_EXCEPTION_UNKNOWN;
	}
}

string whoOpsServiceGetBinaryPathName(const char* szServer, const char* szService) {
	QUERY_SERVICE_CONFIG* pQueryServiceConfig = whoOpsServiceQueryServiceConfig(szServer, szService);
	wstring wstrPath(pQueryServiceConfig->lpBinaryPathName);
	string strPath(wstrPath.begin(), wstrPath.end());
	strPath.erase(std::remove(strPath.begin(), strPath.end(), '"'), strPath.end());
	LocalFree(pQueryServiceConfig);
	return strPath;
}

QUERY_SERVICE_CONFIG* whoOpsServiceQueryServiceConfig(const char* szServer, const char* szService) {
	/* http://msdn.microsoft.com/en-us/library/windows/desktop/ms684928(v=vs.85).aspx */
	SC_HANDLE serviceHandle = ::whoOpsServiceGetHandle(szServer, szService);
	QUERY_SERVICE_CONFIG* pQueryServiceConfig;
	DWORD dwBytesNeeded;
	QueryServiceConfig(serviceHandle, NULL, 0, &dwBytesNeeded);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		CloseServiceHandle(serviceHandle);
		WHOOPS_THROW_EXCEPTION("QueryServiceConfig() failed.");
	}
	DWORD dwBufferSize = dwBytesNeeded;
	pQueryServiceConfig = (QUERY_SERVICE_CONFIG*) LocalAlloc(LMEM_FIXED, dwBufferSize);
	if (!QueryServiceConfig(serviceHandle, pQueryServiceConfig, dwBufferSize, &dwBytesNeeded)) {
		char* szMessage = ::whoOpsErrorGetLastMessage();
		CloseServiceHandle(serviceHandle);
		WHOOPS_THROW_EXCEPTION("QueryServiceConfig() failed with the following: '%s'", szMessage);
	}
	CloseServiceHandle(serviceHandle);
	return pQueryServiceConfig;
}

extern "C" __declspec(dllexport) bool dllexportWhoOpsServiceIsServiceStopped(const char* szServer, const char* szService) {
	/* We are going to allow this guy to throw exceptions. */
	wchar_t wszServer[5000];
	_snwprintf_s(wszServer, CYCLOPSSIZEOF(wszServer), _TRUNCATE, L"%S", szServer);
	wchar_t wszService[5000];
	_snwprintf_s(wszService, CYCLOPSSIZEOF(wszService), _TRUNCATE, L"%S", szService);
	return whoOpsServiceIsServiceStopped(wszServer, wszService);
}

bool whoOpsServiceIsServiceStopped(const wchar_t* wszServer, const wchar_t* wszService) {
	DWORD dwCurrentState = ::whoOpsServiceGetCurrentState(wszServer, wszService);
	return dwCurrentState == SERVICE_STOPPED;
}

DWORD whoOpsServiceGetCurrentState(const wchar_t* wszServer, const wchar_t* wszService) {
	SERVICE_STATUS serviceStatus;
	::whoOpsServiceQueryServiceStatusII(wszServer, wszService, &serviceStatus);
	return serviceStatus.dwCurrentState;
}

void whoOpsServiceStopStopStopStopStop(const wchar_t* wszServer, const wchar_t* wszService) {
	for (int i = 0; i < 5 && !whoOpsServiceIsServiceStopped(wszServer, wszService); ++i) { 
		CYCLOPSINFO("Attempt %d to stop service.", i);
		try {
			::whoOpsServiceStop(wszServer, wszService);
		} catch (const whoOps::ExceptionServiceManagement e) {
			DWORD error = e.getLastError();
			if (error == ERROR_SERVICE_CANNOT_ACCEPT_CTRL) {
				CYCLOPSDEBUG("Service is pending stop probably.");
			} else {
				throw;
			}
		}
	}
}

JNIEXPORT jstring JNICALL Java_whoOps_ProcessOpotamus_createProcessIINative (JNIEnv * env, jobject obj, jstring input) {
	string strResponseXML = "<?xml version='1.0' encoding='utf-8' ?><root><status>UNKNOWN</status><message>The results of CreateProcess() are unknown.</message><exitCode>666</exitCode></root>";
	try {
		cyclOps::JNIRequest jniRequest(env, obj, input);
		whoOps::ProcessPrincess processPrincess;
		cyclOps::JNIResponse jniResponse = processPrincess.CreateProcessIII(jniRequest); CYCLOPSDEBUG("CreateProcessIII() done.");
		strResponseXML = jniResponse.getPropertyMapAsXMLString().c_str();
	} catch (...) {
		printf("************UNKNOWN EXCEPTION CAUGHT!!!************");
		strResponseXML = "<?xml version='1.0' encoding='utf-8' ?><root><status>EXCEPTION</status><message>An unknown exception was caught in Java_whoOps_ProcessOpotamus_createProcessIINative().</message><exitCode>666</exitCode></root>";
	}
	CYCLOPSDEBUG("strResponseXML = %s", strResponseXML.c_str());
	return env->NewStringUTF(strResponseXML.c_str());
}

extern "C" __declspec(dllexport) bool dllexportWhoOpsRegOpenKeyEx(const char * szServer, const char * szKey) {
	whoOps::RegistryRhinoceros rhino;
	try {
		rhino.RegOpenKeyEx(szServer, szKey, KEY_ALL_ACCESS);
		return true;
	} catch (const std::exception& e) {
		printf("Exception caught: %s - %s\n", typeid(e).name(), e.what());
		return false;
	} catch (...) {
		printf("Unknown exception caught.\n");
		return false;
	}
}

void whoOpsAddBackslashesToServerIfNecessary(wchar_t* wszServerWithBackslashes, size_t iSize, const wchar_t* wszServerOriginal) {
	wchar_t* wszPrefix = L"\\\\";
	if (wcsncmp(L"\\\\", wszServerOriginal, 2) == 0) {
		wszPrefix = L"";
	}
	_snwprintf_s(wszServerWithBackslashes, iSize, _TRUNCATE, L"%s%s", wszPrefix, wszServerOriginal);
}

extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsCreateProcessQnD(const char* szCommandLine) {
	try {
		whoOps::ProcessPrincess princess;
		princess.CreateProcessQnD(szCommandLine);
		return WHOOPS_SUCCESS;
	} catch (const std::exception& e) { 
		CYCLOPSERROR("%s - %s", typeid(e).name(), e.what());
		return WHOOPS_EXCEPTION_STD;
	} catch (...) {
		CYCLOPSERROR("Unknown Exception.");
		return WHOOPS_EXCEPTION_UNKNOWN;
	}
}

extern "C" __declspec(dllexport) WHOOPS_RESULT dllexportWhoOpsNetSessionDel(const char* szHost, const char* szTarget, const WHOOPS_SESSION_TYPE sessionType) {
	try {
		whoOps::SessionSuckah session(szHost);
		session.NetSessionDel(szTarget, WHOOPS_CLIENT);
		return WHOOPS_SUCCESS;
	} catch (const std::exception& e) { 
		CYCLOPSERROR("%s - %s", typeid(e).name(), e.what());
		return WHOOPS_EXCEPTION_STD;
	} catch (...) {
		CYCLOPSERROR("Unknown Exception.");
		return WHOOPS_EXCEPTION_UNKNOWN;
	}

}

JNIEXPORT jstring JNICALL Java_whoOps_ShareshankRedemption_getPathNative(JNIEnv * env, jobject obj, jstring input) {
	cyclOps::JNIResponse response;
	response.setStatus("UNKNOWN");
	response.setMessage("The results of getPath() are as yet unknown.");
	try {
		cyclOps::JNIRequest request(env, obj, input);
		whoOps::ShareshankRedemption share(request);
		string strPath = share.getPath(); CYCLOPSINFO("strPath = %s", strPath.c_str());
		response.putProperty("path", "%s", strPath.c_str()); CYCLOPSINFO("putProperty() done.");
		response.setStatus("SUCCESS"); CYCLOPSINFO("Path %s succesfully gotten.", strPath.c_str());
	} catch (const std::exception& e) {
		response.setStatus("EXCEPTION");
		stringstream ss; ss << typeid(e).name() << " - " << e.what();
		response.setMessage(ss.str());
	} catch (...) {
		response.setStatus("EXCEPTION");
		response.setMessage("An unknown exception occurred.");
	}
	return env->NewStringUTF(response.getPropertyMapAsXMLString().c_str());
}

JNIEXPORT jstring JNICALL Java_whoOps_ShareshankRedemption_netShareEnumNative(JNIEnv * env, jobject obj, jstring input) {
	whoOps::JNIResponseShareshankRedemption response;
	response.setStatus("UNKNOWN");
	response.setMessage("The results of netShareEnum() are as yet unknown.");
	try {
		cyclOps::JNIRequest request(env, obj, input);
		whoOps::ShareshankRedemption server(request);
		server.setContinueOnLookupAccountSidFailure(request.getPropertyAs_bool("continueOnLookupAccountSIDFailure", false));
		server.netShareEnum(response);
		response.setStatus("SUCCESS");
	} catch (const std::exception& e) {
		response.setStatus("EXCEPTION");
		stringstream ss; ss << typeid(e).name() << " - " << e.what();
		response.setMessage(ss.str());
	} catch (...) {
		response.setStatus("EXCEPTION");
		response.setMessage("An unknown exception occurred.");
	}
	return env->NewStringUTF(response.getPropertyMapAsXMLString().c_str());
}

JNIEXPORT jstring JNICALL Java_whoOps_ShareshankRedemption_getShareBeanNative(JNIEnv * env, jobject obj, jstring input) {
	whoOps::JNIResponseShareshankRedemption response;
	response.setStatus("UNKNOWN");
	response.setMessage("The results of getShare() are as yet unknown.");
	try {
		cyclOps::JNIRequest request(env, obj, input);
		whoOps::ShareshankRedemption share(request);
		share.setContinueOnLookupAccountSidFailure(request.getPropertyAs_bool("continueOnLookupAccountSIDFailure", false));
		share.getShare(response);
		response.setStatus("SUCCESS");
	}
	catch (const std::exception& e) {
		response.setStatus("EXCEPTION");
		stringstream ss; ss << typeid(e).name() << " - " << e.what();
		response.setMessage(ss.str());
	}
	catch (...) {
		response.setStatus("EXCEPTION");
		response.setMessage("An unknown exception occurred.");
	}
	return env->NewStringUTF(response.getPropertyMapAsXMLString().c_str());

}


JNIEXPORT jstring JNICALL Java_whoOps_ShareshankRedemption_setRemarkNative(JNIEnv *env, jobject obj, jstring input) {
	whoOps::JNIResponseShareshankRedemption response;
	response.setStatus("UNKNOWN");
	response.setMessage("The results of setRemark() are as yet unknown.");
	try {
		cyclOps::JNIRequest request(env, obj, input);
		g_boCyclOpsDebug = request.getDebug();
		whoOps::ShareshankRedemption server(request);
		string strRemark = request.getProperty("remark");
		server.setRemark(strRemark);
		response.setStatus("SUCCESS");
		response.setMessage("setRemark() was called successfully.");
	} catch (const std::exception& e) { CYCLOPSDEBUG("%s caught.", typeid(e).name());
		response.setStatus("EXCEPTION");
		stringstream ss; ss << typeid(e).name() << " - " << e.what();
		response.setMessage(ss.str());
	} catch (...) {
		response.setStatus("EXCEPTION");
		response.setMessage("An unknown exception occurred.");
	}
	string strResponse = response.getPropertyMapAsXMLString().c_str();
	return env->NewStringUTF(strResponse.c_str());
}


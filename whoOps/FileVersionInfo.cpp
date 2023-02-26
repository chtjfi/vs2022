#include "StdAfx.h"
#include <stdio.h>

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include <windows.h>

#include "FileVersionInfo.h"
//#include "whoOps.h"
#include "whoOpsPrivate.h"
#include "ExceptionGeneric.h"
#include "..\cyclOps\cyclOps.h"


whoOps::FileVersionInfo::FileVersionInfo(void)
{
}


whoOps::FileVersionInfo::~FileVersionInfo(void)
{
}

void whoOps::FileVersionInfo::getFileVersionInfo(const char* szFile, char* szVersion,
	size_t iSize, bool boDebug) 
{
	CYCLOPSVAR(szFile, "%s");
	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSizeA(szFile, &dwHandle);
	if (dwSize == 0) {
        DWORD dwLastError = ::GetLastError();
		char szMessage[1000];
		_snprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE,
			"GetFileVersionInfoSize() failed.  %s", whoOpsErrorFormatMessage(dwLastError));
		throw whoOps::ExceptionGeneric(szMessage);
	}
	BYTE* p_VerInfo = new BYTE[dwSize];
	if (GetFileVersionInfoA(szFile, 0, dwSize + 1, (LPVOID) p_VerInfo) == 0 ) {
        DWORD dwLastError = ::GetLastError();
		char szMessage[1000];
        _snprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE,
			"GetFileVersionInfo() failed.  %s", ::whoOpsErrorFormatMessage(dwLastError));
		throw whoOps::ExceptionGeneric(szMessage);
	}
	VS_FIXEDFILEINFO* p_stFixedFileInfo;
	UINT iLengthBuf;
	if (!VerQueryValueA(p_VerInfo, "\\", (LPVOID*) &p_stFixedFileInfo, &iLengthBuf)) {
        DWORD dwLastError = GetLastError();
		char szMessage[1000];
        _snprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), _TRUNCATE,
			"VerQueryValue() failed.  %s", ::whoOpsErrorFormatMessage(dwLastError));
		throw whoOps::ExceptionGeneric(szMessage);
	}
	CYCLOPSDEBUG(
        "FileVersion=%d.%d.%d.%d  "
        "ProductVersion=%d.%d.%d.%d\n",
        HIWORD(p_stFixedFileInfo->dwFileVersionMS),
        LOWORD(p_stFixedFileInfo->dwFileVersionMS),
        HIWORD(p_stFixedFileInfo->dwFileVersionLS),
        LOWORD(p_stFixedFileInfo->dwFileVersionLS),
        HIWORD(p_stFixedFileInfo->dwProductVersionMS),
        LOWORD(p_stFixedFileInfo->dwProductVersionMS),
        HIWORD(p_stFixedFileInfo->dwProductVersionLS),
        LOWORD(p_stFixedFileInfo->dwProductVersionLS)
	);
	_snprintf_s(szVersion, iSize, _TRUNCATE, 
        "%d.%d.%d.%d",
        HIWORD(p_stFixedFileInfo->dwFileVersionMS),
        LOWORD(p_stFixedFileInfo->dwFileVersionMS),
        HIWORD(p_stFixedFileInfo->dwFileVersionLS),
        LOWORD(p_stFixedFileInfo->dwFileVersionLS)
	);
	if ((p_stFixedFileInfo->dwFileVersionMS
        != p_stFixedFileInfo->dwProductVersionMS)
        || (p_stFixedFileInfo->dwFileVersionLS
        != p_stFixedFileInfo->dwProductVersionLS))
	{
        CYCLOPSWARNING("Versions don't match");
	}
}
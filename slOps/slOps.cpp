// slOps.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <WinSock2.h>
#include "slOps.h"
#include "slOpsPrivate.h"
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include "ExceptionFatale.h"
#include "..\cyclOps\cyclOps.h"
#include "ExceptionSourceFileMissing.h"
#include "Synculator.h"

using std::stringstream;
using std::wstring;
using std::string;

// These functions aren't defined in slOps.h because the outside world
// will never use them.  They are private, if you will.
char* slOpsGetErrorMessage(DWORD dwError);
void slOpsSetReadWrite(const char* szFile);
void slOpsFormatErrorMessage(DWORD dwLastError, char** pp_chErrorMessage);
bool slOpsDoFilesMatch(char* szSourceFile, char* szTargetFile);
bool slOpsDoFilesMatchII(const WIN32_FIND_DATA* findDataSource, char* szTargetFile);
wstring slOpsCreatePath(const string& strPath);

bool g_boSlOpsDebug = false;
cyclOps::Logger g_slOpsLogger;

extern "C" __declspec(dllexport) bool dllexportSlOpsCopyDirectory(const char *szSourceDir, const char *szTargetDir, 
	bool boIncludeSubs, bool boPurge, bool boDebug, const char* szLogFile, bool boContinueOnCopyFailure)  { 
	try {
		g_boSlOpsDebug = boDebug; SLOPSDEBUG("szLogFile = %s", szLogFile); 
		g_boCyclOpsDebug = boDebug; 
		if (szLogFile != NULL) {
			g_slOpsLogger.initializeLogFile(szLogFile);
		} 
		slOps::Synculator synculator; SLOPSDEBUG("synculator = %p", &synculator);
		synculator.setIncludeSubdirectories(boIncludeSubs);
		synculator.setPurge(boPurge);
		synculator.setContinueOnCopyFailure(boContinueOnCopyFailure);
		synculator.sync(szSourceDir, szTargetDir);
		return synculator.isSuccesful();
	} catch (const std::exception& e) { 
		SLOPSERROR("%s - %s", e.what(), typeid(e).name());
		return false;
	} catch (...) { 
		printf("Unknown exception. [%s - %d]\n", __FILE__, __LINE__);
		return false;
	}
}

extern "C" __declspec(dllexport) bool dllexportSlOpsCopyDirectory_DEPRECATED(const char *szSourceDir, const char *szTargetDir, bool boIncludeSubs, 
	bool boPurge, bool boDebug, const char* szLogFile) 
{

	g_boSlOpsDebug = boDebug; SLOPSDEBUG("szLogFile = %s", szLogFile); 
	if (szLogFile != NULL) {
		g_slOpsLogger.initializeLogFile(szLogFile);
	} SLOPSDEBUG("Goodbye.");
	return ::slOpsCopyDirectory(szSourceDir, szTargetDir, boIncludeSubs, boPurge);
}

bool slOpsCopyDirectory(const char *szSourceDir, const char *szTargetDir, bool boIncludeSubs, bool boPurge) {
	

	/* SLOPSDEBUG("szSourceDir = %s, szTargetDir = %s, boIncludeSubs = %s, boPurge = %s)", 
		szSourceDir, szTargetDir, boIncludeSubs ? "true" : "false", boPurge ? "true" : "false");*/

	try {
		///////
		//create target sub
		if (!slOpsCreateDirectory(szTargetDir))
		{
			SLOPSERROR("ERROR: unable to create directory %s.", szTargetDir);
			return false;
		}
		wchar_t wszFileSpec[SLOPS_MAX_PATH] = { 0 };
		_snwprintf_s(wszFileSpec, SLOPSSIZEOF(wszFileSpec), _TRUNCATE, L"%S\\*.*", szSourceDir);
		SLOPSDEBUG("wszFileSpec is %S", wszFileSpec);
		WIN32_FIND_DATA stFindData;
		HANDLE hFind = FindFirstFile(wszFileSpec, &stFindData);
		BOOL boNextFile = false;
		if (hFind != INVALID_HANDLE_VALUE) {
			do  {
				/* SLOPSDEBUG("cFileName is %S", stFindData.cFileName); */
				if (wcscmp(stFindData.cFileName, L".") != 0) {
					if (wcscmp(stFindData.cFileName, L"..") != 0) {
						//if it's a directory we might have to recurse
						if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							// it's a directory
							/* SLOPSDEBUG("%S is supposedly a directory.", stFindData.cFileName); */
							char szSourceSub[SLOPS_MAX_PATH];
							_snprintf_s(szSourceSub, SLOPSSIZEOF(szSourceSub), _TRUNCATE, "%s\\%S", szSourceDir, stFindData.cFileName);
							char szTargetSub[SLOPS_MAX_PATH];
							_snprintf_s(szTargetSub, SLOPSSIZEOF(szTargetSub), _TRUNCATE, "%s\\%S", szTargetDir, stFindData.cFileName);
							bool boSuccess = ::slOpsCopyDirectory(szSourceSub, szTargetSub, boIncludeSubs, boPurge);
							if (!boSuccess) {
								SLOPSERROR("Failed to copy %s to %s.", szSourceSub, szTargetSub);
								return false;
							}
						} else {
							// it's a file
							char szTargetFile[SLOPS_MAX_PATH] = { 0 };
							_snprintf_s(szTargetFile, SLOPSSIZEOF(szTargetFile), _TRUNCATE, "%s\\%S", szTargetDir, stFindData.cFileName);
							SLOPSDEBUG("target %s", szTargetFile);
							//create source file name
							char szSourceFile[SLOPS_MAX_PATH] = { 0 };
							_snprintf_s(szSourceFile, SLOPSSIZEOF(szSourceFile), _TRUNCATE, "%s\\%S", szSourceDir, stFindData.cFileName);
							SLOPSDEBUG("source %s", szSourceFile);
							try {
								if ( ! slOpsDoFilesMatchII(&stFindData, szTargetFile)) { SLOPSDEBUG("Files don't match.");
									if ( ! ::slOpsCopyFile(szSourceFile, szTargetFile)) {
										SLOPSERROR("Unable to download %s", szTargetFile);
										return false;
									} else {
										//download succesful
										SLOPSDEBUG("...file updated succesfully.");
									}
								} else {
									SLOPSDEBUG("Files match.");
								}
							} catch (const slOps::ExceptionSourceFileMissing& e) {
								SLOPSERROR("%s - %s", typeid(e).name(), e.what());
								return false;
							} SLOPSDEBUG("Aardvark.");
						} SLOPSDEBUG("Baboon."); // "file" was a file (not a directory)
					} SLOPSDEBUG("Crocodile."); //filename was not ".."
				} SLOPSDEBUG("Dinosaur."); //filename was not "."
				boNextFile = FindNextFile(hFind, &stFindData); SLOPSDEBUG("boNextFile = %d"); 
			} while (boNextFile); SLOPSDEBUG("Elephant."); // end of do loop
		} else {
			//FindFirstFile returned an invalid handle
			DWORD dwLastError = GetLastError();
			SLOPSERROR("FindFirstFile(\"%s\") failed", wszFileSpec);
			return false;
		} SLOPSDEBUG("Flamingo.");
		if (boPurge) { SLOPSDEBUG("Purging.");
/*
			bool boResult = slOpsPurgeDirectory(szTargetDir, szSourceDir);
			if (!boResult) {
				return false;
			}
*/
		} else {
			SLOPSDEBUG("Not purging.");
		} SLOPSDEBUG("Returning true.");
		return true;
	} catch (slOps::ExceptionFatale e) {
		SLOPSERROR("Fatal exception caught while copying %s to %s.", szSourceDir, szTargetDir);
		return false;
	}
}

bool slOpsCreateDirectory(const char *szDirectory) { // SLOPSDEBUG("szDirectory = %s", szDirectory);
	/* char *szFunctionName="createDirectory"; */
	char *pChar;
	DWORD dwLastError;
	if(!CreateDirectoryA(szDirectory,NULL))	{ 	printf("WTF?\n"); 
		dwLastError=GetLastError(); printf("WTF.5?\n"); /* SLOPSDEBUG("CreateDirectory() failed."); */
		if(dwLastError == ERROR_ALREADY_EXISTS) { /* SLOPSDEBUG("....that's because it already exists!  Continuing ..."); */
			return true;
		}
		char* szError = ::slOpsGetErrorMessage(dwLastError); SLOPSDEBUG("CreateDirectory() failed with the error '%s'.", szError);
		//make the parent directory and try again
		char szParentDirectory[SLOPS_MAX_PATH];
		if((szParentDirectory[strlen(szParentDirectory) - 1]) == '\\') {
			szParentDirectory[strlen(szParentDirectory) - 1] = '\0';
		}
		strcpy_s(szParentDirectory, _TRUNCATE, szDirectory);
		pChar = strrchr(szParentDirectory,'\\');
		if (pChar == NULL) {
			//if there are no backslashes, we've reached the root
			SLOPSERROR("Reached root.  Directory creation failed.");
			throw slOps::ExceptionFatale();
		} printf("WTFIII?\n");
		*pChar='\0';
		if(!::slOpsCreateDirectory(szParentDirectory)) { printf("WTFIV?\n");
			SLOPSERROR("Couldn't create directory %s.", szDirectory);
			return false;
		} else { printf("WTFV?\n");
			SLOPSDEBUG("Parent directory created.");
			if(!CreateDirectoryA(szDirectory,NULL))
			{
				dwLastError=GetLastError();
				SLOPSERROR("Can't create %s!  Error from CreateDirectory is %d.", 
					szDirectory, dwLastError);
				return false;
			}
			else 
			{
				SLOPSDEBUG("Directory created succesfully.");
			}
		}
	} else { printf("WTFVI?\n");
		SLOPSDEBUG("Directory %s created.", szDirectory);
	}
	return true;
}

bool slOpsPurgeDirectory(const char *szTargetDir, const char *szSourceDir) { SLOPSDEBUG("{szTargetDir = %s} {szSourceDir = %s}", szTargetDir, szSourceDir);
	bool boEverythingOK = true;
	wchar_t wszFileSpec[SLOPS_MAX_PATH] = { 0 };
	_snwprintf_s(wszFileSpec, SLOPSSIZEOF(wszFileSpec), _TRUNCATE, L"%S\\*.*", szTargetDir);
	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(wszFileSpec, &stFindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (wcscmp(stFindData.cFileName, L".") != 0) {
				if (wcscmp(stFindData.cFileName, L"..") != 0) {
					//whatever it is, file or directory, if it ain't in source remove it!!
					char szTargetFile[SLOPS_MAX_PATH] = { 0 };
					_snprintf_s(szTargetFile, SLOPSSIZEOF(szTargetFile), _TRUNCATE, "%s\\%S", szTargetDir, stFindData.cFileName);
					//create source file name
					char szSourceFile[SLOPS_MAX_PATH] = { 0 };
					_snprintf_s(szSourceFile, SLOPSSIZEOF(szSourceFile), _TRUNCATE, "%s\\%S", szSourceDir, stFindData.cFileName);
					//does source exists?
					SLOPSDEBUG("checking if source file exists");
					//get _stat for source
					struct _stat stSourceStat;
					int iReturn = _stat(szSourceFile, &stSourceStat);
					if (iReturn != 0) {
						//source file  not there - purge target
						SLOPSDEBUG("source file does not exist, purging!");
						//regardless of file or directory, we must remove readonly
						//attribute 
						::slOpsSetReadWrite(szTargetFile);
						//is it a file or a directory?
						if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							// it's a directory
							SLOPSDEBUG("%s is a directory.  ", szTargetFile);
							bool boCool = ::slOpsRemoveDirectory(szTargetFile, true);
							if (!boCool) {
								SLOPSERROR(" Unable to remove directory %s", szTargetFile);
								boEverythingOK = false;
							} else {
								SLOPSDEBUG("succesfully purged.  Hooray!");
							}
						} else {
							// it's a file
							BOOL boCool = DeleteFileA(szTargetFile);
							if (!boCool) {
								SLOPSERROR(" Unable to delete file %s", szTargetFile);
								boEverythingOK = false;
							} else {
								SLOPSDEBUG("...succesfully purged.  Hooray!");
							}
						}
					} else {
						SLOPSDEBUG("...source file exists, not purging.");
					}
				} //filename was not ".."
			} //filename was not "."
		}  //do loop
		while (FindNextFile(hFind, &stFindData));
	} //FindFirstFile returned a valid handle
	return boEverythingOK;
}

bool slOpsCopyFile(const char *szSourceFile, const char *szTargetFile) {
	SLOPSDEBUG("download(\"%s\", \"%s\"", szSourceFile, szTargetFile);
	::slOpsSetReadWrite(szTargetFile);
	/* In the ANSI version of CopyFileEx, the name is limited to MAX_PATH characters. To extend this limit to 32,767 wide characters, call the Unicode version of the function and prepend "\\?\" to the path. */
	wstring wstrSourceFile = slOpsCreatePath(szSourceFile); SLOPSDEBUG("wstrSourceFile = %S", wstrSourceFile.c_str());
	wstring s1 = L"\\\\?\\UNC\\dcvprdctxdfs01\\XenApp\\PushAppSource\\Reuters_KTP\\6.1.3.L2.SP2.23\\SB01\\KTPDEV\\Groupe_Front\\Flex\\curve\\srcview\\source\\bin-release1\\Copy of curve\\srcview\\source\\bin-release1\\DataBasingFB\\ANewFlexDataDay\\src\\services\\datatestservice\\_Super_DatatestService.as.html"; SLOPSDEBUG("s1 = %S", s1.c_str());
	wstring wstrTargetFile = slOpsCreatePath(szTargetFile);	SLOPSDEBUG("wstrTargetFile = %S", wstrTargetFile.c_str());
	BOOL boReturn = ::CopyFileExW(s1.c_str(), wstrTargetFile.c_str(), NULL, NULL, NULL, 0); 
	if (!boReturn) {
		DWORD dwLastError = GetLastError();
		char* p_chError = NULL;
		char* szError = ::slOpsGetErrorMessage(dwLastError);
		SLOPSERROR("Unable to download %s (%s)", szSourceFile, szError);
		if (p_chError != NULL) {
			free(p_chError);
		}
		return false;
	}
	return true;
}

char* slOpsGetErrorMessage(DWORD dwError)
{
   char* szMessage = NULL;
   if (::FormatMessageA(
           FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,
           dwError,
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
           reinterpret_cast<char*>(&szMessage),
           0, NULL) && (szMessage != NULL))
   {
           szMessage = cyclOpsReplace(szMessage, "\r\n", "");
   }
   else
   {
      szMessage = "unknown error";
   }

   return szMessage;
}

void slOpsSetReadWrite(const char* szFile)
{
    DWORD dwAttrs = GetFileAttributesA(szFile); 
	if (dwAttrs != 0xFFFFFFFF)
	{
		if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) 
		{ 
			SetFileAttributesA(szFile, 
				dwAttrs & (~FILE_ATTRIBUTE_READONLY)); 
		} 
	}    
}

bool slOpsRemoveDirectory(const char* szDirectory, bool boDeleteReadOnly)
{
	//delete all files 
	wchar_t wszFileSpec[SLOPS_MAX_PATH] = { 0 };
	_snwprintf_s(wszFileSpec, SLOPSSIZEOF(wszFileSpec), _TRUNCATE, L"%S\\*.*", szDirectory);
	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(wszFileSpec, &stFindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (wcscmp(stFindData.cFileName, L".") != 0)
			{
				if (wcscmp(stFindData.cFileName, L"..") != 0)
				{
					char szTarget[SLOPS_MAX_PATH];
					_snprintf_s(szTarget, SLOPSSIZEOF(szTarget), _TRUNCATE, "%s\\%S", szDirectory, stFindData.cFileName);
					//whatever it is, we should remove readonly attribute
					//if requested by caller
					if (boDeleteReadOnly) {
						::slOpsSetReadWrite(szTarget);
					}
					//if it's a directory we have to recurse
					if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						::slOpsRemoveDirectory(szTarget, boDeleteReadOnly);
					}
					else
					{
						DeleteFileA(szTarget);
					} //file was not a directory
				} //filename was not ".."
			} //filename was not "."
		}  //do loop
		while (FindNextFile(hFind, &stFindData));
		FindClose(hFind);
	} //FindFirstFile returned a valid handle
	BOOL boReturn = RemoveDirectoryA(szDirectory);
	if (boReturn)
	{
		return true;
	}
	else {
		//DWORD dwLastError = GetLastError();
		//char* p_chLastError;
		//frogFormatErrorMessage(dwLastError, &p_chLastError);
		return false;
	}
}

void slOpsFormatErrorMessage(DWORD dwLastError, char** pp_chErrorMessage)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	*pp_chErrorMessage = _strdup((char*) lpMsgBuf);
	//replace newlines with spaces
	char* pChar;
	while ((pChar = strpbrk(*pp_chErrorMessage, "\r\n")) != NULL){
		*pChar = ' ';
	}
	LocalFree( lpMsgBuf );
}

bool slOpsDoFilesMatch(char* szSourceFile, char* szTargetFile) {
	struct _stat stTargetStat;
	int iReturn = _stat(szTargetFile, &stTargetStat);
	char tarTimebuf[26];
	ctime_s(tarTimebuf, 26, &stTargetStat.st_mtime);
	tarTimebuf[24] = '\0';
	if (iReturn != 0) {
		//target file doesn't exist
		return false;
	} else {
		// target exits, compare it to source
		SLOPSDEBUG("target exists");
		struct _stat stSourceStat;
		iReturn = _stat(szSourceFile, &stSourceStat);
		char srcTimebuf[26];
		ctime_s(srcTimebuf, 26, &stSourceStat.st_mtime);
		srcTimebuf[24] = '\0';
		if (iReturn != 0) {
			//source file  not there??  whoa!
			SLOPSERROR("ERROR: source file missing!!! %s", szSourceFile);
			stringstream ss;
			ss << "The source file " << szSourceFile << " is missing.";
			throw slOps::ExceptionSourceFileMissing(ss.str());
		} else {
			// source file exists, compare file dates
			SLOPSDEBUG("mod time for source file: %s, %u", srcTimebuf, stSourceStat.st_mtime);
			SLOPSDEBUG("mod time for target file: %s, %u", tarTimebuf, stTargetStat.st_mtime);
			bool boGood = false;
			if (stSourceStat.st_mtime == stTargetStat.st_mtime) {
				boGood = true;
			} if (boGood) {
				SLOPSDEBUG("...file is good.");
				return true;
			} else {
				SLOPSDEBUG("debug: ...file times don't match...");
				__time64_t dt = stSourceStat.st_mtime - stTargetStat.st_mtime;
				time_t stDiff = abs((int) dt);
				SLOPSDEBUG("...difference is %d...", stDiff);
				int iTolerance = 3;
				if (stDiff > iTolerance) {
					SLOPSDEBUG("...difference is greater than tolerance value...");
					SLOPSDEBUG("...%s requires update...", szTargetFile);
					return false;
				} else {
					SLOPSDEBUG("...difference is within tolerance value - not downloading.");
					return true;
				}
			} //file dates did not match
		} //_stat on source file succeeded (it existed)
	} // stat succeeded on target (it existed)
}

bool slOpsDoFilesMatchII(const WIN32_FIND_DATA* pFindDataSource, char* szTargetFile) {
	wchar_t wszTargetFile[5000];
	CYCLOPS_SNWPRINTF_S(wszTargetFile, L"%S", szTargetFile);
	WIN32_FIND_DATA findDataTarget;
	HANDLE hFindTarget = FindFirstFile(wszTargetFile, &findDataTarget);
	if (hFindTarget == INVALID_HANDLE_VALUE) {
		return false;
	}
	FILETIME file_time1 = pFindDataSource->ftLastWriteTime;
	FILETIME file_time2 = findDataTarget.ftLastWriteTime;
	LONGLONG diffInTicks =
		((LARGE_INTEGER*)(&file_time1))->QuadPart -
		((LARGE_INTEGER*)(&file_time2))->QuadPart;
	LONGLONG diffInMillis = diffInTicks / 10000000; SLOPSDEBUG("diffInMillis = %d\n", diffInMillis);
	if (diffInMillis > 3000 || diffInMillis < -3000) { SLOPSDEBUG("Returning false.");
		return false;
	} else { SLOPSDEBUG("Returning true.");
		return true;
	} 
}

wstring slOpsCreatePath(const string& strPath) {
	string strTemp(strPath); SLOPSDEBUG("strTemp = %s", strTemp.c_str());
	if (strTemp.length() > 250) { 
		if (strTemp.compare(0, 2, "\\\\") == 0) {
			/* Remove the double backslash. */
			strTemp.erase(0, 2); SLOPSDEBUG("strTemp = %s", strTemp.c_str());
			/* Add UNC\ to beginning. (Yes I could have just removed one backslash in the last step but I think this documents better. */
			strTemp = "UNC\\" + strTemp; SLOPSDEBUG("strTemp = %s", strTemp.c_str());
		}
		strTemp = "\\\\?\\" + strTemp; SLOPSDEBUG("strTemp = %s", strTemp.c_str());
	}
	return wstring(strTemp.begin(), strTemp.end());
}
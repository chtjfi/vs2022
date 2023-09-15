#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>
#include <exception>

#include "cyclOps.h"

bool g_boCyclOpsDebug = false;
bool g_boCyclOpsDebugPrevious = false;
char* g_szCyclOpsDebugFile = 0;
char* g_szCyclOpsDebugFunction = 0;

bool cyclOpsLogDebugFilter(const char* szLevel, const char* szFile, const char* szFunction) {
	if (_stricmp(szLevel, "DEBUG") == 0) {
		if (g_szCyclOpsDebugFile != 0) {
			if (_stricmp(g_szCyclOpsDebugFile, szFile) == 0) {
				return true;
			}
		}
		if (g_szCyclOpsDebugFunction != 0) {
			if (_stricmp(g_szCyclOpsDebugFunction, szFunction) == 0) {
				return true;
			}
		}
		return false;
	} else {
		return true;
	}
}

void cyclOpsLogToConsoleQND(const char* szLevel, const char* szFile, const char* szFunction, int iLine, 
	const char* szFormat, ...) { 
	/* if ( ! cyclOpsLogDebugFilter(szLevel, szFile, szFunction)) {
		return;
	} */
//printf("%s/%s/%s/%d/%s\n", szLevel, szFile, szFunction, iLine, szFormat);
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	const int iSize = 5000;
	char szMessage[iSize];
	_vsnprintf_s(szMessage, sizeof(szMessage) / sizeof(szMessage[0]) - 1, _TRUNCATE, szFormat, vaArguments);
	bool boTruncated = strlen(szMessage) >= iSize - 1;
	std::string strTimeString = ::cyclOpsGetISOTimeIII();
	if (_stricmp(szLevel, "INFO") == 0) {
		printf_s("%s INFO: %s %s\n", strTimeString.c_str(), szMessage, 
			boTruncated ? "[TRUNCATED]" : "");
	} else {
		printf("%s %s: %s %s [%s - %s(%d)]\n", strTimeString.c_str(), szLevel,
			szMessage, boTruncated ? "[TRUNCATED]" : "", szFile, szFunction, iLine);
	}
}

void cyclOpsLogToConsoleQNDW(wchar_t* wszLevel, char* szFile, char* szFunction, int iLine, wchar_t* wszFormat, ...) { 
	/* if ( ! cyclOpsLogDebugFilter(szLevel, szFile, szFunction)) {
		return;
	} */
	va_list vaArguments;
	va_start(vaArguments, wszFormat);
	const int iSize = 5000;
	wchar_t wszMessage[iSize];
	_vsnwprintf_s(wszMessage, sizeof(wszMessage) / sizeof(wszMessage[0]), _TRUNCATE, wszFormat, vaArguments);
	bool boTruncated = wcslen(wszMessage) >= iSize - 1;
	std::string strTimeString = ::cyclOpsGetISOTimeIII();
	if (_wcsicmp(wszLevel, L"INFO") == 0) {
		wprintf_s(L"%S INFO: %s %s\n", strTimeString.c_str(), wszMessage, 
			boTruncated ? L"[TRUNCATED]" : L"");
	} else {
		wprintf(L"%S %s: %s %s [%S - %S(%d)]\n", strTimeString.c_str(), wszLevel,
			wszMessage, boTruncated ? L"[TRUNCATED]" : L"", szFile, szFunction, iLine);
	}
}


/* RETIRED:  Use CommandLinebacker instead. 
std::string cyclOpsGetCommandLineOptionIII(int argc, char* argv[], char* pchOption) {
	char szValue[5000];
	if (!::cyclOpsGetCommandLineOption(argc, argv, pchOption, szValue, CYCLOPSSIZEOF(szValue))) {
		CYCLOPSDEBUG("cyclOpsGetCommandLineOption() returned false.");
		std::stringstream ss;
		ss << "Command line argument " << pchOption << " not found.";
		throw cyclOps::ExceptionOptionNotSet(ss.str().c_str());
	}
	return std::string(szValue);
}
*/
/* RETIRED:  Use CommandLinebacker instead. 
std::string cyclOpsGetCommandLineOptionII(int argc, char* argv[], char* pchOption) {
	char szValue[5000];
	if (!::cyclOpsGetCommandLineOption(argc, argv, pchOption, szValue, CYCLOPSSIZEOF(szValue))) {
		CYCLOPSDEBUG("cyclOpsGetCommandLineOption() returned false.");
		// CYCLOPS_THROW_EXCEPTION("The command line option %s was not found.", pchOption);
		throw 1;
	}
	return std::string(szValue);
}
*/

/* RETIRED:  Use CommandLinebacker instead. 
bool cyclOpsGetCommandLineOption(int argc, char* argv[], char* pchOption, char* szValue, int iValueSize) 
{

	// Create the full option string.  For example, if pchOption points at "booble", then 
	// the full option string is "/booble".  A trailing colon will
	// be appended depending on if pchValue is NULL or not.  If it is NULL, no return value
	// is being sought and so no colon is used to separate option from value.  However, if pchValue
	// is not NULL, then we append a colon, because it separates the option from the value.  OK?
	char szFullOption[500];
	sprintf_s(szFullOption, CYCLOPSSIZEOF(szFullOption), "/%s%s", pchOption, 
		szValue == NULL ? "" : ":");

	// Make the option lowercase so we can match the strlwr'ed arguments.
	_strlwr_s(szFullOption, CYCLOPSSIZEOF(szFullOption));

	// Loop through the command line arguments looking for the option.
	for (int i = 0; i < argc; ++i) {
		// Because we are going to strlwr the string, I feel more comfortable working on 
		// a copy of it, which is what we create here...
		char szArgvI[5000];
		size_t iSize = CYCLOPSSIZEOF(szArgvI);
		sprintf_s(szArgvI, iSize, "%s", argv[i]);
		_strlwr_s(szArgvI, CYCLOPSSIZEOF(szArgvI));
		// If szValue was NULL, szFullOption must match szArgvI exactly.
		if (szValue) {
			// If szValue was not NULL, szArgvI should *begin* with szFullOption for 
			// this to be a match, because we are expecting a return value.
			char* pchLocation = strstr(szArgvI, szFullOption);
			if (pchLocation == szArgvI) {
				// At this point we know szValue cannot be NULL because we checked for that
				// a few lines back.  So a value is expected to be returned.
				// We want to use the original argv[i] because szArgvI has been strlwr'ed
				// and maybe we need the real case of the argument?
				char* pchColon = strstr(argv[i], ":");
				sprintf_s(szValue, iValueSize, "%s", pchColon + 1);
				return true;
			}
		} else {
			// Otherwise, szValue was NULL, no return value is expected, and we return tru
			// only if there is an EXACT match between szArgvI and szFullOption.
			if (_stricmp(szArgvI, szFullOption) == 0) {
				return true;
			} 
		}
	}
	return false;
}
*/

bool cyclOpsGetCommandLineOptionClassic(char* pchOption, char** ppchValue)
{
	// I have decided it is safer to create new char buffers to hold
	// the arguments passed in.
	size_t iOptionBufferSize = strlen(pchOption) + 1;
	char* szOption = new char[iOptionBufferSize];
	CYCLOPSDEBUG("sizeof(szOption): %d", sizeof(szOption));
	sprintf_s(szOption, iOptionBufferSize, "%s", pchOption);


        char* p_chCommandLine;
		// UH OH!!!  BIG FAT PROBLEM, THIS NAME IS ALMOST IDENTICAL TO ONE OF THE FUNCTION ARGUMENTS!!!!!!!!!!!!!
        char* p_chOption;
        char* p_chTemp;
        char szMsg[BIGBUF];

        //make a copy of and get a pointer to the command line
        p_chCommandLine = _strdup(::GetCommandLineA());
		CYCLOPSDEBUG("p_chCommandLine - %s", p_chCommandLine);
		size_t iSizeCommandLine = strlen(p_chCommandLine) + 1;
		//convert command line and szOption to lowercase for comparison
        _strlwr_s(p_chCommandLine, iSizeCommandLine);
		CYCLOPSDEBUG("baboon");
		CYCLOPSDEBUG("szOption: %s", szOption);
        _strlwr_s(szOption, iOptionBufferSize);
        //make some room for a temp string the size of the commandline
		CYCLOPSDEBUG("beatle");
        p_chTemp = (char*) malloc(strlen(p_chCommandLine) + 1);
		CYCLOPSDEBUG("crocodile");
        //find out if the option is on the list
        if((p_chOption = strstr(p_chCommandLine, szOption)) == NULL) {
                //option is not on command line
				CYCLOPSDEBUG("%s option not used.", pchOption);
                free(p_chCommandLine);
                free(p_chTemp);
				*ppchValue = NULL;
                return FALSE;
        } else {
                sprintf_s(szMsg, CYCLOPSSIZEOF(szMsg), "%s option used.", szOption);
                //frogDebugMessage( NULL, szMsg, szFunctionName, MB_OK);
        }
        //don't know if this is going to do what I want..
        if (!ppchValue) {
                //NULL was passed as second argument - user is just testing for
                //presence of command line switch
                //frogDebugMessage(NULL, "NULL passed as second argument.", szFunctionName, MB_OK);
                free(p_chCommandLine);
                free(p_chTemp);
                return TRUE;
        }
		CYCLOPSDEBUG("disco");
        switch (*(p_chOption + strlen(szOption)))
        {
        case '"':
                {
                        //now, if the first character after the option is a quote we handle
                        //this differently...
                        //we've got a quoted string here, boys and girls
                        //
                        //judgement call: if they never closed the quote, i'm just going to
                        //return everything up to the newline - that should teach 'em
                        char* p_chMover;
                        int i = 0;

                        //frogDebugMessage(NULL, "Found a quote in the option", szFunctionName, MB_OK);
                        p_chMover = p_chOption + strlen(szOption) + 1; //start right after quote
                        while (*p_chMover != '\0' && *p_chMover != '"') {
                                p_chTemp[i] = *p_chMover;
                                //printf("%c\n", p_chTemp[i]);
                                ++p_chMover;
                                ++i;
                        }
                        //by the way, if the option was used but no setting was specified, we will
                        //get a string with a single zero terminator in it - guess that's ok, huh?
                        p_chTemp[i] = '\0';
                        *ppchValue = _strdup(p_chTemp);
                        break;
                }
        case ' ':
                //for space, newline and slash, return empty string
        case '\n':
        case '/':
                *ppchValue = _strdup("");
                break;
        default:
                {
                        //first character after option is not a quote
                        int i = 0;
                        char ch;
                        //frogDebugMessage(NULL, "No quote in the option", szFunctionName, MB_OK);
                        //a space, newline, or slash will terminate the string (if the user wants
                        //a slash or space in the setting, they should quote it)
                        while(!isspace(ch = *(p_chOption + strlen(szOption) + i)) && ch != '\0' && ch != '/') {
                                p_chTemp[i] = ch;
                                ++i;
                        }
                        //by the way, if the option was used but no setting was specified, we will
                        //get a string with a single zero terminator in it - guess that's ok, huh?
                        p_chTemp[i] = '\0';
                        *ppchValue = _strdup(p_chTemp);
                }
        }
        sprintf_s(szMsg, CYCLOPSSIZEOF(szMsg), "%s option is set to \"%s\"", szOption, *ppchValue);
        //frogDebugMessage(NULL, szMsg, szFunctionName, MB_OK);
        return TRUE;
}

BOOL cyclOpsSendAutomail(char szAutomailShare[], char szRecipients[], char szSubject[], char szAutomailMessage[], 
					  char szAttachment[], BOOL boVerbose, char szOriginatingProgram[])
{
	/* Automail format:
	 * 
	 * line 1: anything
	 * line 2: recipients
	 * line 3: blank (cc, I think)
	 * line 4: blank (no idea)
	 * line 5: subject
	 * line 6 -> n: body
	 * line n + 1: <attach: attachment>
	 */
	//static FROGRUNTIMEOPTIONS stRuntimeOptions;
	static BOOL boInitialized = FALSE;
	char szMessage[BIGBUF];
	char szAutomailFullText[HUGEBUF];
	char szTempFile[MAX_PATH];
	char szAutomailFile[MAX_PATH];
	HANDLE hTempFile = INVALID_HANDLE_VALUE;
	DWORD dwWritten;

	//get username, pdc, computer, logonserver
	sprintf_s(szAutomailFullText, _TRUNCATE, "%s\r\n", __argv[0]); //anything
	sprintf_s(szAutomailFullText, _TRUNCATE, "%s%s\r\n", szAutomailFullText, szRecipients); //recipients
	sprintf_s(szAutomailFullText, _TRUNCATE, "%s\r\n", szAutomailFullText); //cc
	sprintf_s(szAutomailFullText, _TRUNCATE, "%s\r\n", szAutomailFullText); //don't know what this is
	sprintf_s(szAutomailFullText, _TRUNCATE, "%s%s\r\n", szAutomailFullText, szSubject); //subject
	sprintf_s(szAutomailFullText, _TRUNCATE, "%s%s\r\n\r\n", szAutomailFullText, szAutomailMessage); //message
	//attachment?
	if (szAttachment) 
	{
		char szAttachmentDestinationPath[MAX_PATH];
		char szAttachmentShortFilename[SMALLBUF];
		char szAttachmentExtension[SMALLBUF];
		//copy file to attach directory
		_splitpath_s(szAttachment, NULL, 0, NULL, 0, szAttachmentShortFilename, 
			CYCLOPSSIZEOF(szAttachmentShortFilename), szAttachmentExtension,
			CYCLOPSSIZEOF(szAttachmentExtension));
		sprintf_s(szAttachmentDestinationPath, _TRUNCATE, "%s\\attach\\%s%s", szAutomailShare,
			szAttachmentShortFilename, szAttachmentExtension);
		if (!CopyFileA(szAttachment, szAttachmentDestinationPath, FALSE)) {
			DWORD dwLastError = GetLastError();
			//tell recipient the attachment was dropped
			sprintf_s(szAutomailFullText, _TRUNCATE, "%s\n\nAUTOMAIL ERROR:  The system was attempting to attach the file %s, "
				"But for some reason, the attachment was lost.  Sorry.\r\n"
				"CopyFile() error %d\r\n", szAutomailFullText, szAttachment, dwLastError);
		} else {
			//add <attach:> to message
			sprintf_s(szAutomailFullText, _TRUNCATE, "%s<Attach:%s>\r\n", szAutomailFullText, szAttachmentDestinationPath);
		}
	}
	sprintf_s(szMessage, _TRUNCATE, "Automail message is:\n%s", szAutomailFullText);
	static BOOL boFirstTime = TRUE;
	if (boFirstTime)
	{
		srand((unsigned int) time(NULL));
		boFirstTime = FALSE;
	}
	//get temp dir
	if (GetEnvironmentVariableA("temp", szTempFile, MAX_PATH) == 0)
	{
		sprintf_s(szTempFile, _TRUNCATE, "%s", "c:\\");
	}
	//add 
	sprintf_s(szTempFile, _TRUNCATE, "%s\\cyclOpsAutomail%d.txt", szTempFile, rand());
	sprintf_s(szAutomailFile, _TRUNCATE, "%s\\cyclOpsAutomail%d.txt", szAutomailShare, 
		rand());
	hTempFile = CreateFileA(szTempFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hTempFile != INVALID_HANDLE_VALUE) 
	{
		if (!WriteFile(hTempFile, szAutomailFullText, (DWORD) strlen(szAutomailFullText), 
			&dwWritten, NULL)) 
		{
			CloseHandle(hTempFile);
			DeleteFileA(szTempFile);
			return FALSE;
		} 
		else 
		{
			CloseHandle(hTempFile);
			sprintf_s(szMessage, CYCLOPSSIZEOF(szMessage), "Copying %s to %s", szTempFile, szAutomailFile);
			if (!CopyFileA(szTempFile, szAutomailFile, FALSE)) 
			{
				DeleteFileA(szTempFile);
				return FALSE;
			}
			DeleteFileA(szTempFile);
		}
	} 
	else 
	{
		return FALSE;
	}
	return TRUE;
}



void cyclOpsUnixTimeToFileTime(time_t t, LPFILETIME pft)
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;
	
	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
	/*
	Once the UNIX time is converted to a FILETIME structure, 
	other Win32 time formats can be easily obtained by using 
	Win32 functions such as FileTimeToSystemTime() and 
	FileTimeToDosDateTime(). 
	*/
}

void cyclOpsUnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst)
{
	FILETIME ft;

	cyclOpsUnixTimeToFileTime(t, &ft);
	FileTimeToSystemTime(&ft, pst);
}


void cyclOpsUnixTimeToISO(time_t t, char* szTime, DWORD dwBufSize)
{
	//printf("debug: baboon\n");
	SYSTEMTIME stSystemTime;
	cyclOpsUnixTimeToSystemTime(t, &stSystemTime);
	//printf("debug: crocodile\n");
	_snprintf_s(szTime, dwBufSize - 1, _TRUNCATE, //"hi");
		"%04d-%02d-%02dT%02d:%02d:%02d",
		stSystemTime.wYear,
		stSystemTime.wMonth,
		stSystemTime.wDay,
		stSystemTime.wHour,
		stSystemTime.wMinute,
		stSystemTime.wSecond);
	//printf("debug: donkey\n");
	szTime[dwBufSize - 1] = '\0';
	
}

void cyclOpsGetISOTime(char* szISOTime, int iSize) {
	time_t time;
	::time(&time);
	//printf("debug: aardvark\n");
	cyclOpsUnixTimeToISO(time, szISOTime, iSize - 1);
}

BOOL cyclOpsGetISOTime2(char* szISOTime, int iSize, char chTimeDelimiter) {
	time_t time;
	::time(&time);
	struct tm stTM;
	errno_t iError = localtime_s(&stTM, &time);
	if (iError != 0) {
		// printf("ERROR: localtime_s failed.\n");
		return false;
	}
	
	sprintf_s(szISOTime, iSize, 
		"%4d-%02d-%02dT%02d%c%02d%c%02d", 
		stTM.tm_year + 1900,
		stTM.tm_mon + 1,
		stTM.tm_mday,
		stTM.tm_hour,
		chTimeDelimiter,
		stTM.tm_min,
		chTimeDelimiter,
		stTM.tm_sec);
	return true;
}

std::string cyclOpsGetISOTimeIII() {
	char szISOTime[100];
	::cyclOpsGetISOTime2(szISOTime, CYCLOPSSIZEOF(szISOTime), ':');
	std::string strISOTime(szISOTime);
	return strISOTime;
}

char * cyclOpsReplace( 
    char const * const original,  
    char const * const pattern,  
    char const * const replacement 
) { 
  size_t const replen = strlen(replacement); 
  size_t const patlen = strlen(pattern); 
  size_t const orilen = strlen(original); 
 
  size_t patcnt = 0; 
  const char * oriptr; 
  const char * patloc; 
 
  // find how many times the pattern occurs in the original string 
  for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen) 
  { 
	  CYCLOPSDEBUG("Pattern %s found in %s.", pattern, oriptr);
    patcnt++; 
  } 
  { 
    // allocate memory for the new string 
    size_t const retlen = orilen + patcnt * (replen - patlen); 
	size_t iReturnedSize = sizeof(char) * (retlen + 1);
    char * const returned = (char *) malloc(iReturnedSize); 
 
    if (returned != NULL) 
    { 
      // copy the original string,  
      // replacing all the instances of the pattern 
      char * retptr = returned; 
      for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen) 
      { 
        size_t const skplen = patloc - oriptr; 
        // copy the section until the occurence of the pattern 
        // strncpy(retptr, oriptr, skplen); 
		strncpy_s(retptr, iReturnedSize, oriptr, skplen);
        retptr += skplen; 
		iReturnedSize -= sizeof(char) * skplen;
        // copy the replacement  
        // strncpy(retptr, replacement, replen); 
		strncpy_s(retptr, iReturnedSize, replacement, replen); 
        retptr += replen; 
		iReturnedSize -= sizeof(char) * skplen;
      } 
      // copy the rest of the string. 
      // strcpy(retptr, oriptr); 
	  strcpy_s(retptr, iReturnedSize, oriptr); 
	  //strcpy_s(retptr, (sizeof(retptr) / sizeof(retptr[0])) - 1, oriptr);
    } 
    return returned; 
  } 
} 



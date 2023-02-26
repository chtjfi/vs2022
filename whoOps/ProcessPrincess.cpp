#include "stdafx.h"


/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include <windows.h>
#include <stdio.h>
#include <string>

#include <sstream>

#include "../cyclOps/cyclOps.h"
#include "../cyclOps/StringEmUp.h"

#include "whoOpsPrivate.h"
#include "whoOpsBonanza.h"
#include "ExceptionCreateProcessFailed.h"
#include "ProcessPrincess.h"

using std::stringstream;
using std::string;

// Constructor and destructor are defined in the header file.

DWORD whoOps::ProcessPrincess::CreateProcessII(const string& strCommandLine, cyclOps::Logger* pLogger) {
	// CreateProcess() can modify the command line string it is passed.  So you must take the const char* 
	// from strCommandLine and put it in a char[].
	CYCLOPSDEBUG("pLogger = %p", pLogger);
	char szCommandLine[10000];
	_snprintf_s(szCommandLine, CYCLOPSSIZEOF(szCommandLine), _TRUNCATE, "%s", strCommandLine.c_str());
	return whoOps::ProcessPrincess::CreateProcessII(szCommandLine, pLogger);
}

/**
 * This one prints output of child to stdout.
**/
DWORD whoOps::ProcessPrincess::CreateProcessII(char* szCommandLine, cyclOps::Logger* pLogger) {
	CYCLOPSDEBUG("pLogger = %p", pLogger);
	HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
	HANDLE hErrorWrite;
	SECURITY_ATTRIBUTES sa;

    // Set up the security attributes struct.
    sa.nLength= sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

	// Create the child output pipe.
	if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
		CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "CreatePipe() failed.");

    // Create a duplicate of the output write handle for the std error
    // write handle. This is necessary in case the child application
    // closes one of its std output handles.
    if (!DuplicateHandle(GetCurrentProcess(),hOutputWrite,
                        GetCurrentProcess(),&hErrorWrite,0,
                        TRUE,DUPLICATE_SAME_ACCESS))
	{
		CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "DuplicateHandle() failed.");
	}

    // Create new output read handle and the input write handles. Set
    // the Properties to FALSE. Otherwise, the child inherits the
    // properties and, as a result, non-closeable handles to the pipes
    // are created.
    if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
                        GetCurrentProcess(),
                        &hOutputRead, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS))
	{
		CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "DuplicateHandle() failed.");
	}

    // Close inheritable copies of the handles you do not want to be
    // inherited.
    if (!CloseHandle(hOutputReadTmp)) CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "CloseHandle() failed.");

    PROCESS_INFORMATION pi;
    STARTUPINFOA si;

    // Set up the start up info struct.
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hOutputWrite;
    si.hStdError  = hErrorWrite;

    // Launch the process that you want to redirect (in this case,
    // Child.exe). Make sure Child.exe is in the same directory as
    // redirect.c launch redirect from a command line to prevent location
    // confusion.
    if (!CreateProcessA(NULL, szCommandLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) {
		string strLastError = whoOps::ErroroneousMonk(::GetLastError()).formatMessage();
		string strWhat = cyclOps::StringEmUp::format("CreateProcess() failed launching '%s'.  Error was '%s'.", 
			szCommandLine, strLastError.c_str());
        CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, strWhat); CYCLOPSDEBUG("Rolling...");
	}

    // Set global child process handle to cause threads to exit.
    HANDLE hChildProcess = pi.hProcess; 

    // Close any unnecessary handles.
    if (!CloseHandle(pi.hThread)) CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "CloseHandle() failed."); CYCLOPSDEBUG("Rolling...");

    // Close pipe handles (do not continue to modify the parent).
    // You need to make sure that no handles to the write end of the
    // output pipe are maintained in this process or else the pipe will
    // not close when the child process exits and the ReadFile will hang.
    if (!CloseHandle(hOutputWrite)) CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "CloseHandle() failed."); CYCLOPSDEBUG("Rolling...");
    if (!CloseHandle(hErrorWrite)) CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "CloseHandle() failed."); CYCLOPSDEBUG("Rolling...");

    // Read the child's output.
    CHAR lpBuffer[2560];
    DWORD nBytesRead;

	
	char* szLine = "================================================\n";

	if (false) {
		if (pLogger != NULL) pLogger->log("Output of %s follows:", szCommandLine); CYCLOPSDEBUG("Rolling...");
		if (pLogger != NULL) pLogger->raw(szLine, strlen(szLine));
	}
    while(TRUE)
    {
        if ( ! ReadFile(hOutputRead, lpBuffer, sizeof(lpBuffer), &nBytesRead, NULL) || ! nBytesRead) { CYCLOPSDEBUG("ReadFile() failed.");
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				break; // pipe done - normal exit path.
			} else {
				CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "ReadFile() failed."); // Something bad happened.
			}
        } CYCLOPSVAR(nBytesRead, "%d");
		char* szOutput = new char[nBytesRead + 1]; CYCLOPSVAR(szOutput, "%p");
		strncpy_s(szOutput, nBytesRead, lpBuffer, _TRUNCATE); 
		if (pLogger == NULL) { CYCLOPSDEBUG("Rolling...");
			printf("%s", szOutput); CYCLOPSDEBUG("Rolling...");
		} else { CYCLOPSDEBUG("Rolling...");
			pLogger->raw(szOutput, nBytesRead); CYCLOPSDEBUG("Rolling...");
		}
		delete[] szOutput;
    }
    // Redirection is complete
	if (false) {
		if (pLogger != NULL) pLogger->raw(szLine, strlen(szLine)); CYCLOPSDEBUG("Rolling...");
		if (pLogger != NULL) pLogger->log("Output of %s complete.", szCommandLine); CYCLOPSDEBUG("Rolling...");
	}
	

    if (!CloseHandle(hOutputRead)) CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, "CloseHandle() failed."); CYCLOPSDEBUG("Rolling...");


	DWORD dwExitCode = 666;
	if (!::GetExitCodeProcess(pi.hProcess, &dwExitCode)) { CYCLOPSDEBUG("Rolling...");
		string strWhat = cyclOps::StringEmUp::format("GetExitCodeProcess() failed after launching %s.", szCommandLine);
		CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, strWhat);
	}

	if (pLogger != NULL) pLogger->log("Exit code is %d.", dwExitCode); CYCLOPSDEBUG("Rolling...");

	return dwExitCode;
}


DWORD whoOps::ProcessPrincess::CreateProcess(char* szCommandLine, bool boWait) {
	STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    if( !::CreateProcessA( NULL,   // No module name (use command line)
			szCommandLine,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
    {
		string strWhat = cyclOps::StringEmUp::format("Unable to launch process '%s'.", szCommandLine);
        CYCLOPS_THROW_EXCEPTION_III(cyclOps::Exception, strWhat);
    }

    // Wait until child process exits.
	if (boWait) {
		WaitForSingleObject( pi.hProcess, INFINITE );
	}

	DWORD dwExitCode;
	if (!::GetExitCodeProcess(pi.hProcess, &dwExitCode)) {
		string strWhat = cyclOps::StringEmUp::format("GetExitCodeProcess() failed after launching %s.", szCommandLine);
		CYCLOPS_THROW_EXCEPTION_III(cyclOps::Exception, strWhat);
	}

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	return dwExitCode;
}

cyclOps::JNIResponse whoOps::ProcessPrincess::CreateProcessIII(const cyclOps::JNIRequest& jniRequest) {
	::g_boCyclOpsDebug = jniRequest.getDebug();
	cyclOps::JNIResponse jniResponse;
	cyclOps::Logger logger;
	try {
		std::string strCommandLine = jniRequest.getTextContentRelativeToRoot("commandLine"); CYCLOPSINFO("Command line is '%s'.", strCommandLine.c_str()); CYCLOPSDEBUG("%s", strCommandLine.c_str());
		try {
			std::string strLogPath = jniRequest.getLogPath(); CYCLOPSINFO("Log path is '%s'.", strLogPath.c_str());
			logger.initializeLogFile(strLogPath);
		} catch (const std::exception& e) {
			CYCLOPSDEBUG("No log file: %s %s.", typeid(e).name(), e.what());
		}
		CYCLOPSDEBUG("About to call CreateProcessII().");
		DWORD dwReturn = CreateProcessII(strCommandLine, &logger); 
		CYCLOPSDEBUG("CreateProcessII() complete.");
		jniResponse.putProperty("exitCode", "%d", dwReturn);
		jniResponse.setStatus("SUCCESS");
	} catch (const std::exception& e) {
		jniResponse.setStatus("EXCEPTION");
		stringstream ss;
		ss << typeid(e).name() << " " << e.what();
		const string tmp = ss.str();
		jniResponse.setMessage(tmp);
		CYCLOPSERROR("%s", tmp.c_str());
	} catch (...) {
		printf("Unknown exception.\n");
		jniResponse.setStatus("EXCEPTION");
	} 
	return jniResponse;
}

void whoOps::ProcessPrincess::CreateProcessQnD(const string& strCommandLine) {
    PROCESS_INFORMATION pi;
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
	/* Apparently CreateProcessA wants a non-const char* ! */
	char szCommandLine[5000];
	CYCLOPS_SNPRINTF_S(szCommandLine, "%s", strCommandLine.c_str());
	if (!CreateProcessA(NULL, szCommandLine, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		string strLastError = whoOps::ErroroneousMonk(::GetLastError()).formatMessage();
		stringstream ss;
		ss << "CreateProcessA() failed.  " << strLastError;
		CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionCreateProcessFailed, ss.str());
	}
}

DWORD whoOps::ProcessPrincess::GetCurrentProcessHandleCount(void) {
	HANDLE hCurrentProcess = ::GetCurrentProcess();
	DWORD dwHandles;
	::GetProcessHandleCount(hCurrentProcess, &dwHandles);
	return dwHandles;
}
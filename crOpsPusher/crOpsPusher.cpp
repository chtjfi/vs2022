// crOpsPusher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/* Because cyclOps.h includes WinSock2.h, you cannot include Windows.h here.  See cyclOps.h for details. */
/* #include <Windows.h> */

#undef DOMDocument 

#include <iostream>

#include "..\crOps\crOps.h"
#include "..\cyclOps\cyclOps.h"


#ifdef _WIN64
	#pragma comment(lib, "..\\x64\\Release\\crOps.lib")
#else 
	#pragma comment(lib, "..\\Release\\crOps.lib")
#endif

// Application name.
#define APPLICATION_NAME "crOpsPusher"

using std::string;


void crOpsPusherGetConfigFile(char*, size_t);
void crOpsPusherPrintUsage();

// What follows is a little bit much on the fugly side of ugly, I believe.
// This variable g_boCrOpsDebug is also defined in crOps.cpp (which is made 
// available to us through crOps.dll).  But I don't want to expose that variable 
// across the dll boundary so I define it here again.  The declaration is in crOps.h.
bool g_boCrOpsDebug = false;

// This is declared in cyclOps.dpp:
extern bool g_boCyclOpsDebug;


int main(int argc, char* argv[])
{	
	if (argc == 1) {
		::crOpsPusherPrintUsage();
		return 1;
	}

	// Get the debug argument from the command line and set the crOps and cyclOps debug vars.
	g_boCyclOpsDebug = false; // Turn off cyclOps debugging for the retrieval of the debug switch.
	cyclOps::CommandLinebacker cmd(argc, argv);
	g_boCrOpsDebug = cmd.isOptionPresent("debug");
	g_boCyclOpsDebug = g_boCrOpsDebug; CYCLOPSVAR(g_boCrOpsDebug, "%d"); 

	// First find out if we are being asked to push all apps to all servers.
	bool boPushTarget = false;
	string strPushTarget;
	if ( ! cmd.isOptionPresent("allServers")) {
		boPushTarget = true;
		try {
			strPushTarget = cmd.getOption("pushTarget"); 
		} catch (const cyclOps::ExceptionOptionNotSet& ignore) { CYCLOPSDEBUG("%s - %s", typeid(ignore).name(), ignore.what());
			printf("\n\nYOU MUST SPECIFY EITHER /allServers OR /pushTarget:server \n\n");
			::crOpsPusherPrintUsage();
			return 1;
		}
	}

	// Get the "list only" option, which is /l to match robocopy's.
	bool boListOnly = cmd.isOptionPresent("l");

	// Get the crOpsPusher.xml config file location.  First check the command line
	// then if not specified look for ../config/crOps.xml.
	string strConfigFile;
	try {
		strConfigFile = cmd.getOption("configFile");
	} catch (const cyclOps::ExceptionOptionNotSet& e) { CYCLOPSDEBUG("%s - %s", typeid(e).name(), e.what());
		printf("\n");
		CYCLOPSERROR("No /configFile option was specified!");
		::crOpsPusherPrintUsage();
		return 1;
	}

	// Was the /application switch used to specify a single application to replicate?
	bool boSingleApplication = true;
	string strApplication;
	try {
		strApplication = cmd.getOption("application");
	} catch (const cyclOps::ExceptionOptionNotSet& e) { CYCLOPSDEBUG("%s - %s", typeid(e).name(), e.what());
		boSingleApplication = false;
	}

	// crOpsPushApplications returns a pointer to a new'ed string that IS the xml
	// results, not a path to a result file.  
	unsigned int iResult = ::crOpsPushApplications(strConfigFile.c_str(), 
		boPushTarget ? strPushTarget.c_str() : NULL, 
		g_boCrOpsDebug, boListOnly, boSingleApplication ? strApplication.c_str() : NULL);
	CYCLOPSVAR(iResult, "%d");

	// Print the result and return.
	char* pchResultSuffix;
	switch (iResult) {
		case 0:
			pchResultSuffix = "succesfully.";
			break;
		case 1:
			pchResultSuffix = "with warnings.";
			break;
		case 2:
			pchResultSuffix = "with errors.";
			break;
		case 3:
			pchResultSuffix = "with fatal errors.";
			break;
	}
	CYCLOPSINFO("Completed %s", pchResultSuffix);
	return iResult;
}

void crOpsPusherGetConfigFile(char * szConfigFile, size_t iSize) {
	char szFullModulePath[MAX_PATH];
	::GetModuleFileNameA(NULL, szFullModulePath, MAX_PATH);
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s( szFullModulePath, drive, _MAX_DRIVE, dir, _MAX_DIR, 
		fname, _MAX_FNAME, ext, _MAX_EXT );
	char szExecutableDir[MAX_PATH];
	sprintf_s(szExecutableDir, MAX_PATH, "%s%s", drive, dir);
	char* pLastSlash = strrchr(szExecutableDir, '\\');
	*pLastSlash = '\0';
	pLastSlash = strrchr(szExecutableDir, '\\');
	*pLastSlash = '\0';

	sprintf_s(szConfigFile, iSize, "%s\\data\\crOpsPusher.xml", szExecutableDir);
	CYCLOPSVAR(szConfigFile, "%s");
}

void crOpsPusherPrintUsage() {
	printf(
		"\n"
		"USAGE: " APPLICATION_NAME " /configFile:filename.xml </allServers | /pushTarget:server> [/application:name] [/debug] [/l]\n"
		"\n"
		"  /configFile:filename        MANDATORY: Location of " APPLICATION_NAME ".xml config file.\n"
		"  /allServers                 Push to all servers. Either this argument \n"
		"                              or /pushTarget must be used.\n"
		"  /pushTarget:server          Synchronize only single server.  If /allServers\n"
		"                              is not used, then this argument should be.\n"
		"  /application:<app name>     Specify a single application to replicate.\n"
		"                              The name must match the name as it appears\n"
		"                              in the xml maintained by the Citrix team.\n"
		"  /debug                      Verbose debugging output.\n"
		"  /l                          List the apps and servers you will sync, \n"
		"                              but do not actually sync.\n"
		"\n"
		"EXAMPLES:\n"
		"  " APPLICATION_NAME ".exe /allServers /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME ".exe /allServers /xmlResultFile:output.xml /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME ".exe /pushTarget:chhprdctxapp20 /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME ".exe /allServers /l /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME ".exe /allServers /debug /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME ".exe /pushTarget:chhprdctxapp20 /l /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME " /allServers /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"  " APPLICATION_NAME " /allServers /application:\"ESG - QA\" /configFile:\\\\server\\share\\dir\\" APPLICATION_NAME ".xml\n"
		"\n"
		"ERRORLEVELS:\n"
		"\n"
		"  " APPLICATION_NAME ".exe returns the following errorlevels...\n"
		"\n"
		"  0 = SUCCESS.  Everything was fine.\n"
		"  1 = WARNINGS.  All replications completed as requested, but \n"
		"      something occurred which you should be aware of.  Some \n"
		"      examples are... \n"
		"      a> you ran " APPLICATION_NAME ".exe in list-only mode, so \n"
		"      nothing actually replicated or \n"
		"      b> the output xml file could not be written to or \n"
		"      c> the server specified with /pushTarget is not listed for \n"
		"      any applications.  \n"
		"      d> you run " APPLICATION_NAME ".exe with the wrong\n"
		"      arguments, or no arguments (and you are seeing this help \n"
		"      screen as well).\n"
		"      There may be other situations causing WARNING as well, so \n"
		"      you should examine the output and look for lines that begin \n"
		"      with  WARNING.  \n"
		"  2 = ERRORS.  The program ran to completion, but errors occurred \n"
		"      during replication.  Usually this means at least one \n"
		"      application could not be pushed to at least one server.  \n"
		"      Examine the output and look for lines that begin with ERROR.\n"
		"  3 = FATAL.  This means that " APPLICATION_NAME ".exe encountered an \n"
		"      error that it could not recover from and exited prematurely.\n" 
		"      Examples of this are not being able to find both of the \n"
		"      xml config files, or encountering a network connectivity\n"
		"      issue.\n"
		"\n");
}

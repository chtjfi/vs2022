#include "StdAfx.h"
#include "JNIPusher.h"
#include "crOps.h"
#include "ExceptionFatal.h"
#include "ExceptionNONFatalCrOps.h"
#include  "../cyclOps/cyclOps.h"
#include "ApplicationPusher.h"
#include <string>
#include "Constants.h"

crOps::JNIPusher::JNIPusher(JNIPusherInputDocument* pInputDoc)
{
	_pInputDoc = pInputDoc;	
}


crOps::JNIPusher::~JNIPusher(void)
{
}


void crOps::JNIPusher::pushApplications(void)
{
	std::string strConfigFile;
	_pInputDoc->getPusherConfig(&strConfigFile);
	char* szPushTarget = _pInputDoc->getPushTarget();  /* NULL means all servers. */
	bool boDebug = _pInputDoc->getDebug();
	g_boCrOpsDebug = g_boCyclOpsDebug = boDebug;
	bool boListOnly = _pInputDoc->getListOnly(); 
	char* pchApplication = NULL;
	try {
		std::string strApplication = _pInputDoc->getApplication();
		/*  Also a blank value means all apps, so we leave pchApplication as NULL. */
		if (strApplication.compare("") != 0) {
			size_t iSize = strApplication.size() + 1;
			pchApplication = new char[iSize];
			_snprintf_s(pchApplication, iSize, _TRUNCATE, "%s", strApplication.c_str());
		}
	} catch (const crOps::ExceptionNONFatal& e) {
		/* An exception probably means no application property is absent. Do nothing because 
		   szApplication is already NULL, which will  mean "all apps" to crOpsPushApplications(). 		*/
		CYCLOPSDEBUG("No application was specified. [%s]", e.getMessage());
	}

	/* Get the output log (which is specified by crOps so it can output log to user). */
	std::string strCrOpsLog(crOps::Constants::MAGIC_WORD_NO_CROPS_LOG_FILE);
	try {
		strCrOpsLog = _pInputDoc->getCrOpsLog();
	} catch (const crOps::ExceptionNONFatal& e) {
		CYCLOPSDEBUG("No crOps log was specified. [%s]", e.getMessage());
	}
	CYCLOPSDEBUG("strCrOpsLog = %s.", strCrOpsLog.c_str());

	// The following pasted from crOpsPusher.exe...
	//
	// crOpsPushApplications returns a pointer to a new'ed string that IS the xml
	// results, not a path to a result file.  
	unsigned int iResult = ::crOpsPushApplications(
		(char*) strConfigFile.c_str(), 
		szPushTarget, 
		boDebug, 
		boListOnly, 
		pchApplication,
		&_strResultXML,
		strCrOpsLog);
	CROPSVARII(iResult, "%d");
	CYCLOPSDEBUG("_strResultXML = %s", _strResultXML.c_str());
	delete pchApplication;
	
	// iResult of 3 is fatal.  The reason crOpsPushApplications() doesn't throw an Exception itself 
	// is that it is exported as a dll function, and I don't  yet know how to throw Exceptions across
	// a dll boundary.
	if (iResult >= 3) {
		throw crOps::ExceptionFatal("crOpsPushApplications() returned FATAL.");
	}
}


std::string* crOps::JNIPusher::getResultXML(void)
{
	return &_strResultXML;
}


void crOps::JNIPusher::createFatalXML(char* szMessageInput, char* szXMLOutput, size_t iSize)
{
	CROPSFATAL(szMessageInput);
	_snprintf_s(szXMLOutput, iSize, _TRUNCATE, 
				"<?xml version='1.0' encoding='utf-8' ?>"
				"<root>"
					"<results>"
						"<status>FATAL</status>"
						"<message>%s</message>"
					"</results>"
				"</root>",
				szMessageInput);
}

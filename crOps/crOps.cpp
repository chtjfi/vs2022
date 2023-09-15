// crOps.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <stdio.h>
#include <iostream>

#include <Windows.h>

#include <boost/filesystem.hpp>

#include "..\cyclOps\cyclOps.h"

#include "crOpsPrivate.h"
#include "ExceptionFatal.h"
#include "ExceptionNONFatalCrOps.h"
#include "ReturnWidglet.h"
#include "ExceptionFatal.h"
#include "ResultDocument.h"
#include "PusherConfigDoc.h"
#include "ApplicationPusher.h"
#include "JNIPusher.h"
#include "JNIPusherInputDocument.h"
#include "crOps_replication_JNICrOpsPusher.h"
#include "Constants.h"

bool g_boCrOpsDebug = false;
cyclOps::Logger g_crOpsLogger;

using namespace xercesc;

extern "C" __declspec(dllexport) unsigned int crOpsPushApplications(const char *szCrOpsConfigFile, const char* szPushTarget, bool boDebug, 
	bool boListOnly, const char* szApplication)  
{
	printf("Hello from the log-free version.");
	return ::crOpsPushApplications(
		szCrOpsConfigFile, 
		szPushTarget, 
		boDebug, 
		boListOnly, 
		szApplication,
		NULL, // No result xml desired.
		crOps::Constants::MAGIC_WORD_NO_CROPS_LOG_FILE); // No log file specified.
}

unsigned int crOpsPushApplications(const char *szCrOpsConfigFile, const char* szPushTarget, bool boDebug, 
	bool boListOnly, const char* szApplication, std::string* pstrResultXML, 
	std::string strCrOpsLog)  
{
	CROPSDEBUG("Hello from the logging version.  strCrOpsLog = '%s'", strCrOpsLog.c_str());
	/* If the user specified a log, initialize it. */
	if (strCrOpsLog.compare(crOps::Constants::MAGIC_WORD_NO_CROPS_LOG_FILE) != 0) {
		CROPSDEBUG("About to initialize log with %s.", strCrOpsLog.c_str());
		g_crOpsLogger.initializeLogFile(strCrOpsLog);
	}

	g_boCrOpsDebug = boDebug;

	// A ReturnWidget starts at 0 (SUCCESS) and lets you
	// set the level higher, but never lower.
	crOps::ReturnWidglet returnWidglet;
	
	// Initialize the XML system.
	try {
		XMLPlatformUtils::Initialize();
	} catch (const XMLException& e) {
		const XMLCh* xmlChMessage = e.getMessage();
		CROPSERROR("Could not initialize XML platform utilities - %S", xmlChMessage);
		return 3;
	}

	// The following is set apart in a block because the destructors of some of the
	// objects delete parsers, which can not happen after the call to 
	// XMLPlatformUtils::Terminate(), which is therefore called just after the block.
	{ 
		// Create the result xml document.
		crOps::ResultDocument resultDocument;
		try {
			resultDocument.initialize();
			resultDocument.addRootElement(L"crOpsConfigFile", szCrOpsConfigFile);
			resultDocument.addRootElement(L"pushTarget", szPushTarget);
			resultDocument.addRootElement(L"application", szApplication);
		} catch (...) {
			CROPSWARNING("Could not initialize result document.");
			// disable() will cause all methods in the result document to be non-ops.
			resultDocument.disable();
			// We also need to flag a warning for the output 
			returnWidglet.setWarning();
		}

		// Create the pusher config document.  The constructor can throw an ExceptionFatale.
		crOps::PusherConfigDoc configDocument;
		try {
			CROPSDEBUG("About to call initialize().");
			configDocument.initialize(szCrOpsConfigFile);
		} catch (crOps::ExceptionFatal e) {
			CROPSFATAL("Could not initialize crOps config document %s.", szCrOpsConfigFile);
			// addRootException() also sets root status to ERROR.
			resultDocument.addRootException(e.getMessage());
			unsigned int iStatus = 3; // 3 is fatal
			resultDocument.setRootStatus(iStatus);
			// We won't set the ReturnWidglet because we are returning immediately.
			return iStatus;
		} catch (...) {
			CROPSFATAL("Could not initialize crOps config document %s.", szCrOpsConfigFile);
			// addRootException() also sets root status to ERROR.
			resultDocument.addRootException("Unknown exception");
			unsigned int iStatus = 3; // 3 is fatal
			resultDocument.setRootStatus(iStatus);
			// We won't set the ReturnWidglet because we are returning immediately.
			return iStatus;
		}


		// Do the replication.
		try {
			crOps::ApplicationPusher pusher;
			pusher.setPushTarget(szPushTarget);
			pusher.setApplication(szApplication);
			pusher.pushApplications(&configDocument, &resultDocument, boListOnly);
			if (pusher.wereThereWarnings()) {
				returnWidglet.setWarning();
			}
			if (pusher.wereThereErrors()) {
				returnWidglet.setError();
			}
		}  catch (crOps::ExceptionFatal& e) {
			CROPSFATAL(e.getMessage());
			resultDocument.addRootException(e.getMessage());
			resultDocument.setRootStatus(3);
			returnWidglet.setFatal();
		} catch (...) {
			CROPSFATAL("Unknown exception caught.");
			resultDocument.addRootException("Unknown exception caught");
			resultDocument.setRootStatus(3);
			returnWidglet.setFatal();
		}

		// Write the result xml to a file and if requested string passed in.
		try {
			char* pchResultXMLFile = configDocument.getResultXMLFile();
			int iReturn = resultDocument.writeToFile(pchResultXMLFile);
			if (iReturn == 1) {
				returnWidglet.setWarning();
			}
			if (pstrResultXML != NULL) {
				resultDocument.writeToString(pstrResultXML);
			}
		} catch (crOps::ExceptionNONFatal e) {
			returnWidglet.setWarning();
			CROPSWARNING("Non fatal exception caught while trying to write xml results: %s", 
				e.getMessage());
		}
	}

	// Uninitialize the XML system.
	XMLPlatformUtils::Terminate();

	return returnWidglet.getStatus();
}

void crOpsThrowFatalException(char* szFile, char* szFunction, int iLine, char* szFormat, ...) 
{
	char szMessage[5000];
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	_vsnprintf_s(szMessage, CROPSSIZEOF(szMessage), _TRUNCATE, szFormat, vaArguments);
	CROPSFATAL("%s", szMessage);
	/* The extra 100 if for the line number. */
	size_t iSize = strlen(szMessage) + strlen(szFile) + strlen(szFunction) + 100; 
	char* pchMessage = new char[iSize];
	sprintf_s(pchMessage, iSize, "%s [%s %s(%d)]", szMessage, szFile, szFunction, iLine);
	CROPSDEBUG("Throwing ExceptionFatal.");
	throw crOps::ExceptionFatal(pchMessage);
}

void crOpsThrowNONFatalException(char* szFile, char* szFunction, int iLine, char* szFormat, ...) 
{
	char szMessage[5000];
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	_vsnprintf_s(szMessage, CROPSSIZEOF(szMessage), _TRUNCATE, szFormat, vaArguments);
	
	/* Do NOT write an error!!!  Let the caller decide if he/she wants to! */
	/* CROPSERROR("%s", szMessage); */

	/* The extra 100 if for the line number. */
	size_t iSize = strlen(szMessage) + strlen(szFile) + strlen(szFunction) + 100; 
	char* pchMessage = new char[iSize];
	sprintf_s(pchMessage, iSize, "%s [%s %s(%d)]", szMessage, szFile, szFunction, iLine);
	CROPSDEBUG("Throwing ExceptionNONFatal.");
	throw crOps::ExceptionNONFatal(pchMessage);
}

JNIEXPORT jstring JNICALL Java_crOps_replication_JNICrOpsPusher_pushApplicationsNative(JNIEnv * env, jobject obj, jstring input) {
	char *szInput = (char*) env->GetStringUTFChars(input, NULL);
	char szError[5000]; 
	if (szInput == NULL) {
		_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, "Input to " __FUNCTION__ "() was null.");
	} else {
		try {
			crOps::JNIPusherInputDocument pusherInputDoc(szInput);
			crOps::JNIPusher jniPusher(&pusherInputDoc);
			jniPusher.pushApplications();
			std::string* pstrResultXML = jniPusher.getResultXML();
			return env->NewStringUTF(pstrResultXML->c_str());
		} catch (crOps::ExceptionFatal e) {
			_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, 
				"ExceptionFatal caught during " __FUNCTION__ "(). %s", e.getMessage());
		} catch (crOps::ExceptionNONFatal e) {
			_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, 
				"ExceptionNONFatal caught during " __FUNCTION__ "(). %s", e.getMessage());
		} catch (...) {
			_snprintf_s(szError, sizeof(szError) / sizeof(szError[0]), _TRUNCATE, 
				"An unknown exception occurred during " __FUNCTION__ "().  See log.");
		}
	}
	// If we are here then an exception must have occurred because we return in the try section.
	// The error message is in szError.
	char szXMLReturn[5000];
	crOps::JNIPusher::createFatalXML(szError, szXMLReturn, sizeof(szXMLReturn) / sizeof(szXMLReturn[0]));
	return env->NewStringUTF(szXMLReturn);
}

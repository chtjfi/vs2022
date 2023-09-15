#include "StdAfx.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>

#include <boost/filesystem.hpp>

#include "..\cyclOps\cyclOps.h"

#include "ApplicationPusher.h"
#include "ResultDocument.h"
#include "Utility.h"
#include "..\whoOps\ACLAmigo.h"
#include "Constants.h"


using namespace xercesc;
using std::string;
using std::wstring;



crOps::ApplicationPusher::ApplicationPusher(void)  : 
	_boErrorsOccurred(false), 
	_pchPushTarget(NULL), 
	_boAtLeastOneServerFound(false), 
	_boAtLeastOneApplicationFound(false),
	_boWarningsOccurred(false),
	_pchApplication(NULL)
{
}

crOps::ApplicationPusher::~ApplicationPusher(void)
{
	CROPSDEBUG("hello");
	if (_pchPushTarget) {
		delete _pchPushTarget;
	}
}

void crOps::ApplicationPusher::pushApplications(PusherConfigDoc* pPusherConfigDocument, crOps::ResultDocument* pResultDocument, bool boListOnly) {
	_boListOnly = boListOnly;
	if (boListOnly) {
		_boWarningsOccurred = true;
		pResultDocument->addRootMessage("Running in list-only mode, no replication will occur!");
	}
	_pResultDocument = pResultDocument;
	_pPusherConfigDocument = pPusherConfigDocument;
	try {
		xercesc::DOMDocument* pDocumentCitrix = _pPusherConfigDocument->getCitrixDocument();
		this->processDocumentCitrix(pDocumentCitrix);
	} catch (const XMLException& e) {
		CROPS_THROW_FATAL("XMLException caught - %S", e.getMessage());
	} catch (const DOMException& e) {
		CROPS_THROW_FATAL("DOMException caught - %S", e.getMessage());
	}
	unsigned int iStatus = 0;
	iStatus = this->wereThereWarnings() ? 1 : iStatus;
	iStatus = this->wereThereErrors() ? 2 : iStatus;
	CROPSVARII(iStatus, "%d");
	pResultDocument->setRootStatus(iStatus);
	CROPSDEBUG("goodbye");
}

void crOps::ApplicationPusher::processDocumentCitrix(xercesc::DOMDocument* pDocumentCitrix) {
	DOMElement* pRootCitrix = pDocumentCitrix->getDocumentElement();
	CROPSDEBUG("pRoot: %p", pRootCitrix);
	if (pRootCitrix != NULL) {
		// Process each child.
		DOMNodeList* pAppNodeList = pRootCitrix->getChildNodes();
		if (pAppNodeList != NULL) {
			this->processAppNodeList(pAppNodeList);
		} else {
			_boErrorsOccurred = true;
			CROPSERROR("Could not get app node list.");
			throw crOps::ExceptionFatal("Could not get app NodeList.");
		}
	} else {
		_boErrorsOccurred = true;
		CROPSERROR("Could not get document root.");
		throw crOps::ExceptionFatal("Could not get document root.");
	}
	if (_pchPushTarget && !_boAtLeastOneServerFound) {
		_boWarningsOccurred = true;
		char szMessage[1000];
		sprintf_s(szMessage, CROPSSIZEOF(szMessage), 
			"There were no applications found to replicate to the server %s.", _pchPushTarget);
		CROPSWARNING(szMessage);
		_pResultDocument->addRootMessage(szMessage);
	}
	if (_pchApplication && !_boAtLeastOneApplicationFound) {
		_boWarningsOccurred = true;
		char szMessage[1000];
		sprintf_s(szMessage, CROPSSIZEOF(szMessage), "There was no application found with the name '%s'.", _pchApplication);
		CROPSWARNING(szMessage);
		_pResultDocument->addRootMessage(szMessage);
	}
	CROPSDEBUG("goodbye");
}

bool crOps::ApplicationPusher::wereThereErrors(void)
{
	return _boErrorsOccurred;
}

void crOps::ApplicationPusher::processAppNodeList(DOMNodeList* pAppNodeList) {
	CROPSDEBUG("pAppNodeList: %p", pAppNodeList);
	for (unsigned int i = 0; i < pAppNodeList->getLength(); ++i) {
		DOMNode* pAppNode = pAppNodeList->item(i);
		DOMNode::NodeType nodeType = pAppNode->getNodeType();
		if (nodeType == DOMNode::ELEMENT_NODE) {
			CROPSDEBUG("i: %d", i);
			try {
				this->processAppNode(pAppNode);
			} catch (crOps::ExceptionNONFatal e) {
				_boErrorsOccurred = true;
				CROPSERROR("ExceptionNONFatal: %s", e.getMessage());
			}
		}
	}
	CROPSDEBUG("goodbye");
}

void crOps::ApplicationPusher::processAppNode(DOMNode* pAppNode) {

	CROPSDEBUG("pAppNode: %p", pAppNode);

	// First we need to get the application name to use when creating the
	// <application> node in the result xml.  
	char szAppName[250];
	DOMElement* pAppElement = (DOMElement*) pAppNode;
	this->getChildElementTextContent(pAppElement, L"Name", szAppName,
			CROPSSIZEOF(szAppName));

	// If a single application was set to replicate, is this the one?
	if (_pchApplication != NULL) {
		bool boIsApplication = this->isApplication(szAppName);
		if (!boIsApplication) {
			CROPSDEBUG("%s is not %s.", _pchApplication, szAppName);
			return;
		}
	}
	_boAtLeastOneApplicationFound = true;

	// If a single server was specified as pushTarget, is that server on this apps list?
	// Why do we care?  Because if we check now, we can exit before creating an app result
	// node in the result doc, and this is less confusing to someone reading the result.xml.
	// Previously, when an app wasn't replicated to a server, that servers xml file had a
	// section for that app with an overall app status of SUCCESS.
	if (_pchPushTarget != NULL) {
		if (!this->isPushTargetOnAppList(pAppNode)) {
			CROPSDEBUG("%s is not on list of servers for %s", _pchPushTarget, szAppName);
			return;
		}
	}

	// This is the <application> node that contains the results for this particular app.
	// We can receive a NULL if the result document has been disbled.
	DOMNode* pAppResultNode = _pResultDocument->addAppNode(szAppName);

	// This variable is set to false if anything goes wrong, and then is
	// used later to determine what to write to the <result> element under
	// the <application> node for this application.
	bool boSuccess = true;

	// Essentially everything done in this method is wrapped in a try/catch
	// which catches only ExceptionNONFatal.  If ExceptionNONFatal is thrown
	// boSuccess is set to false and this causes the <result> element under
	// the <application> node for this application to be set to error.
	try {
		char szAppTargetDir[250];
		// These guys will throw an ExceptionNONFatal if there is an error, which dad will catch
		// and then hopefully continue with next app node.
		this->getChildElementTextContent(pAppElement, L"Dir", szAppTargetDir, 
			CROPSSIZEOF(szAppTargetDir));

		// So now I know the name of the application we are processing in this iteration,
		// and the target directory.
		CROPSINFO("Pushing application %s", szAppName);
		CROPSVARII(szAppTargetDir, "%s");
	
		// Get the list of servers to replicate to.
		if (pAppNode->getNodeType() != DOMNode::ELEMENT_NODE) {
			char* szMessage = "Node is not an element node.";
			CROPSERROR(szMessage);
			throw ExceptionNONFatal(szMessage);
		}
		DOMNodeList* pServerNodeList = ((DOMElement*)pAppNode)->getElementsByTagName(L"Server");
		if (pServerNodeList == NULL) {
			char* szMessage = "Could not get server elements.";
			CROPSERROR(szMessage);
			throw ExceptionNONFatal(szMessage);
		}

		// Replicate to all the servers.
		for (unsigned int i = 0; i < pServerNodeList->getLength(); ++i) {
			CROPSVARII(i, "%d");
			DOMNode* pServerNode = pServerNodeList->item(i);
			// This next method call can throw an ExceptionNONFatal which dad will catch and
			// then continue with next app node.
			bool boResult = this->pushApplicationToServer(pServerNode, szAppName, szAppTargetDir,
				pAppResultNode);
			if (!boResult) {
				boSuccess = false;
			}
			CROPSVARII(boSuccess, "%d");
		}
	} catch (ExceptionNONFatal e) {
		_boErrorsOccurred = true;
		CROPSERROR("%s [%s]", e.getMessage(), typeid(e).name());
		boSuccess = false;
	}

	// The desired result here is that if there was an error in pushApplicationToServer() the
	// status is ERROR, otherwise if /l was used the status is WARNING, otherwise if /pushTarget 
	// was finally if neither
	// are true then status is SUCCESS.
	_pResultDocument->setAppStatus(pAppResultNode, 
		boSuccess ? (_boListOnly ? L"WARNING" : L"SUCCESS") : L"ERROR");
	CROPSDEBUG("goodbye");
}

/**
* Given an element, get the text content of the first child element
* with the tag name specified in wszElement.
**/
bool crOps::ApplicationPusher::getChildElementTextContent(DOMElement* pElement, 
	wchar_t* wszChildElement, char* szTextContent,	size_t iSizeOfSZTextContent) 
{
	wchar_t* wszTextContent = new wchar_t[iSizeOfSZTextContent]; 
	bool boResult = this->getChildElementTextContent(pElement, wszChildElement, wszTextContent, iSizeOfSZTextContent);
	_snprintf_s(szTextContent, iSizeOfSZTextContent, _TRUNCATE, "%S", wszTextContent);
	delete[] wszTextContent;
	return boResult;
}

bool crOps::ApplicationPusher::getChildElementTextContent(DOMElement* pElement, 
	wchar_t* wszChildElement, wchar_t* wszTextContent,	size_t iSize) 
{
	//size_t iSize = CROPSSIZEOF(szTextContent);
	CROPSVARII(iSize, "%d");
	DOMNodeList* pNodeList = pElement->getElementsByTagName(wszChildElement);
	if (pNodeList == NULL) {
		CROPSERROR("No <%S> element found.", wszChildElement);
		_boErrorsOccurred = true;
		throw ExceptionNONFatal("Element not found");
	}
	if (pNodeList->getLength() >= 1) {
		DOMNode* pNode = pNodeList->item(0);
		// I'm fairly certain this is an ELEMENT_NODE.
		XMLCh pxmlchTextContent[5000]; 
		crOps::Utility::getTextContent(pNode, pxmlchTextContent, CROPSSIZEOF(pxmlchTextContent));
		_snwprintf_s(wszTextContent, iSize, _TRUNCATE, L"%s", pxmlchTextContent);
		return true;
	} else {
		return false;
	}
}



bool crOps::ApplicationPusher::pushApplicationToServer(DOMNode* pServerNode, char* szAppName, 
	char* szAppTargetDir, DOMNode* pAppOutputNode)
{
	// We know that pServerNode is an element node because we got it with
	// getElementsByTagName().
	XMLCh pXmlchServerName[5000];
	crOps::Utility::getTextContent(pServerNode, pXmlchServerName, CROPSSIZEOF(pXmlchServerName));
	if (cyclOps::StringEmUp::inW(pXmlchServerName, ApplicationPusher::_vectorOfOfflineServers)) {
		CYCLOPSINFO("%S is marked OFFLINE.", pXmlchServerName);
		return false;
	}

	// If _pchPushTarget is not NULL, and it is not equal to this server name, then 
	// we will not replicate to this server because the user has specified /pushTarget
	// on the command line and only wants to sync that server.  No server result
	// element will be written for this iterations's app and server combination.
	if (_pchPushTarget != NULL && !this->isPushTarget(pXmlchServerName)) {
		CROPSDEBUG("%S is not the specified push target.", pXmlchServerName);
		// I think we need to get a SKIPPED written to the result xml file for this app/server combo.
		// Because Matthias was confused when he saw SUCCESS for a bunch of application statuses
		// making him think that the app had been succesfully replicated when in fact the SUCCESS
		// meant that no attempt to replicate had been made.
		//
		// BUT THIS DOES NOT CHANGE THE OVERALL STATUS FOR THE APPLICATION OR THE WHOLE FILE
		// (which is controlled by returning true)!!!  The reason for this is if we changed
		// the app status by returning false, we also change the overall status for the whole
		// replication ("/root/results/status").  Under such circumstances, pretty much
		// every replication will have an overall status of WARNING, because there is always
		// at least one app that is not published on a server.  
		//
		// The reason we want to write SKIPPED is because if we write nothing, an admin
		// only sees "SUCCESS" under that app and assumes that the app was replicated, when it
		// was not (because it is not published there).
		_pResultDocument->setServerStatus(pAppOutputNode, pXmlchServerName, L"SKIPPED");
		return true;
	}

	// If the /l switch was specified, we want the server status (along with the overall status)
	// to be WARNING.  
	if (_boListOnly) {
		CROPSWARNING("Running in list only mode.  Not replicating %s to %S.", szAppName, pXmlchServerName);
		_pResultDocument->setServerStatus(pAppOutputNode, pXmlchServerName, L"WARNING");
		// Returning true here causes the overall status not to be switched to ERROR (I think).
		return true;
	}

	// There exists the danger that the user has misspelled the /pushTarget.  In this case,
	// because the server is never found in any application's list of servers to replicate to,
	// no error ever occurs.  To protect against this, we introduce a flag that indicates if
	// at least one server was FOUND to replicate to (no indication of success/failure).  Then
	// when the replication is over we check if (_boPushTarget && !_boAtLeastOnceServerFound)
	// we write a WARNING to the console, but still return 0 from exe because the server simply
	// might not be a replication target for any app.
	_boAtLeastOneServerFound = true;

	// Create the target directory path.
	char szTargetUNC[5000];
	this->createTargetUNC(pXmlchServerName, szAppTargetDir, szTargetUNC, CROPSSIZEOF(szTargetUNC));

	// Get the source directory path from crOps.xml.
	char szSourceDirectory[5000];
	try {
		this->getSourceDirectory(szAppName, szSourceDirectory, CROPSSIZEOF(szSourceDirectory));
		CROPSVARII(szSourceDirectory, "%s");
	} catch (ExceptionNONFatal e) {
		_pResultDocument->addAppError(pAppOutputNode, "Could not get source directory for application.");
		throw e;
	}

	/* Get the purge value. */
	bool boPurge = this->getPurgeValue(szAppName);

	// Sync the directories.
	try {
		printf("About to call syncDirectories()\n");
		bool boResult = this->syncDirectories(szSourceDirectory, szTargetUNC, boPurge);
		if (boResult) {
			CROPSINFO("Replication of %s to %S succesful.", szAppName, pXmlchServerName);
		}
		else {
			_boErrorsOccurred = true;
			CROPSERROR("Errors occurred during the replication of %s to %S.", szAppName, pXmlchServerName);
		}
		// Set the permissions.  This call CANNOT go in the parent function because this function
		// contains the logic to skip servers that were either not specified as /pushTarget or
		// abort if /l (list only) was specified.
		this->setPermissions(szTargetUNC, szAppName);

		_pResultDocument->setServerStatus(pAppOutputNode, pXmlchServerName, boResult ? L"SUCCESS" : L"ERROR");
		CROPSDEBUG("setServerStatus() complete.");
		return boResult;

	} catch (const boost::filesystem::filesystem_error& e) {
		printf("!!! %S OFFLINE POSSIBLY SO MARKING IT SO...\n", pXmlchServerName);
		ApplicationPusher::_vectorOfOfflineServers.push_back(pXmlchServerName);
		/* We return false because on 2023-08-06 I believe the only effect of that is to set the 
		   status of the app 's replication to ERROR. */
		return false;
	}


}


void crOps::ApplicationPusher::createTargetUNC(const XMLCh* pXmlchServer, char* szAppTargetDir,
	char* szTargetUNC, size_t iSize)
{
	CROPSVARII(szAppTargetDir, "%s");
	char chDrive = szAppTargetDir[0];
	char szDirectory[5000];
	strcpy_s(szDirectory, CROPSSIZEOF(szDirectory), (const char*) szAppTargetDir + 2);
	sprintf_s(szTargetUNC, iSize, "\\\\%S\\%c$%s", pXmlchServer, chDrive, szDirectory);
	CROPSVARII(szTargetUNC, "%s");
}




void crOps::ApplicationPusher::getSourceDirectory(char* szAppName, char* szSourceDirectory, 
	size_t iSize)
{
	DOMElement* pAppElement = this->getApplicationElement(szAppName);
	this->getChildElementTextContent(pAppElement, L"source", szSourceDirectory,	iSize);

}




bool crOps::ApplicationPusher::syncDirectories(char* szSourceDirectory, char* szTargetDirectory, bool boPurge)
{

	CROPSINFO("Synchronization source directory - %s", szSourceDirectory);
	CROPSINFO("Synchronization target directory - %s", szTargetDirectory);
	bool boReturn = true;
	string strLogFileName = ::g_crOpsLogger.getLogFileName();
	if (!_boListOnly) {
		boReturn = ::dllexportSlOpsCopyDirectory(szSourceDirectory, szTargetDirectory, true, boPurge, g_boCrOpsDebug, strLogFileName.c_str());
	} else {
		CROPSWARNING("Running in 'list-only' mode, no sync occurring.");
	}
	return boReturn;

}



void crOps::ApplicationPusher::setPushTarget(const char* pchPushTarget)
{
	CROPSVARII(pchPushTarget, "'%s'");
	CROPSVARII(pchPushTarget, "%p");
	if (pchPushTarget) {
		CROPSDEBUG("pchPushTarget not null.");
		size_t iSize = strlen(pchPushTarget) + 1;
		_pchPushTarget = new char[iSize];
		strcpy_s(_pchPushTarget, iSize, pchPushTarget);
		CROPSINFO("Push target is %s", _pchPushTarget);
	} else {
		CROPSINFO("No push target specified.  Pushing to all eligible servers.");
	}
	CROPSVARII(_pchPushTarget, "%p");
	CROPSVARII(_pchPushTarget, "'%s'");
}


bool crOps::ApplicationPusher::isPushTarget(const XMLCh* pXmlchServerName)
{
	char szServer[1000];
	sprintf_s(szServer, CROPSSIZEOF(szServer), "%S", pXmlchServerName);
	if (_stricmp(szServer, _pchPushTarget) == 0) {
		return true;
	}
	return false;
}

bool crOps::ApplicationPusher::isApplication(const char* pchApplication)
{
	if (_stricmp(pchApplication, _pchApplication) == 0) {
		return true;
	}
	return false;
}

bool crOps::ApplicationPusher::wereThereWarnings(void)
{
	return _boWarningsOccurred;
}


void crOps::ApplicationPusher::setApplication(const char* szApplication)
{
	CROPSVARII(szApplication, "'%s'");
	if (szApplication) {
		CROPSVARII(szApplication, "'%s'");
		size_t iSize = strlen(szApplication) + 1;
		_pchApplication = new char[iSize];
		strcpy_s(_pchApplication, iSize, szApplication);
		CROPSINFO("Application to replicate is %s", _pchApplication);
	} else {
		CROPSINFO("No application specified.  Pushing all applications.");
	}
	CROPSVARII(_pchApplication, "'%s'");
}

void crOps::ApplicationPusher::setPermissions(char* szTargetUNC, char* szAppName) {
	std::vector<DOMNode*> grantNodeVector;
	try {
		_pPusherConfigDocument->populateGrantNodeVector(szAppName, &grantNodeVector);
	} catch (const crOps::ExceptionNONFatal& ignore) { CROPSDEBUG("%s - %s", typeid(ignore).name(), ignore.getMessage());
		/* I think we can ignore this, it means there is no <permissions> element. 
			It will leave grantNodeVector with 0 size. */ 
	}
	if (grantNodeVector.size() == 0) {
		CROPSINFO("No permission list for %s", szAppName);
		return;
	}
	for (unsigned int i = 0; i < grantNodeVector.size(); ++i) {
		try {
			DOMNode* pGrantNode = grantNodeVector[i];
			this->processGrantNode(pGrantNode, szTargetUNC);
		} catch (const cyclOps::Exception& e) {
			this->_boErrorsOccurred = true;
			CROPSERROR("Error setting permisions.  [type:'%s' - what:'%s' - win32:'%s'] over", 
				typeid(e).name(), e.what(), e.formatMessage().c_str());
		} catch (const std::exception& e) {
			this->_boErrorsOccurred = true;
			CROPSERROR("Error setting permisions.  [%s - %s]", typeid(e).name(), e.what());
		}
	}
}

void crOps::ApplicationPusher::processGrantNode(DOMNode* pGrantNode, char* szTargetUNC) {
	CROPSDEBUG("pGrantNode = %p, szTargetUNC = %s", pGrantNode, szTargetUNC);
	char szAccount[1000];
	this->getChildElementTextContent((DOMElement*) pGrantNode, L"account", szAccount, sizeof(szAccount) / sizeof(szAccount[0])); CROPSVARII(szAccount, "%s");
	wchar_t wszFile[MAX_PATH];
	this->getChildElementTextContent((DOMElement*) pGrantNode, L"file", wszFile, sizeof(wszFile) / sizeof(wszFile[0])); CROPSVARII(wszFile, "%S");
	std::wstringstream wssFullPath;
	string strTargetDirectory(szTargetUNC);
	wssFullPath << wstring(strTargetDirectory.begin(), strTargetDirectory.end()) << L"\\" << wszFile;
	char szPermission[100];
	this->getChildElementTextContent((DOMElement*) pGrantNode, L"permission", szPermission, sizeof(szPermission) / sizeof(szPermission[0]));
	whoOps::ACLAmigo aclAmigo;
	aclAmigo.addPermissionsToFileOrFolder(wssFullPath.str(), szAccount, szPermission);
}


bool crOps::ApplicationPusher::isPushTargetOnAppList(DOMNode* pAppNode)
{
	CROPSDEBUG("Hello.");
	DOMNodeList* pNodeList = pAppNode->getChildNodes();
	if (pNodeList == NULL || pNodeList->getLength() == 0) {
		// Something is malformed in the xml.  Do we return false and continue?  Or do we throw an exception?
		// I'm going to say for now that we be forgiving and continue, just returning false.
		return false;
	}
	for (unsigned int i = 0; i < pNodeList->getLength(); ++i) {
		DOMNode* pNode = pNodeList->item(i); CROPSVARII(pNode, "%p");
		if (pNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			const XMLCh* xmlchNodeName = pNode->getNodeName(); CROPSVARII(xmlchNodeName, "%S");
			if (wcscmp(xmlchNodeName, crOps::Constants::ELEMENT_SERVER) == 0) {
				const XMLCh* xmlchTextContent = pNode->getTextContent(); CROPSVARII(xmlchTextContent, "%S");
				XMLCh xmlchTextContentNONConst[1000];
				_snwprintf_s(xmlchTextContentNONConst, CROPSSIZEOF(xmlchTextContentNONConst), _TRUNCATE, L"%s", xmlchTextContent);
				xercesc::XMLString::trim(xmlchTextContentNONConst);
				wchar_t wszPushTarget[1000];
				_snwprintf_s(wszPushTarget, CROPSSIZEOF(wszPushTarget), _TRUNCATE, L"%S", _pchPushTarget); CROPSVARII(wszPushTarget, "%S");
				if (_wcsicmp(xmlchTextContentNONConst, wszPushTarget) == 0) {
					CROPSDEBUG("Returning true.");
					return true;
				}
			}
		}
	}
	return false;
}


DOMElement* crOps::ApplicationPusher::getApplicationElement(char* szAppName) {
	DOMElement* pPusherElement = _pPusherConfigDocument->getPusherElement();
	DOMNodeList* pAppsNodeList = pPusherElement->getElementsByTagName(L"applications");
	if (pAppsNodeList == NULL || pAppsNodeList->getLength() < 1) {
		char* szMessage = "Could not get applications node list.";
		CROPSERROR(szMessage);
		throw crOps::ExceptionFatal(szMessage);
	}
	DOMNode* pAppsNode = pAppsNodeList->item(0);
	if (pAppsNode == NULL) {
		char* szMessage = "Could not get applications node.";
		CROPSERROR(szMessage);
		throw crOps::ExceptionFatal(szMessage);
	}
	DOMNodeList* pAppNodeList = ((DOMElement*) pAppsNode)->getElementsByTagName(L"application");
	if (pAppNodeList == NULL || pAppNodeList->getLength() < 1) {
		char* szMessage = "Could not get app node list.";
		CROPSERROR(szMessage);
		throw crOps::ExceptionFatal(szMessage);
	}
	CROPSVARII(szAppName, "%s");
	// Iterate each <application>, looking to see if szAppName matches the <name> element.
	for (unsigned int i = 0; i < pAppNodeList->getLength(); ++i) {
		// We know these are elements already.
		DOMElement* pAppElement = (DOMElement*) pAppNodeList->item(i);
		try {
			char szTextContent[500];
			this->getChildElementTextContent(pAppElement, L"name", szTextContent, CROPSSIZEOF(szTextContent));
			CROPSVARII(szTextContent, "%s");
			if (_stricmp(szAppName, szTextContent) == 0) {
				return pAppElement;
			}
		} catch (crOps::ExceptionNONFatal e) {
			// Not a big deal, continue looking.
		}
	}
	char* szMessage = "The application could not be found in crOpsPusher.xml";
	CROPSERROR(szMessage);
	throw crOps::ExceptionNONFatal(szMessage);
}


bool crOps::ApplicationPusher::getPurgeValue(char* szAppName)
{
	DOMElement* pAppElement = this->getApplicationElement(szAppName);
	char szPurge[100];
	bool boPurgeElementPresent = 
		this->getChildElementTextContent(pAppElement, L"purge", szPurge, sizeof(szPurge) / sizeof(szPurge[0]));
	if (boPurgeElementPresent) {
		return _stricmp("true", szPurge) == 0;
	} else {
		/* Default is to not purge. */
		return false;
	}
}



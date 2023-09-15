#include "StdAfx.h"

#include "..\cyclOps\cyclOps.h"

#include "ResultDocument.h"
#include "ExceptionNONFatalCrOps.h"

crOps::ResultDocument::ResultDocument(void) :
	_boDisabled(true), // Disabled until initialize() is called.
	_iStatus(-1) // So the first call to setStatus(0) will write the element.
{
}


crOps::ResultDocument::~ResultDocument(void)
{
	if (_boDisabled) return;
	CROPSDEBUG("goodbye");
}

void crOps::ResultDocument::initialize(void)
{
	DOMImplementation* pDOMImplementation 
		= DOMImplementationRegistry::getDOMImplementation(L"LS");
	_pResultDocument = pDOMImplementation->createDocument(0, L"root", 0);
	_pRootNode = _pResultDocument->getDocumentElement();
	_pAppsNode = _pResultDocument->createElement(L"apps");
	_pRootNode->appendChild(_pAppsNode);
	// Add the result element to the result xml.
	try {
		this->createRootResultsElement();
		this->setRootStatus(0);
		this->setTimestamp();
		_boDisabled = false;
	} catch (crOps::ExceptionNONFatal e) {
		CROPSWARNING("Can not initialize result document - %s", e.getMessage());
		_boDisabled = true;
	}
}

void crOps::ResultDocument::setTimestamp() {
	char szISOTime[50];
	::cyclOpsGetISOTime2(szISOTime, CROPSSIZEOF(szISOTime), ':');
	wchar_t wszISOTime[50];
	swprintf_s(wszISOTime, CROPSSIZEOF(wszISOTime), L"%S", szISOTime);
	DOMElement* pTimestampElement = _pResultDocument->createElement(L"timestamp");
	pTimestampElement->setTextContent(wszISOTime);
	_pRootNode->appendChild(pTimestampElement);
}

void crOps::ResultDocument::createRootResultsElement(void)
{
	// DO NOT ENABLE THE FOLLOWING LINE BECAUSE THIS FUNCTION IS CALLED BY initialize()
	// if (_boDisabled) return;

	// Create the <results> element to contain <status> and <message> elements.
	_pRootResultsElement = _pResultDocument->createElement(L"results");
	CROPSVARII(_pRootResultsElement, "%p");
	DOMNode* pRootNode = _pResultDocument->getDocumentElement();
	CROPSVARII(pRootNode, "%p");
	pRootNode->appendChild(_pRootResultsElement);
	DOMElement* pStatusElement = _pResultDocument->createElement(L"status");
	_pRootResultsElement->appendChild(pStatusElement);
	CROPSDEBUG("done");

}

void crOps::ResultDocument::addRootMessage(char* szMessage) {
	
	if (_boDisabled) return;

	wchar_t wszMessage[5000];
	swprintf(wszMessage, CROPSSIZEOF(wszMessage), L"%S", szMessage);
	DOMElement* pMessageElement = _pResultDocument->createElement(L"message");
	pMessageElement->setTextContent(wszMessage);
	_pRootResultsElement->appendChild(pMessageElement);
}

void crOps::ResultDocument::initializeRootStatus() {
	
	// Maintain a member variable with status.
	_iStatus = 0;

	// Create the <status> sub-element.
	wchar_t* wszStatus = L"SUCCESS";
	CROPSVARII(_pRootResultsElement, "%p");
	DOMNodeList* pStatusNodeList = _pRootResultsElement->getElementsByTagName(L"status");
	CROPSVARII(pStatusNodeList, "%p");
	if (pStatusNodeList == NULL || pStatusNodeList->getLength() < 1) {
		char* szMessage = "Could not get root status node list.";
		CROPSWARNING(szMessage);
		throw crOps::ExceptionNONFatal(szMessage);
	}
	DOMNode* pStatusNode = pStatusNodeList->item(0);
	if (pStatusNode == NULL) {
		char* szMessage = "Could not get root status node.";
		CROPSWARNING(szMessage);
		throw crOps::ExceptionNONFatal(szMessage);
	}
	CROPSVARII(pStatusNode, "%p");
	pStatusNode->setTextContent(wszStatus);
}


void crOps::ResultDocument::setRootStatus(int iStatus) {
	
	if (_boDisabled) return;

	// If the status being set is lower than what is already set, exit.
	if (iStatus <= _iStatus) {
		CROPSDEBUG("%d is lower than %d", iStatus, _iStatus);
		return;
	}

	// Maintain a member variable with status.
	_iStatus = iStatus;

	// Create the <status> sub-element.
	wchar_t* wszStatus = 0;
	switch (iStatus) {
		case 0:
			wszStatus = L"SUCCESS";
			break;
		case 1:
			wszStatus = L"WARNING";
			break;
		case 2:
			wszStatus = L"ERROR";
			break;
		default:
			wszStatus = L"FATAL";
			break;
	}
	CROPSVARII(_pRootResultsElement, "%p");
	DOMNodeList* pStatusNodeList = _pRootResultsElement->getElementsByTagName(L"status");
	CROPSVARII(pStatusNodeList, "%p");
	if (pStatusNodeList == NULL || pStatusNodeList->getLength() < 1) {
		char* szMessage = "Could not get root status node list.";
		CROPSWARNING(szMessage);
		// No exceptions, the result document should not crash the program.
		// throw crOps::ExceptionNONFatal(szMessage);
		return;
	}
	DOMNode* pStatusNode = pStatusNodeList->item(0);
	if (pStatusNode == NULL) {
		char* szMessage = "Could not get root status node.";
		CROPSWARNING(szMessage);
		// No exceptions, the result document should not crash the program.
		// throw crOps::ExceptionNONFatal(szMessage);
		return;
	}
	CROPSVARII(pStatusNode, "%p");
	pStatusNode->setTextContent(wszStatus);
}

char* crOps::ResultDocument::getString(void)
{
	if (_boDisabled) return NULL;

	DOMImplementation *pDOMImplementation = DOMImplementationRegistry::getDOMImplementation(L"LS");
	DOMLSSerializer *pDOMSerializer = ((DOMImplementationLS*)pDOMImplementation)->createLSSerializer();
	MemBufFormatTarget* pFormatTarget = new MemBufFormatTarget();
	DOMLSOutput *pDOMLSOutput = ((DOMImplementationLS*)pDOMImplementation)->createLSOutput();
	pDOMLSOutput->setByteStream(pFormatTarget);
	pDOMSerializer->write(_pResultDocument, pDOMLSOutput);
	const XMLByte* xmlByte = pFormatTarget->getRawBuffer();
	XMLSize_t iSize = pFormatTarget->getLen();
	char* pchResultXML = new char[iSize + 100];
	CROPSVARII(iSize, "%d");
	strcpy_s(pchResultXML, iSize + 50, (const char*) xmlByte);
	pDOMLSOutput->release();
	pDOMSerializer->release();
	delete pFormatTarget;
	return pchResultXML;
}

DOMNode* crOps::ResultDocument::addAppNode(char* szApp)
{
	if (_boDisabled) return NULL;

	DOMNode* pResultAppNode = _pResultDocument->createElement(L"app");
	_pAppsNode->appendChild(pResultAppNode);
	DOMNode* pNameNode = _pResultDocument->createElement(L"name");
	wchar_t wszApp[500];
	swprintf_s(wszApp, CROPSSIZEOF(wszApp), L"%S", szApp);
	CROPSVARII(wszApp, "%S");
	pNameNode->setTextContent(wszApp);
	pResultAppNode->appendChild(pNameNode);
	DOMNode* pServersNode = _pResultDocument->createElement(L"servers");
	pResultAppNode->appendChild(pServersNode);
	DOMNode* pResultsNode = _pResultDocument->createElement(L"results");
	pResultAppNode->appendChild(pResultsNode);
	return pResultAppNode;
}

void crOps::ResultDocument::setServerStatus(DOMNode* pAppResultNode, 
	XMLCh const * pXmlchServerName, wchar_t* wszResult)
{
	if (_boDisabled) return;

	DOMNodeList* pServersNodeList 
		= ((DOMElement*) pAppResultNode)->getElementsByTagName(L"servers");
	// If the nodelist is null or empty we have a fundamental problem with our program.
	if (pServersNodeList == NULL || pServersNodeList->getLength() < 1) {
		char* szMessage = "Servers node list could not be retrieved.";
		CROPSWARNING(szMessage);
		// No exceptions, the result document should not crash the program.
		// throw ExceptionFatal(szMessage);
		return;
	}
	DOMNode* pServersNode = pServersNodeList->item(0);
	if (pServersNode == NULL) {
		char* szMessage = "Servers node could not be retrieved.";
		CROPSWARNING(szMessage);
		// No exceptions, the result document should not crash the program.
		// throw ExceptionFatal(szMessage);
		return;
	}
	DOMNode* pServerNode = _pResultDocument->createElement(pXmlchServerName);
	pServerNode->setTextContent(wszResult);
	pServersNode->appendChild(pServerNode);
}

void crOps::ResultDocument::setAppStatus(DOMNode* pAppNode, wchar_t* wszStatus)
{
	if (_boDisabled) return;

	DOMNodeList* pResultsNodeList = ((DOMElement*) pAppNode)->getElementsByTagName(L"results");
	// An exception here is fatal folks, because we should have created the <results> node when
	// we created the <app> node.
	if (pResultsNodeList == NULL || pResultsNodeList->getLength() < 1) {
		char* szMessage = "Could not get <results> node.";
		CROPSWARNING(szMessage);
		// No exceptions, the result document should not crash the program.
		// throw ExceptionFatal(szMessage);
		return;
	}
	DOMNode* pResultsNode = pResultsNodeList->item(0);
	DOMNode* pStatusNode = _pResultDocument->createElement(L"status");
	pStatusNode->setTextContent(wszStatus);
	pResultsNode->appendChild(pStatusNode);
}

void crOps::ResultDocument::addAppError(DOMNode* pAppOutputNode, char* szError)
{
	if (_boDisabled) return;

	DOMNodeList* pResultsNodeList = ((DOMElement*) pAppOutputNode)->getElementsByTagName(L"results");
	if (pResultsNodeList == NULL || pResultsNodeList->getLength() < 1) {
		char* szError = "Could not get results node list.";
		CROPSWARNING(szError);
		// No exceptions, the result document should not crash the program.
		// throw ExceptionFatal(szError);
		return;
	}
	DOMNode* pResultsNode = pResultsNodeList->item(0);
	if (pResultsNode == NULL) {
		char* szError = "Could not get results node.";
		CROPSWARNING(szError);
		// No exceptions, the result document should not crash the program.
		// throw ExceptionFatal(szError);
		return;
	}
	DOMNode* pErrorNode = _pResultDocument->createElement(L"error");
	wchar_t wszError[1000];
	swprintf_s(wszError, CROPSSIZEOF(wszError), L"%S", szError);
	pErrorNode->setTextContent(wszError);
	pResultsNode->appendChild(pErrorNode);
}

void crOps::ResultDocument::addRootException(char* szMessage) {

	if (_boDisabled) return;

	size_t iSize = 2500;
	char* pchMessage = new char[iSize];
	sprintf_s(pchMessage, iSize, "%s", szMessage);
	this->addRootMessage(pchMessage);
	this->setRootStatus(2);
}




int crOps::ResultDocument::writeToFile(char* szResultFile)
{
	if (_boDisabled) {
		CROPSWARNING("Result document disabled.");
		return 1;
	}

	CROPSDEBUG(szResultFile, "%s");

	char* pchResultXML = this->getString();
	std::ofstream xmlResultFile;
	xmlResultFile.open (szResultFile);
	if (xmlResultFile.fail()) {
		CROPSWARNING("Could not open output file %s.", szResultFile);
		return 1;
	}
	xmlResultFile << pchResultXML;
	xmlResultFile.flush();
	xmlResultFile.close();
	return 0;
}


void crOps::ResultDocument::disable(void)
{
	_boDisabled = true;
}


void crOps::ResultDocument::addRootElement(wchar_t* wszElement, const char* szTextContent)
{
	if (_boDisabled) return;
	DOMElement* pElement = _pResultDocument->createElement(wszElement);
	if (pElement == NULL) {
		CROPSWARNING("Could not create <%S> element.", wszElement);
		return;
	}
	wchar_t wszTextContent[MAX_PATH];
	swprintf_s(wszTextContent, CROPSSIZEOF(wszTextContent), L"%S", szTextContent);
	pElement->setTextContent(wszTextContent);
	_pRootNode->appendChild(pElement);
}


void crOps::ResultDocument::writeToString(std::string* pstrResultXML)
{
	if (_boDisabled) {
		CROPSWARNING("Result document disabled.");
		return;
	}
	char* szResultXML = this->getString();
	CYCLOPSVAR(szResultXML, "%s");
	pstrResultXML->assign(szResultXML);
	CYCLOPSDEBUG("pstrResultXML = %s", pstrResultXML->c_str());
	delete szResultXML;
}

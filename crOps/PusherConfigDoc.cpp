#include "StdAfx.h"

#include <string.h>

#include "..\cyclOps\cyclOps.h"

#include "PusherConfigDoc.h"
#include "ExceptionNONFatalCrOps.h"
#include "Utility.h"

crOps::PusherConfigDoc::PusherConfigDoc(void)
{
}


crOps::PusherConfigDoc::~PusherConfigDoc(void)
{
}

void crOps::PusherConfigDoc::initialize(const char* szPusherConfigFile) {
	_pParserCrOpsConfig = new XercesDOMParser();
    CROPSVARII(_pParserCrOpsConfig, "%p");
    if (_pParserCrOpsConfig == NULL) {
            CROPS_THROW_FATAL("crOps config parser was NULL!!!");
    }
    _pParserCrOpsConfig->parse(szPusherConfigFile);
	CYCLOPSDEBUG("parse() complete.");
    _pCrOpsConfigDocument = _pParserCrOpsConfig->getDocument();
    CROPSVARII(_pCrOpsConfigDocument, "%p");
    if (_pCrOpsConfigDocument == NULL) {
            CROPS_THROW_FATAL("Could not open crOps configuration document %s.",
                            szPusherConfigFile);
    }
}

char* crOps::PusherConfigDoc::getResultXMLFile()
{
	DOMNodeList* pResultFileNodeList = _pPusherElement->getElementsByTagName(L"resultFile");
        if (pResultFileNodeList == NULL || pResultFileNodeList->getLength() < 1) {
                CROPSERROR("Could not get <resultFile> from %S", _pCrOpsConfigDocument->getDocumentURI());
                throw crOps::ExceptionNONFatal("Could not get <resultFile> element");
        }
        DOMNode* pResultFileNode = pResultFileNodeList->item(0);
        if (pResultFileNode == NULL) {
                CROPSERROR("Could not get <resultFile> from %S", _pCrOpsConfigDocument->getDocumentURI());
                throw crOps::ExceptionNONFatal("Could not get <resultFile> element");
        }
        XMLCh pXmlchResultFile[MAX_PATH];
		crOps::Utility::getTextContent(pResultFileNode, pXmlchResultFile, CROPSSIZEOF(pXmlchResultFile));
        char szHostname[1000];
        DWORD dwSize = CROPSSIZEOF(szHostname);
        if (!::GetComputerNameA(szHostname, &dwSize)) {
                CROPSERROR("Could not get hostname.");
                throw crOps::ExceptionNONFatal("Could not get hostname.");
        }
		CROPSVARII(szHostname, "%s");
		char szResultXMLFile[MAX_PATH];
        sprintf_s(szResultXMLFile, CROPSSIZEOF(szResultXMLFile), "%S", pXmlchResultFile);
		CROPSVARII(szResultXMLFile, "%s");
        char* pchResultXMLFile = ::cyclOpsReplace(szResultXMLFile, "${hostname}", szHostname);
		CROPSVARII(pchResultXMLFile, "%s");
		return pchResultXMLFile;
}

xercesc::DOMDocument* crOps::PusherConfigDoc::getCitrixDocument(void)
{

		DOMElement* pRootCrOps = _pCrOpsConfigDocument->getDocumentElement();
        if (pRootCrOps == NULL) {
                CROPS_THROW_FATAL("Could not get root node from %S", _pCrOpsConfigDocument->getDocumentURI());
        }
        CROPSVARII(pRootCrOps, "%p");
        DOMNodeList* pPusherNodeList = pRootCrOps->getElementsByTagName(L"pusher");
        if (pPusherNodeList == NULL || pPusherNodeList->getLength() < 1) {
                CROPS_THROW_FATAL("Could not get <pusher> element from %S.", _pCrOpsConfigDocument->getDocumentURI());
        }
        CROPSVARII(pPusherNodeList, "%p");
        _pPusherElement = (DOMElement*) pPusherNodeList->item(0);

        // We know that _pPusherElement is a element.
        DOMNodeList* pCitrixNodeList = _pPusherElement->getElementsByTagName(L"citrixApplicationList");
        if (pCitrixNodeList == NULL || pCitrixNodeList->getLength() < 1) {
                CROPS_THROW_FATAL("Could not get citrix app list element.");
        }
        CROPSVARII(pCitrixNodeList, "%p");
        DOMNode* pCitrixNode = pCitrixNodeList->item(0);
        CROPSVARII(pCitrixNode, "%p");
        if (pCitrixNode == NULL) {
                CROPS_THROW_FATAL("Could not get citrix node.");
        }
        XMLCh pxmlchCitrixConfigFile[MAX_PATH];
		crOps::Utility::getTextContent(pCitrixNode, pxmlchCitrixConfigFile, CROPSSIZEOF(pxmlchCitrixConfigFile));
		/* CROPSINFO("Using Citrix application list at %S.", pxmlchCitrixConfigFile); */

        // ALERT: You will never delete this parser.  And the pointer will go
        // out of scope before the document you are retrieving is finished being
        // used (actually before it is even begun to be used).  Will this be a problem?
        XercesDOMParser* pParserCitrixConfig = new XercesDOMParser();
        pParserCitrixConfig->parse(pxmlchCitrixConfigFile);
        xercesc::DOMDocument* pCitrixConfigDocument = pParserCitrixConfig->getDocument();
        if (pCitrixConfigDocument == NULL) {
                CROPS_THROW_FATAL("Could not get Citrix config document %S.", pxmlchCitrixConfigFile);
        }
        return pCitrixConfigDocument;
}

DOMElement* crOps::PusherConfigDoc::getPusherElement(void)
{
	return _pPusherElement;
}

void crOps::PusherConfigDoc::populateGrantNodeVector(char* szApplication, std::vector<DOMNode*>* pNodeVector) {
	CROPSDEBUG(szApplication, "%s");
	DOMNode* pApplicationNode = this->getApplicationNode(szApplication);
	DOMNode* pPermissionsNode = this->getNode(pApplicationNode, "permissions");
	this->populateNodeVector(pPermissionsNode, "grant", pNodeVector);
}

DOMNode* crOps::PusherConfigDoc::getApplicationNode(char* szApplication) {
	CROPSDEBUG("Hello.");
	DOMNodeList* pAppsNodeList = _pPusherElement->getElementsByTagName(L"applications");
	if (pAppsNodeList == NULL || pAppsNodeList->getLength() < 1) {
		// I consider this a pretty serious event...
		CROPS_THROW_FATAL("Could not get applications node list.");
	}
	DOMNode* pAppsNode = pAppsNodeList->item(0);
	if (pAppsNode == NULL) {
		char* szMessage = "Could not get applications node.";
		CROPSERROR(szMessage);
		throw crOps::ExceptionNONFatal(szMessage);
	}
	DOMNodeList* pAppNodeList 
		= ((DOMElement*) pAppsNode)->getElementsByTagName(L"application");
	if (pAppNodeList == NULL || pAppNodeList->getLength() < 1) {
		char* szMessage = "Could not get app node list.";
		CROPSERROR(szMessage);
		throw crOps::ExceptionNONFatal(szMessage);
	}

	// Iterate each <application>, looking to see if szAppName matches the <name> element.
	for (unsigned int i = 0; i < pAppNodeList->getLength(); ++i) {
		// We know these are elements already.
		DOMElement* pAppElement = (DOMElement*) pAppNodeList->item(i);
		try {
			char szTextContent[500];
			this->getChildElementTextContent(pAppElement, L"name", szTextContent, 
				CROPSSIZEOF(szTextContent));
			CROPSVARII(szTextContent, "%s");
			if (_stricmp(szApplication, szTextContent) == 0) {
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

/**
* Given an element, get the text content of the first child element
* with the tag name specified in wszElement.
**/
bool crOps::PusherConfigDoc::getChildElementTextContent(DOMElement* pElement, 
	wchar_t* wszChildElement, char* szTextContent,	size_t iSize) 
{
	//size_t iSize = CROPSSIZEOF(szTextContent);
	CROPSVARII(iSize, "%d");
	DOMNodeList* pNodeList = pElement->getElementsByTagName(wszChildElement);
	if (pNodeList == NULL) {
		CROPSERROR("No <%S> element found.", wszChildElement);
		throw crOps::ExceptionNONFatal("Element not found");
	}
	if (pNodeList->getLength() >= 1) {
		DOMNode* pNode = pNodeList->item(0);
		// I'm fairly certain this is an ELEMENT_NODE.
		XMLCh pxmlchTextContent[5000]; 
		crOps::Utility::getTextContent(pNode, pxmlchTextContent, CROPSSIZEOF(pxmlchTextContent));
		_snprintf_s(szTextContent, iSize, _TRUNCATE, "%S", pxmlchTextContent);
		return true;
	} else {
		CROPSERROR("No <%S> element found.", wszChildElement);
		return false;
	}
}

DOMNode* crOps::PusherConfigDoc::getNode(DOMNode* pNode, char* szElement) {
	CROPSVARII(szElement, "%s"); CROPSDEBUG("getNodeName() = %S", pNode->getNodeName());
	wchar_t wszElement[1000];
	_snwprintf_s(wszElement, sizeof(wszElement) / sizeof(wszElement[0]), _TRUNCATE, L"%S", szElement);
	CROPSVARII(wszElement, "'%s'");
	DOMNodeList* pNodeList = pNode->getChildNodes();
	CROPSVARII(pNodeList, "%p"); CROPSDEBUG("getLength() = %d", pNodeList->getLength());
	if (pNodeList == NULL || pNodeList->getLength() == 0) {
		CROPS_THROW_NON_FATAL("The node %s cannot be found.", szElement);
	}
	for (unsigned int i = 0; i < pNodeList->getLength(); ++i) {
		DOMNode* pChildNode = pNodeList->item(i);
		const XMLCh* xmlchNodeName = pChildNode->getNodeName();
		CROPSVARII(xmlchNodeName, "'%S'");
		if (_wcsicmp(wszElement, xmlchNodeName) == 0) {
			return pChildNode;
		}
	}
	CROPS_THROW_NON_FATAL("The node %s cannot be found.", szElement);
}

void crOps::PusherConfigDoc::populateNodeVector(DOMNode* pNode, char* szElement, std::vector<DOMNode*>* pNodeVector) { 
	CROPSVARII(szElement, "%s"); CROPSDEBUG("getNodeName() = %S", pNode->getNodeName());
	wchar_t wszElement[1000];
	_snwprintf_s(wszElement, sizeof(wszElement) / sizeof(wszElement[0]), _TRUNCATE, L"%S", szElement); CROPSVARII(wszElement, "'%S'");
	DOMNodeList* pNodeList = pNode->getChildNodes(); CROPSVARII(pNodeList, "%p"); CROPSDEBUG("getLength() = %d", pNodeList->getLength());
	if (pNodeList == NULL || pNodeList->getLength() == 0) {
		CROPS_THROW_NON_FATAL("The node %s cannot be found.", szElement);
	}
	for (unsigned int i = 0; i < pNodeList->getLength(); ++i) {
		DOMNode* pChildNode = pNodeList->item(i); 
		if (pChildNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			const XMLCh* xmlchNodeName = pChildNode->getNodeName(); CROPSVARII(xmlchNodeName, "'%S'");
			if (_wcsicmp(xmlchNodeName, wszElement) == 0) {
				pNodeVector->push_back(pChildNode);
			}
		}
	}
}
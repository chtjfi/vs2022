#include "StdAfx.h"
#include "JNIPusherInputDocument.h"
#include "..\cyclOps\cyclOps.h"
#include <xercesc\dom\DOM.hpp>
#include <xercesc\parsers\XercesDOMParser.hpp>
#include <xercesc\framework\MemBufInputSource.hpp>
#include <xercesc\dom\DOMNodeList.hpp>
#include "ExceptionFatal.h"
#include "crOpsPrivate.h"

crOps::JNIPusherInputDocument::JNIPusherInputDocument(char* szInputXML)
{
	xercesc::XMLPlatformUtils::Initialize();
	xercesc::XercesDOMParser* pParser = new xercesc::XercesDOMParser();		CYCLOPSVAR(pParser, "%p");
	XMLByte* xmlByteInput = (XMLByte*) szInputXML;							CYCLOPSVAR(xmlByteInput, "%s");
	xercesc::MemBufInputSource source(xmlByteInput, strlen(szInputXML), "BufferID");
	pParser->parse(source);
	_pDocument = pParser->getDocument();
	_pRootElement = _pDocument->getDocumentElement();						CYCLOPSVAR(_pDocument, "%p");
}


crOps::JNIPusherInputDocument::~JNIPusherInputDocument()
{

}


char* crOps::JNIPusherInputDocument::getPushTarget(void)
{
	// NULL means push to all servers which is all we support at the moment.
	// In the future the caller will be able to specify server to push to.
	return NULL;
}


bool crOps::JNIPusherInputDocument::getDebug(void)
{
	bool boDebug = false;
	try {
		std::string strDebug = this->getProperty(std::wstring(L"debug"));
		boDebug = _stricmp(strDebug.c_str(), "true") == 0;
	} catch (...) { 
		/* Ignore. */
	}
	return boDebug;
}


bool crOps::JNIPusherInputDocument::getListOnly(void)
{
	return false;
}


std::string crOps::JNIPusherInputDocument::getApplication(void)
{
	return this->getProperty(std::wstring(L"application"));
}


void crOps::JNIPusherInputDocument::getPusherConfig(std::string* pstrConfigFile)
{
	CYCLOPSDEBUG("Hello.");
	xercesc::DOMNodeList* pNodeList = _pRootElement->getChildNodes();
	if (pNodeList == NULL || pNodeList->getLength() == 0) { 
		throw ExceptionFatal("No child nodes under root of input document.");
	}
	for (unsigned int i = 0; i < pNodeList->getLength(); ++i) {
		xercesc::DOMNode* pNode = pNodeList->item(i);
		if (pNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			const XMLCh* xmlchNodeName = pNode->getNodeName();
			CYCLOPSVAR(xmlchNodeName, "%S");
			if (wcscmp(xmlchNodeName, L"pusherConfig") == 0) {
				// Return the text content.
				const XMLCh* xmlchTextContent = pNode->getTextContent();
				CYCLOPSVAR(xmlchTextContent, "%S");
				size_t iSize = wcslen(xmlchTextContent);
				char* pchTextContent = new char[iSize + 2];
				_snprintf_s(pchTextContent, iSize + 1, _TRUNCATE, "%S", xmlchTextContent);
				pstrConfigFile->assign(pchTextContent);
				delete pchTextContent;
			}
		}
	}
}

std::string crOps::JNIPusherInputDocument::getProperty(std::wstring wstrProperty)
{
	xercesc::DOMNodeList* pNodeList = _pRootElement->getChildNodes();
	if (pNodeList == NULL || pNodeList->getLength() == 0) { 
		throw ExceptionFatal("No child nodes under root of input document.");
	}
	for (unsigned int i = 0; i < pNodeList->getLength(); ++i) {
		xercesc::DOMNode* pNode = pNodeList->item(i);
		if (pNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			const XMLCh* xmlchNodeName = pNode->getNodeName();
			if (wstrProperty.compare(xmlchNodeName) == 0) {
				// Return the text content.
				const XMLCh* xmlchTextContent = pNode->getTextContent();
				XMLCh xmlchTextContentNONConst[1000];
				_snwprintf_s(xmlchTextContentNONConst, CROPSSIZEOF(xmlchTextContentNONConst), _TRUNCATE, L"%s", xmlchTextContent);
				xercesc::XMLString::trim(xmlchTextContentNONConst);
				std::wstring wstrTextContent(xmlchTextContentNONConst);
				std::string strTextContent(wstrTextContent.begin(), wstrTextContent.end());
				return strTextContent;
			}
		}
	}
	// This is non fatal because sometimes it is acceptable for a property not to be present,
	// for example the application property is not present when all apps are replicating.
	CROPS_THROW_NON_FATAL("Property '%S' not found in input document.", wstrProperty.c_str());
}


std::string crOps::JNIPusherInputDocument::getCrOpsLog(void)
{
	return this->getProperty(std::wstring(L"crOpsLog"));
}

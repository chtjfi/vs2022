#pragma once

#include <xercesc\dom\DOM.hpp>
#include <xercesc\framework\MemBufFormatTarget.hpp>
#include <stdio.h>
#include "crOpsPrivate.h"
#include "ExceptionFatal.h"
#include <fstream>

using namespace xercesc;

namespace crOps {
	class ResultDocument
	{
	private: 
		xercesc::DOMDocument* _pResultDocument;
		DOMNode* _pAppsNode;
		DOMNode* _pRootNode;
		DOMElement* _pRootResultsElement;
		void createRootResultsElement(void);
		bool _boDisabled;
		int _iStatus;
		void setTimestamp(void);
		void initializeRootStatus();
	public:
		void     initialize(void);
		void     addRootMessage(char* szMessage);
		void     setRootStatus(int iStatus);
		DOMNode* addAppNode(char* szApp);
		char*    getString(void);
		void     setServerStatus(DOMNode* pAppResultNode, XMLCh const * pXmlchServerName, wchar_t* wszStatus);
		void     setAppStatus(DOMNode* pAppNode, wchar_t* wszStatus);
		void     addAppError(DOMNode* pAppOutputNode, char* szError);
		void     addRootException(char* szMessage);
		void     addRootElement(wchar_t* wszElement, const char* szTextContent);
		ResultDocument(void);
		~ResultDocument(void);
		int writeToFile(char* szResultFile);
		void disable(void);
		
		void writeToString(std::string* pstrResultXML);
	};
}
#pragma once

#include <stdio.h>
#include <vector>

#include <xercesc\dom\DOM.hpp>
#include <xercesc\parsers\XercesDOMParser.hpp>

#include "crOpsPrivate.h"

namespace crOps {
	class PusherConfigDoc
	{
	private:
		xercesc::XercesDOMParser* _pParserCrOpsConfig;
		xercesc::DOMDocument* _pCrOpsConfigDocument;
		xercesc::DOMElement* _pPusherElement;
	public:
		void initialize(const char* szPusherConfigFile);
		char* getResultXMLFile();
		xercesc::DOMDocument* getCitrixDocument(void);
		xercesc::DOMElement* getPusherElement(void);
		void populateGrantNodeVector(char* szApplication, std::vector<xercesc::DOMNode*>* pGrantNodeVector);
		xercesc::DOMNode* getApplicationNode(char* szApplication);
		bool getChildElementTextContent(xercesc::DOMElement* pElement, 
			wchar_t* wszChildElement, char* szTextContent,	size_t iSize);
		xercesc::DOMNode* getNode(xercesc::DOMNode* pNode, char* szElement);
		void populateNodeVector(xercesc::DOMNode* pNode, char* szElement, std::vector<xercesc::DOMNode*>* pNodeVector);
		PusherConfigDoc(void);
		~PusherConfigDoc(void);
	};
}
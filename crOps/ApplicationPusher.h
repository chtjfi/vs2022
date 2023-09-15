#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <xercesc\dom\DOM.hpp>
#include <xercesc\parsers\XercesDOMParser.hpp>
#include "crOps.h"
#include "ExceptionFatal.h"
#include "ExceptionNONFatalCrOps.h"
#include "..\slOps\slOps.h"
#include "PusherConfigDoc.h"
#include "ResultDocument.h"
#include "..\cyclOps\Logger.h"

namespace crOps {

	class ApplicationPusher
	{
		crOps::PusherConfigDoc* _pPusherConfigDocument;
		crOps::ResultDocument* _pResultDocument;
		xercesc::DOMNode* _pResultAppsNode;
		void processDocumentCitrix(xercesc::DOMDocument* pDocumentCitrix);
		void processAppNodeList(xercesc::DOMNodeList* pAppNodeList);
		void processAppNode(xercesc::DOMNode* pAppNode);
		bool getChildElementTextContent(xercesc::DOMElement* pElement, wchar_t* wszChildElement, 
			char* szTextContent, size_t iSize);
		bool getChildElementTextContent(xercesc::DOMElement* pElement, wchar_t* wszChildElement, 
			wchar_t* wszTextContent, size_t iSize);

		// This is initialized to trupusee in constructor and set to false if any error 
		// occurs anywhere.
		bool _boErrorsOccurred;
		bool pushApplicationToServer(xercesc::DOMNode* pServerNode, char* szAppName, char* szAppTargetDir,
			xercesc::DOMNode* pResultAppNode);
		void setPermissions(char* szTargetUNC, char* szAppName);
		void createTargetUNC(const XMLCh* pXmlchServer, char* szAppTargetDir,
			char* szTargetUNC, size_t iSize);
		char* _pchPushTarget;
		bool isPushTarget(const XMLCh* pXmlchServerName);
		bool syncDirectories(char* szSourceDirectory, char* szTargetDirectory, bool boPurge);
		bool _boAtLeastOneServerFound;
		bool _boAtLeastOneApplicationFound;
		void getSourceDirectory(char* szAppName, char* szSourceDirectory, size_t iSize);
		bool _boListOnly;
		void createPusherResultsElement(void);
		bool _boWarningsOccurred;
		char* _pchApplication;
		bool isApplication(const char* pchApplication);
		void processGrantNode(xercesc::DOMNode* pGrantNode, char* szTargetUNC);
		std::vector<std::wstring> _vectorOfOfflineServers;
	public:
		ApplicationPusher(void);
		~ApplicationPusher(void);
		void pushApplications(
			PusherConfigDoc* pPusherConfigDocument, 
			crOps::ResultDocument* pResultDocument, 
			bool boListOnly);
		bool wereThereErrors(void);
		void setPushTarget(const char* pchPushTarget);
		bool wereThereWarnings(void);
		void setApplication(const char* szApplication);
	private:
		bool isPushTargetOnAppList(xercesc::DOMNode* pAppNode);
	public:
		DOMElement* getApplicationElement(char* szAppName);
		bool getPurgeValue(char* szAppName);
	};
}

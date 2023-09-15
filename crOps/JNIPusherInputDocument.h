#pragma once

#include <xercesc\dom\DOMDocument.hpp>
#include <xercesc\dom\DOMElement.hpp>
#include <string>

namespace crOps {
	class JNIPusherInputDocument
	{
	private:
		xercesc::DOMDocument* _pDocument;
	public:
		JNIPusherInputDocument(char* szInputXML);
		~JNIPusherInputDocument(void);
		char* getPushTarget(void);
		bool getDebug(void);
		bool getListOnly(void);
		std::string getApplication(void);
		void getPusherConfig(std::string* pstrPusherConfig);
	private:
		xercesc::DOMElement* _pRootElement;
		std::string getProperty(std::wstring wstrProperty);
	public:
		std::string getCrOpsLog(void);
	};
}
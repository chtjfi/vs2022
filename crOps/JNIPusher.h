#pragma once

#include "JNIPusherInputDocument.h"
#include <xercesc\dom\DOMDocument.hpp>
#include <string>

namespace crOps {
	class JNIPusher
	{
	private:
		crOps::JNIPusherInputDocument* _pInputDoc;
		std::string _strResultXML;
		/* std::string _strLogFile; // NEVER USED! */
	public:
		JNIPusher(JNIPusherInputDocument* pInputDoc);
		~JNIPusher(void);
		void pushApplications(void);
		std::string* getResultXML(void);
		static void createFatalXML(char* szMessage, char* szXML, size_t iSize);
	};
}
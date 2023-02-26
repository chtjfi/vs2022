#include "StdAfx.h"
#include "JNIRequest.h"

#include <xercesc\parsers\XercesDOMParser.hpp>
#include <xercesc\framework\MemBufInputSource.hpp>

#include <boost/algorithm/string.hpp>

#include "cyclOps.h"
#include "XMLigator.h"

using std::string;

// using xercesc::DOMDocument;
using xercesc::XMLPlatformUtils;
using xercesc::XercesDOMParser;
using xercesc::MemBufInputSource;

namespace cyclOps {
	JNIRequest::JNIRequest(JNIEnv *env, jobject obj, jstring input)
	{
		char *szInputXML = (char*) env->GetStringUTFChars(input, NULL);
		XMLPlatformUtils::Initialize();

		/* TODO:  This should be a member variable. */
		XercesDOMParser* pParser = new XercesDOMParser();		CYCLOPSVAR(pParser, "%p");
		
		XMLByte* xmlByteInput = (XMLByte*) szInputXML;							CYCLOPSVAR(xmlByteInput, "%s");
		xercesc::MemBufInputSource source(xmlByteInput, strlen(szInputXML), "BufferID");
		pParser->parse(source);

		/* TODO:  This should be a member variable. */
		xercesc::DOMDocument* pDocument = pParser->getDocument();

		_pRootElement = pDocument->getDocumentElement();	

	}


	JNIRequest::~JNIRequest(void)
	{
	}
	/*const char* JNIRequest::NO_LOG = "NO_LOG";*/
	std::string JNIRequest::getTextContentRelativeToRoot(const std::string& strElement) const { CYCLOPSDEBUG("Hello.");
		return cyclOps::XMLigator::getTextContent(this->_pRootElement, strElement);
	}
	std::string JNIRequest::getLogPath() const {
		/* Throws an std::exception if no log file was specified. */
		return this->getTextContentRelativeToRoot("logPath");
	}

	bool JNIRequest::getDebug() const {
		try {
			string strDebug = this->getTextContentRelativeToRoot("debug");
			return boost::iequals(strDebug, "true");
		} catch (...) {
			return false;
		}
	}

	string JNIRequest::getProperty(const string& strProperty) const {
		return cyclOps::XMLigator::getTextContent(this->_pRootElement, strProperty);
	}

	bool JNIRequest::getPropertyAs_bool(const string& strProp, const bool& boDefault) {
		string strValue = this->getProperty(strProp);
		if (_stricmp(strValue.c_str(), "tRuE") == 0) {
			return true;
		} else if (_stricmp(strValue.c_str(), "fAlSe") == 0) {
			return false;
		}
		return boDefault;
		
	}
}
#include "StdAfx.h"
#include "JNIResponse.h"

#include <stdio.h>

#include "cyclOps.h"
#include "XMLigator.h"

using std::wstring;
using std::string;
using std::map;

// using xercesc::DOMDocument;
using xercesc::DOMElement;

namespace cyclOps { 
	JNIResponse::JNIResponse(void)
	{
	}


	JNIResponse::~JNIResponse(void)
	{
	}

	void JNIResponse::putProperty(const string strProperty, const char* szFormat, ...) { CYCLOPSDEBUG("%s/%s", strProperty.c_str(), szFormat);
		va_list vaList;
		va_start(vaList, szFormat);
		char szValue[10000];
		_vsnprintf_s(szValue, sizeof(szValue) / sizeof(szValue[0]), _TRUNCATE, szFormat, vaList); CYCLOPSVAR(szValue, "%s");
		this->_propertyMap[strProperty] = szValue;
	}

	void JNIResponse::putProperty(const string& property, const string& value) {
		this->putProperty(property, "%s", value.c_str());
	}

	void JNIResponse::putProperty(const std::string& property, const long& value) {
		string s = std::to_string(value);
		this->putProperty(property, s);
	}

	string JNIResponse::getPropertyMapAsXMLString() { CYCLOPSDEBUG("Hello.");
		xercesc::DOMDocument* pDocument = cyclOps::XMLigator::createDocument(L"root"); CYCLOPSDEBUG("document = %p", pDocument); CYCLOPSDEBUG("_propertyMap = %p/%d", &_propertyMap, _propertyMap.size());
		DOMElement* pRootElement = pDocument->getDocumentElement();
		DOMNode* pNodeProperties = pDocument->createElement(L"properties");
		pRootElement->appendChild(pNodeProperties);
		for (map<string, string>::iterator iterator = _propertyMap.begin();
			iterator != _propertyMap.end();
			++iterator) 
		{
			wstring wstrProperty = wstring(iterator->first.begin(), iterator->first.end()); CYCLOPSDEBUG("wstrProperty = %S", wstrProperty.c_str());
			wstring wstrValue = wstring(iterator->second.begin(), iterator->second.end()); CYCLOPSDEBUG("wstrValue = %S", wstrValue.c_str());
			DOMElement* element = pDocument->createElement(wstrProperty.c_str()); CYCLOPSDEBUG("createElement() complete.");
			element->setTextContent(wstrValue.c_str()); CYCLOPSDEBUG("setTextContent() complete.");
			pNodeProperties->appendChild(element); CYCLOPSDEBUG("appendChild() complete.");
		}
		for (map<string, string>::iterator iterator = _resultMap.begin();
			iterator != _resultMap.end();
			++iterator) 
		{
			wstring wstrProperty = wstring(iterator->first.begin(), iterator->first.end()); CYCLOPSDEBUG("wstrProperty = %S", wstrProperty.c_str());
			wstring wstrValue = wstring(iterator->second.begin(), iterator->second.end()); CYCLOPSDEBUG("wstrValue = %S", wstrValue.c_str());
			DOMElement* element = pDocument->createElement(wstrProperty.c_str()); CYCLOPSDEBUG("createElement() complete.");
			element->setTextContent(wstrValue.c_str()); CYCLOPSDEBUG("setTextContent() complete.");
			pRootElement->appendChild(element); CYCLOPSDEBUG("appendChild() complete.");
		}

		this->appendCustomChildren(pDocument); 
		return XMLigator::documentToString(pDocument);
	}

	void JNIResponse::setStatus(string strStatus) {
		this->_resultMap["status"] = strStatus;
	}

	void JNIResponse::setMessage(string strMessage) {
		this->_resultMap["message"] = strMessage;
	}

	void JNIResponse::appendCustomChildren(xercesc::DOMDocument* pRootElement) {
		/* Do nothing.  This is for derived classes to add additional info to the XML. */
	}
}


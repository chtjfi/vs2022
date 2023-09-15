#ifndef JNIRESPONSE_H
#define JNIRESPONSE_H

#include <string>
#include <map>

#include <xercesc\dom\DOMDocument.hpp>
#include <xercesc\dom\DOMElement.hpp>

namespace cyclOps { 
	class JNIResponse
	{
	public:
		JNIResponse(void);
		~JNIResponse(void);
		void putProperty(const std::string strProperty, const char* szFormat, ...);
		void putProperty(const std::string& property, const std::string& value);
		void putProperty(const std::string& property, const long& value);
		std::string getPropertyMapAsXMLString();
		virtual void appendCustomChildren(xercesc::DOMDocument* pRootElement);
		void setStatus(std::string);
		void setMessage(std::string);
	private:
		std::map<std::string, std::string> _propertyMap;
		std::map<std::string, std::string> _resultMap;
	};
}

#endif
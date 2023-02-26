#ifndef CYCLOPS_JNIREQUEST_H
#define CYCLOPS_JNIREQUEST_H

#include "jni.h"

#include <string>
#include <xercesc\dom\DOM.hpp>

using xercesc::DOMElement;

namespace cyclOps {
	class JNIRequest
	{
	public:
		JNIRequest(JNIEnv *env, jobject obj, jstring input);
		~JNIRequest(void);
		std::string getTextContentRelativeToRoot(const std::string& strElement) const;
		std::string getLogPath(void) const;
		bool getDebug() const;
		std::string getProperty(const std::string& strProperty) const;
		/* static const char* NO_LOG; */
		bool getPropertyAs_bool(const std::string& strProp, const bool& boDefault);
	private:
		DOMElement* _pRootElement;

		/* Prohibit copy constructor and assignment operator by making them private and not defining them. */
		JNIRequest(const JNIRequest&);
		JNIRequest& operator=(const JNIRequest&);
	};
}

#endif
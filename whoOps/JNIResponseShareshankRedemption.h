#ifndef JNIRESPONSESHARESHANKREDEMPTION_H
#define JNIRESPONSESHARESHANKREDEMPTION_H

#include <vector>
#include <string>

#include <xercesc\dom\DOMDocument.hpp>
#include <xercesc\dom\DOMElement.hpp>

#include "..\cyclOps\JNIResponse.h"

#include "Share.h"

namespace whoOps {
	class JNIResponseShareshankRedemption :
		public cyclOps::JNIResponse
	{
	public:
		JNIResponseShareshankRedemption(void);
		~JNIResponseShareshankRedemption(void);
		virtual void appendCustomChildren(xercesc::DOMDocument* pRootElement);
		void addShare(const Share& share);
	private:
		void addShareNode(xercesc::DOMDocument* pDocument, xercesc::DOMElement* pSharesElement, const Share& share);
		void addACENode(xercesc::DOMDocument* pDocument, xercesc::DOMElement* pACLElement, const ShareACE& ace);
		void appendChild(xercesc::DOMDocument* pDocument, xercesc::DOMElement* pParent, const std::wstring& wstrElement, const std::string& strText);
		std::vector<Share> _vectorOfShares;
	};
}

#endif
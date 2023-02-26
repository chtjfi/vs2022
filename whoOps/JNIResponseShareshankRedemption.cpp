#include "StdAfx.h"

#include "..\cyclOps\cyclOps.h"

#include "ExceptionNullSecurityDescriptor.h"

#include "JNIResponseShareshankRedemption.h"

// using xercesc::DOMDocument;
using xercesc::DOMElement;

using std::wstring;
using std::vector;
using std::string;

namespace whoOps {
	JNIResponseShareshankRedemption::JNIResponseShareshankRedemption(void)
	{
	}


	JNIResponseShareshankRedemption::~JNIResponseShareshankRedemption(void)
	{
	}

	void JNIResponseShareshankRedemption::appendCustomChildren(xercesc::DOMDocument* pDocument) { CYCLOPSDEBUG("Hello.");
		/* This is called from JNIResponse::getPropertyMapAsXMLString() */
		DOMElement* pRootElement = pDocument->getDocumentElement();
		DOMElement* pSharesElement = pDocument->createElement(L"shares");
		pRootElement->appendChild(pSharesElement);
		for (size_t i = 0; i < _vectorOfShares.size(); ++i) {
			this->addShareNode(pDocument, pSharesElement, _vectorOfShares[i]);
		}
	}

	void JNIResponseShareshankRedemption::appendChild(xercesc::DOMDocument* pDocument, DOMElement* pParent, const wstring& wstrElement, const string& strText) { CYCLOPSDEBUG("appendChild(%S, %s)", wstrElement.c_str(), strText.c_str());
		wstring wstrText(strText.begin(), strText.end());
		DOMElement* pElement = pDocument->createElement(wstrElement.c_str());
		pElement->setTextContent(wstrText.c_str());
		pParent->appendChild(pElement);
	}

	void JNIResponseShareshankRedemption::addShareNode(xercesc::DOMDocument* pDocument, DOMElement* pSharesElement, const Share& share) { CYCLOPSDEBUG("Hello.");
		/* This is called from JNIResponse::getPropertyMapAsXMLString() */
		DOMElement* pShareElement = pDocument->createElement(L"share");
		
		this->appendChild(pDocument, pShareElement, L"name", share.getName());
		this->appendChild(pDocument, pShareElement, L"remark", share.getRemark());
		this->appendChild(pDocument, pShareElement, L"server", share.getServer());
		this->appendChild(pDocument, pShareElement, L"path", share.getPath());
		this->appendChild(pDocument, pShareElement, L"isSecurityDescriptorNull", share.isSecurityDescriptorNull() ? "true" : "false");
		this->appendChild(pDocument, pShareElement, L"totalGB", std::to_string(share.getTotalGB()));
		this->appendChild(pDocument, pShareElement, L"freeGB", std::to_string(share.getFreeGB()));
		DOMElement* pACLElement = pDocument->createElement(L"acl"); CYCLOPSDEBUG("createElement('acl') complete.");
		pShareElement->appendChild(pACLElement);  CYCLOPSDEBUG("appendChild() complete.");
		try {
			const vector<ShareACE> aceVector = share.getACEVector(); 
			for (size_t i = 0; i < aceVector.size(); ++i) {
				this->addACENode(pDocument, pACLElement, aceVector[i]);
			}
		} catch (const whoOps::ExceptionNullSecurityDescriptor& e) {
			CYCLOPSDEBUG("%s - %s", typeid(e).name(), e.what());
		}
		pSharesElement->appendChild(pShareElement);
	}

	void JNIResponseShareshankRedemption::addACENode(xercesc::DOMDocument* pDocument, DOMElement* pACLElement, const ShareACE& ace) { CYCLOPSDEBUG("Hello.");
		/* This is called from JNIResponse::getPropertyMapAsXMLString() */
		const string& strAccount = ace.getAccount(); 
		const string& strDomain = ace.getDomain();
		const string& strMask = ace.getMaskString();
		DOMElement* pACEElement = pDocument->createElement(L"ace");
		DOMElement* pAccountElement = pDocument->createElement(L"account");
		DOMElement* pDomainElement = pDocument->createElement(L"domain");
		DOMElement* pMaskElement = pDocument->createElement(L"mask");
		pAccountElement->setTextContent(wstring(strAccount.begin(), strAccount.end()).c_str());
		pDomainElement->setTextContent(wstring(strDomain.begin(), strDomain.end()).c_str());
		pMaskElement->setTextContent(wstring(strMask.begin(), strMask.end()).c_str());
		pACLElement->appendChild(pACEElement);
		pACEElement->appendChild(pAccountElement);
		pACEElement->appendChild(pDomainElement);
		pACEElement->appendChild(pMaskElement);
		
	}

	void JNIResponseShareshankRedemption::addShare(const Share& share) {
		_vectorOfShares.push_back(share);
	}
}
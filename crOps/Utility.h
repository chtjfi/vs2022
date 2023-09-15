#pragma once

#include <xercesc\dom\DOM.hpp>

using namespace xercesc;

namespace crOps {
	class Utility
	{
	public:
		Utility(void);
		~Utility(void);
		static void getTextContent(DOMNode* pNode, XMLCh* pXMLCHReturn, size_t iSize);
	};
}
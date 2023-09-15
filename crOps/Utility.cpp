#include "StdAfx.h"
#include "Utility.h"
#include <stdio.h>


crOps::Utility::Utility(void)
{
}


crOps::Utility::~Utility(void)
{
}


void crOps::Utility::getTextContent(DOMNode* pNode, XMLCh* pXMLCHReturn, size_t iSize)
{
	const XMLCh* xmlchTextContent = pNode->getTextContent();
	swprintf_s(pXMLCHReturn, iSize, L"%s", xmlchTextContent);
	XMLString::trim(pXMLCHReturn);
}

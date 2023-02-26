#include "StdAfx.h"

#include <stdlib.h>

#include <string>
#include <fstream>

#include "XMLigator.h"
#include "cyclOps.h"
#include <boost/algorithm/string.hpp> 
#include <xercesc\dom\DOM.hpp>
#include <xercesc\dom\DOMLSOutput.hpp>
#include <xercesc\sax\SAXException.hpp>
#include <xercesc\dom\DOMImplementation.hpp>
#include <xercesc\dom\DOMImplementationLS.hpp>
#include <xercesc\dom\DOMImplementationRegistry.hpp>
#include <xercesc\dom\DOMLSSerializer.hpp>
#include <xercesc\framework\MemBufFormatTarget.hpp>
#include <xercesc\framework\MemBufInputSource.hpp>

using xercesc::XMLPlatformUtils;
using xercesc::XercesDOMParser;
using xercesc::DOMImplementation;
using xercesc::DOMImplementationLS;
using xercesc::DOMImplementationRegistry;
using xercesc::DOMLSSerializer;
using xercesc::DOMLSOutput;
using xercesc::MemBufFormatTarget;
using xercesc::MemBufInputSource ;

using std::wstring;
using std::string;
using std::vector;
using std::map;


namespace cyclOps {

cyclOps::XMLigator::XMLigator() : m_pDocument(NULL), m_pParser(NULL) {
}

cyclOps::XMLigator::~XMLigator(void)
{
	try {
		if (m_pDocument) {
			// Do not re-enable the next line because it causes a "pure virtual function call". 
			// m_pDocument->release();
		}
		if (m_pParser) {
			delete m_pParser;
		}
	} catch (...) {
		// Do nothing. 
	}
}

void cyclOps::XMLigator::initializeFromFilename(const string&  strFile)
{
	this->m_strFile = strFile;						CYCLOPSDEBUG("m_strFile = %s", m_strFile.c_str());
	try {
		XMLPlatformUtils::Initialize();
		m_pParser = new XercesDOMParser();	CYCLOPSDEBUG("m_pParser = %p", m_pParser);
		m_pParser->parse(m_strFile.c_str());		CYCLOPSDEBUG("Function parse() succesful.");
		m_pDocument = m_pParser->getDocument();		CYCLOPSDEBUG("m_pDocument = %p", m_pDocument); 
		if (m_pDocument == 0) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "Error initializing XML document '%s'.  Document is null.", m_strFile.c_str());
		}
	} catch (const xercesc::XMLException& e) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "Error initializing XML document %s.  EXCEPTION: %s - %s", m_strFile.c_str(), typeid(e).name(), e.getMessage());
	} catch (const xercesc::SAXException& e) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "Error initializing XML document %s.  EXCEPTION: %s - %s", m_strFile.c_str(), typeid(e).name(), e.getMessage());
	} catch (const xercesc::DOMException& e) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "Error initializing XML document %s.  EXCEPTION: %s - %s", m_strFile.c_str(), typeid(e).name(), e.getMessage());
	}
}

void cyclOps::XMLigator::initializeFromString(const string&  strXML) {
	try {
		XMLPlatformUtils::Initialize();
		m_pParser = new XercesDOMParser();	
		/* You save string as member variable so that it remains available for life of this object. */
		_strXML = strXML;
		XMLByte* xmlByteInput = (XMLByte*) _strXML.c_str();
		MemBufInputSource source(xmlByteInput, _strXML.length(), "BufferID");
		m_pParser->parse(source);
		m_pDocument = m_pParser->getDocument();
	} catch (const xercesc::XMLException& e) {
		CYCLOPSERROR("EXCEPTION: %s - %s", typeid(e).name(), e.getMessage());
	} catch (const xercesc::SAXException& e) {
		CYCLOPSERROR("EXCEPTION: %s - %s", typeid(e).name(), e.getMessage());	
	} catch (const xercesc::DOMException& e) {
		CYCLOPSERROR("EXCEPTION: %s - %s", typeid(e).name(), e.getMessage());		
	} catch (...) {
		CYCLOPSERROR("Unknown exception caught.");
	}
}


string cyclOps::XMLigator::getTextContent(const string&  strElement) const { 
	wstring wstrContent = this->getTextContentW(strElement);
	string strContent(wstrContent.begin(), wstrContent.end());
	return strContent;
}

wstring cyclOps::XMLigator::getTextContentW(const string&  strElement) const {
	vector<wstring> textContentVector;
	this->getTextContentVectorW(strElement, textContentVector); 
	/* No need to check vector for size > 0 because it is done in getTextContentVectorW(). */
	return textContentVector[0];
}

int cyclOps::XMLigator::getTextContentAs_int(const std::string& element) {
	wstring wstrValue = this->getTextContentW(element);
	return std::stoi(wstrValue);
}

void cyclOps::XMLigator::getTextContentVector(const string& strPath, vector<string>& vectorOfContent) const {
	vector<wstring> vectorOfWstrings;
	this->getTextContentVectorW(strPath, vectorOfWstrings);
	for (int i = 0; i < vectorOfWstrings.size(); ++i) {
		string content(vectorOfWstrings[i].begin(), vectorOfWstrings[i].end());
		vectorOfContent.push_back(content);
	}
}


void cyclOps::XMLigator::getTextContentVectorW(const string& strPathToElement, vector<wstring>& vectorOfContent) const {

	// Now split the string into its elements.  
	// The first element om the vector will be a blank string, because the path started with the delimiter '/'.
	vector<string> elementPathVector = this->getElementPathVectorAbsolute(strPathToElement);

	// There is a special case where the user might have asked for the text content of the root node.
	// In that case there will be no more elements in the elementVector.  In this case get the root
	// node text content and return it.
	size_t iElementVectorSize = elementPathVector.size(); CYCLOPSVAR(iElementVectorSize, "%d");
	if (iElementVectorSize == 2) {
		return this->getRootTextContentVectorW(vectorOfContent);
	}
		
	// If we are here, then the user has asked for some child of the root node.  So pass the root node
	// and vector elements 2 to (iElementVectorSize - 1) to the static getTextContentVector().
	vector<string> subVectorOfElements(&elementPathVector[2], &elementPathVector[iElementVectorSize]); 
	this->assertDocumentNotNull(__LINE__);
	xercesc::DOMElement* pRootElement = m_pDocument->getDocumentElement(); CYCLOPSVAR(pRootElement, "%p");
	return cyclOps::XMLigator::getTextContentVectorW(pRootElement, subVectorOfElements, vectorOfContent);
}

void cyclOps::XMLigator::getTextContentVectorW(const DOMNode* pNode, const vector<string>& elementVector, 
	vector<wstring>& vectorOfContent) {
	// Get the node list corresponding to first element.
	vector<DOMNode*> nodeVector;
	cyclOps::XMLigator::getChildNodeVector(pNode, elementVector[0], nodeVector);	CYCLOPSDEBUG("[nodeVector = %p]", &nodeVector);
	size_t iNodeVectorSize = nodeVector.size();	CYCLOPSVAR(iNodeVectorSize, "%d");
	if (iNodeVectorSize < 1) { CYCLOPSDEBUG("No children.");
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchNode, "The node <%S> has no children named <%s>.",
			pNode->getNodeName(), elementVector[0].c_str());
	}

	// Are there more elements in the vector?  
	size_t iElementVectorSize = elementVector.size(); CYCLOPSVAR(iElementVectorSize, "%d");
	if (iElementVectorSize > 1) {
		// If so call recursively passing first node from node vector, and all elements after the first.
		vector<string> subVector(&elementVector[1], &elementVector[iElementVectorSize]); CYCLOPSDEBUG("subVector.size() = %d", subVector.size());
		cyclOps::XMLigator::getTextContentVectorW(nodeVector[0], subVector, vectorOfContent);
	} else {
		// If not return the text content of each node on the node list.
		for (size_t i = 0; i < iNodeVectorSize; ++i) {
			std::wstring wstrTextContent(nodeVector[i]->getTextContent());
			boost::trim(wstrTextContent);
			vectorOfContent.push_back(wstrTextContent);
		}
	}
}

void cyclOps::XMLigator::getChildNodeVector(const DOMNode* pParentNode, const string& strElement, vector<DOMNode*>& vectorOfNodes, bool boStopAtFirst) { CYCLOPSDEBUG("pParentNode/strElement = %S/%s", pParentNode->getNodeName(), strElement.c_str());
	DOMNodeList* pNodeList = pParentNode->getChildNodes(); CYCLOPSDEBUG("pNodeList->getLenght() = %d", pNodeList->getLength());
	if (pNodeList == NULL || pNodeList->getLength() == 0) {
		string strWhat = StringEmUp::format("The node %S has no children.", pParentNode->getNodeName()); //CYCLOPSDEBUG("strWhat = %s", strWhat.c_str());
		CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionNoSuchNode, strWhat);
	}
	bool boFoundAMatch = false;
	//vector<DOMNode*> vectorOfNodes;
	for (size_t i = 0; i < pNodeList->getLength(); ++i) { //CYCLOPSVAR(i, "%d");
		DOMNode* pChildNode = pNodeList->item(i);
		if (pChildNode->getNodeType() == DOMNode::ELEMENT_NODE) { //CYCLOPSDEBUG("pChildNode->getNodeName() = %S", pChildNode->getNodeName());
			std::wstring wstrNodeName; wstrNodeName.assign(pChildNode->getNodeName()); //CYCLOPSDEBUG("wstrNodeName = %S", wstrNodeName.c_str());
			string strNodeName(wstrNodeName.begin(), wstrNodeName.end()); //CYCLOPSDEBUG("strNodeName = %s", strNodeName.c_str());
			if (strElement.compare(strNodeName) == 0) {
				if ( ! (boStopAtFirst && boFoundAMatch)) { //CYCLOPSDEBUG("size = %d", vectorOfNodes.size()); CYCLOPSDEBUG("pChildNode->getNodeName() = %S", pChildNode->getNodeName());
					/* Here's the problem:  The thing pointed at stays valid after this function, but the pointer itself is not. (?) */
					vectorOfNodes.push_back(pChildNode); //CYCLOPSDEBUG("Ok?");
				}
			} //CYCLOPSDEBUG("Goodbye?");
		}
	}
}

void cyclOps::XMLigator::getNodeVector(const string&  strPathToElement, vector<DOMNode*>& vectorOfNodes) const { CYCLOPSDEBUG("size = %d", vectorOfNodes.size());
	vector<string> elementPathVector = this->getElementPathVectorAbsolute(strPathToElement);
	size_t iElementVectorSize = elementPathVector.size(); CYCLOPSVAR(iElementVectorSize, "%d");
	this->assertDocumentNotNull(__LINE__);
	xercesc::DOMElement* pRootElement = m_pDocument->getDocumentElement(); CYCLOPSVAR(pRootElement, "%p");
	if (iElementVectorSize == 2) {
		// The special case of requesting the node vector of "/root"...
		vectorOfNodes.push_back(pRootElement);
	} else {
		// If we are here, then the user has asked for some descendant of the root node.  So pass the root node
		// and vector elements 2 to (iElementVectorSize - 1) to the static getNodeVector().
		vector<string> subVectorOfElementNames(&elementPathVector[2], &elementPathVector[iElementVectorSize]); CYCLOPSDEBUG("subVector.size() = %d", subVectorOfElementNames.size());
		cyclOps::XMLigator::getNodeVector(pRootElement, subVectorOfElementNames, vectorOfNodes);
	}
}




vector<string> cyclOps::XMLigator::getElementPathVectorAbsolute(const string& strElementPath) const
{
	// This method only accepts an absolute path from the root which must start with a backslash.
	// So first check that this is the case.
	if (strElementPath.compare(0, 1, "/") != 0) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Element path does not begin with a slash.");
	}

	// Now split the string into its elements.  
	// The first element om the vector will be a blank string, because the path started with the delimiter '/'.
	vector<string> elementVector;
	boost::split(elementVector, strElementPath, boost::is_any_of("/")); 

	// There should be at least two elements, the blank one at the beginning and the root node.
	size_t iElementVectorSize = elementVector.size(); CYCLOPSVAR(iElementVectorSize, "%d");
	if (iElementVectorSize < 2) { CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Improper format for element path."); } CYCLOPSVAR(m_pDocument, "%p");

	// The second element of the vector must match the root element.
	this->assertDocumentNotNull(__LINE__);
	xercesc::DOMElement* pRootElement = m_pDocument->getDocumentElement(); CYCLOPSVAR(pRootElement, "%p");
	if (pRootElement == NULL) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "NULL root element in document while searching for '%s'.", strElementPath.c_str());
	} 
	const XMLCh* xmlchRootNodeName = pRootElement->getNodeName(); CYCLOPSVAR(xmlchRootNodeName, "%S"); 
	std::wstring wstrRootNodeName(xmlchRootNodeName);
	string strRootNodeName(wstrRootNodeName.begin(), wstrRootNodeName.end());
	if (elementVector[1].compare(strRootNodeName) != 0) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Root element in path '%s' does not match document root <%S>.", 
			strElementPath.c_str(), pRootElement->getNodeName());
	}
	return elementVector;
}

vector<string> cyclOps::XMLigator::getElementPathVectorRelative(const string&  strElementPath) { CYCLOPSDEBUG("Hello.");
	// This method only accepts a relative path which cannot start with a backslash.
	// So first check that this is the case.
	if (strElementPath.compare(0, 1, "/") == 0) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Element path begins with a backslash.");
	}

	// Now split the string into its elements.  
	vector<string> elementVector;
	boost::split(elementVector, strElementPath, boost::is_any_of("/")); 

	return elementVector;
}


void cyclOps::XMLigator::getRootTextContentVectorW(vector<wstring>& vectorOfContent) const
{
		vector<string> textContentVector;
		this->assertDocumentNotNull(__LINE__);
		xercesc::DOMElement* pRootElement = m_pDocument->getDocumentElement(); CYCLOPSVAR(pRootElement, "%p");
		std::wstring wstrTextContent(pRootElement->getTextContent());
		boost::trim(wstrTextContent);
		vectorOfContent.push_back(wstrTextContent);
}

void cyclOps::XMLigator::getNodeVector(const DOMNode* pDOMNode, const string& strElementPathRelative, vector<DOMNode*>& vectorOfNodes) {
	vector<string> elementVector = cyclOps::XMLigator::getElementPathVectorRelative(strElementPathRelative);
	cyclOps::XMLigator::getNodeVector(pDOMNode, elementVector, vectorOfNodes);
}

/* Only works for direct children!!! Sorry. */
DOMNode* cyclOps::XMLigator::getDirectChildNode(const DOMNode* pParentNode, const string& element) {
	vector<DOMNode*> vectorOfAllMatchingNodes;
	cyclOps::XMLigator::getChildNodeVector(pParentNode, element, vectorOfAllMatchingNodes, true);
	if (vectorOfAllMatchingNodes.size() == 0) {
		string strWhat = StringEmUp::format("The node '%S' has no children named '%s'.", pParentNode->getNodeName(), element);
		CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionNoSuchNode, strWhat);
	} else {
		return vectorOfAllMatchingNodes[0];
	}
}

void cyclOps::XMLigator::getNodeVector(const DOMNode* pDOMNode, vector<string> elementNameVector, vector<DOMNode*>& vectorOfNodes) { CYCLOPSDEBUG("size = %d", vectorOfNodes.size());
	size_t iElementNameVectorSize = elementNameVector.size(); CYCLOPSVAR(iElementNameVectorSize, "%d");
	if (iElementNameVectorSize == 1) {
		// If there's only one element in the element name vector, then we want all children of pDOMNode that have that name.
		//vectorOfNodes = cyclOps::XMLigator::getChildNodeVector(pDOMNode, elementNameVector[0]); CYCLOPSDEBUG("childNodeVector.size() = %d", vectorOfNodes.size());
		cyclOps::XMLigator::getChildNodeVector(pDOMNode, elementNameVector[0], vectorOfNodes); CYCLOPSDEBUG("childNodeVector.size() = %d", vectorOfNodes.size());
		if (vectorOfNodes.size() == 0) {
			string strWhat = StringEmUp::format("The node '%S' has no children named '%s'.", pDOMNode->getNodeName(), elementNameVector[0]);
			CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionNoSuchNode, strWhat);
		}
	} else {
		// If there's more than one element, get the node corresponding to first element, the remaining elements, and call recursive.
		vector<string> subVector(&elementNameVector[1], &elementNameVector[iElementNameVectorSize]); CYCLOPSDEBUG("subVector.size() = %d", subVector.size());
		DOMNode* pChildNode = cyclOps::XMLigator::getDirectChildNode(pDOMNode, elementNameVector[0]);
		cyclOps::XMLigator::getNodeVector(pChildNode, subVector, vectorOfNodes);
	}
}

DOMNode* cyclOps::XMLigator::getFirstNodeWhereChildElementContains(const string&  strNodePath, const string&  strChildElement, 
	const wstring&  wstrTextContent) const {
	vector<DOMNode*> nodeVector;
	this->getNodeVector(strNodePath, nodeVector);  CYCLOPSDEBUG("nodeVector.size() = %d", nodeVector.size());
	for (size_t i = 0; i < nodeVector.size(); ++i) {
		if (XMLigator::doesNodeHaveChildWithContent(nodeVector[i], strChildElement, wstrTextContent)) { 
			return nodeVector[i];
		}
	}
	CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchNode, 
		"There is no node %s that has a child named %s with text content %S.", 
		strNodePath.c_str(), strChildElement.c_str(), wstrTextContent.c_str());
}


DOMNode* cyclOps::XMLigator::getFirstNodeWhereChildElementContains(const string&  strNodePath, const string&  strChildElement, 
	const string&  strTextContent) const {
	wstring wstrTextContent(strTextContent.begin(), strTextContent.end());
	return getFirstNodeWhereChildElementContains(strNodePath, strChildElement, wstrTextContent);
}

bool cyclOps::XMLigator::doesNodeHaveChildWithContent(const DOMNode* pNode, const string&  strChildElement, 
	const string&  strTextContent) { 
	wstring wstrTextContent(strTextContent.begin(), strTextContent.end());
	return doesNodeHaveChildWithContent(pNode, strChildElement, wstrTextContent);
}


bool cyclOps::XMLigator::doesNodeHaveChildWithContent(const DOMNode* pNode, const string&  strChildElement, 
	const wstring&  wstrTextContent) { CYCLOPSDEBUG("wstrTextContent = '%S'", wstrTextContent.c_str());
	vector<DOMNode*> childNodeVector;
	XMLigator::getChildNodeVector(pNode, strChildElement, childNodeVector); CYCLOPSDEBUG("childNodeVector.size() = %d", childNodeVector.size());
	for (size_t i = 0; i < childNodeVector.size(); ++i) {
		const XMLCh* xmlchTextContent = childNodeVector[i]->getTextContent(); CYCLOPSDEBUG("xmlchTextContent = %S", xmlchTextContent);
		if (boost::iequals(wstrTextContent, xmlchTextContent)) { CYCLOPSDEBUG("Node found.");
			return true;
		}
	}
	return false;
}

string cyclOps::XMLigator::getTextContent(const DOMNode* pNode, const string&  strElement) { 
	wstring wstrContent = getTextContentW(pNode, strElement);
	string strContent(wstrContent.begin(), wstrContent.end());
	return strContent;
}

wstring cyclOps::XMLigator::getTextContentW(const DOMNode* pNode, const string& strElement) {
	vector<wstring> vectorOfTextContent;
	cyclOps::XMLigator::getTextContentVectorW(pNode, strElement, vectorOfTextContent);
	if (vectorOfTextContent.size() == 0) {
		char szMessage[1000];
		CYCLOPS_SNPRINTF_S(szMessage, "Node '%S' has no such child node '%s'.", pNode->getNodeName(), strElement.c_str());
		throw cyclOps::ExceptionNoSuchNode(szMessage, __FILE__, __FUNCTION__, __LINE__);
	}
	return vectorOfTextContent[0];
}

int cyclOps::XMLigator::getTextContentAs_int(const string& element, int default) {
	try {
		return this->getTextContentAs_int(element);
	} catch (const cyclOps::ExceptionNoSuchNode& e) {
		CYCLOPSWARNING("No node %s, using default value %d (%s)", element.c_str(), default, e.what());
		return default;
	} catch (const std::invalid_argument& e) {
		CYCLOPSWARNING("Value %s is not an integer, using default value %d (%s)", element.c_str(), default, e.what());
		return default;
	}
}

int cyclOps::XMLigator::getTextContentAs_int(const DOMNode* pNode, const string& strElement) {
	string strContent = XMLigator::getTextContent(pNode, strElement);
	return std::stoi(strContent);
}

int cyclOps::XMLigator::getTextContentAs_int(const DOMNode* pNode, const string& strElement, int default) {
	try {
		return XMLigator::getTextContentAs_int(pNode, strElement);
	} catch (const cyclOps::ExceptionNoSuchNode& ignore) {
		return default;
	}
}


string cyclOps::XMLigator::documentToString(xercesc::DOMDocument* pDocument) {
	string strOutput;
	XMLigator::documentToString(pDocument, strOutput);
	return strOutput;
}

void cyclOps::XMLigator::documentToString(xercesc::DOMDocument* pDocument, string& strOutput) {
	/* 
		CAREFUL: You will encounter alternatives that use DOMWriter.  This class
		is no longer available.  It is now DOMLSSerializer.
	*/
	DOMImplementation *pDOMImplementation = DOMImplementationRegistry::getDOMImplementation(L"LS");
	DOMLSSerializer *pDOMSerializer = ((DOMImplementationLS*)pDOMImplementation)->createLSSerializer();
	DOMLSOutput *pDOMLSOutput = ((DOMImplementationLS*)pDOMImplementation)->createLSOutput();
	MemBufFormatTarget* pFormatTarget = new MemBufFormatTarget();
	pDOMLSOutput->setByteStream(pFormatTarget);
	pDOMSerializer->write(pDocument, pDOMLSOutput);
	const XMLByte* xmlByte = pFormatTarget->getRawBuffer(); 
	XMLSize_t iSize = pFormatTarget->getLen();
	char* pchResultXML = new char[iSize + 100];
	strcpy_s(pchResultXML, iSize + 50, (const char*) xmlByte); CYCLOPSDEBUG("pchResultXML = %s", pchResultXML);
	strOutput.assign(pchResultXML);
	delete pchResultXML;
	pDOMLSOutput->release();
	pDOMSerializer->release();
	/* TODO:  You have a memory leak here.  But when you delete pFormatTarget you are getting access violations. */
	// delete pFormatTarget;
}

DOMNode* XMLigator::getNode(const DOMNode* pNode, const std::string& path) { 
	vector<DOMNode*> vectorOfNodes;
	XMLigator::getNodeVector(pNode, path, vectorOfNodes);
	if (vectorOfNodes.size() == 0) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchNode, "The node '%s' does not exist under '%S'.", path.c_str(), pNode->getNodeName());
	} else {
		return vectorOfNodes[0];
	}

}

DOMNode* cyclOps::XMLigator::getNode(const string& path) {
	vector<DOMNode*> vectorOfNodes;
	this->getNodeVector(path, vectorOfNodes);
	if (vectorOfNodes.size() == 0) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchNode, "The node '%s' does not exist in '%s'.", path.c_str(), this->m_strFile.c_str());
	} else {
		return vectorOfNodes[0];
	}
}


void cyclOps::XMLigator::getTextContentVectorW(const DOMNode* pNode, const string&  strElementPath, vector<wstring>& vectorOfContent) { CYCLOPSDEBUG("Hello.");
	vector<string> elementVector = cyclOps::XMLigator::getElementPathVectorRelative(strElementPath);
	cyclOps::XMLigator::getTextContentVectorW(pNode, elementVector, vectorOfContent);
}

void cyclOps::XMLigator::assertDocumentNotNull(int iLine) const {
	if (this->m_pDocument == NULL) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Document pointer is NULL at line %d.", iLine);
	}
}

xercesc::DOMDocument* cyclOps::XMLigator::createDocument(wchar_t* wszRootNodeName) { CYCLOPSDEBUG("Hello.");
	try {
		xercesc::XMLPlatformUtils::Initialize(); CYCLOPSDEBUG("Initialize() done.");
		DOMImplementation* pDOMImplementation = DOMImplementationRegistry::getDOMImplementation(L"LS"); CYCLOPSDEBUG("pDOMImplementation = %p", pDOMImplementation );
		xercesc::DOMDocument* pDocument = pDOMImplementation->createDocument(0, wszRootNodeName, 0); CYCLOPSDEBUG("pDocument = %p", pDocument);
		return pDocument;
	} catch (const xercesc::XMLException& e) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "%s - %s", typeid(e).name(), e.getMessage());
	} catch (const xercesc::SAXException& e) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "%s - %s", typeid(e).name(), e.getMessage());
	} catch (const xercesc::DOMException& e) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "%s - %s", typeid(e).name(), e.getMessage());
	} catch (...) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Unknown exception.");
	}
}

void XMLigator::setTextContent(DOMNode* pNode, const string& strElement, const string& strTextContent) {
	try {
		vector<DOMNode*> nodes;
		XMLigator::getNodeVector(pNode, strElement, nodes);
		for (vector<DOMNode*>::iterator child = nodes.begin(); child != nodes.end(); ++child) {
			wstring wstrTextContent(strTextContent.begin(), strTextContent.end());
			(*child)->setTextContent(wstrTextContent.c_str());
		}
	} catch (cyclOps::ExceptionNoSuchNode e) {
		wstring wstrElement(strElement.begin(), strElement.end());
		DOMElement* pElement = pNode->getOwnerDocument()->createElement(wstrElement.c_str());
		wstring wstrTextContent(strTextContent.begin(), strTextContent.end());
		pElement->setTextContent(wstrTextContent.c_str());
		pNode->appendChild(pElement);
	}
}

void XMLigator::serialize() {
	if (this->m_strFile.length() == 0) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "This XML document was not initialized from a file.");
	}
	
	std::ofstream file;
	file.open(this->m_strFile, std::ios::trunc);
	if (file.fail()) { 
		char error[5000];
		strerror_s(error, CYCLOPSSIZEOF(error), errno);
		std::stringstream msg; msg << error;
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionXMLigator, "Unable to open %s.  [%s]", m_strFile.c_str(), 
			msg.str().c_str());
	}
	string strContent = this->documentToString(this->m_pDocument);
	file << strContent << "\n";
	file.close();
}

string XMLigator::escapeXML(const string& strOriginal) {
	string strNew = strOriginal;
	StringEmUp::replaceAll(strNew, "&", "&amp;");
	StringEmUp::replaceAll(strNew, "<", "&lt;");
	return strNew;
}

string XMLigator::getTextContentEncryptedWonky(const std::string& element) {
	string encrypted = this->getTextContent(element);
	return cyclOps::Encryptigator::decryptWonky(encrypted);
}

/* An empty string will get the direct childer as a map.*/
void XMLigator::getChildTextContentMap(const DOMNode* pNode, const std::string& element, std::map<std::string, std::string>& map) {
	const DOMNode* pParentNode = element.empty() ? pNode : XMLigator::getNode(pNode, element);
	XMLigator::getTextContentOfDirectChildNodesAsMap(pParentNode, map);
}

void XMLigator::getChildTextContentMap(const std::string& element, std::map<std::string, std::string>& map) {
	DOMNode* pParentNode = this->getNode(element);
	XMLigator::getTextContentOfDirectChildNodesAsMap(pParentNode, map);
}

void XMLigator::getTextContentOfDirectChildNodesAsMap(const DOMNode* pParentNode, map<string, string>& map) {
	DOMNodeList* pNodeList = pParentNode->getChildNodes();
	for (int i = 0; i < pNodeList->getLength(); ++i) { CYCLOPSVAR(i, "%d");
		DOMNode* pNode = pNodeList->item(i);
		if (pNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			wstring wstrNodeName = pNode->getNodeName(); CYCLOPSDEBUG("wstrNodeName = %S", wstrNodeName.c_str());
			wstring wstrTextContent = pNode->getTextContent();
			string strNodeName(wstrNodeName.begin(), wstrNodeName.end());
			string strTextContent(wstrTextContent.begin(), wstrTextContent.end());
			cyclOps::StringEmUp::trim(strTextContent);
			map[strNodeName] = strTextContent;
		}
	}
}

unsigned char XMLigator::getTextContentAsBoolean(const std::string& element) {
	try {
		string text = this->getTextContent(element);
		return StringEmUp::compareIgnoreCase(text, "true") == 0;
	} catch (const cyclOps::ExceptionNoSuchNode& e) { CYCLOPSDEBUG("%s", e.getMessage());
		return false;
	}
}

bool XMLigator::getTextContentAsBoolean(const xercesc::DOMNode* pNode, const std::string& element) {
	try {
		string text = XMLigator::getTextContent(pNode, element);
		return StringEmUp::compareIgnoreCase(text, "true") == 0;
	} catch (const cyclOps::ExceptionNoSuchNode ignore) {
		return false;
	}
}

void XMLigator::getVectorOfMaps(const string& element, vector<map<string, string>>& vectorOfMapsOfReportConfigs) {
	vector<DOMNode*> vectorOfNodes;
	this->getNodeVector(element, vectorOfNodes);
	for (int i = 0; i < vectorOfNodes.size(); ++i) {
		DOMNode* pNode = vectorOfNodes[i];
		map<string, string> mapOfChilds;
		XMLigator::getTextContentOfDirectChildNodesAsMap(pNode, mapOfChilds);
		vectorOfMapsOfReportConfigs.push_back(mapOfChilds);
	}
}

string XMLigator::getAttribute(const DOMNode* pNode, const std::string& attributeName) {
	DOMElement* pElement = (DOMElement*) pNode;
	wstring wstrName(attributeName.begin(), attributeName.end());
	const XMLCh* wszValue = pElement->getAttribute(wstrName.c_str());
	wstring wstrValue = wszValue;
	return string(wstrValue.begin(), wstrValue.end());
}
}
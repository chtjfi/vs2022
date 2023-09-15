#pragma once

#include <string>
#include <vector>
#include <map>

#include <winsock2.h>
#include <windows.h>

#include <xercesc\parsers\XercesDOMParser.hpp>
#include <xercesc\dom\DOMNodeList.hpp>
#include <xercesc\dom\DOMDocument.hpp>

using xercesc::DOMNode;
using xercesc::DOMNodeList;
/* using xercesc::DOMDocument; */

namespace cyclOps {
	class XMLigator	{
	private:
		/* This is set only if the object is initialized from a file. */
		std::string m_strFile;
		/* This is set only if the object is initialized from a string. */
		std::string _strXML;
		/* The parser is stored on the heap. */
		xercesc::XercesDOMParser* m_pParser;
		xercesc::DOMDocument* m_pDocument;
		void assertDocumentNotNull(int iLine) const;

		/* Prohibit copy constructor and assignment operator. */
		XMLigator(const XMLigator&);
		XMLigator& operator=(const XMLigator&);
		
	public:
		explicit XMLigator();
		~XMLigator(void);
		void initializeFromFilename(const std::string& strFile);
		void initializeFromString(const std::string& strXML);
		std::string			getFilename(void) { return m_strFile;  }
		std::string			getTextContent(const std::string& strElement) const;
		std::wstring		getTextContentW(const std::string& strElement) const;
		static std::string	getTextContent(const DOMNode* pNode, const std::string& strElement);
		static std::wstring getTextContentW(const DOMNode* pNode, const std::string& strElement);
		static int			getTextContentAs_int(const DOMNode* pNode, const std::string& strElement);
		static int			getTextContentAs_int(const DOMNode* pNode, const std::string& strElement, int iDefault);
		int					getTextContentAs_int(const std::string& element);
		int					getTextContentAs_int(const std::string& element, int iDefault);
		void				getNodeVector(const std::string& strElement, std::vector<DOMNode*>& vectorOfNodes) const;
		void				getChildTextContentMap(const std::string& element, std::map<std::string, std::string>& map);
		static void			getChildTextContentMap(const DOMNode* pNode, const std::string& element, std::map<std::string, std::string>& map);
		static void			getTextContentOfDirectChildNodesAsMap(const DOMNode* pParentNode, std::map<std::string, std::string>& map);
		DOMNode*			getNode(const std::string& path);
		static DOMNode*		getNode(const DOMNode* pNode, const std::string& path);
		static DOMNode*		getDirectChildNode(const DOMNode* pParentNode, const std::string& element);
		void				getRootTextContentVectorW(std::vector<std::wstring>& vectorOfContent) const;
		DOMNode*			getFirstNodeWhereChildElementContains(const std::string& strNodePath, const std::string& strChildElement, const std::string& strTextContent) const;
		DOMNode*			getFirstNodeWhereChildElementContains(const std::string& strNodePath, const std::string& strChildElement, const std::wstring& wstrTextContent) const;
		/* The absolute version references the root node, so it cannot be static. */
		std::vector<std::string>	
							getElementPathVectorAbsolute(const std::string& strElementPath) const;
		/* The relative version does not reference the root node, and is needed in static methods, so it is static. */
		static std::vector<std::string> 
							getElementPathVectorRelative(const std::string& strElementPath);
		static void			getChildNodeVector(const DOMNode* pNode, const std::string& strElement, std::vector<DOMNode*>& vectorOfNodes, bool boStopAtFirst = false);
		static void			getTextContentVectorW(const DOMNode* pNode, const std::vector<std::string>& elementVector, std::vector<std::wstring>& vectorOfContent);
		static void			getTextContentVectorW(const DOMNode* pNode, const std::string&  strElementPath, std::vector<std::wstring>& vectorOfContent);
		static bool 
							getTextContentAsBoolean(const DOMNode* pNode, const std::string& element);
		unsigned char		getTextContentAsBoolean(const std::string& element);
		void				getTextContentVectorW(const std::string& strPath, std::vector<std::wstring>& vectorOfContent) const;
		void				getTextContentVector(const std::string& strPath, std::vector<std::string>& vectorOfContent) const;
		static void			getNodeVector(const DOMNode* pDOMNode, std::vector<std::string> elementNameVector, std::vector<DOMNode*>& vectorOfNodes);
		static void			getNodeVector(const DOMNode* pDOMNode, const std::string& strElementPathRelative, std::vector<DOMNode*>& vectorNodes);
		static bool			doesNodeHaveChildWithContent(const DOMNode* pNode, const std::string& strChildElement, const std::string& strTextContent);
		static bool			doesNodeHaveChildWithContent(const DOMNode* pNode, const std::string& strChildElement, const std::wstring& wstrTextContent);
		static void			setTextContent(DOMNode* pNode, const std::string& strElement, const std::string& strTextContent);
		static std::string	documentToString(xercesc::DOMDocument* pDocument);
		std::string			getTextContentEncryptedWonky(const std::string& element);
		static void			documentToString(xercesc::DOMDocument* pDocument, std::string& strOutput);
		/* Not yet implemented? */
		/* static string documentToStringAlternate(xercesc::DOMDocument* pDocument); */
		static xercesc::DOMDocument* 
							createDocument(const wchar_t* wszRootNodeName);
		void				serialize(void);
		static std::string	escapeXML(const std::string& strOriginal);
		void				getVectorOfMaps(const std::string& element, std::vector<std::map<std::string, std::string>>& vectorOfMapsOfReportConfigs);
		static std::string	getAttribute(const DOMNode* pNode, const std::string& attributeName);
	};
};

#include "StdAfx.h"

#include <regex>

#include "..\cyclOps\cyclOps.h"

#include "RegExtravaganza.h"

using std::string;
using std::wstring;

namespace cyclOps {
	RegExtravaganza::RegExtravaganza(void)
	{
	}


	RegExtravaganza::~RegExtravaganza(void)
	{
	}

	bool RegExtravaganza::matches(const string& strString, const string& strPattern) { 
		std::regex pattern(strPattern, std::regex_constants::icase); CYCLOPSDEBUG("Aardvark.");
		std::smatch sm;    /* Same as std::match_results<string::const_iterator> sm; */ CYCLOPSDEBUG("Baboon.");
		std::regex_match(strString.cbegin(), strString.cend(), sm, pattern); CYCLOPSDEBUG("sm = %p", &sm);
		bool boReturn = sm.size() > 0; CYCLOPSDEBUG("boReturn = %d", boReturn);
		return boReturn;
	}
	
	bool RegExtravaganza::grep(const string& strString, const string& strPatternInner) { 
		string strPattern = ".*" + strPatternInner + ".*";
		std::regex pattern(strPattern, std::regex_constants::icase); CYCLOPSDEBUG("Aardvark.");
		std::smatch sm;    /* Same as std::match_results<string::const_iterator> sm; */ CYCLOPSDEBUG("Baboon.");
		std::regex_match(strString.cbegin(), strString.cend(), sm, pattern); CYCLOPSDEBUG("sm = %p", &sm);
		return sm.size() > 0;
	}

	bool RegExtravaganza::grep(const wstring& wstrString, const wstring& wstrPatternInner) { 
		wstring wstrPattern = L".*" + wstrPatternInner + L".*";
		std::wregex pattern(wstrPattern, std::regex_constants::icase); CYCLOPSDEBUG("Aardvark.");
		std::wsmatch sm;    /* Same as std::match_results<string::const_iterator> sm; */ CYCLOPSDEBUG("Baboon.");
		std::regex_match(wstrString.cbegin(), wstrString.cend(), sm, pattern); CYCLOPSDEBUG("sm = %p", &sm);
		return sm.size() > 0;
	}

	void RegExtravaganza::replace(const string& text, const string& pattern, const string& replaceWith, 
		string& result, RegExtravaganza::IgnoreCase ignoreCase) 
	{
		if (ignoreCase == True) {
			std::regex reg(pattern, std::regex_constants::icase);
			result.assign(std::regex_replace(text, reg, replaceWith));
		} else {
			std::regex reg(pattern);
			result.assign(std::regex_replace(text, reg, replaceWith));
		}
	}

}
#pragma once

#include <string>

namespace cyclOps {
	class RegExtravaganza
	{
	public:
		RegExtravaganza(void);
		~RegExtravaganza(void);
		enum IgnoreCase { True, False };
		static bool grep(const std::string& strString, const std::string& strPattern);
		static bool grep(const std::wstring& wstrString, const std::wstring& wstrPattern);
		static bool matches(const std::string& strString, const std::string& strPattern);
		static void replace(const std::string& text, const std::string& pattern, const std::string& replaceWith, 
			std::string& result, IgnoreCase ignoreCase);
	};
}
#pragma once

#include <string>
#include <vector>
#include <map>

namespace cyclOps {
	class StringEmUp
	{
	public:
		StringEmUp(void);
		~StringEmUp(void);
		static char							getRandomCharacter(void);
		static void							tokenize(const std::string& string, const std::string& patten, std::vector<std::string>& tokens);
		static std::string					capitalizeFirstLetter(const std::string& s);
		static void							trim(std::string& s);
		static void							ltrim(std::string& s);
		static void							rtrim(std::string& s);
		static std::string					getGUID();
		static void							removeAllNonAsciiCharacters(const std::wstring& original, std::string& neww);
		static bool							isEmpty(const std::string& s) { return s.empty(); }
		static bool							isEmpty(const std::wstring& s) { return s.empty(); }
		static std::string					toLower(const std::string&);
		static std::string					toUpper(const std::string& strOriginal);
		static void							replaceAll(std::string& strTarget, const std::string& strSearch, const std::string& strReplace);
		static void							replaceAll(std::wstring& wstrTarget, const std::wstring& wstrSearch, const std::wstring& wstrReplace);
		static void							split(const std::string &s, char delim, std::vector<std::string> &elems);
		static std::vector<std::string>		split(const std::string &s, char delim);
		static std::string					format(const char* szFormat, ...);
		static bool							startsWithUsingFind(const std::string& strString, const std::string& strStartsWith);
		static bool							startsWithUsingCompare(const std::string& strString, const std::string& strStartsWith);
		static int							compareIgnoreCase(const std::string& str1, const std::string& str2);
		static bool							isWhitespaceOnly(const std::string& str);
		static bool							containsIgnoreCase(const std::string& searched, const std::string& sought);
		static bool							contains(const std::string& searched, const std::string& sought);
		static bool							containsAnyCharacterInString(const std::string& searched, const std::string& sought);
		static int							toInt(const std::string& s);
		static int							toInt(const std::wstring& s);
		static unsigned long				toUnsignedLong(const std::wstring& s);
		static long long					toLongLong(const std::wstring& s);
		static void							removeAll(std::string& s, char c);
		static wchar_t*						new_wcharArray(const std::string& str);
		static wchar_t*						new_wcharArray(const std::wstring& wstr);
		static char*						new_charArray(const std::string& str);
		static std::string					to_string(const unsigned long ul) { return std::to_string(ul); }
		static void							toString(const std::map<std::string, std::string>& mapp, std::string& stringg);
		static void							toString(const std::map<std::string, std::vector<std::string>>& mapOfStringsToVectors, std::string& stringg);
		static bool							endsWithUsingEqualsAndReverseIterators(const std::string& searched, const std::string& sought);
		static bool							endsWithUsingEqualsAndReverseIteratorsW(const std::wstring& searched, const std::wstring& sought);
		static bool							endsWithUsingCompare(const std::string& searched, const std::string& sought);
		static std::string					substringAfterLastOccurrenceOfCharacter(const std::string& s, char c);
		static std::string					substringAfterFirstOccurrenceOfCharacter(const std::string& s, char c);
		static bool							equalsIgnoreCase(const std::string& s1, const std::string& s2);
		static bool							in(const std::string& s1, std::vector<std::string> strings);
		static std::string					reverse(const std::string& original);
	};
}
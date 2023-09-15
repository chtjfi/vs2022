#include "StdAfx.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <stdarg.h>
#include <functional> 
#include <cctype>
#include <locale>


#include <boost/algorithm/string/replace.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/regex.hpp>

#include "cyclOps.h"
#include "StringEmUp.h"

using std::string;
using std::wstring;
using std::vector;
using std::map;
using std::stringstream;
using std::cout;

namespace cyclOps {

	StringEmUp::StringEmUp(void)
	{
	}


	StringEmUp::~StringEmUp(void)
	{
	}

	string StringEmUp::capitalizeFirstLetter(const string& word) {
		/* My own code :/  - No internet connect :( */
		string firstLetter = word.substr(0, 1); CYCLOPSDEBUG("firstLetter = %s", firstLetter.c_str());
		string firstLetterCapitalized = StringEmUp::toUpper(firstLetter);
		string remainder = word.substr(1, word.size() - 1); CYCLOPSDEBUG("remainder = %s", remainder.c_str());
		return firstLetterCapitalized + remainder;
	}

	void StringEmUp::ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
	}

	void StringEmUp::rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	}

	void StringEmUp::trim(std::string &s) {
		/* http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring */
		StringEmUp::ltrim(s);
		StringEmUp::rtrim(s);
	}

	int StringEmUp::compareIgnoreCase(const std::string& str1, const std::string& str2) {
		/* Why the hell do I toLower() these if I am calling _strIcmp() ??? */
		string str1Lower = toLower(str1);
		string str2Lower = toLower(str2);
		/* Even Scott Meyers uses _stricmp() on p154. */
		return _stricmp(str1Lower.c_str(), str2Lower.c_str());
	}

	string StringEmUp::toLower(const string& strOriginal) {
		/* http://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case */
		/* http://stackoverflow.com/questions/19200528/is-it-safe-for-inputiterator-and-outputiterator-in-stdtransform-to-be-from-the */
		string strNew = strOriginal;
		std::transform(strNew.begin(), strNew.end(), strNew.begin(), ::tolower);
		return strNew;
	}
	string StringEmUp::toUpper(const string& strOriginal) {
		/* http://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case */
		/* http://stackoverflow.com/questions/19200528/is-it-safe-for-inputiterator-and-outputiterator-in-stdtransform-to-be-from-the */
		string strNew = strOriginal;
		std::transform(strNew.begin(), strNew.end(), strNew.begin(), ::toupper);
		return strNew;
	}

	void StringEmUp::replaceAll(string& strTarget, const string& strSearch, const string& strReplace) {
		boost::replace_all(strTarget, strSearch, strReplace);
	}

	void StringEmUp::replaceAll(wstring& wstrTarget, const wstring& wstrSearch, const wstring& wstrReplace) {
		boost::replace_all(wstrTarget, wstrSearch, wstrReplace);
	}

	void StringEmUp::tokenize(const string& stringg, const string& pattern, vector<string>& tokens) {
		boost::regex regexPattern(pattern);
		boost::sregex_token_iterator i(stringg.begin(), stringg.end(), regexPattern, -1);
		boost::sregex_token_iterator j;
		while (i != j) {
			string out = *i++;
			tokens.push_back(out);
		}
	}

	void StringEmUp::removeAllNonAsciiCharacters(const std::wstring& original, std::string& neww) {
		throw std::runtime_error("This method removeAllNonAsciiCharacters() has not been tested yet.");
		for (int i = 0; i < original.size(); ++i) {
			wchar_t c = original[i];
			if (c > 0 && c < 256) {
				char c2 = c;
				neww += c2;
			}
		}
	}

	/* http://stackoverflow.com/questions/236129/splitting-a-string-in-c */
	void StringEmUp::split(const string &s, char delim, vector<string> &elems) {
		size_t found = s.find(delim);
		if (found == string::npos) {
			elems.push_back(s);
		} else {
			stringstream ss(s);
			string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			} 
		}
	}

	/* http://stackoverflow.com/questions/236129/splitting-a-string-in-c */
	vector<std::string> StringEmUp::split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		StringEmUp::split(s, delim, elems);
		return elems;
	}

	string StringEmUp::format(const char* szFormat, ...) {
		char szMessage[5000];
		va_list vaArguments;
		va_start(vaArguments, szFormat);
		_vsnprintf_s(szMessage, sizeof(szMessage) / sizeof(szMessage[0]), _TRUNCATE, szFormat, vaArguments); CYCLOPSVAR(szMessage, "%s");
		return string(szMessage);
	}

	bool startsWithUsingFind(const std::string& strString, const std::string& strStartsWith) {
		/* http://stackoverflow.com/questions/7755719/check-if-string-starts-with-another-string-find-or-compare */
		/* The disadvantage of find is that if str1 is long, then it will pointlessly search all the way through it for str2. */
		if ( strString.find(strStartsWith) == 0 ) {
			return true;
		} else {
			return false;
		}
	}

	bool startsWithUsingCompare(const std::string& strString, const std::string& strStartsWith) {
		/* http://stackoverflow.com/questions/7755719/check-if-string-starts-with-another-string-find-or-compare */
		try {
			if ( strString.compare(0, strStartsWith.length(), strStartsWith) == 0 ) {
				return true;
			} else {
				return false;
			}
		} catch (const std::out_of_range& e) { CYCLOPSDEBUG("%s - %s", typeid(e).name(), e.what());
			/* This is thrown when the start position is past the end of the string. */
			return false;
		}
	}
	
	bool StringEmUp::isWhitespaceOnly(const std::string& str) {
		return str.find_first_not_of("\t\n ") == std::string::npos;
	}

	bool StringEmUp::containsIgnoreCase(const std::string& searched, const std::string& sought) {
		/* http://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find */
		/* http://stackoverflow.com/questions/7099138/case-insensitive-find-method-in-string-class */
		string str1Lower = searched;
		std::transform(str1Lower.begin(), str1Lower.end(), str1Lower.begin(), ::tolower);
		string str2Lower = sought;
		std::transform(str2Lower.begin(), str2Lower.end(), str2Lower.begin(), ::tolower);
		std::size_t found = str1Lower.find(str2Lower);
		return found == std::string::npos ? false : true;
	}

	bool StringEmUp::contains(const std::string& searched, const std::string& sought) {
		std::size_t found = searched.find(sought);
		return found == std::string::npos ? false : true;
	}


	bool StringEmUp::containsAnyCharacterInString(const std::string& searched, const std::string& sought) {
		string searchedLower = searched;
		std::transform(searchedLower.begin(), searchedLower.end(), searchedLower.begin(), ::tolower);
		string soughtLower = sought;
		std::transform(soughtLower.begin(), soughtLower.end(), soughtLower.begin(), ::tolower);
		return std::string::npos != searchedLower.find_first_of(soughtLower);
	}

	int StringEmUp::toInt(const string& s) {
		return std::stoi(s);
	}

	int StringEmUp::toInt(const wstring& s) {
		return std::stoi(s);
	}
	unsigned long StringEmUp::toUnsignedLong(const wstring& s) {
		return std::stoul(s);
	}

	long long StringEmUp::toLongLong(const wstring& s) {
		return std::stoll(s);
	}
	
	void StringEmUp::removeAll(string& s, char c) {
		s.erase(std::remove(s.begin(), s.end(), c), s.end());
	}

	wchar_t* StringEmUp::new_wcharArray(const std::string& str) {
		size_t size = str.size() + 1;
		wchar_t* pwchNew = new wchar_t[size];
		::swprintf_s(pwchNew, size, L"%S", str.c_str());
		return pwchNew;
	}

	wchar_t* StringEmUp::new_wcharArray(const std::wstring& wstr) {
		size_t size = wstr.size() + 1;
		wchar_t* pwchNew = new wchar_t[size];
		::wcscpy_s(pwchNew, size, wstr.c_str());
		return pwchNew;
	}

	char* StringEmUp::new_charArray(const std::string& str) {
		size_t size = str.size() + 1; CYCLOPSVAR(size, "%d");
		char* pchNew = new char[size];
		::sprintf_s(pchNew, size, "%s", str.c_str()); CYCLOPSVAR(pchNew, "%s");
		return pchNew;
	}

	bool StringEmUp::endsWithUsingEqualsAndReverseIterators(const string& value, const string& ending) {
		/* I always hate calculating indices of substrings, it's very off-by-one prone... 
			I'ld rather iterate backwards from the end of both strings, trying to find a mismatch.–  
			xtoflMay 18 '09 at 8:15 */
		if (ending.size() > value.size()) {
			return false;
		}
		else {
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}
	}

	bool StringEmUp::endsWithUsingEqualsAndReverseIteratorsW(const wstring& value, const wstring& ending) {
		/* I always hate calculating indices of substrings, it's very off-by-one prone...
		I'ld rather iterate backwards from the end of both strings, trying to find a mismatch.–
		xtoflMay 18 '09 at 8:15 */
		if (ending.size() > value.size()) {
			return false;
		}
		else {
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}
	}


	bool StringEmUp::endsWithUsingCompare(const string& fullString, const string& ending) {
	   if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
		} else {
			return false;
		}

	}

	string StringEmUp::getGUID() {
	    boost::uuids::uuid uuid = boost::uuids::random_generator()();
		stringstream ss;
		ss << uuid;
		return ss.str();
	}

	string	StringEmUp::substringAfterLastOccurrenceOfCharacter(const std::string& s, char c) {
		size_t lastOccurrence = s.rfind(c);
		if (lastOccurrence == string::npos) {
			return s;
		}
		else {
			return s.substr(lastOccurrence + 1);
		}
	}
	
	string	StringEmUp::substringAfterFirstOccurrenceOfCharacter(const std::string& s, char c) {
		size_t firstOccurrence = s.find(c);
		if (firstOccurrence == string::npos) {
			return s;
		}
		else {
			return s.substr(firstOccurrence + 1);
		}
	}

	void StringEmUp::toString(const map<string, string>& mapp, string& stringg) { CYCLOPSDEBUG("size = %d", mapp.size());
		stringstream ss;
		ss << "{ ";
		boolean firstTime = true;
		for (map<string, string>::const_iterator i = mapp.begin(); i != mapp.end(); ++i) {
			if (firstTime) {
				firstTime = false;
			} else {
				ss << ", ";
			}
			ss << "\"" << i->first << "\" : \"" << i->second << "\" " ;
		}
		ss << " }";
		stringg.assign(ss.str());
	}

	void StringEmUp::toString(const map<string, vector<string>>& mapOfStringsToVectors, string& stringg) {
		stringstream ss;
		ss << "{ ";
		boolean firstTimeForMap = true; 
		for (map<string, vector<string>>::const_iterator i = mapOfStringsToVectors.begin(); i != mapOfStringsToVectors.end(); ++i) {
			if (firstTimeForMap) { 
				firstTimeForMap = false;
			} else {
				ss << ", ";
			}
			const string& key = i->first;
			ss << " \"" << key << "\" : [ ";
			const vector<string>& vectorOfStrings = i->second;
			boolean firstTimeForVector = true;
			for (vector<string>::const_iterator j = vectorOfStrings.begin(); j != vectorOfStrings.end(); ++j) {
				if (firstTimeForVector) {
					firstTimeForVector = false;
				} else {
					ss << ", ";
				}
				const string& value = *j;
				ss << " \"" << value << "\"";
			}
			ss << " ]";
		}
		ss << " }";
		stringg.assign(ss.str());
	}

	bool StringEmUp::equalsIgnoreCase(const string& s1, const string& s2) { CYCLOPSDEBUG("s1/s2 = '%s'/'%s'", s1.c_str(), s2.c_str());
		return ::_stricmp(s1.c_str(), s2.c_str()) == 0;
	}

	bool StringEmUp::equalsIgnoreCaseW(const wstring& s1, const wstring& s2) {
		CYCLOPSDEBUG("s1/s2 = '%S'/'%S'", s1.c_str(), s2.c_str());
		return ::_wcsicmp(s1.c_str(), s2.c_str()) == 0;
	}

	bool StringEmUp::in(const std::string& s1, const vector<string>& strings) { CYCLOPSDEBUG("s1 = %s", s1.c_str());
		for (int i = 0; i < strings.size(); ++i) { CYCLOPSVAR(i, "%d");
			string compareTo = strings[i]; CYCLOPSDEBUG("compareTo = %s", compareTo.c_str());
			if (cyclOps::StringEmUp::equalsIgnoreCase(s1, compareTo)) { CYCLOPSDEBUG("Returning true.");
				return true;
			}
		} CYCLOPSDEBUG("Returning false.");
		return false;
	}

	bool StringEmUp::inW(const std::wstring& s1, const vector<wstring>& strings) {
		CYCLOPSDEBUG("s1 = %S", s1.c_str());
		for (int i = 0; i < strings.size(); ++i) {
			CYCLOPSVAR(i, "%d");
			wstring compareTo = strings[i]; CYCLOPSDEBUG("compareTo = %S", compareTo.c_str());
			if (cyclOps::StringEmUp::equalsIgnoreCaseW(s1, compareTo)) {
				CYCLOPSDEBUG("Returning true.");
				return true;
			}
		} CYCLOPSDEBUG("Returning false.");
		return false;
	}


	string StringEmUp::reverse(const string& original) {
		string reversed;
		int position = 0;
		for (string::const_reverse_iterator rit = original.rbegin(); rit != original.rend(); ++rit) {
			reversed += *rit;
			++position;
		}
		return reversed;
	}

	char StringEmUp::getRandomCharacter(void) {
		return 'a' + (rand() % 26);
	}
}
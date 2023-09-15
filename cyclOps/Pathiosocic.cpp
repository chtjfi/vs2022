#include "StdAfx.h"

#include <algorithm>
#include <string>
#include <regex>
#include <iostream>


#include "cyclOps.h"

#include "Pathiosocic.h"
#include "StringEmUp.h"

using cyclOps::Pathiosocic;
using std::wstring;
using std::wstringstream;
using std::string;

namespace cyclOps {
	Pathiosocic::Pathiosocic(const wstring& wstrPath) 
	{
		this->initialize(wstrPath);
	}

	Pathiosocic::Pathiosocic(const string& strPath) 
	{
		wstring wstrPath(strPath.begin(), strPath.end());
		this->initialize(wstrPath);
	}


	Pathiosocic::Pathiosocic(void) { }

	void Pathiosocic::initialize(const wstring& wstrPath) {
		_wstrPath = wstrPath;
		wchar_t wszDrive[5000];
		wchar_t wszDir[5000];
		wchar_t wszFname[5000];
		wchar_t wszExt[5000];
		_wsplitpath_s(
			wstrPath.c_str(), 
			wszDrive, CYCLOPSSIZEOF(wszDrive),
			wszDir, CYCLOPSSIZEOF(wszDir),
			wszFname, CYCLOPSSIZEOF(wszFname),
			wszExt, CYCLOPSSIZEOF(wszExt));
		_wstrDrive.assign(wszDrive);
		_wstrDir.assign(wszDir);
		_wstrFname.assign(wszFname);
		_wstrExt.assign(wszExt);
	}

	Pathiosocic::~Pathiosocic(void)
	{
	}

	string Pathiosocic::getDrive() const {
		string strThing(_wstrDrive.begin(), _wstrDrive.end());
		return strThing;
	}
	string Pathiosocic::getDir() const {
		string strThing(_wstrDir.begin(), _wstrDir.end());
		return strThing;
	}
	string Pathiosocic::getFname() const {
		string strThing(_wstrFname.begin(), _wstrFname.end());
		return strThing;
	}
	string Pathiosocic::getExt() const {
		string strThing(_wstrExt.begin(), _wstrExt.end());
		return strThing;
	}
	string Pathiosocic::getPath() const {
		string path(_wstrPath.begin(), _wstrPath.end());
		return path;
	}
	wstring Pathiosocic::getPathW() const {
		return _wstrPath;
	}
	Pathiosocic Pathiosocic::getUNCFromLocalPath(string strServer) {
		wstring wstrServer(strServer.begin(), strServer.end());
		wstring wstrTemp = _wstrPath;
		std::replace(wstrTemp.begin(), wstrTemp.end(), ':', '$');
		wstring wstrUNC = L"\\\\" + wstrServer + L"\\" + wstrTemp;		
		return Pathiosocic(wstrUNC);
	}

	bool Pathiosocic::isGreaterThanMAX_PATH() const {
		return this->_wstrPath.length() > (MAX_PATH - 1);
	}

	wstring Pathiosocic::getAbsolutePathW() const {
		if ( ! this->isRelative()) { CYCLOPSDEBUG("Path is relative.");
			if (this->isGreaterThanMAX_PATH()) { CYCLOPSDEBUG("Path is greater than MAX_PATH.");
				return this->getPathExtendedLengthW();
			} else { CYCLOPSDEBUG("Path is less than MAX_PATH.  _wstrPath = %S", _wstrPath.c_str());
				return this->_wstrPath;
			}
		} else { 
			if (this->isGreaterThanMAX_PATH()) { 
				/* We have a problem.  Because you cannot use the "\\?\" prefix with a relative path, relative paths 
					are always limited to a total of MAX_PATH characters. */
				string strWhat = cyclOps::StringEmUp::format("The relative path 'S%' is greater than MAX_PATH [%d] characters, which is not supported by Windows OR cyclOps!",
					this->_wstrPath.c_str(), MAX_PATH);
				CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionPathiosocic, strWhat.c_str());
			} else {
				/* Read the "Remarks" in http://msdn.microsoft.com/en-us/library/windows/desktop/aa364963(v=vs.85).aspx, especially the threading issues!!! */
				/* It's safe to call the ANSI version because we have ruled out that the path is greater than MAX_PATH. */
				wchar_t wszFullPathName[MAX_PATH + 1];
				::GetFullPathNameW(this->_wstrPath.c_str(), sizeof(wszFullPathName) / sizeof(wszFullPathName[0]), wszFullPathName, NULL); 
				/* WARNING:  I'm not really sure this is legit!!! */
				return wszFullPathName;
			}
		}
	}

	string Pathiosocic::getAbsolutePath() const {
		wstring wstrPath = this->getAbsolutePathW();
		string strPath(wstrPath.begin(), wstrPath.end());
		return strPath;
	}

	bool Pathiosocic::isRelative() const {
		/* OLD WAY 
		  Although the dox say this is limited to MAX_PATH, I have tested and it works with at least 700 characters! 
		 BOOL boReturn = ::PathIsRelativeA(this->_strPath.c_str());  
		*/


		/* If it starts with a backslash it is absolute or if it starts with a single letter 
		   followed by a colon it is absolute (also c:abc). */
		const char *patternArray[]  = {"^\\\\.*", "^[a-zA-Z]:.*"};
		string strPath(_wstrPath.begin(), _wstrPath.end());
		for (int i = 0; i < (sizeof(patternArray) / sizeof(patternArray[0])); ++i) {
			/* printf("%d : %s, %s\n", i, patternArray[i], _strPath.c_str()); */
			std::regex pattern(patternArray[i]);
			std::smatch sm;    // same as std::match_results<string::const_iterator> sm;
			if (std::regex_match( strPath, sm, pattern)) {
				return false;
			}
		}
		/* Otherwise it is relative. */
		return true;
	}

	wstring Pathiosocic::getPathExtendedLengthW() const {
		/* Because you cannot use the "\\?\" prefix with a relative path, relative paths are always limited to a total of 
		 MAX_PATH characters. http://msdn.microsoft.com/en-us/library/aa365247(VS.85).aspx */
		if (this->isRelative()) {
			wstringstream ss;
			ss << L"The relative path " << _wstrPath << L" cannot be converted to an extended length path.";
			/* stringstream.str() returns a temporary string object that's destroyed at the end of the full expression. */
			wstring error = ss.str();
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionPathiosocic, "%S", error.c_str());
		}
		if (this->isExtendedLength()) {
			return this->_wstrPath;
		} else {
			wstring wstrTemp(this->_wstrPath); CYCLOPSDEBUG("strTemp.length() = %d", wstrTemp.length());
			if (wstrTemp.compare(0, 2, L"\\\\") == 0) {
				/* Remove the double backslash. */
				wstrTemp.erase(0, 2); 
				/* Add UNC\ to beginning. (Yes I could have just removed one backslash in the last step but I think this documents better. */
				wstrTemp = L"UNC\\" + wstrTemp; 
			}
			wstrTemp = L"\\\\?\\" + wstrTemp; 
			return wstrTemp;
		}
	}

	bool Pathiosocic::isExtendedLength() const {
		if (this->_wstrPath.compare(0, 4, L"UNC\\") == 0) {
			CYCLOPSDEBUG("Path starts with UNC\\\n");
			return true;
		}
		if (this->_wstrPath.compare(0, 4, L"\\\\?\\") == 0) {
			CYCLOPSDEBUG("Path starts with \\\\?\\\n");
			return true;
		}
		return false;
	}

	string Pathiosocic::getPathExtendedLength() {
		wstring wstrPathExtendendLength = this->getPathExtendedLengthW();
		string strPathExtendendLength(wstrPathExtendendLength.begin(), wstrPathExtendendLength.end());
		return strPathExtendendLength;
	}
}
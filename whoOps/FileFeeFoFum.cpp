#include "StdAfx.h"

#include <string>
#include <vector>
#include <iterator>
#include <exception>
#include <locale>
#include <codecvt>
#include <iostream>
#include <time.h>
#include <sys\stat.h>

#include "..\cyclOps\cyclOps.h"
#include "..\cyclOps\Pathiosocic.h"
#include "..\cyclOps\StringEmUp.h"

#include "whoOpsBonanza.h"

#include "FileFeeFoFum.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::vector;
using std::iterator;
using std::wstringstream;

namespace whoOps {

	void FileFeeFoFum::initialize(const std::string& strPath) { 
		wstring wstrPath(strPath.begin(), strPath.end()); 
		_path.initialize(wstrPath); 
	}

	FileFeeFoFum::~FileFeeFoFum(void)
	{
	}

	size_t FileFeeFoFum::getSizeInBytes(void) const { CYCLOPSDEBUG("Hello.");
		size_t size = 0;
		/*boost::filesystem::recursive_directory_iterator iterator("c:\\temp");
		if (iterator != boost::filesystem::recursive_directory_iterator()) {
			printf("Hi.");
		}
		++iterator; 
		boost::filesystem::path path = *iterator;
		stringstream ssPath; ssPath << path;
		printf("Ho. %s", ssPath.str().c_str());*/
		if ( ! this->isDirectory()) {
			throw std::runtime_error("File sizes not supported (but you really should add it).");
		}
		for (boost::filesystem::recursive_directory_iterator iterator(_path.getAbsolutePath().c_str());
			iterator != boost::filesystem::recursive_directory_iterator();
			++iterator)
		{
			boost::filesystem::path path = *iterator;
			stringstream ssPath; ssPath << path; CYCLOPSDEBUG("%s", ssPath.str().c_str());
			if ( ! boost::filesystem::is_directory(path)) {
				size += boost::filesystem::file_size(path);
			}
		}
		return size;
	}

	wstring FileFeeFoFum::getFindFirstFileSpec(void) const {
		wstring wstrPath;
		if (this->_path.isGreaterThanMAX_PATH()) {
			wstrPath = this->_path.getPathExtendedLengthW(); 
		} else {
			wstrPath = this->_path.getPathW();
		} CYCLOPSDEBUGW(L"wstrPath = %s", wstrPath.c_str());
		wstrPath.append(L"\\*.*");
		return wstrPath;

	}

	vector<string> FileFeeFoFum::listFilenames(const string& strPattern) const {
		vector<string> list;
		this->listFilenames(list, strPattern);
		return list;
	}

	void FileFeeFoFum::listFilenames(std::vector<std::wstring>& vectorOfFilenames, const string& strPattern) const {
		wstring wstrPattern(strPattern.begin(), strPattern.end());
		WIN32_FIND_DATAW stFindDataSource;
		wstring wstrFindFirstFileSpec = this->getFindFirstFileSpec(); CYCLOPSDEBUG("wstrFindFirstFileSpec = %S", wstrFindFirstFileSpec.c_str());
		HANDLE hFind = FindFirstFileW(wstrFindFirstFileSpec.c_str(), &stFindDataSource);
		BOOL boNextFile = false;
		if (hFind != INVALID_HANDLE_VALUE) {
			do  {
				if ( ! this->isDotOrDoubleDot(stFindDataSource)) { 
					wstring wstrFile(stFindDataSource.cFileName); CYCLOPSDEBUG("'%S' is not a '.' or '..' ", wstrFile.c_str());
					/* TODO:  Here we have an interesting problem.  If we apply a filename filter, and we 
					are doing a recursive search, and this entry is a directory, we want to continue to 
					traverse, but NOT include the directory name in the vectorOfFilenames. */
					if (cyclOps::RegExtravaganza::grep(wstrFile, wstrPattern)) {
						vectorOfFilenames.push_back(wstrFile);
					}
				}
				boNextFile = FindNextFileW(hFind, &stFindDataSource);
			} while (boNextFile);
			FindClose(hFind);
		} else {
			DWORD dwLastError = ::GetLastError(); CYCLOPSVAR(dwLastError, "%d");
			whoOps::ErroroneousMonk error(dwLastError);
			string strError = error.formatMessage();
			string strWhat = cyclOps::StringEmUp::format("Error from FindFirstFileW('%S') %d - %s", wstrFindFirstFileSpec.c_str(), error.getLastError(), error.formatMessage().c_str()); CYCLOPSDEBUG("strWhat = %s", strWhat.c_str());
			switch (dwLastError) {
			case 5:
				CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionAccessDenied, strWhat);
			case 59:
				CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionUnexpectedNetworkError, strWhat);
			default:
				// CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionFileFeeFoFum, error.getLastError(), "%s", strWhat.c_str());
				CYCLOPSDEBUG("strWhat = %s", strWhat.c_str());
				char* pchWhat = cyclOps::StringEmUp::new_charArray(strWhat); CYCLOPSVAR(pchWhat, "%s");
				CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionFileFeeFoFum, "%s", pchWhat);
				delete[] pchWhat;
			}
		}
	}


	void FileFeeFoFum::listFilenames(vector<string>& vectorOfFilenames, const std::string& strPattern) const {
		vector<wstring> vectorOfFilenamesW;
		this->listFilenames(vectorOfFilenamesW, strPattern);
		for (size_t i = 0; i < vectorOfFilenamesW.size(); ++i) {
			wstring wstrFilename = vectorOfFilenamesW[i];
			string strFilename(wstrFilename.begin(), wstrFilename.end());
			vectorOfFilenames.push_back(strFilename);
		}
	}

	bool FileFeeFoFum::isDotOrDoubleDot(const WIN32_FIND_DATA& stFindData) const {  CYCLOPSDEBUG("cFileName = %S", stFindData.cFileName);
		if (wcscmp(stFindData.cFileName, L".") == 0 || wcscmp(stFindData.cFileName, L"..") == 0) {  
			return true;
		} else {
			return false;
		}
	}

	vector<FileFeeFoFum> FileFeeFoFum::listFiles(const string& strPattern) const {
		vector<FileFeeFoFum> list;
		this->listFiles(list, strPattern);
		return list;
	}

	void FileFeeFoFum::copyToDirectory(const std::string& strTarget) const {
		wstring wstrTarget(strTarget.begin(), strTarget.end());
		this->copyToDirectory(wstrTarget);
	}
	 
	void FileFeeFoFum::copyToDirectory(const wstring& wstrTargetDirectory) const {
		wstring wstrSource = this->getAbsolutePathW(); // this->_path.getPathExtendedLengthW();
		wstring wstrTargetFile = wstrTargetDirectory + L"\\" + this->getFnameWithExtW();
		BOOL boResult = CopyFileW(wstrSource.c_str(), wstrTargetFile.c_str(), false /* Don't fail if exists. */);
		if ( ! boResult) { 
			DWORD dwLastError = ::GetLastError();
			ErroroneousMonk err(dwLastError);
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToCopyFile, "Unable to copy '%S' to '%S'.  The error was '%s'.",
				wstrSource.c_str(), wstrTargetFile.c_str(), err.formatMessage().c_str());
		}
	}

	void FileFeeFoFum::listFiles(std::vector<whoOps::FileFeeFoFum>& fileList) const {
		this->listFiles(fileList, ".*");
	}

	void FileFeeFoFum::listFiles(vector<FileFeeFoFum>& fileList, const string& strPattern) const { CYCLOPSHELLO();
		vector<wstring> fileNameList;
		this->listFilenames(fileNameList, strPattern); CYCLOPSDEBUG("fileNameList.size() = %d", fileNameList.size());
		for (vector<wstring>::iterator i = fileNameList.begin(); i < fileNameList.end(); ++i) {
			wstringstream wssPath;
			wssPath << this->_path.getPathW() << L"\\" << *i; CYCLOPSDEBUG("wssPath = %S", wssPath.str().c_str());
			FileFeeFoFum file(wssPath.str());
			fileList.push_back(file); 
		} CYCLOPSDEBUG("fileList.size = %d", fileList.size());
	}

	void FileFeeFoFum::listFilesRecursive(std::vector<whoOps::FileFeeFoFum>& fileList, const string& strPattern) const { CYCLOPSHELLO();
		if ( ! this->isDirectory()) { 
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionFileFeeFoFum, "'%s' is not a directory.", this->_path.getAbsolutePath().c_str());
		}
		vector<FileFeeFoFum> vectorOfEntries;
		/* We have to retrieve all files, not just those matching the pattern.  Otherwise we will
		   not traverse the subdirectories whose name does not match the pattern. */
		this->listFiles(vectorOfEntries, ".*"); CYCLOPSDEBUG("vectorOfEntries.size() = %d", vectorOfEntries.size());
		for (size_t i = 0; i < vectorOfEntries.size(); ++i) {
			FileFeeFoFum entry = vectorOfEntries[i];
			if (entry.isDirectory()) {
				entry.listFilesRecursive(fileList, strPattern);
			} else {
				if (cyclOps::RegExtravaganza::grep(entry.getFnameWithExt(), strPattern)) {
					fileList.push_back(entry);
				}
			}
		}
	}

	string FileFeeFoFum::getName() {
		stringstream ssFname;
		ssFname << this->_path.getFname() << this->_path.getExt();
		return ssFname.str();
	}

	string FileFeeFoFum::getAbsolutePath() const {
		wstring wstrAbs = this->_path.getAbsolutePathW();
		string strAbs(wstrAbs.begin(), wstrAbs.end());
		return strAbs;
	}

	wstring FileFeeFoFum::getAbsolutePathW() const {
		return _path.getAbsolutePathW();
	}

	bool FileFeeFoFum::isDirectory() const { 
		string strPath = this->_path.getPath(); CYCLOPSDEBUG("strPath = %s", strPath.c_str());
		DWORD dwFileAttributes = ::GetFileAttributesA(strPath.c_str()); CYCLOPSDEBUG("dwFileAttributes = %d", dwFileAttributes);
		bool boReturn;
		if (dwFileAttributes == INVALID_FILE_ATTRIBUTES) { CYCLOPSDEBUG("dwFileAttributes contains INVALID_FILE_ATTRIBUTES.");
			boReturn = false;
		} else if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { CYCLOPSDEBUG("%s is a directory.", strPath.c_str());
			boReturn = true; 
		} else { CYCLOPSDEBUG("Attributes are neither INVALID_FILE_ATTRIBUTES nor FILE_ATTRIBUTE_DIRECTORY", strPath.c_str());
			boReturn = false;
		}
		return boReturn;
	}

	bool FileFeeFoFum::exists() const { CYCLOPSDEBUG("Hello.");
		struct _stat stSourceStat;
		wstring wstrAbsolutePath = this->_path.getAbsolutePathW();
		string strAbsolutePath(wstrAbsolutePath.begin(), wstrAbsolutePath.end());
		int iReturn = _stat(strAbsolutePath.c_str(), &stSourceStat); CYCLOPSVAR(iReturn, "%d");
		return iReturn == 0;
	}

	void FileFeeFoFum::setReadWrite() {
		wstring wstrFile = this->_path.getAbsolutePathW();
	    DWORD dwAttrs = GetFileAttributesW(wstrFile.c_str()); 
		if (dwAttrs != 0xFFFFFFFF)
		{
			if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) 
			{ 
				SetFileAttributesW(wstrFile.c_str(), dwAttrs & (~FILE_ATTRIBUTE_READONLY)); 
			} 
		}    
	}

	void FileFeeFoFum::writeLineAppend(const string& strLine) const {
		this->writeLine(strLine, std::fstream::app);
	}

	void FileFeeFoFum::writeLine(const string& strLine, std::ios_base::openmode mode) const {
		std::ofstream file;
		string strFile = this->_path.getAbsolutePath();
		/* Use std::ios::trunc to overwrite file. */
		/* std::ios::app as mode will append. */
		file.open(strFile, mode);
		if (file.fail()) {
			char error[5000];
			strerror_s(error, CYCLOPSSIZEOF(error), errno);
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToOpenFile, "The file '%s' cannot be opened.  [%s]", strFile.c_str(), error);
		}

		file << strLine << "\n";
		file.close();
	}

	void FileFeeFoFum::writeLine(const wstring& wstrLine, std::ios_base::openmode mode) const { CYCLOPSDEBUG("wstrLine = %S", wstrLine.c_str());
		std::wofstream file;
		wstring wstrFile = this->_path.getAbsolutePathW();
		file.open(wstrFile, mode);
		if (file.fail()) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToOpenFile, "The file '%S' cannot be opened.", wstrFile.c_str());
		}
		file << wstrLine << L"\n";
		file.close();
	}

	void FileFeeFoFum::getContentsAsVectorUTF16LittleEndian_I(vector<wstring>& vectorOfContents) {
		/* This guy doesn't work, use II instead. */
		/* http://unicode.org/faq/utf_bom.html#BOM */
		/* http://cfc.kizzx2.com/index.php/reading-a-unicode-utf16-file-in-windows-c */
		wstring wstrPath = this->_path.getAbsolutePathW();
		string strPath(wstrPath.begin(), wstrPath.end());
		std::ifstream input(strPath, std::ios::binary);
		if (input.fail()) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToOpenFile, "The file '%s' cannot be opened.", strPath.c_str());
		}
		stringstream ss;
		ss << input.rdbuf() << '\0'; CYCLOPSDEBUG("%S\n", (wchar_t*) ss.str().c_str());
		wstring text((wchar_t *) ss.str().c_str());
		input.close();
		printf("text %S\n", text.c_str());
	}

	void FileFeeFoFum::getContentsAsVectorUTF16LittleEndian_II(vector<wstring>& vectorOfContents) {
		/* Cubbi's answer in: http://stackoverflow.com/questions/10504044/correctly-reading-a-utf-16-text-file-into-a-string-without-external-libraries */
		string path = this->_path.getAbsolutePath();
		std::wifstream input(path, std::ios::binary);
		if (input.fail()) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToOpenFile, "The file '%s' cannot be opened.", path.c_str());
		}
		input.imbue(std::locale(input.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
		wstring wstrOut;
		for (wchar_t c; input.get(c); ) {
			if (c == '\r') {
				/* Eat it. */
			} else if (c == '\n') { CYCLOPSDEBUG("Going to push_back() %S", wstrOut.c_str());
				vectorOfContents.push_back(wstrOut);
				wstrOut.clear();
			} else {
				wstrOut.push_back(c);
			}
		}
		if (wstrOut.length() > 0) {
			vectorOfContents.push_back(wstrOut);
		}
		input.close();
	}

	void FileFeeFoFum::getContentsAsVector(vector<string>& contents) { 
		wstring wstrPath = this->_path.getAbsolutePathW(); CYCLOPSDEBUG("wstrPath = %S", wstrPath.c_str());
		string strPath(wstrPath.begin(), wstrPath.end()); CYCLOPSDEBUG("strPath = %s", strPath.c_str());
		std::ifstream input(strPath);
		if (input.fail()) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToOpenFile, "The file '%s' cannot be opened.", strPath.c_str());
		}
		for (string line; std::getline(input, line ); ) { CYCLOPSDEBUG("line = %s", line.c_str());
			contents.push_back(line);
		}
		input.close();
	}

	bool FileFeeFoFum::isOlderThanXDays(int iMaxDays) const {
		time_t tDiffSecs = this->getAgeInSeconds();
		time_t iDiffDays = tDiffSecs / 86400; CYCLOPSVAR(iDiffDays, "%d");
		return iDiffDays > iMaxDays;
	}

	bool FileFeeFoFum::isOlderThanISOTime(const string& isoTime) const {
		time_t tLastModified = this->getLastModifiedAsTime_t();
		cyclOps::TimeTiger tiger;
		time_t tISOTime = tiger.get_time_tFromISO8601(isoTime);
		return tISOTime > tLastModified;
	}

	time_t FileFeeFoFum::getAgeInHours() const {
		time_t diffInSeconds = this->getAgeInSeconds();
		return diffInSeconds / (60 * 60);
	}

	time_t FileFeeFoFum::getAgeInSeconds() const {
		time_t tLastModified = this->getLastModifiedAsTime_t();
		time_t tCurrentTime = ::time(NULL); CYCLOPSVAR(tCurrentTime, "%d");
		return tCurrentTime - tLastModified; 
	}

	string FileFeeFoFum::getLastModifiedAsISO8601(void) const {
		time_t timeLastModified = this->getLastModifiedAsTime_t();
		cyclOps::TimeTiger tiger;
		string iso8601 = tiger.getISO8601From_time_t(timeLastModified);
		return iso8601;
	}

	/**
		From: https://msdn.microsoft.com/en-us/library/vstudio/14h5k7ff(v=vs.100).aspx

		"Users should note that there is undocumented behavior when using the 32-bit _stat functions 
		with files whose sizes are too large to be represented by 32 bits.  In Visual Studio 2005, 
		_stat will place the lower 32 bits of the length in st_size.  In Visual Studio 2010, _stat 
		will return -1 and will not set errno appropriately.  _stati64 should work correctly."

	**/
	time_t FileFeeFoFum::getLastModifiedAsTime_t(void) const {
		struct _stat64 buf;
		int result = _stati64( this->_path.getAbsolutePath().c_str(), &buf);
		if( result != 0 ) {
			const char* pchError;
			int myErrno = errno;
			switch (myErrno) {
				case ENOENT:
					pchError = "File not found.";
					break;
				case EINVAL:
					pchError = "Invalid parameter to _stat.";
					break;
				default:
					/* Should never be reached. */
					pchError = "Unexpected error in _stat.";
			}
			stringstream ss;
			ss << "Error during _stat() for '" << _path.getAbsolutePath() << "' - errno = " << myErrno << " " << pchError;
			throw std::runtime_error(ss.str().c_str());
		} CYCLOPSDEBUG("Modified time of %s: %d", this->_path.getFname().c_str(), buf.st_mtime);
		return buf.st_mtime;
	}

	void FileFeeFoFum::deleet(const unsigned int flags) const {
		wstring wstrPath;
		if (flags && FLAG_USE_EXTENDED_PATH) {
			wstrPath = this->_path.getPathExtendedLengthW(); 
		} else {
			wstrPath = this->_path.getAbsolutePathW(); 
		}
		if ( ! this->exists() ) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionFileFeeFoFum, "'%S' does not exist.", wstrPath.c_str());
		} else {
			if (this->isDirectory()) {
				this->removeDirectory(wstrPath);
			} else {
				if ( ! ::DeleteFileW(wstrPath.c_str())) {
					DWORD dwLastError = ::GetLastError();
					CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionFileFeeFoFum, dwLastError, 
						cyclOps::Exception::TYPE_WIN32, 
						"Unable to delete %S.", wstrPath.c_str());
				}
			}
		}
	}

	void FileFeeFoFum::removeDirectory(const wstring& wstrDirectory) const { CYCLOPSDEBUG("wstrDirectory = %S", wstrDirectory.c_str());
		wstring wstrFileSpec = wstrDirectory + L"\\*.*";
		WIN32_FIND_DATA stFindData;
		HANDLE hFind = FindFirstFile(wstrFileSpec.c_str(), &stFindData);
		if (hFind == INVALID_HANDLE_VALUE) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionFileFeeFoFum, "File spec '%S' matches no files.", wstrFileSpec.c_str());
		} else {
			do {
				if ( ! this->isSelfOrParent(stFindData.cFileName) ) { CYCLOPSDEBUG("cFileName = %S", stFindData.cFileName);
					wstring wstrTarget = wstrDirectory + L"\\" + stFindData.cFileName;
					this->setReadWrite(wstrTarget);
					if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						this->removeDirectory(wstrTarget);
					} else {
						cyclOps::Pathiosocic path(wstrTarget);
						wstring wstrPathExtendedLength = path.getPathExtendedLengthW();
						DeleteFileW(wstrPathExtendedLength.c_str());
					}
				} 
			} while (FindNextFile(hFind, &stFindData));
			FindClose(hFind);
		} 
		BOOL boReturn = RemoveDirectoryW(wstrDirectory.c_str());
		if ( ! boReturn ) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionFileFeeFoFum, "Unable to delete '%S'.", wstrDirectory.c_str());
		}
	}

	bool FileFeeFoFum::isSelfOrParent(wstring const& wstrThing) const {
		if (wcscmp(wstrThing.c_str(), L".") == 0) {
			return true;
		} else if (wcscmp(wstrThing.c_str(), L"..") == 0) {
			return true;
		} else {
			return false;
		}
	}

	void FileFeeFoFum::setReadWrite(const wstring& wstrFile) const {
		DWORD dwAttrs = GetFileAttributesW(wstrFile.c_str()); 
		if (dwAttrs != 0xFFFFFFFF) {
			if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) { 
				SetFileAttributesW(wstrFile.c_str(), dwAttrs & (~FILE_ATTRIBUTE_READONLY)); 
			} 
		}    
	}
	
	string FileFeeFoFum::getFnameWithExt(void) const { 
		string strFname = _path.getFname() + _path.getExt();  CYCLOPSDEBUG("strFname = %s", strFname.c_str());
		return strFname;
	}

	bool FileFeeFoFum::containsText(const std::string& text) {
		wstring wstrPath = this->_path.getAbsolutePathW();
		string strPath(wstrPath.begin(), wstrPath.end());
		std::ifstream input(strPath);
		if (input.fail()) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionUnableToOpenFile, "The file '%s' cannot be opened.", strPath.c_str());
		}
		string line;
		bool found = false;
		while (std::getline(input, line ) && ! found ) { CYCLOPSDEBUG("line = %s", line.c_str());
			if (cyclOps::StringEmUp::contains(line, text)) {
				found = true;
			}
		}
		input.close();
		return found;
	}
}
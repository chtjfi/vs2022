#pragma once

#include <string>

#include "..\cyclOps\Logger.h"

namespace slOps {
		class Synculator
	{
	public:
		explicit Synculator(void);
		~Synculator(void);
		void setIncludeSubdirectories(const bool& boIncludeSubs);
		void setPurge(const bool& boPurge);
		void sync(const char* szSourceDir, const char* szTargetDir);
		void sync(const std::string& strSource, const std::string& strTarget);
		bool isSuccesful(void) { return this->_boSuccesful; }
		void setContinueOnCopyFailure(bool boContinue) { _boContinueOnCopyFailure = boContinue; }
	private:
		bool _boPurge;
		bool _boIncludeSubdirectories;
		bool _boSuccesful;
		bool _boContinueOnCopyFailure;
		void copyDirectory(const std::wstring& strSource, const std::wstring& strTarget);
		void purgeTarget(const std::string& strSource, const std::string& strTarget);
		bool isDirectory(const WIN32_FIND_DATA& stFindData);
		void copySubdirectory(const std::wstring& wstrSourceDirectory, const std::wstring& wstrTargetDirectory, const WIN32_FIND_DATA& findData);
		bool doFilesMatch(const WIN32_FIND_DATA& findDataSource, const std::wstring& strTargetFile);
		void copyFile(const std::wstring& wstrSourceFile, const std::wstring& wstrTargetFile);
		void copyFileIfNecessary(const std::wstring& wstrSourceDirectory, const std::wstring& wstrTargetDirectory, const WIN32_FIND_DATA& findData);
		bool isDotOrDoubleDot(const WIN32_FIND_DATA& stFindData);
		std::wstring adjustPath(const std::wstring& strPath);
		void setReadWrite(const std::wstring& strFile);
		void processEntry(const WIN32_FIND_DATAW& stFindDataSource, const std::wstring& wstrSourceAdjusted, const std::wstring& wstrTargetAdjusted);
		HANDLE findFirstFile(const std::wstring& wstrDirectory, WIN32_FIND_DATAW& stFindDataSource);
		bool remove(const char* szTargetFile);
		bool doesPathEndWithBackslash(const std::wstring& path);
	};
}
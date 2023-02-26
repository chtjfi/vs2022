#pragma once

#include <vector>
#include <ios>
#include <fstream>
#include <boost/filesystem.hpp>

#include <Windows.h>

#include "..\cyclOps\Pathiosocic.h"

namespace whoOps {
	class FileFeeFoFum
	{
	public:
		static const unsigned int FLAG_DO_NOTHING_BECAUSE_I_MISS_MO = 0x00;
		static const unsigned int FLAG_USE_EXTENDED_PATH = 0x01;
		/* You can pass the path in the constructor... */
		explicit FileFeeFoFum(const std::string& strPath) : _path(strPath) { };
		explicit FileFeeFoFum(const std::wstring& wstrPath) : _path(wstrPath) { };
		/* ...or you can initialize later, useful when file is a member or global 
		 whose path you don't know at construction time. */
		explicit FileFeeFoFum(void) { } ;
		void initialize(const std::string& strPath);
		~FileFeeFoFum(void);

		size_t getSizeInBytes(void) const;
		std::vector<std::string> listFilenames(const std::string& strPattern) const;
		void listFilenames(std::vector<std::string>& vectorOfFilenames, const std::string& strPattern) const;
		void listFilenames(std::vector<std::wstring>& vectorOfFilenames, const std::string& strPattern) const;
		void listFiles(std::vector<whoOps::FileFeeFoFum>& fileList) const;
		std::vector<FileFeeFoFum> listFiles(const std::string& strPattern) const;
		void listFiles(std::vector<whoOps::FileFeeFoFum>& fileList, const std::string& strPattern) const;
		void listFilesRecursive(std::vector<whoOps::FileFeeFoFum>& fileList, const std::string& strPattern) const;
		std::string getName();
		std::string getAbsolutePath() const;
		std::wstring getAbsolutePathW() const;
		bool isDirectory() const;
		bool isOlderThanXDays(int i) const;
		bool isOlderThanISOTime(const std::string& isoTime) const;
		time_t getLastModifiedAsTime_t(void) const;
		std::string getLastModifiedAsISO8601(void) const;
		time_t getAgeInSeconds() const;
		time_t getAgeInHours() const;
		bool exists() const;
		void setReadWrite();
		void getContentsAsVector(std::vector<std::string>& contents );
		void getContentsAsVectorUTF16LittleEndian_I(std::vector<std::wstring>& v);
		void getContentsAsVectorUTF16LittleEndian_II(std::vector<std::wstring>& v);
		void writeLine(const std::string& strLine, std::ios_base::openmode mode = std::ios::app) const;
		void writeLine(const std::wstring& wstrLine, std::ios_base::openmode mode = std::ios::app) const;
		void writeLineAppend(const std::string& strLine) const;
		void deleet(const unsigned int flags) const;
		std::string getFname(void) const { return _path.getFname(); }
		std::string getFnameWithExt(void) const;
		std::string getParent(void) const { 
			std::wstring wstrParent;
			_path.getParent(wstrParent);
			std::string strParent(wstrParent.begin(), wstrParent.end());
			return strParent;
		}
		std::wstring getFnameWithExtW(void) const { return _path.getFnameW() + _path.getExtW(); }
		void copyToDirectory(const std::string& strTarget) const;
		void copyToDirectory(const std::wstring& wstrTarget) const;
		bool containsText(const std::string& text);
	private:
		/* Prohibit copy constructor and assignment operator. */
//		FileFeeFoFum(const FileFeeFoFum& f);
//		FileFeeFoFum& operator=(const FileFeeFoFum& f);
		cyclOps::Pathiosocic _path;
		bool isDotOrDoubleDot(const WIN32_FIND_DATA& stFindData) const;
		void removeDirectory(const std::wstring& wstrDirectory) const;
		bool isSelfOrParent(const std::wstring & wstrThing) const;
		void setReadWrite(const std::wstring& wstrFile) const;
		std::wstring getFindFirstFileSpec(void) const;
	};
}
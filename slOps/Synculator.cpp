#include "StdAfx.h"

#include <iostream>

#include <boost/filesystem.hpp>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

#include "slOpsPrivate.h"

#include "Synculator.h"

/* DO NOT PUT A using std::string HERE BECAUSE YOU WANT TO DISCOURAGE ANSI STRING USAGE !!! */
using std::wstring;
using std::string;
using std::wstringstream;

namespace slOps {
	Synculator::Synculator(void) : _boSuccesful(true), _boPurge(false),
		_boIncludeSubdirectories(true), _boContinueOnCopyFailure(false) { }

	Synculator::~Synculator(void) {	}

	void Synculator::setIncludeSubdirectories(const bool& boIncludeSubs) { SLOPSDEBUG("Hello.");
		_boIncludeSubdirectories = boIncludeSubs; 
	}

	void Synculator::setPurge(const bool& boPurge) { SLOPSDEBUG("Hello.");
		_boPurge = boPurge; 
	}

	/* I am allowing ANSI strings here because this is the entry point. */
	void Synculator::sync(const char* szSourceDir, const char* szTargetDir) { 
		SLOPSINFO("Synchronizing %s to %s.", szSourceDir, szTargetDir);
		if ( ! szSourceDir) { 
			throw slOps::ExceptionInvalidArguments("The source directory was null.");
		}
		if ( ! szTargetDir) {
			throw slOps::ExceptionInvalidArguments("The target directory was null.");
		}
		this->sync(std::string(szSourceDir), std::string(szTargetDir));
		SLOPSDEBUG("Done synchronizing %s to %s.", szSourceDir, szTargetDir);
	}

	/* I am allowing ANSI strings here because this is the entry point. */
	void Synculator::sync(const std::string& strSource, const std::string& strTarget) { SLOPSDEBUG("Hello.");
		whoOps::FileFeeFoFum sourceFile(strSource);
		whoOps::FileFeeFoFum targetFile(strTarget);
		if ( ! sourceFile.isDirectory()) {
			CYCLOPS_THROW_EXCEPTION_IV(slOps::ExceptionNoSuchDirectory, "There is no directory named '%s'", strSource.c_str());
		} 
		this->copyDirectory(sourceFile.getAbsolutePathW(), targetFile.getAbsolutePathW());
		if (_boPurge) {
			this->purgeTarget(strSource, strTarget);
		}
	}

	void Synculator::copySubdirectory(const wstring& wstrSourceDirectory, const wstring& wstrTargetDirectory, 
		const WIN32_FIND_DATA& findData) {
		SLOPSDEBUG("wstrSourceDirectory = %S", wstrSourceDirectory.c_str()); SLOPSDEBUG("wstrTargetDirectory = %S", wstrTargetDirectory.c_str());
		size_t iSourceDirLength = wstrSourceDirectory.length() + 1 /* for the backslash */ 
			+ wcslen(findData.cFileName) + 1 /* for the trailing null */; SLOPSDEBUG("iSourceDirLength = %d", iSourceDirLength );
		wchar_t* pwchSourceDirectory = new wchar_t[iSourceDirLength];
		_snwprintf_s(pwchSourceDirectory, iSourceDirLength, _TRUNCATE, L"%s\\%s", wstrSourceDirectory.c_str(), findData.cFileName); SLOPSDEBUG("pchSourceDirectory = %S", pwchSourceDirectory);
		size_t iTargetDirLength = wstrTargetDirectory.length() + 1 /* for the backslash */ 
			+ wcslen(findData.cFileName) + 1 /* for the trailing null */; SLOPSDEBUG("iTargetDirLength = %d", iTargetDirLength );
		wchar_t* pwchTargetDirectory = new wchar_t[iTargetDirLength];
		bool boTargetEndsWithBackslash = this->doesPathEndWithBackslash(wstrTargetDirectory);
		_snwprintf_s(pwchTargetDirectory, iTargetDirLength, _TRUNCATE, L"%s%s%s", wstrTargetDirectory.c_str(), 
			boTargetEndsWithBackslash ? L"" : L"\\", findData.cFileName); SLOPSDEBUG("pchTargetDirectory = %S", pwchTargetDirectory);
		try {
			this->copyDirectory(pwchSourceDirectory, pwchTargetDirectory); SLOPSDEBUG("copyDirectory() done.");
		} catch (const std::exception& e) {
			this->_boSuccesful = false;
			SLOPSERROR("Failed to copy directory '%S' to '%S'.  Full error message on next line.", pwchSourceDirectory, pwchTargetDirectory);
			SLOPSERROR("%s [%s]", e.what(), typeid(e).name());
			if (!_boContinueOnCopyFailure) {
				throw;
			}
		}
		delete [] pwchSourceDirectory;
		delete [] pwchTargetDirectory;
	}

	wstring Synculator::adjustPath(const wstring& wstrPath) {
		cyclOps::Pathiosocic path(wstrPath); 
		if (true /* path.isGreaterThanMAX_PATH() */) { SLOPSDEBUG("Path is greater than MAX_PATH");
			wstring wstrExtendedLength = path.getPathExtendedLengthW(); SLOPSDEBUG("strExtendedLength = %S", wstrExtendedLength.c_str());
			return wstrExtendedLength;
		} else { SLOPSDEBUG("%S (%d chars) is less than %d.", wstrPath.c_str(), wstrPath.size(), MAX_PATH);
			return wstrPath;
		}
	}

	HANDLE Synculator::findFirstFile(const wstring& wstrDirectory, WIN32_FIND_DATAW& stFindDataSource) {
		wstring wstrSourceFileSpec = wstrDirectory + L"\\*.*";
		//wstrSourceFileSpec.append(L"\\*.*");
		HANDLE hFind = FindFirstFileW(wstrSourceFileSpec.c_str(), &stFindDataSource); 
		if (hFind != INVALID_HANDLE_VALUE) { SLOPSDEBUG("cFileName = %S", stFindDataSource.cFileName);
			return hFind;
		} else {
			DWORD dwLastError = ::GetLastError(); SLOPSDEBUG("dwLastError = %d / ERROR_FILE_NOT_FOUND = %d", dwLastError, ERROR_FILE_NOT_FOUND);
			whoOps::ErroroneousMonk error(dwLastError);
			if (dwLastError == ERROR_FILE_NOT_FOUND) {
				CYCLOPS_THROW_EXCEPTION_IV(slOps::ExceptionNoMatchingFiles, "ERROR_FILE_NOT_FOUND (No files matching %S can be found.  [%s])", 
					wstrSourceFileSpec.c_str(), error.formatMessage().c_str());
			} else {
				std::stringstream massage;
				massage << error.formatMessage();
				CYCLOPS_THROW_EXCEPTION_IV(slOps::ExceptionSynculator, 
					"FindFirstFileW(%S) failed: '%s'", wstrSourceFileSpec.c_str(), massage.str().c_str()); 
			}
		} 
	}

	void Synculator::copyDirectory(const wstring& wstrSource, const wstring& wstrTarget) { SLOPSDEBUG("wstrSource = %S", wstrSource.c_str());
		wstring wstrSourceDirAdjusted = this->adjustPath(wstrSource); 
		wstring wstrTargetDirAdjusted = this->adjustPath(wstrTarget); SLOPSDEBUG("wstrTargetDirAdjusted = %S", wstrTargetDirAdjusted.c_str());
		DWORD dwHandlesOne = whoOps::ProcessPrincess::GetCurrentProcessHandleCount();
		boost::filesystem::create_directories(wstrTargetDirAdjusted);
		WIN32_FIND_DATAW stFindDataSource; 
		HANDLE hFind = this->findFirstFile(wstrSourceDirAdjusted, stFindDataSource);
		BOOL boNextFile = false;
		do  {
			this->processEntry(stFindDataSource, wstrSourceDirAdjusted, wstrTargetDirAdjusted);
			boNextFile = FindNextFileW(hFind, &stFindDataSource); SLOPSDEBUG("boNextFile = %s", boNextFile ? "true" : "false");
		} while (boNextFile); 
		FindClose(hFind);
		DWORD dwHandlesTwo = whoOps::ProcessPrincess::GetCurrentProcessHandleCount();
		CYCLOPSDEBUG("Handle count is %d/%d.", dwHandlesOne, dwHandlesTwo);
	}

	void Synculator::processEntry(const WIN32_FIND_DATAW& stFindDataSource, const wstring& wstrSourceAdjusted, 
		const wstring& wstrTargetDirectoryAdjusted) { SLOPSDEBUG("wstrTargetDirectoryAdjusted = %S", wstrTargetDirectoryAdjusted.c_str());
		/* IMPORTANT! Do nothing if this is the directory itself or the parent.  
		Not checking will cause you to call copySubdirectory() on the directory itself. */
		if ( ! this->isDotOrDoubleDot(stFindDataSource)) {
			if (this->isDirectory(stFindDataSource)) {
				if (this->_boIncludeSubdirectories) {
					this->copySubdirectory(wstrSourceAdjusted, wstrTargetDirectoryAdjusted, stFindDataSource); SLOPSDEBUG("copySubdirectory() done.");
				}
			} else { 
				this->copyFileIfNecessary(wstrSourceAdjusted, wstrTargetDirectoryAdjusted, stFindDataSource);
			}
		}
	}

	/* TODO: This should be fixed to only accept UNICODE strings !!! */
	void Synculator::purgeTarget(const std::string& strSourceDir, const std::string& strTargetDir) {
		wchar_t wszTargetFileSpec[SLOPS_MAX_PATH] = { 0 };
		_snwprintf_s(wszTargetFileSpec, SLOPSSIZEOF(wszTargetFileSpec), _TRUNCATE, L"%S\\*.*", strTargetDir.c_str());
		WIN32_FIND_DATA stTargetFindData;
		HANDLE hFind = FindFirstFile(wszTargetFileSpec, &stTargetFindData);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (wcscmp(stTargetFindData.cFileName, L".") != 0) {
					if (wcscmp(stTargetFindData.cFileName, L"..") != 0) {
						char szTargetFile[SLOPS_MAX_PATH] = { 0 };
						_snprintf_s(szTargetFile, SLOPSSIZEOF(szTargetFile), _TRUNCATE, "%s\\%S", strTargetDir.c_str(), stTargetFindData.cFileName);
						char szSourceFile[SLOPS_MAX_PATH] = { 0 };
						_snprintf_s(szSourceFile, SLOPSSIZEOF(szSourceFile), _TRUNCATE, "%s\\%S", strSourceDir.c_str(), stTargetFindData.cFileName); SLOPSDEBUG("checking if source file exists");
						whoOps::FileFeeFoFum fileSource(szSourceFile);
						if ( ! fileSource.exists()) { SLOPSDEBUG("source file does not exist, purging!");
							this->remove(szTargetFile);
						} else { 
							if (this->isDirectory(stTargetFindData)) {
								this->purgeTarget(szSourceFile, szTargetFile);
							}
						}
					} 
				} 
			}  while (FindNextFile(hFind, &stTargetFindData));
		} 
	}

	bool Synculator::remove(const char* szTargetFile) {
		SLOPSINFO("Purging %s", szTargetFile);
		whoOps::FileFeeFoFum fileTarget(szTargetFile); 
		fileTarget.setReadWrite();
		bool boSuccessful = true;
		if (fileTarget.isDirectory()) { SLOPSDEBUG("%s is a directory.  ", szTargetFile);
			bool boCool = ::slOpsRemoveDirectory(szTargetFile, true);
			if ( ! boCool) { SLOPSERROR(" Unable to remove directory %s", szTargetFile);
				boSuccessful = false;
			} else { SLOPSDEBUG("succesfully purged.  Hooray!"); }
		} else {
			BOOL boCool = DeleteFileA(szTargetFile);
			if (!boCool) { SLOPSERROR(" Unable to delete file %s", szTargetFile);
				boSuccessful = false;
			} else { SLOPSDEBUG("...succesfully purged.  Hooray!"); }
		}
		if ( ! boSuccessful) { this->_boSuccesful = false; }
		return boSuccessful;
	}

	bool Synculator::isDirectory(const WIN32_FIND_DATA& stFindData) { SLOPSDEBUG("Inspecting %S.", stFindData.cFileName);
		if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			return true; 
		}
		return false;
	}

	bool Synculator::isDotOrDoubleDot(const WIN32_FIND_DATA& stFindData) { SLOPSDEBUG("cFileName = %S", stFindData.cFileName);
		if (wcscmp(stFindData.cFileName, L".") == 0 || wcscmp(stFindData.cFileName, L"..") == 0) {
			return true;
		} 
		return false;
	}

	bool Synculator::doFilesMatch(const WIN32_FIND_DATA& findDataSource, const wstring& wstrTarget) {
		WIN32_FIND_DATA findDataTarget;
		HANDLE hFindTarget = ::FindFirstFile(wstrTarget.c_str(), &findDataTarget);
		if (hFindTarget == INVALID_HANDLE_VALUE) { CYCLOPSDEBUG("INVALID_HANDLE_VALUE returned by FindFirstFile() for %S.", wstrTarget.c_str());
			DWORD dwLastError = ::GetLastError();
			if (dwLastError == ERROR_FILE_NOT_FOUND) { CYCLOPSDEBUG("ERROR_FILE_NOT_FOUND");
				return false;
			} else {
				whoOps::ErroroneousMonk error(dwLastError);
				throw std::runtime_error(error.formatMessage()); 
			}
		}  else {
			FindClose(hFindTarget);
		}
		FILETIME file_time1 = findDataSource.ftLastWriteTime;
		FILETIME file_time2 = findDataTarget.ftLastWriteTime;
		LONGLONG diffInTicks =
			((LARGE_INTEGER*)(&file_time1))->QuadPart -
			((LARGE_INTEGER*)(&file_time2))->QuadPart;
		                                    //1000000000
		LONGLONG diffInMillis = diffInTicks / 10000; SLOPSDEBUG("diffInMillis = %d", diffInMillis); 
		if (diffInMillis > 3000 || diffInMillis < -3000) { SLOPSDEBUG("Returning false.");
			return false;
		} else { SLOPSDEBUG("Returning true.");
			return true;
		} 
	}

	void Synculator::setReadWrite(const wstring& wstrFile) {
		DWORD dwAttrs = GetFileAttributesW(wstrFile.c_str()); 
		if (dwAttrs != 0xFFFFFFFF)
		{
			if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) 
			{ 
				SetFileAttributesW(wstrFile.c_str(), dwAttrs & (~FILE_ATTRIBUTE_READONLY)); 
			} 
		}    
	}

	void Synculator::copyFile(const wstring& wstrSource, const wstring& wstrTarget) { 
		SLOPSINFO("Copy file source: '%S'", wstrSource.c_str()); 
		DWORD dwHandles = whoOps::ProcessPrincess::GetCurrentProcessHandleCount();
		SLOPSINFO("Copy file target: '%S' [%d]", wstrTarget.c_str(), dwHandles);
		this->setReadWrite(wstrTarget);
		BOOL boReturn = ::CopyFileExW(wstrSource.c_str(), wstrTarget.c_str(), NULL, NULL, NULL, 0); 
		if ( ! boReturn) {
			whoOps::ErroroneousMonk error(::GetLastError());
			std::string strError = error.formatMessage();
			std::string strWhat = cyclOps::StringEmUp::format("CopyFileExW() failed with error code %d: '%s'", 
				error.getLastError(),
				strError.c_str());
			_boSuccesful = false;
			if (_boContinueOnCopyFailure) {
				CYCLOPSERROR("%s", strWhat.c_str());
			} else {
				CYCLOPS_THROW_EXCEPTION_III(slOps::ExceptionUnableToCopyFile, strWhat);
			}
		}
	}

	void Synculator::copyFileIfNecessary(const wstring& wstrSourceDirectory, const wstring& wstrTargetDirectory, 
		const WIN32_FIND_DATA& findDataSource) 
	{ SLOPSDEBUG("Hello.");
		wstring wstrFileName(findDataSource.cFileName); SLOPSDEBUG("wstrFileName = %S", wstrFileName.c_str());
		wstringstream ssSourceFile;
		ssSourceFile << wstrSourceDirectory << "\\" << wstrFileName; SLOPSDEBUG("ssSourceFile = %S", ssSourceFile.str().c_str());
		wstringstream ssTargetFile; 
		wstring backslash = this->doesPathEndWithBackslash(wstrTargetDirectory) ? L"" : L"\\";
		ssTargetFile << wstrTargetDirectory << backslash << wstrFileName; SLOPSDEBUG("ssTargetFile = %S", ssTargetFile.str().c_str());
		if ( ! this->doFilesMatch(findDataSource, ssTargetFile.str())) {
			try {
				this->copyFile(ssSourceFile.str(), ssTargetFile.str());
			} catch (const slOps::ExceptionUnableToCopyFile& e) {
				this->_boSuccesful = false;
				SLOPSERROR("Failed to copy file %S to %s", ssSourceFile.str().c_str(), ssTargetFile.str().c_str());
				SLOPSERROR("%s [%s]", e.what(), typeid(e).name());
				if ( ! _boContinueOnCopyFailure) {
					throw;
				}
			}
		}
	}

	bool Synculator::doesPathEndWithBackslash(const wstring& path) {
		return cyclOps::StringEmUp::endsWithUsingEqualsAndReverseIteratorsW(path, L"\\");
	}

}
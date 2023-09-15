#pragma once

namespace cyclOps {
	class Pathiosocic
	{
	public:
		explicit Pathiosocic(const std::wstring& wstrPath);
		explicit Pathiosocic(const std::string& strPath);
		explicit Pathiosocic(void);
		void initialize(const std::wstring& wstrPath);
		~Pathiosocic(void);
		std::string getDrive() const;
		std::string getDir() const;
		std::string getFname() const;
		std::string getExt() const;
		std::string getPath() const;
		void getParent(std::wstring& wstrParent) const { wstrParent.assign(this->_wstrDrive + this->_wstrDir); }
		std::wstring getPathW() const;
		std::wstring getDriveW() const { return _wstrDrive; }
		std::wstring getDirW() const { return _wstrDir; }
		std::wstring getFnameW() const { return _wstrFname; }
		std::wstring getExtW() const { return _wstrExt; }
		std::string getPathExtendedLength(); 
		std::wstring getPathExtendedLengthW() const; 
		bool isExtendedLength() const;
		Pathiosocic getUNCFromLocalPath(std::string strServer);
		std::wstring getAbsolutePathW() const;
		std::string getAbsolutePath() const;
		bool isGreaterThanMAX_PATH() const;
		bool isRelative() const;
	private:
		std::wstring _wstrPath;
		std::wstring _wstrDrive;
		std::wstring _wstrDir;
		std::wstring _wstrFname;
		std::wstring _wstrExt;
	};
}

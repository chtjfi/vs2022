#pragma once
namespace whoOps {
	class LogicalDisk
	{
	public:

		LogicalDisk()
		{
		}

		~LogicalDisk()
		{
		}

		void setSizeInBytes(const std::wstring& wstrSize) { _wstrSizeInBytes.assign(wstrSize); }
		void setFreeSpaceInBytes(const std::wstring& wstr) { _wstrFreeSpaceInBytes.assign(wstr); }
		void setDeviceID(const std::wstring& wstr) { _wstrDeviceID.assign(wstr); }
		void setDriveType(unsigned int iDriveType) { _iDriveType = iDriveType; }
		unsigned int getDriveType(void) const { return _iDriveType; }
		std::string getDeviceID(void) const { return std::string(_wstrDeviceID.begin(), _wstrDeviceID.end()); }
		unsigned int getSizeInGB(void) const { 
			unsigned long long ulSize = std::stoll(_wstrSizeInBytes); 
			int iSize = (int) ulSize / (1024 * 1024 * 1024);
			return iSize;
		}
		std::string getSizeInBytes(void) const {
			return std::string(_wstrSizeInBytes.begin(), _wstrSizeInBytes.end());
		}
		std::string getFreeSpaceInBytes(void) const {
			return std::string(_wstrFreeSpaceInBytes.begin(), _wstrFreeSpaceInBytes.end());
		}

		int getFreeSpaceInGB(void) const { 
			unsigned long long ul = std::stoll(_wstrFreeSpaceInBytes); 
			int iSize = (int) ul / (1024 * 1024 * 1024);
			return iSize;
		}
		void setHostname(const std::wstring& hostname) {
			_wstrHostname.assign(hostname);
		}
		std::string getHostname(void) const { CYCLOPSDEBUG("Hello.");
			return std::string(_wstrHostname.begin(), _wstrHostname.end());
		}
		std::string toString(void) const { CYCLOPSDEBUG("Hello.");
			std::wstringstream wss;
			wss << _wstrHostname << L"\t" << _wstrDeviceID << L"\t" << this->getSizeInGB() << L"\t" << this->getFreeSpaceInGB(); CYCLOPSDEBUG("%S", wss.str().c_str());
			const std::wstring& wstr = wss.str();
			std::string s(wstr.begin(), wstr.end());
			return s;
		}
	private:
		unsigned int _iDriveType; 
		std::wstring _wstrHostname;
		std::wstring _wstrSizeInBytes;
		std::wstring _wstrFreeSpaceInBytes;
		std::wstring _wstrDeviceID;
	};

}

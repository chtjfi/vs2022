#pragma once

#include <Windows.h>
#include <string>

namespace whoOps {
	class RegistryRhinoceros
	{
	private:
		HKEY RegConnectRegistry(const std::string& strHostname);
		std::string RegQueryValueSZ(const HKEY& key, const std::string& strValue);
		DWORD RegQueryValueDWORD(const HKEY& key, const std::string& strValue);
		std::string RegQueryValueDWORDAsString(const HKEY& key, const std::string& strValue);
	public:
		RegistryRhinoceros(void);
		~RegistryRhinoceros(void);
		HKEY RegOpenKeyEx(const std::string& strHostname, const std::string& strSubKey, REGSAM access);
		void RegSetValue_REG_SZ(std::string strHostname, std::string strKey, std::string strValue, std::string strData);
		void RegSetValue_DWORD(std::string strHostname, std::string strKey, std::string strValue, DWORD dwData);
		std::string RegQueryValueAsString(const std::string& strHostname, const std::string& strKey, const std::string& strValue);
		DWORD RegQueryValueAsDWORD(const std::string& strHostname, const std::string& strKey, const std::string& strValue);
	};
}

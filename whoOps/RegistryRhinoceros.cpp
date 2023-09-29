#include "StdAfx.h"

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include "RegistryRhinoceros.h"

#include "..\cyclOps\cyclOps.h"
#include "whoOpsPrivate.h"
#include "ExceptionWhoOps.h"
#include "whoOpsBonanza.h"

using std::string;

whoOps::RegistryRhinoceros::RegistryRhinoceros(void)
{
}


whoOps::RegistryRhinoceros::~RegistryRhinoceros(void)
{
}


void whoOps::RegistryRhinoceros::RegSetValue_REG_SZ(std::string strHostname, std::string strKey, std::string strValue, 
	std::string strData) 
{
	HKEY hKey = this->RegOpenKeyEx(strHostname, strKey, KEY_ALL_ACCESS);
	::RegSetValueExA(hKey, strValue.c_str(), 0, REG_SZ, (CONST BYTE *) strData.c_str(), (DWORD) strData.length() + 1);
	::RegCloseKey(hKey);
}

void whoOps::RegistryRhinoceros::RegSetValue_DWORD(std::string strHostname, std::string strKey, std::string strValue, 
	DWORD dwData) 
{
	HKEY hKey = this->RegOpenKeyEx(strHostname, strKey,  KEY_ALL_ACCESS);
	LSTATUS status = ::RegSetValueExA(hKey, strValue.c_str(), 0, REG_DWORD, (CONST BYTE *) &dwData, (DWORD) sizeof(dwData));
	::RegCloseKey(hKey);
	if (status != ERROR_SUCCESS) {
		CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, (DWORD) status, cyclOps::Exception::TYPE_WIN32, "Error during RegSetValueExA().");
	}
}

string whoOps::RegistryRhinoceros::RegQueryValueAsString(const string& strHostname, const string& strKey, const string& strValue) {
	HKEY key = this->RegOpenKeyExW(strHostname, strKey, KEY_READ);
	/* https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911(v=vs.85).aspx */
	DWORD type;
	DWORD dwRet = RegQueryValueExA(key, strValue.c_str(), NULL, &type, NULL, NULL);
	switch (type) {
	case REG_SZ:		CYCLOPSDEBUG("REG_SZ returned.");
		return this->RegQueryValueSZ(key, strValue);
	case REG_DWORD:		CYCLOPSDEBUG("REG_DWORD returned.");
		return this->RegQueryValueDWORDAsString(key, strValue);
	default:
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Type returned by RegQueryValueEx() not recognized.");
	}
}


string whoOps::RegistryRhinoceros::RegQueryValueSZ(const HKEY& key, const string& strValue) {
	DWORD BufferSize = 8192;
	DWORD cbData = BufferSize;
	char* buffer = (char*) malloc(BufferSize);
	DWORD type;
	DWORD dwRet = RegQueryValueExA(key,	strValue.c_str(), NULL, &type, (LPBYTE) buffer, &cbData);
	while (dwRet == ERROR_MORE_DATA) { CYCLOPSDEBUG("More data.");
		BufferSize += 4096;
		buffer = (char*) realloc(buffer, BufferSize);
		cbData = BufferSize;
		dwRet = RegQueryValueExA(key, strValue.c_str(), NULL, &type, (LPBYTE) buffer, &cbData);
	}
	::RegCloseKey(key);
	if (dwRet != ERROR_SUCCESS) {
		CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, dwRet, cyclOps::Exception::TYPE_WIN32, "Error during RegQueryValueEx().");
	}
	return buffer;
}

DWORD whoOps::RegistryRhinoceros::RegQueryValueDWORD(const HKEY& key, const string& strValue) {
	DWORD BufferSize = 8192;
	DWORD cbData = BufferSize;
	DWORD buffer;
	DWORD type;
	DWORD dwRet = RegQueryValueExA(key, strValue.c_str(), NULL, &type, (LPBYTE) &buffer, &cbData);
	::RegCloseKey(key);
	if (dwRet != ERROR_SUCCESS) {
		CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, dwRet, cyclOps::Exception::TYPE_WIN32, "Error during RegQueryValueEx().");
	} else {
		return buffer;
	}
}

string whoOps::RegistryRhinoceros::RegQueryValueDWORDAsString(const HKEY& key, const string& strValue){
	DWORD value = this->RegQueryValueDWORD(key, strValue);
	return std::to_string(value);
}


HKEY whoOps::RegistryRhinoceros::RegConnectRegistry(const string& strHostname)
{
	HKEY hKey;
	LONG lReturn = ::RegConnectRegistryA(
		strHostname.c_str(), 
		HKEY_LOCAL_MACHINE, 
		&hKey);
	if (lReturn != ERROR_SUCCESS) {
		const char* szError = ::whoOpsErrorFormatMessage(lReturn);
		::WHOOPS_THROW_EXCEPTION("Error during RegConnectRegistryA() - '%s'", szError);
	}
	return hKey;
}


HKEY whoOps::RegistryRhinoceros::RegOpenKeyEx(const string& strHostname, const string& strSubKey, REGSAM access)
{
	HKEY hklmRemote = this->RegConnectRegistry(strHostname);
	HKEY hkeySubber;
	LONG lReturn = RegOpenKeyExA(hklmRemote, strSubKey.c_str(), 0, access, &hkeySubber);
	::RegCloseKey(hklmRemote);
	if (lReturn != ERROR_SUCCESS) {
		CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, (DWORD) lReturn, cyclOps::Exception::TYPE_WIN32,
			"Error during RegOpenKeyExA().");
	}
	return hkeySubber;
}

DWORD whoOps::RegistryRhinoceros::RegQueryValueAsDWORD(const std::string& strHostname, const std::string& strKey, const std::string& strValue) {
	HKEY key = this->RegOpenKeyEx(strHostname, strKey, KEY_READ);
	return this->RegQueryValueDWORD(key, strValue);
}

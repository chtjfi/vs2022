#include "StdAfx.h"

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include "ServiceInstalligator.h"
#include "..\slOps\slOps.h"
#include "..\cyclOps\Pathiosocic.h"
#include "whoOpsPrivate.h"
#include "ExceptionWhoOps.h"


whoOps::ServiceInstalligator::ServiceInstalligator(void)
{
}


whoOps::ServiceInstalligator::~ServiceInstalligator(void)
{
}

void whoOps::ServiceInstalligator::setServer(string strValue) { this->_strServer = strValue; }
void whoOps::ServiceInstalligator::setServiceName(string strValue) { this->_strServiceName = strValue; }
void whoOps::ServiceInstalligator::setDisplayName(string strValue) { this->_strDisplayName = strValue; }
void whoOps::ServiceInstalligator::setStartType(DWORD dwStartType) { this->_dwStartType = dwStartType; }
void whoOps::ServiceInstalligator::setBinaryPathName(string strValue) { this->_strBinaryPathName = strValue; }
void whoOps::ServiceInstalligator::setSourceDirectory(string strValue) { this->_strSourceDirectory = strValue; }
/* This here's the user. */
void whoOps::ServiceInstalligator::setServiceStartName(string strValue) { this->_strServiceStartName = strValue; } 
void whoOps::ServiceInstalligator::setPassword(string strValue) { this->_strPassword = strValue; }
void whoOps::ServiceInstalligator::install() { 
	this->copyFiles();
	this->CreateService();
}
void whoOps::ServiceInstalligator::copyFiles() {
	string strTargetDirectory = this->getTargetDirectory();
	::dllexportSlOpsCopyDirectory(_strSourceDirectory.c_str(), strTargetDirectory.c_str(), true, true, true, NULL);
}
void whoOps::ServiceInstalligator::CreateService() {
	SC_HANDLE hSCManager = OpenSCManagerA(_strServer.c_str(), NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		const char* szLastError = ::whoOpsErrorGetLastMessage();
		WHOOPS_THROW_EXCEPTION("%s", szLastError);
	}
	::CreateServiceA(
		hSCManager,
		_strServiceName.c_str(),
		_strDisplayName.c_str(),
		SC_MANAGER_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		_dwStartType,
		SERVICE_ERROR_NORMAL,
		_strBinaryPathName.c_str(),
		NULL,
		NULL,
		NULL,
		_strServiceStartName.c_str(),
		_strPassword.c_str());
	CloseServiceHandle(hSCManager);
}

string whoOps::ServiceInstalligator::getTargetDirectory() {
	cyclOps::Pathiosocic pathBinary(this->_strBinaryPathName);
	/* cyclOps::Pathiosocic pathTargetDirectory = pathBinary.getUNCFromLocalPath(this->_strServer);
	return pathTargetDirectory.getDir();  */
	return "xxx";
}
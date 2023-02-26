#pragma once

#include <string>
#include <Windows.h>

using std::string;

namespace whoOps {
	class ServiceInstalligator
	{
	public:
		ServiceInstalligator(void);
		~ServiceInstalligator(void);
		void setServer(string strValue);
		void setServiceName(string strValue);
		void setDisplayName(string strValue);
		void setStartType(DWORD dwStartType);
		void setBinaryPathName(string strValue);
		void setSourceDirectory(string strValue);
		void setServiceStartName(string strValue);
		void setPassword(string strValue);
		void install();
	private:
		void copyFiles();
		void CreateService();
		string getTargetDirectory();
		string _strServer;
		string _strServiceName;
		string _strDisplayName;
		DWORD _dwStartType;
		string _strBinaryPathName;
		string _strSourceDirectory;
		string _strServiceStartName; /* This here's the user. */
		string _strPassword;
	};
}
 
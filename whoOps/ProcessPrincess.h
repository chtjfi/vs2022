#pragma once

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include <Windows.h>
#include <string>

#include "../cyclOps/Logger.h"
#include "../cyclOps/JNIRequest.h"
#include "../cyclOps/JNIResponse.h"

using std::string;
using cyclOps::JNIResponse;
using cyclOps::JNIRequest;

namespace whoOps {
	class ProcessPrincess {
		private:
		public:
			ProcessPrincess(void) { };
			~ProcessPrincess(void) { };
			static DWORD CreateProcess(char* szCommandLine, bool boWait = true);
			static DWORD CreateProcessII(const string& strCommandLine, cyclOps::Logger* pLogger = NULL);
			static DWORD CreateProcessII(char* szCommandLine, cyclOps::Logger* pLogger = NULL);
			JNIResponse CreateProcessIII(const JNIRequest& jniRequest);
			void CreateProcessQnD(const string& strCommandLine);
			static DWORD GetCurrentProcessHandleCount(void);
	};
}

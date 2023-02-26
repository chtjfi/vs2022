#include "StdAfx.h"

#include "..\cyclOps\cyclOps.h"

#include "ExceptionUnsupportedSessionType.h"
#include "ErroroneousMonk.h"
#include "ExceptionNetSession.h"

#include "SessionSuckah.h"

using std::wstring;
using std::stringstream;

namespace whoOps {
	SessionSuckah::SessionSuckah(const string& strHost) : _strHost(strHost)
	{
	}


	SessionSuckah::~SessionSuckah(void)
	{
	}

	void SessionSuckah::NetSessionDel(const string& strTarget, WHOOPS_SESSION_TYPE sessionType) {
		if (sessionType == WHOOPS_CLIENT) {
			wchar_t wszHost[1000];
			CYCLOPS_SNWPRINTF_S(wszHost, L"%S", _strHost.c_str());
			wchar_t wszClient[1000];
			CYCLOPS_SNWPRINTF_S(wszClient, L"%S", strTarget.c_str()); printf("About to NetSessionDel()\n");
			NET_API_STATUS status = ::NetSessionDel(wszHost, wszClient, NULL);
			if (status != NERR_Success) {
				string strError = this->getReturnString(status);
				printf("strError = %s\n", strError.c_str());
				stringstream ss;
				ss << "NetSessionDel() failed with error " << strError ;
				CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionNetSession, ss.str().c_str());
			} else {
				printf("Ok.\n");
			}
		} else {
			CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionUnsupportedSessionType, "The supplied session type is not supported.");
		}
	}
	string SessionSuckah::getReturnString(const NET_API_STATUS& status) {
		switch (status) {
		case NERR_Success:
			return "NERR_Success";
		case ERROR_ACCESS_DENIED:
			return "ERROR_ACCESS_DENIED";
		case ERROR_INVALID_PARAMETER:
			return "ERROR_INVALID_PARAMETER";
		case ERROR_NOT_ENOUGH_MEMORY:
			return "ERROR_NOT_ENOUGH_MEMORY";
		case NERR_ClientNameNotFound:
			return "NERR_ClientNameNotFound";
		default:
			return "Error not found.";
		}
	}
}
#include "StdAfx.h"

#include <WinSock2.h>
#include <comdef.h>

#include "..\cyclOps\cyclOps.h"

#include "ErroroneousMonk.h"

using std::string;

namespace whoOps {
	ErroroneousMonk::ErroroneousMonk(DWORD dwLastError) : _dwLastError(dwLastError)
	{
	   char* szMessage = NULL;
	   DWORD dwFormatMessage = ::FormatMessageA(
		   FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
		   NULL, 
		   _dwLastError, 
		   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		   reinterpret_cast<char*>(&szMessage), 
		   0, NULL); CYCLOPSDEBUG("szMessage = %s", szMessage);
	   if ((dwFormatMessage > 0) && szMessage != NULL) {
			_strMessage = szMessage;
	   } else {
		  _strMessage = "Unknown error.";
	   }

	}


	ErroroneousMonk::~ErroroneousMonk(void)
	{
	}

	HRESULT ErroroneousMonk::HRESULT_IS_WIN32_ERROR = 0x80070000;

	bool ErroroneousMonk::isFileNotFound(HRESULT hr) {
		return hr == (ERROR_FILE_NOT_FOUND + ErroroneousMonk::HRESULT_IS_WIN32_ERROR);
	}

	string ErroroneousMonk::formatMessage() { 
		return _strMessage;
	}

	string ErroroneousMonk::hresultToString(HRESULT hr) { CYCLOPSDEBUG("Hello.");
		_com_error err(hr); 
		std::wstring wstrError(err.ErrorMessage()); CYCLOPSDEBUG("wstrError = %S", wstrError.c_str());
		std::string strError(wstrError.begin(), wstrError.end());
		return strError;
	}

}
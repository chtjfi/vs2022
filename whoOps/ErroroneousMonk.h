#pragma once
#include <string>
#include <windows.h>

namespace whoOps {
	class ErroroneousMonk
	{
	public:
		explicit ErroroneousMonk(DWORD dwLastError);
		~ErroroneousMonk(void);
		std::string formatMessage();
		DWORD getLastError() const { return _dwLastError; }
		static std::string hresultToString(HRESULT hr);
		/* When the HRESULT is a Win32 error, it will be in the 0x80070000 range. */
		static HRESULT HRESULT_IS_WIN32_ERROR;
		static bool isFileNotFound(HRESULT hr);
	private:
		DWORD _dwLastError;
		std::string _strMessage;
	};
}
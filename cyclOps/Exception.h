#ifndef _CYCLOPS_EXCEPTION_H_
#define _CYCLOPS_EXCEPTION_H_

#include <exception>
#include <string>
#include <sstream>

#include <Windows.h>
#include <comdef.h>

/* Example of a constructor for a subclass: 
		ExceptionInvalidTimeString
			(const std::string& strMessage, const std::string& strFile, const std::string& strFunction, int iLine, DWORD dwLastError = 0, cyclOps::Exception::TYPE type = cyclOps::Exception::TYPE_UNKNOWN)
			: cyclOps::Exception(strMessage, strFile, strFunction, iLine, dwLastError, type) 
			{ } 
*/


namespace cyclOps {
	class Exception : public std::exception {
	public:
		enum TYPE { TYPE_HRESULT, TYPE_WIN32, TYPE_UNKNOWN};
		Exception(
			const std::string& strMessage, 
			const std::string& strFile, 
			const std::string& strFunction, 
			int iLine, 
			long dwLastError = 0, 
			cyclOps::Exception::TYPE type = cyclOps::Exception::TYPE_UNKNOWN) 
		: 
			_strMessage(strMessage), _strFile(strFile), _strFunction(strFunction), _iLine(iLine), 
			_dwLastError(dwLastError) , _type(type)
		{
			std::stringstream ss;
			ss << _strMessage << " [ORIGIN: " << _strFile << " " << _strFunction << "(" << _iLine << ")]"; 
			this->_strWhat = ss.str(); /* printf("_strWhat = %s", _strWhat.c_str()); */
		};
		
		~Exception(void) { };
		/* throw() means nothing will be thrown: http://www.gotw.ca/publications/mill22.htm */
		virtual const char* what() const throw() { 
			return _strWhat.c_str();  
		}
		virtual const DWORD getLastError() const  throw() { 
			return _dwLastError;
		}
		virtual const std::string formatMessage() const throw() {
			if (_type == TYPE_WIN32) { 
				char* szMessage = NULL;
				DWORD dwFormatMessage = ::FormatMessageA(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
					NULL, 
					_dwLastError, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
					reinterpret_cast<char*>(&szMessage), 
					0, NULL);
				if ((dwFormatMessage > 0) && szMessage != NULL) {
					return szMessage;
				} else {
					return "Unknown error.";
				}
			} else if (_type == TYPE_HRESULT) {
				_com_error err(this->_dwLastError); 
				std::wstring wstrError(err.ErrorMessage());
				std::string strError(wstrError.begin(), wstrError.end());
				return strError;
			} else {
				return "Error is not a Win32 or HRESULT type.";
			}
		}
		virtual const Exception::TYPE getType(void) const throw() { 
			return _type;
		}
		/* getMessage() has no file and line info.  what() does.  */
		virtual const std::string getMessage(void) const throw() { return _strMessage; }
	private:
		std::string _strMessage;
		std::string _strWhat; 
		std::string _strFile;
		std::string _strFunction;
		int _iLine;
		unsigned long _dwLastError;
		Exception::TYPE _type;
	};
}

#endif
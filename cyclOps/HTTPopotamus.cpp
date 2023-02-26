#include "StdAfx.h"

#include <sstream>
#include <string>
#include <iomanip>
#include <map>

#include <WinSock2.h>
#include <Windows.h>
#include <winhttp.h>


#include "cyclOps.h"

#include "HTTPopotamus.h"

using std::stringstream;
using std::string;
using std::map;

namespace cyclOps {
	typedef struct {
		DWORD dwError;
		char* szError;
	} HTTPOPOTAMUS_ERROR;

	HTTPOPOTAMUS_ERROR httpErrorTable[] = { 
		{ERROR_WINHTTP_OUT_OF_HANDLES, "ERROR_WINHTTP_OUT_OF_HANDLES"},
		{ERROR_WINHTTP_TIMEOUT, "ERROR_WINHTTP_TIMEOUT"},
		{ERROR_WINHTTP_INTERNAL_ERROR, "ERROR_WINHTTP_INTERNAL_ERROR"},
		{ERROR_WINHTTP_INVALID_URL, "ERROR_WINHTTP_INVALID_URL"},
		{ERROR_WINHTTP_UNRECOGNIZED_SCHEME, "ERROR_WINHTTP_UNRECOGNIZED_SCHEME"},
		{ERROR_WINHTTP_NAME_NOT_RESOLVED, "ERROR_WINHTTP_NAME_NOT_RESOLVED"},
		{ERROR_WINHTTP_INVALID_OPTION, "ERROR_WINHTTP_INVALID_OPTION"},
		{ERROR_WINHTTP_OPTION_NOT_SETTABLE, "ERROR_WINHTTP_OPTION_NOT_SETTABLE"},
		{ERROR_WINHTTP_SHUTDOWN, "ERROR_WINHTTP_SHUTDOWN"},
		{ERROR_WINHTTP_LOGIN_FAILURE, "ERROR_WINHTTP_LOGIN_FAILURE"},
		{ERROR_WINHTTP_OPERATION_CANCELLED, "ERROR_WINHTTP_OPERATION_CANCELLED"},
		{ERROR_WINHTTP_INCORRECT_HANDLE_TYPE, "ERROR_WINHTTP_INCORRECT_HANDLE_TYPE"},
		{ERROR_WINHTTP_INCORRECT_HANDLE_STATE, "ERROR_WINHTTP_INCORRECT_HANDLE_STATE"},
		{ERROR_WINHTTP_CANNOT_CONNECT, "ERROR_WINHTTP_CANNOT_CONNECT"},
		{ERROR_WINHTTP_CONNECTION_ERROR, "ERROR_WINHTTP_CONNECTION_ERROR"},
		{ERROR_WINHTTP_RESEND_REQUEST, "ERROR_WINHTTP_RESEND_REQUEST"},
		{ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED, "ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED"},
		{ERROR_WINHTTP_CANNOT_CALL_BEFORE_OPEN, "ERROR_WINHTTP_CANNOT_CALL_BEFORE_OPEN"},
		{ERROR_WINHTTP_CANNOT_CALL_BEFORE_SEND, "ERROR_WINHTTP_CANNOT_CALL_BEFORE_SEND"},
		{ERROR_WINHTTP_CANNOT_CALL_AFTER_SEND, "ERROR_WINHTTP_CANNOT_CALL_AFTER_SEND"},
		{ERROR_WINHTTP_CANNOT_CALL_AFTER_OPEN, "ERROR_WINHTTP_CANNOT_CALL_AFTER_OPEN"},
		{ERROR_WINHTTP_HEADER_NOT_FOUND, "ERROR_WINHTTP_HEADER_NOT_FOUND"},
		{ERROR_WINHTTP_INVALID_SERVER_RESPONSE, "ERROR_WINHTTP_INVALID_SERVER_RESPONSE"},
		{ERROR_WINHTTP_INVALID_HEADER, "ERROR_WINHTTP_INVALID_HEADER"},
		{ERROR_WINHTTP_INVALID_QUERY_REQUEST, "ERROR_WINHTTP_INVALID_QUERY_REQUEST"},
		{ERROR_WINHTTP_HEADER_ALREADY_EXISTS, "ERROR_WINHTTP_HEADER_ALREADY_EXISTS"},
		{ERROR_WINHTTP_REDIRECT_FAILED, "ERROR_WINHTTP_REDIRECT_FAILED"},
		{ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR, "ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR"},
		{ERROR_WINHTTP_BAD_AUTO_PROXY_SCRIPT, "ERROR_WINHTTP_BAD_AUTO_PROXY_SCRIPT"},
		{ERROR_WINHTTP_UNABLE_TO_DOWNLOAD_SCRIPT, "ERROR_WINHTTP_UNABLE_TO_DOWNLOAD_SCRIPT"},
		{ERROR_WINHTTP_NOT_INITIALIZED, "ERROR_WINHTTP_NOT_INITIALIZED"},
		{ERROR_WINHTTP_SECURE_FAILURE, "ERROR_WINHTTP_SECURE_FAILURE"},
		{ERROR_WINHTTP_SECURE_CERT_DATE_INVALID, "ERROR_WINHTTP_SECURE_CERT_DATE_INVALID"},
		{ERROR_WINHTTP_SECURE_CERT_CN_INVALID, "ERROR_WINHTTP_SECURE_CERT_CN_INVALID"},
		{ERROR_WINHTTP_SECURE_INVALID_CA, "ERROR_WINHTTP_SECURE_INVALID_CA"},
		{ERROR_WINHTTP_SECURE_CERT_REV_FAILED, "ERROR_WINHTTP_SECURE_CERT_REV_FAILED"},
		{ERROR_WINHTTP_SECURE_CHANNEL_ERROR, "ERROR_WINHTTP_SECURE_CHANNEL_ERROR"},
		{ERROR_WINHTTP_SECURE_INVALID_CERT, "ERROR_WINHTTP_SECURE_INVALID_CERT"},
		{ERROR_WINHTTP_SECURE_CERT_REVOKED, "ERROR_WINHTTP_SECURE_CERT_REVOKED"},
		{ERROR_WINHTTP_SECURE_CERT_WRONG_USAGE, "ERROR_WINHTTP_SECURE_CERT_WRONG_USAGE"},
		{ERROR_WINHTTP_AUTODETECTION_FAILED, "ERROR_WINHTTP_AUTODETECTION_FAILED"},
		{ERROR_WINHTTP_HEADER_COUNT_EXCEEDED, "ERROR_WINHTTP_HEADER_COUNT_EXCEEDED"},
		{ERROR_WINHTTP_HEADER_SIZE_OVERFLOW, "ERROR_WINHTTP_HEADER_SIZE_OVERFLOW"},
		{ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW, "ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW"},
		{ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW, "ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW"},
		{ERROR_WINHTTP_CLIENT_CERT_NO_PRIVATE_KEY, "ERROR_WINHTTP_CLIENT_CERT_NO_PRIVATE_KEY"},
		{ERROR_WINHTTP_CLIENT_CERT_NO_ACCESS_PRIVATE_KEY, "ERROR_WINHTTP_CLIENT_CERT_NO_ACCESS_PRIVATE_KEY"},
		{ERROR_NOT_ENOUGH_MEMORY, "ERROR_NOT_ENOUGH_MEMORY"},
		{ERROR_INVALID_PARAMETER, "ERROR_INVALID_PARAMETER"}
	};

	HTTPopotamus::HTTPopotamus(void) : _iPort(80), _boIsTimeoutSpecified(false) { }
	HTTPopotamus::~HTTPopotamus(void) { }

	void HTTPopotamus::setProxy(const string& proxy) { CYCLOPSDEBUG("proxy = %s", proxy.c_str());
		_wstrProxy.assign(proxy.begin(), proxy.end()); CYCLOPSDEBUG("_wstrProxy = %S", _wstrProxy.c_str());
	}

	boolean HTTPopotamus::isHTTPStatusCodeSuccess() { CYCLOPSVAR(_dwStatusCode, "%d");
		return (_dwStatusCode >= 200) && (_dwStatusCode < 300); 
	}


	void HTTPopotamus::setHostname(const string& strHostname) {
		_wstrHostname.assign(strHostname.begin(), strHostname.end()); CYCLOPSDEBUG("_wstrHostname = '%S'", _wstrHostname.c_str());
	}
	void HTTPopotamus::setPort(const int iPort) { 
		_iPort = iPort; CYCLOPSDEBUG("iPort = %d", _iPort);
	}
	void HTTPopotamus::setPort(const string& strPort) {
		this->setPort(std::stoi(strPort));
	}
	void HTTPopotamus::setQuery(const string& strQuery) {
		_wstrQuery.assign(strQuery.begin(), strQuery.end()); CYCLOPSDEBUG("_wstrQuery = '%S'", _wstrQuery.c_str());
	}

	string HTTPopotamus::GET(void) { CYCLOPSDEBUG("Hello.");
		string strOutput;
		this->GET(strOutput);
		return strOutput;
	}

	void HTTPopotamus::GET(string& strOutput) { CYCLOPSDEBUG("Hello.");
		/* TODO:  Replace with http://codereview.stackexchange.com/questions/29621/dealing-with-resource-closure-when-rethrowing-an-exception/29627?noredirect=1#29627 */
	  HINTERNET  hSession = NULL, 
		  hConnect = NULL,
		  hRequest = NULL;
		try {
			hSession = this->open();
			if (_boIsTimeoutSpecified) { this->setTimeoutsPrivate(hSession); }
			hConnect = this->connect(hSession);
			hRequest = this->openRequest(hConnect);
			this->sendRequest(hRequest);
			this->receiveResponse(hRequest);
			this->queryStatusCode(hRequest);
			this->readData(hRequest, strOutput);
			if( hRequest ) WinHttpCloseHandle( hRequest );
			if( hConnect ) WinHttpCloseHandle( hConnect );
			if( hSession ) WinHttpCloseHandle( hSession );
		} catch (const cyclOps::ExceptionHTTPopotamus& e) {
			CYCLOPSERROR("Exception caught - '%s' '%s'", typeid(e).name(), e.what());
			if( hRequest ) WinHttpCloseHandle( hRequest );
			if( hConnect ) WinHttpCloseHandle( hConnect );
			if( hSession ) WinHttpCloseHandle( hSession );
			throw;
		}

	}

	void HTTPopotamus::queryStatusCode(HINTERNET hRequest) {
		DWORD dwSize = sizeof(_dwStatusCode);
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE| WINHTTP_QUERY_FLAG_NUMBER, 
			NULL, &_dwStatusCode, &dwSize, NULL ); CYCLOPSVAR(_dwStatusCode, "%d");
		if (_dwStatusCode >= 400) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionHTTPStatusCodeNotSuccess, "The HTTP status code was %d", _dwStatusCode);
		}
	}

	char* HTTPopotamus::getErrorMessage(const DWORD dwError) {
		/* TODO:  This should be replaced with http://stackoverflow.com/questions/2159458/why-is-formatmessage-failing-to-find-a-message-for-wininet-errors/2159488#2159488 */
		size_t iSize = sizeof(httpErrorTable) / sizeof(httpErrorTable[0]);
		for (size_t i = 0; i < iSize; ++i) {
			if (dwError == httpErrorTable[i].dwError) {
				return httpErrorTable[i].szError;
			}
		}
		return "Error message not found.";
	}

	HINTERNET HTTPopotamus::open(void) { CYCLOPSDEBUG("_wstrProxy = %S", _wstrProxy.c_str());
		bool isProxy = ! cyclOps::StringEmUp::isEmpty(_wstrProxy); CYCLOPSDEBUG("isProxy = %d", isProxy);
		DWORD accessType = isProxy ? WINHTTP_ACCESS_TYPE_NAMED_PROXY : WINHTTP_ACCESS_TYPE_NO_PROXY ;
		LPCWSTR pwszProxyName = isProxy ? _wstrProxy.c_str() : WINHTTP_NO_PROXY_NAME; CYCLOPSDEBUG("pwszProxyName = %s", pwszProxyName);
		HINTERNET hSession = WinHttpOpen( L"WinHTTP Example/1.0",  
                          accessType,
                          pwszProxyName, 
                          WINHTTP_NO_PROXY_BYPASS, 0 );
		if (!hSession) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			stringstream ss;
			ss << "Error during WinHttpOpen() - '" << szMessage << "'";
			throw cyclOps::ExceptionHTTPopotamus(ss.str(), __FILE__, __FUNCTION__, __LINE__);
		} 
		return hSession;
	}

	void HTTPopotamus::setTimeoutsPrivate(HINTERNET hSession) {
		BOOL boSuccess = WinHttpSetTimeouts(hSession, 
			0, /* Resolve timeout is infinite. */
			60000, /* Connect timeout is the default of 60 seconds. */
			30000, /* Send timeout is the default of 30 seconds. */
			_iReceiveTimeoutMilliseconds); /* Receive timeout is set by caller. */
		if (!boSuccess) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionHTTPopotamus, "Exception setting timeouts.  [%s]", szMessage);
		}
	}

	HINTERNET HTTPopotamus::connect(HINTERNET hSession) {
		HINTERNET hConnect = WinHttpConnect( hSession, _wstrHostname.c_str(), _iPort, 0 );
		if( !hConnect ) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			stringstream ss;
			ss << "Error during WinHttpConnect() - '" << szMessage << "'";
			throw cyclOps::ExceptionHTTPopotamus(ss.str(), __FILE__, __FUNCTION__, __LINE__);
		}
		return hConnect;
	}

	HINTERNET HTTPopotamus::openRequest(HINTERNET hConnect) { CYCLOPSDEBUG("_wstrQuery = %S", _wstrQuery.c_str());
		HINTERNET hRequest = WinHttpOpenRequest( hConnect, L"GET", _wstrQuery.c_str(), NULL, 
			WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0 );

		if( !hRequest ) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			stringstream ss;
			ss << "Error during WinHttpOpenRequest() - '" << szMessage << "'";
			throw cyclOps::ExceptionHTTPopotamus(ss.str(),  __FILE__, __FUNCTION__, __LINE__);
		}
		/* 
		
		From http://microsoft.public.winhttp.narkive.com/wFnpeMUk/proxy-authentication-and-winhttpsetcredentials 

		If you want to use your Windows logon credentials to
		respond to NTLM or Negotiate (aka Integrated Windows
		Authentication) challenges, set both user name and
		password to NULL. For WinHttp 5.x, however, whether the
		logon credential will actually be used is subject to the
		WinHttp "auto-logon policy". If the policy is LOW, your
		logon credentials will be used to respond to
		NTLM/Negotiate challenges for all sites; If the policy is
		HIGH, your logon credentials will not be used for any
		site; and If the policy is MEDIUM (the default), then
		WinHttp will use your logon creds to authenticate
		with "Intranet" sites only.

		By default WinHttp considers any web site an Internet
		site unless you specify a proxy and the site you are
		visiting is explicitly being bypassed, via the proxy
		bypass list.

		For details on adjusting auto-logon policy, refer to the
		WinHttpSetOption/WINHTTP_OPTION_AUTOLOGON_POLICY
		documentation. For setting proxy and bypass list, refer
		to the WinHttpOpen and
		WinHttpSetOption/WINHTTP_OPTION_PROXY documentation.

		*/
		DWORD data = WINHTTP_AUTOLOGON_SECURITY_LEVEL_LOW;
		DWORD size = sizeof(DWORD); 
		WinHttpSetOption(hRequest, WINHTTP_OPTION_AUTOLOGON_POLICY, &data, size);
		WinHttpSetCredentials(hRequest, WINHTTP_AUTH_TARGET_SERVER, WINHTTP_AUTH_SCHEME_NTLM, NULL, NULL, NULL);
		return hRequest;
	}

	void HTTPopotamus::sendRequest(HINTERNET hRequest) {
		BOOL boResults = WinHttpSendRequest( hRequest,
                                   WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   WINHTTP_NO_REQUEST_DATA, 0, 
                                   0, 0 );
		if (!boResults) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			stringstream ss;
			ss << "Error during WinHttpSendRequest(): - '" <<
				szMessage << " - hostname = " << this->getHostname() << 
				" port = " << this->getPort() << " query = " << this->getQuery();
			throw cyclOps::ExceptionHTTPopotamus(ss.str(),  __FILE__, __FUNCTION__, __LINE__);
		}
	}
	void HTTPopotamus::receiveResponse(HINTERNET hRequest) {
		BOOL boResults = WinHttpReceiveResponse( hRequest, NULL );
		if (!boResults) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			stringstream ss;
			ss << "Error during WinHttpReceiveResponse() - " << szMessage;
			throw cyclOps::ExceptionHTTPopotamus(ss.str(),  __FILE__, __FUNCTION__, __LINE__);
		}
	}

	bool HTTPopotamus::isDownloadToFile() {
		return this->_strOutputFile.length() > 0;
	}

	void HTTPopotamus::readData(HINTERNET hRequest, string& strOutput) {
		DWORD dwSize = 0;
		int iIteration = 0;
		FILE * pFile = 0; 
		if (this->isDownloadToFile()) { CYCLOPSDEBUG("Downloading to file %s.", _strOutputFile.c_str());
			if ( fopen_s(&pFile, _strOutputFile.c_str(), "w+b") != 0) {
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionHTTPopotamus, "Failed to open %s", _strOutputFile.c_str());
			}
		}
		do  {
			CYCLOPSVAR(iIteration++, "%d");
			dwSize = this->queryDataAvailable(hRequest); CYCLOPSVAR(dwSize, "%d");
			LPSTR pszOutBuffer = new char[dwSize + 1];
			if( ! pszOutBuffer ) {
				throw cyclOps::ExceptionHTTPopotamus("Can't allocate memory for pszOutBuffer.",  __FILE__, __FUNCTION__, __LINE__);
			}
			ZeroMemory( pszOutBuffer, dwSize + 1);
			DWORD dwDownloaded = 0;
			if( ! WinHttpReadData( hRequest, (LPVOID) pszOutBuffer, dwSize, &dwDownloaded ) ) {
				DWORD dwLastError = ::GetLastError();
				char* szMessage = this->getErrorMessage(dwLastError);
				stringstream ss;
				ss << "Error during WinHttpReadData() - " << szMessage;
				throw cyclOps::ExceptionHTTPopotamus(ss.str(),  __FILE__, __FUNCTION__, __LINE__);
			} else { CYCLOPSDEBUG("pszOutBuffer = %s", pszOutBuffer);
				strOutput += pszOutBuffer; CYCLOPSDEBUG("strOutput = %s", strOutput.c_str());
				if (this->isDownloadToFile()) { CYCLOPSDEBUG("Writing to file.");
					fwrite(pszOutBuffer, (size_t) dwDownloaded, (size_t) 1, pFile); CYCLOPSDEBUG("File written to.");
				}
			}
			delete [] pszOutBuffer;
		} while (dwSize > 0); CYCLOPSDEBUG("strOutput = '%s'", strOutput.c_str());
	}

	DWORD HTTPopotamus::queryDataAvailable(HINTERNET hRequest) { CYCLOPSDEBUG("Hello.");
		DWORD dwSize = 0;
		if ( ! WinHttpQueryDataAvailable( hRequest, &dwSize )) {
			DWORD dwLastError = ::GetLastError();
			char* szMessage = this->getErrorMessage(dwLastError);
			stringstream ss;
			ss << "Error during WinHttpQueryDataAvailable() - " << szMessage;
			throw cyclOps::ExceptionHTTPopotamus(ss.str(),  __FILE__, __FUNCTION__, __LINE__);
		}
		return dwSize;
	}

	string HTTPopotamus::getHostname(void) {
		return string(_wstrHostname.begin(), _wstrHostname.end());
	}
	
	string HTTPopotamus::getQuery(void) {
		return string(_wstrQuery.begin(), _wstrQuery.end());
	}

	int HTTPopotamus::getPort(void) {
		return _iPort;
	}

	void HTTPopotamus::setOutputFile(const string& strFile) { 
		this->_strOutputFile = strFile;
	}
	void HTTPopotamus::setReceiveTimeout(int iMilliseconds) {
		this->_boIsTimeoutSpecified = true;
		this->_iReceiveTimeoutMilliseconds = iMilliseconds;
	}

	/* http://stackoverflow.com/a/17708801/502556 */
	void HTTPopotamus::encodeURL(const string& original, string& encoded) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;
		for (string::const_iterator i = original.begin(), n = original.end(); i != n; ++i) {
			string::value_type c = (*i);
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				// Keep alphanumeric and other accepted characters intact
				escaped << c;
			} else {
				// Any other characters are percent-encoded
				escaped << std::uppercase;
				escaped << '%' << std::setw(2) << int((unsigned char) c);
				escaped << std::nouppercase;
			}
		}
		encoded.assign(escaped.str());
	}

	void HTTPopotamus::addParametersFromMap(const map<string, string>& mapOfParameters) {
		string strQueryBeforeStarting(_wstrQuery.begin(), _wstrQuery.end());
		boolean boFirstTime = true;
		for (map<string, string>::const_iterator i = mapOfParameters.begin(); i != mapOfParameters.end(); ++i) {
			string secondEncoded;
			this->encodeURL(i->second, secondEncoded); 
			if (boFirstTime) {
				boFirstTime = false;
				if ( ! cyclOps::StringEmUp::endsWithUsingCompare(strQueryBeforeStarting, "?")) {
					strQueryBeforeStarting.append("&");
				}
				strQueryBeforeStarting.append(i->first + "=" + secondEncoded);
			} else {
				strQueryBeforeStarting.append("&" + i->first + "=" + secondEncoded);
			}
		}
		_wstrQuery.assign(strQueryBeforeStarting.begin(), strQueryBeforeStarting.end());
	}
}
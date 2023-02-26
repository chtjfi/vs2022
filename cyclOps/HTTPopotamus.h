#ifndef HTTPOPOTAMUS_H
#define HTTPOPOTAMUS_H

#include <Windows.h>
#include <winhttp.h>

#include "cyclOps.h"

namespace cyclOps {
	class HTTPopotamus {
	public:
		HTTPopotamus(void);
		~HTTPopotamus(void);
		void setHostname(const std::string&);
		void setPort(const int);
		void setPort(const std::string& strPort);
		void setQuery(const std::string&);
		void setOutputFile(const std::string&);
		void setReceiveTimeout(int iMilliseconds);
		void setProxy(const std::string& proxy);
		void GET(std::string& strOutput);
		/* GET(void) calls GET(const std::string&). */
		std::string GET(void);
		DWORD getStatusCode() { return _dwStatusCode; };
		boolean isHTTPStatusCodeSuccess();
		std::string getHostname(void);
		std::string getQuery(void);
		int getPort(void);
		static void encodeURL(const std::string& original, std::string& encoded);
		void addParametersFromMap(const std::map<std::string, std::string>& mapOfParameters);
	private:
		HINTERNET open(void);
		DWORD _dwStatusCode;
		/* TODO: These HINTERNET handles should be constant references in most cases? */
		HINTERNET connect(HINTERNET hSession);
		HINTERNET openRequest(HINTERNET hConnect);
		void sendRequest(HINTERNET hRequest);
		void readData(HINTERNET hRequest, std::string& strOutput);
		void receiveResponse(HINTERNET hRequest);
		void queryStatusCode(HINTERNET hRequest);
		DWORD queryDataAvailable(HINTERNET hRequest);
		void setTimeoutsPrivate(HINTERNET hSession);
		/* TODO:  This should be replaced with http://stackoverflow.com/questions/2159458/why-is-formatmessage-failing-to-find-a-message-for-wininet-errors/2159488#2159488 */
		char* getErrorMessage(const DWORD dwError);
		bool isDownloadToFile();
		std::wstring _wstrHostname;
		int _iPort;
		std::wstring  _wstrQuery;
		std::string _strOutputFile;
		bool _boIsTimeoutSpecified;
		int _iReceiveTimeoutMilliseconds;
		std::wstring _wstrProxy;
	};
}


#endif
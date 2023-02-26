#include "StdAfx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include "cyclOps.h"
#include "ExceptionNetworkNanny.h"
#include "NetworkNanny.h"

/* If you are getting an error like this:

'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings

...add the following to stdafx.h:

#define  _WINSOCK_DEPRECATED_NO_WARNINGS  */

using std::string;
using std::vector;

namespace cyclOps {
	NetworkNanny::NetworkNanny(void)
	{
	}


	NetworkNanny::~NetworkNanny(void)
	{
	}

	string NetworkNanny::getUserName(void) {
		char username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		::GetUserNameA(username, &username_len);
		return username;
	}

	bool NetworkNanny::ping(const std::string& strHost) {
		unsigned long ipaddr = inet_addr(strHost.c_str());
		if (ipaddr == INADDR_NONE) {
			string strAddress = this->getIPAddress(strHost);
			ipaddr = inet_addr(strAddress.c_str());
			if (ipaddr == INADDR_NONE) {
				return false;
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionUnableToResolveAddress, "%s is not an address or cannot be resolved.", strHost.c_str());
			}
		}
		HANDLE hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNetworkNanny, "Unable to open handle. IcmpCreatefile returned error: %ld\n", GetLastError());
	    }    
		char SendData[32] = "Data Buffer";
		DWORD ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
		LPVOID ReplyBuffer = (VOID*) malloc(ReplySize);
		if (ReplyBuffer == NULL) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNetworkNanny, "Unable to allocate memory.");
	    }	
	    DWORD dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
		if (dwRetVal != 0) {
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY) ReplyBuffer;
			struct in_addr ReplyAddr;
			ReplyAddr.S_un.S_addr = pEchoReply->Address; CYCLOPSDEBUG("Sent icmp message to %s", strHost.c_str());
			CYCLOPSDEBUG("Received %ld icmp message responses", dwRetVal);
			CYCLOPSDEBUG("Received from %s", inet_ntoa( ReplyAddr ) );
			CYCLOPSDEBUG("Status = %ld", pEchoReply->Status);
			CYCLOPSDEBUG("Roundtrip time = %ld milliseconds", pEchoReply->RoundTripTime);
			if (pEchoReply->Status == 0) {
				return true;
			} else {
				return false;
			}
		} else {
			CYCLOPSDEBUG("Call to IcmpSendEcho failed.");
			CYCLOPSDEBUG("IcmpSendEcho returned error: %ld", GetLastError() );
			return false;
		}
	}

	string NetworkNanny::getIPAddress() { CYCLOPSDEBUG("Hello.");
		char hostname[5000];
		//hostname[4999] = '\0';
		//int result = ::gethostname(hostname, 256); 
		DWORD size = sizeof(hostname) / sizeof(hostname[0]);
		boolean result = ::GetComputerNameA(hostname, &size);
		CYCLOPSDEBUG("result/hostname = %d/%s", result, hostname);
		return NetworkNanny::getIPAddress(hostname);
	}

	string NetworkNanny::getIPAddress(const string& strHost) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		hostent * record = gethostbyname(strHost.c_str());
		if (record == NULL) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionUnableToResolveAddress, "%s cannot be resolved.", 
				strHost.c_str());
		}
		in_addr * address = (in_addr * )record->h_addr;
		return inet_ntoa(* address);
	}

	vector<string> NetworkNanny::getIPAddresses(const string& strHost) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		hostent * record = gethostbyname(strHost.c_str());
		if (record == NULL) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionUnableToResolveAddress, "%s cannot be resolved.",
				strHost.c_str());
		}
		vector<string> addresses;
		for (int i = 0; record->h_addr_list[i] != NULL; ++i) { CYCLOPSDEBUG("i = %d", i);
			in_addr * address = (in_addr *) record->h_addr_list[i];
			addresses.push_back(inet_ntoa(*address));
		}
		return addresses;
	}


	string NetworkNanny::getFQDN(void) {
		/* http://stackoverflow.com/questions/504810/how-do-i-find-the-current-machines-full-hostname-in-c-hostname-and-domain-info */
		char hostname[5000];
		hostname[4999] = '\0';
		::gethostname(hostname, 1023);
		struct hostent* h;
		h = gethostbyname(hostname);
		return h->h_name;
	}
}
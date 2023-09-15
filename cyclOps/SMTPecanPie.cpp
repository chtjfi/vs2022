#include "StdAfx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "cyclOps.h"

#include "SMTPecanPie.h"

#define HELO "HELO 192.168.1.1\r\n" 
#define DATA "DATA\r\n" 
#define QUIT "QUIT\r\n" 
const char *host_id="smtp.eu.scor.local"; 
const char *from_id="certus@scor.com";
const char *to_id="jfitzpatrick@scor.com";
const char *sub="testmail";
const char wkstr[100]="hello\r\n"; //"http://dcvprdcertas.eu.scor.local:7003/compliance/apps/ControlServlet\r\n"; 

using std::string;
using std::vector;

namespace cyclOps {

	SMTPecanPie::SMTPecanPie(void)
	{
	}


	SMTPecanPie::~SMTPecanPie(void)
	{
	}

	void SMTPecanPie::setTo(const std::string& s, const char delimiter) {
		if (StringEmUp::containsIgnoreCase(s, string(1, delimiter))) {
			StringEmUp::split(s, delimiter, _vectorOfRecipients);
		} else {
			_vectorOfRecipients.push_back(s);
		}
	}

	void SMTPecanPie::send(void) { CYCLOPSDEBUG("Hello.");
		WSADATA wsaData;
		WORD wVer = MAKEWORD(2,2);    
		if (::WSAStartup(wVer,&wsaData) != NO_ERROR) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionSMTPecanPie, "::WSAStartup() failed.");
		} CYCLOPSDEBUG("::WSAStartup() succesful.");
	    SOCKET sock = ::socket(AF_INET, SOCK_STREAM, 0); 
		if (sock == -1) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionSMTPecanPie, "::socket() failed.");
		} CYCLOPSDEBUG("::socket() succesful.");
		struct sockaddr_in server; 
		server.sin_family = AF_INET; CYCLOPSDEBUG("_strServer = %s", _strServer.c_str());
		struct hostent *hp = ::gethostbyname(_strServer.c_str()); 
		if (hp == (struct hostent *) 0) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionSMTPecanPie, "::gethostbyname() failed, unknown host.");
	    } CYCLOPSDEBUG("::gethostbyname() succesful.");
	    memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length); 
		server.sin_port = htons(25); 
		if (::connect(sock, (struct sockaddr *) &server, sizeof server) == -1) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionSMTPecanPie, "::connect() failed.");
		} CYCLOPSDEBUG("::connect() succesful.");
		this->haveConversation(sock);
		::closesocket(sock); 
	}

	void SMTPecanPie::haveConversation(const SOCKET& sock) {
		readSocket(sock); /* SMTP Server logon string */ 
		string helo = "HELO " + cyclOps::NetworkNanny::getFQDN() + "\r\n"; CYCLOPSDEBUG("helo = %s", helo.c_str());
		sendSocket(sock, helo.c_str()); /* introduce ourselves */ 
		readSocket(sock); /*Read reply */ 
		sendSocket(sock, "MAIL FROM: ");  
		sendSocket(sock, _strFrom.c_str()); 
		sendSocket(sock, "\r\n"); 
		readSocket(sock); /* Sender OK */ 
		sendSocket(sock, "VRFY "); 
		sendSocket(sock, _strFrom.c_str()); 
		sendSocket(sock, "\r\n"); 
		readSocket(sock); // Sender OK */ 
		// for (int i = 0; i < _vectorOfRecipients.size(); ++i) {
			sendSocket(sock, "RCPT TO: "); /*Mail to*/ 
			sendSocket(sock, "jfitzpatrick@scor.com" ); //_vectorOfRecipients[i].c_str()); 
			sendSocket(sock, "\r\n"); 
			readSocket(sock); // Recipient OK*/ 
		// }
		sendSocket(sock, "DATA\r\n");// body to follow*/ 
		/* From http://www.earthinfo.org/example-smtp-conversation/
		   MAIL Data Sections
		   Subject:
		   Cc:
		   Reply-To: Email header lines are not SMTP commands per se. They are sent in the DATA stream for a message. 
		   Header lines appear on a line by themselves, and are seperated from the body of a message by a blank line.
		*/
		sendSocket(sock, "Subject: "); 
		string strSub = _strSubject + "\r\n\r\n";
		sendSocket(sock, strSub.c_str()); 
		readSocket(sock); // Recipient OK*/ 
		sendSocket(sock, _strMessage.c_str()); 
		sendSocket(sock, "\r\n.\r\n"); 
		readSocket(sock);  
		sendSocket(sock, "QUIT\r\n"); /* quit */ 
		readSocket(sock); // log off */ 
	}

	void SMTPecanPie::readSocket(const SOCKET& sock) {
		char buf[BUFSIZ + 1]; 
        int len = recv(sock, buf, BUFSIZ, 0); 
        ::send(1, buf, len, 0); 
		char szOutput[BUFSIZ + 1];
		_snprintf_s(szOutput, BUFSIZ + 1, len, "%s", buf); CYCLOPSDEBUG("szOutput = %s", szOutput);
    } 

	void SMTPecanPie::sendSocket(const SOCKET& sock, const char* s) { CYCLOPSDEBUG("s = %s", s);
        ::send(sock, s, (int) strlen(s), 0); 
        ::send(1, s, (int) strlen(s), 0); CYCLOPSDEBUG("Client: %s",s); 
	}

}
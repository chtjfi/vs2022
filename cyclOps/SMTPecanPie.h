#pragma once

#include <string>
#include <vector>

#include <WinSock2.h>

namespace cyclOps {
	class SMTPecanPie
	{
	public:
		explicit SMTPecanPie(void);
		~SMTPecanPie(void);
		void setTo(const std::string& s, const char delimiter);
		void setFrom(const std::string& s) { _strFrom.assign(s); }
		void setSubject(const std::string& s) { _strSubject.assign(s); }
		void setMessage(const std::string& s) { _strMessage.assign(s); }
		void setServer(const std::string& s) { _strServer.assign(s); }
		void send(void);
	private:
		SMTPecanPie(const SMTPecanPie&);
		SMTPecanPie& operator=(const SMTPecanPie&);
		std::vector<std::string> _vectorOfRecipients;
		std::string _strFrom;
		std::string _strSubject;
		std::string _strMessage;
		std::string _strServer;
		void readSocket(const SOCKET& socket);
		void sendSocket(const SOCKET& sock, const char* s);
		void haveConversation(const SOCKET& sock);
		void haveConversationII();
	};
}
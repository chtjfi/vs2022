#pragma once

#include <string>
#include <vector>

namespace cyclOps {
	class Automail
	{
	public:
		Automail();
		~Automail();
		void setAutomailShare(const std::string& share) { _automailShare = share; }
		void addRecipient(const std::string& recipient) { _recipients.push_back(recipient); }
		void addRecipients(const std::vector<std::string>& recipients);
		void setExecutableOptionally(const std::string& executable) { _executable = executable; }
		void setSubject(const std::string& subject) { _subject = subject; }
		void setMessage(const std::string& message) { _message = message; }
		void addAttachment(const std::string& attachment) { _attachments.push_back(attachment); }
		void send(void);
	private:
		std::string getRecipients();
		std::string _automailShare;
		std::string _executable;
		std::vector<std::string> _recipients;
		std::string _from;
		std::string _subject;
		std::string _message;
		std::vector<std::string> _attachments;
	};
}

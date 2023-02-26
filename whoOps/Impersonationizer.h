#pragma once
#include <string>
namespace whoOps {
	class Impersonationizer
	{
	public:
		explicit Impersonationizer(const std::string& user, const std::string& domain, const std::string& password);
		~Impersonationizer(void);
		void impersonate(void);
		void revert(void);
	private:
		Impersonationizer(const Impersonationizer&);
		Impersonationizer& operator=(const Impersonationizer&);
		std::string _user;
		std::string _domain;
		std::string _password;
		HANDLE _hToken;
	};

}
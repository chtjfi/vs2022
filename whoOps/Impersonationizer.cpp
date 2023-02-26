#include "StdAfx.h"

#include "..\cyclOps\cyclOps.h"
#include "ExceptionImpersonationizer.h"

#include "Impersonationizer.h"

using std::wstring;
using std::string;

namespace whoOps {

	Impersonationizer::Impersonationizer(const string& user, const string& domain, const string& password)
		: _user(user), _domain(domain), _password(password), _hToken(0)
	{
	}

	Impersonationizer::~Impersonationizer(void)
	{
		if (_hToken) 
		{ 
			CloseHandle(_hToken); 
			_hToken = 0; 
		} 
	}

	void Impersonationizer::impersonate(void) {
		wstring wstrUser(_user.begin(), _user.end());
		wstring wstrDomain(_domain.begin(), _domain.end());
		wstring wstrPassword(_password.begin(), _password.end());
		if ( ! ::LogonUser(wstrUser.c_str(), wstrDomain.c_str(), wstrPassword.c_str(), 
			LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &_hToken)) 
		{
			DWORD dwLastError = ::GetLastError();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionImpersonationizer, dwLastError, cyclOps::Exception::TYPE_WIN32, "::LogonUser() failed for %S/%S/%S.", wstrUser.c_str(), wstrDomain.c_str(), wstrPassword.c_str());
		}
		if ( ! ::ImpersonateLoggedOnUser(_hToken))
		{
			DWORD dwLastError = ::GetLastError();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionImpersonationizer, dwLastError, cyclOps::Exception::TYPE_WIN32, "::ImpersonatLoggedOnUser() failed for %S/%S/%S.", wstrUser.c_str(), wstrDomain.c_str(), wstrPassword.c_str());
		}
	}

	void Impersonationizer::revert(void) {
		if ( ! ::RevertToSelf()) {
			DWORD dwLastError = ::GetLastError();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionImpersonationizer, dwLastError, cyclOps::Exception::TYPE_WIN32, "::RevertToSelf() failed.");
		}
	}

}
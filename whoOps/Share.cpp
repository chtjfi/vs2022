#include "StdAfx.h"

#include <algorithm>

#include "..\cyclOps\cyclOps.h"
#include "..\cyclOps\StringEmUp.h"

#include "Share.h"
#include "ExceptionNullSecurityDescriptor.h"

using std::wstring;
using std::string;
using std::vector;
using cyclOps::StringEmUp;

namespace whoOps {
	Share::Share(void) : _boIsNull(true)
	{
	}

	Share::~Share(void)
	{
	}

	void Share::setName(const wstring& wstrName) {
		_strName.assign(wstrName.begin(), wstrName.end());
	}

	void Share::setRemark(const wstring& wstr) {
		_strRemark.assign(wstr.begin(), wstr.end());
	}

	void Share::setPath(const wstring& wstr) {
		_strPath.assign(wstr.begin(), wstr.end());
	}

	const vector<ShareACE>& Share::getACEVector(void) const {  CYCLOPSDEBUG("Hello.");
		if (this->_boIsNull) { CYCLOPSDEBUG("Security descriptor is NULL.");
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionNullSecurityDescriptor, "The security descriptor for \\\\%s\\%s is null.  This means that everyone has unrestricted access to the server.", this->_strServer, this->_strName);
		} else { CYCLOPSDEBUG("Security descriptor is NOT null.");
			return _aceVector; 
		}
	}

	wstring Share::getUNCW() const {
		string strUNC = this->getUNCA();
		wstring wstrUNC(strUNC.begin(), strUNC.end());
		return wstrUNC;
	}

	string Share::getUNCA() const {
		return "\\\\" + this->_strServer + "\\" + this->_strName;
	}

	wstring Share::getServerW() const {
		wstring wstrServer(this->_strServer.begin(), this->_strServer.end());
		return wstrServer;
	}

	typedef vector<ShareACE>::const_iterator ACE_ITERATOR;

	bool Share::isAccountPermitted(const wstring& wstrAccountSought) const { CYCLOPSDEBUG("share/wstrAccountSought = %s/%S", this->_strName.c_str(), wstrAccountSought.c_str());
		if (this->_boIsNull) { return true; }
		string strAccountSought(wstrAccountSought.begin(), wstrAccountSought.end()); CYCLOPSDEBUG("strAccountSought = %s", strAccountSought.c_str());
		for (ACE_ITERATOR  ace = _aceVector.begin(); ace < _aceVector.end(); ++ace) {
			/* First compare the short name. */
			if (_stricmp(strAccountSought.c_str(), ace->getAccount().c_str()) == 0) {
				return true;
			}
			/* Then the long name. */
			string strDomainOfAssignedAccount = ace->getDomain(); CYCLOPSDEBUG("strDomainOfAssignedAccount = '%s'", strDomainOfAssignedAccount.c_str());
			string strPrefix = "";
			if ( ! StringEmUp::isWhitespaceOnly(strDomainOfAssignedAccount)) { CYCLOPSDEBUG("Domain is not whitespace only.");
				strPrefix = strDomainOfAssignedAccount + "\\";
			}
			string strAccountAssigned =  strPrefix + ace->getAccount(); CYCLOPSDEBUG("strAccountAssigned = %s", strAccountAssigned.c_str());
			/* std::transform(strAccountSought.begin(), strAccountSought.end(), strAccountSought.begin(), ::tolower); CYCLOPSDEBUG("strAccountSought lowercase = %s", strAccountSought.c_str()); */
			/* std::transform(strAccountAssigned.begin(), strAccountAssigned.end(), strAccountAssigned.begin(), ::tolower); CYCLOPSDEBUG("share/strAccountAssigned = %s/%s", this->_strName.c_str(), strAccountAssigned.c_str());  */
			if (_stricmp(strAccountSought.c_str(), strAccountAssigned.c_str()) == 0) { CYCLOPSDEBUG("Returning true.");
				return true;
			} 
		} CYCLOPSDEBUG("Returning false.");
		return false;
	}

	bool Share::isAccountPermitted(const std::string& strAccount) const { CYCLOPSDEBUG("Hello.");
		wstring wstrAccount(strAccount.begin(), strAccount.end()); CYCLOPSDEBUG("wstrAccount = %S", wstrAccount.c_str());
		return this->isAccountPermitted(wstrAccount);
	}

	void Share::addACE(const ShareACE& ace) {
		_boIsNull = false;
		_aceVector.push_back(ace); 
	}
}
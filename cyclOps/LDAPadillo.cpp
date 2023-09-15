#include "stdafx.h"

#include <sstream>
#include <regex>
#include <iostream>

#include "cyclOps.h"

#include "ExceptionNoActiveEntries.h"
#include "ExceptionUnableGetCountryFromDN.h"
#include "ExceptionNoSuchAttribute.h"

#include "LDAPadillo.h"
#include "StringEmUp.h"

using std::stringstream;
using std::string;
using std::vector;
using std::cout;
using std::endl;

using cyclOps::ExceptionNoActiveEntries;

namespace cyclOps {

	const char* LDAPadillo::ATTRIBUTE_DISTINGUISHED_NAME = "distinguishedName";
	const char* LDAPadillo::ATTRIBUTE_USER_ACCOUNT_CONTROL = "userAccountControl";
	const char* LDAPadillo::ATTRIBUTE_SAM_ACCOUNT_NAME = "sAMAccountName";
	const char* LDAPadillo::ATTRIBUTE_CN = "cn";
	const char* LDAPadillo::ATTRIBUTE_OPERATING_SYSTEM = "operatingSystem";
	const char* LDAPadillo::ATTRIBUTE_DESCRIPTION = "description";
	const char* LDAPadillo::ATTRIBUTE_MEMBER_OF = "memberOf";
	const char* LDAPadillo::ATTRIBUTE_MEMBER = "member";
	const char* LDAPadillo::ATTRIBUTE_DISPLAY_NAME = "displayName";
	const char* LDAPadillo::ATTRIBUTE_MAIL = "mail";

	void LDAPadillo::search(string strFilter, LDAP_ENTRY_VECTOR& entryVector) { CYCLOPSDEBUG("Hello.  strFilter = '%s'", strFilter.c_str());
		LDAP* pLDAP = this->getLDAP();
		LDAPMessage* pSearchResult;
		char szFilter[1000];
		CYCLOPS_SNPRINTF_S(szFilter, "%s", strFilter.c_str());
		char* pchBase = cyclOps::StringEmUp::new_charArray(_strBaseDN);
		ldap_search_sA(pLDAP, pchBase, LDAP_SCOPE_SUBTREE, szFilter, NULL, 0, &pSearchResult); 
		delete[] pchBase;
		this->fillEntryVector(pLDAP, pSearchResult, entryVector);
	}

	void LDAPadillo::getEntryWithSpecifiedDN(const string& dn, LDAP_ENTRY& entry) {
		LDAP* pLDAP = this->getLDAP();
		LDAPMessage* pSearchResult;
		string strFilter;
		char szFilter[500];
		_snprintf_s(szFilter, sizeof(szFilter) / sizeof(szFilter[0]) - 1, "%s", "(objectClass=*)");
		char* pchDN = StringEmUp::new_charArray(dn);
		ldap_search_sA(pLDAP, pchDN, LDAP_SCOPE_BASE, szFilter, NULL, 0, &pSearchResult); 
		delete[] pchDN;
		ULONG iEntries = ldap_count_entries(pLDAP, pSearchResult);  CYCLOPSDEBUG("entries = %d", iEntries);   
		
		if (iEntries == 0) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchEntry, "There is no entry with DN '%s'", dn.c_str());
		} else {
			LDAPMessage* pLDAPMessageEntry = ldap_first_entry(pLDAP, pSearchResult); 
			this->fillAttributeMap(pLDAP, pLDAPMessageEntry, entry); 
		}
	}

	void LDAPadillo::printEntry(const LDAP_ENTRY& entry) {
		for (LDAP_ENTRY::const_iterator iterator = entry.begin(); iterator != entry.end(); ++iterator) {
			printf("%s\n", iterator->first.c_str());
			vector<string> vals = iterator->second;
			for (int j = 0; j < vals.size(); ++j) {
				printf("\t%s\n", vals[j].c_str());
			}
		}

	}

	void LDAPadillo::fillEntryVector(LDAP* pLDAP, LDAPMessage* pLDAPMessageSearchResult, LDAP_ENTRY_VECTOR& entryVector) { CYCLOPSDEBUG("Hello.");
		ULONG iEntries = ldap_count_entries(pLDAP, pLDAPMessageSearchResult);  CYCLOPSDEBUG("entries = %d", iEntries);   
		LDAPMessage* pLDAPMessageEntry = NULL; 
		for (ULONG  iCnt = 0; iCnt < iEntries; iCnt++ ) {
			if( ! iCnt ) { 
				pLDAPMessageEntry = ldap_first_entry(pLDAP, pLDAPMessageSearchResult); 
			} else {
				pLDAPMessageEntry = ldap_next_entry(pLDAP, pLDAPMessageEntry); 
			}
			LDAP_ENTRY entry;
			this->fillAttributeMap(pLDAP, pLDAPMessageEntry, entry);
			entryVector.push_back(entry);
		}
	}

	void LDAPadillo::fillAttributeMap(LDAP* pLDAP, LDAPMessage* pLDAPMessageEntry, LDAP_ENTRY& entry) { CYCLOPSDEBUG("Hello.");
		BerElement* pBer = NULL;
		char* pAttribute = ldap_first_attributeA(pLDAP, pLDAPMessageEntry, &pBer);
		while (pAttribute != NULL) {
			char** ppValue = ldap_get_valuesA(pLDAP, pLDAPMessageEntry, pAttribute);
			if (ppValue != NULL) {
				ULONG iValue = ldap_count_valuesA(ppValue);
				if (iValue) {
					vector<string> valueVector;
					for (ULONG i = 0; i < iValue; ++i) {
						valueVector.push_back(ppValue[i]);
					}
					entry[pAttribute] = valueVector;
				}
				if(ppValue != NULL)  {
					ldap_value_freeA(ppValue);
				}
				ppValue = NULL;
				ldap_memfreeA(pAttribute);
			} 
			pAttribute = ldap_next_attributeA(pLDAP, pLDAPMessageEntry, pBer);
		}
	}

	LDAP* LDAPadillo::getLDAP(void) { CYCLOPSDEBUG("Hello.");
		/* TODO:  Add error handling to the LDAP calls! */
		/* TODO:  This is definitely leaky, and possibly slower because of multiple connections
			if a user is doing multiple searches.  Maybe the pointers should be members? */
		LDAP* pLDAP = this->init();
		this->setOption(pLDAP);
		this->connect(pLDAP);
		this->bind(pLDAP); CYCLOPSDEBUG("Goodbye.");
		return pLDAP;
	}

	void LDAPadillo::bind(LDAP* pLDAP) { CYCLOPSDEBUG("Hello.");
		ULONG result = ldap_bind_sA(pLDAP, NULL, NULL, LDAP_AUTH_NTLM); 
		this->checkResult(result, "ldap_bind_sA");
	}

	void LDAPadillo::checkResult(ULONG result, const char* szFunction) { CYCLOPSDEBUG("Hello.");
		if (result != LDAP_SUCCESS) {
			ULONG iLastError = LdapGetLastError();
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "%s() failed with error '%s'." , szFunction, ::ldap_err2stringA(iLastError));
		}
	}



	void LDAPadillo::connect(LDAP* pLDAP) { CYCLOPSDEBUG("Hello.");
		ULONG result = ldap_connect(pLDAP, NULL);
		this->checkResult(result, "ldap_connect");
	}


	void LDAPadillo::setOption(LDAP* pLDAP) { CYCLOPSDEBUG("Hello.");
		ULONG version = LDAP_VERSION3;
		ULONG result = ldap_set_option(pLDAP, LDAP_OPT_PROTOCOL_VERSION, (void*) &version);       
		this->checkResult(result, "ldap_set_option");
	}

	LDAP* LDAPadillo::init() { CYCLOPSDEBUG("Hello.");
		char szDomain[1000];
		CYCLOPS_SNPRINTF_S(szDomain, "%s", _strDomain.c_str());
		LDAP* pLDAP = ldap_initA(szDomain, LDAP_PORT);
		if (pLDAP == NULL) {
			ULONG iLastError = LdapGetLastError();
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "ldap_initA() failed with error '%s'.", ::ldap_err2stringA(iLastError));
		}
		return pLDAP;
	}

	LDAP_ENTRY LDAPadillo::getFirstActiveEntry(const LDAP_ENTRY_VECTOR& v) { 
		if (v.size() == 0) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "There are no entries in the entry vector.");
		}
		for (int i = 0; i < v.size(); ++i) {
			if (LDAPadillo::isEntryActive(v[i])) {
				return v[i];
			}
		}
		throw ExceptionNoActiveEntries();
	}

	bool LDAPadillo::isEntryActive(const LDAP_ENTRY& entry) {
		if (LDAPadillo::isUserAccountControlDisabled(entry)) {
			return false;
		}
		if (LDAPadillo::isUserInDeactivatedUsersContainer(entry)) {
			return false;
		}
		return true;
	}

	bool LDAPadillo::isUserInDeactivatedUsersContainer(const LDAP_ENTRY& entry) {
		string strDistinguishedName = LDAPadillo::getAttribute(entry, "distinguishedName");
		if (strDistinguishedName.find("Deactivated Users") != string::npos) { CYCLOPSDEBUG("%s is in Deactivated Users", strDistinguishedName.c_str());
			return true;
		}
		return false;
	}

	void LDAPadillo::getAttributeVector(const LDAP_ENTRY& entry, const string& strAttribute, vector<string>& vectorOfAttributes) {
		LDAP_ENTRY::const_iterator iter = entry.find(strAttribute);
		if (iter == entry.end()) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchAttribute, "No %s in entry.", strAttribute.c_str());
		}
		vector<string> valueVector = iter->second; CYCLOPSDEBUG("valueVector.size() = %d", valueVector.size());
		if (valueVector.size() == 0) {
			string strWhat = StringEmUp::format("%s not present in account.", strAttribute.c_str());
			throw std::runtime_error(strWhat);
		} else {
			for (int i = 0; i < valueVector.size(); ++i) {
				vectorOfAttributes.push_back(valueVector[i]);
			}
		}
	}

	string LDAPadillo::getAttribute(const LDAP_ENTRY& entry, const string& strAttribute) {
		LDAP_ENTRY::const_iterator iter = entry.find(strAttribute);
		if (iter == entry.end()) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchAttribute, "No %s in entry.", strAttribute.c_str());
		}
		vector<string> valueVector = iter->second; CYCLOPSDEBUG("valueVector.size() = %d", valueVector.size());
		if (valueVector.size() == 0) {
			string strWhat = StringEmUp::format("%s not present in account.", strAttribute.c_str());
			throw std::runtime_error(strWhat);
		}
		return valueVector[0];
	}

	const string& LDAPadillo::getAttributeAsReferenceBeCareful(const LDAP_ENTRY& entry, const string& strAttribute) {
		LDAP_ENTRY::const_iterator iter = entry.find(strAttribute);
		if (iter == entry.end()) { 
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionNoSuchAttribute, "No %s in entry.", strAttribute.c_str());
		}
		const vector<string>& valueVector = iter->second; CYCLOPSDEBUG("valueVector.size() = %d", valueVector.size());
		if (valueVector.size() == 0) {
			string strWhat = StringEmUp::format("%s not present in account.", strAttribute.c_str());
			throw std::runtime_error(strWhat);
		}
		return valueVector[0];
	}


	bool LDAPadillo::isUserAccountControlDisabled(const LDAP_ENTRY& entry) {
		string strUserAccountControl = LDAPadillo::getAttribute(entry, ATTRIBUTE_USER_ACCOUNT_CONTROL);
		/* stoi() can throw std::invalid_argument. */
		int iUserAccountControl = std::stoi(strUserAccountControl); CYCLOPSVAR(iUserAccountControl, "%d");
		return (iUserAccountControl & 2) > 0;
	}

	string LDAPadillo::getCountryFromDistinguishedName(const string& strDistinguishedName) {
		string strPattern = "(.*)(OU=)(..)(,DC=eu,DC=scor,DC=local)";
		std::regex pattern(strPattern);
		std::smatch sm;    
		std::regex_match(strDistinguishedName, sm, pattern);
		std::regex_match ( strDistinguishedName.cbegin(), strDistinguishedName.cend(), sm, pattern);
		for (unsigned i = 0; i < sm.size(); ++i) {
			CYCLOPSDEBUG("match %d = %s", i, sm[i].str().c_str());
		}
		if (sm.size() != 5) {
			char szMessage[5000];
			CYCLOPS_SNPRINTF_S(szMessage, "Can't retrieve country from string '%s' using pattern '%s'", strDistinguishedName.c_str(), strPattern.c_str());
			throw ExceptionUnableGetCountryFromDN(szMessage);
		}
		return sm[3].str();
	}

	void LDAPadillo::getAllMembersGivenDistinguishedNameRecursive(const string& groupDN, LDAP_ENTRY_VECTOR& vectorOfMembers) {
		string query = cyclOps::StringEmUp::format(
			"(&(objectClass=user)(memberof:1.2.840.113556.1.4.1941:=%s))", groupDN.c_str()); CYCLOPSDEBUG("query = %s", query.c_str());
		this->search(query, vectorOfMembers);
	}
}

#ifndef LDAPADILLO_H
#define LDAPADILLO_H

#include <string>
#include <vector>
#include <map>

#include <Windows.h>
#include <Winldap.h>

namespace cyclOps {

	typedef std::map<std::string, std::vector<std::string>> LDAP_ENTRY;
	typedef std::vector<LDAP_ENTRY> LDAP_ENTRY_VECTOR;

	class LDAPadillo {
	public:
		LDAPadillo() : _strBaseDN("dc=eu,dc=scor,dc=local") { }
		~LDAPadillo() { }
		LDAPadillo& setBaseDN(const std::string& strBaseDN) { _strBaseDN.assign(strBaseDN); return *this; }
		LDAPadillo& setDomain(const std::string& strDomain) { _strDomain.assign(strDomain); return *this; }
		void search(std::string, LDAP_ENTRY_VECTOR& results);
		static std::string getDistinguishedNameFromName(const std::string&);
		static std::string getCountryFromDistinguishedName(const std::string& strDistinguishedName);
		static LDAP_ENTRY getFirstActiveEntry(const LDAP_ENTRY_VECTOR& v);
		void getEntryWithSpecifiedDN(const std::string& dn, LDAP_ENTRY& entry);
		void getAllMembersGivenDistinguishedNameRecursive(const std::string& groupDN, cyclOps::LDAP_ENTRY_VECTOR& members);
		static bool isEntryActive(const LDAP_ENTRY& entry);
		static bool isUserAccountControlDisabled(const LDAP_ENTRY& entry);
		static std::string getAttribute(const LDAP_ENTRY& entry, const std::string& strAttribute);
		static void getAttributeVector(const LDAP_ENTRY& entry, const std::string& strAttribute, std::vector<std::string>& vectorOfAttributes);
		static const std::string& getAttributeAsReferenceBeCareful(const LDAP_ENTRY& entry, const std::string& strAttribute);
		static bool isUserInDeactivatedUsersContainer(const LDAP_ENTRY& entry);
		static void printEntry(const LDAP_ENTRY& entry);
		static char* ATTRIBUTE_DISTINGUISHED_NAME;
		static char* ATTRIBUTE_USER_ACCOUNT_CONTROL;
		static char* ATTRIBUTE_SAM_ACCOUNT_NAME;
		static char* ATTRIBUTE_CN;
		static char* ATTRIBUTE_OPERATING_SYSTEM;
		static char* ATTRIBUTE_DESCRIPTION;
		static char* ATTRIBUTE_MEMBER_OF;
		static char* ATTRIBUTE_MEMBER;
		static char* ATTRIBUTE_DISPLAY_NAME;
		static char* ATTRIBUTE_MAIL;
	private:
		std::string _strDomain;
		std::string _strBaseDN;
		LDAP* getLDAP(void);
		void fillEntryVector(LDAP* pLDAP, LDAPMessage* pLDAPMessage, LDAP_ENTRY_VECTOR& entryVector);
		void fillAttributeMap(LDAP* pLDAP, LDAPMessage* pLDAPMessageEntry, LDAP_ENTRY& attributeMap);
		LDAP* init();
		void setOption(LDAP* pLDAP);
		void connect(LDAP* pLDAP);
		void bind(LDAP* pLDAP);
		void checkResult(ULONG result, char* szFunction);
	};
}
#endif
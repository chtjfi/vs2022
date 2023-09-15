// LDAPDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>
#include <iostream>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

using std::stringstream;
using std::string;
using std::map;
using std::vector;
using std::cout;
using std::endl;
using std::regex;

struct comparatorAttributeNames
{
    bool operator()(string s1, string s2) const
    {
		return _stricmp(s1.c_str(), s2.c_str()) < 0;
    }
};


map<string, vector<string>, comparatorAttributeNames>* 
	getAttributeMap(
		LDAP* pLdapConnection, 
		LDAPMessage* pEntry);
void outputAttributes(
	map<string, vector<string>,	comparatorAttributeNames>* pAttributeMap, 
	const string& strAttributePattern);
bool doesThisAttributeMatchAttributePattern(
	const string& strAttribute, 
	const string& strAttributePattern);




int main(int argc, char* argv[])
{
	int iReturn = 0;
	try {
		cyclOps::CommandLinebacker cmd(argc, argv); 
		g_boCyclOpsDebug = cmd.isOptionPresent("debug"); 
		char* szFilter = argv[1];
		string strOU = cmd.getOption("ou", "dc=eu,dc=scor,dc=local"); CYCLOPSDEBUG("strOU = %s", strOU.c_str());
		char* szOU = new char[strOU.length() + 10];
		_snprintf_s(szOU, strOU.length() + 1, _TRUNCATE, "%s", strOU.c_str()); CYCLOPSDEBUG("szOU = %s", szOU);
		string strAttributePattern = cmd.getOption("attributes", "ALL_ATTRIBUTES");
		string strDNSDomain = cmd.getOption("dnsDomain", "eu.scor.local");
		char szDNSDomain[5000]; _snprintf_s(szDNSDomain, CYCLOPSSIZEOF(szDNSDomain), _TRUNCATE, "%s", strDNSDomain.c_str());
		LDAP* pLdapConnection = ldap_initA(szDNSDomain, LDAP_PORT); CYCLOPSVAR(pLdapConnection, "%p");
		ULONG version = LDAP_VERSION3;
		ldap_set_option(pLdapConnection, LDAP_OPT_PROTOCOL_VERSION, (void*) &version);       
		ldap_connect(pLdapConnection, NULL);
		PSTR dn = NULL;
		PCHAR cred = NULL;
		ULONG method = LDAP_AUTH_NTLM;
		if (cmd.isOptionPresent("user")) {
			string user = cmd.getOption("user");
			dn = cyclOps::StringEmUp::new_charArray(user);
			string password = cmd.getOption("password");
			cred = cyclOps::StringEmUp::new_charArray(password);
			method = LDAP_AUTH_SIMPLE;
		}
		ldap_bind_sA(pLdapConnection, dn, cred, method);
		LDAPMessage* pSearchResult;
		ldap_search_sA(pLdapConnection, szOU, LDAP_SCOPE_SUBTREE, szFilter, NULL, 0, &pSearchResult);    
		ULONG numberOfEntries = numberOfEntries = ldap_count_entries(pLdapConnection, pSearchResult);  CYCLOPSVAR(numberOfEntries, "%d");   
		if (numberOfEntries < 1) {
			printf("No entires match your criteria.\n");
			iReturn = 1;
		} else {
			LDAPMessage* pEntry = NULL; CYCLOPSDEBUG(".");
			for (ULONG  iCnt = 0; iCnt < numberOfEntries; iCnt++ ) {
				if( !iCnt ) { CYCLOPSDEBUG(".");
					pEntry = ldap_first_entry(pLdapConnection, pSearchResult); CYCLOPSDEBUG(".");
				} else {
					pEntry = ldap_next_entry(pLdapConnection, pEntry); CYCLOPSDEBUG(".");
				}
				map<string, vector<string>, comparatorAttributeNames>* pAttributeMap 
					= getAttributeMap(pLdapConnection, pEntry);
				outputAttributes(pAttributeMap, strAttributePattern);
			}
		}
	} catch (...) {
		printf("exception\n");
		iReturn = 1;
	}
	return iReturn;

}

void outputAttributes(
	map<string, vector<string>, comparatorAttributeNames>* pAttributeMap, 
	const string& strAttributePattern) 
{
	printf("==================================================================\n");
	/* cout << "distinguishedName\t" << (*pAttributeMap)["distinguishedName"][0] << endl; */
	// const char* pchDescription = (*pAttributeMap)["description"].c_str();
	for (map<string, vector<string>, comparatorAttributeNames>::iterator iterator 
		= pAttributeMap->begin();
		iterator != pAttributeMap->end(); ++iterator) 
	{
		if (::doesThisAttributeMatchAttributePattern(iterator->first, strAttributePattern)) {
			vector<string> valueVector = iterator->second;
			size_t iSize = valueVector.size();
			for (size_t i = 0; i < iSize; ++i) {
				string str = valueVector.at(i);
				cout << iterator->first << "\t" << str << "\n";
			}
		}
	}
	printf("\n");
}

map<string, vector<string>, comparatorAttributeNames>* getAttributeMap(LDAP* pLdapConnection, LDAPMessage* pEntry) { CYCLOPSDEBUG("Hello.");
	BerElement* pBer = NULL;
	map<string, vector<string>, comparatorAttributeNames>* pAttributeMap = new map<string, vector<string>, comparatorAttributeNames>() ;
	char* pAttribute = ldap_first_attributeA(pLdapConnection, pEntry, &pBer); CYCLOPSDEBUG("pAttribute = %s", pAttribute);
	while (pAttribute != NULL) {
		char** ppValue = ldap_get_valuesA(pLdapConnection, pEntry, pAttribute);
		if (ppValue == NULL) {
		} else {
			ULONG iValue = ldap_count_valuesA(ppValue);
			if (!iValue) {
				//printf(": [BAD VALUE LIST]");
			} else {
				vector<string> valueVector;
				for (ULONG i = 0; i < iValue; ++i) {
					valueVector.push_back(ppValue[i]);
				}
				(*pAttributeMap)[pAttribute] = valueVector;
			}
			if(ppValue != NULL)  {
				ldap_value_freeA(ppValue);
			}
			ppValue = NULL;
			ldap_memfreeA(pAttribute);
		} 
		pAttribute = ldap_next_attributeA(pLdapConnection, pEntry, pBer);
	}
	return pAttributeMap;
}

bool doesThisAttributeMatchAttributePattern(
	const string& strAttribute, 
	const string& strAttributePattern) 
{ 
	CYCLOPSDEBUG("szAttribute/strAttributePattern = %s/%s", strAttribute.c_str(), strAttributePattern.c_str());
	bool boAreAllAttributesRequested = strAttributePattern.compare("ALL_ATTRIBUTES") == 0;
	if (boAreAllAttributesRequested) {
		return true;
	}
	stringstream ss;
	ss << ".*" << strAttributePattern << ".*";
	regex rgxAttributePattern(ss.str(), std::tr1::regex_constants::icase); 
	if (regex_match(strAttribute, rgxAttributePattern)) { 
		CYCLOPSDEBUG("%s matches %s", strAttribute.c_str(), strAttributePattern.c_str());
		return true;
	} else {
		CYCLOPSDEBUG("%s doesn't match %s", strAttribute.c_str(), strAttributePattern.c_str());
		return false;
	}
}


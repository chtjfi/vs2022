#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>
#include <iostream>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::vector;
using std::map;

using cyclOps::CommandLinebacker;
using cyclOps::StringEmUp;
using cyclOps::XMLigator;

namespace spacename {

	cyclOps::XMLigator _xmlConfig;
	cyclOps::Logger _logger;

	void getLDAPEntries(cyclOps::LDAP_ENTRY_VECTOR& vectorOfLDAPEntries) {
		cyclOps::LDAPadillo ldap;
		string strDomain = _xmlConfig.getTextContent("/root/ldap_export/domain"); // ; strDomain.assign("eu.scor.local");
		string strBaseDN = _xmlConfig.getTextContent("/root/ldap_export/base_dn"); // strBaseDN.assign("DC=eu,DC=scor,DC=local");
		string strSearch = _xmlConfig.getTextContent("/root/ldap_export/filter"); // strSearch.assign("(&(objectCategory=computer))");
		ldap.setDomain(strDomain);
		ldap.setBaseDN(strBaseDN);
		ldap.search(strSearch, vectorOfLDAPEntries);
	}

	void main(const CommandLinebacker& cmd) {
		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		_logger.info("Using %s as configuration file.", _xmlConfig.getFilename().c_str());
		cyclOps::LDAP_ENTRY_VECTOR vectorOfLDAPEntries;
		spacename::getLDAPEntries(vectorOfLDAPEntries);
		for (int i = 0; i < vectorOfLDAPEntries.size(); ++i) {
			cyclOps::LDAP_ENTRY entry = vectorOfLDAPEntries[i];
			cyclOps::LDAPadillo::printEntry(entry);
		}
	}

}

int main(int argc, char* argv[])
{
	try {

		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/

		CommandLinebacker cmd(argc, argv);
		g_boCyclOpsDebug = cmd.isOptionPresent("debug");
		cmd.initializeConfigFile(spacename::_xmlConfig, true); /* Don't use exe name as default. */
		cmd.initializeLogFile(spacename::_logger, cyclOps::CommandLinebacker::MakeLognameUnique::False);
		spacename::main(cmd);
		printf("The command completed successfully.\n");
		return 0;
	}
	catch (const cyclOps::Exception& e) {
		CYCLOPSERROR_II(spacename::_logger, "%s - %s - %s\n", typeid(e).name(), e.what(), e.formatMessage().c_str());
		CYCLOPSERROR_II(spacename::_logger, "%s\n", e.what());
		return 1;
	}
	catch (const std::exception& e) {
		CYCLOPSERROR_II(spacename::_logger, "%s - %s\n", typeid(e).name(), e.what());
		CYCLOPSERROR_II(spacename::_logger, "%s\n", e.what());
		return 1;
	}
	catch (...) {
		CYCLOPSERROR_II(spacename::_logger, "UNKNOWN EXCEPTION\n");
		return 1;
	}
}

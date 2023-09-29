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

	string& getOption(const string& option, const CommandLinebacker& cmd) {
		try {
			string strValue =  cmd.getOption(option);
			return strValue;
		} catch (const cyclOps::ExceptionOptionNotSet& e) {
			stringstream ssElement; ssElement << "/root/ldap_query/" << option;
			string strValue = _xmlConfig.getTextContent(ssElement.str()); // ; strDomain.assign("eu.scor.local");
		}
	}

	void getLDAPEntries(cyclOps::LDAP_ENTRY_VECTOR& vectorOfLDAPEntries, const CommandLinebacker& cmd) {
		cyclOps::LDAPadillo ldap;
		string strDomain = spacename::getOption("domain", cmd);
		string strBaseDN = spacename::getOption("base_dn", cmd); // strBaseDN.assign("DC=eu,DC=scor,DC=local");
		string strSearch = cmd.getOption("filter"); 
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
		spacename::getLDAPEntries(vectorOfLDAPEntries, cmd);
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
		try {
			cmd.initializeConfigFile(spacename::_xmlConfig, true); /* Don't use exe name as default. */
		}
		catch (const cyclOps::ExceptionXMLigator& e) { 
			CYCLOPSDEBUG("No config file specified or file not found or valid. [%s]", e.getMessage().c_str());
		}
		/* cmd.initializeLogFile(spacename::_logger, cyclOps::CommandLinebacker::MakeLognameUnique::False); */
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

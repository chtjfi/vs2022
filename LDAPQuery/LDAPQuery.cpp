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

	string getOption(const string& option, const CommandLinebacker& cmd) {
		try {
			string strValue =  cmd.getOption(option);
			return strValue;
		} catch (const cyclOps::ExceptionOptionNotSet& e) {
			stringstream ssElement; ssElement << "/root/ldap_query/" << option;
			try {
				string strValue = _xmlConfig.getTextContent(ssElement.str()); CYCLOPSDEBUG("strValue = %s", strValue.c_str());
				return strValue;
			} catch (const cyclOps::Exception& e2) {
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "The option '%s' is not specified on the command line or the config file.", option.c_str());
			}
		}
	}

	void getLDAPEntries(cyclOps::LDAP_ENTRY_VECTOR& vectorOfLDAPEntries, const CommandLinebacker& cmd) {
		cyclOps::LDAPadillo ldap;
		string strDomain = spacename::getOption("domain", cmd); CYCLOPSINFO("Domain is %s", strDomain.c_str());
		string strBaseDN = spacename::getOption("base_dn", cmd); CYCLOPSINFO("Base DN is %s", strBaseDN.c_str()); // strBaseDN.assign("DC=eu,DC=scor,DC=local");
		string strSearch = spacename::getOption("filter", cmd);  CYCLOPSINFO("Filter is %s", strSearch.c_str());
		ldap.setDomain(strDomain);
		ldap.setBaseDN(strBaseDN); CYCLOPSDEBUG("About to call ldap.search()");
		ldap.search(strSearch, vectorOfLDAPEntries);
	}

	void main(const CommandLinebacker& cmd) { CYCLOPSDEBUG("Hello.");
		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		cyclOps::LDAP_ENTRY_VECTOR vectorOfLDAPEntries;
		spacename::getLDAPEntries(vectorOfLDAPEntries, cmd); CYCLOPSDEBUG("%d entries match filter.", vectorOfLDAPEntries.size());
		for (int i = 0; i < vectorOfLDAPEntries.size(); ++i) {
			printf("===============================================================================\n");
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
		/* cmd.initializeLogFile(spacename::_logger, cyclOps::CommandLinebacker::MakeLognameUnique::False);*/
		try {
			cmd.initializeConfigFile(spacename::_xmlConfig, true); /* Don't use exe name as default. */
			spacename::_logger.info("Using %s as configuration file.", spacename::_xmlConfig.getFilename().c_str());
		} catch (const cyclOps::ExceptionXMLigator& e) { 
			CYCLOPSINFO("No config file specified or file not found or valid. [%s]", e.getMessage().c_str());
		}
		spacename::main(cmd);
		printf("The command completed successfully.\n");
		return 0;
	} catch (const cyclOps::Exception& e) {
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

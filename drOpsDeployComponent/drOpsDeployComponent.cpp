#include "stdafx.h"

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"
#include "..\drOps\Deployer.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::vector;
using std::map;

using cyclOps::CommandLinebacker;
using cyclOps::StringEmUp;
using cyclOps::XMLigator;

namespace drOps {
	namespace deployComponent {

		cyclOps::XMLigator _xmlConfig;
		boolean _disableNotifications = false;
		cyclOps::Logger _logger; 

		void deployMultipleVersions(drOps::Deployer& deployer, const string& versions) {
			vector<string> vectorOfVersions;
			StringEmUp::tokenize(versions, "[;\n]", vectorOfVersions); CYCLOPSDEBUG("vectorOfVersions.size = %d", vectorOfVersions.size());
			for (int i = 0; i < vectorOfVersions.size(); ++i) { CYCLOPSDEBUG("vectorOfVersions[%d] = %s", i, vectorOfVersions[i].c_str()); }
			for (int i = 0; i < vectorOfVersions.size(); ++i) { CYCLOPSVAR(i, "%d");
				string versionTrimmed = vectorOfVersions[i]; 
				StringEmUp::trim(versionTrimmed);
				deployer.setVersion(versionTrimmed);
				deployer.deploy();
			}
		}

		void deployMultipleFiles(drOps::Deployer& deployer, const string& directory, const string& files) {
			vector<string> vectorOfFiles;
			cyclOps::StringEmUp::split(files, ';', vectorOfFiles);
			for (int i = 0; i < vectorOfFiles.size(); ++i) {
				string file = directory + "\\" + vectorOfFiles[i];
				deployer.setAdditionalParameter("devControlXML", file);
				deployer.deploy();
			}
		}


		void deploy(const string& app, const string& environment, const DOMNode* pNode, 
			map<string, string>& mapOfServerSettings,
			boolean boContinueOnFailures) 
		{
			const string& component = XMLigator::getTextContent(pNode, "componentID");
			map<string, string> mapOfParameters;
			XMLigator::getChildTextContentMap(pNode, "", mapOfParameters);
			drOps::Deployer deployer;
			deployer.setHostname(mapOfServerSettings["hostname"]);
			deployer.setPort(mapOfServerSettings["port"]);
			string queryFormatForDeployment = /*_xmlConfig.getTextContent("/root/drOps/server/queryFormats/deploy"); CYCLOPSDEBUG_II(_logger, "queryFormatForDeployment = %s", queryFormatForDeployment.c_str());*/
				"/drOps/rest/ntlm/queueDeployment?";
				/*"?applicationID=${applicationID}&"
				"componentID=${componentID}&"
				"environment=${environment}";*/

			deployer.setQueryFormatForDeployment(queryFormatForDeployment);
			string queryFormatForStatus = _xmlConfig.getTextContent("/root/drOps/server/queryFormats/status");
			deployer.setQueryFormatForStatus(queryFormatForStatus);
			deployer.setLogger(&_logger);
			deployer.setApplicationID(app);
			deployer.setComponentID(component);
			deployer.setEnvironment(environment);
			deployer.setAdditionalParameters(mapOfParameters); 
			deployer.setWait( ! boContinueOnFailures);
			if (cyclOps::STLadocious::doesMapOfStringsContain(mapOfParameters, "versions")) {
				string versions = mapOfParameters.at("versions");
				drOps::deployComponent::deployMultipleVersions(deployer, versions);
			} else if (cyclOps::STLadocious::doesMapOfStringsContain(mapOfParameters, "files")) {
				string files = mapOfParameters.at("files");
				string directory = mapOfParameters.at("directory");
				drOps::deployComponent::deployMultipleFiles(deployer, directory, files);
			} else {
				deployer.deploy();
			}
		}

		bool isDeploymentInsideAllowedTimeWindows(void) {
			try {
				string not_before = _xmlConfig.getTextContent("/root/deployments/not_before"); CYCLOPSDEBUG_II(_logger, "not_before = %s", not_before.c_str());
				cyclOps::TimeTiger tiger;
				if ( ! tiger.isNowLaterThan(not_before)) { CYCLOPSDEBUG("Too early.");
					return false;
				} else {
					int hoursValid = _xmlConfig.getTextContentAs_int("/root/deployments/valid_for_x_hours");
					if (tiger.isNowLaterThanTimePlusXHours(not_before, hoursValid)) {
						return false;
					}
				}
			} catch (cyclOps::ExceptionNoSuchNode ok) { }
			try {
				string not_after = _xmlConfig.getTextContent("/root/deployments/not_after");
				cyclOps::TimeTiger tiger;
				if ( ! tiger.isNowEarlierThan(not_after)) { CYCLOPSDEBUG("Too late.");
					return false;
				}
			} catch (cyclOps::ExceptionNoSuchNode ok) { }
			return true;
		}

		void main(const CommandLinebacker& cmd) {
			if ( ! drOps::deployComponent::isDeploymentInsideAllowedTimeWindows()) {
				throw std::runtime_error("The current time is not inside the specified deployment time window.");
			}
			map<string, string> mapOfServerSettings;
			_xmlConfig.getChildTextContentMap("/root/drOps/server", mapOfServerSettings);
			for (map<string, string>::iterator i = mapOfServerSettings.begin(); i != mapOfServerSettings.end(); i++) {
				CYCLOPSDEBUG_II(_logger, "%s = %s", (i->first).c_str(), (i->second).c_str());
			}
			const string& app = _xmlConfig.getTextContent("/root/deployments/applicationID");
			const string& env = _xmlConfig.getTextContent("/root/deployments/environment");
			vector<DOMNode*> vectorOfNodes;
			_xmlConfig.getNodeVector("/root/deployments/deployment", vectorOfNodes);
			boolean boContinueOnFailures = false;
			try {
				boContinueOnFailures = _xmlConfig.getTextContentAsBoolean("/root/deployments/continueOnFailures");
			} catch (const cyclOps::ExceptionNoSuchNode& e) { }
			for (int i = 0; i < vectorOfNodes.size(); ++i) {
				drOps::deployComponent::deploy(app, env, vectorOfNodes[i], mapOfServerSettings, boContinueOnFailures);
			}
		}

		void sendLogByEmailIfSpecified(void) { CYCLOPSDEBUG_II(_logger, "Hello.");
			try {
				string to;
				try {
					to.assign(_xmlConfig.getTextContent("/root/mail/to")); CYCLOPSDEBUG_II(_logger, "to = %s", to.c_str());
				} catch (const cyclOps::ExceptionNoSuchNode& e) {
					_logger.info("Email not enabled.");
					return;
				}
				string share = _xmlConfig.getTextContent("/root/mail/automail_share"); 
				cyclOps::Automail automail;
				automail.addRecipient(to);
				automail.setAutomailShare(share);
				automail.setSubject("Bundle Deployment Log");
				string contents; _logger.getLogFileContents(contents);
				automail.setMessage(contents);
				automail.send();
			} catch (const std::exception& e) {
				CYCLOPSERROR_II(drOps::deployComponent::_logger, "%s - %s\n", typeid(e).name(), e.what());
				CYCLOPSERROR_II(drOps::deployComponent::_logger, "%s\n", e.what());
			}
		}
	}
}

int main(int argc, char* argv[]) {
	int iReturn = 0;
	try {
		CommandLinebacker cmd(argc, argv);
		g_boCyclOpsDebug = cmd.isOptionPresent("debug");
		cmd.initializeConfigFile(drOps::deployComponent::_xmlConfig, 
			false, /* Don't use exe name as default. */
			true); /* Use argument in position one if /config not set. */
		cmd.initializeLogFile(drOps::deployComponent::_logger, CommandLinebacker::MakeLognameUnique::True);
		drOps::deployComponent::main(cmd);
		drOps::deployComponent::_logger.info("The command completed successfully.\n");
		iReturn = 0;
	} catch (const cyclOps::Exception& e) {
		CYCLOPSERROR_II(drOps::deployComponent::_logger, "%s - %s - %s\n", typeid(e).name(), e.what(), e.formatMessage().c_str());
		CYCLOPSERROR_II(drOps::deployComponent::_logger, "%s\n", e.what());
		iReturn = 1;
	}
	catch (const std::exception& e) { CYCLOPSDEBUG("Caught an std::exception.");
		CYCLOPSERROR_II(drOps::deployComponent::_logger, "%s - %s\n", typeid(e).name(), e.what());
		CYCLOPSERROR_II(drOps::deployComponent::_logger, "%s\n", e.what());
		iReturn = 1;
	}
	catch (...) {
		CYCLOPSERROR_II(drOps::deployComponent::_logger, "UNKNOWN EXCEPTION\n");
		iReturn = 1;
	}
	drOps::deployComponent::sendLogByEmailIfSpecified();
	return iReturn;
}

#pragma once

#include <string>

#include "StatusOfDeployment.h"

namespace drOps {
	class Deployer
	{
	public:
		Deployer();
		~Deployer();
		void setApplicationID(const std::string& s) { _additionalParameters["applicationID"] = s; }
		std::string getApplicationID(void) { return _additionalParameters.at("applicationID"); }
		void setComponentID(const std::string& s) { _additionalParameters["componentID"] = s; }
		std::string getComponentID(void) { return _additionalParameters.at("componentID"); }
		void setEnvironment(const std::string& s) { _additionalParameters["environment"] = s; }
		std::string getEnvironment(void) { return _additionalParameters.at("environment"); }
		void setWait(bool s) { _wait = s; }
		bool getWait(void) { return _wait; }
		void setHostname(const std::string& s) { _hostname.assign(s); }
		void setPort(int i) { _port = i; }
		void setPort(const std::string& port) { 
			_port = std::stoi(port);
		}
		void deploy(void);
		void outputDeploymentParameters(void);
		void setQueryFormatForDeployment(const std::string s) { _queryFormatForDeployment.assign(s); }
		void setQueryFormatForStatus(const std::string s) { _queryFormatForStatus.assign(s); }
		void setSendNofifications(boolean b) { _sendNotifications = b; }
		void setAdditionalParameters(std::map<std::string, std::string> mapOfAdditionalParameters);
		void setAdditionalParameter(const std::string& parameter, const std::string& value);
		void setLogger(cyclOps::Logger* logger) { _logger = logger; }
		void setVersion(const std::string& version);
	private:
		/*std::string _applicationID;
		std::string _componentID;
		std::string _environment;*/
		std::string _hostname;
		std::string _queryFormatForDeployment;
		std::string _queryFormatForStatus;
		cyclOps::Logger* _logger;
		int _deploymentID;
		boolean _sendNotifications;
		std::map<std::string, std::string> _additionalParameters;
		int _port;
		bool _wait;
		void wait(void);
		std::string createQueryForDeployment();
		void addAdditionalParametersToRequest(std::string& query);
		std::string createQueryForStatus();
		void sendStatus(const std::string& to, const std::string& query, const std::string& output);
		boolean isDeploymentRunningOrQueued();
		boolean isQueueingStatusSuccess(const std::string& json);
		boolean isDeploymentSuccessful(const drOps::StatusOfDeployment& status);
		boolean isRunningOrQueued(const std::string& json);
		void setDeploymentID(const std::string& json);
		drOps::StatusOfDeployment getDeploymentStatus();

	};
}
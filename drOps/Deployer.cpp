#include "stdafx.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "..\cyclOps\cyclOps.h"
#include "..\drOps\ExceptionFailedToQueueDeployment.h"
#include "..\drOps\ExceptionFailedDeployment.h"
#include "..\drOps\ExceptionFailedToQueryDeploymentStatus.h"

#include "Deployer.h"

using std::string;
using std::vector;
using std::map;

namespace drOps {
	Deployer::Deployer() : _wait(true), _port(80), _sendNotifications(true)
	{
	}


	Deployer::~Deployer()
	{
	}

	void Deployer::addAdditionalParametersToRequest(std::string& query) {
		for (map<string, string>::iterator i = _additionalParameters.begin(); i != _additionalParameters.end(); ++i) { CYCLOPSDEBUG("first/second: %s/%s", (*i).first.c_str(), (*i).second.c_str());
			query.append((*i).first + "=" + (*i).second + "&"); CYCLOPSDEBUG("query = %s", query.c_str());
		}
	}

	string Deployer::createQueryForDeployment(void) {
		cyclOps::Strung strung;
		/* At the moment 2018-09-30 we aren't actually using any variables in the format but hey who knows? */
		string url = strung
			.set("applicationID", this->getApplicationID())
			.set("componentID", this->getComponentID())
			.set("environment", this->getEnvironment())
			.addVariables(_additionalParameters)
			.format(_queryFormatForDeployment); CYCLOPSDEBUG("url = %s", url.c_str());
		/* PROBLEM:  How do I add opsOnlyDeployment without modifying every xml out there? 
		ANSWER: Maybe don't use a format at all.  Add all parameters that are provided?  */
		this->addAdditionalParametersToRequest(url); CYCLOPSDEBUG("url = %s", url.c_str());
		return url;
	}

	string Deployer::createQueryForStatus(void) {
		cyclOps::Strung strung;
		string url = strung
			.set("deploymentID", this->_deploymentID)
			.format(_queryFormatForStatus);
		return url;
	}


	void Deployer::sendStatus(const string& to, const string& query, const string& output) {
		boost::property_tree::ptree pt2;
		std::istringstream is (output);
		boost::property_tree::read_json (is, pt2);
		std::string status = pt2.get<std::string> ("status");
		printf("status %s\n", status.c_str());
		if (_sendNotifications) {
			cyclOps::Automail mail;
			mail.addRecipient("jfitzpatrick@scor.com");
			std::stringstream ss; ss << "Deployment status " << status;
			mail.setSubject(ss.str());
			mail.setMessage(query);
			mail.setAutomailShare("\\\\chncifs01\\automail");
			mail.send();
		}
	}

	void Deployer::deploy() { 
		_logger->info("*** Queueing deployment. ***");
		this->outputDeploymentParameters();
		cyclOps::HTTPopotamus http;
		http.setHostname(_hostname);
		http.setPort(_port);
		const string query = this->createQueryForDeployment(); CYCLOPSDEBUG("query = %s", query.c_str());
		http.setQuery(query);
		http.setReceiveTimeout(120 * 1000);
		string jsonQueueingResult;
		http.GET(jsonQueueingResult); CYCLOPSDEBUG("%s\n", jsonQueueingResult.c_str());
		if ( ! http.isHTTPStatusCodeSuccess()) {
			CYCLOPS_THROW_EXCEPTION_IV(drOps::ExceptionFailedToQueueDeployment, 
				"HTTP error status %d returned.", http.getStatusCode());
		} else {
			if ( ! this->isQueueingStatusSuccess(jsonQueueingResult)) {
				CYCLOPS_THROW_EXCEPTION_IV(drOps::ExceptionFailedToQueueDeployment,  "Error queuing.  JSON returned = %s", jsonQueueingResult.c_str());
			} else {
				this->setDeploymentID(jsonQueueingResult);
				_logger->info("Deployment queued successfully with ID %d.", this->_deploymentID);
				if (_wait) {
					this->wait();
				} else {
					_logger->info("continueOnFailures set to true.  Not waiting for completion of deployment.");
				}
				printf("\n");
			}
		}
	}

	void Deployer::setDeploymentID(const string& json) {
		boost::property_tree::ptree pt2;
		std::istringstream is (json);
		boost::property_tree::read_json (is, pt2);
		std::string id = pt2.get<std::string> ("deploymentID"); 
		_deploymentID = cyclOps::StringEmUp::toInt(id);

	}

	void Deployer::wait(void) {
		CYCLOPSINFO("Waiting for deployment to complete.");
		while (this->isDeploymentRunningOrQueued()) {
			::Sleep(5000);
		}
		drOps::StatusOfDeployment status = this->getDeploymentStatus();
		if ( ! this->isDeploymentSuccessful(status)) {
			string json; status.getJSON(json);
			CYCLOPS_THROW_EXCEPTION_IV(drOps::ExceptionFailedDeployment,  "Deployment failed:\n %s", json.c_str());
		} else {
			CYCLOPSINFO("Deployment %d successful.", this->_deploymentID);
		}
	}

	drOps::StatusOfDeployment Deployer::getDeploymentStatus() {
		cyclOps::HTTPopotamus http;
		http.setHostname(_hostname);
		http.setPort(_port);
		const string query = this->createQueryForStatus();
		http.setQuery(query);
		http.setReceiveTimeout(120 * 1000);
		string jsonOutput;
		http.GET(jsonOutput); CYCLOPSDEBUG("\n%s\n", jsonOutput.c_str());
		if ( ! http.isHTTPStatusCodeSuccess()) {
			CYCLOPS_THROW_EXCEPTION_IV(drOps::ExceptionFailedToQueryDeploymentStatus, 
				"HTTP error status %d returned.", http.getStatusCode());
		} else {
			drOps::StatusOfDeployment status(jsonOutput);
			/*boost::property_tree::ptree pt2;
			std::istringstream is (jsonOutput);
			boost::property_tree::read_json (is, pt2);
			status.setStatus(pt2.get<std::string> ("status")); */
			return status;
		}				
	}

	boolean Deployer::isDeploymentRunningOrQueued() {
		drOps::StatusOfDeployment status = this->getDeploymentStatus(); CYCLOPSDEBUG("status = %s", status.getStatus().c_str());
		vector<string> strings = {"RUNNING", "QUEUED", "BACKGROUND"};
		boolean running = cyclOps::StringEmUp::in(status.getStatus(), strings); CYCLOPSDEBUG("running = %d", running);
		return running;
	}

	void Deployer::setAdditionalParameters(map<string, string> mapOfAdditionalParameters) {
		//_additionalParameters.insert(mapOfAdditionalParameters.begin(), mapOfAdditionalParameters.end());
		for (map<string, string>::iterator i = mapOfAdditionalParameters.begin(); i != mapOfAdditionalParameters.end(); ++i) {
			_additionalParameters[(*i).first] = (*i).second;
		}
	}

	void Deployer::setAdditionalParameter(const string& parameter, const string& value)  {
		_additionalParameters[parameter] = value;
	}

	void Deployer::setVersion(const string& version) {
		_additionalParameters["version"] = version;
	}

	boolean Deployer::isQueueingStatusSuccess(const string& json) { CYCLOPSDEBUG("json = %s", json.c_str());
		boost::property_tree::ptree pt2;
		std::istringstream is (json);
		boost::property_tree::read_json (is, pt2);
		std::string status = pt2.get<std::string> ("status"); 
		return cyclOps::StringEmUp::equalsIgnoreCase(status, "SUCCESS");
	}
	boolean Deployer::isDeploymentSuccessful(const drOps::StatusOfDeployment& status) {
		vector<string> strings = {"SUCCESS"};
		return cyclOps::StringEmUp::in(status.getStatus(), strings);
	}

	void Deployer::outputDeploymentParameters(void) {
		for (map<string, string>::iterator i = _additionalParameters.begin(); i != _additionalParameters.end(); ++i) {
			string param = i->first;
			if (cyclOps::StringEmUp::compareIgnoreCase(param, "versions") != 0) {
				string value = i->second;
				_logger->info("%s: %s", param.c_str(), value.c_str());
			}
		}
	}
}
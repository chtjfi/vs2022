#include "stdafx.h"

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

namespace downloadfile {

	cyclOps::XMLigator _xmlConfig; 
	cyclOps::Logger _logger; 

	void getReportConfigs(vector<map<string, string>>& vectorOfMapsOfReportConfigs) {
		_xmlConfig.getVectorOfMaps("/root/reports/report", vectorOfMapsOfReportConfigs);
	}
	
	void downloadReport(const map<string, string>& mapOfSettings) {
		string query = mapOfSettings.at("query");
		_logger.info("Downloading %s", query.c_str());
		cyclOps::HTTPopotamus http;
		http.setHostname(mapOfSettings.at("hostname"));
		http.setPort(mapOfSettings.at("port"));
		http.setQuery(query);
		http.setOutputFile(mapOfSettings.at("outputFile")); 
		http.setReceiveTimeout(std::stoi(mapOfSettings.at("receiveTimeout")));
		http.GET();
		_logger.info("Downoad of %s complete.", query.c_str());
	}

	void downloadReports(const vector<map<string, string>>& vectorOfMapsOfReportConfigs) {
		for (int i = 0; i < vectorOfMapsOfReportConfigs.size(); ++i) {
			downloadfile::downloadReport(vectorOfMapsOfReportConfigs[i]);
		}
	}


	void main(const CommandLinebacker& cmd) {
		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		vector<map<string, string>> vectorOfMapsOfReportConfigs;
		downloadfile::getReportConfigs(vectorOfMapsOfReportConfigs);
		downloadfile::downloadReports(vectorOfMapsOfReportConfigs);
		_logger.info("Download of all reports completed.");
	}

}

int main(int argc, char* argv[])
{
	try {

		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		string username = cyclOps::NetworkNanny::getUserName();
		CYCLOPSINFO("Current user is %s", username.c_str());
		CommandLinebacker cmd(argc, argv);
		g_boCyclOpsDebug = cmd.isOptionPresent("debug"); 
		cmd.initializeConfigFile(downloadfile::_xmlConfig, true); /* Use exe name as default. */
		cmd.initializeLogFile(downloadfile::_logger);
		downloadfile::main(cmd);
		printf("The command completed successfully.\n");
		return 0;
	} catch (const cyclOps::Exception& e) {
		CYCLOPSERROR_II(downloadfile::_logger, "%s - %s - %s\n", typeid(e).name(), e.what(), e.formatMessage().c_str());
		CYCLOPSERROR_II(downloadfile::_logger, "%s\n", e.what());
		return 1;
	} catch (const std::exception& e) {
		CYCLOPSERROR_II(downloadfile::_logger, "%s - %s\n", typeid(e).name(), e.what());
		CYCLOPSERROR_II(downloadfile::_logger, "%s\n", e.what());
		return 1;
	} catch (...) {
		CYCLOPSERROR_II(downloadfile::_logger, "UNKNOWN EXCEPTION\n");
		return 1;
	}
}

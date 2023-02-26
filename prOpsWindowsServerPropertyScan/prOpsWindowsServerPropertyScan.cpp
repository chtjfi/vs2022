#include "stdafx.h"

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

#include "ExceptionZeroValuedPropertiesNotAllowed.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::vector;
using std::map;

using cyclOps::CommandLinebacker;
using cyclOps::StringEmUp;
using cyclOps::XMLigator;

namespace prOps {

	cyclOps::XMLigator _xmlConfig; 
	cyclOps::ODBCulator _odbc;
	boolean _isLogEnabled = false;
	boolean _boWriteResults;
	string _logFile;
	map<string, string> _mapOfColumnNamesToServerProps;
	cyclOps::TimeTiger::OMIT_T _omitT = cyclOps::TimeTiger::OMIT_T::False;

	void initializeLog(const CommandLinebacker& cmd) {
		_isLogEnabled = cmd.isOptionPresent("log");
		if (_isLogEnabled) {
			_logFile = cmd.getOption("log");
			std::ofstream ofs;
			ofs.open(_logFile, std::ofstream::out | std::ofstream::trunc);
			ofs.close();
		}
	}

	void initializeDatabase() {
		string dsn = _xmlConfig.getTextContent("/root/database/dsn"); CYCLOPSDEBUG("%s", dsn.c_str());
		string user = _xmlConfig.getTextContent("/root/database/user"); CYCLOPSDEBUG("%s", user.c_str());
		string password = _xmlConfig.getTextContentEncryptedWonky("/root/database/password"); CYCLOPSDEBUG("%s", password);
		boolean omitT = _xmlConfig.getTextContentAsBoolean("/root/database/omit_t_from_timestrings");
		if (omitT) {
			_omitT = cyclOps::TimeTiger::OMIT_T::False;
		}
		_odbc.setDSN(dsn);
		_odbc.setUser(user);
		_odbc.setPassword(password);
		_xmlConfig.getChildTextContentMap("/root/database/columnToPropertyMap", _mapOfColumnNamesToServerProps);
	}

	void getServers(vector<whoOps::Server> &listOfServers) {
		string select = _xmlConfig.getTextContent("/root/database/select");
		cyclOps::ODBCResultSet resultSet;
		_odbc.getResultSet(select, resultSet); CYCLOPSDEBUG("getResultSet() complete.");
		const cyclOps::ODBCResultSet::ROW_VECTOR& rowVector = resultSet.getRows();
		for (cyclOps::ODBCResultSet::ROW_ITERATOR rowIterator = rowVector.begin(); rowIterator < rowVector.end(); ++rowIterator) { CYCLOPSDEBUG("Hello.");
			whoOps::Server server((*rowIterator).at("HOSTNAME"));
			listOfServers.push_back(server);
		}
	}

	string getValueForUpdateStatement(const whoOps::Server& server, const string& prop) {
		whoOps::wmi::Variant variant = server.getPropertyAsVariant(prop);
		if (cyclOps::StringEmUp::compareIgnoreCase(prop, "Win32_ComputerSystem.TotalPhysicalMemory") == 0) { CYCLOPSDEBUG("Special case for memory.");
			return variant.getStringValueA();
		} else if (cyclOps::StringEmUp::compareIgnoreCase(prop, "Win32_OperatingSystem.Name") == 0) {
			stringstream ss; ss << "'" << server.getOperatingSystem() << "'";
			return ss.str();
		} else {
			stringstream ss;
			unsigned int iValue = 0;
			if ( (! variant.isTypeSet()) || variant.isNullType()) {
					CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionNoSuchProperty,
						"The server '%s' has no type set for the property '%s'.  Sad!",
						server.getHostname().c_str(), prop.c_str());
			} else {
				switch (variant.getType()) {
				case VT_BSTR: 
					 ss << "'" << variant.getStringValueA() << "'";
					 break;
				case VT_I4:
					iValue = variant.getUintValue();
					ss << iValue;
					break;
				default:
					ss << "Unrecognized type '" << variant.getType() << "' for property '" << prop << "'. " << __FILE__ << "(" << __LINE__ << ")";
					throw std::exception(ss.str().c_str());
				}
				return ss.str();
			}
		}
	}

	void updateDatabase(const whoOps::Server& server) {
		for (map<string, string>::iterator i = _mapOfColumnNamesToServerProps.begin(); i != _mapOfColumnNamesToServerProps.end(); ++i) {
			string column = i->first; CYCLOPSDEBUG("column = %s", column.c_str());
			string prop = i->second; CYCLOPSDEBUG("prop = %s", prop.c_str());
			try {
				string value = prOps::getValueForUpdateStatement(server, prop);
				string format = "UPDATE PROPSSERVERINVENTORY SET ${column} = ${value} WHERE HOSTNAME = '${hostname}'";
				cyclOps::Strung strung;
				string update = strung
					.set("column", column)
					.set("value", value)
					.set("hostname", server.getHostname())
					.format(format); CYCLOPSDEBUG("update = %s", update.c_str());
				cyclOps::ODBCResultSet result;
				_odbc.getResultSet(update, result); CYCLOPSDEBUG("getResultSet() complete.");
			} catch (const whoOps::ExceptionNoSuchProperty& e) {
				CYCLOPSERROR("%s", e.getMessage().c_str());
			}
		}
	}

	void log(const string& line) {
		CYCLOPSINFO("%s", line.c_str());
		if (_isLogEnabled) {
			std::ofstream file;
			file.open(_logFile, std::fstream::app);
			if (file.fail()) {
				CYCLOPSERROR("Failed to open log file %s", _logFile.c_str());
			} else {
				file << line << std::endl;
			}
			file.flush();
			file.close();
		}
	}

	void logSuccessToDatabase(const whoOps::Server& server) {
		cyclOps::Strung strung; CYCLOPSDEBUG("Strung instantiated.");
		cyclOps::TimeTiger time; CYCLOPSDEBUG("TimeTiger instantiated.");
		string update = strung.set("hostname", server.getHostname())
			.set("lastWMIUpdate", time.getISOTimeStringCurrent(_omitT))
			.format("UPDATE PROPSSERVERINVENTORY SET LAST_WMI_UPDATE = '${lastWMIUpdate}' WHERE HOSTNAME = '${hostname}'");
		cyclOps::ODBCResultSet result;
		_odbc.getResultSet(update, result); CYCLOPSDEBUG("getResultSet() complete.");
	}

	void logSuccess(const whoOps::Server& server) {
		stringstream ss;
		cyclOps::TimeTiger time;
		ss << "'" << time.getISOTimeStringCurrent() << "'\t" 
			<< server.getHostname() << "\t" 
			<< "SUCCESS\t" 
			<< server.getModel() << "\t"
			<< server.getOperatingSystem() << "\t" 
			<< server.getMemoryInGigabytes() << "\t" 
			<< server.getCores() << "\t" 
			<< server.getProcessorName();
		prOps::log(ss.str());
		prOps::logSuccessToDatabase(server);
	}
	 
	void logFailure(const whoOps::Server& server, const cyclOps::Exception& e) {
		stringstream ss;
		cyclOps::TimeTiger time;
		ss << time.getISOTimeStringCurrent() << "\t" << server.getHostname() << "\tFAILURE\t" << typeid(e).name() << " " << e.what() << " " << e.formatMessage();

		prOps::log(ss.str());
	}

	void addDiskToDatabase(const whoOps::LogicalDisk& disk) { CYCLOPSDEBUG("Hello.");
		cyclOps::Strung strung; CYCLOPSDEBUG("Strung instantiated.");
		cyclOps::TimeTiger time; CYCLOPSDEBUG("TimeTiger instantiated.");
		strung.set("hostname", disk.getHostname())
			.set("deviceID", disk.getDeviceID())
			.set1000("driveType", "%u", disk.getDriveType())
			.set("sizeInBytes", disk.getSizeInBytes())
			.set("freeSpaceInBytes", disk.getFreeSpaceInBytes())
			.set("lastUpdate", time.getISOTimeStringCurrent(_omitT)); CYCLOPSDEBUG("strung is dung.");
		string insert = strung.format(
			"INSERT INTO PROPSDISKINVENTORY "
			"(HOSTNAME, Win32_LogicalDisk_DeviceID, Win32_LogicalDisk_Size, Win32_LogicalDisk_FreeSpace, " 
			" Win32_LogicalDisk_DriveType, LASTUPDATE) "
			"VALUES('${hostname}', '${deviceID}', ${sizeInBytes}, ${freeSpaceInBytes}, ${driveType}, '${lastUpdate}')"); CYCLOPSDEBUG("insert = %s", insert.c_str());
		cyclOps::ODBCResultSet resultSet;
		_odbc.getResultSet(insert, resultSet); CYCLOPSDEBUG("getResultSet() done.");
	}

	void deleteAllDisksForHost(const  whoOps::Server& server) {
		stringstream ss; ss << "DELETE FROM PROPSDISKINVENTORY WHERE HOSTNAME = '" << server.getHostname() << "'";
		cyclOps::ODBCResultSet resultSet;
		prOps::_odbc.getResultSet(ss.str(), resultSet);
	}

	void addDisksToDatabase(const whoOps::Server& server) {
		vector<whoOps::LogicalDisk> disks;
		server.getLogicalDisks(disks);
		prOps::deleteAllDisksForHost(server);
		for (int i = 0; i < disks.size(); ++i) {
			whoOps::LogicalDisk disk = disks[i]; CYCLOPSDEBUG("%s/%u/%u/%u\n", disk.getDeviceID().c_str(), disk.getDriveType(), disk.getSizeInGB(), disk.getFreeSpaceInGB());
			prOps::addDiskToDatabase(disk);
		}
	}

	void main(const CommandLinebacker& cmd) {
		_boWriteResults = ! cmd.isOptionPresent("no_db_write");
		vector<whoOps::Server> listOfServers; prOps::getServers(listOfServers); CYCLOPSINFO("%d servers returned.", listOfServers.size());
		for (int i = 0; i < listOfServers.size(); ++i) { CYCLOPSDEBUG("%s", listOfServers[i].getHostname().c_str());
			whoOps::Server server = listOfServers[i];
			whoOps::wmi::WMIAmTheWalrus wmi;
			try {
				wmi.populateAllServerProperties(server); CYCLOPSDEBUG("%s", server.getProcessorName().c_str());
				prOps::updateDatabase(server);
				prOps::addDisksToDatabase(server);
				prOps::logSuccess(server);
			} catch (const cyclOps::Exception& e) {
				CYCLOPSERROR("%s - %s - %s\n", typeid(e).name(), e.what(), e.formatMessage().c_str());
				prOps::logFailure(server, e);
			} catch (...) {
				CYCLOPSERROR("Exception caught (and being rethrown) for server %s.", server.getHostname().c_str());
				throw;
			}	
		}

	}

}

int main(int argc, char* argv[])
{
	try {
		CommandLinebacker cmd(argc, argv);
		g_boCyclOpsDebug = cmd.isOptionPresent("debug"); 
		cmd.initializeConfigFile(prOps::_xmlConfig);
		prOps::initializeLog(cmd);
		prOps::initializeDatabase();
		whoOps::wmi::WMIAmTheWalrus::initialize();
		prOps::main(cmd);
		printf("The command completed successfully.\n");
		// ::MessageBoxA(NULL, "Done.", "Done.", MB_OK);
		return 0;
	} catch (const cyclOps::Exception& e) {
		char sz[50]; _snprintf_s(sz, CYCLOPSSIZEOF(sz), _TRUNCATE, "%%s - %%s - %%s - %s \n", 
			e.getType() == cyclOps::Exception::TYPE_HRESULT ? "0x%08x" : "%d"); CYCLOPSDEBUG("sz = %s", sz);
		
		CYCLOPSERROR(sz  , typeid(e).name(), e.what(), e.formatMessage().c_str(), e.getLastError());
		return 1;
	} catch (const std::exception& e) {
		CYCLOPSERROR("%s - %s\n", typeid(e).name(), e.what());
		return 1;
	} catch (...) {
		CYCLOPSERROR("UNKNOWN EXCEPTION\n");
		return 1;
	}
}

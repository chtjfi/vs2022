#include "StdAfx.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "cyclOps.h"

#include "CommandLinebacker.h"

using std::string;
using std::wstring;
using std::vector;
using std::stringstream;

namespace cyclOps {

	const char* const CommandLinebacker::OPTION_CONFIG = "config";
	const char* const CommandLinebacker::OPTION_LOG = "log";

	CommandLinebacker::CommandLinebacker(int argc, char* argv[]) 
		: _argc(argc), _argv(argv)
	{
	}


	CommandLinebacker::~CommandLinebacker(void)
	{
	}

	int CommandLinebacker::getInt(char* szOption, int iDefault) const { 
		int iReturn = iDefault;
		try {
			iReturn = this->getInt(szOption);
		} catch (const cyclOps::ExceptionOptionNotSet& e) {
			CYCLOPSDEBUG("%s - %s", typeid(e).name(), e.what());
		} 
		return iReturn;
	}

	int CommandLinebacker::getInt(char* szOption) const {
		string strValue = this->getOption(szOption); 
		try {
			return std::stoi(strValue);
		} catch (const std::invalid_argument& e) { e;
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionInvalidCommandLineOptions, "The value '%s' supplied for option '%s' is not a valid integer.", strValue, szOption);
		}  
	}

	bool CommandLinebacker::isOptionPresent(const char* szOption) const {
		try {
			/* First see if option is present with colon. */
			string throwaway = this->getOption(szOption);
			return true;
		} catch (const cyclOps::ExceptionOptionNotSet& ignore) { 
			ignore;
			/* Not present with colon, try without. */
			return getOption(szOption, NULL, 0);
		}
	}

	bool CommandLinebacker::getOption(const char* pchOption, char* szValue, int iValueSize) const	{

		// Create the full option string.  For example, if pchOption points at "booble", then 
		// the full option string is "/booble".  A trailing colon will
		// be appended depending on if pchValue is NULL or not.  If it is NULL, no return value
		// is being sought and so no colon is used to separate option from value.  However, if pchValue
		// is not NULL, then we append a colon, because it separates the option from the value.  OK?
		char szFullOption[500];
		sprintf_s(szFullOption, CYCLOPSSIZEOF(szFullOption), "/%s%s", pchOption, 
			szValue == NULL ? "" : ":");

		// Make the option lowercase so we can match the strlwr'ed arguments.
		_strlwr_s(szFullOption, CYCLOPSSIZEOF(szFullOption));

		// Loop through the command line arguments looking for the option.
		for (int i = 0; i < _argc; ++i) {
			// Because we are going to strlwr the string, I feel more comfortable working on 
			// a copy of it, which is what we create here...
			char szArgvI[5000];
			size_t iSize = CYCLOPSSIZEOF(szArgvI);
			sprintf_s(szArgvI, iSize, "%s", _argv[i]);
			_strlwr_s(szArgvI, CYCLOPSSIZEOF(szArgvI));
			// If szValue was NULL, szFullOption must match szArgvI exactly.
			if (szValue) {
				// If szValue was not NULL, szArgvI should *begin* with szFullOption for 
				// this to be a match, because we are expecting a return value.
				char* pchLocation = strstr(szArgvI, szFullOption);
				if (pchLocation == szArgvI) {
					// At this point we know szValue cannot be NULL because we checked for that
					// a few lines back.  So a value is expected to be returned.
					// We want to use the original argv[i] because szArgvI has been strlwr'ed
					// and maybe we need the real case of the argument?
					char* pchColon = strstr(_argv[i], ":");
					sprintf_s(szValue, iValueSize, "%s", pchColon + 1);
					return true;
				}
			} else {
				// Otherwise, szValue was NULL, no return value is expected, and we return tru
				// only if there is an EXACT match between szArgvI and szFullOption.
				if (_stricmp(szArgvI, szFullOption) == 0) {
					return true;
				} 
			}
		}
		return false;
	}
	
	string CommandLinebacker::getOption(const char* pchOption) const {
		char szValue[5000];
		if ( ! this->getOption(pchOption, szValue, CYCLOPSSIZEOF(szValue))) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionOptionNotSet, "Command line argument '%s' not found.", pchOption);
		}
		return std::string(szValue);
	}

	wstring CommandLinebacker::getOptionW(const string& strOption) const {
		string strValue = this->getOption(strOption.c_str());
		return wstring(strValue.begin(), strValue.end());
	}

	string CommandLinebacker::getOption(char* pchOption, const string& strDefault) const { CYCLOPSDEBUG("Hello");
		char szValue[5000];
		if ( ! this->getOption(pchOption, szValue, CYCLOPSSIZEOF(szValue))) {
			return strDefault;
		}
		return std::string(szValue);
	}

	string CommandLinebacker::getOptionInPosition(const int& iPosition) const {
		/* Position 0 is the executable. */
		if (this->_argc < (iPosition + 1)) {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionOptionNotSet, "There is no option in position %d", iPosition);
		} else {
			return this->_argv[iPosition];
		}
	}

	int CommandLinebacker::getIntInPosition(const int& iPosition) {
		string strValue = this->getOptionInPosition(iPosition);
		try {
			int i = stoi(strValue);
			return i;
		} catch (const std::invalid_argument& e) { e;
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionInvalidCommandLineOptions, "The value '%s' in position %d is not an integer.", strValue, iPosition);
		}
	}

	void CommandLinebacker::getOption(const string& option, string& value) const { CYCLOPSDEBUG("option = %s", option);
		char szValue[5000];
		bool boResult = this->getOption(option.c_str(), szValue, (sizeof(szValue) / sizeof(szValue[0])) - 1); 
		if (boResult) {		CYCLOPSDEBUG("szValue = %s", szValue);
			value.assign(szValue); CYCLOPSDEBUG("value = %s", value.c_str());
		} else {
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionOptionNotSet, "The option /%s is not set.", option.c_str());
		}
	}

	void CommandLinebacker::getFileNameFromExecutable(string& value, const string& extension) const { CYCLOPSDEBUG("argv[0] = %s", _argv[0]);
		Pathiosocic path(this->_argv[0]);
		string drive = path.getDrive();		CYCLOPSDEBUG("drive = '%s'", drive.c_str());
		string dir = path.getDir();			CYCLOPSDEBUG("dir = %s", dir.c_str());
		string file = path.getFname();		CYCLOPSDEBUG("file = %s", file.c_str());
		//stringstream ss;
		if ( ! drive.empty()) {
			drive += "\\";
			value.append(drive);
		}
		if ( ! dir.empty()) {
			value.append(dir + "\\");
		}
		value.append(file + extension);	CYCLOPSDEBUG("value = %s", value.c_str());
	}

	void CommandLinebacker::makeLognameUnique(string& logname) const {
		cyclOps::Pathiosocic path(logname);
		string user = NetworkNanny::getUserName();
		TimeTiger tiger;
		string iso = tiger.getISOTimeStringCurrentForFilename();
		stringstream newname; newname << path.getFname() << "_" << user << "_" << iso << path.getExt();
		logname.assign(newname.str());
	}

	void CommandLinebacker::initializeLogFile(cyclOps::Logger& log, MakeLognameUnique makeLognameUnique) const {
		string strLog;
		try {
			this->getOption(CommandLinebacker::OPTION_LOG, strLog);
		} catch (const cyclOps::ExceptionOptionNotSet ignore) { 
			this->getFileNameFromExecutable(strLog, ".log");
			if (makeLognameUnique == MakeLognameUnique::True) {
				this->makeLognameUnique(strLog);
			}
		} CYCLOPSDEBUG("strLog = %s", strLog.c_str());
		log.initializeLogFile(strLog);
	
	}

	void CommandLinebacker::initializeConfigFile(
		cyclOps::XMLigator& xml, 
		boolean useExecutableNameAsDefault, 
		boolean useArgumentInPositionOne ) const 
	{
		string strConfig;
		try {
			this->getOption(CommandLinebacker::OPTION_CONFIG, strConfig); CYCLOPSDEBUG("/config option found.");
		} catch (const cyclOps::ExceptionOptionNotSet ignore) { CYCLOPSDEBUG("/config option not present.");
			if (useArgumentInPositionOne) {
				try {
					strConfig.assign(this->getOptionInPosition(1));
				} catch (const cyclOps::ExceptionOptionNotSet ignore) { 
					CYCLOPSDEBUG("No argument in position 1."); 
				}
			} else if (useExecutableNameAsDefault) {
				this->getFileNameFromExecutable(strConfig, ".xml");
			} else {
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionOptionNotSet, "The mandatory option /config is not set.");
			}
		} CYCLOPSDEBUG("strConfig = %s", strConfig.c_str());
		xml.initializeFromFilename(strConfig);
	}

	string CommandLinebacker::getExecutablePath() const {
		return this->_argv[0];
	}

	string CommandLinebacker::getExecutableFilename() const {
		char szDrive[5000];
		char szDir[5000];
		char szFname[5000];
		char szExt[5000];
		_splitpath_s(
			this->_argv[0], 
			szDrive, CYCLOPSSIZEOF(szDrive),
			szDir, CYCLOPSSIZEOF(szDir),
			szFname, CYCLOPSSIZEOF(szFname),
			szExt, CYCLOPSSIZEOF(szExt));
		stringstream ssFilename;
		ssFilename << szFname << szExt;
		return ssFilename.str();
	}

	void CommandLinebacker::getSTDIN(vector<string>& vectorOfSTDIN) const {
		std::string line;
		int lineNumber = 1;
		while (std::getline(std::cin, line)) {
		  cyclOps::StringEmUp::trim(line);
		  vectorOfSTDIN.push_back(line);
		  ++lineNumber;
		}	

	}
}
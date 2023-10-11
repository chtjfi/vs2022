#pragma once

#include <string>

#include "cyclOps.h"

namespace cyclOps {
	class CommandLinebacker
	{
	public:
		CommandLinebacker(int argc, char* argv[]);
		~CommandLinebacker(void);

		enum MakeLognameUnique { True, False };

		static const char* const OPTION_CONFIG;
		static const char* const OPTION_LOG;

		std::string getExecutablePath() const;
		std::string getExecutableFilename() const;
		int getInt(char* szOption, int iDefault) const;
		int getInt(char* szOption) const;
		int getIntInPosition(const int& iPosition);

		bool			getOption(const char* pchOption, char* szValue, int iValueSize) const;
		std::string		getOption(const char* pchOption) const;
		std::string		getOption(const std::string& strOption) const;
		/* void			getOption(const std::string& option, std::string& value) const;  //Still in .cpp if you need it. */
		std::string		getOptionWithDefault(const char* pchOption, const std::string& strDefault) const;

		std::wstring	getOptionW(const std::string& s) const;
		std::string getOptionInPosition(const int& iPosition) const;
		bool isOptionPresent(const char* szOption) const;
		void initializeConfigFile(cyclOps::XMLigator& xml, boolean useExecutableNameAsDefault = true, 
			boolean useArgumentInPositionOne = false) const;
		
		void getFileNameFromExecutable(std::string& value, const std::string& extension) const;
		void initializeLogFile(cyclOps::Logger& log, MakeLognameUnique makeLognameUnique = False) const;
		std::string getArgv(int i) const { return _argv[i]; }
		std::wstring getArgvW(int i) const { 
			std::string strArgv = _argv[i];
			return std::wstring(strArgv.begin(), strArgv.end());
		}
		int getArgc(void) const { return _argc; }
		void getSTDIN(std::vector<std::string>& vectorOfSTDIN) const;
	private:
		int _argc;
		char** _argv;
		/* Make copy constructor and assignment operator private and don't define
		   because we have a pointer member. */
		CommandLinebacker(const CommandLinebacker& cmd); 
		CommandLinebacker& operator=(const CommandLinebacker& cmd);
		void makeLognameUnique(std::string& logname) const;
	};
}
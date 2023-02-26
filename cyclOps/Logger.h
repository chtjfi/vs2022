#pragma once

#include <string>
#include <fstream>

namespace cyclOps {
	class Logger
	{
	private:
		std::string m_strLogFile;
		std::ofstream m_logFile;
		static const char* MAGIC_WORD_NOT_INITIALIZED;
		std::string packageVariableArguments(char* szFormat, va_list vaList);
		std::string getTimeString();
		bool _boDebug;
	public:
		Logger();
		~Logger(void);
		void setDebug(const bool& boDebug) { _boDebug = boDebug; }
		void log(char* szFormat, ...);
		void logWithoutTimestamp(char* szFormat, ...);
		void info(char* szFormat, ...);
		void warn(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void debug(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void error(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void fatal(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void raw(char* szOutput, size_t iSize);
		void logAs(char* szLogLevel, char* szFile, char* szFunction, int iLine, char* szFormat, va_list vaList);
		void initializeLogFile(const std::string& strLogFile);
		bool isLogFileInitialized(void);
		void getLogFileContents(std::string& contents);
		std::string getLogFileName(void);
	};
}
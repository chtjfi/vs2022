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
		std::string packageVariableArguments(const char* szFormat, va_list vaList);
		std::string getTimeString();
		bool _boDebug;
	public:
		Logger();
		~Logger(void);
		void setDebug(const bool& boDebug) { _boDebug = boDebug; }
		void log(const char* szFormat, ...);
		void logWithoutTimestamp(char* szFormat, ...);
		void info(const char* szFormat, ...);
		void warn(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void debug(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void error(const char* szFile, const char* szFunction, int iLine, const char* szFormat, ...);
		void fatal(char* szFile, char* szFunction, int iLine, char* szFormat, ...);
		void raw(const char* szOutput, size_t iSize);
		void logAs(const char* szLogLevel, const char* szFile, const char* szFunction, int iLine, const char* szFormat, va_list vaList);
		void initializeLogFile(const std::string& strLogFile);
		bool isLogFileInitialized(void);
		void getLogFileContents(std::string& contents);
		std::string getLogFileName(void);
	};
}
#include "StdAfx.h"

#include <stdio.h>
#include <stdarg.h>
/* Because cyclOps.h includes WinSock2.h, you cannot include Windows.h here.  See cyclOps.h for details. */
/* #include <Windows.h> */
#include <iostream>

#include "cyclOps.h"

#include "Logger.h"

using std::string;

cyclOps::Logger::Logger() :	m_strLogFile(cyclOps::Logger::MAGIC_WORD_NOT_INITIALIZED), _boDebug(false) {
}

const char* cyclOps::Logger::MAGIC_WORD_NOT_INITIALIZED = "NOT_INITIALIZED";

void cyclOps::Logger::initializeLogFile(const string& strLogFile)
{
	/* You might try to initialize a Logger with the log file name (string) of another Logger.
	   However, that Logger might not have been initialized, and the string returned by 
	   getLogFileName() will be MAGIC_WORD_NOT_INITIALIZED, which will have been passed to 
	   us just now.  So we check if that is the case. */
	if (strLogFile.compare(cyclOps::Logger::MAGIC_WORD_NOT_INITIALIZED) == 0) { CYCLOPSDEBUG("Log not initialized.");
		return;
	}
	m_strLogFile.assign(strLogFile); 
	m_logFile.open(m_strLogFile, std::ios::app); //std::ios::out || 
	if (m_logFile.fail()) {
		char error[5000];
		strerror_s(error, CYCLOPSSIZEOF(error), errno);
		CYCLOPS_THROW_EXCEPTION_IV(ExceptionUnableToOpenFileII, "The file '%s' cannot be opened. [%s]", m_strLogFile.c_str(), error);
	}

	m_logFile << "";
	m_logFile.close(); /* printf("Goodbye."); */
}


cyclOps::Logger::~Logger(void)
{
}

void cyclOps::Logger::debug(char* szFile, char* szFunction, int iLine, char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	this->logAs("DEBUG", szFile, szFunction, iLine, szFormat, vaArguments);
}

void cyclOps::Logger::logAs(const char* szLogLevel, const char* szFile, const char* szFunction, int iLine, const char* szFormat, va_list vaList) {
	std::string strOutput = this->packageVariableArguments(szFormat, vaList);
	this->log("%s: [%s(%s:%d)] %s", szLogLevel, szFunction, szFile, iLine, strOutput.c_str());
}

void cyclOps::Logger::warn(char* szFile, char* szFunction, int iLine, char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	this->logAs("WARNING", szFile, szFunction, iLine, szFormat, vaArguments);
}

void cyclOps::Logger::error(const char* szFile, const char* szFunction, int iLine, const char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	this->logAs("ERROR", szFile, szFunction, iLine, szFormat, vaArguments);
}

void cyclOps::Logger::fatal(char* szFile, char* szFunction, int iLine, char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	this->logAs("FATAL", szFile, szFunction, iLine, szFormat, vaArguments);
}

std::string cyclOps::Logger::packageVariableArguments(const char* szFormat, va_list vaList) {
	char szOutput[10000];
	_vsnprintf_s(szOutput, sizeof(szOutput) / sizeof(szOutput[0]), _TRUNCATE, szFormat, vaList);
	return std::string(szOutput);
}

void cyclOps::Logger::info(const char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	std::string strOutput = this->packageVariableArguments(szFormat, vaArguments);
	this->log("INFO: %s", strOutput.c_str());
}

void cyclOps::Logger::log(const char* szFormat, ...)
{
	va_list vaArguments; 
	va_start(vaArguments, szFormat); 
	char szOutput[10000]; 
	_vsnprintf_s(szOutput, sizeof(szOutput) / sizeof(szOutput[0]), _TRUNCATE, szFormat, vaArguments); 
	std::string strTime = this->getTimeString(); 
	printf("%s %s\n", strTime.c_str(), szOutput); 
	if (this->isLogFileInitialized()) { 
		m_logFile.open(m_strLogFile, std::ios::out | std::ios::app ); 
		m_logFile << strTime << " " << szOutput << "\n"; 
		m_logFile.close(); 
	} 
}

void cyclOps::Logger::logWithoutTimestamp(char* szFormat, ...)
{
	va_list vaArguments; 
	va_start(vaArguments, szFormat); 
	char szOutput[10000]; 
	_vsnprintf_s(szOutput, sizeof(szOutput) / sizeof(szOutput[0]), _TRUNCATE, szFormat, vaArguments); 
	printf("%s\n", szOutput); 
	if (this->isLogFileInitialized()) { 
		m_logFile.open(m_strLogFile, std::ios::out | std::ios::app ); 
		m_logFile << szOutput << "\n"; 
		m_logFile.close(); 
	} 
}


void cyclOps::Logger::raw(const char* szOutput, size_t iSize)
{
	/* This guy is useful for outputting the stdout from a process launched with ProcessPrincess. 
	   It omits timestamps and newlines and writes to the file in binary mode. */
	printf("%s", szOutput);
	if (this->isLogFileInitialized()) {
		m_logFile.open(m_strLogFile, std::ios::out | std::ios::app | std::ios::binary);
		m_logFile.write(szOutput, iSize);
		m_logFile.close();
	}
}




bool cyclOps::Logger::isLogFileInitialized(void)
{	
	return this->m_strLogFile.compare(cyclOps::Logger::MAGIC_WORD_NOT_INITIALIZED) != 0;
}

std::string cyclOps::Logger::getTimeString() {
	SYSTEMTIME systemtime;
	::GetLocalTime(&systemtime);
	char szTime[100];
	_snprintf_s(szTime, sizeof(szTime) / sizeof(szTime[0]), _TRUNCATE, "%i-%02i-%02iT%02i:%02i:%02i", 
		systemtime.wYear, 
		systemtime.wMonth, 
		systemtime.wDay, 
		systemtime.wHour, 
		systemtime.wMinute, 
		systemtime.wSecond);
	return std::string(szTime);
}

std::string cyclOps::Logger::getLogFileName() {
	return this->m_strLogFile;
}

void cyclOps::Logger::getLogFileContents(string& contents) {
	if (this->isLogFileInitialized()) { 
		std::ifstream file(this->m_strLogFile);
		if (file.fail()) {
			std::stringstream msg; msg << "Unable to open log file " << this->m_strLogFile;
			CYCLOPS_THROW_EXCEPTION_IV(cyclOps::ExceptionUnableToOpenFileII, "%s", msg.str().c_str());
		}
		for (string line; std::getline(file, line ); ) { CYCLOPSDEBUG("line = %s", line.c_str());
			contents.append(line + "\n");
		}
		file.close();
	} 
}
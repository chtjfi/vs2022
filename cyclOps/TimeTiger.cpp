#include "StdAfx.h"

#include <stdio.h>
#include <regex>
#include <iostream>
#include <ctime>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "cyclOps.h"
#include "TimeTiger.h"
#include "StringEmUp.h"


using std::string;
using std::stringstream;

namespace cyclOps {
	TimeTiger::TimeTiger(void)
	{
		::GetSystemTime(&_systemtime); 
		time(&_now);

	}


	TimeTiger::~TimeTiger(void)
	{
	}

	::clock_t TimeTiger::getDifferenceInMilliseconds(const ::clock_t begin, const ::clock_t end) const {
		/* Have to 
			#include <time.h> 
		   Get a clock_t like this: 
			clock_t t = clock(); */
		::clock_t diff = end - begin; CYCLOPSVAR(diff, "%d");
		return diff;
	}

	bool TimeTiger::isNowLaterThan(const std::string& iso8601) { CYCLOPSDEBUG("iso8601 = %s", iso8601.c_str());
		std::time_t when = this->get_time_tFromISO8601(iso8601); CYCLOPSVAR(when, "%d");
		std::time_t now = std::time(0);   CYCLOPSVAR(now, "%d");
		return now > when;
	}

	bool TimeTiger::isNowLaterThanTimePlusXHours(const std::string& iso8601, int hours) {
		time_t timeToCompareWith = this->get_time_tFromISO8601(iso8601) + (hours * 3600);
		time_t differenceInSeconds = this->_now - timeToCompareWith;
		return differenceInSeconds > 0;

	}

	bool TimeTiger::isNowEarlierThan(const std::string& iso8601) {
		std::time_t when = this->get_time_tFromISO8601(iso8601);
		std::time_t now = std::time(0);   // get time now
		return now < when;
	}


	string TimeTiger::variantTimeToISO8601(const DATE& variantTime) {
		SYSTEMTIME systemTime;
		::VariantTimeToSystemTime(variantTime, &systemTime);
		char iso8601[100];
		TimeTiger::systemtimeToISO8601(systemTime, iso8601, CYCLOPSSIZEOF(iso8601));
		return iso8601;
	}

	void TimeTiger::systemtimeToISO8601(const SYSTEMTIME& systemtime, char* szISO8601, int iLength, OMIT_T omitT,
		char timeSeparator)
	{
		_snprintf_s(szISO8601, iLength, _TRUNCATE, "%i-%02i-%02i%s%02i%c%02i%c%02i", 
			systemtime.wYear, 
			systemtime.wMonth, 
			systemtime.wDay, 
			omitT == False ? "T" : " ",
			systemtime.wHour, 
			timeSeparator,
			systemtime.wMinute, 
			timeSeparator,
			systemtime.wSecond);
	}






	string TimeTiger::getTimeString(void) const {
		char szTime[100];
		cyclOps::TimeTiger::systemtimeToISO8601(_systemtime, szTime, CYCLOPSSIZEOF(szTime));
		return std::string(szTime);
	}

	string TimeTiger::getTimeString(const string& format) const {
		time_t now;
		time(&now);
		return this->time_tToString(now, format.c_str());
	}

	int TimeTiger::getLastMonthsYear(void) { 
		return _systemtime.wMonth > 1 ? _systemtime.wYear : _systemtime.wYear - 1;
	}

	int TimeTiger::getLastMonthsMonth(void) {
		return _systemtime.wMonth > 1 ? _systemtime.wMonth - 1 : 12;
	}


	string TimeTiger::getFirstDayOfLastMonthISO8601(void) {
		char szDate[11];
		_snprintf_s(szDate, sizeof(szDate) / sizeof(szDate[0]), _TRUNCATE, "%04d-%02d-01",
			this->getLastMonthsYear(), this->getLastMonthsMonth());
		return szDate;
	}

	string TimeTiger::getLastDayOfLastMonthISO8601(void) {
		int iLastDayOflastMonth = this->getLastDayOfMonth(
			this->getLastMonthsYear(), this->getLastMonthsMonth()); CYCLOPSVAR(iLastDayOflastMonth, "%d");
		char szDate[11];
		_snprintf_s(szDate, sizeof(szDate) / sizeof(szDate[0]), _TRUNCATE, "%04d-%02d-%02d",
			this->getLastMonthsYear(), this->getLastMonthsMonth(), iLastDayOflastMonth);
		return szDate;
	}

	int TimeTiger::getLastDayOfMonth(int year, int month) { CYCLOPSDEBUG("month/year = %d/%d", month, year);
		int ndays[] = {-1,31,28,31,30,31,30,31,31,30,31,30,31};
		int ret = ndays[month]; CYCLOPSVAR(ret, "%d");
		if (this->isLeapYear(year) && month == 2) {
			ret++;
		} 
		return ret;
	}

	bool TimeTiger::isLeapYear(int year) {
		if (year % 4 != 0) {
			return false;
		}
		if (year % 400 == 0) {
			return true;
		}
		if(year % 100 == 0) {
			/* The 400-divisible years have already returned true, so this is ok. */
			return false;
		}
		/* Year is divisible by 4 but not by 100. */
		return true;
	}

	int TimeTiger::getYearFromISOTimeAsInt(const string& strISOTime) { 
		return this->getFromISOTimeAsInt(strISOTime, "year", 0, 4);
	}

	int TimeTiger::getFromISOTimeAsInt(const string& strISOTime, const string& strPartSought, size_t iFirstPosition, int iLength) {
		/* This accepts partial ISO timestrings. Needed for SUDZ. */
		if (strISOTime.length() < iFirstPosition + iLength) {
			string strWhat = StringEmUp::format("The string %s does not contain enough characters to contain a %s.", strISOTime.c_str(), strPartSought.c_str());
			CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionInvalidTimeString, strWhat);
		}
		string strPartFound = strISOTime.substr(iFirstPosition, iLength);
		string strRegex; 
		for (int i = 0; i < iLength; ++i) {
			strRegex += "\\d";
		}
		std::regex rgxDigits(strRegex); 
		if ( ! std::regex_match(strPartFound, rgxDigits) ) {
			string strWhat = StringEmUp::format("The string %s is not a valid %s.", strPartFound.c_str(), strPartSought.c_str());
			CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionInvalidTimeString, strWhat);
		}
		try {
			return std::stoi(strPartFound);
		} catch (const std::invalid_argument& e) {
			string strWhat = StringEmUp::format("The string %s is not a valid %s. [%s - %s]", strPartFound.c_str(), strPartSought.c_str(), typeid(e).name(), e.what());
			CYCLOPS_THROW_EXCEPTION_III(cyclOps::ExceptionInvalidTimeString, strWhat);
		}
	}

	int TimeTiger::getMonthFromISOTimeAsInt(const string& strISOTime) {
		return this->getFromISOTimeAsInt(strISOTime, "month", 5, 2);
	}

	time_t TimeTiger::getDifferenceInDaysFromISO8601(const std::string& strISO8601) const {
		time_t hours = this->getDifferenceInHoursFromISO8601(strISO8601);
		return hours / 24;
	}

	time_t TimeTiger::getDifferenceInHoursFromISO8601(const std::string& strISO8601) const {
		time_t minutes = this->getDifferenceInMinutesFromISO8601(strISO8601);
		return minutes / 60;
	}

	time_t TimeTiger::getDifferenceInMinutesFromISO8601(const std::string& strISO8601) const {
		time_t seconds = this->getDifferenceInSecondsFromISO8601(strISO8601);
		return seconds / 60;
	}


	time_t TimeTiger::getDifferenceInSecondsFromISO8601(const std::string& strISO8601) const {
		time_t tt = this->get_time_tFromISO8601(strISO8601);
		time_t diff = this->_now - tt;
		return diff;
	}


	bool TimeTiger::isISO8601(const std::string& time) {
		try {
			boost::posix_time::time_from_string(time);
			return true;
		} catch (std::exception e) {
			return false;
		}
		

	}

	time_t TimeTiger::get_time_tFromISO8601(const string& strISO8601) const {
		string strModified = strISO8601;
		char chars[] = "-.:";
		for (size_t i = 0; i < strlen(chars); ++i) {
			StringEmUp::removeAll(strModified, chars[i]);
		} CYCLOPSDEBUG("strModified = %s", strModified.c_str());
		/* You maybe should have used time_from_string which doesn't require the reformatting. */
		boost::posix_time::ptime t(boost::posix_time::from_iso_string(strModified)); CYCLOPSDEBUG("t = %p", &t);
		::tm pTM = to_tm( t );
		return ::mktime(&pTM); 
	}

	string TimeTiger::getISO8601From_time_t(const time_t& t) {
		char buff[20];
		tm tm;
		localtime_s(&tm, &t);
		strftime(buff, 20, "%Y-%m-%dT%H:%M:%S", &tm);
		return buff;
	}

	string TimeTiger::getISOTimeStringCurrentForFilename(void) {
		return this->getISOTimeStringCurrent(OMIT_T::False, '.');
	}

	string TimeTiger::getISOTimeStringCurrent(OMIT_T omitT, char timeSeparator) {
		SYSTEMTIME systemtime;
		::GetLocalTime(&systemtime);
		char szTime[100];
		TimeTiger::systemtimeToISO8601(systemtime, szTime, sizeof(szTime) / sizeof(szTime[0]), omitT, timeSeparator);
		return szTime;
	}

	string TimeTiger::getISODateStringCurrent(void) {
		::SYSTEMTIME systemtime;
		::GetSystemTime(&systemtime);
		char szDate[15];
		_snprintf_s(szDate, sizeof(szDate) / sizeof(szDate[0]), _TRUNCATE, "%i-%02i-%02i", 
			systemtime.wYear, 
			systemtime.wMonth, 
			systemtime.wDay);
		return szDate;
	}


	string TimeTiger::time_tToString(const time_t& timeT, const char* format) const {
		tm tm;
		::localtime_s(&tm, &timeT);
		return this->tmToString(tm, format);
	}

	string TimeTiger::tmToString(const tm& x, const char* format) const {
		/* From C++ Kochbuch p222. */
		/* ISO8601 = "%Y-%m-%dT%H:%M:%S" */
		stringstream ss;
		const std::time_put<char>& dateWriter = std::use_facet<std::time_put<char>>(ss.getloc());
		size_t n = strlen(format);
		if (dateWriter.put(ss, ss, ' ', &x, format, format + n).failed()) {
			throw std::runtime_error("Unable to format time.");
		}
		return ss.str();
	}
}
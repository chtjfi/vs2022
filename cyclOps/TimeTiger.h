#pragma once

#include <string>

#include <Windows.h>

#include <time.h>

namespace cyclOps {
	class TimeTiger
	{
	public:
		explicit TimeTiger(void);
		~TimeTiger(void);
		enum OMIT_T { True, False };
		static void systemtimeToISO8601(const SYSTEMTIME& systemtime, char* szISO8601, int iLength, 
			OMIT_T omitT = False, char timeSeparator = ':');
		std::string getTimeString(void) const;
		std::string getTimeString(const std::string& format) const;
		std::string getFirstDayOfLastMonthISO8601(void);
		std::string getLastDayOfLastMonthISO8601(void);
		bool isNowLaterThan(const std::string& iso8601);
		bool isNowLaterThanTimePlusXHours(const std::string& iso8601, int hours);
		bool isNowEarlierThan(const std::string& iso8601);
		int getLastMonthsYear(void);
		int getLastMonthsMonth(void);
		bool isLeapYear(int year);
		int getLastDayOfMonth(int year, int month);
		int getYearFromISOTimeAsInt(const std::string& strISOTime);
		int getMonthFromISOTimeAsInt(const std::string& strISOTime);
		int getFromISOTimeAsInt(const std::string& strISOTime, const std::string& strPartSought, size_t iFirstPosition, int iLength);
		time_t getDifferenceInDaysFromISO8601(const std::string& strISO8601) const;
		time_t getDifferenceInHoursFromISO8601(const std::string& strISO8601) const;
		time_t getDifferenceInSecondsFromISO8601(const std::string& strISO8601) const;
		time_t getDifferenceInMinutesFromISO8601(const std::string& strISO8601) const;
		::clock_t getDifferenceInMilliseconds(const ::clock_t begin, const ::clock_t end) const;
		std::string getISO8601From_time_t(const time_t& t);
		std::string getISOTimeStringCurrent(OMIT_T omitT = False, char timeSeparator = ':');
		std::string getISOTimeStringCurrentForFilename(void);
		std::string getISODateStringCurrent(void);
		time_t get_time_tFromISO8601(const std::string& strISO8601) const;
		std::string variantTimeToISO8601(const DATE& variantTime);
		std::string tmToString(const tm& x, const char* format) const;
		std::string time_tToString(const time_t& timeT, const char* format) const;
		bool isISO8601(const std::string& time);
	private:
		TimeTiger(const TimeTiger& t);
		TimeTiger& operator=(const TimeTiger& t);
		SYSTEMTIME _systemtime;
		time_t _now;
		

	};
}

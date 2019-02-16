/*
 * TimeManager.h
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */

#ifndef TIMEMANAGER_H_
#define TIMEMANAGER_H_
#include <Arduino.h>
#include <RTClib.h>
#include <GeneralFunctions.h>

class TimeManager{
	HardwareSerial& _HardSerial;
	GeneralFunctions& generalFunctions;

	public:
		TimeManager(GeneralFunctions& g, HardwareSerial& serial);
                void start();
		long dateAsSeconds(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
		void hourlyTasks(long time, int previousHour );
		void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear );
		void monthlyTasks(long time);
		void yearlyTasks(long time);
		long getCurrentTimeInSeconds();
		String getElapsedTimeHoursMinutesSecondsString(long elapsedTime);
		DateTime getCurrentDateTime();
		long getTimeForCodeGeneration();
		//
		// Functions that represent Serial commands
		//
		boolean getTime();
		boolean setTime(String);

	private:

		RTC_DS1307 RTC;
		DateTime now;
		int chipSelect = 10;
		int timeZoneHours=11;
		int SECONDOFFSET=10;
	};
#endif /* TIMEMANAGER_H_ */

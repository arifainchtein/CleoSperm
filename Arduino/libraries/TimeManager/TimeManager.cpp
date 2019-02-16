/*
 * TimeManager.cpp
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <TimeManager.h>
#include <Wire.h>
#include <RTClib.h>
#include <GeneralFunctions.h>

#define LEAP_YEAR(_year) ((_year%4)==0)
RTC_DS1307 RTC;
DateTime now;
const int chipSelect = 10; //cs or the save select pin from the sd shield is connected to 10.
int timeZoneHours=11;
int SECONDOFFSET=10;
static  byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
#define DS1307_ADDRESS 0x68
byte zero = 0x00;

TimeManager::TimeManager(GeneralFunctions& g, HardwareSerial& serial):generalFunctions(g),  _HardSerial(serial){
	Wire.begin();
}

//
// Functions that represents commands received via the serial port
//



byte TimeManager::decToBcd(byte val){
	// Convert normal decimal numbers to binary coded decimal
	return ( (val/10*16) + (val%10) );
}


boolean TimeManager::setTime(String command){
	int date = generalFunctions.getValue(command, '#', 1).toInt();
	int month = generalFunctions.getValue(command, '#', 2).toInt();
	int year = generalFunctions.getValue(command, '#', 3).toInt();
	int weekDay = generalFunctions.getValue(command, '#', 4).toInt();
	int hour = generalFunctions.getValue(command, '#', 5).toInt();
	int min = generalFunctions.getValue(command, '#', 6).toInt();
	int sec = generalFunctions.getValue(command, '#', 7).toInt();
	if(year>1999)year=year-2000;
	//	  byte second =      00; //0-59
	//	  byte minute =      19; //0-59
	//	  byte hour =        21; //0-23
	//	  byte weekDay =     4; //1-7
	//	  byte monthDay =    27; //1-31
	//	  byte month =       10; //1-12
	//	  byte year  =       12; //0-99

	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write(zero); //stop Oscillator

	Wire.write(decToBcd(sec));
	Wire.write(decToBcd(min));
	Wire.write(decToBcd(hour));
	Wire.write(decToBcd(weekDay));
	Wire.write(decToBcd(date));
	Wire.write(decToBcd(month));
	Wire.write(decToBcd(year));

	Wire.write(zero); //start

	Wire.endTransmission();



	//	setTime(hour,min,sec,date,month,year);
	//	RTC.set(now());
	//getTime();
	return true;

}

boolean TimeManager::printTimeToSerial(){

	DateTime now = RTC.now();
	_HardSerial.print(now.day());
	_HardSerial.print("/");//month
	_HardSerial.print(now.month());
	_HardSerial.print("/");//month
	_HardSerial.print(now.year());
	_HardSerial.print(" ");//month
	_HardSerial.print(now.hour());
	_HardSerial.print(":");
	_HardSerial.print(now.minute());
	_HardSerial.print(":");
	_HardSerial.println(now.second());
	_HardSerial.println("Ok-GetTime");
	_HardSerial.flush();
	return true;
}
//
// End of Functions that represents commands received via the serial port
//


void TimeManager::start(){
	RTC.begin();
	//check or the Real Time Clock is on
	if (! RTC.isrunning()) {
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		// uncomment it & upload to set the time, date and start run the RTC!
		RTC.adjust(DateTime(__DATE__, __TIME__));
		Serial.println("Current time:");
		printTimeToSerial();
	}

}



long TimeManager::dateAsSeconds(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){


	// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008

	int i;
	long seconds;

	if(year < 69)
		year+= 2000;
	// seconds from 1970 till 1 jan 00:00:00 this year
	seconds= (year-1970)*(60*60*24L*365);

	// add extra days for leap years
	for (i=1970; i<year; i++) {
		if (LEAP_YEAR(i)) {
			seconds+= 60*60*24L;
		}
	}
	// add days for this year
	for (i=0; i<month; i++) {
		if (i==1 && LEAP_YEAR(year)) {
			seconds+= 60*60*24L*29;
		} else {
			seconds+= 60*60*24L*monthDays[i];
		}
	}
	seconds+= (date-1)*3600*24L;
	seconds+= hour*3600L;
	seconds+= minute*60L;
	seconds -=  timeZoneHours*3600L;
	seconds+=second;

	return seconds;
}

String TimeManager::getCurrentTimeForDisplay(){
	DateTime now = RTC.now();
	String displayTime =  "";
	displayTime.concat(now.hour());
	displayTime.concat(":");
	displayTime.concat(now.minute());
	displayTime.concat(":");
	displayTime.concat(now.second());
	return displayTime;
}

String TimeManager::getCurrentDateTimeForDisplay(){
	DateTime now = RTC.now();
	String displayTime =  "";
	displayTime.concat(now.day());
	displayTime.concat("/");
	displayTime.concat(now.month());
	displayTime.concat("/");
	displayTime.concat(now.year());
	displayTime.concat(" ");
	displayTime.concat(now.hour());
	displayTime.concat(":");
	displayTime.concat(now.minute());
	displayTime.concat(":");
	displayTime.concat(now.second());

	return displayTime;
}

String TimeManager::getCurrentDateForDisplay(){
	DateTime now = RTC.now();
	String displayTime =  "";
	displayTime.concat(now.day());
	displayTime.concat("/");
	displayTime.concat(now.month());
	displayTime.concat("/");
	displayTime.concat(now.year());

	return displayTime;
}


long TimeManager::getTimeForCodeGeneration(){

	DateTime now = RTC.now();
	int seconds = now.second()+SECONDOFFSET;
	int month = now.month()-1;
	return dateAsSeconds(now.year(), month, now.day(), now.hour(), now.minute(), seconds);
}



DateTime TimeManager::getCurrentDateTime(){
	return  RTC.now();
}


long TimeManager::getCurrentTimeInSeconds(){
	now = RTC.now();
	long currentDateAsSeconds =dateAsSeconds(now.year(), now.month(), now.day(),now.hour(), now.minute(), now.second());
	return currentDateAsSeconds;
}

String TimeManager::getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {
	//String seconds = String(elapsedTime % 60);
	long seconds = elapsedTime/1000;
	int minutes = (seconds % 3600) / 60;
	String minP ="";
	if(minutes<10)minP="0";


	int hours = seconds / 3600;
	String hoursS = "";
	if(hours<10)hoursS="0";


	String time =  hoursS + hours + ":" + minP + minutes;// + ":" + seconds;
	return time;
}

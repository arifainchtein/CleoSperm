/*
  SDCardManager.h - Library Managing the high level functions of the Sd Card
  Created by Ari Fainchtein, Feb 13, 2019.
  Released into the public domain.
 */
#ifndef SDCardManager_h
#define SDCardManager_h
#include <WPSSensorRecord.h>

#include "Arduino.h"
#include <SD.h>
#include <GeneralFunctions.h>
#include <TimeManager.h>

class SDCardManager{
	TimeManager & timeManager;
	GeneralFunctions & generalFunctions;
	HardwareSerial& _HardSerial;

public:
	SDCardManager(TimeManager & t, GeneralFunctions  & f, HardwareSerial& serial);
	boolean start();
	boolean readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year);
	boolean readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName);
	void storeRememberedValue(long time, const char *name, float value, String unit);
	float searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor);
	void storeLifeCycleEvent(long time, const char *eventType, int eventValue);
	long printDirectory(File dir, int numTabs);
	long getDiskUsage();
	long getSDCardDiskUse(File dir );
	boolean getHistoricalData(const char *dirName, int date, int month, int year);
	void saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord);
	//
	// Functions that represent Serial commands
	//
	boolean testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus);
	float listFiles();
	boolean setTime(String);

private:
	File dataFile;

};

#endif

/*
 * SDCardManager.cpp
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */

#include "rgb_lcd.h"
#include <SPI.h>
#include <SD.h>
#include "Arduino.h"
#include <SDCardManager.h>
#include <GeneralFunctions.h>
#include <TimeManager.h>
#include <GravityRtc.h>
#include <WPSSensorRecord.h>
#include <RTCInfoRecord>
#define SD_PIN 53
extern rgb_lcd lcd;
extern char sensorDirName[10];
extern char lifeCycleFileName[10];
extern char remFileName[10];
const int chipSelect = 10;
extern const char *MAXIMUM_VALUE;//="Max";
extern const char *MINIMUM_VALUE;//="Min";
extern const char *AVERAGE_VALUE;//="Avg";


extern const char  *WPSSensorDataDirName; //="WPSSensr";
extern const char  *LifeCycleDataDirName; //="LifeCycl";
extern const char  *RememberedValueDataDirName;//  = "RememVal";
extern const char  *unstraferedFileName;// ="Untransf.txt";
extern char sensorDirName[10];
extern char lifeCycleFileName[10];
extern char remFileName[10];
//TimeManager & timeManager;
//	GeneralFunctions & generalFunctions;
//	HardwareSerial& _HardSerial;

SDCardManager::SDCardManager(TimeManager& t, GeneralFunctions& f,HardwareSerial& serial ): timeManager(t), generalFunctions(f), _HardSerial(serial)
{}

boolean SDCardManager::start(){
	//setup SD card
	_HardSerial.print("Initializing SD card...");

	if (!SD.begin(SD_PIN)) {
			Serial.println("No SD-card.");
			lcd.setCursor(0, 0);
			lcd.print("No SD-card.") ;
			return;
		}else{
			// Check dir for db files
			if (!SD.exists(sensorDirName )) {
				Serial.println("wpsSensorData Dir does not exist, creating...");
				SD.mkdir(sensorDirName);
			}
			if (!SD.exists(lifeCycleFileName)) {
				SD.mkdir(lifeCycleFileName);
			}
			if (!SD.exists(remFileName)) {
				SD.mkdir(remFileName);
			}
			File sensorFile = SD.open(sensorDirName );
			long totalDiskUse=getSDCardDiskUse(sensorFile);

			File lifeCycleFile = SD.open(lifeCycleFileName );
			totalDiskUse+=getSDCardDiskUse(lifeCycleFile);

			File rememberedValueFile = SD.open(remFileName );
			totalDiskUse+=getSDCardDiskUse(rememberedValueFile);

			sensorFile.close();
			lifeCycleFile.close();
			rememberedValueFile.close();

			lcd.setCursor(0, 0);
			lcd.print("Finish Init") ;
			lcd.setCursor(0, 1);
			lcd.print("SD use ") ;
			lcd.print(totalDiskUse/1024) ;
			lcd.print("Kb") ;


			_HardSerial.println("card initialized.");
			return true;

		}
}

//
// Functions that represents commands received via the serial port
//
boolean SDCardManager::testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus){
	long lastWPSRecordSeconds = timeManager.getCurrentTimeInSeconds();
	char fileName[25];
	snprintf(fileName, sizeof fileName, "/%s/%s", WPSSensorDataDirName, unstraferedFileName);

	File untransferredFile = SD.open(fileName, FILE_WRITE);
	if (untransferredFile) {
		// Write to file
		;

		untransferredFile.print(lastWPSRecordSeconds);
		untransferredFile.print("#");
		untransferredFile.print(batteryVoltage);
		untransferredFile.print("#");
		untransferredFile.print(current);
		untransferredFile.print("#");
		untransferredFile.print(stateOfCharge);
		untransferredFile.print("#");
		untransferredFile.print(operatingStatus);


		File sensorFile = SD.open(sensorDirName);
		long totalDiskUse=getDiskUsage();
		untransferredFile.print("#");
		untransferredFile.println(totalDiskUse/1024);


		untransferredFile.close(); // close the file
	}
}

float SDCardManager::listFiles(){

	File sensorFile = SD.open(sensorDirName );
	File lifeCycleFile = SD.open(lifeCycleFileName );
	File rememberedValueFile = SD.open(remFileName );

	long totalDiskUse=printDirectory(sensorFile, 1);
	_HardSerial.println(" ");
	_HardSerial.println(lifeCycleFileName);
	totalDiskUse+=printDirectory(lifeCycleFile, 1);
	_HardSerial.println(" ");
	_HardSerial.println(remFileName);
	totalDiskUse+=printDirectory(rememberedValueFile, 1);
	sensorFile.close();
	lifeCycleFile.close();
	rememberedValueFile.close();
	float total= (float)totalDiskUse/1024;
	return total;
}


//
// End of Functions that represents commands received via the serial port
//

boolean SDCardManager::readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year){
	//GetRememberedValueData#0

	char fileName[25] = "/";
	snprintf(fileName, sizeof fileName, "/%s/%s", dirName, unstraferedFileName);

	File uf = SD.open(fileName, FILE_WRITE);
	File tf;
	boolean result=false;
	if(moveData==1){
		char fileNameTF[24];
		snprintf(fileNameTF, sizeof fileName, "/%s/%i_%i_%i.txt", dirName, date,month, year);
		tf = SD.open(fileNameTF, FILE_WRITE);
	}

	if (uf) {
		uf.seek(0);
		while (uf.available()){
			//
			// Read each line, send it to the serial port
			// and copy it into today's file
			String line = uf.readStringUntil('\n');
			if(sendToSerial)_HardSerial.print(line);
			if(moveData==1)tf.print(line);
		}
		uf.close(); // close the file
		if(moveData==1){
			tf.close(); // close the file
			//
			// since we just transferred the records and copy them
			// delete the file untransferredFile
			SD.remove(fileName);
		}
		result=true;
	}
	return result;
}

boolean SDCardManager::readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName){
	RTCInfoRecord anRTCInfoRecord = timeManager.getCurrentDateTime();
		int year = anRTCInfoRecord.year-2000;
		int month = anRTCInfoRecord.month-1;
	return readUntransferredFileFromSDCardByDate( moveData,  sendToSerial,  dirName,  anRTCInfoRecord.date,  month,  year);
}



void SDCardManager::storeRememberedValue(long time, const char *name, float value, String unit){
	//File untransferredFile = SD.open("/" + RememberedValueDataDirName + "/" + unstraferedFileName, FILE_WRITE);

	char untransferredFileName[25];
	sprintf(untransferredFileName,"/%s/%s",RememberedValueDataDirName,unstraferedFileName);
	File untransferredFile = SD.open(untransferredFileName, FILE_WRITE);

	if (untransferredFile) {
		// Write to file
		untransferredFile.print(time);
		untransferredFile.print("#");
		untransferredFile.print(name);
		untransferredFile.print("#");
		untransferredFile.print(value);
		untransferredFile.print("#");
		untransferredFile.println(unit);
		untransferredFile.close(); // close the file
	}
}

float SDCardManager::searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor){

	float result=-9999;
	String line="";
	char anyLabel[50];
	float value=0;
	int sampleCount=0;
	int sampleSum=0;
	char fileName[32];
	sprintf(fileName,"/%s/%i_%i_%i/.txt",RememberedValueDataDirName,date,month,year);
	File todayFile = SD.open(fileName, FILE_WRITE);


	//File todayFile = SD.open("/" + RememberedValueDataDirName + "/" + date + "_" +  month + "_" + year + ".txt", FILE_WRITE);

	if (todayFile) {
		todayFile.seek(0);
		while (todayFile.available()){
			//
			// Read each line, send it to the serial port
			// and copy it into today's file
			line = todayFile.readStringUntil('\n');

			generalFunctions.getValue(line, '#', 1).toCharArray(anyLabel, sizeof anyLabel);
			if(strcmp(label, anyLabel) == 0){
				value = generalFunctions.stringToFloat(generalFunctions.getValue(line, '#', 2));
				if(whatToSearchFor == MAXIMUM_VALUE){
					if(value>result)result=value;
				}else if(whatToSearchFor == MINIMUM_VALUE){
					if(value<result)result=value;
				}else if(whatToSearchFor == AVERAGE_VALUE){
					sampleCount++;
					sampleSum+=value;
				}
			}

		}
		todayFile.close(); // close the file
	}
	if(whatToSearchFor == AVERAGE_VALUE && sampleCount>0){
		result = sampleSum/sampleCount;
	}
	return result;
}

void SDCardManager::storeLifeCycleEvent(long time, const char *eventType, int eventValue){

	char untransferredFileName[25];
	sprintf(untransferredFileName,"/%s/%s",LifeCycleDataDirName,unstraferedFileName);
	File untransferredFile = SD.open(untransferredFileName, FILE_WRITE);


	//File untransferredFile = SD.open("/" + LifeCycleDataDirName + "/" + unstraferedFileName, FILE_WRITE);
	if (untransferredFile) {
		// Write to file
		untransferredFile.print(time);
		untransferredFile.print("#");
		untransferredFile.print(eventType);
		untransferredFile.print("#");
		untransferredFile.println(eventValue);
		untransferredFile.close(); // close the file
	}
}


boolean SDCardManager::getHistoricalData(const char *dirName, int date, int month, int year){
	boolean result=false;

	char fileName[24];
	snprintf(fileName, sizeof fileName, "/%s/%i_%i_%i.txt", dirName, date,month, year);


	File todayFile1 = SD.open(fileName, FILE_WRITE);

	if (todayFile1) {
		todayFile1.seek(0);
		while (todayFile1.available()){
			//
			// Read each line, send it to the serial port
			// and copy it into today's file
			String line = todayFile1.readStringUntil('\n');
			_HardSerial.print(line);
		}
		todayFile1.close(); // close the file
		//
		// tell the hypothalamus we are done
		//
		result=true;
	}
	return result;
}

long SDCardManager::getSDCardDiskUse(File dir ) {

	long total=0L;
	while(true) {

		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files
			break;
		}
		if (entry.isDirectory()) {
			total+=getSDCardDiskUse(entry);
		} else {
			// files have sizes, directories do not
			total+=entry.size();
		}
		entry.close();
	}
	return total;
}


long SDCardManager::getDiskUsage(){
	File sensorFile = SD.open(sensorDirName );
	File lifeCycleFile = SD.open(lifeCycleFileName );
	File rememberedValueFile = SD.open(remFileName );

	long totalDiskUse=getSDCardDiskUse(sensorFile);
	totalDiskUse+=getSDCardDiskUse(lifeCycleFile);
	totalDiskUse+=getSDCardDiskUse(rememberedValueFile);

	sensorFile.close();
	lifeCycleFile.close();
	rememberedValueFile.close();

	return totalDiskUse;
}

long SDCardManager::printDirectory(File dir, int numTabs) {
	long total=0L;
	while(true) {

		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files
			break;
		}
		for (uint8_t i=0; i<numTabs; i++) {
			_HardSerial.print('\t');
		}
		_HardSerial.print(entry.name());
		if (entry.isDirectory()) {
			_HardSerial.println("/");
			printDirectory(entry, numTabs+1);
		} else {
			// files have sizes, directories do not
			_HardSerial.print("\t\t");
			total+=entry.size();
			_HardSerial.println(entry.size(), DEC);
		}
		entry.close();
	}
	return total;
}


void SDCardManager::saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord){
	char fileName[30];
	snprintf(fileName, sizeof fileName, "/%s/%s", WPSSensorDataDirName, unstraferedFileName);
	File untransferredFile = SD.open(fileName, FILE_WRITE);
	if (untransferredFile) {
		// Write to file
		lcd.setRGB(0,0,255);
		float batteryVoltage = anWPSSensorRecord.batteryVoltage;
		float current = anWPSSensorRecord.current;
		int sc = anWPSSensorRecord.stateOfCharge;
		untransferredFile.print(anWPSSensorRecord.lastWPSRecordSeconds);
		untransferredFile.print("#");
		untransferredFile.print(batteryVoltage);
		untransferredFile.print("#");
		untransferredFile.print(current);
		untransferredFile.print("#");
		//
		// calculate the energy used in mAhr
		//
		//		float energy = wpsPulseFrequencySeconds*current/3600;
		//		dailyBatteryOutEnergy+=energy;
		//		untransferredFile.print(energy);
		//		untransferredFile.print("#");
		//		hourlyBatteryOutEnergy+=energy;

		untransferredFile.print(anWPSSensorRecord.hourlyBatteryOutEnergy);
		untransferredFile.print("#");

		untransferredFile.print(anWPSSensorRecord.dailyBatteryOutEnergy);
		untransferredFile.print("#");

		untransferredFile.print(anWPSSensorRecord.hourlyPoweredDownInLoopSeconds);
		untransferredFile.print("#");

		untransferredFile.print(anWPSSensorRecord.dailyPoweredDownInLoopSeconds);
		untransferredFile.print("#");

		untransferredFile.print(anWPSSensorRecord.pauseDuringWPS);
		untransferredFile.print("#");

		untransferredFile.print(sc);
		untransferredFile.print("#");
		untransferredFile.print(anWPSSensorRecord.operatingStatus);

		untransferredFile.print("#");
		untransferredFile.println(anWPSSensorRecord.totalDiskUse/1024);

		untransferredFile.close(); // close the file
		lcd.setCursor(0,0);
		lcd.print("saved wps record");
		lcd.setCursor(0,1);
		lcd.print("to SD card ");


	} else {
		lcd.setRGB(255,255,0);
		lcd.setCursor(0,0);
		lcd.print("error opening");
		lcd.setCursor(0,1);
		lcd.print(fileName);
		delay(2000);
	}
}


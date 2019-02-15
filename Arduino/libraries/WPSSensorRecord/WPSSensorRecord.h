 #ifndef  WPSSENSORRECORD_H
 #define WPSSENSORRECORD_H
struct WPSSensorRecord{

		float current;
		int stateOfCharge;
		long lastWPSRecordSeconds;
		float  batteryVoltage;
		float hourlyBatteryOutEnergy;
		float dailyBatteryOutEnergy;
		float hourlyPoweredDownInLoopSeconds;
		float dailyPoweredDownInLoopSeconds;
		boolean pauseDuringWPS;
		String operatingStatus;
		long totalDiskUse;
};
#endif

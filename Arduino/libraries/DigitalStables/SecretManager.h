/*
  SecretManager.h - Library Managing the TOTP
  Created by Ari Fainchtein, Feb 13, 2019.
  Released into the public domain.
*/
#ifndef SecretManager_h
#define SecretManager_h
#include "Arduino.h"
#include "TimeManager.h"
#include "EEPROM.h"

class SecretManager{
		TimeManager & timeManager;
		 int SHARED_SECRET_LENGTH=27;
			char code[7];
			int currentCommandCodeHistoryPos=0;
			int numberOfCommandCodesInHistory=5;
			long commandCodeHistory[5]={999999,999999,999999,99999,99999};

	public:
		SecretManager(TimeManager & t) ;
		void saveSecret(String secret, int numberDigits, int periodSeconds );
		void readSecret(char *secretCode);
		long generateCode();
		boolean checkCode(long userCode);

	private:

};

#endif

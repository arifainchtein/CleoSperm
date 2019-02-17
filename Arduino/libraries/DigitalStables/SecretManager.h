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



	public:
		SecretManager(TimeManager & t) ;
		void saveSecret(String secret, int numberDigits, int periodSeconds );
		void readSecret(char *secretCode);
		long generateCode();
		boolean checkCode(long userCode);

	private:

};

#endif

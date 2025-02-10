/*

	Remote IO Control for Arduino
	
	Copyright 2016-2025 Robin Zhang & Labs
 	https://github.com/robinz-labs/rioc-arduino

	version 0.41

	NOTE: Please edit file RiocOptional.h for more option settings.

	This program has been tested with Arduino UNO, Duemilanove, Nano, MEGA and DUE. 

	This version is also partially compatible with some other types of boards, 
	such as ESP32 and RP2040.

*/


// UNIT_ID can be set to a number between 1 and 254
// If UNIT_ID is set to 0 here, the unit ID can also be reconfigured remotely later

#define UNIT_ID 1

#include "RiocDevice.h"

void setup()
{   
  Serial.begin(115200);
  initRioc(UNIT_ID);
}

void loop()
{
  processRioc();
}

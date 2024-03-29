/*

	Remote IO Control for Arduino
	
	Copyright 2016-2024 Robin Zhang & Labs
 	https://github.com/robinz-labs/rioc-arduino

	version 0.31

	NOTE: Please edit file RiocOptional.h for more option settings.

	This program has been tested with Arduino UNO, Duemilanove, Nano, MEGA and DUE, 
	other Arduino boards may not be supported.
  
*/



// UNIT_ID can be set as a number between 1 ~ 254
// the unit ID can be also reconfigurable by remote if here set UNIT_ID as 0

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

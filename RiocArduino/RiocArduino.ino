/*

	Remote IO Control for Arduino
	
	Copyright 2016-2020 Robin Zhang & Labs
 	http://robinz.org/labs/

	version 0.25

	NOTE: Please edit file RiocOptional.h for more option settings.

	This program has been tested with Arduino UNO, Duemilanove, Nano, MEGA and DUE, 
	other Arduino boards may not be supported.
  
*/


#include "RiocDevice.h"

void setup()
{   
  Serial.begin(115200);
  initRioc();
}

void loop()
{
  processRioc();
}

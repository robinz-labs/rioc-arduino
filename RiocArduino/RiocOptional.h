/*
    Optional parts can be enabled or disabled

	Arduino UNO, Nano or any others with ATmega328 MCU has only 32K bytes for program storage.
	You can edit file RiocOptional.h to disable some optional parts when you need recompile 
	the program for a smaller storage space occupied in Arduino.

	If your Arduino is a MEGA 1280 / 2560, please just compile the program with keeping all 
	OPT_ENABLE options on.
*/

// library Adafruit_NeoPixel is required to enable option RGBLED
// http://github.com/adafruit/Adafruit_NeoPixel

// #define OPT_ENABLE_RGBLED


// library DHT is required to enable option THERMOMETER
// http://arduino.cc/playground/Main/DHTLib

// #define OPT_ENABLE_THERMOMETER


#if not defined(__SAM3X8E__) 

// library Arduino-IRremote is required to enable option IR_TRANSMITTER and IR_RECEIVER
// http://z3t0.github.io/Arduino-IRremote/

// #define OPT_ENABLE_IR_TRANSMITTER
// #define OPT_ENABLE_IR_RECEIVER

#endif


// use software tone playing 

#if defined(OPT_ENABLE_IR_TRANSMITTER) || defined(OPT_ENABLE_IR_RECEIVER) || defined(__SAM3X8E__) 
	#define OPT_SOFTWARE_TONE
#endif


// use software reset

#define OPT_SOFTWARE_RESET


// need enable the option if your arduino is an aladdin 2560 user device

// #define OPT_UD_ALADDIN

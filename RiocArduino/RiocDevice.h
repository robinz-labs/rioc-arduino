#ifndef LIB_RIOC_DEVICE_H_
#define LIB_RIOC_DEVICE_H_

#include "RiocMessager.h"
#include "RiocObject.h"
#include "RiocUserDevice.h"

#define RIOC_VERSION_MAJOR	0
#define RIOC_VERSION_MINOR	41

#define RO_GENERAL_DIGITAL_IN		    0x01
#define RO_GENERAL_DIGITAL_OUT		    0x02
#define RO_GENERAL_ANALOG_IN		    0x03
#define RO_GENERAL_ANALOG_OUT		    0x04
#define RO_GENERAL_UART_SERIAL		    0x05
#define RO_GENERAL_MULTIPLE_DIGITAL_IN  0x06
#define RO_GENERAL_MULTIPLE_DIGITAL_OUT 0x07
#define RO_MOTION_MOTOR				    0x11
#define RO_MOTION_STEPPER			    0x12
#define RO_MOTION_RUDDER			    0x13
#define RO_MOTION_SERVO				   	0x14
#define RO_SENSOR_ENCODER			    0x21
#define RO_SENSOR_ULTRASONIC_RANGER	    0x22
#define RO_SENSOR_THERMOMETER		    0x23
#define RO_SOUND_TONE				    0x31
#define RO_LIGHT_RGBLED				    0x41
#define RO_IR_TRANSMITTER			    0x51
#define RO_IR_RECEIVER				    0x52

extern byte unitDescription[4];

extern RiocMessager* messager;

extern bool pinOccupied[PIN_COUNT];
extern RiocObject* pinObject[PIN_COUNT];

extern bool portOccupied[PORT_COUNT];
extern RiocObject* portObject[PORT_COUNT];

void initRioc(byte unitId);
void processRioc();

RiocObject* setupRioc(byte msg[8], byte address_from);
void executeRioc(byte msg[8], byte address_from);

RiocObject* createObject(byte objectType, 
                         byte param1=0, byte param2=0, byte param3=0, 
                         byte param4=0, byte param5=0, byte param6=0);

#endif // LIB_RIOC_DEVICE_H_

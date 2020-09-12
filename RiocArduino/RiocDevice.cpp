#if !defined(__SAM3X8E__)
#include <EEPROM.h>
#endif

#include "RiocDevice.h"
#include "RiocGlobal.h"
#include "RiocSerialMessager.h"
#include "RODigitalIn.h"
#include "RODigitalOut.h"
#include "ROAnalogIn.h"
#include "ROAnalogOut.h"
#include "ROUartSerial.h"
#include "ROMultipleDigitalIn.h"
#include "ROMultipleDigitalOut.h"
#include "ROMotor.h"
#include "ROStepper.h"
#include "RORudderServo.h"
#include "ROEncoder.h"
#include "ROUltrasonicRanger.h"
#include "ROThermometer.h"
#include "ROTone.h"
#include "RORgbLed.h"
#include "ROIrTransmitter.h"
#include "ROIrReceiver.h"

byte unitDescription[4] = {'R', 'I', 'O', 'C'};

RiocMessager* messager;

bool pinOccupied[PIN_COUNT];
RiocObject* pinObject[PIN_COUNT];

bool portOccupied[PORT_COUNT];
RiocObject* portObject[PORT_COUNT];

bool pinAvailable(int pin) { return (pin < PIN_COUNT && !pinOccupied[pin]); }
bool portAvailable(int port) { return (port < PORT_COUNT && !portOccupied[port]); }
bool pinManaged(int pin) { return (pin < PIN_COUNT && pinObject[pin]!=NULL); }
bool portManaged(int port)  { return (port < PORT_COUNT && portObject[port]!=NULL); }

bool pinsAvailable(int pin, int number) {
    for (int n=0; n<number ; n++) {
      if (!pinAvailable(pin+n)) return false;
    }
    return true;
}

void onMessageReceived(byte msg[8], byte address_from);

//
// initialize the rioc device 
//
void initRioc()
{
  for (int n=0 ; n<PIN_COUNT ; n++) {
    pinOccupied[n] = false;    
    pinObject[n] = NULL;
  }
  for (int n=0 ; n<PORT_COUNT ; n++) {
    portOccupied[n] = false;    
    portObject[n] = NULL;
  }
  
  // pins occupied
  pinOccupied[0] = true;    // uart0
  pinOccupied[1] = true;

  #if defined(__SAM3X8E__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  
  pinOccupied[14] = true;   // uart3
  pinOccupied[15] = true;
  pinOccupied[16] = true;   // uart2
  pinOccupied[17] = true;
  pinOccupied[18] = true;   // uart1
  pinOccupied[19] = true;
  #endif

  pinOccupied[PIN_RESET] = true;    // pin for wire to reset

  // ports occupied
  portOccupied[0] = true;   // rioc messager

  // get unit id of this rioc device
  byte unitId = 1;
  #if !defined(__SAM3X8E__)
  unitId = EEPROM.read(0);
  #endif

  if (unitId==0 || unitId==0xff) unitId=1;

  // init messager
  messager = (RiocMessager*)(new RiocSerialMessager(unitId));
  messager->setCallbackMessageReceived(onMessageReceived);
  messager->begin();

  byte msg[] = {
    0x00, 0x8f, 
    RIOC_VERSION_MAJOR, RIOC_VERSION_MINOR, 
    unitDescription[0], unitDescription[1], 
    unitDescription[2], unitDescription[3]
  };
  messager->sendMessage(msg, 0);       

  // init others for user device
  initUserDevice();
}

//
// process routine jobs for rioc objects while system idle   
//
void processRioc()
{
  messager->process();

  for (int n=0 ; n<PIN_COUNT ; n++) {
    if (pinObject[n] != NULL) pinObject[n]->process();
  }
  for (int n=0 ; n<PORT_COUNT ; n++) {
    if (portObject[n] != NULL) portObject[n]->process();
  }

  // do more things for user device
  processUserDevice();
}

//
// when received a rioc
//
void onMessageReceived(byte msg[8], byte address_from)
{
  // echo (debug)
  ///messager->sendMessage(msg, address_from);

  if (msg[0]==0x00) {

    // rioc unit control message

    if (msg[1]==0x01) {

      // reset device
      pinMode(PIN_RESET, OUTPUT);
      digitalWrite(PIN_RESET, LOW);
      delay(200);
      byte msg[] = {0x00, 0x81, 0, 0, 0, 0, 0, 0};
      messager->sendMessage(msg, address_from); // send it only when failed to reset
    
    } else if (msg[1]==0x02) {
      
      // get unit version
      byte msg[] = {
        0x00, 0x82, 
        RIOC_VERSION_MAJOR, RIOC_VERSION_MINOR, 
        unitDescription[0], unitDescription[1], 
        unitDescription[2], unitDescription[3]
      };
      messager->sendMessage(msg, address_from);
    
    } else if (msg[1]==0x03) {

      // set unit address
      byte unitId = msg[2];
      if (unitId>0 && unitId<0xff) {
        #if !defined(__SAM3X8E__)
        EEPROM.write(0, unitId);
        byte msg[] = {0x00, 0x83, unitId, 0, 0, 0, 0, 0};
        messager->sendMessage(msg, address_from);
        #endif
      }
    
    } else if (msg[1]==0x06) {

      // wait for milliseconds
      unsigned int ms = ((unsigned int)msg[2]<<8) | (unsigned int)msg[3];
      byte rspMode = msg[4];
      
      if (rspMode & 0x01) {
        byte msg1[] = {0x00, 0x86, ms >> 8, ms & 0xff, 0, 0, 0, 0};
        messager->sendMessage(msg1, address_from);
      }

      delay(ms);

      if (rspMode & 0x02) {
        byte msg2[] = {0x00, 0x86, 0x00, 0x00, 0, 0, 0, 0};
        messager->sendMessage(msg2, address_from);
      }
    
    } else if (msg[1]==0x07) {

      // set silent device
      byte silent = msg[2];
      
      if (silent) 
        g_isSilentDevice = true;
      else
        g_isSilentDevice = false;

      byte msg[] = {0x00, 0x87, silent, 0, 0, 0, 0, 0};
      messager->sendMessage(msg, address_from);
    }

  } else {

    // rioc object control message

    if (msg[1]==0x00)
      setupRioc(msg, address_from);     // setup a new object
    else
      executeRioc(msg, address_from);   // execute an action of the specified object
  }
}

//
// create and initialize a rioc object
//
RiocObject* setupRioc(byte msg[8], byte address_from) 
{
  RiocObject* obj = NULL;

  // new object
  int type = msg[0];
  int mode = -1;

  if (type==RO_GENERAL_DIGITAL_IN) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new RODigitalIn());

  } else if (type==RO_GENERAL_DIGITAL_OUT) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new RODigitalOut());

  } else if (type==RO_GENERAL_ANALOG_IN) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new ROAnalogIn());
  
  } else if (type==RO_GENERAL_ANALOG_OUT) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new ROAnalogOut());

  } else if (type==RO_GENERAL_UART_SERIAL) {

    if (portAvailable(msg[2])) obj = (RiocObject*)(new ROUartSerial());

  } else if (type==RO_GENERAL_MULTIPLE_DIGITAL_IN) {

    if (pinsAvailable(msg[2], msg[3])) obj = (RiocObject*)(new ROMultipleDigitalIn());

  } else if (type==RO_GENERAL_MULTIPLE_DIGITAL_OUT) {

    if (pinsAvailable(msg[2], msg[3])) obj = (RiocObject*)(new ROMultipleDigitalOut());

  } else if (type==RO_MOTION_MOTOR) {

    if (pinAvailable(msg[2]) && pinAvailable(msg[3])) obj = (RiocObject*)(new ROMotor());

  } else if (type==RO_MOTION_STEPPER) {

    mode = msg[6];

    if ((mode==STEPPER_MODE_NORMAL && pinAvailable(msg[2]) && pinAvailable(msg[3]) && pinAvailable(msg[4]) && pinAvailable(msg[5])) ||
        (mode==STEPPER_MODE_PUL_DIR && pinAvailable(msg[2]) && pinAvailable(msg[3]))) obj = (RiocObject*)(new ROStepper());

  } else if (type==RO_MOTION_RUDDER) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new RORudderServo());    

  } else if (type==RO_SENSOR_ENCODER) {

    if (pinAvailable(msg[2]) && pinAvailable(msg[3])) obj = (RiocObject*)(new ROEncoder());

  } else if (type==RO_SENSOR_ULTRASONIC_RANGER) {

    if (pinAvailable(msg[2]) && pinAvailable(msg[3])) obj = (RiocObject*)(new ROUltrasonicRanger());

  } else if (type==RO_SENSOR_THERMOMETER) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new ROThermometer());
    
  } else if (type==RO_SOUND_TONE) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new ROTone());

  } else if (type==RO_LIGHT_RGBLED) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new RORgbLed());

  } else if (type==RO_IR_TRANSMITTER) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new ROIrTransmitter());
  
  } else if (type==RO_IR_RECEIVER) {

    if (pinAvailable(msg[2])) obj = (RiocObject*)(new ROIrReceiver());
  }

  // setup object
  if (obj != NULL) {
    if (!(obj->setup(msg, address_from))) {
    
      delete obj;
      obj = NULL; 
    }
  }

  // set occupied pin or port
  if (obj != NULL) {

    if (type==RO_GENERAL_UART_SERIAL) {

      // one port occupied
      int port = msg[2];
      portOccupied[port] = true;
      portObject[port] = obj;
    
    } else if (type==RO_MOTION_MOTOR || type==RO_SENSOR_ENCODER || type==RO_SENSOR_ULTRASONIC_RANGER) {

      // two pins occupied
      for (int n=2 ; n<=3 ; n++) {
        int pin = msg[n];
        pinOccupied[pin] = true;
        pinObject[pin] = obj;
      }

    } else if (type==RO_MOTION_STEPPER && mode==STEPPER_MODE_NORMAL) {

      // four pins occupied
      for (int n=2 ; n<=5 ; n++) {
        int pin = msg[n];
        pinOccupied[pin] = true;
        pinObject[pin] = obj;
      }

    } else if (type==RO_MOTION_STEPPER && mode==STEPPER_MODE_PUL_DIR) {

      // two pins occupied
      for (int n=2 ; n<=3 ; n++) {
        int pin = msg[n];
        pinOccupied[pin] = true;
        pinObject[pin] = obj;
      }

    } else if (type==RO_GENERAL_MULTIPLE_DIGITAL_IN || type==RO_GENERAL_MULTIPLE_DIGITAL_OUT) {

      // multiple pins occupied
      int pin = msg[2];
      int number = msg[3];
      for (int n=0 ; n<number ; n++) {
        pinOccupied[pin+n] = true;
        pinObject[pin+n] = obj;
      }

    } else {

      // one pin occupied
      int pin = msg[2];
      pinOccupied[pin] = true;
      pinObject[pin] = obj;
    }
  }

  // reply message
  if (obj != NULL) {

    obj->setMessager(messager);

    byte rsp[] = {type, 0x80, msg[2], 0x01, 0, 0, 0, 0};
    messager->sendMessage(rsp, address_from);
  } else {

    byte rsp[] = {type, 0x80, msg[2], 0x00, 0, 0, 0, 0};
    messager->sendMessage(rsp, address_from);
  }

  return obj;
}

//
// execute an action of the specified rioc object
//
void executeRioc(byte msg[8], byte address_from) 
{
    int type = msg[0];
    int cmd = msg[1];

    if (type == RO_GENERAL_UART_SERIAL) {
      int port = msg[2];
      if (portManaged(port)) 
        if (cmd>=0x70)
          portObject[port]->executeReserved(msg, address_from);
        else
          portObject[port]->execute(msg, address_from);
    } else {
      int pin = msg[2];
      if (pinManaged(pin)) 
        if (cmd>=0x70)
          pinObject[pin]->executeReserved(msg, address_from);
        else
          pinObject[pin]->execute(msg, address_from);
    }
}

//
// create local rioc object
//
RiocObject* createObject(byte objectType, 
                         byte param1, byte param2, byte param3, 
                         byte param4, byte param5, byte param6)
{
    byte cmd[] = { objectType, 0x00, param1, param2, param3, param4, param5, param6 };
    return setupRioc(cmd, INTERNAL_MESSAGING);
}

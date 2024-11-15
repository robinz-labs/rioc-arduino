#include "RORudderServo.h"
#include "RiocMessager.h"

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

RORudderServo::RORudderServo()
{
  _pin = -1;
  _mode = -1;
  _enabled = false;
  _servo = NULL;
}


RORudderServo::~RORudderServo()
{
}

bool RORudderServo::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false;

  int pin = msg[2];
  int mode = msg[3];

  if (pin<=DO_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode;
    _enabled = false;

    _servo = new Servo();
    return true;
  } 
  return false;
}

void RORudderServo::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
    
    // SET ANGLE
    
    int angle = msg[3];

    if (angle > 180) angle = 180;
    _servo->write(angle);

    // enable the servo as soon as write the first angle
    if (!_enabled) setEnabled(true);

    if (!isSilent()) {
      byte rsp[] = {0x13, 0x81, _pin, angle, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x02) {

    // GET ANGLE

    int angle = _servo->read();

    byte rsp[] = {0x13, 0x82, _pin, angle, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);

  } else if (cmd==0x03) {

    // SET ENABLE

    bool enabled = msg[3];
    setEnabled(enabled);

    if (!isSilent()) {
      byte rsp[] = {0x13, 0x83, _pin, enabled, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
  
  } else if (cmd==0x04) {

    // GET ENABLE

    bool enabled = _enabled; //_servo->attached();

    byte rsp[] = {0x13, 0x84, _pin, enabled, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  }  

}

void RORudderServo::process()
{    
}

void RORudderServo::setAngle(int angle)
{
  _servo->write(angle);
}

void RORudderServo::setEnabled(bool enabled)
{
  if (enabled == _enabled) return;

  if (enabled) {
    _servo->attach(_pin);
  } else {
    _servo->detach();
  }
  _enabled = enabled;
}


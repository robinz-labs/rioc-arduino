#include "RODigitalOut.h"
#include "RiocMessager.h"

RODigitalOut::RODigitalOut()
{
  _pin = -1;
  _mode = -1;

  _val = LOW;
  _pwm = 0;
}


RODigitalOut::~RODigitalOut()
{
}

bool RODigitalOut::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3]; // reserved
  if (pin<=DO_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode;

    pinMode(_pin, OUTPUT);
    return true;
  } 
  return false;
}

void RODigitalOut::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // WRITE
    
    int val = (msg[3]==0 ? LOW : HIGH);
    write(val);
    
    if (!isSilent()) {
      byte rsp[] = {0x02, 0x81, _pin, _val, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x02) {

    // CONFIG PWM

  } else if (cmd==0x03) {

    // WRITE PWM

    int pwm = msg[3];
    writePWM(pwm);

    if (!isSilent()) {
      byte rsp[] = {0x02, 0x83, _pin, _pwm, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  }  else if (cmd==0x04) {

    // PULSE

    int val = (msg[3]==0 ? LOW : HIGH);
    long pw = ((long)msg[4]<<16) | ((long)msg[5]<<8) | (long)msg[6];
    pulse(pw, val);

    if (!isSilent()) {
      byte rsp[] = {0x02, 0x84, _pin, val, msg[4], msg[5], msg[6], 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x05) {
      
    // READ
    
    byte rsp[] = {0x02, 0x85, _pin, _val, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  
  } else if (cmd==0x06) {
      
    // READ PWM
    
    byte rsp[] = {0x02, 0x86, _pin, _pwm, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  } 
}

void RODigitalOut::process()
{
    
}

void RODigitalOut::write(int val)
{
  _val = val;
  digitalWrite(_pin, _val);
}

void RODigitalOut::writePWM(int pwm)
{
  _pwm = pwm;

  #if defined (__SAM3X8E__)
  analogWriteResolution(8);
  #endif
  analogWrite(_pin, _pwm);
}

void RODigitalOut::pulse(long pw, int val)
{
  digitalWrite(_pin, val);
  delay(pw / 1000);
  delayMicroseconds(pw % 1000);
  digitalWrite(_pin, !val);
  _val = !val;
}


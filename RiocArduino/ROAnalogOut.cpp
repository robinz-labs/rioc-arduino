#include "ROAnalogOut.h"
#include "RiocMessager.h"


ROAnalogOut::ROAnalogOut()
{
  _pin = -1;
  _mode = -1;

  _val = 0;
}


ROAnalogOut::~ROAnalogOut()
{
}

bool ROAnalogOut::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  #if defined (__SAM3X8E__)

  int pin = msg[2];
  int mode = msg[3]; // reserved

  if (pin>=AO_PIN_MIN_NUMBER && pin<=AO_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode;

    return true;
  } 

  #endif
  
  return false;
}

void ROAnalogOut::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // WRITE
    unsigned int val = ((unsigned int)msg[3]<<8) | (unsigned int)msg[4];
    write(val);

    if (!isSilent()) {
      byte rsp[] = {0x04, 0x81, _pin, _val>>8, _val & 0xff, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  
  } else if (cmd==0x02) {
      
    // READ
    
    byte rsp[] = {0x04, 0x82, _pin, _val>>8, _val & 0xff, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  }
}

void ROAnalogOut::process()
{
    
}

void ROAnalogOut::write(unsigned int val)
{
  _val = val;

  #if defined (__SAM3X8E__)
  analogWriteResolution(12);
  analogWrite(_pin, map(_val, 0, 0xffff, 0, 4095));
  #endif
}

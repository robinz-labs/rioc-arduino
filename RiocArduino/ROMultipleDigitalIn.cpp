#include "ROMultipleDigitalIn.h"
#include "RiocMessager.h"

ROMultipleDigitalIn::ROMultipleDigitalIn()
{
  _pin = -1;
  _number = 0;
  _mode = -1;

  _notifyEnabled = false;
  _notifyAddress = 0;
  _interval = 0;
  _lastValue = 0;
  _lastTime = millis();
}


ROMultipleDigitalIn::~ROMultipleDigitalIn()
{
}

bool ROMultipleDigitalIn::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int number = msg[3]; 
  int mode = msg[4]; 

  if (number>=1 && number<=32 && 
      pin<=DI_PIN_MAX_NUMBER && (pin+number-1)<=DI_PIN_MAX_NUMBER) {

    _pin = pin;
    _number = number;
    _mode = mode;
    
    for (int n=_pin ; n <= (_pin+_number-1) ; n++) {
      
#if defined(OPT_UD_ALADDIN)
      pinMode(n, INPUT); // use pulldown resister on aladdin board
#else
      pinMode(n, (_mode ? INPUT : INPUT_PULLUP));
#endif
      
    }

    return true;
  } 
  return false;
}

void ROMultipleDigitalIn::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // READ
      
    unsigned long val = read();
    byte rsp[] = {0x06, 0x81, _pin, val & 0xff, val>>8, val>>16, val>>24, 0}; 
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    
  } else if (cmd==0x02) {

    // SET NOTIFICATION

    int enable = msg[3];
    unsigned int interval = ((unsigned int)msg[4]<<8) | (unsigned int)msg[5];
    
    if (enable) {
      _notifyEnabled = true;
      _notifyAddress = address_from;
      _interval = interval;
      _lastValue = read();
      _lastTime = millis();
      byte rsp[] = {0x06, 0x82, _pin, _notifyEnabled, _interval>>8, _interval & 0xff, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    } else {
      _notifyEnabled = false;
      byte rsp[] = {0x06, 0x82, _pin, _notifyEnabled, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
    
  }
}

void ROMultipleDigitalIn::process()
{
  if (_pin == -1) return; 

  // notify
  if (_notifyEnabled) {
    
    bool needNotify = false;
    unsigned long thisTime = millis();
    unsigned long val = 0;

    if (_interval==0) {
    
      // notify on value changed
    
      val = read();
      if (val != _lastValue) {
        needNotify = true;
      }

    } else {

      // notify on timer fired

      if ( thisTime - _lastTime > _interval || thisTime < _lastTime ) {
        val = read();
        needNotify = true;
      }
    }

    if (needNotify) {
      byte rsp[] = {0x06, 0x83, _pin, val & 0xff, val>>8, val>>16, val>>24, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);
      _lastValue = val;
      _lastTime = thisTime;
    }
  }
}

unsigned long ROMultipleDigitalIn::read()
{
    unsigned long val = 0;

    for (int n=0 ; n<_number ; n++) {
      int bit = digitalRead(_pin+n);
      val |= (bit<<n);
    }

    return val;
}

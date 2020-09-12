#include "RODigitalIn.h"
#include "RiocMessager.h"


RODigitalIn::RODigitalIn()
{
  _pin = -1;
  _mode = -1;
  _filter = -1;
  _sampleInterval = 0;
  _lastSampleTime = micros();
  _sampleIndex = 0;
  _samples = NULL;
  _value = LOW;

  _notifyEnabled = false;
  _notifyAddress = 0;
  _lastValue = LOW;
}

RODigitalIn::~RODigitalIn()
{

}

bool RODigitalIn::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3];
  int filter = (msg[4]>32 ? 32 : msg[4]);
  unsigned int sampleInterval = ((unsigned int)msg[5] << 8) | (unsigned int)msg[6];

  if (pin<=DI_PIN_MAX_NUMBER && mode<=1 && filter<=255) {

    _pin = pin;
    _mode = mode;
    _filter = filter;

    if (_filter>1) {
      _samples = new int[filter];
      _sampleIndex = 0;
      for (int n=0 ; n < _filter ; n++) _samples[n]=LOW;
    }

#if defined(OPT_UD_ALADDIN)
    pinMode(_pin, INPUT); // use pulldown resister on aladdin board
#else
    pinMode(_pin, (_mode ? INPUT : INPUT_PULLUP));
#endif
    return true;
  } 
  return false;
}

void RODigitalIn::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // READ
      
    int val = read();
    byte rsp[] = {0x01, 0x81, _pin, val, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    
  } else if (cmd==0x02) {

    // SET NOTIFICATION

    int enable = msg[3];
    if (enable) {
      _notifyEnabled = true;
      _notifyAddress = address_from;
      _lastValue = read();
    } else {
      _notifyEnabled = false;
    }
    byte rsp[] = {0x01, 0x82, _pin, _notifyEnabled, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  }
}

void RODigitalIn::process()
{
  if (_pin == -1) return; 

  // sample
  unsigned long thisSampleTime = micros();
  if (_filter > 1 && (thisSampleTime - _lastSampleTime > _sampleInterval || thisSampleTime < _lastSampleTime)) {
    
    _lastSampleTime = thisSampleTime;
    
    int val = digitalRead(_pin);
    _samples[_sampleIndex] = val;
    _sampleIndex++;
    if (_sampleIndex == _filter) _sampleIndex = 0;
  }

  // notify
  if (_notifyEnabled) {
    int val = read();
    if (val != _lastValue) {
      byte rsp[] = {0x01, 0x83, _pin, val, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);
      _lastValue = val;
    }
  }
}

int RODigitalIn::read()
{
    if (_filter > 1) {
    
      // with filter
      int sum = 0;
      for (int n=0 ; n < _filter ; n++) sum += _samples[n];
      if (sum==0)
        _value = LOW;  // steady low state
      else if (sum==_filter)
        _value = HIGH; // steady high state
    
    } else {

      // without filter
      _value = digitalRead(_pin);
    }

    return _value;
}

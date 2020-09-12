#include "ROAnalogIn.h"
#include "RiocMessager.h"

ROAnalogIn::ROAnalogIn()
{
  _pin = -1;
  _mode = -1;
  _filter = -1;
  _sampleInterval = 0;
  _lastSampleTime = micros();
  _sampleIndex = 0;
  _samples = NULL;

  _notifyEnabled = false;
  _notifyAddress = 0;
  _interval = 0;
  _significantBitMask = 0;
  _lastValue = 0;
  _lastTime = millis();
}


ROAnalogIn::~ROAnalogIn()
{
}

bool ROAnalogIn::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3];  // reserved
  int filter = (msg[4]>32 ? 32 : msg[4]);
  unsigned int sampleInterval = ((unsigned int)msg[5] << 8) | (unsigned int)msg[6];

  if (pin>=A0 && pin<=AI_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode;
    _filter = filter;
    _sampleInterval = sampleInterval; // microseconds
    
    if (_filter>1) {
      _samples = new unsigned int[filter];
      _sampleIndex = 0;
      for (int n=0 ; n < _filter ; n++) _samples[n]=0;
    }

    #if defined (__SAM3X8E__)
    analogReadResolution(12);
    #endif

    return true;
  } 
  return false;
}

void ROAnalogIn::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // READ
      
    unsigned int val = read();

    byte rsp[] = {0x03, 0x81, _pin, val>>8, val & 0xff, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    
  } else if (cmd==0x02) {

    // SET NOTIFICATION

    int enable = msg[3];
    unsigned int interval = ((unsigned int)msg[4]<<8) | (unsigned int)msg[5];
    int significantBits = (msg[6]>16 ? 16 : msg[6]);
    
    if (enable) {
      _notifyEnabled = true;
      _notifyAddress = address_from;
      _interval = interval;
      _significantBitMask = 0xffff << (16-significantBits);
      _lastValue = read();
      _lastTime = millis();
      byte rsp[] = {0x03, 0x82, _pin, _notifyEnabled, _interval>>8, _interval & 0xff, significantBits, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    } else {
      _notifyEnabled = false;
      byte rsp[] = {0x03, 0x82, _pin, _notifyEnabled, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
    
  }
}

void ROAnalogIn::process()
{
  if (_pin == -1) return; 

  // sample
  unsigned long thisSampleTime = micros();
  if (_filter > 1 && (thisSampleTime - _lastSampleTime > _sampleInterval || thisSampleTime < _lastSampleTime)) {
    
    _lastSampleTime = thisSampleTime;
    
    unsigned int val = analogRead(_pin);

    #if defined (__SAM3X8E__)
    val = map(val, 0, 4095, 0, 0xffff);
    #else
    val = map(val, 0, 1023, 0, 0xffff);
    #endif

    _samples[_sampleIndex] = val;
    _sampleIndex++;
    if (_sampleIndex == _filter) _sampleIndex = 0;
  }

  // notify
  unsigned long thisTime = millis();
  if (_notifyEnabled && ( thisTime - _lastTime > _interval || thisTime < _lastTime )) {
    
    unsigned int val = read();
    if (_significantBitMask==0 || (val & _significantBitMask) != (_lastValue & _significantBitMask)) {
      
      byte rsp[] = {0x03, 0x83, _pin, val>>8, val & 0xff, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);
      _lastValue = val;
      _lastTime = thisTime;
    }
  }
}

unsigned int ROAnalogIn::read()
{
    unsigned int val;
    if (_filter > 1) {
    
      // with filter
      long sum = 0;
      for (int n=0 ; n < _filter ; n++) sum += (long)_samples[n];
      val = sum / _filter;
    
    } else {

      // without filter
      val = analogRead(_pin);

      #if defined (__SAM3X8E__)
      val = map(val, 0, 4095, 0, 0xffff);
      #else
      val = map(val, 0, 1023, 0, 0xffff);
      #endif
    }

    return val;
}

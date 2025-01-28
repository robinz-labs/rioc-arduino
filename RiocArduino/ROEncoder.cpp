#include "ROEncoder.h"
#include "RiocMessager.h"

ROEncoder::ROEncoder()
{
  _pinA = -1;
  _pinB = -1;
  _mode = -1;

  _value = 0;
  _rangeLower = -0xffffff;
  _rangeUpper =  0xffffff;
  _sampleInterval = 0;
  _lastSampleTime = micros();
  _valA = HIGH;
  _valB = HIGH;

  _notifyEnabled = false;
  _notifyAddress = 0;
  _interval = 0;
  _significantBitMask = 0;
  _lastValue = 0;
  _lastTime = millis();

}


ROEncoder::~ROEncoder()
{
}

bool ROEncoder::setup(byte msg[8], byte address_from)
{
  if (_pinA != -1) return false; 


  int pinA = msg[2];
  int pinB = msg[3];
  int mode = msg[4]; // reserved
  unsigned int sampleInterval = ((unsigned int)msg[5] << 8) | (unsigned int)msg[6];

  if (pinA<=DI_PIN_MAX_NUMBER && pinB<=DI_PIN_MAX_NUMBER && pinA!=pinB) {

    _pinA = pinA;
    _pinB = pinB;
    _mode = mode;
    _sampleInterval = sampleInterval; // microseconds

    _value = 0;
    _valA = HIGH;
    _valB = HIGH;

#if defined(OPT_UD_ALADDIN)
    pinMode(_pinA, INPUT); // use pulldown resister on aladdin board
    pinMode(_pinB, INPUT);
#else
    pinMode(_pinA, (_mode ? INPUT : INPUT_PULLUP));
    pinMode(_pinB, (_mode ? INPUT : INPUT_PULLUP));
#endif

    return true;
  } 
  
  return false;
}

void ROEncoder::execute(byte msg[8], byte address_from)
{
  if (_pinA == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // READ
    byte sig = (_value>=0 ? 0 : 1);
    long val = (_value>=0 ? _value : -_value);

    byte rsp[] = {0x21, 0x81, _pinA, sig, val>>16, val>>8, val & 0xff, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    
  } else if (cmd==0x02) {

    // SET NOTIFICATION

    int enable = msg[3];
    unsigned int interval = ((unsigned int)msg[4]<<8) | (unsigned int)msg[5];
    int significantBits = (msg[6]>24 ? 24 : msg[6]);
    
    if (enable) {
      _notifyEnabled = true;
      _notifyAddress = address_from;
      _interval = interval;
      _significantBitMask = (0xffffff << (24-significantBits)) & 0xffffff;
      _lastValue = _value;
      _lastTime = millis();
      byte rsp[] = {0x21, 0x82, _pinA, _notifyEnabled, _interval>>8, _interval & 0xff, significantBits, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    } else {
      _notifyEnabled = false;
      byte rsp[] = {0x21, 0x82, _pinA, _notifyEnabled, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
    
  } else if (cmd==0x04) {
      
    // WRITE
    byte sig = (msg[3] ? 1 : 0);
    long val = ((long)msg[4]<<16) | ((long)msg[5]<<8) | (long)msg[6];
    _value = (sig ? -1 : 1)*val;
    
    if (!isSilent()) {
      byte rsp[] = {0x21, 0x84, _pinA, sig, val>>16, val>>8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x05) {
    
    // SET RANGE LOWER
    byte sig = (msg[3] ? 1 : 0);
    long val = ((long)msg[4]<<16) | ((long)msg[5]<<8) | (long)msg[6];
    _rangeLower = (sig ? -1 : 1)*val;
    
    if (!isSilent()) {
      byte rsp[] = {0x21, 0x85, _pinA, sig, val>>16, val>>8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x06) {
    
    // SET RANGE UPPER
    byte sig = (msg[3] ? 1 : 0);
    long val = ((long)msg[4]<<16) | ((long)msg[5]<<8) | (long)msg[6];
    _rangeUpper = (sig ? -1 : 1)*val;
    
    if (!isSilent()) {
      byte rsp[] = {0x21, 0x86, _pinA, sig, val>>16, val>>8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  }
}

void ROEncoder::process()
{
  if (_pinA == -1) return; 

  // sample
  unsigned long thisSampleTime = micros();
  if (thisSampleTime - _lastSampleTime > _sampleInterval || thisSampleTime < _lastSampleTime) {
    _lastSampleTime = thisSampleTime;

    int turn = getEncoderTurn();
    if (turn!=0) {
      _value += (long)turn; 
      _value = constrain(_value, _rangeLower, _rangeUpper);
    } 
  }

  // notify
  unsigned long thisTime = millis();
  if (_notifyEnabled && ( thisTime - _lastTime > _interval || thisTime < _lastTime )) {
    
    byte thisSign = (_value>=0 ? 0 : 1);
    unsigned long thisValue = (_value>=0 ? _value : -_value);

    byte lastSign = (_lastValue>=0 ? 0 : 1);
    unsigned long lastValue = (_lastValue>=0 ? _lastValue : -_lastValue);

    if (_significantBitMask==0 || 
        thisSign != lastSign ||
        (thisValue & _significantBitMask) != (lastValue & _significantBitMask)) {
      
      byte rsp[] = {0x21, 0x83, _pinA, thisSign, thisValue>>16, thisValue>>8, thisValue & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);
      _lastValue = _value;
      _lastTime = thisTime;
    }
  }
}

long ROEncoder::read()
{
  return _value;
}

void ROEncoder::write(long value)
{
  _value = value;
}

int ROEncoder::getEncoderTurn()
{
  // return -1, 0, or +1
  int result = 0;
  int valA = digitalRead(_pinA);
  int valB = digitalRead(_pinB);
  if (valA != _valA)
  {
    // something has changed
    if (_valA == HIGH && valA == LOW)
    {
      result = -(_valB * 2 - 1);
    }
  }
  _valA = valA;
  _valB = valB;
  return result;
} 

#include "ROTone.h"
#include "RiocMessager.h"
#include "RiocOptional.h"

ROTone::ROTone()
{
  _pin = -1;
  _mode = -1;

  // variables for software tone
  _halfPeriod = 0;  // zero means tone playing stopped
  _lastShiftTime = 0;
  _lastShiftValue = LOW;
}

ROTone::~ROTone()
{
}

bool ROTone::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3]; // reserved
  if (pin<=DO_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode;

    #ifdef OPT_SOFTWARE_TONE
    pinMode(_pin, OUTPUT);
    #endif

    return true;
  } 
  return false;
}

void ROTone::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
    
    // PLAY
    
    unsigned int frequency = ((unsigned int)msg[3] << 8) | (unsigned int)msg[4];
    unsigned int duration = ((unsigned int)msg[5] << 8) | (unsigned int)msg[6];
    frequency = constrain(frequency, 20, 20000);

    play(frequency, duration);

    if (!isSilent()) {
      byte rsp[] = {0x31, 0x81, _pin, frequency >> 8, frequency & 0xff, duration >> 8, duration & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x02) {

    // STOP

    stop();

    if (!isSilent()) {
      byte rsp[] = {0x31, 0x82, _pin, 0, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
  } 
}

void ROTone::process()
{
  #ifdef OPT_SOFTWARE_TONE

  if (_pin == -1 || _halfPeriod==0) return; 

  // shift output value to generate tone pulses
  unsigned long thisTime = micros();
  if (thisTime - _lastShiftTime >= _halfPeriod || thisTime < _lastShiftTime) {
    
    int thisShiftValue = !_lastShiftValue;
    digitalWrite(_pin, thisShiftValue);

    _lastShiftTime = thisTime;
    _lastShiftValue = thisShiftValue;
  }

  #endif
}

void ROTone::play(unsigned int frequency, unsigned int duration)
{
  if (duration==0) {

    #ifdef OPT_SOFTWARE_TONE
      _halfPeriod = 1000000L / frequency / 2;
      _lastShiftTime = micros();
      _lastShiftValue = LOW;
    #else
      tone(_pin, frequency);
    #endif

  } else {
    
    #ifdef OPT_SOFTWARE_TONE
      _halfPeriod = 0;
      digitalWrite(_pin, LOW);
      playTone(frequency, duration);
    #else
      tone(_pin, frequency, duration);
      delay(duration);
    #endif
  }
}

void ROTone::stop()
{
  #ifdef OPT_SOFTWARE_TONE
    _halfPeriod = 0;
    digitalWrite(_pin, LOW);
  #else
    noTone(_pin);
  #endif
}

void ROTone::playTone(unsigned int frequency, unsigned int duration)
{
  unsigned int period = 1000000L / frequency;
  unsigned int halfPeriod = period / 2;
  for (long n = 0; n < duration * 1000L; n += period) {
    digitalWrite(_pin, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(_pin, LOW);
    delayMicroseconds(halfPeriod);
  }
}


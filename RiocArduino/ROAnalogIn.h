#ifndef LIB_RO_ANALOG_IN_H_
#define LIB_RO_ANALOG_IN_H_

#include "RiocObject.h"

class ROAnalogIn : public RiocObject {
		
public:
  ROAnalogIn();
  ~ROAnalogIn();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();
		
  unsigned int read();

private:  
  int _pin;
  int _mode;
  int _filter;
  unsigned int _sampleInterval;
  unsigned long _lastSampleTime;
  int _sampleIndex;
  unsigned int* _samples;

  bool _notifyEnabled;
  int _notifyAddress;
  unsigned int _interval;
  unsigned int _significantBitMask;
  unsigned int _lastValue;
  unsigned long _lastTime;

};


#endif // LIB_RO_ANALOG_IN_H_

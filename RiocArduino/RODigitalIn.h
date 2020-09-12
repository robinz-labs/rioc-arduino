#ifndef LIB_RO_DIGITAL_IN_H_
#define LIB_RO_DIGITAL_IN_H_

#include "RiocObject.h"

class RODigitalIn : public RiocObject {
		
public:
  RODigitalIn();
  ~RODigitalIn();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  int read();

private: 
  int _pin;
  int _mode;
  int _filter;
  unsigned int _sampleInterval;
  unsigned long _lastSampleTime;
  int _sampleIndex;
  int* _samples;
  int _value;

  bool _notifyEnabled;
  int _notifyAddress;
  int _lastValue;
};


#endif // LIB_RO_DIGITAL_IN_H_

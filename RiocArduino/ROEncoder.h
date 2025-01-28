#ifndef LIB_RO_ENCODER_H_
#define LIB_RO_ENCODER_H_

#include "RiocObject.h"

class ROEncoder : public RiocObject {
		
public:
  ROEncoder();
  ~ROEncoder();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  long read();
  void write(long value);
		
private:  
  int _pinA;
  int _pinB;
  int _mode;

  long _value;
  long _rangeLower;
  long _rangeUpper;
  unsigned int _sampleInterval;
  unsigned long _lastSampleTime;
  int _valA;
  int _valB;

  bool _notifyEnabled;
  int _notifyAddress;
  unsigned int _interval;
  unsigned long _significantBitMask;
  long _lastValue;
  unsigned long _lastTime;

  int getEncoderTurn();
};


#endif // LIB_RO_ENCODER_H_

#ifndef LIB_RO_MULTIPLE_DIGITAL_IN_H_
#define LIB_RO_MULTIPLE_DIGITAL_IN_H_

#include "RiocObject.h"

class ROMultipleDigitalIn : public RiocObject {
		
public:
  ROMultipleDigitalIn();
  ~ROMultipleDigitalIn();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  unsigned long read();
		
private:  
  int _pin;
  int _number;
  int _mode;

  bool _notifyEnabled;
  int _notifyAddress;
  unsigned int _interval;
  unsigned long _lastValue;
  unsigned long _lastTime;
};


#endif // LIB_RO_MULTIPLE_DIGITAL_IN_H_

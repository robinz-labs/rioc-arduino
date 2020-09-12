#ifndef LIB_RO_ANALOG_OUT_H_
#define LIB_RO_ANALOG_OUT_H_

#include "RiocObject.h"

class ROAnalogOut : public RiocObject {
		
public:
  ROAnalogOut();
  ~ROAnalogOut();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void write(unsigned int val);  // val: 0~65535 

private:  
  int _pin;
  int _mode;

  unsigned int _val;
};


#endif // LIB_RO_ANALOG_OUT_H_

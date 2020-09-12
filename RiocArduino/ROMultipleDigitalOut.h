#ifndef LIB_RO_MULTIPLE_DIGITAL_OUT_H_
#define LIB_RO_MULTIPLE_DIGITAL_OUT_H_

#include "RiocObject.h"

class ROMultipleDigitalOut : public RiocObject {
		
public:
  ROMultipleDigitalOut();
  ~ROMultipleDigitalOut();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void write(unsigned long val);
		
private:  
  int _pin;
  int _number;
  int _mode;

  unsigned long _val;
};


#endif // LIB_RO_MULTIPLE_DIGITAL_OUT_H_

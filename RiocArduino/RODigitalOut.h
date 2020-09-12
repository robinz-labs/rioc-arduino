#ifndef LIB_RO_DIGITAL_OUT_H_
#define LIB_RO_DIGITAL_OUT_H_

#include "RiocObject.h"

class RODigitalOut : public RiocObject {

public:
  RODigitalOut();
  ~RODigitalOut();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void write(int val);              // val: 0 or 1 
  void writePWM(int pwm);           // pwm: 0~255
  void pulse(long pw, int val = 0); // pw: in microseconds val: 0 or 1 

private:  
  int _pin;
  int _mode;

  int _val;
  int _pwm;
};

#endif // LIB_RO_DIGITAL_OUT_H_

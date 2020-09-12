#ifndef LIB_RO_ULTRASONIC_RANGER_H_
#define LIB_RO_ULTRASONIC_RANGER_H_

#include "RiocObject.h"

class Servo;

class ROUltrasonicRanger : public RiocObject {

public:
  ROUltrasonicRanger();
  ~ROUltrasonicRanger();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  unsigned int read();  // return the distance in centimeters

private:  
  int _pinOut;
  int _pinIn;
  int _mode;

};

#endif // LIB_RO_ULTRASONIC_RANGER_H_

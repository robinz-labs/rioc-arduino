#ifndef LIB_RO_STEPPER_H_
#define LIB_RO_STEPPER_H_

#include "RiocObject.h"

#define STEPPER_MODE_NORMAL   0
#define STEPPER_MODE_PUL_DIR  1
#define STEPPER_MODE_PUL_DIR_ 2

class Stepper;

class ROStepper : public RiocObject {

public:
  ROStepper();
  ~ROStepper();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void step(unsigned int steps, int dir); // steps: 0~0x7fff dir: 0 or 1
  void stepTo(long position);             // position: -0xffffff~0xffffff
  void stop();
  void setSpeed(unsigned int speed);      // speed: steps per second
  void setPosition(long position);        // position: -0xffffff~0xffffff
  long getPosition();

private:  
  int _pin1; // PUL line for mode 1       or A+ line for mode 0
  int _pin2; // DIR line for mode 1          A- line for mode 0
  int _pin3; //                              B+ line for mode 0
  int _pin4; //                              B- line for mode 0
  int _mode;

  unsigned int _speed;
  unsigned int _stepInterval;
  unsigned int _lastStepInterval;
  unsigned long _lastStepTime;
  long _positionCurrent;
  long _positionTarget;

  Stepper* _stepper; // only for mode 0
  int _dir;

  void takeOneStep(int dir);

};

#endif // LIB_RO_STEPPER_H_

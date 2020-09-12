#ifndef LIB_RO_MOTOR_H_
#define LIB_RO_MOTOR_H_

#include "RiocObject.h"

#define MOTOR_MODE_NORMAL  0
#define MOTOR_MODE_PWM_DIR 1

class ROMotor : public RiocObject {

public:
  ROMotor();
  ~ROMotor();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void run(int power, int dir = 0);  // power: 0~255 dir: 0 or 1 

private:  
  int _pin1;
  int _pin2;
  int _mode;

  int _dir;
  int _power;

};

#endif // LIB_RO_MOTOR_H_

#ifndef LIB_RO_RUDDER_SERVO_H_
#define LIB_RO_RUDDER_SERVO_H_

#include "RiocObject.h"

class Servo;

class RORudderServo : public RiocObject {

public:
  RORudderServo();
  ~RORudderServo();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void setAngle(int angle);       // angle: 0~180
  void setEnabled(bool enabled);  // enabled: true or false

private:  
  int _pin;
  int _mode;
  bool _enabled;

  Servo* _servo;
};

#endif // LIB_RO_RUDDER_SERVO_H_

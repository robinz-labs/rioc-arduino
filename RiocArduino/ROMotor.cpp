#include "ROMotor.h"
#include "RiocMessager.h"

ROMotor::ROMotor()
{
  _pin1 = -1;
  _pin2 = -1;
  _mode = -1;

  _dir = 0;
  _power = 0;
}


ROMotor::~ROMotor()
{
}

bool ROMotor::setup(byte msg[8], byte address_from)
{
  if (_pin1 != -1) return false;

  int pin1 = msg[2];
  int pin2 = msg[3];
  int mode = msg[4];

  if (pin1<=DO_PIN_MAX_NUMBER && pin2<=DO_PIN_MAX_NUMBER && pin1!=pin2) {

    _pin1 = pin1; // dc v+ or pwm
    _pin2 = pin2; // dc v- or dir
    _mode = mode; // mode 0: normal dc motor, mode 1: motor driven with pwm and dir pins

    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);

    return true;
  } 
  return false;
}

void ROMotor::execute(byte msg[8], byte address_from)
{
  if (_pin1 == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {

    // RUN
    
    int dir = (msg[3] ? 1 : 0);
    int power = msg[4];
    run(power, dir);

    if (!isSilent()) {
      byte rsp[] = {0x11, 0x81, _pin1, _dir, _power, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }  
  } else if (cmd==0x02) {

    // READ

    byte rsp[] = {0x11, 0x82, _pin1, _dir, _power, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  }
}

void ROMotor::process()
{    
}

void ROMotor::run(int power, int dir)
{
    _power = power;
    _dir = dir;

    #if defined(__SAM3X8E__)
    analogWriteResolution(8);
    #endif

    if (_mode == MOTOR_MODE_NORMAL) {

      if (_dir) {
        analogWrite(_pin1, 0);
        analogWrite(_pin2, _power);
      } else {
        analogWrite(_pin1, _power);
        analogWrite(_pin2, 0);      
      } 
    } else if (_mode == MOTOR_MODE_PWM_DIR) {

      analogWrite(_pin1, _power);
      digitalWrite(_pin2, (_dir ? HIGH : LOW));
    }
}

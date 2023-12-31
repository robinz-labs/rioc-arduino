#include "ROStepper.h"
#include "RiocMessager.h"
#include <Stepper.h>

#define STEP_INTERVAL_START         5000
#define STEP_INTERVAL_START_FADING  20
#define STEP_INTERVAL_STOP          2500
#define STEP_INTERVAL_STOP_FADING   100

ROStepper::ROStepper()
{
  _pin1 = -1;
  _pin2 = -1;
  _pin3 = -1;
  _pin4 = -1;
  _mode = -1;

  _stepper = NULL;
  _dir = 0;

  _speed = 100;
  _stepInterval = 1000000 / _speed;
  _lastStepInterval = STEP_INTERVAL_START;
  _lastStepTime = micros();
  _positionCurrent = 0;
  _positionTarget = 0;
}

ROStepper::~ROStepper()
{
}

bool ROStepper::setup(byte msg[8], byte address_from)
{
  if (_pin1 != -1) return false;

  int pin1 = msg[2];
  int pin2 = msg[3];
  int pin3 = msg[4];
  int pin4 = msg[5];  
  int mode = msg[6];

  if (mode == STEPPER_MODE_NORMAL) {

    if (pin1<=DO_PIN_MAX_NUMBER && pin2<=DO_PIN_MAX_NUMBER && 
        pin3<=DO_PIN_MAX_NUMBER && pin4<=DO_PIN_MAX_NUMBER &&
        pin1!=pin2 && pin1!=pin3 && pin1!=pin4 && 
        pin2!=pin3 && pin2!=pin4 && pin3!=pin4 ) {

      _pin1 = pin1; // A+
      _pin2 = pin2; // A-
      _pin3 = pin3; // B+
      _pin4 = pin4; // B-
      _mode = mode;

      _stepper = new Stepper(60, _pin1, _pin2, _pin3, _pin4); // need set steps per rotation as 60
      _stepper->setSpeed(_speed);

      return true;
    }
  
  } else if (mode == STEPPER_MODE_PUL_DIR || mode == STEPPER_MODE_PUL_DIR_) {

    if (pin1<=DO_PIN_MAX_NUMBER && pin2<=DO_PIN_MAX_NUMBER &&
        pin1!=pin2) {

      _pin1 = pin1; // PUL
      _pin2 = pin2; // DIR
      _mode = mode;

      pinMode(_pin1, OUTPUT);
      pinMode(_pin2, OUTPUT);

      digitalWrite(_pin1, (mode == STEPPER_MODE_PUL_DIR_ ? HIGH : LOW));
    
      return true;
    }
  } 

  return false;
}

void ROStepper::execute(byte msg[8], byte address_from)
{
  if (_pin1 == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {

    // STEP

    int dir = (msg[3] ? 1 : 0);
    unsigned int steps = ((unsigned int)msg[4] << 8) | (unsigned int)msg[5];
    steps = constrain(steps, 0, 0x7fff);

    step(steps, dir);

    if (!isSilent()) {
      byte rsp[] = {0x12, 0x81, _pin1, dir, steps >> 8, steps & 0xff, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }  
  
  } else if (cmd==0x02) {

    // GOTO

    long position = (msg[3] ? -1 : 1) * 
                    (((long)msg[4] << 16) | ((long)msg[5] << 8) | (long)msg[6]);
    stepTo(position);

    if (!isSilent()) {
      byte sig = (_positionTarget>=0 ? 0 : 1);
      long val = (_positionTarget>=0 ? _positionTarget : -_positionTarget);
      byte rsp[] = {0x12, 0x82, _pin1, sig, val >> 16, val >> 8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }  
  
  } else if (cmd==0x03) {
      
    // STOP

    stop();

    if (!isSilent()) {
      byte rsp[] = {0x12, 0x83, _pin1, 0, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x04) {
      
    // SET SPEED

    setSpeed(((unsigned int)msg[3]<<8) | (unsigned int)msg[4]);
    
    if (!isSilent()) {
      byte rsp[] = {0x12, 0x84, _pin1, _speed>>8, _speed & 0xff, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x05) {
      
    // GET SPEED

    byte rsp[] = {0x12, 0x85, _pin1, _speed>>8, _speed & 0xff, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  
  } else if (cmd==0x06) {
    
    // SET POSITION

    byte sig = (msg[3] ? 1 : 0);
    long val = ((long)msg[4]<<16) | ((long)msg[5]<<8) | (long)msg[6];
    long position = (sig ? -1 : 1)*val;
    setPosition(position);
    
    if (!isSilent()) {
      byte rsp[] = {0x12, 0x86, _pin1, sig, val>>16, val>>8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x07) {
      
    // GET POSITION

    byte sig = (_positionCurrent>=0 ? 0 : 1);
    long val = (_positionCurrent>=0 ? _positionCurrent : -_positionCurrent);

    byte rsp[] = {0x12, 0x87, _pin1, sig, val>>16, val>>8, val & 0xff, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  }
}

void ROStepper::process()
{
  if (_pin1 == -1) return; 

  // step
  unsigned long thisTime = micros();

  if (_positionCurrent != _positionTarget && ( thisTime - _lastStepTime > _lastStepInterval || thisTime < _lastStepTime )) {
    
    int dir = (_positionCurrent > _positionTarget ? 1 : 0);
    takeOneStep(dir);

    _positionCurrent += (dir ? -1 : 1);
    _positionCurrent = constrain(_positionCurrent, -0xffffff, 0xffffff);
    _lastStepTime = thisTime;
    
    if (_lastStepInterval > _stepInterval) {
      
      // speed up (fading)
      _lastStepInterval += -STEP_INTERVAL_START_FADING; 
      if (_lastStepInterval < _stepInterval) _lastStepInterval = _stepInterval;
      
    } else if (_lastStepInterval < _stepInterval) {
      
      // speed down (asap)
      _lastStepInterval = _stepInterval;
    }

    if (abs(_positionCurrent - _positionTarget)<100) {

      // slow down when approaching
      if (_lastStepInterval < STEP_INTERVAL_STOP) {
          _lastStepInterval += STEP_INTERVAL_STOP_FADING;
      }
    }
  } 
}

void ROStepper::step(unsigned int steps, int dir)
{
  if (_mode == STEPPER_MODE_NORMAL) {

    _stepper->step((dir ? -1: 1) * steps);

  } else if (_mode == STEPPER_MODE_PUL_DIR || _mode == STEPPER_MODE_PUL_DIR_) {
    
    for (int n=0 ; n<steps ; n++) {
      takeOneStep(dir);
      delayMicroseconds(_stepInterval);
    }
  }

  _positionCurrent += (long)(dir ? -1: 1) * (long)steps;
  _positionCurrent = constrain(_positionCurrent, -0xffffff, 0xffffff);
  _positionTarget = _positionCurrent;
}

void ROStepper::stepTo(long position)
{
  bool isStopped = (_positionTarget == _positionCurrent);
  _positionTarget = position;
  if (isStopped) _lastStepInterval = STEP_INTERVAL_START;
}

void ROStepper::stop()
{
  _positionTarget = _positionCurrent;
}

void ROStepper::setSpeed(unsigned int speed)
{
  _speed = speed;
  _stepInterval = 1000000 / _speed; // microseconds per step 

  if (_mode == STEPPER_MODE_NORMAL) _stepper->setSpeed(_speed);
}

void ROStepper::setPosition(long position)
{
  bool isStopped = (_positionTarget == _positionCurrent);
  _positionCurrent = position;
  if (isStopped) _positionTarget = _positionCurrent;
}

long ROStepper::getPosition()
{
  return _positionCurrent;
}

void ROStepper::takeOneStep(int dir)
{
  if (_mode == STEPPER_MODE_NORMAL) {

    _stepper->step(dir ? -1: 1);

  } else if (_mode == STEPPER_MODE_PUL_DIR) {

    if (dir != _dir) {
      _dir = dir;
      digitalWrite(_pin2, dir);
      delayMicroseconds(10);
    }

    digitalWrite(_pin1, HIGH);
    delayMicroseconds(2);
    digitalWrite(_pin1, LOW);
    delayMicroseconds(2);

  } else if (_mode == STEPPER_MODE_PUL_DIR_) {

    if (dir != _dir) {
      _dir = dir;
      digitalWrite(_pin2, !dir);
      delayMicroseconds(10);
    }

    digitalWrite(_pin1, LOW);
    delayMicroseconds(2);
    digitalWrite(_pin1, HIGH);
    delayMicroseconds(2);
  }
}

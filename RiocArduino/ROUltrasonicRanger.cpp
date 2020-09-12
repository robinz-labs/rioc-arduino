#include "ROUltrasonicRanger.h"
#include "RiocMessager.h"

ROUltrasonicRanger::ROUltrasonicRanger()
{
  _pinOut = -1;
  _pinIn = -1;
  _mode = -1;
}


ROUltrasonicRanger::~ROUltrasonicRanger()
{
}

bool ROUltrasonicRanger::setup(byte msg[8], byte address_from)
{
  if (_pinOut != -1) return false;

  int pinOut = msg[2];
  int pinIn = msg[3];
  int mode = msg[4];

  if (pinOut<=DO_PIN_MAX_NUMBER && pinIn<=DI_PIN_MAX_NUMBER && pinOut!=pinIn) {

    _pinOut = pinOut; // echo pin
    _pinIn = pinIn;   // trig pin
    _mode = mode;

    pinMode(_pinOut, OUTPUT);
    pinMode(_pinIn, INPUT);

    return true;
  } 
  return false;
}

void ROUltrasonicRanger::execute(byte msg[8], byte address_from)
{
  if (_pinOut == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {

    // GET DISTANCE

    unsigned int distance = read();

    byte rsp[] = {0x22, 0x81, _pinOut, distance >> 8, distance & 0xff, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  } 
}

void ROUltrasonicRanger::process()
{    
}

unsigned int ROUltrasonicRanger::read()
{
  digitalWrite(_pinOut, LOW); 
  delayMicroseconds(2);
  digitalWrite(_pinOut, HIGH);
  delayMicroseconds(10);
  digitalWrite(_pinOut, LOW);
  unsigned int distance = pulseIn(_pinIn, HIGH, 20000); 
  distance = distance / 58;

  return distance;
}

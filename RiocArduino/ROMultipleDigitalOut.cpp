#include "ROMultipleDigitalOut.h"
#include "RiocMessager.h"


ROMultipleDigitalOut::ROMultipleDigitalOut()
{
  _pin = -1;
  _number = 0;
  _mode = -1;

  _val = 0;
}


ROMultipleDigitalOut::~ROMultipleDigitalOut()
{
}

bool ROMultipleDigitalOut::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int number = msg[3]; 
  int mode = msg[4]; // reserved

  if (number>=1 && number<=32 && 
      pin<=DI_PIN_MAX_NUMBER && (pin+number-1)<=DI_PIN_MAX_NUMBER) {

    _pin = pin;
    _number = number;
    _mode = mode;

    for (int n=_pin ; n <= (_pin+_number-1) ; n++) {
      pinMode(n, OUTPUT);
    }

    return true;
  } 
  
  return false;
}

void ROMultipleDigitalOut::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // WRITE
    
    unsigned long val = (unsigned long)msg[3] | 
                        ((unsigned long)msg[4]<<8) | 
                        ((unsigned long)msg[5]<<16) | ((unsigned long)msg[6]<<24);
    write(val);

    if (!isSilent()) {
      byte rsp[] = {0x07, 0x81, _pin, _val & 0xff, _val>>8, _val>>16, _val>>24, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x02) {
      
    // READ
    
    byte rsp[] = {0x07, 0x82, _pin, _val & 0xff, _val>>8, _val>>16, _val>>24, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  }
}

void ROMultipleDigitalOut::process()
{
    
}

void ROMultipleDigitalOut::write(unsigned long val)
{
  _val = val;
  
  for (int n=0 ; n<_number ; n++) {
    digitalWrite(_pin+n, ((_val & (1<<n))==0 ? LOW : HIGH)); 
  }
}
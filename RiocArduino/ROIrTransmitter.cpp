#include "ROIrTransmitter.h"
#include "RiocMessager.h"
#include "RiocOptional.h"

#ifndef OPT_ENABLE_IR_TRANSMITTER

// no implementation if ir emitter is disabled
ROIrTransmitter::ROIrTransmitter() { }
ROIrTransmitter::~ROIrTransmitter() { }
bool ROIrTransmitter::setup(byte msg[8], byte address_from) { return false; }
void ROIrTransmitter::execute(byte msg[8], byte address_from) { }
void ROIrTransmitter::process() { }

#else

#include <IRremote.h>

// TIMER_PWM_PIN is defined in IRremoteInt.h
#define IR_OUT_PIN TIMER_PWM_PIN


void resetTimer()
{
  cli();

  TIMER_CONFIG_NORMAL();
  TIMER_ENABLE_INTR;
  TIMER_RESET;

  sei(); 
}

ROIrTransmitter::ROIrTransmitter()
{
  _pin = -1;
  _mode = -1;
  _ir = NULL;
}

ROIrTransmitter::~ROIrTransmitter()
{
}

bool ROIrTransmitter::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3]; 
  if (pin<=DO_PIN_MAX_NUMBER && pin==IR_OUT_PIN) {

    _pin = pin;
    _mode = mode; // reserved

    _ir = new IRsend();
    return true;
  } 
  return false;
}

void ROIrTransmitter::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // SEND
    
    int format = msg[3]; // reserved
    byte data[] = {msg[4], msg[5], msg[6], msg[7]}; // 32bit data will be sent by IR LED
    
    uint32_t dataAll = 
      ((uint32_t)data[0]<<24) | ((uint32_t)data[1]<<16) | 
      ((uint32_t)data[2]<<8)  | (uint32_t)data[3];
    _ir->sendNEC(dataAll, 32);

    resetTimer();

    if (!isSilent()) {
      byte rsp[] = {0x51, 0x81, _pin, format, data[0], data[1], data[2], data[3]};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
  } 
}

void ROIrTransmitter::process()
{

}

#endif

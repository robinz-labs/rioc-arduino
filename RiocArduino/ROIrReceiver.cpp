#include "ROIrReceiver.h"
#include "RiocMessager.h"
#include "RiocOptional.h"

#ifndef OPT_ENABLE_IR_RECEIVER

// no implementation if ir receiver is disabled
ROIrReceiver::ROIrReceiver() { }
ROIrReceiver::~ROIrReceiver() { }
bool ROIrReceiver::setup(byte msg[8], byte address_from) { return false; }
void ROIrReceiver::execute(byte msg[8], byte address_from) { }
void ROIrReceiver::process() { }

#else

#include <IRremote.h>

ROIrReceiver::ROIrReceiver()
{
  _pin = -1;
  _mode = -1;
  _notifyAddress = 0;
  _ir = NULL;
}

ROIrReceiver::~ROIrReceiver()
{
}

bool ROIrReceiver::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3]; 
  if (pin<=DI_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode; // reserved
    _notifyAddress = address_from;

    _ir = new IRrecv(_pin);
    _ir->enableIRIn();
    return true;
  } 
  return false;
}

void ROIrReceiver::execute(byte msg[8], byte address_from)
{
}

void ROIrReceiver::process()
{
  if (_pin == -1) return; 

  static decode_results results;

  if (_ir->decode(&results)) {
    uint32_t dataAll = results.value;
    byte data[] = {
      (dataAll>>24)&0xff, 
      (dataAll>>16)&0xff, 
      (dataAll>>8)&0xff, 
      (dataAll)&0xff };

    byte format = 0; // reserved

    byte rsp[] = {0x52, 0x81, _pin, format, data[0], data[1], data[2], data[3]};
    if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);

    _ir->resume(); // receive the next value
  }
}

#endif

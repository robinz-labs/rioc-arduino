#ifndef LIB_RO_IR_TRANSMITTER_H_
#define LIB_RO_IR_TRANSMITTER_H_

#include "RiocObject.h"

class IRsend;

class ROIrTransmitter : public RiocObject {

public:
  ROIrTransmitter();
  ~ROIrTransmitter();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

private:  
  int _pin;
  int _mode;

  IRsend* _ir;
};

#endif // LIB_RO_IR_TRANSMITTER_H_

#ifndef LIB_RO_IR_RECEIVER_H_
#define LIB_RO_IR_RECEIVER_H_

#include "RiocObject.h"

class IRrecv;

class ROIrReceiver : public RiocObject {

public:
  ROIrReceiver();
  ~ROIrReceiver();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

private:  
  int _pin;
  int _mode;
  int _notifyAddress;

  IRrecv* _ir;
};


#endif // LIB_RO_IR_RECEIVER_H_

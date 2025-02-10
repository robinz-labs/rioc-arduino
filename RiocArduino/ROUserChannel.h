#ifndef LIB_RO_USER_CHANNEL_H_
#define LIB_RO_USER_CHANNEL_H_

#include "RiocObject.h"

class ROUserChannel : public RiocObject {
		
public:
  ROUserChannel();
  ~ROUserChannel();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  uint32_t read();
  void write(uint32_t value);
  int mode();

private:  

  int _channel;
  int _mode;
  uint32_t _value;

  bool _notifyEnabled;
  int _notifyAddress;
  unsigned int _interval;
  uint32_t _lastValue;
  uint32_t _lastTime;
};


#endif // LIB_RO_USER_CHANNEL_H_

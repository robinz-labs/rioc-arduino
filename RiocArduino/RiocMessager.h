#ifndef LIB_RIOC_MESSAGER_H_
#define LIB_RIOC_MESSAGER_H_

#include <Arduino.h>

#define INTERNAL_MESSAGING 0xff

class RiocMessager {
		
public:
  RiocMessager(byte address = 1);
  ~RiocMessager();
	
  virtual void begin();
  virtual void sendMessage(byte msg[8], byte address_to);				
  virtual void process();

  void clearInternalMessage();
  void setInternalMessage(byte msg[8]);
  byte * getInternalMessage();

  // set callbacks on receiving messages
  void setCallbackMessageReceived(void (*fptr)(byte msg[8], byte address_from));
	
protected:
  byte _internalMessage[8];
  bool _internalMessageReceived;
  void (*_callbackMessageReceived)(byte msg[8], byte address_from);
  
};

#endif // LIB_RIOC_MESSAGER_H_

#ifndef LIB_RIOC_SERIAL_MESSAGER_H_
#define LIB_RIOC_SERIAL_MESSAGER_H_

#include "RiocMessager.h"

#define RIOC_SERIAL Serial

class RiocSerialMessager : public RiocMessager {
		
public:
  RiocSerialMessager(byte address = 1);
  ~RiocSerialMessager();
	
  void begin();
  void sendMessage(byte msg[8], byte address_to);				
  void process();
	
private:  
  byte _address;     // address of present messager
  byte _bufIn[32];   // buffer to receive bytes
  int  _lenIn;       // length of the received bytes
  
  bool _syncMsgEnabled;  // enable receiving sync messages
  unsigned long _syncMsgBeginTime; 
                         // time to begin receiving sync messages (milliseconds)
  byte _syncMsg[320];    // cache space for sync messages received
  int  _syncMsgCount;    // number of the sync messages received
  byte _syncMsgFrom;     // source address of the sync messages received

  void parse(byte* bytes, int length);
  void send(byte* bytes, int length);

  void finishSyncMessaging();

};

// extern RiocSerialMessager Messager;

#endif // LIB_RIOC_SERIAL_MESSAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "RiocSerialMessager.h"

#define SYNC_MSG_MAX_COUNT  40
#define SYNC_MSG_TIMEOUT    5000

byte checksum(byte *bytes, int length)
{
  byte checksum = 0;
  for (int n=0 ; n<length ; n++) {
    checksum += bytes[n];
  }
  return ~checksum + 1; 
}

RiocSerialMessager::RiocSerialMessager(byte address)
{
  _address = address;
  
  // clear buffer to receive bytes
  _lenIn = 0; 
  
  // initialise callbacks to NULL pointer
  _callbackMessageReceived = NULL;

  // clear sync messages
  _syncMsgEnabled = false;
  _syncMsgBeginTime = 0;
  _syncMsgCount = 0;
  _syncMsgFrom = 0;
}


RiocSerialMessager::~RiocSerialMessager()
{

}

void RiocSerialMessager::begin()
{

}

void RiocSerialMessager::sendMessage(byte msg[8], byte address_to)
{
  if (address_to == INTERNAL_MESSAGING) {
    setInternalMessage(msg);
    return;
  }

  static byte bufOut[14];
  
  bufOut[0] = 0x5a;        // 0  leading character
  bufOut[1] = 0x00;        // 1  data frame version
  bufOut[2] = _address;    // 2  source station address
  bufOut[3] = address_to;  // 3  destination station address
  bufOut[4] = msg[0];      // 4  data 0
  bufOut[5] = msg[1];      // 5  data 1
  bufOut[6] = msg[2];      // 6  data 2
  bufOut[7] = msg[3];      // 7  data 3
  bufOut[8] = msg[4];      // 8  data 4
  bufOut[9] = msg[5];      // 9  data 5
  bufOut[10] = msg[6];     // 10 data 6
  bufOut[11] = msg[7];     // 11 data 7
  bufOut[12] = checksum((byte*)(bufOut+2), 10);  
                           // 12 checksum
  bufOut[13] = 0xa5;       // 13 ending character

  send((byte*)bufOut, sizeof(bufOut));
}

void RiocSerialMessager::send(byte* bytes, int length)
{
  for (int n=0 ; n<length ; n++) RIOC_SERIAL.write(bytes[n]); 
}

void RiocSerialMessager::process()
{
  static int frameLength = 14;

  while (RIOC_SERIAL.available() > 0) {
    byte c = RIOC_SERIAL.read();
      
    if (_lenIn >= sizeof(_bufIn)) _lenIn = 0; // buffer is full, then clear the buffer
    _bufIn[_lenIn] = c;
    _lenIn++;
    
    if (c==0xa5) {
      // ending character found
      // parse the received bytes

      if (_lenIn>=frameLength && _bufIn[_lenIn-frameLength]==0x5a) {
        // found a valid frame
        parse((byte*)(&_bufIn[_lenIn-frameLength]), frameLength);
        // clear buffer
        _lenIn = 0;
      }
    }
  }

  // process cached sync messages if necessary
  if (_syncMsgEnabled && (millis()-_syncMsgBeginTime > SYNC_MSG_TIMEOUT)) {
    // sync messaging is timeout
    // have to stop receiving sync messages,
    // and process the cached messages
    finishSyncMessaging();
  }
}

void RiocSerialMessager::parse(byte* bytes, int length)
{
  // confirm sync characters
  if (bytes[0]!=0x5a || bytes[length-1]!=0xa5) return;
  // confirm frame version
  if (bytes[1]!=0x00 || length!=14) return;
  // confirm checksum 
  if (bytes[length-2]!=checksum((byte*)(bytes+2), length-4) && bytes[length-2]!=0xff) return;  
  // confirm address
  if (bytes[3]!=_address && bytes[3]!=0xff) return;

  byte from = bytes[2];
  byte msg[8] = {
      bytes[4],
      bytes[5],
      bytes[6],
      bytes[7],
      bytes[8],
      bytes[9],
      bytes[10],
      bytes[11]
  };

  if (_syncMsgEnabled && _syncMsgFrom == from) {

    // RECEIVING SYNC MESSAGES

    if (msg[0]==0x00 && msg[1]==0x05) {

        // stop receiving sync messages
        // and process all cached sync messages
        finishSyncMessaging();

    } else {

      if (_syncMsgCount < SYNC_MSG_MAX_COUNT) {

        // cache the sync message
        for (int n=0 ; n<8 ; n++)
          _syncMsg[_syncMsgCount*8+n] = msg[n];
        _syncMsgCount++;

      } else {

        // cache space is full
        // have to stop receiving sync messages,
        // and process the cached messages
        finishSyncMessaging();
      }
    }
  } else {

    // RECEIVING NORMAL MESSAGES

    if (msg[0]==0x00 && msg[1]==0x04) {
      
      // start receiving sync messages
      if (!_syncMsgEnabled) {
        _syncMsgEnabled = true;
        _syncMsgBeginTime = millis();
        _syncMsgCount = 0;
        _syncMsgFrom = from;

        // send response
        byte rsp[8] = { 0x00, 0x84, 0, 0, 0, 0, 0, 0 };
        sendMessage(rsp, from);
      }    
    } else {

      // launch callback to process the message instantly
      if (_callbackMessageReceived != NULL) 
        _callbackMessageReceived(msg, from);
    }
  } 
}

void RiocSerialMessager::finishSyncMessaging() 
{
  // process cached sync messages
  if (_callbackMessageReceived != NULL) {
    for (int n=0; n<_syncMsgCount ; n++) {
        _callbackMessageReceived(_syncMsg+(8*n), _syncMsgFrom);
    }
  }
  
  // send response
  byte rsp[8] = { 0x00, 0x85, 0, 0, 0, 0, 0, 0 };
  sendMessage(rsp, _syncMsgFrom);

  // stop receiving sync messages
  _syncMsgEnabled = false;
  _syncMsgCount = 0;
}

#include "ROUartSerial.h"
#include "RiocMessager.h"

ROUartSerial::ROUartSerial()
{
  _serial = NULL;
  _port = -1;
  _baud = -1;
  _config = -1;
  _notifyAddress = 0;
}


ROUartSerial::~ROUartSerial()
{
}

bool ROUartSerial::setup(byte msg[8], byte address_from)
{
  if (_port != -1) return false; 

  int port = msg[2];
  long baud = ((long)msg[3]<<16) | ((long)msg[4]<<8) | (long)msg[5];
  int config = msg[6];

  if (port>=0 && port<=UART_PORT_MAX_NUMBER) {

    if (baud==0 && config==0) {
      // default settings
      baud = 9600;
      config = SERIAL_8N1; 
    }

    _port = port;
    _baud = baud;
    _config = config;
    _notifyAddress = address_from;

    switch (_port) {
      case 0: _serial = &Serial;  break;

      #if defined(__SAM3X8E__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
      case 1: _serial = &Serial1; break;
      case 2: _serial = &Serial2; break;
      case 3: _serial = &Serial3; break;
      #endif

    }
    
    #if defined (__SAM3X8E__)
    _serial->begin(_baud);
    #else
    _serial->begin(_baud, _config);
    #endif
    
    return true;
  } 
  return false;
}

void ROUartSerial::execute(byte msg[8], byte address_from)
{
  if (_port == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // SEND
      
    int length = msg[3];
    if (length<=4) {
      for (int n=0 ; n<length ; n++) {
        _serial->write(msg[n+4]);
      }
    }    
  } 
}

void ROUartSerial::process()
{
  if (_port == -1) return; 

  int length = 0;
  byte data[] = {0,0,0,0};
  while (_serial->available()) {
    data[length] = _serial->read();
    length++;
    if (length==4) break;
  }

  if (length>0) {
    byte rsp[] = {0x05, 0x82, _port, length, data[0], data[1], data[2], data[3]};
    if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);
  }
}

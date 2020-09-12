#ifndef LIB_RO_UART_SERIAL_H_
#define LIB_RO_UART_SERIAL_H_

#include "RiocObject.h"

/* uart serial configurations defined in HardwareSerial.h
#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E
*/

class ROUartSerial : public RiocObject {
		
public:
  ROUartSerial();
  ~ROUartSerial();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();
		
private:
  HardwareSerial* _serial;
  int _port;
  long _baud;
  int _config;
  int _notifyAddress;

};


#endif // LIB_RO_UART_SERIAL_H_

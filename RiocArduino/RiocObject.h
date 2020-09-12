#ifndef LIB_RIOC_OBJECT_H_
#define LIB_RIOC_OBJECT_H_

#include <Arduino.h>

#pragma GCC diagnostic ignored "-Wnarrowing"

#if defined(__SAM3X8E__)

#define DI_PIN_MAX_NUMBER A11 
#define DO_PIN_MAX_NUMBER A11 
#define AI_PIN_MAX_NUMBER A11 
#define AO_PIN_MIN_NUMBER DAC0
#define AO_PIN_MAX_NUMBER DAC1
#define UART_PORT_MAX_NUMBER 3

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define DI_PIN_MAX_NUMBER A15
#define DO_PIN_MAX_NUMBER A15
#define AI_PIN_MAX_NUMBER A15
#define UART_PORT_MAX_NUMBER 3

#else

#define DI_PIN_MAX_NUMBER A7
#define DO_PIN_MAX_NUMBER A7
#define AI_PIN_MAX_NUMBER A7
#define UART_PORT_MAX_NUMBER 0

#endif


class RiocMessager;

class RiocObject {
		
public:
  RiocObject();
  ~RiocObject();

  void setMessager(RiocMessager* messager);
  void executeReserved(byte msg[8], byte address_from = 0);

  virtual bool setup(byte msg[8], byte address_from = 0);
  virtual void execute(byte msg[8], byte address_from = 0);
  virtual void process();

  bool isSilent();
		
protected:

  RiocMessager* _messager;
  bool _silent;
};


#endif // LIB_RIOC_OBJECT_H_

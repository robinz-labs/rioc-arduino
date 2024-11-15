#ifndef LIB_RIOC_OBJECT_H_
#define LIB_RIOC_OBJECT_H_

#include <Arduino.h>

#pragma GCC diagnostic ignored "-Wnarrowing"

#if defined(__SAM3X8E__)

// arduino due
#define PIN_COUNT 79
#define PORT_COUNT 4

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// arduino mega
#define PIN_COUNT 70
#define PORT_COUNT 4

#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega_328__)

// arduino uno 
// arduino nano
#define PIN_COUNT 22
#define PORT_COUNT 1

#elif defined(__AVR_ATmega32U4__)

// arduino leonardo
// arduino micro
#define PIN_COUNT 31
#define PORT_COUNT 2

#else

// general
#define PIN_COUNT 50
#define PORT_COUNT 1

#endif

#define DI_PIN_MAX_NUMBER (PIN_COUNT-1)
#define DO_PIN_MAX_NUMBER (PIN_COUNT-1)
#define UART_PORT_MAX_NUMBER (PORT_COUNT-1)

#if defined(__SAM3X8E__)

#define AI_PIN_MAX_NUMBER A11 
#define AO_PIN_MIN_NUMBER DAC0
#define AO_PIN_MAX_NUMBER DAC1

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define AI_PIN_MAX_NUMBER A15

#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega_328__)

#define AI_PIN_MAX_NUMBER A7

#elif defined(__AVR_ATmega32U4__)

#define AI_PIN_MAX_NUMBER A11

#else

#define AI_PIN_MAX_NUMBER DI_PIN_MAX_NUMBER

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

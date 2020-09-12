#ifndef LIB_RO_RGB_LED_H_
#define LIB_RO_RGB_LED_H_

#include "RiocObject.h"

class Adafruit_NeoPixel;

class RORgbLed : public RiocObject {

public:
  RORgbLed();
  ~RORgbLed();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  void setRGB(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
  void show();

private:  
  int _pin;
  int _mode;
  int _count;

  Adafruit_NeoPixel* _leds;
};

#endif // LIB_RO_RGB_LED_H_

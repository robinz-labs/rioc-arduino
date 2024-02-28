#include "RORgbLed.h"
#include "RiocMessager.h"
#include "RiocOptional.h"

#ifndef OPT_ENABLE_RGBLED

// no implementation if rgb led is disabled
RORgbLed::RORgbLed() { }
RORgbLed::~RORgbLed() { }
bool RORgbLed::setup(byte msg[8], byte address_from) { return false; }
void RORgbLed::execute(byte msg[8], byte address_from) { }
void RORgbLed::process() { }

#else

#include <Adafruit_NeoPixel.h>

RORgbLed::RORgbLed()
{
  _pin = -1;
  _mode = -1;
  _count = -1;
  _leds = NULL;
}


RORgbLed::~RORgbLed()
{
}

bool RORgbLed::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3];
  int count = ((int)msg[4] << 8) | (int)msg[5];

  if (pin<=DO_PIN_MAX_NUMBER && count<=1024) {

    _pin = pin;
    _mode = mode;
    _count = count;

    _leds = new Adafruit_NeoPixel(_count, _pin, (_mode ? _mode : NEO_GRB + NEO_KHZ800));
    _leds->begin();

    return true;
  } 
  return false;
}

void RORgbLed::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
    
    // SHOW RGB
    
    uint16_t index = ((uint16_t)msg[3] << 8) | (uint16_t)msg[4];
    uint8_t red = msg[5];
    uint8_t green = msg[6];
    uint8_t blue = msg[7];

    _leds->setPixelColor(index, _leds->Color(red, green, blue));
    _leds->show();

    if (!isSilent()) {
      byte rsp[] = {0x41, 0x81, _pin, index >> 8, index & 0xff, red, green, blue};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x02) {
    
    // SET RGB
    
    uint16_t index = ((uint16_t)msg[3] << 8) | (uint16_t)msg[4];
    uint8_t red = msg[5];
    uint8_t green = msg[6];
    uint8_t blue = msg[7];

    _leds->setPixelColor(index, _leds->Color(red, green, blue));

    if (!isSilent()) {
      byte rsp[] = {0x41, 0x82, _pin, index >> 8, index & 0xff, red, green, blue};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x03) {
    
    // SHOW    
    _leds->show();

    if (!isSilent()) {
      byte rsp[] = {0x41, 0x83, _pin, 0, 0, 0, 0, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }

  } else if (cmd==0x04) {

    // GET RGB

    uint16_t index = ((uint16_t)msg[3] << 8) | (uint16_t)msg[4];
    uint32_t rgb = _leds->getPixelColor(index);
    uint8_t red = (rgb >> 16) & 0xff;
    uint8_t green = (rgb >> 8) & 0xff;
    uint8_t blue = rgb & 0xff;

    byte rsp[] = {0x41, 0x84, _pin, index >> 8, index & 0xff, red, green, blue};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  } 
}

void RORgbLed::process()
{    
}

void RORgbLed::setRGB(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    _leds->setPixelColor(index, _leds->Color(red, green, blue));
}

void RORgbLed::show()
{
    _leds->show();
}

#endif

#include "ROThermometer.h"
#include "RiocMessager.h"
#include "RiocOptional.h"

#ifndef OPT_ENABLE_THERMOMETER

// no implementation if thermometer is disabled
ROThermometer::ROThermometer() { }
ROThermometer::~ROThermometer() { }
bool ROThermometer::setup(byte msg[8], byte address_from) { return false; }
void ROThermometer::execute(byte msg[8], byte address_from) { }
void ROThermometer::process() { }

#else

#include <dht.h>

ROThermometer::ROThermometer()
{
  _pin = -1;
  _mode = -1;
  _dht = NULL;
}

ROThermometer::~ROThermometer()
{
}

bool ROThermometer::setup(byte msg[8], byte address_from)
{
  if (_pin != -1) return false; 

  int pin = msg[2];
  int mode = msg[3]; 
  if (pin<=DI_PIN_MAX_NUMBER) {

    _pin = pin;
    _mode = mode; // thermometer model

    _dht = new dht();
    return true;
  } 
  return false;
}

void ROThermometer::execute(byte msg[8], byte address_from)
{
  if (_pin == -1) return; 

  int cmd = msg[1];
  if (cmd==0x01) {
      
    // MEASURE
    
    uint16_t humidity = 0;
    uint16_t temperature = 0; // kelvins
    read(&temperature, &humidity);
    
    byte rsp[] = {0x23, 0x81, _pin, (temperature>>8)&0xff, temperature&0xff, (humidity>>8)&0xff, humidity&0xff, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
  } 
}

void ROThermometer::process()
{

}

bool ROThermometer::read(uint16_t * temperature, uint16_t * humidity)
{
  int result = DHTLIB_INVALID_VALUE;
  if (_mode == 0 || _mode == THERMOMETER_DHT11) 
    result = _dht->read11(_pin);
  else if (_mode == THERMOMETER_DHT22) {
    result = _dht->read22(_pin);
  }

  if (result == DHTLIB_OK) {
    *humidity = _dht->humidity * 10;
    *temperature = (_dht->temperature + 273) * 10;
    return true;
  }
  return false;
}

#endif

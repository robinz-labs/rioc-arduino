#ifndef LIB_RO_THERMOMETER_H_
#define LIB_RO_THERMOMETER_H_

#include "RiocObject.h"

#define THERMOMETER_DHT11 1
#define THERMOMETER_DHT22 2

class dht;

class ROThermometer : public RiocObject {

public:
  ROThermometer();
  ~ROThermometer();

  bool setup(byte msg[8], byte address_from = 0);
  void execute(byte msg[8], byte address_from = 0);
  void process();

  bool read(uint16_t * temperature, uint16_t * humidity);

private:  
  int _pin;
  int _mode;

  dht* _dht;
};

#endif // LIB_RO_THERMOMETER_H_

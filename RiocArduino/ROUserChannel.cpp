#include "ROUserChannel.h"
#include "RiocMessager.h"

ROUserChannel::ROUserChannel()
{
  _channel = -1;
  _mode = -1;
  _value = 0;
  _notifyEnabled = false;
  _notifyAddress = 0;
  _interval = 0;
  _lastValue = 0;
  _lastTime = millis();
}

ROUserChannel::~ROUserChannel()
{
}

bool ROUserChannel::setup(byte msg[8], byte address_from)
{
  if (_channel != -1) return false; 

  byte channel = msg[2];
  byte mode = msg[3];

  if (channel>=0 && channel<CHANNEL_COUNT) {
    _channel = channel;
    _mode = mode;
    _notifyEnabled = false;
    _notifyAddress = address_from;
    return true;
  } 
  return false;
}

void ROUserChannel::execute(byte msg[8], byte address_from)
{
  if (_channel == -1) return; 

  byte cmd = msg[1];
  if (cmd==0x01) {
      
    // READ
    uint32_t val = _value;
    byte rsp[] = {0xE1, 0x81, _channel, val>>24, val>>16, val>>8, val & 0xff, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    
  } else if (cmd==0x02) {

    // SET NOTIFICATION
    byte enable = msg[3];
    uint16_t interval = ((unsigned int)msg[4]<<8) | (unsigned int)msg[5];

    if (enable) {
      _notifyEnabled = true;
      _notifyAddress = address_from;
      _interval = interval;
      _lastValue = _value;
      _lastTime = millis();
    } else {
      _notifyEnabled = false;
    }

    byte rsp[] = {0xE1, 0x82, _channel, enable, interval>>8, interval & 0xff, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    
  } else if (cmd==0x04) {
    
    // WRITE
    uint32_t val = ((uint32_t)msg[3]<<24) | ((uint32_t)msg[4]<<16) | ((uint32_t)msg[5]<<8) | (uint32_t)msg[6];
    _value = val;
    if (!isSilent()) {
      byte rsp[] = {0xE1, 0x84, _channel, val>>24, val>>16, val>>8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, address_from);
    }
  }
}

void ROUserChannel::process()
{
  // notify
  if (_notifyEnabled && _value != _lastValue) {

    unsigned long thisTime = millis();
    if (thisTime - _lastTime > _interval || thisTime < _lastTime) {
      
      uint32_t val = _value;
      byte rsp[] = {0xE1, 0x83, _channel, val>>24, val>>16, val>>8, val & 0xff, 0};
      if (_messager!=NULL) _messager->sendMessage(rsp, _notifyAddress);

      _lastValue = val;
      _lastTime = thisTime;
    }
  }
}

uint32_t ROUserChannel::read()
{
  return _value;
}

void ROUserChannel::write(uint32_t value)
{
  _value = value;
}

int ROUserChannel::mode()
{
  return _mode;
}
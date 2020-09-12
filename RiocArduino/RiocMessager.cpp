#include "RiocMessager.h"


RiocMessager::RiocMessager(byte address)
{
  _callbackMessageReceived = NULL;

  for (int n=0 ; n<sizeof(_internalMessage) ; n++) _internalMessage[n] = 0;
  clearInternalMessage();
}


RiocMessager::~RiocMessager()
{

}

void RiocMessager::begin()
{

}

void RiocMessager::sendMessage(byte msg[8], byte address_to)
{

}

void RiocMessager::process()
{

}

void RiocMessager::setCallbackMessageReceived(void (*fptr)(byte msg[8], byte address_from))
{
  _callbackMessageReceived = fptr;
}

void RiocMessager::clearInternalMessage()
{
 	_internalMessageReceived = false;
}

void RiocMessager::setInternalMessage(byte msg[8])
{
  memcpy((void *)_internalMessage, (const void *)msg, 8);
  _internalMessageReceived = true;
}

byte * RiocMessager::getInternalMessage()
{
  if (_internalMessageReceived) 
    return _internalMessage;
  else
    return NULL;
}


#include "RiocGlobal.h"
#include "RiocObject.h"
#include "RiocMessager.h"

RiocObject::RiocObject()
{
  _messager = NULL;
  _silent = false;
}

RiocObject::~RiocObject()
{
}

void RiocObject::setMessager(RiocMessager* messager)
{
  _messager = messager;
}

void RiocObject::executeReserved(byte msg[8], byte address_from)
{
  int cmd = msg[1];
  if (cmd==0x70) {
     
    // SET SILENT MODE
      
    _silent = (msg[3]==0 ? false : true);
    byte rsp[] = {msg[0], 0xF0, msg[2], _silent, 0, 0, 0, 0};
    if (_messager!=NULL) _messager->sendMessage(rsp, address_from);  
  }
}

bool RiocObject::setup(byte msg[8], byte address_from)
{
  return false;
}

void RiocObject::execute(byte msg[8], byte address_from)
{
}

void RiocObject::process()
{
}

bool RiocObject::isSilent()
{
  return (_silent || g_isSilentDevice);
}
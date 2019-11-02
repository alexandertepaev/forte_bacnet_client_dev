#include "bacnet_readproperty_handle.h"

CBacnetReadPropertyHandle::CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution)
{
}

CBacnetReadPropertyHandle::~CBacnetReadPropertyHandle()
{
}


void CBacnetReadPropertyHandle::get(CIEC_ANY &) {
  
}
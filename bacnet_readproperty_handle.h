#ifndef _BACNET_READPROPERTY_HANDLE_H_
#define _BACNET_READPROPERTY_HANDLE_H_

#include "bacnet_service_handle.h"

class CBacnetReadPropertyHandle : public CBacnetServiceHandle
{
private:
  /* data */
public:
  CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution);
  ~CBacnetReadPropertyHandle();

  virtual void get(CIEC_ANY &);
protected:
  virtual void set(const CIEC_ANY &){};

};

#endif

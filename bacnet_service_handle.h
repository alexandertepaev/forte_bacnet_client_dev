#ifndef _BACNET_SERVICE_HANDLE_H_
#define _BACNET_SERVICE_HANDLE_H_


#include <devexec.h>
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_handle.h"

class CBacnetServiceHandle : public forte::core::io::IOHandle
{
private:
  /* data */
public:
  CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution);
  ~CBacnetServiceHandle();
};

#endif
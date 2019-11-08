#ifndef _BACNET_SERVICE_HANDLE_H_
#define _BACNET_SERVICE_HANDLE_H_


#include <devexec.h>
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_handle.h"
#include "include/bacnet.h"

class CBacnetServiceHandle : public forte::core::io::IOHandle
{
private:
  /* data */
public:
  CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution);
  ~CBacnetServiceHandle();

  virtual int encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *my_address, BACNET_ADDRESS *dest) = 0;
  virtual void decodeServiceResp(const uint8_t &invoke_id) = 0;
};

#endif
#ifndef _BACNET_READPROPERTY_HANDLE_H_
#define _BACNET_READPROPERTY_HANDLE_H_


#include "BACnetReadProperty.h"
#include "bacnet_service_handle.h"



class CBacnetReadPropertyHandle : public CBacnetServiceHandle
{
private:
  /* data */
public:
  CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetReadPropertyHandle();

  virtual void get(CIEC_ANY &);

  CBacnetServiceConfigFB *mConfigFB;

  int encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *my_address, BACNET_ADDRESS *dest);
  void decodeServiceResp(uint8_t *pdu, const uint8_t &len);

protected:
  virtual void set(const CIEC_ANY &){};

 
};

#endif

#ifndef _BACNET_UNCONFIRMED_COV_HANDLE_H_
#define _BACNET_UNCONFIRMED_COV_HANDLE_H_


#include "BACnetSubscribeUnconfirmedCOV.h"
#include "bacnet_service_handle.h"


class CBacnetServiceHandle;

class CBacnetUnconfirmedCOVHandle : public CBacnetServiceHandle
{
public:
  
  friend class CBacnetClientController;
  
  CBacnetUnconfirmedCOVHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetUnconfirmedCOVHandle();

  virtual void get(CIEC_ANY &);
  
  int encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src) { };

  void decodeServiceResp(uint8_t *pdu, const uint32_t &len) { };

  // void notificationReceived(CIEC_ANY paValue);
  void notificationReceived(BACNET_PROPERTY_VALUE paPropertyValue);

  void subscriptionAcknowledged();
protected:
  virtual void set(const CIEC_ANY &){};
private:
  struct SubscriptionData {
    bool bAcknowledgedFlag;
    TForteUInt8 nSubscriptionInvokeID;

    // SubscriptionData() : bAcknowledgedFlag(false), nSubscriptionInvokeID(0) { }
  };

  SubscriptionData m_stSubscriptionData = { };

};

#endif

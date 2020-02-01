#include "bacnet_unconfirmed_cov_handle.h"

CBacnetUnconfirmedCOVHandle::CBacnetUnconfirmedCOVHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution, paServiceConfigFB)
{

   DEVLOG_DEBUG("[CBacnetUnconfirmedCOVHandle] CBacnetUnconfirmedCOVHandle(): Initializing UnconfirmedCOV Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d\n", paServiceConfigFB->m_stServiceConfig->mDeviceId, paServiceConfigFB->m_stServiceConfig->mObjectType, paServiceConfigFB->m_stServiceConfig->mObjectId);

  //TODO: check if we know the address of the device. (controller->checkAddr(...)) If we don't know it, construct WHO-IS pdu and send it.
}

CBacnetUnconfirmedCOVHandle::~CBacnetUnconfirmedCOVHandle()
{
}


void CBacnetUnconfirmedCOVHandle::get(CIEC_ANY &paValue) {
  if (m_eHandleState == e_AwaitingResponse) {
    DEVLOG_DEBUG("[CBacnetUnconfirmedCOVHandle] get()\n");
    if(mType == CIEC_ANY::e_DWORD) {
      static_cast<CIEC_DWORD&>(paValue) = *static_cast<CIEC_DWORD *>(mValue);
    } else if (mType == CIEC_ANY::e_BOOL) {
      static_cast<CIEC_BOOL&>(paValue) = *static_cast<CIEC_BOOL *>(mValue);
    }
  }
}

// void CBacnetUnconfirmedCOVHandle::notificationReceived(CIEC_ANY &paValue) {
//   DEVLOG_DEBUG("Hello from handler! Notification received\n");
//   mValue = paValue;
//   fireConfirmationEvent();
// }
void CBacnetUnconfirmedCOVHandle::notificationReceived(BACNET_PROPERTY_VALUE paPropertyValue) {
  DEVLOG_DEBUG("Hello from handler! Notification received\n");
  if(paPropertyValue.value.tag == BACNET_APPLICATION_TAG_REAL) {
    mValue->setValue(static_cast<CIEC_DWORD>(paPropertyValue.value.type.Real));
  } else if(paPropertyValue.value.tag == BACNET_APPLICATION_TAG_ENUMERATED){
    mValue->setValue(static_cast<CIEC_BOOL>(paPropertyValue.value.type.Enumerated));
  }
  fireConfirmationEvent();
}

void CBacnetUnconfirmedCOVHandle::subscriptionAcknowledged() {
  m_eHandleState = e_AwaitingResponse;
}

// int CBacnetUnconfirmedCOVHandle::encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src){
  
// }

// void CBacnetUnconfirmedCOVHandle::decodeServiceResp(uint8_t *pdu, const uint32_t &len) {

// }
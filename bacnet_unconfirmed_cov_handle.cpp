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
  
}

int CBacnetUnconfirmedCOVHandle::encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src){
  
}

void CBacnetUnconfirmedCOVHandle::decodeServiceResp(uint8_t *pdu, const uint32_t &len) {

}
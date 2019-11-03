#include "bacnet_readproperty_handle.h"

CBacnetReadPropertyHandle::CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution), mConfigFB(paServiceConfigFB)
{
  CBacnetReadPropertyConfigFB::ServiceConfig mServiceConfig = static_cast<CBacnetReadPropertyConfigFB *>(paServiceConfigFB)->m_stServiceConfig;
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] CBacnetReadPropertyHandle(): Initializing ReadProperty Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, dummy_value=%d\n",mServiceConfig.mDeviceId, mServiceConfig.mObjectType, mServiceConfig.mObjectId, mServiceConfig.mObjectProperty, mServiceConfig.mArrayIndex, mServiceConfig.dummy_value);

  //TODO: check if we know the address of the device. (controller->checkAddr(...)) If we don't know it, construct WHO-IS pdu and send it.

}

CBacnetReadPropertyHandle::~CBacnetReadPropertyHandle()
{
}


void CBacnetReadPropertyHandle::get(CIEC_ANY &) {
  
}
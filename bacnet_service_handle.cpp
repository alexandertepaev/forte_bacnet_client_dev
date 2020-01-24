#include "bacnet_service_handle.h"

CBacnetServiceHandle::CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IOHandle(controller, direction, type), CExternalEventHandler(paDeviceExecution), m_eHandleState(e_Idle), mConfigFB(paServiceConfigFB), mValue(new CIEC_ANY)
{
    mConfigFB->setHandle(this);
}

CBacnetServiceHandle::~CBacnetServiceHandle()
{
    mConfigFB->setHandle(0);
}

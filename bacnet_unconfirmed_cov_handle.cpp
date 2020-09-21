/*******************************************************************************
 * Copyright (c) 2020 Alexander Tepaev github.com/alexandertepaev
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev
 *******************************************************************************/

#include "bacnet_unconfirmed_cov_handle.h"

CBacnetUnconfirmedCOVHandle::CBacnetUnconfirmedCOVHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution, paServiceConfigFB)
{

  m_enHandleType = UnconfirmedCOVServiceHandle;
  
  DEVLOG_DEBUG("[CBacnetUnconfirmedCOVHandle] CBacnetUnconfirmedCOVHandle(): Initializing UnconfirmedCOV Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d\n", paServiceConfigFB->m_stServiceConfig->deviceID, paServiceConfigFB->m_stServiceConfig->objectType, paServiceConfigFB->m_stServiceConfig->objectID);
}

CBacnetUnconfirmedCOVHandle::~CBacnetUnconfirmedCOVHandle()
{
}

  
void CBacnetUnconfirmedCOVHandle::notificationReceived(BACNET_PROPERTY_VALUE paPropertyValue) {
  if(paPropertyValue.value.tag == BACNET_APPLICATION_TAG_REAL) {
    DEVLOG_DEBUG("[CBacnetUnconfirmedCOVHandle] notificationReceived(): Notification received, PresentValue=%f\n", paPropertyValue.value.type.Real);
    mValue->setValue(static_cast<CIEC_REAL>(paPropertyValue.value.type.Real));
  } else if(paPropertyValue.value.tag == BACNET_APPLICATION_TAG_ENUMERATED){
    DEVLOG_DEBUG("[CBacnetUnconfirmedCOVHandle] notificationReceived(): Notification received, PresentValue=%d\n", paPropertyValue.value.type.Enumerated);
    mValue->setValue(static_cast<CIEC_BOOL>(paPropertyValue.value.type.Enumerated));
  }
  fireConfirmationEvent();
}

void CBacnetUnconfirmedCOVHandle::subscriptionAcknowledged() {
  m_enHandleState = AwaitingResponse;
}

void CBacnetUnconfirmedCOVHandle::readResponse(CIEC_ANY *paData) {
  if(AwaitingResponse == m_enHandleState) {
    if(mType == CIEC_ANY::e_DWORD) {
      static_cast<CIEC_REAL &>(*paData) = *static_cast<CIEC_REAL *>(mValue);
    } else if (mType == CIEC_ANY::e_BOOL) {
      static_cast<CIEC_BOOL &>(*paData) = *static_cast<CIEC_BOOL *>(mValue);
    }
    // no need to change the handle's state, it always stays in the 'awaiting response'
  }
}
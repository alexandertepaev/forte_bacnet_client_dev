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
#include "bacnet_writeproperty_handle.h"
#include "bacnet_writeproperty_service_config_fb.h"
#include "bacnet_client_controller.h"

CBacnetWritePropertyHandle::CBacnetWritePropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution, paServiceConfigFB)
{
  m_enHandleType = WritePropertyServiceHandle;

  DEVLOG_DEBUG("[CBacnetWritePropertyHandle] CBacnetWritePropertyHandle(): Initializing WriteProperty Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, Priority=%d\n", paServiceConfigFB->m_stServiceConfig->deviceID, paServiceConfigFB->m_stServiceConfig->objectType, paServiceConfigFB->m_stServiceConfig->objectID, static_cast<CBacnetWritePropertyConfigFB::SServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->objectProperty, static_cast<CBacnetWritePropertyConfigFB::SServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->arrayIndex, static_cast<CBacnetWritePropertyConfigFB::SServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->priority);

}

CBacnetWritePropertyHandle::~CBacnetWritePropertyHandle()
{
}

int CBacnetWritePropertyHandle::encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src){
  // encode npdu data
  TForteUInt16 PDULen = BVLC_HEADER_LEN;
  BACNET_NPDU_DATA NPDUData;
  npdu_encode_npdu_data(&NPDUData, true, MESSAGE_PRIORITY_NORMAL);
  PDULen = (TForteUInt16) (PDULen + npdu_encode_pdu(&pdu[PDULen], dest, src, &NPDUData));
  

  BACNET_WRITE_PROPERTY_DATA data;
  data.object_type = static_cast<BACNET_OBJECT_TYPE>(mConfigFB->m_stServiceConfig->objectType);
  data.object_instance = mConfigFB->m_stServiceConfig->objectID;
  data.object_property = static_cast<BACNET_PROPERTY_ID>(static_cast<CBacnetWritePropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->objectProperty);
  data.priority = static_cast<CBacnetWritePropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->priority;
  data.array_index = static_cast<CBacnetWritePropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->arrayIndex;

  BACNET_APPLICATION_DATA_VALUE application_data;
  application_data.context_specific = false;
  application_data.next = NULL;

  if(data.object_type == OBJECT_ANALOG_OUTPUT || 
     data.object_type == OBJECT_ANALOG_INPUT ||
     data.object_type == OBJECT_ANALOG_VALUE) {

    application_data.tag =  BACNET_APPLICATION_TAG_REAL;
    application_data.type.Real = *((TForteFloat *) mValue->getDataPtr());
  
  } else if(data.object_type == OBJECT_BINARY_OUTPUT ||
            data.object_type == OBJECT_BINARY_INPUT ||
            data.object_type == OBJECT_BINARY_VALUE) {

    application_data.tag =  BACNET_APPLICATION_TAG_ENUMERATED;
    application_data.type.Enumerated = *((bool *) mValue->getDataPtr());

  }
  
  data.application_data_len = bacapp_encode_data(data.application_data, &application_data);
  
  PDULen = (TForteUInt16) (PDULen + wp_encode_apdu(&pdu[PDULen], invoke_id, &data));

  pdu[BVLC_TYPE_BYTE] = BVLL_TYPE_BACNET_IP;
  pdu[BVLC_FUNCTION_BYTE] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&pdu[BVLC_LEN_BYTE], PDULen);

  return PDULen;
  
}

void CBacnetWritePropertyHandle::decodeServiceResp(uint8_t *pdu, const TForteUInt16 &len) {
  DEVLOG_DEBUG("[CBacnetWritePropertyHandle] decodeServiceResp(): Received WriteProperty Acknowledge!\n");
  std::ignore = pdu;
  std::ignore = len;
  fireConfirmationEvent();
}

void CBacnetWritePropertyHandle::sendRequest(CIEC_ANY *paData) {
  if(m_enHandleState == Idle) {
    mValue->setValue(*paData);
    CBacnetClientController *controller = static_cast<CBacnetClientController *>(mController);
    controller->pushToRingBuffer(this);
    m_enHandleState = AwaitingResponse;
  }
}

void CBacnetWritePropertyHandle::readResponse(CIEC_ANY *paData) {
  // do nothing, ack received
  std::ignore = paData;
  if(AwaitingResponse == m_enHandleState)
    m_enHandleState = Idle;
}
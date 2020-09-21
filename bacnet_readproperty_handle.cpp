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

#include "bacnet_readproperty_handle.h"
#include "bacnet_readproperty_service_config_fb.h"
#include "bacnet_client_controller.h"

CBacnetReadPropertyHandle::CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution, paServiceConfigFB)
{

  m_enHandleType = ReadPropertyServiceHandle;
  
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] CBacnetReadPropertyHandle(): Initializing ReadProperty Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d\n", paServiceConfigFB->m_stServiceConfig->deviceID, paServiceConfigFB->m_stServiceConfig->objectType, paServiceConfigFB->m_stServiceConfig->objectID, static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->objectProperty, static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->arrayIndex);
}

CBacnetReadPropertyHandle::~CBacnetReadPropertyHandle()
{
}

int CBacnetReadPropertyHandle::encodeServiceReq(TForteUInt8 *paBuffer, const TForteUInt8 &paInvokeID, BACNET_ADDRESS *paDestAddr, BACNET_ADDRESS *paSrcAddr){
  
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] encodeServiceReq(): Encoding ReadProperty request with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, invoke_id=%d\n", mConfigFB->m_stServiceConfig->deviceID, mConfigFB->m_stServiceConfig->objectType, mConfigFB->m_stServiceConfig->objectID, static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->objectProperty, static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->arrayIndex, paInvokeID);

  TForteUInt16 PDULen = BVLC_HEADER_LEN;
  BACNET_NPDU_DATA NPDUData;
  npdu_encode_npdu_data(&NPDUData, true, MESSAGE_PRIORITY_NORMAL);

  BACNET_READ_PROPERTY_DATA data; 
  data.object_type = static_cast<BACNET_OBJECT_TYPE>(mConfigFB->m_stServiceConfig->objectType);
  data.object_instance = mConfigFB->m_stServiceConfig->objectID;
  data.object_property = static_cast<BACNET_PROPERTY_ID>(static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->objectProperty);
  data.array_index = static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->arrayIndex;

  PDULen = (TForteUInt16) (PDULen + npdu_encode_pdu(&paBuffer[PDULen], paDestAddr, paSrcAddr, &NPDUData));

  PDULen = (TForteUInt16) (PDULen + rp_encode_apdu(&paBuffer[PDULen], paInvokeID, &data));

  paBuffer[BVLC_TYPE_BYTE] = BVLL_TYPE_BACNET_IP;
  paBuffer[BVLC_FUNCTION_BYTE] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&paBuffer[BVLC_LEN_BYTE], PDULen);

  return PDULen;
}

void CBacnetReadPropertyHandle::decodeServiceResp(TForteUInt8 *paBuffer, const TForteUInt16 &paLen) {
  // decode rp app data
  BACNET_READ_PROPERTY_DATA data;
  int RPLen = rp_ack_decode_service_request(&paBuffer[COMPLEX_ACK_APP_TAGS_OFFSET], paLen-COMPLEX_ACK_APP_TAGS_OFFSET, &data);
  // check if the read data is actually the one we requested
  if(RPLen > 0 && data.object_type == static_cast<BACNET_OBJECT_TYPE>(mConfigFB->m_stServiceConfig->objectType) &&
                   data.object_instance == mConfigFB->m_stServiceConfig->objectID &&
                   data.object_property == static_cast<BACNET_PROPERTY_ID>(static_cast<CBacnetReadPropertyConfigFB::SServiceConfig *>(mConfigFB->m_stServiceConfig)->objectProperty)){
    BACNET_APPLICATION_DATA_VALUE value;
    if(bacapp_decode_application_data(data.application_data, (uint8_t) data.application_data_len, &value) > 0){
      if(value.tag == BACNET_APPLICATION_TAG_REAL && mType == CIEC_ANY::e_DWORD) {
        DEVLOG_DEBUG("[CBacnetReadPropertyHandle] ReadProperty response value=%f\n", value.type.Real);
        mValue->setValue(static_cast<CIEC_REAL>(value.type.Real)); // set the mValue
        fireConfirmationEvent();
      } else if(value.tag == BACNET_APPLICATION_TAG_ENUMERATED && mType == CIEC_ANY::e_BOOL ) { 
        DEVLOG_DEBUG("[CBacnetReadPropertyHandle] ReadProperty response value=%d\n", value.type.Enumerated);
        mValue->setValue(static_cast<CIEC_BOOL>(value.type.Enumerated)); // set the mValue
        fireConfirmationEvent();
      }
    }
  }
}

void CBacnetReadPropertyHandle::sendRequest(CIEC_ANY *paData) {
  if(NULL != paData)
    return; // readproperty service -> requires no write data
    
  if(m_enHandleState == Idle) {
    CBacnetClientController *controller = static_cast<CBacnetClientController *>(mController);
    controller->pushToRingBuffer(this);
    m_enHandleState = AwaitingResponse;
  } 
}

void CBacnetReadPropertyHandle::readResponse(CIEC_ANY *paData){
  if(NULL == paData)
    return; // response contains read data -> pointer to the FB's output port is needed
  if(m_enHandleState == AwaitingResponse) {
    if(mType == CIEC_ANY::e_DWORD) {
      static_cast<CIEC_REAL &>(*paData) = *static_cast<CIEC_REAL *>(mValue);
    } else if (mType == CIEC_ANY::e_BOOL) {
      static_cast<CIEC_BOOL &>(*paData) = *static_cast<CIEC_BOOL *>(mValue);
    }
    m_enHandleState = Idle;
  }
}
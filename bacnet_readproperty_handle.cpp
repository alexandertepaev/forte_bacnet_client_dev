#include "bacnet_readproperty_handle.h"

CBacnetReadPropertyHandle::CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution, paServiceConfigFB)
{

   DEVLOG_DEBUG("[CBacnetReadPropertyHandle] CBacnetReadPropertyHandle(): Initializing ReadProperty Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d\n", paServiceConfigFB->m_stServiceConfig->mDeviceId, paServiceConfigFB->m_stServiceConfig->mObjectType, paServiceConfigFB->m_stServiceConfig->mObjectId, paServiceConfigFB->m_stServiceConfig->mObjectProperty, paServiceConfigFB->m_stServiceConfig->mArrayIndex);

  //TODO: check if we know the address of the device. (controller->checkAddr(...)) If we don't know it, construct WHO-IS pdu and send it.
}

CBacnetReadPropertyHandle::~CBacnetReadPropertyHandle()
{
}


void CBacnetReadPropertyHandle::get(CIEC_ANY &paValue) {
  if(m_eHandleState == e_Idle) {
    DEVLOG_DEBUG("[CBacnetReadPropertyHandle] get(): \n");
    CBacnetClientController *controller = static_cast<CBacnetClientController *>(mController);
    controller->pushToRingbuffer(this);
    m_eHandleState = e_AwaitingResponse;
  } else if (m_eHandleState == e_AwaitingResponse) {

    if(mType == CIEC_ANY::e_DWORD) {
      static_cast<CIEC_DWORD&>(paValue) = *static_cast<CIEC_DWORD *>(mValue);
    } else if (mType == CIEC_ANY::e_BOOL) {
      static_cast<CIEC_BOOL&>(paValue) = *static_cast<CIEC_BOOL *>(mValue);
    }
    
    
    m_eHandleState = e_Idle;
  } else {
    // 
  }
}

int CBacnetReadPropertyHandle::encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src){
  
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] encodeServiceReq(): Encoding ReadProperty request with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, invoke_id=%d\n", mConfigFB->m_stServiceConfig->mDeviceId, mConfigFB->m_stServiceConfig->mObjectType, mConfigFB->m_stServiceConfig->mObjectId, mConfigFB->m_stServiceConfig->mObjectProperty, mConfigFB->m_stServiceConfig->mArrayIndex, invoke_id);

  int pdu_len = 4;
  BACNET_NPDU_DATA npdu_data;
  npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);

  BACNET_READ_PROPERTY_DATA data; 
  data.object_type = static_cast<BACNET_OBJECT_TYPE>(mConfigFB->m_stServiceConfig->mObjectType);
  data.object_instance = mConfigFB->m_stServiceConfig->mObjectId;
  data.object_property = static_cast<BACNET_PROPERTY_ID>(mConfigFB->m_stServiceConfig->mObjectProperty);
  data.array_index = mConfigFB->m_stServiceConfig->mArrayIndex;

  pdu_len += npdu_encode_pdu(&pdu[pdu_len], dest, src, &npdu_data);

  pdu_len += rp_encode_apdu(&pdu[pdu_len], invoke_id, &data);

  pdu[0] = BVLL_TYPE_BACNET_IP;
  pdu[1] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&pdu[2], pdu_len); // encode bvlc length

  return pdu_len;
}

void CBacnetReadPropertyHandle::decodeServiceResp(uint8_t *pdu, const uint32_t &len) {
  
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Decoding APDU of length: %d\n", len);
  BACNET_READ_PROPERTY_DATA data;
  int rp_len = rp_ack_decode_service_request(&pdu[3], len-3, &data);

  if(rp_len > 0 && data.object_type == static_cast<BACNET_OBJECT_TYPE>(mConfigFB->m_stServiceConfig->mObjectType) &&
                   data.object_instance == mConfigFB->m_stServiceConfig->mObjectId &&
                   data.object_property == static_cast<BACNET_PROPERTY_ID>(mConfigFB->m_stServiceConfig->mObjectProperty)){

    DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Decoded: Obj_Type=%d, Obj_Instance=%d, Obj_Prop=%d, App_Data_Len=%d\n", data.object_type, data.object_instance, data.object_property, data.application_data_len);
    BACNET_APPLICATION_DATA_VALUE value;
    int len = bacapp_decode_application_data(data.application_data, (uint8_t) data.application_data_len, &value);
    DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Application TAG=%d\n", value.tag);
    if(value.tag == BACNET_APPLICATION_TAG_REAL) {

      DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Application Value=%f (Real) \n", value.type.Real);
      mValue->setValue(static_cast<CIEC_DWORD>(value.type.Real));
      fireConfirmationEvent();

    } else if(value.tag == BACNET_APPLICATION_TAG_ENUMERATED && mType == CIEC_ANY::e_BOOL ) {
      
      DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Application Value=%d (Enumerated)\n", value.type.Enumerated);
      mValue->setValue(static_cast<CIEC_BOOL>(value.type.Enumerated));
      fireConfirmationEvent();

    }
  
  }
}
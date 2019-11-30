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


void CBacnetReadPropertyHandle::get(CIEC_ANY &paValue) {
  if(m_eHandleState == e_Idle) {
    DEVLOG_DEBUG("[CBacnetReadPropertyHandle] get(): \n");
    CBacnetClientController *controller = static_cast<CBacnetClientController *>(mController);
    controller->pushToRingbuffer(this);
    m_eHandleState = e_AwaitingResponse;
  } else if (m_eHandleState == e_AwaitingResponse) {
    static_cast<CIEC_DWORD&>(paValue) = present_value;
    m_eHandleState = e_Idle;
  } else {
    // 
  }
}

int CBacnetReadPropertyHandle::encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *my_address, BACNET_ADDRESS *dest) {

  int pdu_len = 4;
  BACNET_NPDU_DATA npdu_data;
  npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);

  BACNET_READ_PROPERTY_DATA data;
  CBacnetReadPropertyConfigFB::ServiceConfig mServiceConfig = static_cast<CBacnetReadPropertyConfigFB *>(mConfigFB)->m_stServiceConfig;
  data.object_type = static_cast<BACNET_OBJECT_TYPE>(mServiceConfig.mObjectType);
  data.object_instance = mServiceConfig.mObjectId;
  data.object_property = static_cast<BACNET_PROPERTY_ID>(mServiceConfig.mObjectProperty);
  data.array_index = mServiceConfig.mArrayIndex;

  unsigned max_apdu = 0;
  BACNET_ADDRESS Target_Address;
  bool found = address_bind_request(mServiceConfig.mDeviceId, &max_apdu, &Target_Address); //TODO - no need to do it every time, can be done one time during initialization of the handle
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] encodeServiceReq(): Address found=%d\n", found);

  //pdu_len += npdu_encode_pdu(&pdu[pdu_len], dest, my_address, &npdu_data);
  pdu_len += npdu_encode_pdu(&pdu[pdu_len], &Target_Address, my_address, &npdu_data);

   DEVLOG_DEBUG("[CBacnetReadPropertyHandle] encodeServiceReq(): Encoding ReadProperty request with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, dummy_value=%d, invoke_id=%d\n",mServiceConfig.mDeviceId, mServiceConfig.mObjectType, mServiceConfig.mObjectId, mServiceConfig.mObjectProperty, mServiceConfig.mArrayIndex, mServiceConfig.dummy_value, invoke_id);

  pdu_len += rp_encode_apdu(&pdu[pdu_len], invoke_id, &data);

  pdu[0] = BVLL_TYPE_BACNET_IP;
  pdu[1] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&pdu[2], pdu_len); // encode bvlc length

  return pdu_len;
}

void CBacnetReadPropertyHandle::decodeServiceResp(uint8_t *pdu, const uint8_t &len) {
  if(pdu[0] != SERVICE_CONFIRMED_READ_PROPERTY) {
    return;
  }
  
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Decoding APDU of length: %d\n", len);
  BACNET_READ_PROPERTY_DATA data;
  int rp_len = rp_ack_decode_service_request(&pdu[1], len-1, &data);
  CBacnetReadPropertyConfigFB::ServiceConfig mServiceConfig = static_cast<CBacnetReadPropertyConfigFB *>(mConfigFB)->m_stServiceConfig;

  if(rp_len > 0 && data.object_type == static_cast<BACNET_OBJECT_TYPE>(mServiceConfig.mObjectType) &&
                   data.object_instance == mServiceConfig.mObjectId &&
                   data.object_property == static_cast<BACNET_PROPERTY_ID>(mServiceConfig.mObjectProperty)){

    DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Decoded: Obj_Type=%d, Obj_Instance=%d, Obj_Prop=%d, App_Data_Len=%d\n", data.object_type, data.object_instance, data.object_property, data.application_data_len);
    BACNET_APPLICATION_DATA_VALUE value;
    int len = bacapp_decode_application_data(data.application_data, (uint8_t) data.application_data_len, &value);
    if(value.tag == BACNET_APPLICATION_TAG_REAL) {
      DEVLOG_DEBUG("[CBacnetReadPropertyHandle] Application Value=%f\n", value.type.Real);
      // in_qo = true;
      // in_data = (float) value.type.Real;
      // fireConfirmationEvent();
      present_value = static_cast<CIEC_DWORD>(value.type.Real);
      fireConfirmationEvent();
      //output_value = &(present_value);
      //output_value.setValue(static_cast<const CIEC_ANY &>(value.type.Real));

    }
  
  }
}
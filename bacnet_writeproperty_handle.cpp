#include "bacnet_writeproperty_handle.h"
#include "BACnetWriteProperty.h"
#include "bacnet_client_controller.h"

CBacnetWritePropertyHandle::CBacnetWritePropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution, paServiceConfigFB)
{

   DEVLOG_DEBUG("[CBacnetWritePropertyHandle] CBacnetWritePropertyHandle(): Initializing WriteProperty Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, Priority=%d\n", paServiceConfigFB->m_stServiceConfig->mDeviceId, paServiceConfigFB->m_stServiceConfig->mObjectType, paServiceConfigFB->m_stServiceConfig->mObjectId, static_cast<CBacnetWritePropertyConfigFB::ServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->mObjectProperty, static_cast<CBacnetWritePropertyConfigFB::ServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->mArrayIndex, static_cast<CBacnetWritePropertyConfigFB::ServiceConfig *>(paServiceConfigFB->m_stServiceConfig)->mPriority);

}

CBacnetWritePropertyHandle::~CBacnetWritePropertyHandle()
{
}

void CBacnetWritePropertyHandle::set(const CIEC_ANY &paValue) {
  if(m_eHandleState == e_Idle) {
    mValue->setValue(paValue);
    CBacnetClientController *controller = static_cast<CBacnetClientController *>(mController);
    controller->pushToRingbuffer(this);
    m_eHandleState = e_AwaitingResponse;
  } else if(m_eHandleState == e_AwaitingResponse) {
    m_eHandleState = e_Idle;
  }

}

int CBacnetWritePropertyHandle::encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src){
  // TODO: create a table of object properties and application tags and corresponding forte/cpp types
  // TODO: write choice function

  /* 
    encode npdu data
    fill wp_data variable + encode application data 
    encode apdu
    return
  */

  // encode npdu data
  int pdu_len = 4;
  BACNET_NPDU_DATA npdu_data;
  npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);
  pdu_len += npdu_encode_pdu(&pdu[pdu_len], dest, src, &npdu_data);
  

  BACNET_WRITE_PROPERTY_DATA data;
  data.object_type = static_cast<BACNET_OBJECT_TYPE>(mConfigFB->m_stServiceConfig->mObjectType);
  data.object_instance = mConfigFB->m_stServiceConfig->mObjectId;
  data.object_property = static_cast<BACNET_PROPERTY_ID>(static_cast<CBacnetWritePropertyConfigFB::ServiceConfig *>(mConfigFB->m_stServiceConfig)->mObjectProperty);
  data.array_index = static_cast<CBacnetWritePropertyConfigFB::ServiceConfig *>(mConfigFB->m_stServiceConfig)->mArrayIndex;

  BACNET_APPLICATION_DATA_VALUE application_data;
  application_data.context_specific = false;
  application_data.next = NULL;

  if(data.object_type == BACNET_OBJECT_TYPE::OBJECT_ANALOG_OUTPUT || 
     data.object_type == BACNET_OBJECT_TYPE::OBJECT_ANALOG_INPUT ||
     data.object_type == BACNET_OBJECT_TYPE::OBJECT_ANALOG_VALUE) {

    application_data.tag =  BACNET_APPLICATION_TAG_REAL;
    application_data.type.Real = *((TForteFloat *) mValue->getDataPtr());
  
  } else if(data.object_type == BACNET_OBJECT_TYPE::OBJECT_BINARY_OUTPUT ||
            data.object_type == BACNET_OBJECT_TYPE::OBJECT_BINARY_INPUT ||
            data.object_type == BACNET_OBJECT_TYPE::OBJECT_BINARY_VALUE) {

    application_data.tag =  BACNET_APPLICATION_TAG_ENUMERATED;
    application_data.type.Enumerated = *((bool *) mValue->getDataPtr());

  }
  
  data.application_data_len = bacapp_encode_data(data.application_data, &application_data);
  data.priority = static_cast<CBacnetWritePropertyConfigFB::ServiceConfig *>(mConfigFB->m_stServiceConfig)->mPriority;

  pdu_len += wp_encode_apdu(&pdu[pdu_len], invoke_id, &data);

  pdu[0] = BVLL_TYPE_BACNET_IP;
  pdu[1] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&pdu[2], pdu_len);

  return pdu_len;
  
}

void CBacnetWritePropertyHandle::decodeServiceResp(uint8_t *pdu, const uint32_t &len) {
  DEVLOG_DEBUG("[CBacnetWritePropertyHandle] decodeServiceResp(): Received WriteProperty Acknowledge!\n");
  fireConfirmationEvent();
}
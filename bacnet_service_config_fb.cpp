#include "bacnet_service_config_fb.h"
#include <core/io/mapper/io_mapper.h>
#include "bacnet_client_controller.h"
#include "bacnet_client_config_fb.h"

CBacnetServiceConfigFB::CBacnetServiceConfigFB(CResource *paSrcRes, const SFBInterfaceSpec *paInterfaceSpec, const CStringDictionary::TStringId paInstanceNameId, TForteByte *paFBConnData, TForteByte *paFBVarsData): forte::core::io::IOConfigFBBase(paSrcRes, paInterfaceSpec, paInstanceNameId, paFBConnData, paFBVarsData), m_stServiceConfig(0), m_enNotificationType(UnknownNotificationType)
{
}

CBacnetServiceConfigFB::~CBacnetServiceConfigFB()
{
}


void CBacnetServiceConfigFB::setNotificationType(EServiceConfigFBNotificationType paNotifycationType) {
  m_enNotificationType = paNotifycationType;
}

void CBacnetServiceConfigFB::executeEvent(int pa_nEIID){
  if(BACnetAdapterIn().INIT() == pa_nEIID) {
    const char* const error = init();
    if(error){
      QO() = false;
      STATUS() = error;
    }
    // initialization done, continue
    if(BACnetAdapterOut().getPeer() == 0) {
      // backpropagate inito if last fb in the chain
      sendAdapterEvent(scm_nBACnetAdapterInAdpNum, BACnetAdapter::scm_nEventINITOID);
    } else {
      // forward init evenet
      sendAdapterEvent(scm_nBACnetAdapterOutAdpNum, BACnetAdapter::scm_nEventINITID);
    }

  } else if(BACnetAdapterOut().INITO() == pa_nEIID) {
     // backpropagate inito
      sendAdapterEvent(scm_nBACnetAdapterInAdpNum, BACnetAdapter::scm_nEventINITOID);
  } else if(cg_nExternalEventID == pa_nEIID){
    // external event, client controller notifies about some event
    switch(m_enNotificationType){
      case Success:
        QO() = true;
        STATUS() = scm_sInitOK;
        break;
      case AddrFetchFailed:
        QO() = false;
        STATUS() = scm_sAddrFetchFailed;
        break;
      case COVSubscriptionFailed:
        QO() = false;
        STATUS() = scm_sCOVSubscriptionFailed;
        break;
      default:
        break;
    }
  }
}


const char *CBacnetServiceConfigFB::init() {
  // set configuration struct
  if(!setConfig())
    return scm_sFBInitFailed;
  // initialize new handle
  CBacnetClientController *clientController = static_cast<CBacnetClientController *>(CBacnetClientConfigFB::getClientConfigFB()->getDeviceController());
  if(!initHandle(clientController))
    return scm_sHandleInitFailed;
  // set executor for external events
  setEventChainExecutor(m_poInvokingExecEnv);
  return 0;
}

CIEC_ANY::EDataTypeID CBacnetServiceConfigFB::objectPropertyAndTypeToIECDataType(BACNET_OBJECT_TYPE paBacnetObjectType, BACNET_PROPERTY_ID paBacnetObjectProperty) {

  if((OBJECT_ANALOG_OUTPUT == paBacnetObjectType || 
     OBJECT_ANALOG_INPUT == paBacnetObjectType ||
     OBJECT_ANALOG_VALUE ==  paBacnetObjectType) 
     && PROP_PRESENT_VALUE == paBacnetObjectProperty) {
        return CIEC_ANY::e_DWORD;
  } else if ((OBJECT_BINARY_OUTPUT == paBacnetObjectType || 
             OBJECT_BINARY_INPUT ==  paBacnetObjectType ||
             OBJECT_BINARY_VALUE ==  paBacnetObjectType) &&
             PROP_PRESENT_VALUE == paBacnetObjectProperty) { 
        return CIEC_ANY::e_BOOL;
  } 
  return CIEC_ANY::e_Max;
}

BACNET_OBJECT_TYPE CBacnetServiceConfigFB::stringToBacnetObjectType(CIEC_WSTRING paObjectType) {
  if("ANALOG_OUTPUT" == paObjectType) {
    return OBJECT_ANALOG_OUTPUT;
  } else if ("ANALOG_INPUT" == paObjectType) {
    return OBJECT_ANALOG_INPUT;
  } else if ("ANALOG_VALUE" == paObjectType) {
    return OBJECT_ANALOG_VALUE;
  } else if ("BINARY_OUTPUT" == paObjectType) {
    return OBJECT_BINARY_OUTPUT;
  } else if ("BINARY_INPUT" == paObjectType) {
    return OBJECT_BINARY_INPUT;
  } else if ("BINARY_VALUE" == paObjectType) {
    return OBJECT_BINARY_VALUE;
  } else {
    return MAX_BACNET_OBJECT_TYPE;
  }
}

BACNET_PROPERTY_ID CBacnetServiceConfigFB::stringToBacnetObjectProperty(CIEC_WSTRING paObjectProperty) {
  if("PRESENT_VALUE" == paObjectProperty){
    return PROP_PRESENT_VALUE;
  }  else {
    return MAX_BACNET_PROPERTY_ID;
  }
}
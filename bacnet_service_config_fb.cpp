#include "bacnet_service_config_fb.h"
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_mapper.h"
#include "bacnet_client_controller.h"
#include "BACnetClient.h"

const char* const CBacnetServiceConfigFB::scmFBInitFailed = "FB initialization failed";
const char* const CBacnetServiceConfigFB::scmHandleInitFailed = "Handle initialization failed";
const char* const CBacnetServiceConfigFB::scmAddrFetchFailed = "Address discovery failed";
const char* const CBacnetServiceConfigFB::scmCOVSubscriptionFailed = "COV subscription failed";
const char* const CBacnetServiceConfigFB::scmInitOK = "Initialized";

CBacnetServiceConfigFB::CBacnetServiceConfigFB(EServiceType paServiceType, CResource *paSrcRes, const SFBInterfaceSpec *paInterfaceSpec, const CStringDictionary::TStringId paInstanceNameId, TForteByte *paFBConnData, TForteByte *paFBVarsData): forte::core::io::IOConfigFBBase(paSrcRes, paInterfaceSpec, paInstanceNameId, paFBConnData, paFBVarsData), mServiceType(paServiceType), m_stServiceConfig(0), mServiceHandle(0), mNotificationType(e_UnknownNotificationType)
{
}

CBacnetServiceConfigFB::~CBacnetServiceConfigFB()
{
}


BACNET_OBJECT_TYPE CBacnetServiceConfigFB::getObjectType(CIEC_WSTRING paObjectType) {
  if(paObjectType == "ANALOG_OUTPUT") {
    return BACNET_OBJECT_TYPE::OBJECT_ANALOG_OUTPUT;
  } else if (paObjectType == "ANALOG_INPUT") {
    return BACNET_OBJECT_TYPE::OBJECT_ANALOG_INPUT;
  } else if (paObjectType == "ANALOG_VALUE") {
    return BACNET_OBJECT_TYPE::OBJECT_ANALOG_VALUE;
  } else if (paObjectType == "BINARY_OUTPUT") {
    return BACNET_OBJECT_TYPE::OBJECT_BINARY_OUTPUT;
  } else if (paObjectType == "BINARY_INPUT") {
    return BACNET_OBJECT_TYPE::OBJECT_BINARY_INPUT;
  } else if (paObjectType == "BINARY_VALUE") {
    return BACNET_OBJECT_TYPE::OBJECT_BINARY_VALUE;
  } else {
    return BACNET_OBJECT_TYPE::MAX_BACNET_OBJECT_TYPE;
  }
}

BACNET_PROPERTY_ID CBacnetServiceConfigFB::getObjectProperty(CIEC_WSTRING paObjectProperty) {
  if(paObjectProperty == "PRESENT_VALUE"){
    return BACNET_PROPERTY_ID::PROP_PRESENT_VALUE;
  } else if (paObjectProperty == "COV_INCREMENT") {
    return BACNET_PROPERTY_ID::PROP_COV_INCREMENT;
  } else {
    return BACNET_PROPERTY_ID::MAX_BACNET_PROPERTY_ID;
  }
}

void CBacnetServiceConfigFB::setHandle(CBacnetServiceHandle* handle) {
  mServiceHandle = handle;
}


void CBacnetServiceConfigFB::setNotificationType(EServiceConfigFBNotificationType paNotifycationType) {
  mNotificationType = paNotifycationType;
}

void CBacnetServiceConfigFB::executeEvent(int pa_nEIID){
  if(BACnetAdapterIn().INIT() == pa_nEIID) {
    DEVLOG_DEBUG("[BACnetReadPropertyConfigFB] init event\n");
    
    // todo error message if init() return -1
    const char* const error = init();
  
    if(error){
      QO() = false;
      STATUS() = error;
    }

    if(BACnetAdapterOut().getPeer() == 0) {
      // backpropagate inito
      BACnetAdapterIn().QO() = QO();
      sendAdapterEvent(scm_nBACnetAdapterInAdpNum, BACnetAdapter::scm_nEventINITOID);
    } else {
      // forward init
      BACnetAdapterOut().MasterId() = BACnetAdapterIn().MasterId();
      BACnetAdapterOut().Index() = (TForteUInt16) (BACnetAdapterIn().Index() + 1);
      BACnetAdapterOut().QI() = BACnetAdapterIn().QI();
      sendAdapterEvent(scm_nBACnetAdapterOutAdpNum, BACnetAdapter::scm_nEventINITID);
    }

  } else if(BACnetAdapterOut().INITO() == pa_nEIID) {
     // backpropagate inito
      BACnetAdapterIn().QO() = BACnetAdapterOut().QO() && QO();
      sendAdapterEvent(scm_nBACnetAdapterInAdpNum, BACnetAdapter::scm_nEventINITOID);
  } else if(cg_nExternalEventID == pa_nEIID){
    switch(mNotificationType){
      case e_Success:
        QO() = true;
        STATUS() = scmInitOK;
        break;
      case e_AddrFetchFailed:
        QO() = false;
        STATUS() = scmAddrFetchFailed;
        break;
      case e_COVSubscriptionFailed:
        QO() = false;
        STATUS() = scmCOVSubscriptionFailed;
        break;
      default:
        break;
    }
  }
}


const char *CBacnetServiceConfigFB::init() {
  if(!setConfig())
    return scmFBInitFailed;

  CBacnetClientController *clientController = static_cast<CBacnetClientController *>(CBacnetClientConfigFB::getClientConfigFB()->getDeviceController());

  if(!initHandle(clientController))
    return scmHandleInitFailed;

  clientController->updateSCFBsList(this);
  
  setEventChainExecutor(m_poInvokingExecEnv);
  
  return 0;
}

#include "bacnet_service_config_fb.h"

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
  }
}

BACNET_PROPERTY_ID CBacnetServiceConfigFB::getObjectProperty(CIEC_WSTRING paObjectProperty) {
  if(paObjectProperty == "PRESENT_VALUE"){
    return BACNET_PROPERTY_ID::PROP_PRESENT_VALUE;
  } else if (paObjectProperty == "COV_INCREMENT") {
    return BACNET_PROPERTY_ID::PROP_COV_INCREMENT;
  }
}

void CBacnetServiceConfigFB::setHandle(CBacnetServiceHandle* handle) {
  mServiceHandle = handle;
}


void CBacnetServiceConfigFB::setNotificationType(EServiceConfigFBNotificationType paNotifycationType) {
  mNotificationType = paNotifycationType;
}
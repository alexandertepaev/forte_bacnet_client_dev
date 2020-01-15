#include "bacnet_service_config_fb.h"

CBacnetServiceConfigFB::CBacnetServiceConfigFB(/* args */) : m_stServiceConfig(0)
{
}

CBacnetServiceConfigFB::~CBacnetServiceConfigFB()
{
}


BACNET_OBJECT_TYPE CBacnetServiceConfigFB::getObjectType(CIEC_WSTRING paObjectType) {
  if(paObjectType == "ANALOG_OUTPUT") {
    return BACNET_OBJECT_TYPE::OBJECT_ANALOG_OUTPUT;
  } else if (paObjectType == "BINARY_OUTPUT") {
    return BACNET_OBJECT_TYPE::OBJECT_BINARY_OUTPUT;
  }
}

BACNET_PROPERTY_ID CBacnetServiceConfigFB::getObjectProperty(CIEC_WSTRING paObjectProperty) {
  if(paObjectProperty == "PRESENT_VALUE"){
    return BACNET_PROPERTY_ID::PROP_PRESENT_VALUE;
  }
}
#ifndef _BACNET_SERVICE_CONFIG_FB_H_
#define _BACNET_SERVICE_CONFIG_FB_H_

#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_master_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_mapper.h"

#include "include/bacnet.h"
// #include "bacnet_service_handle.h"

class CBacnetServiceHandle;

class CBacnetServiceConfigFB
{
private:
  /* data */

protected:

  BACNET_OBJECT_TYPE getObjectType(CIEC_WSTRING paObjectType);
  BACNET_PROPERTY_ID getObjectProperty(CIEC_WSTRING paObjectProperty);


public:
  CBacnetServiceConfigFB(/* args */);
  ~CBacnetServiceConfigFB();

  // rework
  struct ServiceConfig {
    uint32_t mDeviceId;
    uint32_t mObjectType;
    uint32_t mObjectId;
    //uint32_t mObjectProperty;
    //uint32_t mArrayIndex;

    // ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId, uint32_t paObjectProperty, uint32_t paArrayIndex) :
    // mDeviceId(paDeviceId), mObjectType(paObjectType), mObjectId(paObjectId), mObjectProperty(paObjectProperty), mArrayIndex(paArrayIndex) {

    // }
    ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId) :
    mDeviceId(paDeviceId), mObjectType(paObjectType), mObjectId(paObjectId) {

    }
  };

  //void foo(forte::core::io::IOHandle* handle);
  void setHandle(CBacnetServiceHandle* handle);
  

  ServiceConfig *m_stServiceConfig;
  CBacnetServiceHandle *mServiceHandle;

};

#endif
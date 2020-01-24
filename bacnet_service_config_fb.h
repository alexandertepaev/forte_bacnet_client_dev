#ifndef _BACNET_SERVICE_CONFIG_FB_H_
#define _BACNET_SERVICE_CONFIG_FB_H_

#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_master_multi.h"
#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_base.h"
#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_mapper.h"

#include "include/bacnet.h"
//#include "bacnet_client_controller.h"
// #include "bacnet_service_handle.h"

class CBacnetServiceHandle;

class CBacnetServiceConfigFB: public forte::core::io::IOConfigFBBase
{
private:
  /* data */

protected:

  BACNET_OBJECT_TYPE getObjectType(CIEC_WSTRING paObjectType);
  BACNET_PROPERTY_ID getObjectProperty(CIEC_WSTRING paObjectProperty);


public:

  enum EServiceType {
    e_ReadProperty,
    e_WriteProperty,
    e_UnconfirmedCOVSub,
  };

  EServiceType mServiceType;

  CBacnetServiceConfigFB(EServiceType paServiceType, CResource *paSrcRes, const SFBInterfaceSpec *paInterfaceSpec, const CStringDictionary::TStringId paInstanceNameId, TForteByte *paFBConnData, TForteByte *paFBVarsData);
  ~CBacnetServiceConfigFB();

  // rework
  struct ServiceConfig {
    uint32_t mDeviceId;
    uint32_t mObjectType;
    uint32_t mObjectId;
  
    ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId) :
    mDeviceId(paDeviceId), mObjectType(paObjectType), mObjectId(paObjectId) {

    }
  };

  void setHandle(CBacnetServiceHandle* handle);
  

  ServiceConfig *m_stServiceConfig;
  CBacnetServiceHandle *mServiceHandle;

  

  enum EServiceConfigFBNotificationType {
      e_UnknownNotificationType,
      e_AddrFetchFailed,
      e_COVSubscriptionFailed,
      e_Success,
  };

  EServiceConfigFBNotificationType mNotificationType;

  void setNotificationType(EServiceConfigFBNotificationType paNotificationType);

};

#endif
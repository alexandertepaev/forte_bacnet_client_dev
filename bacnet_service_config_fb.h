#ifndef _BACNET_SERVICE_CONFIG_FB_H_
#define _BACNET_SERVICE_CONFIG_FB_H_

// #include"../../forte-incubation_1.11.0/src/core/io/configFB/io_base.h"
#include <core/io/configFB/io_base.h>
// #include "include/bacnet.h"
#include "forte_bacnet.h"
#include "BACnetAdapter.h"

class CBacnetServiceHandle;
class CBacnetClientController; 

class CBacnetServiceConfigFB: public forte::core::io::IOConfigFBBase {

private:
  /* data */

  BACnetAdapter& BACnetAdapterOut() {
    return (*static_cast<BACnetAdapter*>(m_apoAdapters[0]));
  };
  static const int scm_nBACnetAdapterOutAdpNum = 0;
  BACnetAdapter& BACnetAdapterIn() {
    return (*static_cast<BACnetAdapter*>(m_apoAdapters[1]));
  };
  static const int scm_nBACnetAdapterInAdpNum = 1;

  CIEC_BOOL &QO() {
    return *static_cast<CIEC_BOOL*>(getDO(0));
  };

  CIEC_WSTRING &STATUS() {
    return *static_cast<CIEC_WSTRING*>(getDO(1));
  };


  const char* init();
  
protected:

  static const char* const scmFBInitFailed;
  static const char* const scmHandleInitFailed;
  static const char* const scmAddrFetchFailed;
  static const char* const scmCOVSubscriptionFailed;
  static const char* const scmInitOK;

  void executeEvent(int pa_nEIID);

  //const char* finalizeInit();
  
  virtual bool setConfig() = 0;

  virtual bool initHandle(CBacnetClientController *paController) = 0;



  BACNET_OBJECT_TYPE getObjectType(CIEC_WSTRING paObjectType);
  BACNET_PROPERTY_ID getObjectProperty(CIEC_WSTRING paObjectProperty);

  CIEC_ANY::EDataTypeID getIECDataType(BACNET_OBJECT_TYPE);


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
    uint32_t mDeviceID;
    BACNET_OBJECT_TYPE mObjectType;
    uint32_t mObjectID;
  
    ServiceConfig(uint32_t paDeviceId, BACNET_OBJECT_TYPE paObjectType, uint32_t paObjectId) :
    mDeviceID(paDeviceId), mObjectType(paObjectType), mObjectID(paObjectId) {

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
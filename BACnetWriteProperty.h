/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetWriteProperty
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2019-12-30/root -  - 
 *************************************************************************/

#ifndef _BACNETWRITEPROPERTY_H_
#define _BACNETWRITEPROPERTY_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_string.h>
#include <forte_uint.h>
#include <forte_wstring.h>
#include "BACnetAdapter.h"
#include "bacnet_service_config_fb.h"
#include "bacnet_client_controller.h"


//class CBacnetWritePropertyConfigFB: public forte::core::io::IOConfigFBBase, public CBacnetServiceConfigFB{
class CBacnetWritePropertyConfigFB: public CBacnetServiceConfigFB{ 
  DECLARE_FIRMWARE_FB(CBacnetWritePropertyConfigFB)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];
  CIEC_BOOL &QI() {
    return *static_cast<CIEC_BOOL*>(getDI(0));
  };

  CIEC_WSTRING &ObserverName() {
    return *static_cast<CIEC_WSTRING*>(getDI(1));
  };

  CIEC_UINT &DeviceID() {
    return *static_cast<CIEC_UINT*>(getDI(2));
  };

  CIEC_WSTRING &ObjectType() {
    return *static_cast<CIEC_WSTRING*>(getDI(3));
  };

  CIEC_UINT &ObjectID() {
    return *static_cast<CIEC_UINT*>(getDI(4));
  };

  CIEC_WSTRING &ObjectProperty() {
    return *static_cast<CIEC_WSTRING*>(getDI(5));
  };

  CIEC_UINT &Priority() {
    return *static_cast<CIEC_UINT*>(getDI(6));
  };

  CIEC_UINT &ArrayIndex() {
    return *static_cast<CIEC_UINT*>(getDI(7));
  };

  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];
  CIEC_BOOL &QO() {
    return *static_cast<CIEC_BOOL*>(getDO(0));
  };

  CIEC_WSTRING &STATUS() {
    return *static_cast<CIEC_WSTRING*>(getDO(1));
  };

  static const TEventID scm_nEventINDID = 0;
  static const TForteInt16 scm_anEOWithIndexes[];
  static const TDataIOID scm_anEOWith[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SAdapterInstanceDef scm_astAdapterInstances[];

  BACnetAdapter& BACnetAdapterOut() {
    return (*static_cast<BACnetAdapter*>(m_apoAdapters[0]));
  };
  static const int scm_nBACnetAdapterOutAdpNum = 0;
  BACnetAdapter& BACnetAdapterIn() {
    return (*static_cast<BACnetAdapter*>(m_apoAdapters[1]));
  };
  static const int scm_nBACnetAdapterInAdpNum = 1;
  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

   FORTE_FB_DATA_ARRAY(1, 8, 2, 2);

  void executeEvent(int pa_nEIID);

  const char* init();

  int m_nIndex;

  static const char* const scmError;
  static const char* const scmOK;
  static const char* const scmAddrFetchFailed;
  static const char* const scmHandleInitFailed;


public:
  // FUNCTION_BLOCK_CTOR_WITH_BASE_CLASS(CBacnetWritePropertyConfigFB, forte::core::io::IOConfigFBBase), CBacnetServiceConfigFB(e_WriteProperty) {
  
  // };
  CBacnetWritePropertyConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
  CBacnetServiceConfigFB(e_WriteProperty, pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {

  };


  virtual ~CBacnetWritePropertyConfigFB(){};


  struct ServiceConfig : CBacnetServiceConfigFB::ServiceConfig {

    uint32_t mObjectProperty;
    uint32_t mPriority;
    uint32_t mArrayIndex;
    
    ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId, uint32_t paObjectProperty, uint32_t paArrayIndex, uint32_t paPriority) :
    CBacnetServiceConfigFB::ServiceConfig(paDeviceId, paObjectType, paObjectId), mObjectProperty(paObjectProperty), mArrayIndex(paArrayIndex) {
      if(paPriority < 1 || paPriority > 16) {
        mPriority = 16;
      } else {
        mPriority = paPriority;
      }
    };
  };

};

#endif //close the ifdef sequence from the beginning of the file


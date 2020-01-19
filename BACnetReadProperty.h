/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetReadProperty
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2019-10-30/root -  - 
 *************************************************************************/

#ifndef _BACNETREADPROPERTY_H_
#define _BACNETREADPROPERTY_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_uint.h>
#include <forte_wstring.h>
#include "BACnetAdapter.h"
#include "bacnet_service_config_fb.h"
#include "bacnet_client_controller.h"


#include "../../forte-incubation_1.11.0/src/core/io/configFB/io_master_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller_multi.h"
#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_master_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_mapper.h"
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_handle.h"

class CBacnetReadPropertyConfigFB: public forte::core::io::IOConfigFBBase, public CBacnetServiceConfigFB{
  DECLARE_FIRMWARE_FB(CBacnetReadPropertyConfigFB)

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

  CIEC_UINT &ArrayIndex() {
    return *static_cast<CIEC_UINT*>(getDI(6));
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

   FORTE_FB_DATA_ARRAY(1, 7, 2, 2);

  void executeEvent(int pa_nEIID);

  const char* init();

  int m_nIndex;

  static const char* const scmError;
  static const char* const scmOK;

public:



  FUNCTION_BLOCK_CTOR_WITH_BASE_CLASS(CBacnetReadPropertyConfigFB, forte::core::io::IOConfigFBBase), CBacnetServiceConfigFB() {
  };

  // CBacnetReadPropertyConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
  // forte::core::io::IOConfigFBBase( pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData), CBacnetServiceConfigFB() {

  // };


  virtual ~CBacnetReadPropertyConfigFB(){};

  /*  adding new config fields
    struct ServiceConfig : CBacnetServiceConfigFB::ServiceConfig {
      uint32_t dummy_value;
      
      ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId, uint32_t paObjectProperty, uint32_t paArrayIndex, uint32_t paDummyValue) :
      CBacnetServiceConfigFB::ServiceConfig(paDeviceId, paObjectType, paObjectId, paObjectProperty, paArrayIndex), dummy_value(paDummyValue) {

      };
    };
  */
  struct ServiceConfig : CBacnetServiceConfigFB::ServiceConfig {

    uint32_t mObjectProperty;
    uint32_t mArrayIndex;
    
    // ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId, uint32_t paObjectProperty, uint32_t paArrayIndex, uint32_t paPriority) :
    // CBacnetServiceConfigFB::ServiceConfig(paDeviceId, paObjectType, paObjectId, paObjectProperty, paArrayIndex) {
    //   if(paPriority < 1 || paPriority > 16) {
    //     mPriority = 16;
    //   } else {
    //     mPriority = paPriority;
    //   }
    // };
    ServiceConfig(uint32_t paDeviceId, uint32_t paObjectType, uint32_t paObjectId, uint32_t paObjectProperty, uint32_t paArrayIndex) :
    CBacnetServiceConfigFB::ServiceConfig(paDeviceId, paObjectType, paObjectId), mObjectProperty(paObjectProperty), mArrayIndex(paArrayIndex) {
      
    };
  };


};

#endif //close the ifdef sequence from the beginning of the file


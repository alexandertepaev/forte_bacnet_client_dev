/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetSubscribeUnconfirmedCOV
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2020-01-18/root -  - 
 *************************************************************************/

#ifndef _BACNETSUBSCRIBEUNCONFIRMEDCOV_H_
#define _BACNETSUBSCRIBEUNCONFIRMEDCOV_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_uint.h>
#include <forte_wstring.h>
#include "BACnetAdapter.h"
#include "bacnet_service_config_fb.h"
#include "bacnet_client_controller.h"

// class CBacnetSubscribeUnconfirmedCOVConfigFB: public forte::core::io::IOConfigFBBase, public CBacnetServiceConfigFB {
class CBacnetSubscribeUnconfirmedCOVConfigFB: public CBacnetServiceConfigFB {
  DECLARE_FIRMWARE_FB(CBacnetSubscribeUnconfirmedCOVConfigFB)

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

   FORTE_FB_DATA_ARRAY(1, 5, 2, 2);

  void executeEvent(int pa_nEIID);

  const char* init();

  int m_nIndex;

  static const char* const scmError;
  static const char* const scmOK;
  static const char* const scmAddrFetchFailed;
  static const char* const scmHandleInitFailed;
  static const char* const scmCOVSubscriptionFailed;

public:

  CBacnetSubscribeUnconfirmedCOVConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
  CBacnetServiceConfigFB(e_UnconfirmedCOVSub, pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {

  };

  virtual ~CBacnetSubscribeUnconfirmedCOVConfigFB(){};


};

#endif //close the ifdef sequence from the beginning of the file


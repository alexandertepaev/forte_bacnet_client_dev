/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetClient
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2019-10-24/root - null - 
 *************************************************************************/

#ifndef _BACNETCLIENT_H_
#define _BACNETCLIENT_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_uint.h>
#include <forte_wstring.h>
#include "BACnetAdapter.h"

#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_configFB_controller.h"
//#include "bacnet_client_controller.h"

class CBacnetClientConfigFB: public forte::core::io::IOConfigFBController{
  DECLARE_FIRMWARE_FB(CBacnetClientConfigFB)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];

  static CBacnetClientConfigFB *mBacnetClientConfigFB;

  
  CIEC_BOOL &QI() {
    return *static_cast<CIEC_BOOL*>(getDI(0));
  };

  CIEC_UINT &Port() {
    return *static_cast<CIEC_UINT*>(getDI(1));
  };

  // CIEC_UINT &DeviceObjectID() {
  //   return *static_cast<CIEC_UINT*>(getDI(2));
  // };

  // CIEC_WSTRING &DeviceObjectName() {
  //   return *static_cast<CIEC_WSTRING*>(getDI(3));
  // };

  // CIEC_WSTRING &PathToAddrFile() {
  //   return *static_cast<CIEC_WSTRING*>(getDI(4));
  // };

  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];
  CIEC_BOOL &QO() {
    return *static_cast<CIEC_BOOL*>(getDO(0));
  };

  CIEC_WSTRING &STATUS() {
    return *static_cast<CIEC_WSTRING*>(getDO(1));
  };

  static const TEventID scm_nEventINITID = 0;
  static const TForteInt16 scm_anEIWithIndexes[];
  static const TDataIOID scm_anEIWith[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TEventID scm_nEventINITOID = 0;
  static const TForteInt16 scm_anEOWithIndexes[];
  static const TDataIOID scm_anEOWith[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SAdapterInstanceDef scm_astAdapterInstances[];

  BACnetAdapter& BACnetAdapterOut() {
    return (*static_cast<BACnetAdapter*>(m_apoAdapters[0]));
  };
  static const int scm_nBACnetAdapterOutAdpNum = 0;
  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

  // FORTE_FB_DATA_ARRAY(1, 5, 2, 1);
  FORTE_FB_DATA_ARRAY(1, 2, 2, 1);

  void executeEvent(int pa_nEIID);

  static int sm_nControllerInstances;

  int m_nId;

  void onStartup();

public:
 
  /*
  FUNCTION_BLOCK_CTOR_WITH_BASE_CLASS(CBacnetClientConfigFB, forte::core::io::IOConfigFBController){
  }
  */
  CBacnetClientConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes);
  ~CBacnetClientConfigFB();

  forte::core::io::IODeviceController* createDeviceController(CDeviceExecution& paDeviceExecution);

  void setConfig();

  static CBacnetClientConfigFB* getClientConfigFB();

};

#endif //close the ifdef sequence from the beginning of the file


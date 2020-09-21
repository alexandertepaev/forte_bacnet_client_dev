/*******************************************************************************
 * Copyright (c) 2020 Alexander Tepaev github.com/alexandertepaev
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev
 *******************************************************************************/

#ifndef _BACNETREADPROPERTY_H_
#define _BACNETREADPROPERTY_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_uint.h>
#include <forte_wstring.h>

#include "BACnetAdapter.h"
#include "bacnet_service_config_fb.h"


/*! @brief Concrete class representing BACnet ReadProperty service configuration fb
 *
 * ReadProperty Service Configuration FB class. 
 * BACnet ReadProperty Service Configuration FB is a FB that is responible for configuration of 
 * a BACnet ReadProperty server request and a creation of a corresepnding service handle.
 */
class CBacnetReadPropertyConfigFB: public CBacnetServiceConfigFB {
  DECLARE_FIRMWARE_FB(CBacnetReadPropertyConfigFB)

public:
  CBacnetReadPropertyConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes);

  ~CBacnetReadPropertyConfigFB();

  //! Overriden BACnet service request configuration struct, objectProperty and arrayIndex members added
  struct SServiceConfig : CBacnetServiceConfigFB::SServiceConfig {
    TForteUInt16 objectProperty; // Integer, indicating target property of the accessed object
    TForteUInt32 arrayIndex; // Integer, indicating index of the target array element in case the accessed property is an array, hardcoded atm
      SServiceConfig(TForteUInt32 paDeviceId, BACNET_OBJECT_TYPE paObjectType, TForteUInt16 paObjectId, TForteUInt16 paObjectProperty, TForteUInt32 paArrayIndex) :
      CBacnetServiceConfigFB::SServiceConfig(paDeviceId, paObjectType, paObjectId), objectProperty(paObjectProperty), arrayIndex(paArrayIndex) {
      };
  };

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


  /*! @brief Sets configuration struct
   *
   * Overriden absctract method of CBacnetServiceConfigFB class, which is used for checking
   * the configuration data found on the FBs input interface and storing this data into the
   * configuration struct (m_stServiceConfig).
   * 
   * @return 'true' in case of success, 'false' otherwise
   */
  bool setConfig();

  /*! @brief Creates a handle descriptor and passes it to client controller for the initialization of a service handle
   *
   * In this method, a handle descriptor is created and passed as an argument to the client controller's addHandle method.
   * In the initHandle method, specific BACnet service handle instance is created and registered into the IOMapper.
   * 
   * @param paController Pointer to the client controller instance
   * @return 'true' in case of success, 'false' otherwise
   */
  bool initHandle(CBacnetClientController *paController);
};

#endif


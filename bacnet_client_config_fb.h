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

#ifndef _BACNETCLIENT_H_
#define _BACNETCLIENT_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_uint.h>
#include <forte_wstring.h>
#include "BACnetAdapter.h"

//#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_configFB_controller.h"
#include <core/io/configFB/io_configFB_controller.h>
//#include "bacnet_client_controller.h"

class CBacnetClientController;
typedef CSinglyLinkedList<CBacnetClientController *> TControllerList;

/*! @brief BACnet client configuration fb
 *
 * BACnet client configuration FB class, which represents a FB standing in the head of the client configuration daisy-chain.
 * Inherits from the IOConfigFBController abstract class, which is used to instantiate the device (client) controller. 
 */
class CBacnetClientConfigFB: public forte::core::io::IOConfigFBController{
  DECLARE_FIRMWARE_FB(CBacnetClientConfigFB)

public:
 
  CBacnetClientConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes);
  ~CBacnetClientConfigFB();

  /*! @brief Returns a pointer to client controller
   *
   * This method returns a pointer to client controller instance.
   * Used by SCFBs. 
   * 
   * @param paControllerID Numeric value indicating ID of the Client Controller
   * @return Pointer to client controller instance
   */
  static CBacnetClientController* getClientController(TForteUInt16 paControllerID);
  
protected:
  /*! @brief Creates a BACnet client controller and pushes it to the list of controller instances
   *
   * 
   * @param paDeviceExecution Reference to the instance responsible for handling IEC 61499 FB execution requests
   * @return Pointer to the instance of the created client controller 
   */
  inline forte::core::io::IODeviceController* createDeviceController(CDeviceExecution& paDeviceExecution);

  /*! @brief Creates and sets client controller's configuration structure
   *
   * Creates a structure with BACnet client's configuration parameters (e.g., port number of the communication socket)
   * Sets the corresponding member structure of the client controller (m_stConfig) by calling controller's setConfig method  
   * 
   */
  void setConfig();


  /*! @brief Signals start of the client controller
   * 
   * This method is called after a successful start of the client controller.
   * Passes the BACnetAdapter.INIT event to the next fb of the configuration daisy-chain 
   */
  void onStartup();

private:

  static TForteUInt16 sm_nControllerCounter; //! Static member for counting Client Controller instances, 
  
  static TControllerList smControllerInstances; //! Static list holding Server Controller instances



  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];
  
  CIEC_BOOL &QI() {
    return *static_cast<CIEC_BOOL*>(getDI(0));
  };

  CIEC_UINT &Port() {
    return *static_cast<CIEC_UINT*>(getDI(1));
  };

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

  FORTE_FB_DATA_ARRAY(1, 2, 2, 1);

  /*! @brief Handles input events 
   *
   * This method represents an entry point of ClientConfigFB's execution.
   * Calls IOConfigFBController's executeEvent, which results in the initialization of device controller.
   * 
   * @param pa_nEIID ID of the triggered input event
   */
  void executeEvent(int pa_nEIID);
  
};

#endif //close the ifdef sequence from the beginning of the file


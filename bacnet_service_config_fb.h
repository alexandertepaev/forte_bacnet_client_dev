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
#ifndef _BACNET_SERVICE_CONFIG_FB_H_
#define _BACNET_SERVICE_CONFIG_FB_H_

#include <core/io/configFB/io_base.h>
#include "forte_bacnet.h"
#include "BACnetAdapter.h"

class CBacnetServiceHandle;
class CBacnetClientController; 

/*! @brief Abstract BACnet service configuration fb class
 *
 * BACnet service configuration FB class. 
 * BACnet Service Configuration FB is a FB that is responible for configuration of a BACnet server request.
 * Together with BACnet client controller (CBacnetClientController), it is responsible for creation of a BACnet service handle.
 */
class CBacnetServiceConfigFB: public forte::core::io::IOConfigFBBase {

public:

  CBacnetServiceConfigFB(CResource *paSrcRes, const SFBInterfaceSpec *paInterfaceSpec, const CStringDictionary::TStringId paInstanceNameId, TForteByte *paFBConnData, TForteByte *paFBVarsData);
  
  ~CBacnetServiceConfigFB();

  //! BACnet service request base configuration struct
  struct SServiceConfig {
    TForteUInt32 deviceID; //!< Device ID of a remote device
    BACNET_OBJECT_TYPE objectType; //!< Target object type (BACNET_OBJECT_TYPE enum provided by libbacnet)
    TForteUInt32 objectID; //!< Target object ID
    SServiceConfig(TForteUInt32 paDeviceID, BACNET_OBJECT_TYPE paObjectType, TForteUInt32 paObjectID) :
    deviceID(paDeviceID), objectType(paObjectType), objectID(paObjectID) {
    }
  };
  SServiceConfig *m_stServiceConfig; //!< Member variable for storing service configuration

  //! Notification types enum
  enum EServiceConfigFBNotificationType {
      UnknownNotificationType,
      AddrFetchFailed,
      COVSubscriptionFailed,
      Success,
  };

  /*! @brief Sets the m_enNotificationType member variable
   *
   * Method for setting the notification type member variable m_enNotificationType.
   * Used by the client controller in the ConfigFBsNotification state.
   * The notification itself is done by triggering fbs external event.
   * 
   * @param paNotificationType Notification type of type EServiceConfigFBNotificationType
   */
  void setNotificationType(EServiceConfigFBNotificationType paNotificationType); 

protected:

  /*! @brief Abstract method for setting the configuration struct of the config FB
   *
   * In this method, derived classes implement a logic, following which the configuration data of the 
   * service config FB is checked and saved into the configuration struct (m_stServiceConfig)
   * Overriden by CBacnetReadPropertyConfigFB, CBacnetWritePropertyConfigFB, CBacnetUnconfirmedCOVHandle.
   * 
   * @return 'true' in case of success, 'false' otherwise
   */
  virtual bool setConfig() = 0;

  /*! @brief Abstract method for initialization of a new service handle
   *
   * In this method, a service config fb asks the client controller to initialize a new service handle.
   * Overriden by CBacnetReadPropertyConfigFB, CBacnetWritePropertyConfigFB, CBacnetUnconfirmedCOVHandle.
   * 
   * @param paController Pointer to the client controller instance
   * @return 'true' in case of success, 'false' otherwise
   */
  virtual bool initHandle(CBacnetClientController *paController) = 0;


  /*! @brief Method for figuring out the datatype of a target property based on the object type and the property type
   *
   */
  CIEC_ANY::EDataTypeID objectPropertyAndTypeToIECDataType(BACNET_OBJECT_TYPE paBacnetObjectType, BACNET_PROPERTY_ID paBacnetObjectProperty);
  
  /*! @brief Method for converting input string to BACNET_OBJECT_TYPE enum provided by libbacnet
   *
   * @param paObjectType String containing the name of the type of accessed object
   * @return BACNET_OBJECT_TYPE enum (see bacenum.h)
   */
  BACNET_OBJECT_TYPE stringToBacnetObjectType(CIEC_WSTRING paObjectType);

  /*! @brief Method for converting input string to BACNET_PROPERTY_ID enum provided by libbacnet
   *
   * @param paObjectProperty String containing the name of the property of accessed object
   * @return BACNET_PROPERTY_ID enum (see bacenum.h)
   * 
   */
  BACNET_PROPERTY_ID stringToBacnetObjectProperty(CIEC_WSTRING paObjectProperty);

private:
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

  constexpr static const char* const scm_sFBInitFailed = "FB initialization failed"; //!< Message, indicating that the initialization of config fg has failed
  constexpr static const char* const scm_sHandleInitFailed = "Handle initialization failed"; //!< Message, indicating that the initialization of the handle for this config fb has failed
  constexpr static const char* const scm_sAddrFetchFailed = "Address discovery failed"; //!< Message, indicating that the address discovery of the target device has failed 
  constexpr static const char* const scm_sCOVSubscriptionFailed = "COV subscription failed"; //!< Message, indicating that the COV subscription to the target object has failed
  constexpr static const char* const scm_sInitOK = "Initialized"; //!< Message, indicating that the initialization succeeded

  EServiceConfigFBNotificationType m_enNotificationType; //!< Member variable indicating notification type, can be set using setNotificationType method

  /*! @brief Handles input events 
   *
   * This method represents execution entry point of service configuration FBs.
   * Upon the initialization (incoming BACnetAdapterIn.INIT), calls init method and continues the propagation of BACnetAdapterOut.INIT event.
   * Upon reception of BACnetAdapterOut.INITO event, continues its backpropagation.
   * Upon reception of external event, updates FBs STATUS and QO outputs
   * 
   * @param pa_nEIID ID of the triggered input event
   */
  void executeEvent(int pa_nEIID);

  /*! @brief Initializes service configuration fb
   *
   * This method is responsible for the initialization of service config fb.
   * In particular, it calls setConfig method of the derived class, in which the configuration struct is set.
   * After setting the configuration struct, it initializes a new handle by calling overriden initHandle method.
   * Afrer sucessfully initializing the configuration struct and the handle, it sets the executer for external events. 
   * 
   * @return Error message, or 0 in case of successful initialization
   */
  const char* init();

};

#endif
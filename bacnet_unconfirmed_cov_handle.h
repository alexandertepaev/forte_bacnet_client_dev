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

#ifndef _BACNET_UNCONFIRMED_COV_HANDLE_H_
#define _BACNET_UNCONFIRMED_COV_HANDLE_H_


#include "bacnet_subucov_service_config_fb.h"
#include "bacnet_service_handle.h"


class CBacnetServiceHandle;

/*! @brief Concrete BACnet service handle class, representing unconfirmedCOVNotifiaction service handle
 *
 * Together with BACnet client controller (CBacnetClientController), CBacnetUnconfirmedCOVHandle is responsible for subscribing to COV notifications (see controller's subscribeToCOVNotifications)
 * and providing the notification data (only value of the present value property atm) to the corresponding IOObserver. 
 * It is mapped to exactly one instance of the IOObserver class by the IOMapper singleton.
 * IOObservers are the BACnet IO function blocks (see ./bacnet_io_fbs folder), which represent IO function blocks, with help of which, a BACnet clien IEC 61499 application reads and writes data.
 * BACnet IO FBs make use of CBacnetServiceHandle's sendRequest and readResponse methods. 
 */
class CBacnetUnconfirmedCOVHandle : public CBacnetServiceHandle
{
public:
  CBacnetUnconfirmedCOVHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetUnconfirmedCOVHandle();

  //! Struct containing COV subscription data.
  // In particular, it tells if the subsscription is acknowledged (bAcknowledgedFlag)
  // And if so, what was the invoke if of the subscrbe service request
  // Used by client controller in subscribeToCOVNotifications
  struct SubscriptionData {
    bool bAcknowledgedFlag;
    TForteUInt8 nSubscriptionInvokeID;
  };
  SubscriptionData m_stSubscriptionData = { };

  //! Needed by the CBacnetServiceHandle, not used in CBacnetUnconfirmedCOVHandle
  int encodeServiceReq(TForteUInt8 *paBuffer, const TForteUInt8 &paInvokeID, BACNET_ADDRESS *paDestAddr, BACNET_ADDRESS *paSrcAddr) { std::ignore = paBuffer;  std::ignore = paInvokeID; std::ignore = paDestAddr; std::ignore = paSrcAddr; return 0; };
  void decodeServiceResp(TForteUInt8 *paBuffer, const TForteUInt16 &paLen) { std::ignore = paBuffer;  std::ignore = paLen; };

  /*! @brief Signals to handle, that a COV notification has been received
   *  
   * This method is called by the client controller upon receiving a COV notification.
   * Sets handle's mValue member to the value received in the notification.
   * 
   * @param paPropertyValue Structure, containing notification data (see libbacnet's BACNET_PROPERTY_VALUE struct)
   */
  void notificationReceived(BACNET_PROPERTY_VALUE paPropertyValue); 

  /*! @brief Signals to handle, that a COV subscription has been acknowledged
   *  
   * This method is called by the client controller upon receiving a COV subscription acknowledge.
   * Changes handle's state to AwaitingResponse
   * 
   * @param paPropertyValue Structure, containing notification data (see libbacnet's BACNET_PROPERTY_VALUE struct)
   */
  void subscriptionAcknowledged();
protected:

private:

  /*! @brief Method called by the BACnet IO FB when requesting a service
   *  
   * Not used in case of CBacnetUnconfirmedCOVHandle.
   */
  void sendRequest(CIEC_ANY *paData) { std::ignore = paData; };

  /*! @brief Method called by the BACnet IO FB when it wants to read the data from the UCOV notification
   *
   * This method is used by the BACnet IO FBs when reading UCOV notification data.
   * Used for getting the data (value of the mValue), and setting the specific output datapoint.
   * 
   * @param paData Pointer to the output port of IO FB for putting out the cov data; there is always cov data, thus always not NULL
   */
  void readResponse(CIEC_ANY *paData);

  

};

#endif

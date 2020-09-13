/*******************************************************************************
 * Copyright (c) 2017 - 2020 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev - initial implementation and documentation
 *******************************************************************************/

#ifndef _BACNET_SERVICE_HANDLE_H_
#define _BACNET_SERVICE_HANDLE_H_


#include <devexec.h>
#include <extevhan.h>
// #include "../../forte-incubation_1.11.0/src/core/io/mapper/io_handle.h"
#include <core/io/mapper/io_handle.h>
// #include "include/bacnet.h"
#include "forte_bacnet.h"
#include <processinterface.h>


class CBacnetServiceConfigFB;
class CBacnetClientController;


/*! @brief Abstract BACnet service handle class
 *
 * BACnet Service handle is a software entity, that is responible for telling the client controller to send out a specific service request.
 * Together with BACnet client controller (CBacnetClientController), it encodes a service request (see encodeServiceReq method)
 * and decodes the application part of the received response (see decodeServiceResp method).
 * It is mapped to exactly one instance of the IOObserver class by the IOMapper singleton.
 * IOObservers are the BACnet IO function blocks (see ./bacnet_io_fbs folder), which represent IO function blocks, with help of which, a BACnet clien IEC 61499 application reads and writes data.
 * BACnet IO FBs make use of CBacnetServiceHandle's sendRequest and readResponse methods. 
 */
class CBacnetServiceHandle : public forte::core::io::IOHandle, protected CExternalEventHandler
{
public:
  CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetServiceHandle();

  CBacnetServiceConfigFB *mConfigFB; //!< Pointer to the service configuration fb holding service configuration data needed for encoding/decoding operations
  
  //! States of the service handle
  enum EBacnetHandleState{
    Idle, AwaitingResponse 
  };
  EBacnetHandleState m_enHandleState;

  //! Type of the handle
  enum EBacnetHandleType{
    ReadPropertyServiceHandle,
    WritePropertyServiceHandle,
    UnconfirmedCOVServiceHandle 
  };
  EBacnetHandleType m_enHandleType;

  /*! @brief Virtual method for encoding a service request
   *
   * This method is used for encoding a specific service request into the provided buffer
   * Overriden by the derived classes (CBacnetReadPropertyHandle, CBacnetWritePropertyHandle, CBacnetUnconfirmedCOVHandle)
   * 
   * @param paBuffer Pointer to a buffer, into which the service request is to be encoded
   * @param paInvokeID Integer, indicating the invoke id of the encoded service request
   * @param paDestAddr Request destination address
   * @param paSrcAddr Request source address
   * @return Integer, indicating byte length of the encoded service request
   */
  virtual int encodeServiceReq(TForteUInt8 *paBuffer, const TForteUInt8 &paInvokeID, BACNET_ADDRESS *paDestAddr, BACNET_ADDRESS *paSrcAddr) = 0;
  
  /*! @brief Virtual method for decoding a service response
   *
   * This method is used for decoding a response to the specific service request
   * Overriden by the derived classes (CBacnetReadPropertyHandle, CBacnetWritePropertyHandle, CBacnetUnconfirmedCOVHandle)
   * 
   * @param paBuffer Pointer to a buffer holding service response
   * @param paLen Integer, indicating byte length of the response
   */
  virtual void decodeServiceResp(TForteUInt8 *paBuffer, const TForteUInt16 &paLen) = 0;
  
  /*! @brief Virtual method called by the BACnet IO FB when requesting a service
   *
   * This method is used by the BACnet IO FBs when requesting a BACnet service.
   * Used to tell the client controller, that there is a request to be sent (handle gets pushed into the controller's handles ring buffer).
   * Overriden by the derived classes (CBacnetReadPropertyHandle, CBacnetWritePropertyHandle, CBacnetUnconfirmedCOVHandle)
   * 
   * @param paData Pointer to the input port of IO FB, which holds the request data, or NULL if the request data is not needed
   */
  virtual void sendRequest(CIEC_ANY *paData) = 0;

  /*! @brief Virtual method called by the BACnet IO FB when it wants to read the service response
   *
   * This method is used by the BACnet IO FBs when reading a service response.
   * Used for getting the response data (value of the mValue), and setting the specific output datapoint.
   * Overriden by the derived classes (CBacnetReadPropertyHandle, CBacnetWritePropertyHandle, CBacnetUnconfirmedCOVHandle)
   * 
   * @param paData Pointer to the output port of IO FB for putting out the response, or NULL if the response does not contain response data
   */
  virtual void readResponse(CIEC_ANY *paData) = 0;

  /*! @brief Tells the handle that transaction deadline was missed
   *
   * Method used by the client controller for setting handles state back to idle upon
   * missing the service request transaction deadline (-> Service request left unresponded)
   */
  void missedTransactionDeadline() {
    m_enHandleState = Idle;
  }

protected:
  /*! Member for holding service request/response data. 
   * Since only present value is supported atm, we store the values of request/response 
   * in the CIEC value variable, which is the converted to either CIEC_REAL or CIEC_BOOL.
   */ 
  CIEC_ANY *mValue; 

  /*! @brief Starts IOObserver's (BACnet IO FB's) external event execution chain 
   *
   * This method is used to start observer's external event execution, which signals to it
   * that a service response is received.
   * As a result of triggering the observer's external event, readResponse gets called. (See Bacnet IO FBs' executeEvent) 
   */
  void fireConfirmationEvent() {
   startNewEventChain(static_cast<forte::core::io::ProcessInterface *>(getObserver()));
  };

private:
  //!< Methods needed for the CExternalEventHandler, not used
  void enableHandler(void) {};
  void disableHandler(void) {};
  void setPriority(int paPriority) {};
  int getPriority(void) const { return 0; };
  size_t getIdentifier() const { return 0; };
  //!< Methods needed for the IOHandle, not used, see sendRequest / readResponse + Bacnet IO FBs
  void get(CIEC_ANY &paValue) { };
  void set(const CIEC_ANY &) { };
};

#endif
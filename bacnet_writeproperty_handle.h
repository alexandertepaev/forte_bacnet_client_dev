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

#ifndef _BACNET_WRITEPROPERTY_HANDLE_H_
#define _BACNET_WRITEPROPERTY_HANDLE_H_

#include "bacnet_service_handle.h"

/*! @brief Concrete BACnet service handle class, representing WriteProperty service handle
 *
 * Together with BACnet client controller (CBacnetClientController), CBacnetWritePropertyHandle encodes a WriteProperty service request (see encodeServiceReq method)
 * and decodes the application part of the received response (see decodeServiceResp method).
 * It is mapped to exactly one instance of the IOObserver class by the IOMapper singleton.
 * IOObservers are the BACnet IO function blocks (see ./bacnet_io_fbs folder), which represent IO function blocks, with help of which, a BACnet clien IEC 61499 application reads and writes data.
 * BACnet IO FBs make use of CBacnetServiceHandle's sendRequest and readResponse methods. 
 */
class CBacnetWritePropertyHandle : public CBacnetServiceHandle
{
public:
  CBacnetWritePropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetWritePropertyHandle();

  /*! @brief Virtual method for encoding a writeproperty service request
  *
  * This method is used for encoding a writeproperty service request into the provided buffer
  * 
  * @param paBuffer Pointer to a buffer, into which the service request is to be encoded
  * @param paInvokeID Integer, indicating the invoke id of the encoded service request
  * @param paDestAddr Request destination address
  * @param paSrcAddr Request source address
  * @return Integer, indicating byte length of the encoded service request
  */
  int encodeServiceReq(TForteUInt8 *paBuffer, const TForteUInt8 &paInvokeID, BACNET_ADDRESS *paDestAddr, BACNET_ADDRESS *paSrcAddr);
  
  /*! @brief Method for decoding a writeproperty service response
   *
   * This method is used for firing IO FB's external event, indicating the reception of a response
   * 
   * @param paBuffer Pointer to a buffer holding service response
   * @param paLen Integer, indicating byte length of the response
   */
  void decodeServiceResp(TForteUInt8 *paBuffer, const TForteUInt16 &paLen);

protected:
  /*! @brief Method called by the BACnet IO FB when requesting a writeproperty service
   *
   * This method is used by the BACnet IO FBs when requesting BACnet writeproperty  service.
   * Used to tell the client controller, that there is a request to be sent (handle gets pushed into the controller's handles ring buffer).
   * Changes handle's state to AwaitingResponse.
   * 
   * @param paData Pointer to the input port of IO FB, which holds the request data; Data is needed for WriteProperty request, thus always not NULL
   */
  void sendRequest(CIEC_ANY *paData);

  /*! @brief Method called by the BACnet IO FB when it wants to read the writeproperty service response
   *
   * This method is used by the BACnet IO FBs to change handle's state to Idle, 
   * signaling it that the writepoperty ack is handled (cnf+ + qo outputs updated)
   * 
   * @param paData Pointer to the output port of IO FB for putting out the response; there is no writeproperty response data, thus always NULL
   */
  void readResponse(CIEC_ANY *paData);

};

#endif

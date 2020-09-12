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


class CBacnetWritePropertyHandle : public CBacnetServiceHandle
{
private:
  /* data */
public:
  CBacnetWritePropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetWritePropertyHandle();

  virtual void set(const CIEC_ANY &);
  
  int encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src);

  void decodeServiceResp(uint8_t *pdu, const uint32_t &len);

protected:
  virtual void get(CIEC_ANY &){};
  
};

#endif

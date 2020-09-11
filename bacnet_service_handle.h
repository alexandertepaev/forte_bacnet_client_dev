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

class CBacnetServiceHandle : public forte::core::io::IOHandle, protected CExternalEventHandler
{
public:
  CBacnetServiceHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB);
  ~CBacnetServiceHandle();

  virtual int encodeServiceReq(uint8_t *pdu, const uint8_t &invoke_id, BACNET_ADDRESS *dest, BACNET_ADDRESS *src) = 0;
  virtual void decodeServiceResp(uint8_t *pdu, const uint32_t &len) = 0;
  void fireConfirmationEvent() {
   startNewEventChain(static_cast<forte::core::io::ProcessInterface *>(getObserver()));
  };
  void missedTransactionDeadline() {
    m_eHandleState = e_Idle;
  }

  void enableHandler(void) {};
  void disableHandler(void) { };
  void setPriority(int paPriority) { };
  int getPriority(void) const { return 0; };
  size_t getIdentifier() const { return 0; };

  CBacnetServiceConfigFB *mConfigFB;
  CIEC_ANY *mValue;

  enum EBacnetHandleState{
    e_Idle, e_AwaitingResponse 
  };
  EBacnetHandleState m_eHandleState;

  enum EBacnetHandleType{
    e_ReadPropertyServiceHandle,
    e_WritePropertyServiceHandle,
    e_UnconfirmedCOVServiceHandle 
  };
  EBacnetHandleType m_eHandleType;
};

#endif
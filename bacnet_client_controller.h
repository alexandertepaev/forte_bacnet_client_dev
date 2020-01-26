#ifndef _BACNET_CLIENT_CONTROLLER_H_
#define _BACNET_CLIENT_CONTROLLER_H_

// #include <devexec.h>
//#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller.h"
#include "../../forte-incubation_1.11.0/src/arch/utils/timespec_utils.h"

#include "bacnet_device_object.h"

#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include "bacnet_service_config_fb.h"
#include "BACnetReadProperty.h"
#include "BACnetWriteProperty.h"
#include "BACnetSubscribeUnconfirmedCOV.h"

#include "bacnet_readproperty_handle.h"
#include "bacnet_writeproperty_handle.h"
#include "bacnet_unconfirmed_cov_handle.h"


class CBacnetServiceHandle;

  class CBacnetClientController: public forte::core::io::IODeviceController {
  
  friend class CBacnetReadPropertyConfigFB;
  friend class CBacnetWritePropertyConfigFB;
  friend class CBacnetSubscribeUnconfirmedCOVConfigFB;
  friend class CBacnetServiceHandle;
  public:
    CBacnetClientController(CDeviceExecution& paDeviceExecution, int id);
    ~CBacnetClientController();

    void setConfig(Config* paConfig);
  

    // controller configuration
    struct SBacnetClientControllerConfig : forte::core::io::IODeviceController::Config {
      uint16_t nPortNumber;
      uint32_t nDeviceObjID;
      char *sDeviceObjName;
      char *sPathToAddrFile;
    };

    class HandleDescriptor : public forte::core::io::IODeviceController::HandleDescriptor {
      public:
        CBacnetServiceConfigFB *mServiceConfigFB;
        int mServiceType;

        HandleDescriptor(CIEC_WSTRING const &paId, forte::core::io::IOMapper::Direction paDirection, int paServiceType, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IODeviceController::HandleDescriptor(paId, paDirection), mServiceType(paServiceType), mServiceConfigFB(paServiceConfigFB) {
        }
    };

    const int& getSocket() {
      return mBacnetSocket;
    }

    bool pushToRingbuffer(CBacnetServiceHandle *handle);
    CBacnetServiceHandle * consumeFromRingbuffer();

    bool decodeNPDU(uint8_t *pdu, uint32_t &apdu_offset, uint32_t &apdu_len, uint8_t &apdu_type, uint8_t &service_choice);
    void handleAPDU(uint8_t *apdu, const uint32_t &apdu_len, const uint8_t &apdu_type, const uint8_t &service_choice);

    sockaddr_in getMyNetworkAddress();

    bool initNetworkAddresses();
    struct in_addr mLocalAddr;
    struct in_addr mBroadcastAddr;
    uint16_t mPort;

    bool addInvokeIDHandlePair(const uint8_t &paInvokeID, CBacnetServiceHandle *handle);

    bool removeInvokeIDHandlePair(const uint8_t &paInvokeID);

    void initDone(); // better naming?

  protected:
    const char* init(); // Initialize the device object (call it's init function)
    void deInit() {}; //TODO
    void runLoop();
    forte::core::io::IOHandle* initHandle(IODeviceController::HandleDescriptor *handleDescriptor);
  
  private:

    // controller instance id
    int m_nControllerID;

    // controller config
    struct SBacnetClientControllerConfig m_stConfig;
    
    // invoke id
    uint8_t invoke_id;
    uint8_t getNextInvokeID();

    //ringbuffer of outgoint messages (TODO change typedef from int to service handles)
    typedef CBacnetServiceHandle *TBacnetServiceHandlePtr;
    static const uint8_t cm_nSendRingbufferSize = 64;
    TBacnetServiceHandlePtr m_aSendRingbuffer[cm_nSendRingbufferSize];
    int m_nSendRingbufferHeadIndex;
    int m_nSendRingbufferTailIndex;
    int m_nSendRingbufferSize;
    //! SyncObject for protecting the buffer
    CSyncObject mSendRingbufferSync;

    //socket descriptor
    typedef int TSocketDescriptor;
    TSocketDescriptor mBacnetSocket;
    TSocketDescriptor openBacnetIPSocket();

    // reuests awaiting response
    typedef std::map<uint8_t, CBacnetServiceHandle *> TInvokeIDHandleMap;
    TInvokeIDHandleMap mInvokeIDsHandles; // TODO - better naiming

    //outgoing Transaction Buffer
    uint8_t mSendBuffer[MAX_APDU];
    //receive buffer
    uint8_t mReceiveBuffer[MAX_APDU];

    // states
    enum EBacnetClientControllerState {
      e_Init, 
      e_AddressFetch, //TODO remove
      e_AddressDiscovery, 
      e_COVSubscription, 
      e_Operating, //TODO remove
      e_ConfigFBsNotification,
      e_NormalOperation
    };
    EBacnetClientControllerState m_eClientControllerState;
    
    // cov subs list
    struct SBacnetCOVSubsListEntry {
      bool mSubscriptionAcknowledged;
      uint8_t mAssignedInvokeId;
      CBacnetServiceConfigFB *mServiceConfigFB;
    };
    typedef CSinglyLinkedList<SBacnetCOVSubsListEntry *> TBacnetCOVSubList;
    TBacnetCOVSubList *pmCOVSubscribers;
    void populateCOVSubscribers();

    void discoverNetworkAddresses();
    void sendWhoIs(uint32_t paDeviceId);
    void receiveIAm(uint32_t paDeviceId);
    void handleIAm(uint32_t paDeviceId, const struct sockaddr_in &src, uint8_t paAPDUOffset);
    bool timeoutMillis(uint16_t paMillis, timespec paStartTime);
    void notifyConfigFBs();

    void subscribeToCOVNotifications();
    void sendUnconfirmedCOVSubscribe(SBacnetCOVSubsListEntry *paCOVSubsEntry);
    void receiveCOVSubscriptionAck(SBacnetCOVSubsListEntry *paCOVSubsEntry);
    void handleCOVSubscriptionAck(SBacnetCOVSubsListEntry *paCOVSubsEntry, uint8_t paAPDUOffset);

    void executeOperationCycle();
    void encodeRequest(CBacnetServiceHandle *paHandle, uint8_t &paInvokeId, uint16_t &paRequestLen, in_addr &paDestAddr, uint16_t &paDestPort);
    void handleReceivedPacket();

    // addr list
    struct SBacnetAddressListEntry {
      bool mAddrInitFlag;
      uint32_t mDeviceId;
      struct in_addr mAddr;
      uint16_t mPort;
    };
    typedef CSinglyLinkedList<SBacnetAddressListEntry *> TBacnetAddrList;
    TBacnetAddrList *pmAddrList;

    typedef CSinglyLinkedList<CBacnetServiceConfigFB *> TServiceConfigFBsList;
    TServiceConfigFBsList *pmServiceConfigFBsList;
    void updateSCFBsList(CBacnetServiceConfigFB *paConfigFB);

    void handleUnconfirmedCOVNotifation(uint8_t *apdu, const uint32_t &apdu_len);

    int sendPacket(uint16_t len, struct in_addr dest_addr, uint16_t dest_port);
    int receivePacket(uint16_t timeout, sockaddr_in *src);
    bool getAddressByDeviceId(uint32_t paDeviceId, struct in_addr *paDeviceAddr, uint16_t *paDeviceAddrPort);
    BACNET_ADDRESS ipToBacnetAddress(struct in_addr paDeviceAddr, uint16_t paPort, bool paBroadcastAddr);

    void populateAddrTable();
    bool checkAddrTableForDuplicate(uint32_t device_id);

    
};
#endif
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

//class CBacnetClientController: public forte::core::io::IODeviceMultiController {
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
    

    // class HandleDescriptor : public forte::core::io::IODeviceMultiController::HandleDescriptor {
    //   public:
    //     CBacnetServiceConfigFB *mServiceConfigFB;
    //     int mServiceType;

    //     HandleDescriptor(CIEC_WSTRING const &paId, forte::core::io::IOMapper::Direction paDirection, int paSlaveIndex, int paServiceType, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IODeviceMultiController::HandleDescriptor(paId, paDirection, paSlaveIndex), mServiceType(paServiceType), mServiceConfigFB(paServiceConfigFB) {
    //     }
    // };

    class HandleDescriptor : public forte::core::io::IODeviceController::HandleDescriptor {
      public:
        CBacnetServiceConfigFB *mServiceConfigFB;
        int mServiceType;

        HandleDescriptor(CIEC_WSTRING const &paId, forte::core::io::IOMapper::Direction paDirection, int paServiceType, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IODeviceController::HandleDescriptor(paId, paDirection), mServiceType(paServiceType), mServiceConfigFB(paServiceConfigFB) {
        }
    };

    // functions like - service handles (eg. ReadProperty - has to be supported - iterate through the object list and read the property)
    //                - sending functions (sending functions have to be implemented in CBacnetServiceHandle)
    //

    const int& getSocket() {
      return mBacnetSocket;
    }

    bool pushToRingbuffer(CBacnetServiceHandle *handle);
    CBacnetServiceHandle * consumeFromRingbuffer();

    void decodeBacnetPacket(uint8_t *pdu, uint16_t len);

    /*void decodeBacnetBVLC(uint8_t *pdu, uint16_t len, sockaddr_in *src);
    int decodeBacnetBVLC_(uint8_t *pdu, uint16_t &npdu_len);
    int decodeBacnetAPDU_(uint8_t *pdu, uint16_t &apdu_len, uint8_t &apdu_type);*/

    void buildPacketAndSend(CBacnetServiceHandle *handle);
    void receiveAndHandle();
    bool decodeNPDU(uint8_t *pdu, uint32_t &apdu_offset, uint32_t &apdu_len, uint8_t &apdu_type, uint8_t &service_choice);
    void handleAPDU(uint8_t *apdu, const uint32_t &apdu_len, const uint8_t &apdu_type, const uint8_t &service_choice, const struct sockaddr_in &src);
    void handleRPAck(uint8_t *apdu, const uint32_t &apdu_len);
    void handleWPAck(uint8_t *apdu, const uint32_t &apdu_len);

    void buildWhoIsAndSend(uint32_t device_id, uint8_t *buffer);
    void decodeAndHandleIAm(uint8_t *apdu, const uint32_t &apdu_len, const struct sockaddr_in &src);

    sockaddr_in getMyNetworkAddress();
    sockaddr_in mMyNetworkAddress;

    bool initNetworkAddresses();
    struct in_addr mLocalAddr;
    struct in_addr mBroadcastAddr;
    uint16_t mPort;

    bool addInvokeIDHandlePair(const uint8_t &paInvokeID, CBacnetServiceHandle *handle);

    bool removeInvokeIDHandlePair(const uint8_t &paInvokeID);

    void initDone(); // better naming?

    // enum EServiceConfigFBNotificationType {
    //   e_UnknownNotificationType,
    //   e_Success,
    //   e_AddrFetchFailed,
    //   e_COVSubscriptionFailed
    // };
    // EServiceConfigFBNotificationType mServiceConfigFBNotificationType;


  protected:
    const char* init(); // Initialize the device object (call it's init function)
    void deInit() {}; //TODO
    void runLoop();
    forte::core::io::IOHandle* initHandle(IODeviceController::HandleDescriptor *handleDescriptor);
    bool checkSlaveType(int index, int type) {}; //TODO;
  
  private:

    
    // const char* mServiceConfigFBNotification;
    // static const char* const scmAddrFetchFailed;
    // static const char* const scmCOVSubscriptionFailed;
    // static const char* const scmOK;

    bool isSlaveAvailable(int index) {}; //TODO;

    // controller instance id
    int m_nControllerID;

    // controller config
    struct SBacnetClientControllerConfig m_stConfig;

     // list of objects
    typedef CSinglyLinkedList<CBacnetObject *> TBacnetObjectList;
    TBacnetObjectList *pmObjectList;

     // list of service handles (TODO change to proper type)
    typedef CSinglyLinkedList<int *> TBacnetServiceHandleList;
    TBacnetServiceHandleList *pmServiceList;

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

    // states
    enum EBacnetClientControllerState {
      e_Init, e_AddressFetch, e_COVSubscription, e_Operating 
    };
    EBacnetClientControllerState m_eClientControllerState;

    // addr list
    struct SBacnetAddressListEntry {
      bool mAddrInitFlag;
      uint32_t mDeviceId;
      struct in_addr mAddr;
      uint16_t mPort;
    };
    typedef CSinglyLinkedList<SBacnetAddressListEntry *> TBacnetAddrList;
    TBacnetAddrList *pmAddrList;
    int encodeWhoIs(uint32_t device_id, uint8_t *buffer);

    // cov subs list
    // struct SBacnetCOVSubsListEntry {
    //   bool mSubscriptionAcknowledged;
    //   uint8_t mAssignedInvokeId;
    //   CBacnetServiceHandle *mHandle;
    //   CBacnetServiceConfigFB::ServiceConfig *mSubscriptionConfig;
    // };
    // typedef CSinglyLinkedList<SBacnetCOVSubsListEntry *> TBacnetCOVSubList;
    // TBacnetCOVSubList *pmCOVSubList;

    typedef CSinglyLinkedList<CBacnetServiceConfigFB *> TServiceConfigFBsList;
    TServiceConfigFBsList *pmServiceConfigFBsList;
    void updateSCFBsList(CBacnetServiceConfigFB *paConfigFB);

    void buildSubscribeCOVAndSend(CBacnetServiceConfigFB::ServiceConfig *paConfig, uint8_t *buffer, uint8_t &paAssignedInvokeId);
    void handleCOVSubscriptionAck(uint8_t *apdu, const uint32_t &apdu_len);
    void handleUnconfirmedCOVNotifation(uint8_t *apdu, const uint32_t &apdu_len);

    int sendPacket(uint8_t *buffer, uint16_t len, struct in_addr dest_addr, uint16_t dest_port);
    int receivePacket(uint8_t *buffer, size_t buffer_size, uint16_t timeout, sockaddr_in &src);
    bool getAddressByDeviceId(uint32_t paDeviceId, struct in_addr &paDeviceAddr, uint16_t &paDeviceAddrPort);
    BACNET_ADDRESS ipToBacnetAddress(struct in_addr paDeviceAddr, uint16_t paPort, bool paBroadcastAddr);

    void populateAddrTable();
    bool checkAddrTableForDuplicate(uint32_t device_id);
    // cov subs list
    struct SBacnetCOVSubsListEntry {
      bool mSubscriptionAcknowledged;
      uint8_t mAssignedInvokeId;
      CBacnetServiceConfigFB *mServiceConfigFB;
    };
    typedef CSinglyLinkedList<SBacnetCOVSubsListEntry *> TBacnetCOVSubList;
    TBacnetCOVSubList *pmCOVSubscribers;
    void populateCOVSubscribers();

    
};
#endif
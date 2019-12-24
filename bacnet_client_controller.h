#ifndef _BACNET_CLIENT_CONTROLLER_H_
#define _BACNET_CLIENT_CONTROLLER_H_

// #include <devexec.h>
#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller_multi.h"
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

#include "bacnet_readproperty_handle.h"

class CBacnetServiceHandle;

class CBacnetClientController: public forte::core::io::IODeviceMultiController {
  friend class CBacnetReadPropertyConfigFB;
  friend class CBacnetServiceHandle;
  public:
    CBacnetClientController(CDeviceExecution& paDeviceExecution, int id);
    ~CBacnetClientController();

    void setConfig(Config* paConfig);
    void addSlaveHandle(int index, forte::core::io::IOHandle* handle);
    void dropSlaveHandles(int index) {}; //TODO;
  

    // controller configuration
    struct SBacnetClientControllerConfig : forte::core::io::IODeviceController::Config {
      uint16_t nPortNumber;
      uint32_t nDeviceObjID;
      char *sDeviceObjName;
      char *sPathToAddrFile;
    };
    

    class HandleDescriptor : public forte::core::io::IODeviceMultiController::HandleDescriptor {
      public:
        CBacnetServiceConfigFB *mServiceConfigFB;
        int mServiceType;

        HandleDescriptor(CIEC_WSTRING const &paId, forte::core::io::IOMapper::Direction paDirection, int paSlaveIndex, int paServiceType, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IODeviceMultiController::HandleDescriptor(paId, paDirection, paSlaveIndex), mServiceType(paServiceType), mServiceConfigFB(paServiceConfigFB) {
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

    void decodeBacnetBVLC(uint8_t *pdu, uint16_t len, sockaddr_in *src);

    sockaddr_in getMyNetworkAddress();
    sockaddr_in mMyNetworkAddress;

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
    bool checkSlaveType(int index, int type) {}; //TODO;
  
  private:
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
    EBacnetClientControllerState m_eClienControllerState;

    // addr list
    struct SBacnetAddressListEntry {
      bool mAddrInitFlag;
      uint32_t mDeviceId;
      sockaddr_in addr;
    };

    typedef CSinglyLinkedList<SBacnetAddressListEntry *> TBacnetAddrList;

    TBacnetAddrList *pmAddrList;

    bool addAddrListEntry(uint32_t device_id);

    bool encodeWhoIs(uint32_t device_id, uint8_t *buffer);

    
};
#endif
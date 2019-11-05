#include "bacnet_client_controller.h"

CBacnetClientController::CBacnetClientController(CDeviceExecution& paDeviceExecution, int id) : forte::core::io::IODeviceMultiController(paDeviceExecution), m_nControllerID(id), m_nSendRingbufferHeadIndex(0), m_nSendRingbufferSize(0), m_nSendRingbufferTailIndex(0), pmObjectList(0), pmServiceList(0), mBacnetSocket(0)
{
  memset(m_aSendRingbuffer, 0, cm_nSendRingbufferSize * sizeof(TBacnetServiceHandlePtr));
  
  m_stConfig.nPortNumber = 0;
  m_stConfig.nDeviceObjID = 0;
  m_stConfig.sDeviceObjName = NULL;
  m_stConfig.sPathToAddrFile = NULL;

}

CBacnetClientController::~CBacnetClientController()
{
}

void CBacnetClientController::setConfig(Config* paConfig) {
  m_stConfig = *static_cast<SBacnetClientControllerConfig *>(paConfig);
  //TODO - addresses (either parse address file or do nothing) addresses - list of "address entry" objects

  // DEVLOG_DEBUG("[CBacnetClientController] CliCtrl config - Port=%d DeviceObjectID=%d DeviceObjectName=%s PathToAddrFile=%s\n", m_nPortNumber, mDeviceObjectDecsriptor->objectId, mDeviceObjectDecsriptor->objectName, cfg->sPathToAddrFile);
}

// Initialie client controller
// - initialize object list
// - initialize device object and push it to the list 
// - parse addr file and create address bindings
// - initialize ringbuffer of outgoing messages
// - ...
// - open socket
//
const char* CBacnetClientController::init() {
  // object list
  pmObjectList = new TBacnetObjectList();
  pmObjectList->pushBack(new CBacnetDeviceObject(m_stConfig.nDeviceObjID, m_stConfig.sDeviceObjName));
  // service list
  pmServiceList = new TBacnetServiceHandleList();
  // ringbuffer
  memset(m_aSendRingbuffer, 0, cm_nSendRingbufferSize * sizeof(TBacnetServiceHandlePtr));
  m_nSendRingbufferHeadIndex = m_nSendRingbufferTailIndex = m_nSendRingbufferSize = 0; 
  // open socket
  mBacnetSocket = openBacnetIPSocket();
  return 0;
}

CBacnetClientController::TSocketDescriptor CBacnetClientController::openBacnetIPSocket() {
  //get my address - why do we need it?
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr)); 
  strncpy(ifr.ifr_name, "wlp4s0", sizeof(ifr.ifr_name));
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  int rv = ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);
  struct sockaddr_in *my_addr = (struct sockaddr_in *)&ifr.ifr_addr;

  //open socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd <= 0) {
    DEVLOG_DEBUG("[CBacnetClientController] Failed to open socket\n");
    return -1;
  }
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(m_stConfig.nPortNumber);

  //bind socket
  if( bind(sockfd, (const sockaddr *)&address, sizeof(sockaddr_in)) < 0 ){
    DEVLOG_DEBUG("[CBacnetClientController] Failed to bind socket\n");
    return -1;
  } else {
    my_addr->sin_port = address.sin_port;
    DEVLOG_DEBUG("[CBacnetClientController] Opened BACnet client UDP socket %s:%04X\n", inet_ntoa(my_addr->sin_addr), ntohs(my_addr->sin_port));
    return sockfd;
  }
}

void CBacnetClientController::runLoop() {
  DEVLOG_DEBUG("[CBacnetClientController] runLoop(): Starting controller loop\n");
  while(isAlive()) {
    if(m_nSendRingbufferSize != 0) {
      CBacnetServiceHandle *handle = consumeFromRingbuffer();
      uint8_t pdu[MAX_MPDU];
      BACNET_ADDRESS dst;
      dst.mac_len = 6;
      dst.mac[0] = 192;
      dst.mac[1] = 168;
      dst.mac[2] = 1;
      dst.mac[3] = 1;
      dst.mac[4] = 0xBA;
      dst.mac[5] = 0xC0;
      dst.net = 0;
      dst.len = 0;

      BACNET_ADDRESS my_adr;
      my_adr.mac_len = 6;
      my_adr.mac[0] = 192;
      my_adr.mac[1] = 168;
      my_adr.mac[2] = 1;
      my_adr.mac[3] = 0;
      my_adr.mac[4] = 0xBA;
      my_adr.mac[5] = 0xC0;
      my_adr.net = 0;
      my_adr.len = 0;

      int pdu_len = handle->encodeServiceReq(pdu, 1, &my_adr, &dst);

      for(int i = 0; i<pdu_len; i++){
        printf("%02x ", pdu[i]);
      }
      printf("\n");

      struct in_addr address;
      uint16_t port = 0;
      memcpy(&address.s_addr, &dst.mac[0], 4);
      memcpy(&port, &dst.mac[4], 2);

      struct sockaddr_in bvlc_dest;
      bvlc_dest.sin_family = AF_INET;
      bvlc_dest.sin_addr.s_addr = address.s_addr;
      bvlc_dest.sin_port = port;
  
      sendto(mBacnetSocket, (char *) pdu, pdu_len, 0, (struct sockaddr *) &bvlc_dest, sizeof(struct sockaddr));


    }
  }
}

 forte::core::io::IOHandle* CBacnetClientController::initHandle(IODeviceController::HandleDescriptor *handleDescriptor) {

  HandleDescriptor *desc = static_cast<CBacnetClientController::HandleDescriptor *>(handleDescriptor);
  switch (desc->mServiceType)
  {
    case SERVICE_CONFIRMED_READ_PROPERTY:
      return new CBacnetReadPropertyHandle(this, desc->mDirection, CIEC_ANY::e_DWORD, mDeviceExecution, desc->mServiceConfigFB); // Question: is it better to compose a pdu one single time here and store its value or is it better to compose it every time we want to send the request
      break;
    default:
      DEVLOG_DEBUG("[CBacnetClientController] initHandle(): Unknown/Unsupported BACnet Service\n");
      return 0;
      break;
  }
 }


void CBacnetClientController::addSlaveHandle(int index, forte::core::io::IOHandle* handle){
  DEVLOG_DEBUG("[CBacnetClientController] addSlaveHandle(): Registering handle to controller\n");
  // CBacnetServiceHandle *bacnet_handle = static_cast<CBacnetServiceHandle *>(handle);
  // uint8_t pdu[MAX_MPDU];

  // int pdu_len = bacnet_handle->encodeServiceReq(pdu, 1);
  // printf("%d ----- \n", pdu_len);

  // for(int i = 0; i<pdu_len-1; i++){
  //   printf("%02x ", pdu[i]);
  // }
  // printf("\n");
}

bool CBacnetClientController::pushToRingbuffer(CBacnetServiceHandle *handle) {
  if(m_nSendRingbufferSize == cm_nSendRingbufferSize) {
    return false;
  }
  m_aSendRingbuffer[m_nSendRingbufferHeadIndex] = handle;
  m_nSendRingbufferSize++;
  m_nSendRingbufferHeadIndex = (m_nSendRingbufferHeadIndex + 1) % cm_nSendRingbufferSize;

  DEVLOG_DEBUG("Pushed to ringbuffer: head: %d tail: %d size: %d\n", m_nSendRingbufferHeadIndex, m_nSendRingbufferTailIndex, m_nSendRingbufferSize);
  return true;
}

CBacnetServiceHandle * CBacnetClientController::consumeFromRingbuffer() {
  if(m_nSendRingbufferSize == 0) {
    return NULL;
  }
  CBacnetServiceHandle *ret = m_aSendRingbuffer[m_nSendRingbufferTailIndex];
  m_nSendRingbufferSize--;
  m_nSendRingbufferTailIndex = (m_nSendRingbufferTailIndex + 1) % cm_nSendRingbufferSize; 
  DEVLOG_DEBUG("Consumed from ringbuffer: head: %d tail: %d size: %d\n", m_nSendRingbufferHeadIndex, m_nSendRingbufferTailIndex, m_nSendRingbufferSize);
  return ret;
}



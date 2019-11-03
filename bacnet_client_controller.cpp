#include "bacnet_client_controller.h"

CBacnetClientController::CBacnetClientController(CDeviceExecution& paDeviceExecution, int id) : forte::core::io::IODeviceMultiController(paDeviceExecution), m_nControllerID(id), pmObjectList(0), m_nSendRingbufferStartIndex(0), m_nSendRingbufferEndIndex(0), pmServiceList(0), mBacnetSocket(0)
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
  m_nSendRingbufferStartIndex = m_nSendRingbufferEndIndex = 0; 
  // open socket
  //mBacnetSocket = openBacnetIPSocket();
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
    
  }
}

 forte::core::io::IOHandle* CBacnetClientController::initHandle(IODeviceController::HandleDescriptor *handleDescriptor) {

  HandleDescriptor *desc = static_cast<CBacnetClientController::HandleDescriptor *>(handleDescriptor);
  switch (desc->mServiceType)
  {
    case SERVICE_CONFIRMED_READ_PROPERTY:
      return new CBacnetReadPropertyHandle(this, desc->mDirection, CIEC_ANY::e_WORD, mDeviceExecution, desc->mServiceConfigFB); // Question: is it better to compose a pdu one single time here and store its value or is it better to compose it every time we want to send the request
      break;
    default:
      DEVLOG_DEBUG("[CBacnetClientController] initHandle(): Unknown/Unsupported BACnet Service\n");
      return 0;
      break;
  }
 }


void CBacnetClientController::addSlaveHandle(int index, forte::core::io::IOHandle* handle){
   DEVLOG_DEBUG("[CBacnetClientController] addSlaveHandle(): Registering handle to controller\n");
   
}

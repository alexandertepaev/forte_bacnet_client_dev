#include "bacnet_client_controller.h"

CBacnetClientController::CBacnetClientController(CDeviceExecution& paDeviceExecution, int id) : forte::core::io::IODeviceMultiController(paDeviceExecution), m_nControllerID(id), m_nSendRingbufferHeadIndex(0), m_nSendRingbufferSize(0), m_nSendRingbufferTailIndex(0), pmObjectList(0), pmServiceList(0), mBacnetSocket(0), m_eClienControllerState(e_Init), pmAddrList(0)
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
  DEVLOG_DEBUG("[CBacnetClientController] setConfig(): Client controller's configuration parameters set\n");
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
  //addr list
  pmAddrList = new TBacnetAddrList();
  
  // // get my address
  // mMyNetworkAddress = getMyNetworkAddress();
  // // get broadcast address
  // mBroadcastAddress = getBroadcastNetworkAddress();

  // init addresses
  initNetworkAddresses();

  // open socket
  mBacnetSocket = openBacnetIPSocket();
  
  // DEVLOG_DEBUG("[CBacnetClientController] init(): Opened BACnet client UDP socket %s:%04X, BroadcastAddr: %s\n", inet_ntoa(mMyNetworkAddress.sin_addr), ntohs(mMyNetworkAddress.sin_port), inet_ntoa(mBroadcastAddress.sin_addr));
  DEVLOG_DEBUG("[CBacnetClientController] init(): Opened BACnet client UDP socket %s:%04X\n", inet_ntoa(mLocalAddr), mPort);

  address_init(); // TODO - maybe not needed since we are using Who-Is/I-Am services
  
  return 0;
}

#define NETWORK_IFACE_NAME "enp0s25"
sockaddr_in CBacnetClientController::getMyNetworkAddress() {
  sockaddr_in my_addr = {};
  ifreq ifr = {};
  strncpy(ifr.ifr_name, NETWORK_IFACE_NAME, sizeof(ifr.ifr_name));
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  int rv = ioctl(fd, SIOCGIFADDR, &ifr); // TODO
  close(fd);
  my_addr = *((struct sockaddr_in *)&ifr.ifr_addr);
  my_addr.sin_port = htons(m_stConfig.nPortNumber);
  return my_addr;
}

bool CBacnetClientController::initNetworkAddresses() {
  ifreq ifr = {};
  struct sockaddr_in ip_address;
  int rv, fd;
  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  strncpy(ifr.ifr_name, NETWORK_IFACE_NAME, sizeof(ifr.ifr_name));
  if(fd < 0){
    //error
    DEVLOG_DEBUG("error 1\n");
  } else {
    // local address
    rv = ioctl(fd, SIOCGIFADDR, &ifr);
    ip_address = *((struct sockaddr_in *) &ifr.ifr_addr);
    mLocalAddr = ip_address.sin_addr;
    mPort = m_stConfig.nPortNumber;
    DEVLOG_DEBUG("[CBacnetClientController] initNetworkAddresses() Local Address: %s:%04X\n", inet_ntoa(mLocalAddr), mPort);

    // netmask
    rv = ioctl(fd, SIOCGIFNETMASK, &ifr);
    ip_address = *((struct sockaddr_in *) &ifr.ifr_addr);
    DEVLOG_DEBUG("[CBacnetClientController] initNetworkAddresses() Netmask: %s\n", inet_ntoa(ip_address.sin_addr));
    close(fd);

    // broadcast
    mBroadcastAddr.s_addr = mLocalAddr.s_addr | (~ip_address.sin_addr.s_addr);
    DEVLOG_DEBUG("[CBacnetClientController] initNetworkAddresses() Broadcast Address: %s\n", inet_ntoa(mBroadcastAddr));
    
  }
}


CBacnetClientController::TSocketDescriptor CBacnetClientController::openBacnetIPSocket() {
  //open socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd <= 0) {
    DEVLOG_DEBUG("[CBacnetClientController] Failed to open socket\n");
    return -1;
  }
  // allow reuse addr:port combo
  int sockopt = 1;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
    DEVLOG_DEBUG("[CBacnetClientController] Setsockopt failed: reuse\n");
  }
  // allow broadcast msgs
  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt)) < 0) {
    DEVLOG_DEBUG("[CBacnetClientController] Setsockopt failed: broadcast\n");
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
    return sockfd;
  }
}

void CBacnetClientController::runLoop() {
  DEVLOG_DEBUG("[CBacnetClientController] runLoop(): Starting controller loop\n");
  while(isAlive()) {

    if(m_eClienControllerState == e_AddressFetch) {
      TBacnetAddrList::Iterator itEnd = pmAddrList->end();
      for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it){
        //send
        DEVLOG_DEBUG("[CBacnetClientController] Sending WHO-IS to DeviceID %d\n", (*it)->mDeviceId);
        uint8_t pdu[MAX_MPDU];
        uint16_t len = encodeWhoIs((*it)->mDeviceId, pdu);
        struct sockaddr_in bvlc_dest = { 0 };
        bvlc_dest.sin_family = AF_INET;
        bvlc_dest.sin_addr.s_addr = mBroadcastAddr.s_addr;
        bvlc_dest.sin_port = htons(mPort);
        memset(bvlc_dest.sin_zero, 0, 8);
        int send_len = sendto(mBacnetSocket, (char *) pdu, len, 0, (struct sockaddr *) &bvlc_dest, sizeof(struct sockaddr));
        
        if(send_len > 0) { 
          DEVLOG_DEBUG("[CBacnetClientController] Sent WHO-IS packet of length %d: ", send_len);
          for(int i = 0; i<len; i++){
              printf("%02X ", pdu[i]);
            }
          printf("\n");
        } else {
          DEVLOG_DEBUG("[CBacnetClientController] Sending WHO-IS failed\n");
        }

        //receive, but ingnore my messages
        while(true) {
          struct timeval select_timeout;
          select_timeout.tv_sec = 0;
          select_timeout.tv_usec = 1000 * 100; // 100 ms timeout on receive - non-blocking recv
          fd_set read_fds;
          FD_ZERO(&read_fds);
          FD_SET(mBacnetSocket, &read_fds);
          if(select(mBacnetSocket+1, &read_fds, NULL, NULL, &select_timeout) > 0){
            uint8_t npdu[MAX_PDU];
            sockaddr_in src;
            socklen_t srcLen = sizeof(src);
            int rcv_retval = recvfrom(mBacnetSocket, npdu, sizeof(npdu), 0, (sockaddr *) &src, &srcLen);
            if(rcv_retval > 0) {
              if((src.sin_addr.s_addr != mLocalAddr.s_addr || htons(src.sin_port) != mPort)){
                DEVLOG_DEBUG("[CBacnetClientController] Received packet of length %d: ", rcv_retval);
                for(int i=0; i<rcv_retval; i++){
                  printf("%02X ", npdu[i]);
                }
                printf("\n");
                //DEVLOG_DEBUG("From %s:%04X\n", inet_ntoa(src.sin_addr), src.sin_port);
                // TODO - decode packet here
                decodeBacnetBVLC(npdu, rcv_retval, &src);
                break;
              }
            } else {
              DEVLOG_DEBUG("[CBacnetClientController] recvfrom() failed\n");
              break;
            }
          } else {
            DEVLOG_DEBUG("[CBacnetClientController] select() timeout\n");
            break;
          }
        }

      }


      // FIXME -- skip state
      m_eClienControllerState = e_COVSubscription;      

    } else if (m_eClienControllerState == e_COVSubscription) {

      // FIXME -- skip state
      //m_eClienControllerState = e_Operating;  

    } else if (m_eClienControllerState == e_Operating) {
      // sending from ringbuffer
      if(m_nSendRingbufferSize != 0) {
        CBacnetServiceHandle *handle = consumeFromRingbuffer();
        uint8_t pdu[MAX_MPDU];
        BACNET_ADDRESS dst;
        dst.mac_len = 6;
        dst.mac[0] = 169;
        dst.mac[1] = 254;
        dst.mac[2] = 95;
        dst.mac[3] = 92;
        dst.mac[4] = 0xBA;
        dst.mac[5] = 0xC0;
        dst.net = 0;
        dst.len = 0;

        BACNET_ADDRESS my_adr;
        my_adr.mac_len = 6;
        my_adr.mac[0] = 169;
        my_adr.mac[1] = 254;
        my_adr.mac[2] = 95;
        my_adr.mac[3] = 91;
        my_adr.mac[4] = 0xBA;
        my_adr.mac[5] = 0xC0;
        my_adr.net = 0;
        my_adr.len = 0;

        uint8_t invoke_id = 1; // TODO get free invoke id

        //int pdu_len = handle->encodeServiceReq(pdu, invoke_id, &my_adr, &dst);
        int pdu_len = handle->encodeServiceReq(pdu, invoke_id);

        struct sockaddr_in bvlc_dest;
        bvlc_dest.sin_family = AF_INET;
        memcpy(&bvlc_dest.sin_addr.s_addr, &dst.mac[0], 4);
        memcpy(&bvlc_dest.sin_port, &dst.mac[4], 2);
        memset(bvlc_dest.sin_zero, 0, 8);
        
        int send_len = sendto(mBacnetSocket, (char *) pdu, pdu_len, 0, (struct sockaddr *) &bvlc_dest, sizeof(struct sockaddr));
        if(send_len > 0){
          DEVLOG_DEBUG("[CBacnetClientController] Sent packet of length %d: ", send_len);
          for(int i = 0; i<pdu_len; i++){
            printf("%02X ", pdu[i]);
          }
          printf("\n");

          addInvokeIDHandlePair(invoke_id, handle);
        } else {
          DEVLOG_DEBUG("[CBacnetClientController] sendto() failed");
        }
      }


      // receiving response or COV reports
      struct timeval select_timeout;
      select_timeout.tv_sec = 0;
      select_timeout.tv_usec = 1000 * 100; // 100 ms timeout on receive - non-blocking recv
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(mBacnetSocket, &read_fds);
      if(select(mBacnetSocket+1, &read_fds, NULL, NULL, &select_timeout) > 0){
        uint8_t npdu[MAX_PDU];
        sockaddr_in src;
        socklen_t srcLen = sizeof(src);
        int rcv_retval = recvfrom(mBacnetSocket, npdu, sizeof(npdu), 0, (sockaddr *) &src, &srcLen);
        if(rcv_retval > 0) {
          DEVLOG_DEBUG("[CBacnetClientController] Received packet of length %d: ", rcv_retval);
          for(int i=0; i<rcv_retval; i++){
            printf("%02X ", npdu[i]);
          }
          printf("\n");
          decodeBacnetBVLC(npdu, rcv_retval, &src);
          //decodeBacnetPacket(npdu, rcv_retval);
        } else  {
          DEVLOG_DEBUG("[CBacnetClientController] recvfrom() failed\n");
        }
      }
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


void CBacnetClientController::decodeBacnetBVLC(uint8_t *pdu, uint16_t len, sockaddr_in *src) {
  if (pdu[0] != BVLL_TYPE_BACNET_IP) {
    return;
  }

  uint16_t npdu_len = 0;
  // decode the length of the pdu (inclusive BVLC)
  (void) decode_unsigned16(&pdu[2], &npdu_len);
  // packet contains only BVLC or too large
  if ((npdu_len < 4) || (npdu_len > (MAX_MPDU-4))) {
        return;
  }

  // substract the BVLC header length
  npdu_len -= 4;

  //decide depending on the function code of the packet
  switch (pdu[1]) {
     // see bvlc.c - bvlc_receive() for other function codes
    case BVLC_ORIGINAL_UNICAST_NPDU:
      if(pdu[4] == BACNET_PROTOCOL_VERSION) {
        // do not process packets from me
        // if(src->sin_addr.s_addr != mMyNetworkAddress.sin_addr.s_addr || 
        //    src->sin_port != mMyNetworkAddress.sin_port) {
        if(src->sin_addr.s_addr != mLocalAddr.s_addr || 
           src->sin_port != mPort) {

          // uint8_t invoke_id = pdu[7];
          // DEVLOG_DEBUG("[CBacnetClientController] decodeBacnetBVLC(): Received supported BACnet Original Unicast NPDU from %s:%04X, InvokeID: %d\n", inet_ntoa(src->sin_addr), ntohs(src->sin_port), invoke_id);

          BACNET_ADDRESS bacnet_dest = {};
          BACNET_ADDRESS bacnet_src = {};
          BACNET_NPDU_DATA npdu_data = {};

          int apdu_offset = npdu_decode(pdu, &bacnet_dest, &bacnet_src, &npdu_data);
          // there is an aplication pdu and it is of the proper length
          if ((apdu_offset > 0) && (apdu_offset <= npdu_len)) {
            // message to our network or broadcast message
            switch(pdu[apdu_offset] & 0xF0) {
              case PDU_TYPE_COMPLEX_ACK:
                BACNET_CONFIRMED_SERVICE_ACK_DATA service_ack_data = {};
                service_ack_data.segmented_message = (pdu[apdu_offset++] & 1<<3) ? true : false;
                service_ack_data.invoke_id = pdu[apdu_offset++];
                // for now support only unsegmented messages -- see apdu.c lines 538-545 for segmented messages
                if (!service_ack_data.segmented_message) {
                  DEVLOG_DEBUG("[CBacnetClientController] decodeBacnetBVLC(): Received supported BACnet Original Unicast NPDU from %s:%04X, InvokeID: %d\n", inet_ntoa(src->sin_addr), ntohs(src->sin_port), service_ack_data.invoke_id);

                  // call handles decode function
                  if(mInvokeIDsHandles.find(service_ack_data.invoke_id) != mInvokeIDsHandles.end()) {
                    DEVLOG_DEBUG("Found entry: npdu_len=%d, apdu_offset=%d\n", npdu_len, apdu_offset);
                    // apdu_offset+2: do not pass service_ack_data + invoke_id, len 
                    mInvokeIDsHandles[service_ack_data.invoke_id]->decodeServiceResp(&pdu[apdu_offset], npdu_len-(apdu_offset-4));
                    removeInvokeIDHandlePair(service_ack_data.invoke_id);
                  }
                
                }
              break;
            }
          }
        } else {
          DEVLOG_DEBUG("Message from me - ignoring\n");
        }
      }
      break;

    case BVLC_ORIGINAL_BROADCAST_NPDU:
      DEVLOG_DEBUG("Received broadcast PDU\n");
      break;

        

  }

}

bool CBacnetClientController::addInvokeIDHandlePair(const uint8_t &paInvokeID, CBacnetServiceHandle *paHandle) {
  // Check for duplicates
  if(mInvokeIDsHandles.find(paInvokeID) != mInvokeIDsHandles.end()) {
    DEVLOG_WARNING("[CBacnetClientController] addInvokeIDHandlePair(): Duplicated mInvokeIDsHandles entry '%d'\n", paInvokeID);
    return false;
  }

  mInvokeIDsHandles.insert(std::make_pair(paInvokeID, paHandle));
  DEVLOG_WARNING("[CBacnetClientController] addInvokeIDHandlePair(): New mInvokeIDsHandles entry '%d', mInvokeIDsHandles's size=%d\n", paInvokeID, mInvokeIDsHandles.size());

  return true;
}

bool CBacnetClientController::removeInvokeIDHandlePair(const uint8_t &paInvokeID) {
  for(TInvokeIDHandleMap::iterator it = mInvokeIDsHandles.begin(); it != mInvokeIDsHandles.end(); ++it) {
    if(it->first == paInvokeID) {
      mInvokeIDsHandles.erase(it);
      DEVLOG_WARNING("[CBacnetClientController] removeInvokeIDHandlePair(): Erased mInvokeIDsHandles's entry '%d', mInvokeIDsHandles's size=%d\n", paInvokeID, mInvokeIDsHandles.size());
      return true;
    }
  }
  return false;
}

void CBacnetClientController::initDone() {
  if(m_eClienControllerState == e_Init) {
    DEVLOG_DEBUG("[CBacnetClientController] initDone() - changing state from init to addr fetch\n");
    m_eClienControllerState = e_AddressFetch;
  }
}

bool CBacnetClientController::addAddrListEntry(uint32_t device_id) {
  // check if entry already exists
  TBacnetAddrList::Iterator itEnd = pmAddrList->end();
  for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it){
    if((*it)->mDeviceId == device_id) {
      DEVLOG_DEBUG("[CBacnetClientController] addAddrListEntry() -- found duplicated entry, ignoring add\n");
      return true;
    }
  }

  SBacnetAddressListEntry *newElement = new SBacnetAddressListEntry();
  newElement->mDeviceId = device_id;
  newElement->mAddrInitFlag = false;
  memset(&newElement->addr, 0, sizeof(struct in_addr));
  pmAddrList->pushBack(newElement);

  DEVLOG_DEBUG("[CBacnetClientController] addAddrListEntry() -- added new entry\n");
  return true;
}

int CBacnetClientController::encodeWhoIs(uint32_t device_id, uint8_t *buffer){

  //get broadcast addr
  BACNET_ADDRESS broadcast_addr;
  broadcast_addr.mac_len = 6;
  memcpy(&broadcast_addr.mac[0], &mBroadcastAddr, 4);
  memcpy(&broadcast_addr.mac[4], &mPort, 2);
  broadcast_addr.net = BACNET_BROADCAST_NETWORK;
  broadcast_addr.len = 0; // no SLEN
  memset(broadcast_addr.adr, 0, MAX_MAC_LEN); // no SADR

  //get local addr
  BACNET_ADDRESS local_addr;
  local_addr.mac_len = 6;
  memcpy(&local_addr.mac[0], &mLocalAddr, 4);
  memcpy(&local_addr.mac[4], &mPort, 2);
  local_addr.net = 0; // local
  local_addr.len = 0; // no SLEN
  memset(local_addr.adr, 0, MAX_MAC_LEN); // no SADR

  // encode npdu control data: no reply expected, normal priority
  BACNET_NPDU_DATA npdu_data;
  npdu_encode_npdu_data(&npdu_data, false, MESSAGE_PRIORITY_NORMAL);

  int pdu_len = 4;
  pdu_len += npdu_encode_pdu(&buffer[pdu_len], &broadcast_addr, &local_addr, &npdu_data);
  pdu_len += whois_encode_apdu(&buffer[pdu_len], device_id, device_id);

  buffer[0] = BVLL_TYPE_BACNET_IP;
  buffer[1] = BVLC_ORIGINAL_BROADCAST_NPDU;
  encode_unsigned16(&buffer[2], pdu_len);

  return pdu_len;
}
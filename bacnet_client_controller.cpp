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
  
  // get my address
  mMyNetworkAddress = getMyNetworkAddress();
  // open socket
  mBacnetSocket = openBacnetIPSocket();
  DEVLOG_DEBUG("[CBacnetClientController] init(): Opened BACnet client UDP socket %s:%04X\n", inet_ntoa(mMyNetworkAddress.sin_addr), ntohs(mMyNetworkAddress.sin_port));

  address_init();
  
  return 0;
}

#define NETWORK_IFACE_NAME "enp0s25"
sockaddr_in CBacnetClientController::getMyNetworkAddress() {
  sockaddr_in my_addr = {};
  ifreq ifr = {};
  strncpy(ifr.ifr_name, NETWORK_IFACE_NAME, sizeof(ifr.ifr_name));
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  (void) ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);
  my_addr = *((struct sockaddr_in *)&ifr.ifr_addr);
  my_addr.sin_port = htons(m_stConfig.nPortNumber);
  return my_addr;
}


CBacnetClientController::TSocketDescriptor CBacnetClientController::openBacnetIPSocket() {
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
    return sockfd;
  }
}

void CBacnetClientController::runLoop() {
  DEVLOG_DEBUG("[CBacnetClientController] runLoop(): Starting controller loop\n");
  while(isAlive()) {
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

      int pdu_len = handle->encodeServiceReq(pdu, invoke_id, &my_adr, &dst);

      struct sockaddr_in bvlc_dest;
      bvlc_dest.sin_family = AF_INET;
      memcpy(&bvlc_dest.sin_addr.s_addr, &dst.mac[0], 4);
      memcpy(&bvlc_dest.sin_port, &dst.mac[4], 2);
      
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
        if(src->sin_addr.s_addr != mMyNetworkAddress.sin_addr.s_addr || 
           src->sin_port != mMyNetworkAddress.sin_port) {

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
      // // shift the buffer
      // // he also copies source address (needed?)
      // for (int i = 0; i < npdu_len; i++) {
      //     pdu[i] = pdu[4 + i];
      // }
      // break;
  }

}

void CBacnetClientController::decodeBacnetPacket(uint8_t *pdu, uint16_t len) {
   // check if we received bacnet/ip packet
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
  // subtract the BVLC header length
  npdu_len -= 4;

  //decide depending on the function code of the packet
  switch (pdu[1]) {
    // see bvlc.c - bvlc_receive() for other function codes
    case BVLC_ORIGINAL_UNICAST_NPDU:
      // shift the buffer
      // he also copies source address (needed?)
      for (int i = 0; i < npdu_len; i++) {
          pdu[i] = pdu[4 + i];
      }
      break;
  }

  if (pdu[0] == BACNET_PROTOCOL_VERSION) {
    
    DEVLOG_DEBUG("[CBacnetClientController] decoding...\n");
    BACNET_ADDRESS dest = { 0 };
    BACNET_ADDRESS src = { 0 };
    BACNET_NPDU_DATA npdu_data = { 0 };
    int apdu_offset = npdu_decode(&pdu[0], &dest, &src, &npdu_data);

    if ((apdu_offset > 0) && (apdu_offset <= npdu_len)) {
      if ((dest.net == 0) || (dest.net == BACNET_BROADCAST_NETWORK)) {
          /* only handle the version that we know how to handle */
          /* and we are not a router, so ignore messages with
              routing information cause they are not for us */
          if ((dest.net == BACNET_BROADCAST_NETWORK) &&
              ((pdu[apdu_offset] & 0xF0) ==
                  PDU_TYPE_CONFIRMED_SERVICE_REQUEST)) {
              /* hack for 5.4.5.1 - IDLE */
              /* ConfirmedBroadcastReceived */
              /* then enter IDLE - ignore the PDU */
          } else {
              // apdu_handler(src, &pdu[apdu_offset],
              //     (uint16_t) (pdu_len - apdu_offset));
              switch(pdu[apdu_offset] & 0xF0) {
                case PDU_TYPE_COMPLEX_ACK: // see apdu.c - apdu_handler() for other types
                  BACNET_CONFIRMED_SERVICE_ACK_DATA service_ack_data = { 0 };
                  service_ack_data.segmented_message = (pdu[apdu_offset] & 1<<3) ? true : false;
                  service_ack_data.more_follows = (pdu[apdu_offset] & 1<<2) ? true : false;
                  service_ack_data.invoke_id = pdu[apdu_offset+1];
                  apdu_offset++;
                  if (service_ack_data.segmented_message) {
                    service_ack_data.sequence_number = pdu[apdu_offset+1];
                    apdu_offset++;
                    service_ack_data.proposed_window_number = pdu[apdu_offset+1];
                    apdu_offset++;
                  }
                  uint8_t service_choice = pdu[apdu_offset+1];
                  apdu_offset++;
                  // service_request = &pdu[len];
                  // service_request_len = apdu_len - (uint16_t) len;
                  DEVLOG_DEBUG("[CBacnetClientController] Received Complex Acknowledge: segmented_message=%d, more_follows=%d, invoke_id=%d, service_choice=", service_ack_data.segmented_message, service_ack_data.more_follows, service_ack_data.invoke_id);
                  switch (service_choice){
                    case SERVICE_CONFIRMED_READ_PROPERTY:
                      printf("CONFIRMED_READ_PROPERTY\n");
                  }
                  break;
              }

          }
      }
    }
    } else {
    DEVLOG_DEBUG("[CBacnetClientController] decodeBacnetPacket(): unknown protocol version");
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
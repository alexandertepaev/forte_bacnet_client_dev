#include "bacnet_client_controller.h"

CBacnetClientController::CBacnetClientController(CDeviceExecution& paDeviceExecution, int id) : forte::core::io::IODeviceController(paDeviceExecution), m_nControllerID(id), m_nSendRingbufferHeadIndex(0), m_nSendRingbufferSize(0), m_nSendRingbufferTailIndex(0), mBacnetSocket(0), m_eClientControllerState(e_Init), pmAddrList(0), invoke_id(0), pmServiceConfigFBsList(0), pmCOVSubscribers(0)
{
  memset(m_aSendRingbuffer, 0, cm_nSendRingbufferSize * sizeof(TBacnetServiceHandlePtr));
  
  m_stConfig.nPortNumber = 0;
  m_stConfig.nDeviceObjID = 0; //TODO: not used
  m_stConfig.sDeviceObjName = NULL; //TODO: not used
  m_stConfig.sPathToAddrFile = NULL; //TODO: not used

}

CBacnetClientController::~CBacnetClientController()
{
}

void CBacnetClientController::setConfig(Config* paConfig) {
  m_stConfig = *static_cast<SBacnetClientControllerConfig *>(paConfig);
  DEVLOG_DEBUG("[CBacnetClientController] setConfig(): Client controller's configuration parameters set\n");
}


const char* CBacnetClientController::init() {
  // SCFB list
  pmServiceConfigFBsList = new TServiceConfigFBsList();
  // ringbuffer
  memset(m_aSendRingbuffer, 0, cm_nSendRingbufferSize * sizeof(TBacnetServiceHandlePtr));
  m_nSendRingbufferHeadIndex = m_nSendRingbufferTailIndex = m_nSendRingbufferSize = 0; // TODO struct for ringbuffer?
  //addr list
  pmAddrList = new TBacnetAddrList();
  //subs list
  pmCOVSubscribers = new TBacnetCOVSubList();
  
  // init addresses
  initNetworkAddresses();

  // open socket
  mBacnetSocket = openBacnetIPSocket();
  
  DEVLOG_DEBUG("[CBacnetClientController] init(): Opened BACnet client UDP socket %s:%04X\n", inet_ntoa(mLocalAddr), mPort);
  
  return 0;
}

//TODO: set default to eth0
#define NETWORK_IFACE_NAME "enp0s25"
//#define NETWORK_IFACE_NAME "wlp4s0"


//TODO: private
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

//TODO: private
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

//TODO: private
int CBacnetClientController::sendPacket(uint8_t *buffer, uint16_t len, struct in_addr dest_addr, uint16_t dest_port) {
  struct sockaddr_in bvlc_dest;
  bvlc_dest.sin_family = AF_INET;
  bvlc_dest.sin_addr.s_addr = dest_addr.s_addr;
  bvlc_dest.sin_port = htons(dest_port);
  memset(bvlc_dest.sin_zero, 0, 8);

  int send_len = sendto(mBacnetSocket, (char *) buffer, len, 0, (struct sockaddr *) &bvlc_dest, sizeof(struct sockaddr));
  if(send_len > 0){
    DEVLOG_DEBUG("[CBacnetClientController] Sent a BACnet packet of length %d\n", send_len);
  } else {
    DEVLOG_DEBUG("[CBacnetClientController] sendto() failed\n");
  }
  return send_len;
}

int CBacnetClientController::receivePacket(uint8_t *buffer, size_t buffer_size, uint16_t timeout, sockaddr_in &src) {
  struct timeval select_timeout;
  select_timeout.tv_sec = 0;
  select_timeout.tv_usec = 1000 * timeout; // 100 ms timeout on receive - non-blocking recv
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(mBacnetSocket, &read_fds);

  if(select(mBacnetSocket+1, &read_fds, NULL, NULL, &select_timeout) > 0){
    //sockaddr_in src;
    socklen_t srcLen = sizeof(src);
    int rcv_retval = recvfrom(mBacnetSocket, buffer, buffer_size, 0, (sockaddr *) &src, &srcLen);
    if(rcv_retval > 0) {
      DEVLOG_DEBUG("[CBacnetClientController] Received a BACnet packet of length %d\n", rcv_retval);
    } else  {
      DEVLOG_DEBUG("[CBacnetClientController] recvfrom() failed\n");
    }

    // msg from myself -- ignore
    if(src.sin_addr.s_addr == mLocalAddr.s_addr && src.sin_port == htons(mPort)) { 
      return 0;
    } else {
      // it's all good
      return rcv_retval;
    }
  }
  return -1;
}

//TODO: private
bool CBacnetClientController::getAddressByDeviceId(uint32_t paDeviceId, struct in_addr &paDeviceAddr, uint16_t &paDeviceAddrPort) {
  TBacnetAddrList::Iterator itEnd = pmAddrList->end();
  for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it){
    if((*it)->mDeviceId == paDeviceId && (*it)->mAddrInitFlag) {
      paDeviceAddr = (*it)->mAddr;
      paDeviceAddrPort = (*it)->mPort;
      return true;
    }
  }
  return false;
}

//TODO: private
BACNET_ADDRESS CBacnetClientController::ipToBacnetAddress(struct in_addr paDeviceAddr, uint16_t paPort, bool paBroadcastAddr){
  BACNET_ADDRESS ret_val;
  ret_val.mac_len = (uint8_t) 6;
  memcpy(&ret_val.mac[0], &paDeviceAddr, 4);
  memcpy(&ret_val.mac[4], &paPort, 2);
  ret_val.net = paBroadcastAddr ? BACNET_BROADCAST_NETWORK : 0;
  ret_val.len = 0; // no SLEN
  memset(ret_val.adr, 0, MAX_MAC_LEN); // no SADR

  return ret_val;
}

//TODO: private, rename to sendWhoIs(uint32_t device_id, uint8_t *buffer)
//_TODO: use this code snippet in the main loop
void CBacnetClientController::buildWhoIsAndSend(uint32_t device_id, uint8_t *buffer) {
  uint16_t len = encodeWhoIs(device_id, buffer);
  
  sendPacket(buffer, len, mBroadcastAddr, mPort);
}

//TODO: combine with sendWhoIs(...)
int CBacnetClientController::encodeWhoIs(uint32_t device_id, uint8_t *buffer){

  //get broadcast addr
  BACNET_ADDRESS broadcast_addr = ipToBacnetAddress(mBroadcastAddr, mPort, true);

  //get local addr
  BACNET_ADDRESS local_addr = ipToBacnetAddress(mLocalAddr, mPort, false);

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

/*
  switch(m_eClientControllerState){
    case e_AddressDiscovery:
      discoverNetworkAddresses();
      m_eClientControllerState = e_COVSubscription;
      break;
    case e_COVSubscription:
      subscribeToCOVNotifications();
      m_eClientControllerState = e_Operation;
      break;
    case e_Operation:
      //processOutputRingbuffer();
      performOperationCycleStep();
      break;
    default:
      break;
  }

  ---

  discoverNetworkAddresses() {
    TBacnetAddrList::Iterator itEnd = pmAddrList->end();
    for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it) {
      sendWhoIs((*it)->mDeviceId);

      timespec current_time;
      clock_gettime(CLOCK_MONOTONIC, current_time);
      while((*it)->mAddr == 0  && !timeoutMillis(500, current_time)){
        receiveIAm((*it)->mDeviceId);
      }

      if((*it)->mAddr != 0){
        notifyConfigFBsAboutAddrDiscorvery((*it)->mDeviceId, CBacnetServiceConfigFB::e_Success);
      } else {
        notifyConfigFBsAboutAddrDiscorvery((*it)->mDeviceId, CBacnetServiceConfigFB::e_AddrFetchFailed);
        pmAddrList->erase(*it);
      }

    }
  }

  subscribeToCOVNotifications() {
    TBacnetCOVSubList::Iterator itEnd = pmCOVSubscribers->end();
    for(TBacnetCOVSubList::Iterator it = pmCOVSubscribers->begin(); it != itEnd; ++it) {

      sendUnconfirmedCOVSubscribe((*it));

      timespec current_time;
      clock_gettime(CLOCK_MONOTONIC, current_time);
      while(!(*it)->mSubscribtionAcknowledged && !timeoutMillis(500, current_time)) {
        receiveCOVSubscriptionAck();
      }

      if((*it)->subscriptionAcknowledged){
        (*it)->mServiceConfigFB->setNotificationType(CBacnetServiceConfigFB::e_Success);
        startNewEventChain((*it)->mServiceConfigFB);
      } else {
        (*it)->mServiceConfigFB->setNotificationType(CBacnetServiceConfigFB::e_COVSubscriptionFailed);
        startNewEventChain((*it)->mServiceConfigFB);
        pmCOVSubscribers->erase((*it));
      }

    }
  }
*/

void CBacnetClientController::runLoop() {
  DEVLOG_DEBUG("[CBacnetClientController] runLoop(): Starting controller loop\n");
  while(isAlive()) {

   if (m_eClientControllerState == e_AddressFetch) {

      TBacnetAddrList::Iterator itEnd = pmAddrList->end();
      for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it) {
        uint8_t pdu[MAX_MPDU]; // TODO: create class member variable 
        buildWhoIsAndSend((*it)->mDeviceId, pdu);

        struct timespec t1, t2, tdiff;
        uint16_t millis_elapsed = 0;
        clock_gettime(CLOCK_MONOTONIC, &t1);

        while(!(*it)->mAddrInitFlag && millis_elapsed <= 500){
          // receive and handle i-am
          
          // TODO: check return value
          receiveAndHandle(); // TODO: only handle I-Am responses
          
          // calculate elapsed milliseconds
          clock_gettime(CLOCK_MONOTONIC, &t2);
          timespecSub(&t2, &t1, &tdiff);
          millis_elapsed = round(tdiff.tv_nsec / 1.0e6) + tdiff.tv_sec * 1000; 

        }

        if(!(*it)->mAddrInitFlag) {
          DEVLOG_DEBUG("[CBacnetClientController] Couldn't fetch address for device %d\n", (*it)->mDeviceId);
          // TODO: notify SCFB - update STATUS+QO and trigger IND event

          // notify all scfbs about addr fetch faliure
          TServiceConfigFBsList::Iterator _itEnd = pmServiceConfigFBsList->end();
          for(TServiceConfigFBsList::Iterator _it = pmServiceConfigFBsList->begin(); _it != _itEnd; ++_it){
            if((*it)->mDeviceId == (*_it)->m_stServiceConfig->mDeviceId){
              (*_it)->setNotificationType(CBacnetServiceConfigFB::e_AddrFetchFailed);
              startNewEventChain((*_it));
              pmServiceConfigFBsList->erase((*_it));
            }
          }

          //remove from COVSubscribe list
          TBacnetCOVSubList::Iterator subCOVItEnd = pmCOVSubscribers->end();
          for(TBacnetCOVSubList::Iterator subCOVIt = pmCOVSubscribers->begin(); subCOVIt != subCOVItEnd; ++subCOVIt) {
            if((*subCOVIt)->mServiceConfigFB->m_stServiceConfig->mDeviceId == (*it)->mDeviceId)
              pmCOVSubscribers->erase((*subCOVIt));
          }

          //TODO erase from SCFB list

          // erase from addr table
          pmAddrList->erase((*it));

        
        } else {

          // notify all fbs but cov subsribers about addr fetch success
          TServiceConfigFBsList::Iterator _itEnd = pmServiceConfigFBsList->end();
          for(TServiceConfigFBsList::Iterator _it = pmServiceConfigFBsList->begin(); _it != _itEnd; ++_it){
            if((*it)->mDeviceId == (*_it)->m_stServiceConfig->mDeviceId && (*_it)->mServiceType != CBacnetServiceConfigFB::EServiceType::e_UnconfirmedCOVSub){

              (*_it)->setNotificationType(CBacnetServiceConfigFB::e_Success);
              startNewEventChain((*_it));
            
            }
          }

        } 
      }
      
      m_eClientControllerState = e_COVSubscription; 

   } else if (m_eClientControllerState == e_COVSubscription) {

      TBacnetCOVSubList::Iterator itEnd = pmCOVSubscribers->end();
      for(TBacnetCOVSubList::Iterator it = pmCOVSubscribers->begin(); it != itEnd; ++it) {
        uint8_t pdu[MAX_MPDU];
        buildSubscribeCOVAndSend((*it)->mServiceConfigFB->m_stServiceConfig, pdu, (*it)->mAssignedInvokeId);
      
        struct timespec t1, t2, tdiff;
        uint16_t millis_elapsed = 0;
        clock_gettime(CLOCK_MONOTONIC, &t1);

        while(!(*it)->mSubscriptionAcknowledged && millis_elapsed <= 500){
          // receive and handle subscription ack

          // TODO: check return value
          receiveAndHandle(); // TODO: only handle sub ack
          
          // calculate elapsed milliseconds
          clock_gettime(CLOCK_MONOTONIC, &t2);
          timespecSub(&t2, &t1, &tdiff);
          millis_elapsed = round(tdiff.tv_nsec / 1.0e6) + tdiff.tv_sec * 1000; 
        }

        if(!(*it)->mSubscriptionAcknowledged){
          (*it)->mServiceConfigFB->setNotificationType(CBacnetServiceConfigFB::e_COVSubscriptionFailed);
          startNewEventChain((*it)->mServiceConfigFB);
          pmCOVSubscribers->erase((*it));
        } else {
          (*it)->mServiceConfigFB->setNotificationType(CBacnetServiceConfigFB::e_Success);
          startNewEventChain((*it)->mServiceConfigFB);
        }
        //TODO notify scfb about failure/success? see line 725

      }


      m_eClientControllerState = e_Operating;
   } else if (m_eClientControllerState == e_Operating) {
    //consume from ringbuffer
    if(m_nSendRingbufferSize != 0) {
      CBacnetServiceHandle *handle = consumeFromRingbuffer();
      buildPacketAndSend(handle);
    }

    // receive response or some kind of notification
    // TODO: check return value
    receiveAndHandle();


    //TODO: push the InvokeId+Handle to a list (queue) and, if receive and handle returns 0 and timeout, remove it from the list (erase) and set handle's state to Idle + notify config fb - Connection Lost
   }
    
  }
}


//TODO: private
void CBacnetClientController::decodeAndHandleIAm(uint8_t *apdu, const uint32_t &apdu_len, const struct sockaddr_in &src) {
  uint32_t device_id;
  int len = iam_decode_service_request(&apdu[2], &device_id, NULL, NULL, NULL);
  if(len != -1) {
    DEVLOG_DEBUG("[CBacnetClientController] decodeAndHandleIAm(): Updating addr entry for Device %d, addr %s:%04X\n", device_id, inet_ntoa(src.sin_addr), htons(src.sin_port));
    TBacnetAddrList::Iterator itEnd = pmAddrList->end();
    for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it){
      if((*it)->mDeviceId == device_id) {
        (*it)->mAddr = src.sin_addr;
        (*it)->mPort = htons(src.sin_port);
        if(!(*it)->mAddrInitFlag){
          (*it)->mAddrInitFlag = true;
          // TODO: tell config fb to update it's status? 
        }
      }
    }
  }
}


//TODO: rewrite function to get next FREE invoke id? - array of booleans, containing 256 entries 
uint8_t CBacnetClientController::getNextInvokeID(){
  uint8_t ret_val = invoke_id;
  invoke_id = invoke_id == 255 ? 0 : invoke_id+1;
  return ret_val;
}


//TODO: private
void CBacnetClientController::buildPacketAndSend(CBacnetServiceHandle *handle) {

  // get target device id and address
  struct in_addr dest_addr;
  uint16_t dest_port;
  getAddressByDeviceId(handle->mConfigFB->m_stServiceConfig->mDeviceId, dest_addr, dest_port); // TODO - do nothing if addr not found + do not packet from the ring buffer?
  DEVLOG_DEBUG("[CBacnetClientController] buildPAcketAndSend(): Destenation address %s:%04X\n", inet_ntoa(dest_addr), dest_port);

  // get next free invoke id
  uint8_t invoke_id = getNextInvokeID(); // TODO get free invoke id

  // encode bacnet packet
  uint8_t pdu[MAX_MPDU];
  BACNET_ADDRESS dest = ipToBacnetAddress(dest_addr, dest_port, false);
  BACNET_ADDRESS src = ipToBacnetAddress(mLocalAddr, mPort, false);

  int pdu_len = handle->encodeServiceReq(pdu, invoke_id, &dest, &src);

  // send it and if send was successful, add invokeid:handle pair
  if( sendPacket(pdu, pdu_len, dest_addr, dest_port) > 0) {
    addInvokeIDHandlePair(invoke_id, handle);
  }

}

//TODO: private
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

//TODO: private
bool CBacnetClientController::removeInvokeIDHandlePair(const uint8_t &paInvokeID) {
  for(TInvokeIDHandleMap::iterator it = mInvokeIDsHandles.begin(); it != mInvokeIDsHandles.end(); ++it) {
    if(it->first == paInvokeID) {
      mInvokeIDsHandles.erase(it);
      DEVLOG_DEBUG("[CBacnetClientController] removeInvokeIDHandlePair(): Erased mInvokeIDsHandles's entry '%d', mInvokeIDsHandles's size=%d\n", paInvokeID, mInvokeIDsHandles.size());
      return true;
    }
  }
  return false;
}

//TODO: private
void CBacnetClientController::receiveAndHandle(){
  uint8_t npdu[MAX_PDU];
  sockaddr_in src;
  int rcv_retval = receivePacket(npdu, sizeof(npdu), 100, src);

  if(rcv_retval > 0){
    uint8_t service_choice, apdu_type = 0;
    uint32_t apdu_offset, apdu_len = 0;
    if(decodeNPDU(npdu, apdu_offset, apdu_len, apdu_type, service_choice)) {

      DEVLOG_DEBUG("[CBacnetClientController] NPDU info: apdu_offset=%d, apdu_len=%d, apdu_type=%02X, service_choice=%02X\n", apdu_offset, apdu_len, apdu_type, service_choice);

      handleAPDU(&npdu[apdu_offset+4], apdu_len, apdu_type, service_choice, src); // move it here?

    } else {
        DEVLOG_DEBUG("[CBacnetClientController] decodeNPDU() failed\n");
    }
  }
}

//TODO: private
bool CBacnetClientController::decodeNPDU(uint8_t *pdu, uint32_t &apdu_offset, uint32_t &apdu_len, uint8_t &apdu_type, uint8_t &service_choice) {
  if (pdu[0] == BVLL_TYPE_BACNET_IP) {
    /* Bacnet/IP signature check */
    uint16_t npdu_len;
    (void) decode_unsigned16(&pdu[2], &npdu_len);

    if ((npdu_len >= 4) && (npdu_len <= (MAX_MPDU-4))) { 
      /* Length of NPDU is OK (including BVLC length) */

      if(pdu[1] == BVLC_ORIGINAL_UNICAST_NPDU || pdu[1] == BVLC_ORIGINAL_BROADCAST_NPDU) {
        /* For now only handle unicast/broadcast BVLC types */

        if (pdu[4] == BACNET_PROTOCOL_VERSION) {
          /* Protocol version 0x01 */

          BACNET_NPDU_DATA npdu_data = { 0 };
          BACNET_ADDRESS dest = { 0 };
          apdu_offset = npdu_decode(&pdu[4], &dest, NULL, &npdu_data);

          if(!npdu_data.network_layer_message && (apdu_offset > 0) && (apdu_offset <= npdu_len - 4) 
              && ((dest.net == 0) || (dest.net == BACNET_BROADCAST_NETWORK))) {
                /* NOT network layer message, APDU offset is in bounds, NOT routing infromation */

                apdu_len = npdu_len-4-apdu_offset;
                apdu_type = (pdu[apdu_offset+4] & 0xF0);

                if((dest.net != BACNET_BROADCAST_NETWORK) || (apdu_type != PDU_TYPE_CONFIRMED_SERVICE_REQUEST )) {

                  /* NOT confirmed Broadcast (TODO read section 5.4.5.1)*/


                  switch (apdu_type)
                  {
                    case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
                      /* I-Am + UnconfirmedCOVNotification*/
                      service_choice = pdu[apdu_offset+4+1];
                      if(service_choice < MAX_BACNET_UNCONFIRMED_SERVICE)
                        return true;
                      break;

                    case PDU_TYPE_SIMPLE_ACK:
                      /* WriteProperty acknowledge */
                      service_choice = pdu[apdu_offset+4+2];
                      if(service_choice == SERVICE_CONFIRMED_WRITE_PROPERTY ||
                          service_choice == SERVICE_CONFIRMED_SUBSCRIBE_COV)
                        return true;
                      break;

                    case PDU_TYPE_COMPLEX_ACK:
                      /* ReadProperty acknowledge and result */
                      if( !(pdu[apdu_offset+4] & 1<<3) ) {
                        /* NOT segmented */
                        service_choice = pdu[apdu_offset+4+2];
                        if(service_choice == SERVICE_CONFIRMED_READ_PROPERTY)
                          return true;
                      }
                      break;

                    default:
                      return false;
                      break;
                  }
                  
                }
          }
        }
      }
    }
  }

  return false;
}

//TODO: private
void CBacnetClientController::handleAPDU(uint8_t *apdu, const uint32_t &apdu_len, const uint8_t &apdu_type, const uint8_t &service_choice, const struct sockaddr_in &src) {

  switch (apdu_type)
  {
    case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
      if(service_choice ==  SERVICE_UNCONFIRMED_I_AM){
        decodeAndHandleIAm(apdu, apdu_len, src);
      } else if (service_choice == SERVICE_UNCONFIRMED_COV_NOTIFICATION) {
        handleUnconfirmedCOVNotifation(apdu, apdu_len);
      }
      break;
    
    case PDU_TYPE_SIMPLE_ACK:
      if(service_choice == SERVICE_CONFIRMED_WRITE_PROPERTY) {
        handleWPAck(apdu, apdu_len);
      } else if(service_choice == SERVICE_CONFIRMED_SUBSCRIBE_COV) {
        handleCOVSubscriptionAck(apdu, apdu_len);
      }
      break;

    case PDU_TYPE_COMPLEX_ACK:
      if(service_choice == SERVICE_CONFIRMED_READ_PROPERTY) {
        handleRPAck(apdu, apdu_len);
      }
      break;

    default:
      break;
  }

}

//TODO: private
void CBacnetClientController::handleRPAck(uint8_t *apdu, const uint32_t &apdu_len) {
  uint8_t invoke_id = apdu[1];
  if(mInvokeIDsHandles.find(invoke_id) != mInvokeIDsHandles.end()) {
    mInvokeIDsHandles[invoke_id]->decodeServiceResp(apdu, apdu_len);
    removeInvokeIDHandlePair(invoke_id);
  }
}

//TODO: private (1 to 1 with handleRPAck?)
void CBacnetClientController::handleWPAck(uint8_t *apdu, const uint32_t &apdu_len) {
  uint8_t invoke_id = apdu[1];
  if(mInvokeIDsHandles.find(invoke_id) != mInvokeIDsHandles.end()) {
    mInvokeIDsHandles[invoke_id]->decodeServiceResp(apdu, apdu_len);
    removeInvokeIDHandlePair(invoke_id);
  }
}

forte::core::io::IOHandle* CBacnetClientController::initHandle(IODeviceController::HandleDescriptor *handleDescriptor) {

  HandleDescriptor *desc = static_cast<CBacnetClientController::HandleDescriptor *>(handleDescriptor);
  
  CIEC_ANY::EDataTypeID data_type = CIEC_ANY::e_ANY;

  if( desc->mServiceConfigFB->m_stServiceConfig->mObjectType == OBJECT_ANALOG_OUTPUT || 
        desc->mServiceConfigFB->m_stServiceConfig->mObjectType == OBJECT_ANALOG_INPUT ||
        desc->mServiceConfigFB->m_stServiceConfig->mObjectType ==  OBJECT_ANALOG_VALUE) { // TODO - check in scfb (rp,wp) if it tgt object prop is present value/cov increment

        data_type = CIEC_ANY::e_DWORD;

  } else if ( desc->mServiceConfigFB->m_stServiceConfig->mObjectType == OBJECT_BINARY_OUTPUT || 
              desc->mServiceConfigFB->m_stServiceConfig->mObjectType ==  OBJECT_BINARY_INPUT ||
              desc->mServiceConfigFB->m_stServiceConfig->mObjectType ==  OBJECT_BINARY_VALUE) { // TODO - check in scfb (rp,wp) if it tgt object prop is present value/cov increment

        data_type = CIEC_ANY::e_BOOL;

  } else {
    return 0;
  }

  switch (desc->mServiceType)
  {
    case SERVICE_CONFIRMED_READ_PROPERTY:
      // TODO type based on the accessed object params?
      return new CBacnetReadPropertyHandle(this, desc->mDirection, data_type, mDeviceExecution, desc->mServiceConfigFB); //TODO: is it better to compose a pdu one single time here and store it's value in rom or is it better to compose it every single time we want to send a request
      break;
    case SERVICE_CONFIRMED_WRITE_PROPERTY:
      // TODO - type based on the accessed object params? 
      return new CBacnetWritePropertyHandle(this, desc->mDirection, data_type, mDeviceExecution, desc->mServiceConfigFB);
      break;
    case SERVICE_CONFIRMED_SUBSCRIBE_COV:
      return new CBacnetUnconfirmedCOVHandle(this, desc->mDirection, data_type, mDeviceExecution, desc->mServiceConfigFB);
      break;
    default:
      DEVLOG_DEBUG("[CBacnetClientController] initHandle(): Unknown/Unsupported BACnet Service\n");
      return 0;
      break;
  }
}


//TODO: public
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

//TODO: private
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

void CBacnetClientController::initDone() {
  if(m_eClientControllerState == e_Init) {
    DEVLOG_DEBUG("[CBacnetClientController] initDone() - changing state from init to addr fetch\n");
    populateAddrTable();
    populateCOVSubscribers();
    m_eClientControllerState = e_AddressFetch;
  }
}

//TODO private
void CBacnetClientController::buildSubscribeCOVAndSend(CBacnetServiceConfigFB::ServiceConfig *paConfig, uint8_t *buffer, uint8_t &paAssignedInvokeId) {
  // get dest addr
  struct in_addr dest_addr;
  uint16_t dest_port;
  if(!getAddressByDeviceId(paConfig->mDeviceId, dest_addr, dest_port)) {
    return;
  }
    

  //get bacnet dest addr
  BACNET_ADDRESS bacnet_dest_addr = ipToBacnetAddress(dest_addr, dest_port, false);
  //get bacnet local addr
  BACNET_ADDRESS bacnet_local_addr = ipToBacnetAddress(mLocalAddr, mPort, false);

  //encode npdu part
  BACNET_NPDU_DATA npdu_data;
  npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);
  int pdu_len = 4;
  pdu_len += npdu_encode_pdu(&buffer[pdu_len], &bacnet_dest_addr, &bacnet_local_addr, &npdu_data);

  //encode apdu
  BACNET_SUBSCRIBE_COV_DATA cov_data;
  memset(&cov_data, 0, sizeof(BACNET_SUBSCRIBE_COV_DATA));
  cov_data.monitoredObjectIdentifier.type = paConfig->mObjectType;
  cov_data.monitoredObjectIdentifier.instance = paConfig->mObjectId;
  cov_data.subscriberProcessIdentifier = 1; // hardcoded
  cov_data.issueConfirmedNotifications = false; // hardcoded
  cov_data.lifetime = 0; // hardcoded

  uint8_t invoke_id = getNextInvokeID();
  paAssignedInvokeId = invoke_id;

  pdu_len += cov_subscribe_encode_apdu(&buffer[pdu_len], invoke_id, &cov_data);
  
  /*cov_data.monitoredProperty.propertyIdentifier = PROP_PRESENT_VALUE;
  cov_data.monitoredProperty.propertyArrayIndex = BACNET_ARRAY_ALL;
  pdu_len += cov_subscribe_property_encode_apdu(&buffer[pdu_len], invoke_id, &cov_data);*/ // subProperty not supported by bacnet-stack, not possible to test

  buffer[0] = BVLL_TYPE_BACNET_IP;
  buffer[1] = BVLC_ORIGINAL_BROADCAST_NPDU;
  encode_unsigned16(&buffer[2], pdu_len);

  DEVLOG_DEBUG("[CBacnetClientController] buildSubscribeCOVAndSend() - Sending COV Subscribe DeviceId=%d, ObjectType=%d, ObjectId=%d\n", paConfig->mDeviceId, paConfig->mObjectType, paConfig->mObjectId);

  sendPacket(buffer, pdu_len, dest_addr, dest_port);
}

//TODO: private
void CBacnetClientController::handleCOVSubscriptionAck(uint8_t *apdu, const uint32_t &apdu_len) {
  /* Multiple subscription to the same (Device, ObjectType, ObjectID) tuple are sent multiple times -- fix it?*/
  uint8_t invoke_id = apdu[1];
  TBacnetCOVSubList::Iterator itEnd = pmCOVSubscribers->end();
  for(TBacnetCOVSubList::Iterator it = pmCOVSubscribers->begin(); it != itEnd; ++it){
    if((*it)->mAssignedInvokeId == invoke_id){
      (*it)->mSubscriptionAcknowledged = true;
      static_cast<CBacnetUnconfirmedCOVHandle *>((*it)->mServiceConfigFB->mServiceHandle)->subscriptionAcknowledged(); //TODO notify scfb? no need to store handle pointer
      DEVLOG_DEBUG("[CBacnetClientController] handleCOVSubscriptionAck() -- Subscription acknowledged!\n");
      break;
    }
  }
}

//TODO: private
void CBacnetClientController::handleUnconfirmedCOVNotifation(uint8_t *apdu, const uint32_t &apdu_len) {
  DEVLOG_DEBUG("[CBacnetClientController] handleUnconfirmedCOVNotifation() -- Received COV Notification!\n");
  /*Analog+Binary Input/Output/Value return a list of values in a CoV notification, 
    containig two entries - Present Value and Status flag. See Clause 13.1, Table 13-1*/
  /*Create this list*/
  BACNET_PROPERTY_VALUE property_value[2];
  property_value[0].next = &property_value[1];
  BACNET_COV_DATA cov_data;
  cov_data.listOfValues = property_value;

  int len = cov_notify_decode_service_request(&apdu[2], len-2, &cov_data);

  printf("Decoded Unconfirmed CoV Notification: PID=%d, DeviceId=%d, ObjectType=%d, ObjectId=%d, TimeRemaining=%d, Value=%d\n",
  cov_data.subscriberProcessIdentifier, cov_data.initiatingDeviceIdentifier, cov_data.monitoredObjectIdentifier.type, cov_data.monitoredObjectIdentifier.instance, cov_data.timeRemaining, property_value[0].value.type.Enumerated);

  TBacnetCOVSubList::Iterator itEnd = pmCOVSubscribers->end();
  for(TBacnetCOVSubList::Iterator it = pmCOVSubscribers->begin(); it != itEnd; ++it){
    if((*it)->mServiceConfigFB->m_stServiceConfig->mDeviceId == cov_data.initiatingDeviceIdentifier &&
        (*it)->mServiceConfigFB->m_stServiceConfig->mObjectType == cov_data.monitoredObjectIdentifier.type &&
        (*it)->mServiceConfigFB->m_stServiceConfig->mObjectId == cov_data.monitoredObjectIdentifier.instance) {

          static_cast<CBacnetUnconfirmedCOVHandle *>((*it)->mServiceConfigFB->mServiceHandle)->notificationReceived(property_value[0]);
          
    
    }
  }
}

void CBacnetClientController::updateSCFBsList(CBacnetServiceConfigFB *paConfigFB) {
  pmServiceConfigFBsList->pushBack(paConfigFB);
}

void CBacnetClientController::populateAddrTable() {

  TServiceConfigFBsList::Iterator itEnd = pmServiceConfigFBsList->end();
  for(TServiceConfigFBsList::Iterator it = pmServiceConfigFBsList->begin(); it != itEnd; ++it){

    if(!checkAddrTableForDuplicate((*it)->m_stServiceConfig->mDeviceId)) {

      SBacnetAddressListEntry *newElement = new SBacnetAddressListEntry();
      newElement->mDeviceId = (*it)->m_stServiceConfig->mDeviceId;
      newElement->mAddrInitFlag = false;
      memset(&newElement->mAddr, 0, sizeof(struct in_addr));
      newElement->mPort = 0;
      pmAddrList->pushBack(newElement);

    }
  }

}

bool CBacnetClientController::checkAddrTableForDuplicate(uint32_t device_id) {
  TBacnetAddrList::Iterator itEnd = pmAddrList->end();
  for(TBacnetAddrList::Iterator it = pmAddrList->begin(); it != itEnd; ++it){
    if((*it)->mDeviceId == device_id) {
      DEVLOG_DEBUG("[CBacnetClientController] checkAddrTableForDuplicate() - Found duplicated entry for %d, ignoring add\n", (*it)->mDeviceId);
      return true;
    }
  }
  return false;
}

void CBacnetClientController::populateCOVSubscribers() {
  TServiceConfigFBsList::Iterator itEnd = pmServiceConfigFBsList->end();
  for(TServiceConfigFBsList::Iterator it = pmServiceConfigFBsList->begin(); it != itEnd; ++it){

    if((*it)->mServiceType == CBacnetServiceConfigFB::e_UnconfirmedCOVSub) {
      SBacnetCOVSubsListEntry *newElement = new SBacnetCOVSubsListEntry();
      newElement->mSubscriptionAcknowledged = false;
      newElement->mAssignedInvokeId = getNextInvokeID();
      newElement->mServiceConfigFB = (*it);
      pmCOVSubscribers->pushBack(newElement);

    }
  }
}

/*******************************************************************************
 * Copyright (c) 2020 Alexander Tepaev github.com/alexandertepaev
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev
 *******************************************************************************/
#include "bacnet_client_controller.h"
#include "bacnet_service_config_fb.h"
#include "bacnet_service_handle.h"
#include "bacnet_readproperty_handle.h"
#include "bacnet_writeproperty_handle.h"
#include "bacnet_unconfirmed_cov_handle.h"
#include "bacnet_subucov_service_config_fb.h"


CBacnetClientController::CBacnetClientController(CDeviceExecution& paDeviceExecution, TForteUInt16 paControllerID) : forte::core::io::IODeviceController(paDeviceExecution), m_nID(paControllerID), m_enClientControllerState(Init), mCommunicationSocket(0), m_nInvokeID(0)
{
  // Zero the configuration struct
  m_stConfig = { };
  // Zero the handles RingBuffer
  m_stHandlesRingBuffer = { };
  // handles list
  mHandles = new THandlesList();
  //addr list
  mAddrTable = new TAddrTable();
  //subs list
  //pmCOVSubscribers = new TCOVSubscribersList();
  mCOVHandles = new TCOVHandlesList();
  // tx list
  mActiveTransactions = new TTransactionList();
}

CBacnetClientController::~CBacnetClientController()
{
}

bool CBacnetClientController::pushToRingBuffer(CBacnetServiceHandle *paHandle) {
  CCriticalRegion criticalRegion(mRingBufferMutex);

  if(scm_nRingBufferSize == m_stHandlesRingBuffer.nSize)
    return false; // return false if the ring buffer is full

  m_stHandlesRingBuffer.aRingBuffer[m_stHandlesRingBuffer.nHeadIndex] = paHandle;
  m_stHandlesRingBuffer.nSize++;
  m_stHandlesRingBuffer.nHeadIndex = (m_stHandlesRingBuffer.nHeadIndex + 1) % scm_nRingBufferSize;
  return true;
}


void CBacnetClientController::setConfig(Config *paConfig) {
  this->m_stConfig = *static_cast<SBacnetClientControllerConfig *>(paConfig);
  setNetworkAddresses();

  DEVLOG_DEBUG("[CBacnetClientController] setConfig(): Local address: %s:%04X\n", inet_ntoa(m_stConfig.stLocalAddr), m_stConfig.nPort);
  DEVLOG_DEBUG("[CBacnetClientController] setConfig(): Broadcast address: %s:%04X\n",inet_ntoa(m_stConfig.stBroadcastAddr), m_stConfig.nPort);
}

void CBacnetClientController::setNetworkAddresses() {
  ifreq ifr = {};
  struct sockaddr_in IPAddress;
  int rv, fd;
  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  strncpy(ifr.ifr_name, scm_sNetwrokIfaceName, sizeof(ifr.ifr_name));
  if(fd >= 0) {
    // local address
    rv = ioctl(fd, SIOCGIFADDR, &ifr); // TODO: error checking
    IPAddress = *((struct sockaddr_in *) &ifr.ifr_addr);
    this->m_stConfig.stLocalAddr = IPAddress.sin_addr;

    // netmask
    rv = ioctl(fd, SIOCGIFNETMASK, &ifr);
    IPAddress = *((struct sockaddr_in *) &ifr.ifr_addr);
    close(fd);

    // broadcast address
    this->m_stConfig.stBroadcastAddr.s_addr = m_stConfig.stLocalAddr.s_addr | (~IPAddress.sin_addr.s_addr);
  }
}


const char* CBacnetClientController::init() {
  DEVLOG_DEBUG("[CBacnetClientController] init(): Controller ID: %d\n", m_nID);
  // open socket
  if(!(initCommunicationSocket())){
    return scmSocketInitFailed;
  }
  DEVLOG_DEBUG("[CBacnetClientController] init(): Initialized BACnet/IP communication socket\n");
  return 0;
}

void CBacnetClientController::deInit() {
  if(mCommunicationSocket > 0){
    close(mCommunicationSocket);
  }
}

bool CBacnetClientController::initCommunicationSocket() {
  //open socket
  mCommunicationSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(mCommunicationSocket <= 0) {
    DEVLOG_DEBUG("[CBacnetClientController] Failed to open socket\n");
    return false;
  }
  // allow broadcast msgs
  int sockopt = 1;
  if (setsockopt(mCommunicationSocket, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt)) < 0) {
    DEVLOG_DEBUG("[CBacnetClientController] initCommunicationSocket(): setsockopt SO_BROADCAST failed\n");
    return false;
  }
  //bind socket
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = m_stConfig.nPort;
  if(bind(mCommunicationSocket, (const sockaddr *)&address, sizeof(sockaddr_in)) < 0){
    DEVLOG_DEBUG("[CBacnetClientController] initCommunicationSocket(): Failed to bind socket\n");
    return false;
  }
  return true;
}

int CBacnetClientController::sendPacket(const TForteUInt16 &len, const struct in_addr &destinationAddr, const TForteUInt16 &destinationPort) {
  struct sockaddr_in dest;
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = destinationAddr.s_addr;
  dest.sin_port = destinationPort;
  memset(dest.sin_zero, 0, 8);
  int send_len = (int) sendto(mCommunicationSocket, (char *)mSendBuffer, len, 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
  return send_len;
}

int CBacnetClientController::receivePacket(const TForteUInt16 &paTimeout, sockaddr_in *paSourceAddress) {
  struct timeval selectTimeout;
  selectTimeout.tv_sec = 0;
  selectTimeout.tv_usec = scm_nMicrosInMillis * paTimeout;// convert milliseconds to micrseconds
  fd_set readFDs;
  FD_ZERO(&readFDs);
  FD_SET(mCommunicationSocket, &readFDs);
  if(select(mCommunicationSocket+1, &readFDs, NULL, NULL, &selectTimeout) > 0){
    socklen_t srcLen = sizeof(sockaddr_in);
    int rcv_retval = (int) recvfrom(mCommunicationSocket, mReceiveBuffer, sizeof(mReceiveBuffer), 0, (sockaddr *)paSourceAddress, &srcLen);
    if(paSourceAddress != NULL && paSourceAddress->sin_addr.s_addr ==  m_stConfig.stLocalAddr.s_addr && paSourceAddress->sin_port == m_stConfig.nPort) { 
      return 0; // msg from myself
    } else {
      return rcv_retval; // receive succeeded
    }
  }
  return -1; // nothing
}

forte::core::io::IOHandle* CBacnetClientController::initHandle(IODeviceController::HandleDescriptor *paHandleDescriptor) {
  HandleDescriptor *desc = static_cast<CBacnetClientController::HandleDescriptor *>(paHandleDescriptor);
  CBacnetServiceHandle *handle;
  switch (desc->mServiceType)
  {
    case SERVICE_CONFIRMED_READ_PROPERTY:
      handle = new CBacnetReadPropertyHandle(this, desc->mDirection, desc->mIECDataType, mDeviceExecution, desc->mServiceConfigFB);
      break;
    case SERVICE_CONFIRMED_WRITE_PROPERTY:
      handle = new CBacnetWritePropertyHandle(this, desc->mDirection, desc->mIECDataType, mDeviceExecution, desc->mServiceConfigFB);
      break;
    case SERVICE_CONFIRMED_SUBSCRIBE_COV:
      handle = new CBacnetUnconfirmedCOVHandle(this, desc->mDirection, desc->mIECDataType, mDeviceExecution, desc->mServiceConfigFB);
      break;
    default:
      DEVLOG_DEBUG("[CBacnetClientController] initHandle(): Unknown/Unsupported BACnet Service\n");
      return NULL;
      break;
  }

  mHandles->pushBack(handle); // update list of handles
  createNewAddrTableEntry(handle->mConfigFB->m_stServiceConfig->deviceID); // update addr table

  return handle;
}

void CBacnetClientController::createNewAddrTableEntry(const TForteUInt32 &paDeviceID){
  if(!isInAddrTable(paDeviceID)){
    mAddrTable->pushBack(new SBacnetAddressTableEntry(paDeviceID));
  }
}

bool CBacnetClientController::isInAddrTable(const TForteUInt32 &paDeviceID) {
  TAddrTable::Iterator itEnd = mAddrTable->end();
  for(TAddrTable::Iterator it = mAddrTable->begin(); it != itEnd; ++it){
    if((*it)->nDeviceID == paDeviceID) {
      return true;
    }
  }
  return false;
}

bool CBacnetClientController::getAddressByDeviceID(const TForteUInt32 &paDeviceID, struct in_addr *paDeviceAddr, TForteUInt16 *paDevicePort) {
  TAddrTable::Iterator itEnd = mAddrTable->end();
  for(TAddrTable::Iterator it = mAddrTable->begin(); it != itEnd; ++it){
    if((*it)->nDeviceID == paDeviceID && (*it)->stAddr.s_addr != 0) {
      if(paDeviceAddr != NULL && paDevicePort != NULL) {
        (*paDeviceAddr) = (*it)->stAddr;
        (*paDevicePort) = (*it)->nPort;
      }
      return true;
    }
  }
  return false;
}

BACNET_ADDRESS CBacnetClientController::ipToBacnetAddress(const struct in_addr &paAddress, const TForteUInt16 &paPort, const bool &paIsBroadcastAddr){
  // see libbacnet's bip_get_broadcast_address and bip_get_my_address 
  BACNET_ADDRESS retVal;
  retVal.mac_len = (TForteUInt8) (IP_ADDR_LEN+PORT_LEN);
  memcpy(&retVal.mac[0], &paAddress, IP_ADDR_LEN);
  memcpy(&retVal.mac[4], &paPort, PORT_LEN);
  retVal.net = paIsBroadcastAddr ? BACNET_BROADCAST_NETWORK : 0; // broadcast -> all 1s, all 0s otherwise
  retVal.len = 0; // no SLEN, local only, no routing
  memset(retVal.adr, 0, MAX_MAC_LEN); // no SADR -> local only, no routing
  return retVal;
}

void CBacnetClientController::runLoop() {
  while(isAlive()) {
    switch(m_enClientControllerState) {
      case Init:
        // do nothing
        // state change is performed by the client config fb upon the end of the initialization
        break;
      case AddressDiscovery:
        discoverNetworkAddresses();
        m_enClientControllerState = COVSubscription;
        break;
      case COVSubscription:
        subscribeToCOVNotifications();
        m_enClientControllerState = ConfigFBsNotification;
        break;
      case ConfigFBsNotification:
        notifyConfigFBs();
        m_enClientControllerState = NormalOperation;
        break;
      case NormalOperation:
        executeOperationCycle();
        break;
      default:
        break;
    }
  }
}

void CBacnetClientController::discoverNetworkAddresses() {
  TAddrTable::Iterator itEnd = mAddrTable->end();
  for(TAddrTable::Iterator it = mAddrTable->begin(); it != itEnd; ++it) {
    // send who-is request
    sendWhoIs((*it)->nDeviceID);
    // get current time
    timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    // wait until we receive an I-Am message (mAddr struct of the entry is not zero) or until a timeout (1s) occurs
    while((*it)->stAddr.s_addr == 0  && !timeoutMillis(scm_nRequestTimeout * scm_nMicrosInMillis, currentTime)){
      receiveIAm((*it)->nDeviceID);
    }
  }
}

void CBacnetClientController::sendWhoIs(const TForteUInt32 &paDeviceId){
  //get broadcast addr
  BACNET_ADDRESS broadcastAddr = ipToBacnetAddress(this->m_stConfig.stBroadcastAddr, this->m_stConfig.nPort, true);
  //get local addr
  BACNET_ADDRESS localAddr = ipToBacnetAddress(this->m_stConfig.stLocalAddr, this->m_stConfig.nPort, false);
  // encode npdu control data: no reply expected, normal priority
  BACNET_NPDU_DATA NPDUData;
  npdu_encode_npdu_data(&NPDUData, false, MESSAGE_PRIORITY_NORMAL);
  TForteUInt16 PDULen = BVLC_HEADER_LEN;
  PDULen = (TForteUInt16) (PDULen + npdu_encode_pdu(&mSendBuffer[PDULen], &broadcastAddr, &localAddr, &NPDUData));
  // encode apdu data
  PDULen = (TForteUInt16) (PDULen + whois_encode_apdu(&mSendBuffer[PDULen], paDeviceId, paDeviceId));
  // encode virtual link data
  mSendBuffer[BVLC_TYPE_BYTE] = BVLL_TYPE_BACNET_IP;
  mSendBuffer[BVLC_FUNCTION_BYTE] = BVLC_ORIGINAL_BROADCAST_NPDU;
  encode_unsigned16(&mSendBuffer[BVLC_LEN_BYTE], PDULen);
  // send packet
  DEVLOG_DEBUG("[CBacnetClientController] sendWhoIs(): Sending Who-Is request, Device ID=%d\n", paDeviceId);
  sendPacket(PDULen, this->m_stConfig.stBroadcastAddr, this->m_stConfig.nPort);
}

bool CBacnetClientController::timeoutMillis(const TForteUInt16 &paMillis, const timespec &paStartTime) {
  timespec currentTime;
  timespec delta;
  // get current time
  clock_gettime(CLOCK_MONOTONIC, &currentTime);
  // calculate time difference
  timespecSub(&currentTime, &paStartTime, &delta);
  // convert to milliseconds
  TForteUInt16 millisElapsed = (TForteUInt16) (round(delta.tv_nsec / scm_nNanosInMillis) + (TForteUInt16) (delta.tv_sec * scm_nMillisInS));

  return (millisElapsed > paMillis);
}

void CBacnetClientController::receiveIAm(const TForteUInt32 &paDeviceID){
  // TODO: code here is very similar to the code of decodeAndHandleReceivedPacket method.
  // The only difference is that the device id is passed from the sendWhoIs.
  // Any way to combine it? Probably not, because the device id is needed to match the sent who-is to received i-am.
  struct sockaddr_in soruceAddress;
  if(receivePacket(scm_nReceivePacketSelectTimeoutMillis , &soruceAddress) > 0){
    TForteUInt8 serviceChoice = 0;
    TForteUInt8 APDUType = 0;
    TForteUInt16 APDUOffset = 0;
    TForteUInt16 APDULen = 0;
    TForteUInt8 invokeID = 0;
    TForteUInt16 serviceRequestOffset = 0;
    if(decodeNPDU(APDUOffset, APDULen) && 
       decodeAPDU(APDUType, APDUOffset, invokeID, serviceChoice, serviceRequestOffset)  && 
       APDUType == PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST &&
       serviceChoice == SERVICE_UNCONFIRMED_I_AM) {
      handleIAm(paDeviceID, soruceAddress, APDUOffset);
    }
  }
}

void CBacnetClientController::handleIAm(const TForteUInt32 &paDeviceID, const struct sockaddr_in &paSourceAddress, const TForteUInt16 &paAPDUOffset) {
  TForteUInt32 deviceID;
  int len = iam_decode_service_request(&mReceiveBuffer[paAPDUOffset+UNCONFIRMED_REQ_APP_TAGS_OFFSET], &deviceID, NULL, NULL, NULL);
  if(len != -1 && deviceID == paDeviceID) {
    DEVLOG_DEBUG("[CBacnetClientController] handleIAm(): I-Am received, updating addr entry for Device %d, addr %s:%04X\n", deviceID, inet_ntoa(paSourceAddress.sin_addr), htons(paSourceAddress.sin_port));
    TAddrTable::Iterator itEnd = mAddrTable->end();
    for(TAddrTable::Iterator it = mAddrTable->begin(); it != itEnd; ++it){
       if((*it)->nDeviceID == deviceID) {
          (*it)->stAddr = paSourceAddress.sin_addr;
          (*it)->nPort = paSourceAddress.sin_port;
       }
    }
  }
}

void CBacnetClientController::subscribeToCOVNotifications() {
  THandlesList::Iterator itEnd = mHandles->end();
    for(THandlesList::Iterator it = mHandles->begin(); it != itEnd; ++it){
      if((*it)->m_enHandleType == CBacnetServiceHandle::UnconfirmedCOVServiceHandle ){
        // if the handle is of type UnconfirmedCOVServiceHandle , send a subscription service request
        CBacnetUnconfirmedCOVHandle *covHandle = static_cast<CBacnetUnconfirmedCOVHandle *>(*it);
        sendUnconfirmedCOVSubscribe(covHandle);
        // get current time
        timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        // wait until subscription is acknowledged (bAcknowledgedFlag of the handle's subscription data is set) or a timeout occurs (1s)
        while(!covHandle->m_stSubscriptionData.bAcknowledgedFlag && !timeoutMillis(scm_nRequestTimeout * scm_nMicrosInMillis, current_time)) {
          receiveCOVSubscriptionAck(covHandle);
        }
      }
    } 
}

void CBacnetClientController::sendUnconfirmedCOVSubscribe(CBacnetUnconfirmedCOVHandle *paHandle) {
  // get dest addr
  struct in_addr destinationAddr;
  uint16_t destinationPort;

  if(!getAddressByDeviceID(paHandle->mConfigFB->m_stServiceConfig->deviceID, &destinationAddr, &destinationPort))
    return; // address is not founds

  //encode npdu part: reply expected, normal priority + addresses
  BACNET_ADDRESS bacnetDestinationAddr = ipToBacnetAddress(destinationAddr, destinationPort, false);
  BACNET_ADDRESS bacnetLocalAddr = ipToBacnetAddress(this->m_stConfig.stLocalAddr, this->m_stConfig.nPort, false);
  BACNET_NPDU_DATA NPDUData;
  npdu_encode_npdu_data(&NPDUData, true, MESSAGE_PRIORITY_NORMAL);
  TForteUInt16 PDULen = BVLC_HEADER_LEN;
  PDULen = (TForteUInt16)(PDULen + npdu_encode_pdu(&mSendBuffer[PDULen], &bacnetDestinationAddr, &bacnetLocalAddr, &NPDUData));
  //encode apdu: target object, target object's ID, hardcoded process identifier, unconfirmed notifications, infinite lifetime
  BACNET_SUBSCRIBE_COV_DATA COVData;
  memset(&COVData, 0, sizeof(BACNET_SUBSCRIBE_COV_DATA));
  COVData.monitoredObjectIdentifier.type = paHandle->mConfigFB->m_stServiceConfig->objectType;
  COVData.monitoredObjectIdentifier.instance = paHandle->mConfigFB->m_stServiceConfig->objectID;
  COVData.subscriberProcessIdentifier = 1; // hardcoded process identifier
  COVData.issueConfirmedNotifications = false; // hardcoded
  COVData.lifetime = COV_INFINITE_LIFETIME; // hardcoded infinite lifetime
  // get next free invoke id, save it to handle's subsription data struct
  paHandle->m_stSubscriptionData.nSubscriptionInvokeID = this->m_nInvokeID++;
  // encode APDU part of the packet
  PDULen = (TForteUInt16)(PDULen + cov_subscribe_encode_apdu(&mSendBuffer[PDULen], (unsigned int) (sizeof(mSendBuffer) - PDULen), paHandle->m_stSubscriptionData.nSubscriptionInvokeID, &COVData));
  mSendBuffer[BVLC_TYPE_BYTE] = BVLL_TYPE_BACNET_IP;
  mSendBuffer[BVLC_FUNCTION_BYTE] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&mSendBuffer[BVLC_LEN_BYTE], PDULen);
  // send packet
  DEVLOG_DEBUG("[CBacnetClientController] sendUnconfirmedCOVSubscribe(): subscribing to DeviceID=%d, ObjectType=%d, ObjectID=%d\n", paHandle->mConfigFB->m_stServiceConfig->deviceID, paHandle->mConfigFB->m_stServiceConfig->objectType, paHandle->mConfigFB->m_stServiceConfig->objectID);
  sendPacket(PDULen, destinationAddr, destinationPort);
}

void CBacnetClientController::receiveCOVSubscriptionAck(CBacnetUnconfirmedCOVHandle *paHandle) {
  sockaddr_in src;
  if(receivePacket(scm_nReceivePacketSelectTimeoutMillis, &src) > 0){
    TForteUInt8 serviceChoice = 0;
    TForteUInt8 APDUType = 0;
    TForteUInt16 APDUOffset = 0;
    TForteUInt16 APDULen = 0;
    TForteUInt8 invokeID = 0;
    TForteUInt16 serviceRequestOffset = 0;
    if(decodeNPDU(APDUOffset, APDULen) && 
       decodeAPDU(APDUType, APDUOffset, invokeID, serviceChoice, serviceRequestOffset)  && 
       APDUType == PDU_TYPE_SIMPLE_ACK &&
       serviceChoice == SERVICE_CONFIRMED_SUBSCRIBE_COV) {
      handleCOVSubscriptionAck(paHandle, APDUOffset);
    } 
  }
}

void CBacnetClientController::handleCOVSubscriptionAck(CBacnetUnconfirmedCOVHandle *paHandle, TForteUInt16 paAPDUOffset) {
  // Multiple subscription to the same (Device, ObjectType, ObjectID) tuple are sent multiple times -- fix it?
  TForteUInt8 invokeID = mReceiveBuffer[paAPDUOffset+SIMPLE_ACK_INVOKE_ID_OFFSET];
  if(paHandle->m_stSubscriptionData.nSubscriptionInvokeID == invokeID){
    paHandle->m_stSubscriptionData.bAcknowledgedFlag = true;
    paHandle->m_enHandleState = CBacnetServiceHandle::AwaitingResponse; // change handle's state
    DEVLOG_DEBUG("[CBacnetClientController] handleCOVSubscriptionAck(): subscription acknowledged\n");
  }
}

void CBacnetClientController::notifyConfigFBs() {
  THandlesList::Iterator itEnd = mHandles->end();
  for(THandlesList::Iterator it = mHandles->begin(); it != itEnd; ++it){
    // address check
    if(!getAddressByDeviceID((*it)->mConfigFB->m_stServiceConfig->deviceID, NULL, NULL)) {
      // address not found (getAddressByDeviceID() returns 'false' if it equals zero) -> notify about addr fetch failure
      notifyConfigFB(CBacnetServiceConfigFB::AddrFetchFailed, (*it)->mConfigFB);
      continue;
    }
    // cov subscription acknowledged check
    if((*it)->m_enHandleType == CBacnetServiceHandle::UnconfirmedCOVServiceHandle ){
      CBacnetUnconfirmedCOVHandle *covHandle = static_cast<CBacnetUnconfirmedCOVHandle *>(*it);
      if(!covHandle->m_stSubscriptionData.bAcknowledgedFlag){
        // cov subscription wasn't acknowledged -> notify about subscription failure
        notifyConfigFB(CBacnetServiceConfigFB::COVSubscriptionFailed, (*it)->mConfigFB);
        continue;
      }
      // acknowledged -> push to the list of COV handles (needed for relating received COV notifications to proper handles)
      mCOVHandles->pushBack(covHandle);
    }
    // everything is good -> notify configuratuion fb about success
    notifyConfigFB(CBacnetServiceConfigFB::Success, (*it)->mConfigFB);
  }
  // clear and delete the handles list, not needed anymore
  mHandles->clearAll();
  delete mHandles;
  // delete COV handles list list if empty
  if(mCOVHandles->begin() == mCOVHandles->end())
    delete mCOVHandles; 
}

inline void CBacnetClientController::notifyConfigFB(CBacnetServiceConfigFB::EServiceConfigFBNotificationType paNotificationType, CBacnetServiceConfigFB *paConfigFB) {
  paConfigFB->setNotificationType(paNotificationType);
  startNewEventChain(paConfigFB);
}

void CBacnetClientController::executeOperationCycle() {
    // check if send RingBuffer is not empty
    // if(m_nSendRingBufferSize != 0) {
    if(m_stHandlesRingBuffer.nSize != 0) {
      // getch first handle from the rungbuffer,
      CBacnetServiceHandle *handle = consumeFromRingBuffer();
      // encode service request into the mSendBuffer bufffer
      TForteUInt8 invokeID;
      TForteUInt16 len;
      in_addr destinationAddr;
      TForteUInt16 destinationPort;
      encodeRequest(handle, invokeID, len, destinationAddr, destinationPort);
      // send it over the network
      if(sendPacket(len, destinationAddr, destinationPort) > 0) {
        // if sent succeeded, register new transaction with 1s deadline
        timespec deadline;
        clock_gettime(CLOCK_MONOTONIC, &deadline);
        deadline.tv_sec += scm_nRequestTimeout;
        registerTransaction(new STransactionListEntry(invokeID, handle, deadline));
      }
    }
    // try to receive an incoming packet for 100ms
    sockaddr_in src;
    if(receivePacket(scm_nReceivePacketSelectTimeoutMillis, &src) > 0) {
        // handle it in case of a success
        // decodeAndHandleReceivedPacket();
        decodeAndHandleReceivedPacket(src);
    }
    // check deadlines of active transactions
    checkTransactionsDeadlines();
}

CBacnetServiceHandle * CBacnetClientController::consumeFromRingBuffer(){
  CCriticalRegion criticalRegion(mRingBufferMutex);
  if(0 == m_stHandlesRingBuffer.nSize)
    return NULL; // return NULL if ring buffer is empty
  CBacnetServiceHandle *retVal = m_stHandlesRingBuffer.aRingBuffer[m_stHandlesRingBuffer.nTailIndex];
  m_stHandlesRingBuffer.nSize--;
  m_stHandlesRingBuffer.nTailIndex = (m_stHandlesRingBuffer.nTailIndex + 1) % scm_nRingBufferSize;
  return retVal;
}

void CBacnetClientController::encodeRequest(CBacnetServiceHandle *paHandle, TForteUInt8 &paInvokeID, TForteUInt16 &paRequestLen, in_addr &paDestinationAddr, TForteUInt16 &paDestinationPort){
  // get network address and convert it to the libbacnet's addr structure
  getAddressByDeviceID(paHandle->mConfigFB->m_stServiceConfig->deviceID, &paDestinationAddr, &paDestinationPort);
  BACNET_ADDRESS dest = ipToBacnetAddress(paDestinationAddr, paDestinationPort, false);
  BACNET_ADDRESS src = ipToBacnetAddress(m_stConfig.stLocalAddr, m_stConfig.nPort, false);
  // get next invoke id
  paInvokeID = m_nInvokeID++;
  // tell handle to encode the APDU part
  paRequestLen = (TForteUInt16) paHandle->encodeServiceReq(mSendBuffer, paInvokeID, &dest, &src);
}

inline void CBacnetClientController::registerTransaction(STransactionListEntry *paTransaction) {
  mActiveTransactions->pushBack(paTransaction);
}

inline void CBacnetClientController::deregisterTransaction(STransactionListEntry *paTransaction) {
  mActiveTransactions->erase(paTransaction);
}

void CBacnetClientController::decodeAndHandleReceivedPacket(sockaddr_in &paSourceAddress) {
    TForteUInt8 serviceChoice = 0; // see bacnetlib's BACNET_CONFIRMED_SERVICE and BACNET_UNCONFIRMED_SERVICE enums in .../include/bacenum.h 
    TForteUInt8 APDUType = 0; // see bacnetlib's BACNET_PDU_TYPE enum in .../include/bacenum.h
    TForteUInt16 APDUOffset = 0; 
    TForteUInt16 APDULen = 0;
    TForteUInt8 invokeID = 0;
    TForteUInt16 ServiceRequestOffset = 0;

    if(decodeNPDU(APDUOffset, APDULen) && decodeAPDU(APDUType, APDUOffset, invokeID, serviceChoice, ServiceRequestOffset)){
      handleAPDU(&mReceiveBuffer[APDUOffset], APDULen, APDUType, serviceChoice);
    }
}


bool CBacnetClientController::decodeNPDU(TForteUInt16 &paAPDUOffset, TForteUInt16 &paAPDULen){
  if (mReceiveBuffer[BVLC_TYPE_BYTE] == BVLL_TYPE_BACNET_IP) {
    /* Bacnet/IP signature check */
    TForteUInt16 NPDULen;
    (void) decode_unsigned16(&mReceiveBuffer[BVLC_LEN_BYTE], &NPDULen);
    if ((NPDULen >= BVLC_HEADER_LEN) && (NPDULen <= (MAX_MPDU-BVLC_HEADER_LEN))) {
      /* Length of NPDU is OK (including BVLC length) */
      if(mReceiveBuffer[BVLC_FUNCTION_BYTE] == BVLC_ORIGINAL_UNICAST_NPDU || mReceiveBuffer[BVLC_FUNCTION_BYTE] == BVLC_ORIGINAL_BROADCAST_NPDU) {
        /* For now only handle unicast/broadcast BVLC types */
        if (mReceiveBuffer[NPDU_OFFSET] == BACNET_PROTOCOL_VERSION) {
          /* Protocol version 0x01 = version used by libbacnet*/
          BACNET_NPDU_DATA NPDUData = { };
          BACNET_ADDRESS dest = { };
          paAPDUOffset = (TForteUInt16) npdu_decode(&mReceiveBuffer[NPDU_OFFSET], &dest, NULL, &NPDUData);
          if(!NPDUData.network_layer_message && (paAPDUOffset > 0) && (paAPDUOffset <= NPDULen - BVLC_HEADER_LEN) 
              && ((dest.net == 0) || (dest.net == BACNET_BROADCAST_NETWORK))) {
                /* NOT network layer message, APDU offset is in bounds, NOT routing infromation */
                // not network layer message -> APDU follows -> APDUOffset+=4, adds BVLC header length
                paAPDUOffset = (TForteUInt16)(paAPDUOffset+BVLC_HEADER_LEN);
                paAPDULen = (TForteUInt16) (NPDULen-paAPDUOffset);
                return true;
          }
        }
      }
    }
  }
  return false;  
}

bool CBacnetClientController::decodeAPDU(TForteUInt8 &paAPDUType, const TForteUInt16 &paAPDUOffset, TForteUInt8 &paInvokeID, TForteUInt8 &paServiceChoice, TForteUInt16 &paServiceRequestOffset) {
  paAPDUType = (mReceiveBuffer[paAPDUOffset] & APDU_TYPE_MASK);
  switch (paAPDUType)
  {
    case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
      // I-Am + UnconfirmedCOVNotification
      // APDU[0] & 0xF0 = APDU Type
      // APDU[1] = service choice
      // APDU[2..*] = application tags
      paServiceChoice = mReceiveBuffer[paAPDUOffset+UNCONFIRMED_REQ_SERVICE_CHOICE_OFFSET];
      paServiceRequestOffset = (TForteUInt16)(paAPDUOffset+UNCONFIRMED_REQ_APP_TAGS_OFFSET);
      return true;
      break;

    case PDU_TYPE_SIMPLE_ACK:
      // WriteProperty + SubscribeCOV acknowledge
      // APDU[0] & 0xF0 = APDU Type
      // APDU[1] = invoke id
      // APDU[2] = service choice
      paServiceChoice = mReceiveBuffer[paAPDUOffset+SIMPLE_ACK_SERVICE_CHOICE_OFFSET];
      paInvokeID = mReceiveBuffer[paAPDUOffset+SIMPLE_COMPLEX_ACK_INVOKE_ID_OFFSET];
      return true;
      break;

    case PDU_TYPE_COMPLEX_ACK:
      // ReadProperty acknowledge and result
      // APDU[0] & 0xF0 = APDU Type
      // APDU[0] & 0x0F = PDU flags (3d bit - segmented message, 2d bit - more segments to follow)
      // APDU[1] = invoke id
      // ADPU[2] = service choice
      // APDU[3..*] = context+application tags  
      if( !(mReceiveBuffer[paAPDUOffset] & COMPLEX_ACK_NPDU_SEGMENTED_MASK) ) {
        /* NOT segmented */
        paServiceChoice = mReceiveBuffer[paAPDUOffset+COMPLEX_ACK_SERVICE_CHOICE_OFFSET];
        paInvokeID = mReceiveBuffer[paAPDUOffset+SIMPLE_COMPLEX_ACK_INVOKE_ID_OFFSET];
        paServiceRequestOffset = (TForteUInt16)(paAPDUOffset+COMPLEX_ACK_APP_TAGS_OFFSET);
        return true;
      }
      break;
    default:
      return false;
      break;
  }
  return false;    
}


void CBacnetClientController::checkTransactionsDeadlines() {
  timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  TTransactionList::Iterator it = mActiveTransactions->begin();
  while(it != mActiveTransactions->end()){
    // work with a copy to avoid dangling pointer in case of an erase
    TTransactionList::Iterator _it = it; 
    ++it;
    //check the deadline, notify the handle and deregister the transaction in case of a missed deadline
    if(timespecLessThan(&((*_it)->deadline), &now)){
      DEVLOG_DEBUG("[CBacnetClientController] checkTransactionsDeadlines(): deadline missed, invokeid=%d\n", (*_it)->invokeID);
      (*_it)->handle->missedTransactionDeadline();
      deregisterTransaction((*_it));
    }
  }
}


void CBacnetClientController::handleAPDU(TForteUInt8 *paAPDU, const TForteUInt16 &paAPDULen, const TForteUInt8 &paAPDUType, const TForteUInt8 &paServiceChoice) {
  switch (paAPDUType)
  {
    case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
     if (paServiceChoice == SERVICE_UNCONFIRMED_COV_NOTIFICATION) {
        handleUnconfirmedCOVNotifation(paAPDU, paAPDULen);
      }
      break;
    
    case PDU_TYPE_COMPLEX_ACK:
    case PDU_TYPE_SIMPLE_ACK:
      if(paServiceChoice == SERVICE_CONFIRMED_READ_PROPERTY ||
         paServiceChoice == SERVICE_CONFIRMED_WRITE_PROPERTY) {
          handleServiceAck(paAPDU, paAPDULen);
      }
      break;
    default:
      break;
  }
}

void CBacnetClientController::handleServiceAck(TForteUInt8 *paAPDU, const TForteUInt16 &paADPULen) {
  TTransactionList::Iterator it = mActiveTransactions->begin();
  while(it != mActiveTransactions->end()){
    // work with a copy to avoid dangling pointer in case of an erase
    TTransactionList::Iterator _it = it; 
    ++it;
    // decode response and deregister transaction
    if((*_it)->invokeID == paAPDU[SIMPLE_COMPLEX_ACK_INVOKE_ID_OFFSET]){
      (*_it)->handle->decodeServiceResp(paAPDU, paADPULen);
      deregisterTransaction((*_it));
    }
  }
}

void CBacnetClientController::handleUnconfirmedCOVNotifation(TForteUInt8 *paAPDU, const TForteUInt16 &paADPULen) {
  /*Analog+Binary Input/Output/Value return a list of values in CoV notification, 
    containig two entries - Present Value and Status flag. See Clause 13.1, Table 13-1*/
  
  // create this list
  BACNET_PROPERTY_VALUE propertyValue[2];
  propertyValue[0].next = &propertyValue[1];
  BACNET_COV_DATA COVData;
  COVData.listOfValues = propertyValue;
  // decode the APDU into the COVData variable
  cov_notify_decode_service_request(&paAPDU[UNCONFIRMED_REQ_APP_TAGS_OFFSET], paADPULen-UNCONFIRMED_REQ_APP_TAGS_OFFSET, &COVData); // TODO: check if something was actually decoded
  // find corresponding handle in the cov handles list
  TCOVHandlesList::Iterator itEnd = mCOVHandles->end();
  for(TCOVHandlesList::Iterator it = mCOVHandles->begin(); it != itEnd; ++it){
    if((*it)->mConfigFB->m_stServiceConfig->deviceID == COVData.initiatingDeviceIdentifier &&
        (*it)->mConfigFB->m_stServiceConfig->objectType == COVData.monitoredObjectIdentifier.type &&
        (*it)->mConfigFB->m_stServiceConfig->objectID == COVData.monitoredObjectIdentifier.instance) {
          (*it)->notificationReceived(propertyValue[0]); 
    }
  }
}





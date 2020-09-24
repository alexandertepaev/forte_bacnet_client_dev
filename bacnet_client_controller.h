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
#ifndef _BACNET_CLIENT_CONTROLLER_H_
#define _BACNET_CLIENT_CONTROLLER_H_


#include <core/io/device/io_controller.h>
#include <arch/utils/timespec_utils.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <math.h>

#include "forte_bacnet.h"
#include "bacnet_service_config_fb.h"



class CBacnetServiceHandle;
class CBacnetServiceConfigFB;
class CBacnetSubscribeUnconfirmedCOVConfigFB;
class CBacnetUnconfirmedCOVHandle;


/*! @brief BACnet Client Controller

 * This class represents BACnet Client Controller, whose main objective is to run a separate execution thread.
 * Client controller is responsible for the communication over the underlying BACnet/IP network.
 * 
 * Features:
 *    + Created and maintained by the client configuration fb upon the initalization.
 *    + Creates and maintains BACnet service handles (see initHandles).
 *    + Has 5 states: Initialization state, Address discovery state, COV subscription state, Configuration FBs notification state, Normal Operation state (see m_enClientControllerState member).
 *    + Runs an infinite loop in another thread, while the thread is alive (see runLoop method).
 *    + Performs 5 different tasks depending on its current state.
 */
class CBacnetClientController: public forte::core::io::IODeviceController {
  friend class CBacnetClientConfigFB;
  friend class CBacnetReadPropertyConfigFB;
  friend class CBacnetWritePropertyConfigFB;
  friend class CBacnetSubscribeUnconfirmedCOVConfigFB;
public:
  CBacnetClientController(CDeviceExecution& paDeviceExecution, TForteUInt16 paControllerID);
  ~CBacnetClientController();


  /*! @brief Descriptor of service handles
   *
   * This class is used to exchange information about handles between the BACnet client controller and the corresponding service configuration fb.
   * A descriptor instance is passed to the initHandle method, in which a specific instance of the CBacnetServiceHandle base class is created.
   */
  class HandleDescriptor : public forte::core::io::IODeviceController::HandleDescriptor {
    public:
      CBacnetServiceConfigFB *mServiceConfigFB;
      int mServiceType;
      CIEC_ANY::EDataTypeID mIECDataType;

      HandleDescriptor(CIEC_WSTRING const &paId, forte::core::io::IOMapper::Direction paDirection, int paServiceType, CIEC_ANY::EDataTypeID paIECDataType, CBacnetServiceConfigFB *paServiceConfigFB) : forte::core::io::IODeviceController::HandleDescriptor(paId, paDirection), mServiceConfigFB(paServiceConfigFB), mServiceType(paServiceType), mIECDataType(paIECDataType) {
      }
  };

  /*! @brief Pushes a service handle into the ring buffer
   * 
   * This method is called each time a handle is asked to send out a service request.
   * 
   * @param paHandle Pointer to to-be-pushed handle
   * @return 'true' in case of success, 'false' otherwise
   */
  bool pushToRingBuffer(CBacnetServiceHandle *paHandle);


  //! Returns ID on the client controller instance
  TForteUInt16 getControllerID() {
    return m_nID;
  }
  
protected:

private:

  //! Name of the BACnet/IP network interface (e.g, see ifconfig command)
  constexpr static const char * const scm_sNetwrokIfaceName = "wlan0";
  //! Size of the handles ring buffer (m_stHandlesRingBuffer)
  constexpr static TForteUInt8 scm_nRingBufferSize = 64;
  //! Timeout, after which select system call unblocks execution (see receivePacket)
  constexpr static TForteUInt8 scm_nReceivePacketSelectTimeoutMillis = 100;
  //! Timeout in milliseconds, after which a BACnet service request is considered to be not 
  //  acknowledged/not responded to (see discoverNetworkAddresses, subscribeToCOVNotifications, executeOperationCycle) 
  static constexpr TForteUInt32 scm_nRequestTimeout = 1000; //FIXME: Throws 'undifined reference to ...' when declared as TForteUInt16 
  
  //! constants for timeout/deadlines calculations
  constexpr static TForteUInt16 scm_nMicrosInMillis = 1000;
  constexpr static TForteUInt16 scm_nMillisInS = 1000;
  constexpr static TForteUInt32 scm_nNanosInMillis = 1.0e6;

  //!< Socket initialization failed message
  constexpr static const char* const scmSocketInitFailed = "Communication socket initialization failed";; 

  //! Unique ID of the client controller instance
  TForteUInt16 m_nID;

  //! States of the client controller
  enum EBacnetClientControllerState {
    Init,
    AddressDiscovery,
    COVSubscription, 
    ConfigFBsNotification,
    NormalOperation
  };
  EBacnetClientControllerState m_enClientControllerState;

  //! Controller's configuration struct
  struct SBacnetClientControllerConfig : forte::core::io::IODeviceController::Config {
    TForteUInt16 nPort;
    struct in_addr stLocalAddr;
    struct in_addr stBroadcastAddr;
  };
  //! Controller's configuration struct member, initialized in setConfig
  struct SBacnetClientControllerConfig m_stConfig;

  typedef int TSocketDescriptor; 
  TSocketDescriptor mCommunicationSocket; //!< Socket descriptor
  
  /*! Intiger indicating next free invoke ID required for BACnet service requests
   *  Incremented after each use.
   *  TODO: what if all of the invoke ids (255) are currently in use? -> bool array with 255 entries, indicating if an invoke id is in use
   */    
  TForteUInt8 m_nInvokeID;
  
  TForteUInt8 mSendBuffer[MAX_MPDU]; //!< Buffer for outgoing packets
  TForteUInt8 mReceiveBuffer[MAX_MPDU]; //!< Buffer for incoming packets

  typedef CSinglyLinkedList<CBacnetServiceHandle *> THandlesList;
  THandlesList *mHandles; //!< Linked list of service handles

  typedef CSinglyLinkedList<CBacnetUnconfirmedCOVHandle *> TCOVHandlesList;
  TCOVHandlesList *mCOVHandles; //!< Linked list of unconfirmed COV service handles


  //! Address table entry struct
  struct SBacnetAddressTableEntry {
    TForteUInt32 nDeviceID; //!< Device ID of the remote BACnet device
    struct in_addr stAddr; //!< IPv4 address of the remote BACnet device
    TForteUInt16 nPort; //!< IPv4 port number of the remote BACnet device

    SBacnetAddressTableEntry(TForteUInt32 paDeviceID) : nDeviceID(paDeviceID), nPort(0) {
      memset(&stAddr, 0, sizeof(struct in_addr));
    }
  };
  typedef CSinglyLinkedList<SBacnetAddressTableEntry *> TAddrTable;
  TAddrTable *mAddrTable; //!< Linked list representing address table

  /*! @brief Ring buffer of pointers to handle instances
   *
   * This struct and its instance (m_stHandlesRingBuffer) represents a ring buffer data structure,
   * that holds pointers to handles wanting to send a BACnet service request. 
   * Main purpose of this ring buffer is to delegate the task of encoding and sending of a service requests to client controller's execution thread. 
   * Whenever a handle wants to send a request, it pushes itself to this ring buffer (see pushToRingBuffer method).
   * In the executeOperationCycle method, client controller consumes the handle from the ring buffer (see consumeFromRingBuffer method).
   *  Size of the ring buffer is defined in the HANDLES_RING_BUFFER_SIZE macro.
   */
  struct SHandlesRingBuffer {
    CBacnetServiceHandle *aRingBuffer[scm_nRingBufferSize];
    int nHeadIndex;
    int nTailIndex;
    int nSize;
  };
  SHandlesRingBuffer m_stHandlesRingBuffer;
  //! Ring buffer mutex lock, needed because handles and client conroller change the state of the ring buffer
  CSyncObject mRingBufferMutex;

  //! Transaction list entry struct
  struct STransactionListEntry {
    TForteUInt8 invokeID; //!< invoke id of the service request
    CBacnetServiceHandle * handle; //!< handle which initiated the service request
    timespec deadline; //!< deadline of the transaction

    STransactionListEntry(uint8_t paInvokeID, CBacnetServiceHandle *paHandle, timespec paDeadline) : \
      invokeID(paInvokeID), handle(paHandle), deadline(paDeadline) { }
  };
  typedef CSinglyLinkedList<STransactionListEntry *> TTransactionList;
  TTransactionList *mActiveTransactions; //!< Linked list representing active transactions


  /*! @brief Sets configuration struct
   * 
   * Sets the m_stConfig member variable and initializes its internal structs with network addresses by calling setNetworkAddresses() method
   * 
   * @param paConfig pointer to the configuration struct 
   */
  void setConfig(Config *paConfig);

  /*! @brief Queries network addresses of the local network interface and sets the corresponding fields of the m_stConfig struct
   *
   *  Gets the local address and the subnet mask using the ioctl() system call, calculates the broadcast address and sets stLocalAddr and stBroadcastAddr fields of the m_stConfig struct.
   *  Attention - name of the used network interface should be provided via scm_sNetwrokIfaceName.
   * // FIXME: error checking
   */
  void setNetworkAddresses();

   /*! @brief Performs initialization of the client controller
   *
   * In this method the initialization of the client controller is performed. 
   * In particular, it creates and configures a network socket by calling the createCommunicationSocket method.
   *  
   * @return 0 in case of a success, error message in case of a failure
   */
  const char* init(); // Initialize the device object (call it's init function)


  /*! @brief Performs deinitialization of the client controller 
   *
   * In this method the initialization of the client controller is performed by closing the communication socket.
   */
  void deInit();

  /*! @brief Initializes communication socket
   *
   * Creates, configures and binds a UDP socket used for BACnet/IP communication.
   * Initialized socket can be accessed via mCommunicationSocket member variable.
   * 
   *  @return 'true' in case of a success, 'false' in case of a failure
   */
  bool initCommunicationSocket();
  
  /*! @brief Send the data contained in the mSendBuffer over the network
   * 
   * This method is used for sending out a service request that is encoded in the mSendBuffer member 
   * over the mCommunicationSocket socket. Uses sendto() system call. 
   * 
   * @param len Length of the to-be-sent packet
   * @param destinationAddr Struct with destination address
   * @param destinationPort Integer indicating destination port
   * @return Number of sent bytes
   */
  int sendPacket(const TForteUInt16 &len, const struct in_addr &destinationAddr, const TForteUInt16 &destinationPort);

  /*! @brief Tries to receive a packet in a non-blocking fashion
   *
   * Performs a non-blocking receive over the mCommunicationSocket socket using select() and recvfrom() system calls.
   * Saves the received data into mReceiveBuffer buffer and stores the source address into the provided src address struct
   * 
   * @param timeout Amount of milliseconds, during which select() checks if the socket fd is ready to receive a packet
   * @param src Address sctruct in which the source address of the received packet is saved
   * @return Number received bytes, or 0 if own packet is received (broadcast case), or -1 if nothing 
   */
  int receivePacket(const TForteUInt16 &paTimeout, sockaddr_in *paSourceAddress);
  
  /*! @brief Initializes BACnet service handles
   *
   * This method gets called when a configuration fb adds a new handle via the addHandle method of the client controller.
   * In this method, a specific instance of the CBacnetServiceHandle is instantiated and returned. 
   * In addition to that, this instance is added to the list of handles (mHandles) and a new address entry is created in the address table by calling the createNewAddrTableEntry table method.
   * 
   * @param handleDescriptor Service handle descriptor
   * @return Pointer to the instantiated handle or NULL in case a handle could not be instantiated 
   */
  forte::core::io::IOHandle* initHandle(IODeviceController::HandleDescriptor *paHandleDescriptor);

  /*! @brief Creates a new entry in the address table  
   *
   * This method is used for updating the address table of the BACnet client controller. 
   * Address table is updated only in case there is no other entry with the same device id found in the table. (calls isInAddrTable)
   * 
   * @param paDeviceID ID of the remote device's Device object
   */
  void createNewAddrTableEntry(const TForteUInt32 &paDeviceID);

  /*! @brief Checks if address table holds a duplicated entry
   *
   * This method is used to tell if the client controller's address table already contains an entry with the provided device id
   * by iterating over the whole table and comparing the device ids of existing entries.
   * 
   * @param paDeviceID ID of the remote device's Device object
   * @return 'true' if a duplicate is found, 'false' otherwise
   */
  bool isInAddrTable(const TForteUInt32 &paDeviceID);

  /*! @brief Looks up for a remote device's address in the address table based on the provided Device ID 
   *
   * In this method a remotes device's address look up is performed by iterating over the address table and searching
   * for an entry with the provided device id. 
   * Can be used for checking if address is known or not.
   * 
   * @param paDeviceID ID of the remote device's Device object
   * @param paDeviceAddr Pointer to address struct, into which found address is to be saved
   * @param paDeviceAddrPort Pointer to an integer, into which found port number is to be saved
   * @return 'true' if the address is found, 'false' otherwise
   */
  bool getAddressByDeviceID(const TForteUInt32 &paDeviceID, struct in_addr *paDeviceAddr, TForteUInt16 *paDevicePort);
  
  /*! @brief Perform address conversion to the format required by libbacnet
   *
   * @param paDeviceAddr Address struct that has to be converted
   * @param paPort Port number
   * @param paBroadcastAddr Boolean parameter, telling if the address is a broadcast address (true if so, false otherwise)
   * @return BACNET_ADDRESS struct provided by libbacnet
   */
  BACNET_ADDRESS ipToBacnetAddress(const struct in_addr &paAddress, const TForteUInt16 &paPort, const bool &paIsBroadcastAddr);

  /*! @brief Represents the main loop of the client controller
   *
   * This method represents the loop which is executed by the client controller thread
   * Inside the loop, five different actions are performed based on the current state of the client controller
   * Init - nothing
   * AddressDiscovery - discoverNetworkAddresses()
   * COVSubscription - subscribeToCOVNotifications()
   * ConfigFBsNotification - notifyConfigFBs()
   * NormalOperation - executeOperationCycle()
   */
  void runLoop();

  /*! @brief Performs the BACnet address discrovery routine
   * 
   * In this method BACnet's WhoIs service requests are issued and corresponding IAm messages are handled.
   * This is achieved by iteratating over the address table and sending a WhoIs (see sendWhoIs() method) message for each address entry,
   * and waiting for an IAm response (see receiveIAm() method) or a timeout (1s).
   */  
  void discoverNetworkAddresses();

    /*! @brief Checks for a timeout condition 
   *
   * This method is used for checking for a timeout condition by calculating and evaluating the difference between two points in time
   * Uses FORTE's timespecSub method  
   * 
   * @param paMillis Amount of milliseconds after which a timeout should be triggered
   * @param paStartTime Struct containing a point in time, starting from which the timeout condition is calculated
   * @return 'true' in case of a timeout, 'false' otherwise
   */
  bool timeoutMillis(const TForteUInt16 &paMillis, const timespec &paStartTime);

  /*! @brief Sends a WhoIs service request
  *
  * In this method BACnet Who-Is service request is encoded and sent.
  * Uses methods, structures and enums provided by libbacnet (BACNET_NPDU_DATA, npdu_encode_npdu_data, npdu_encode_pdu, whois_encode_apdu, encode_unsigned16).
  * Send if performed by calling the sendPacket() method.
  * 
  * @param paDeviceID ID of the remote device's Device object
  */
  void sendWhoIs(const TForteUInt32 &paDeviceID);

  /*! @brief Receives an IAm service request
   *
   * In this method IAm service request is received, decoded and handled.
   * Receiving is done by calling receivePacket() method.
   * Decoding - by calling decodeAPDUData().
   * Handling - by calling handleIAm().
   * 
   * @param paDeviceID ID of the remote device's Device object
   */
  void receiveIAm(const TForteUInt32 &paDeviceId);

  /*! @brief Handles IAm service request.
   * 
   * In this method the APDU part of the received I-Am service request is decoded and handled.
   * Decoding of the APDU part of the I-Am is done by calling libbacnet's iam_decode_service_request() function
   * Handling is done by searching for the corresponding address entry in the address table and updating its address struct and port number 
   * 
   * @param paDeviceID ID of the remote device's Device object
   * @param paSourceAddress Struct containing address of the remote device
   * @param paAPDUOffset Integer indicating a distance to the start of the APDU portion of PDU
   */
  void handleIAm(const TForteUInt32 &paDeviceID, const struct sockaddr_in &paSourceAddress, const TForteUInt16 &paAPDUOffset);

  /*! @brief Performs COV subscription 
   *  
   * In this method unconfirmed COV subscription service requests are sent out.
   * Iterates over the whole list of handles, encodes and sends requests upon bumping into handles of type e_UnconfirmedCOVServiceHandle (see sendUnconfirmedCOVSubscribe method)
   * Tries to receive and handle an acknowledgement for 1 second of time (see receiveCOVSubscriptionAck() method)
   */
  void subscribeToCOVNotifications();
  
  /*! @brief Sends an unconfirmed COV subscription service request
   *
   * In this method BACnet unconfirmed COVSubscribe service request is encoded and sent.
   * Uses methods, structures and enums provided by libbacnet (BACNET_NPDU_DATA, npdu_encode_npdu_data, npdu_encode_pdu, BACNET_SUBSCRIBE_COV_DATA, cov_subscribe_encode_apdu, encode_unsigned16)
   * Sending is performed by calling the sendPacket() method.
   * 
   * @param paHandle Handle of the COV subscription config FB
   */
  void sendUnconfirmedCOVSubscribe(CBacnetUnconfirmedCOVHandle *paHandle);
  
  /*! @brief Receives a COV subscription acknowledgement
   *
   * In this method an acknowledgment message for the previously sent COV subscription request is received and decoded.
   * Receiving is done by calling receivePacket() method.
   * Decoding - by calling decodeAPDUData().
   * Handling - by calling handleCOVSubscriptionAck().
   * 
   * @param paHandle Handle of the COV subscription config FB
   */
  void receiveCOVSubscriptionAck(CBacnetUnconfirmedCOVHandle *paHandle);
  
  /*! @brief Handles COV subscription acknowledgement
   *
   * In this method the acknowledgment of the previously sent COVSubscribe service request is handled.
   * After getting the invoke id of the received acknowledgement, it is compared to the invoke id saved in the handle's subscription data struct.
   * If the invoke ids are equal, handle's bAcknowledgedFlag is set to 'true' and its state is changed to e_AwaitingResponse 
   * 
   * @param paHandle Handle of the COV subscription config FB
   * @param paAPDUOffset Integer indicating a distance to the start of the APDU portion of PDU
   */
  void handleCOVSubscriptionAck(CBacnetUnconfirmedCOVHandle *paHandle, TForteUInt16 paAPDUOffset);


  /*! @brief Notifies service configuration fbs about the result of the initialization phase (address discovery and COV subsription)
   *
   * Iterates over the whole list of handles and notifies corresponding cofiguration
   * FBs about the results of address discovery and COV subsription phases. (see notifyConfigFB() method)
   * In case of Uncofirmed COV service handles, if both address discovery and cov subscription succeed, pushes the handle to the mCOVHandles list.
   * Clears and deletes pmHandles list at the very end.
   * 
   */
  void notifyConfigFBs();

  /*! @brief Notifies given configuration fb by triggering its external event
   * 
   * This method is used in to set the notification type of the provided configuration fb and to trigger the 
   * execution of the external event of this fb by calling the startNewEventChain() method
   * 
   * @param paNotificationType Notification type
   * @param paConfigFB Configuration FB to be notified
   */
  inline void notifyConfigFB(CBacnetServiceConfigFB::EServiceConfigFBNotificationType paNotificationType, CBacnetServiceConfigFB *paConfigFB);
  
  /*! @brief Executes a series of methods during the normal operation state of the controller
   *
   * This method is executed during the NormalOperation state of the controller.
   * Its execution consists of the following steps: 
   *    1) Check the size of the ring buffer with handles wanting to send a request
   *      1.1) If the size is not zero, get the handle, encode and send a packet + register the transaction into the list of active transaction
   *    2) Try to receive a packet, and, in case a packet is received, decode and handle it
   *    3) Check the deadlines of the active transactions
   */
  void executeOperationCycle();

  /*! @brief Consumes ring buffer's entry
   *
   * In this methos the current size of the ring buffer is checked, and if the buffer is not empty, a pointer
   * to the instance the buffer's tail is pointing to is returned.
   * 
   * @return Pointer to the CBacnetServiceHandle instance, or NULL if the ring buffer is empty
   */
  CBacnetServiceHandle *consumeFromRingBuffer();

  /*! @brief Encodes a service request
   *
   *  In this method a service request is encoded into the buffer for outgoing packets (mSendBuffer).
   *  This is done by calling handle's encodeServiceReq method and passing the buffer as an argument.
   *  
   *  Attention: Parameters paInvokeID, paRequestLen, paDestinationAddr, paDestinationPort are used as 'return' parameters -> passed by their references
   * 
   * @param paHandle Handle that was used to trigger the send operation
   * @param paInvokeID Invoke ID of the to-be-encoded request
   * @param paRequestLen Lenght of the encoded request
   * @param paDestinationAddr Request's destination address
   * @param paDestinationPort Request's destination port
   */
  void encodeRequest(CBacnetServiceHandle *paHandle, TForteUInt8 &paInvokeID, TForteUInt16 &paRequestLen, in_addr &paDestinationAddr, TForteUInt16 &paDestinationPort);

  /*! @brief Adds a new entry into the list of active transactions
   *
   * This method adds a new entry to the list of active transactions (mActiveTransactions) and is used 
   * every time a new service request is sent. 
   * 
   * @param paTransaction Transaction information struct
   */
  inline void registerTransaction(STransactionListEntry *paTransaction);

  /*! @brief Checks deadlines of active transaction and deregisters them in case a deadline is missed
   *
   * In this method all the transaction found in the mActiveTransactions list are checked for a deadline miss.
   * In case a deadline is missed, transaction's handle gets notified and the transaction is deregistered (erased) from the active transactions list.
   */
  void checkTransactionsDeadlines();

  /*! @brief Decodes and handles received BACnet packet
   * 
   * In this method a packet stored in mReceiveBuffer buffer is decoded and handled.
   * Decoding is done by calling decodeAPDUData method.
   * Handling by calling handleAPDU method.
   * 
   * @param paSourceAddress Reference to struct containing source address of the request
   */
  void decodeAndHandleReceivedPacket(sockaddr_in &paSourceAddress);

  /*! @brief Decodes NPDU portion of a packet and returns APDU offset and APDU length 
   *  
   * In this method NPDU portion of a received BACnet request/response is decoded and the provided
   * references to APDU offset and APDU length variables are set. Called in combination with
   * decodeAPDU() method.
   * 
   * @param paAPDUOffset Reference to the variable indicating the offset to the APDU portion
   * @param paAPDULen Reference to the variable indication length of the APDU portion
   * @return 'true' in case of a success, 'false' otherwise
   *  
   */
  bool decodeNPDU(TForteUInt16 &paAPDUOffset, TForteUInt16 &paAPDULen);

  /*! @brief Decodes APDU portion of a packet and returns InvokeID, ServiceChoice and ServiceRequestOffset
   *
   * In this method APDU portion of a received BACnet request is decoded and the provided 
   * refererences to invokeID, serviceChoice and ServiceRequestOffset variables are set.
   * Called in combination with decodeNPDU method (decodeNPDU must be called first).
   * 
   * @param paAPDUType Reference to an integer variable indicating type of the APDU (see bacnetlib's BACNET_PDU_TYPE enum in .../include/bacenum.h)
   * @param paAPDUOffset Reference to the variable indicating the offset to the APDU portion
   * @param paInvokeID Reference to the variable indicating invoke id of the received request
   * @param paServiceChoice Reference to the variable indication service choice of the received packet
   * @param paServiceRequestOffset Reference to the variable indicating the offset service request portion
   * @return 'true' in case of a success, 'false' otherwise
   *  
   */
  bool decodeAPDU(TForteUInt8 &paAPDUType, const TForteUInt16 &paAPDUOffset, TForteUInt8 &paInvokeID, TForteUInt8 &paServiceChoice, TForteUInt16 &paServiceRequestOffset);

  /*! @brief Deletes an entry from the list of active transactions 
   *
   * This method deletes an entry form the list of active transactions (mActiveTransactions) and is used 
   * every time a request response is received or a transaction has missed its deadline.
   * 
   * @param paTransaction Transaction information struct
   */
  inline void deregisterTransaction(STransactionListEntry *paTransaction);

  /*! @brief Handles decoded APDU part of the incoming service request
   *
   * In this method service requests are handled.
   * Depending on the provided information (APDU type and service choice), different methods are called.
   * Note: At the moment ony Unconfirmed COV notifiactions (handleUnconfirmedCOVNotifation) and complex/simple acknowldedgements (handleServiceAck) are supported
   * 
   * @param paAPDU Pointer to the start of the APDU part of the received packet
   * @param paAPDULen Length of the APDU
   * @param paAPDUType Type of the APDU
   * @param paServiceChoice Type of the requested service
   */
  void handleAPDU(TForteUInt8 *paAPDU, const TForteUInt16 &paAPDULen, const TForteUInt8 &paAPDUType, const TForteUInt8 &paServiceChoice);
  

  /*! @brief Handles simple and complex acknowledgments
   *
   * This method is used to handle simple and complex acknowledgements by iterating over the
   * list of active transactions and comparing their invoke ids with the invoke id of the received acknowledgement.
   * Upon finding an active transaction with the required invoke id, it calls the decodeServiceResp method of the corresponding CBacnetServiceHandle
   * handle and deregisters the transaction by calling deregisterTransaction.
   *
   * @param paAPDU Pointer to the start of the APDU part of the received packet
   * @param paADPULen Length of the APDU
   */
  void handleServiceAck(TForteUInt8 *paAPDU, const TForteUInt16 &paADPULen);


  /*! @brief Handles unconfirmed COV notification
   *
   * This method is used to handle unconfirmed COV notifications by decoding the APDU part of the notification
   * and extracting the Device ID of the reporting device and the Object Type as well as Object ID of the object whose property was changed.
   * After finding the corresponding entry in the list of COV handles (mCOVHandles), handle's notificationReceived method gets called.
   * For the decoding of the APDUs uses libbacnet's cov_notify_decode_service_request function, and BACNET_PROPERTY_VALUE and BACNET_COV_DATA structs. 
   * 
   * 
   * @param paAPDU Pointer to the start of the APDU part of the received packet
   * @param paADPULen Length of the APDU
   */
  void handleUnconfirmedCOVNotifation(TForteUInt8 *paAPDU, const TForteUInt16 &paADPULen);

};
#endif
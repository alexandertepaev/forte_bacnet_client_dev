#include "bacnet_readproperty_handle.h"

CBacnetReadPropertyHandle::CBacnetReadPropertyHandle(forte::core::io::IODeviceController *controller, forte::core::io::IOMapper::Direction direction, CIEC_ANY::EDataTypeID type, CDeviceExecution& paDeviceExecution, CBacnetServiceConfigFB *paServiceConfigFB) : CBacnetServiceHandle(controller, direction, type, paDeviceExecution), mConfigFB(paServiceConfigFB)
{
  CBacnetReadPropertyConfigFB::ServiceConfig mServiceConfig = static_cast<CBacnetReadPropertyConfigFB *>(paServiceConfigFB)->m_stServiceConfig;
  DEVLOG_DEBUG("[CBacnetReadPropertyHandle] CBacnetReadPropertyHandle(): Initializing ReadProperty Handle with params: DeviceId=%d, ObjectType=%d, ObjectId=%d ObjectProperty=%d ArrayIndex=%d, dummy_value=%d\n",mServiceConfig.mDeviceId, mServiceConfig.mObjectType, mServiceConfig.mObjectId, mServiceConfig.mObjectProperty, mServiceConfig.mArrayIndex, mServiceConfig.dummy_value);

  //TODO: check if we know the address of the device. (controller->checkAddr(...)) If we don't know it, construct WHO-IS pdu and send it.

  // uint8_t PDU_BUFFER[MAX_PDU];

  // BACNET_NPDU_DATA npdu_data;
  // npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);

  // BACNET_ADDRESS dst;
  // dst.mac_len = 6;
  // dst.mac[0] = 192;
  // dst.mac[1] = 168;
  // dst.mac[2] = 1;
  // dst.mac[3] = 1;
  // dst.mac[4] = 0xBA;
  // dst.mac[5] = 0xC0;
  // dst.net = 0;
  // dst.len = 0;

  // BACNET_ADDRESS my_adr;
  // my_adr.mac_len = 6;
  // my_adr.mac[0] = 192;
  // my_adr.mac[1] = 168;
  // my_adr.mac[2] = 1;
  // my_adr.mac[3] = 0;
  // my_adr.mac[4] = 0xBA;
  // my_adr.mac[5] = 0xC0;
  // my_adr.net = 0;
  // my_adr.len = 0;

  // BACNET_ADDRESS *dest = &dst;
  // BACNET_ADDRESS my_address = my_adr;

  // int pdu_len = 0;
  // pdu_len = npdu_encode_pdu(&PDU_BUFFER[0], dest, &my_address, &npdu_data);

  // BACNET_READ_PROPERTY_DATA data;
  // data.object_type = static_cast<BACNET_OBJECT_TYPE>(mServiceConfig.mObjectType);
  // data.object_instance = mServiceConfig.mObjectId;
  // data.object_property = static_cast<BACNET_PROPERTY_ID>(mServiceConfig.mObjectProperty);
  // data.array_index = mServiceConfig.mArrayIndex;

  // pdu_len += rp_encode_apdu(&PDU_BUFFER[pdu_len], 1, &data);

  // struct sockaddr_in bvlc_dest = { 0 };
  // uint8_t mtu[MAX_MPDU] = { 0 };
  // uint16_t mtu_len = 0;
  // /* addr and port in network format */
  // struct in_addr address;
  // uint16_t port = 0;
  // uint16_t BVLC_length = 0;
  // //bvlc_decode_bip_address(&dest->mac[0], &address, &port);

  // mtu[0] = BVLL_TYPE_BACNET_IP;
  // mtu[1] = BVLC_ORIGINAL_UNICAST_NPDU;

  // memcpy(&address.s_addr, &dest->mac[0], 4);
  // memcpy(&port, &dest->mac[4], 2);

  // bvlc_dest.sin_addr.s_addr = address.s_addr;
  // bvlc_dest.sin_port = port;
  // BVLC_length = (uint16_t) pdu_len + 4 /*inclusive */ ;
  // mtu_len = 2;
  // mtu_len += (uint16_t) encode_unsigned16(&mtu[mtu_len], BVLC_length);
  // memcpy(&mtu[mtu_len], &PDU_BUFFER, pdu_len);
  // mtu_len += (uint16_t) pdu_len;

  // for(int i = 0; i<mtu_len; i++){
  //   printf("%02x ", mtu[i]);
  // }
  // printf("\n");



  // OPTION 2

  // BACNET_ADDRESS dst;
  // dst.mac_len = 6;
  // dst.mac[0] = 192;
  // dst.mac[1] = 168;
  // dst.mac[2] = 1;
  // dst.mac[3] = 1;
  // dst.mac[4] = 0xBA;
  // dst.mac[5] = 0xC0;
  // dst.net = 0;
  // dst.len = 0;

  // BACNET_ADDRESS my_adr;
  // my_adr.mac_len = 6;
  // my_adr.mac[0] = 192;
  // my_adr.mac[1] = 168;
  // my_adr.mac[2] = 1;
  // my_adr.mac[3] = 0;
  // my_adr.mac[4] = 0xBA;
  // my_adr.mac[5] = 0xC0;
  // my_adr.net = 0;
  // my_adr.len = 0;

  // BACNET_ADDRESS *dest = &dst;
  // BACNET_ADDRESS my_address = my_adr;

  // uint8_t PDU_BUFFER[MAX_MPDU];
  // int pdu_len = 0;

  // struct in_addr address;
  // uint16_t port = 0;
  // memcpy(&address.s_addr, &dest->mac[0], 4);
  // memcpy(&port, &dest->mac[4], 2);

  // struct sockaddr_in bvlc_dest;
  // bvlc_dest.sin_addr.s_addr = address.s_addr;
  // bvlc_dest.sin_port = port;
  // PDU_BUFFER[0] = BVLL_TYPE_BACNET_IP;
  // PDU_BUFFER[1] = BVLC_ORIGINAL_UNICAST_NPDU;
  // pdu_len += 4; // +2 for bvll type and bvlc type, +2 for bvlc length (encoded later)
  

  // BACNET_NPDU_DATA npdu_data;
  // npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);
  // int len = pdu_len;
  // len += npdu_encode_pdu(&PDU_BUFFER[len], dest, &my_address, &npdu_data);

  // BACNET_READ_PROPERTY_DATA data;
  // data.object_type = static_cast<BACNET_OBJECT_TYPE>(mServiceConfig.mObjectType);
  // data.object_instance = mServiceConfig.mObjectId;
  // data.object_property = static_cast<BACNET_PROPERTY_ID>(mServiceConfig.mObjectProperty);
  // data.array_index = mServiceConfig.mArrayIndex;

  // len += rp_encode_apdu(&PDU_BUFFER[len], 1, &data);

  // encode_unsigned16(&PDU_BUFFER[2], len); // encode bvlc length

  // for(int i = 0; i<len; i++){
  //   printf("%02x ", PDU_BUFFER[i]);
  // }
  // printf("\n");


  // OPTION 3

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

  BACNET_ADDRESS *dest = &dst;
  BACNET_ADDRESS my_address = my_adr;

  uint8_t PDU_BUFFER[MAX_MPDU];
  int pdu_len = 4;
  BACNET_NPDU_DATA npdu_data;
  npdu_encode_npdu_data(&npdu_data, true, MESSAGE_PRIORITY_NORMAL);
  pdu_len += npdu_encode_pdu(&PDU_BUFFER[pdu_len-1], dest, &my_address, &npdu_data);

  BACNET_READ_PROPERTY_DATA data;
  data.object_type = static_cast<BACNET_OBJECT_TYPE>(mServiceConfig.mObjectType);
  data.object_instance = mServiceConfig.mObjectId;
  data.object_property = static_cast<BACNET_PROPERTY_ID>(mServiceConfig.mObjectProperty);
  data.array_index = mServiceConfig.mArrayIndex;

  pdu_len += rp_encode_apdu(&PDU_BUFFER[pdu_len-1], 1, &data);

  struct in_addr address;
  uint16_t port = 0;
  memcpy(&address.s_addr, &dest->mac[0], 4);
  memcpy(&port, &dest->mac[4], 2);

  struct sockaddr_in bvlc_dest;
  bvlc_dest.sin_addr.s_addr = address.s_addr;
  bvlc_dest.sin_port = port;
  PDU_BUFFER[0] = BVLL_TYPE_BACNET_IP;
  PDU_BUFFER[1] = BVLC_ORIGINAL_UNICAST_NPDU;
  encode_unsigned16(&PDU_BUFFER[2], pdu_len); // encode bvlc length
  for(int i = 0; i<pdu_len-1; i++){
    printf("%02x ", PDU_BUFFER[i]);
  }
  printf("\n");

  //bip_set_socket(static_cast<CBacnetClientController *>(controller)->getSocket());
}

CBacnetReadPropertyHandle::~CBacnetReadPropertyHandle()
{
}


void CBacnetReadPropertyHandle::get(CIEC_ANY &) {
  
}
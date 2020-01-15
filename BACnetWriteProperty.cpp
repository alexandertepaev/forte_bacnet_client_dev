/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetWriteProperty
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2019-12-30/root -  - 
 *************************************************************************/

#include "BACnetWriteProperty.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetWriteProperty_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetWritePropertyConfigFB, g_nStringIdBACnetWriteProperty)

const char* const CBacnetWritePropertyConfigFB::scmError = "Failed";
const char* const CBacnetWritePropertyConfigFB::scmOK = "Initialized";

const CStringDictionary::TStringId CBacnetWritePropertyConfigFB::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdObserverName, g_nStringIdDeviceID, g_nStringIdObjectType, g_nStringIdObjectID, g_nStringIdObjectProperty, g_nStringIdPriority, g_nStringIdArrayIndex};

const CStringDictionary::TStringId CBacnetWritePropertyConfigFB::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdUINT};

const CStringDictionary::TStringId CBacnetWritePropertyConfigFB::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId CBacnetWritePropertyConfigFB::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TDataIOID CBacnetWritePropertyConfigFB::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 CBacnetWritePropertyConfigFB::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId CBacnetWritePropertyConfigFB::scm_anEventOutputNames[] = {g_nStringIdIND};

const SAdapterInstanceDef CBacnetWritePropertyConfigFB::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true },
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterIn, false }};

const SFBInterfaceSpec CBacnetWritePropertyConfigFB::scm_stFBInterfaceSpec = {
  0,  0,  0,  0,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  8,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  2,scm_astAdapterInstances};


void CBacnetWritePropertyConfigFB::executeEvent(int pa_nEIID){
  if(BACnetAdapterIn().INIT() == pa_nEIID) {
    DEVLOG_DEBUG("[BACnetWritePropertyConfigFB] init event\n");
    // todo error message if init() return -1
    const char* const error = init();
    QO() = error == 0;
    STATUS() = scmOK;

    if(BACnetAdapterOut().getPeer() == 0) {
      // backpropagate inito
      BACnetAdapterIn().QO() = QO();
      sendAdapterEvent(scm_nBACnetAdapterInAdpNum, BACnetAdapter::scm_nEventINITOID);
    } else {
      // forward init
      BACnetAdapterOut().MasterId() = BACnetAdapterIn().MasterId();
      BACnetAdapterOut().Index() = (TForteUInt16) (BACnetAdapterIn().Index() + 1);
      BACnetAdapterOut().QI() = BACnetAdapterIn().QI();
      sendAdapterEvent(scm_nBACnetAdapterOutAdpNum, BACnetAdapter::scm_nEventINITID);
    }

  } else if(BACnetAdapterOut().INITO() == pa_nEIID) {
     // backpropagate inito
      BACnetAdapterIn().QO() = BACnetAdapterOut().QO() && QO();
      sendAdapterEvent(scm_nBACnetAdapterInAdpNum, BACnetAdapter::scm_nEventINITOID);
  }
}

/*

*** TODO: move this somewhere?
    functions that return bacnet libs encodings
*/
// BACNET_OBJECT_TYPE CBacnetWritePropertyConfigFB::getObjectType(CIEC_WSTRING paObjectType){
//   if(paObjectType == "ANALOG_OUTPUT"){
//     return BACNET_OBJECT_TYPE::OBJECT_ANALOG_OUTPUT;
//   } else if (paObjectType == "BINARY_OUTPUT") {
//     return BACNET_OBJECT_TYPE::OBJECT_BINARY_OUTPUT;
//   }
// }

// uint32_t CBacnetWritePropertyConfigFB::getObjectProperty(CIEC_WSTRING paObjectProperty){
//   if(paObjectProperty == "PRESENT_VALUE"){
//     return PROP_PRESENT_VALUE;
//   }
// }


const char* CBacnetWritePropertyConfigFB::init(){

  m_nIndex = BACnetAdapterIn().Index();
  forte::core::io::IOConfigFBMultiMaster *master = forte::core::io::IOConfigFBMultiMaster::getMasterById(BACnetAdapterIn().MasterId());
  CBacnetClientController *clictr = static_cast<CBacnetClientController *>(master->getDeviceController());

  m_stServiceConfig = new ServiceConfig(DeviceID(), getObjectType(ObjectType()), ObjectID(), getObjectProperty(ObjectProperty()), BACNET_ARRAY_ALL, Priority());

  //TODO if not analog input/value/object and not binary input/value/object and not present value - return -1 --- for now only allow these type of operations

  clictr->addAddrListEntry(m_stServiceConfig->mDeviceId);

  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::Out, m_nIndex, SERVICE_CONFIRMED_WRITE_PROPERTY, this);
     
  clictr->addHandle(desc); 

  return 0;
}




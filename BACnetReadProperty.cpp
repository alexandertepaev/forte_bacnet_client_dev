/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetReadProperty
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2019-10-30/root -  - 
 *************************************************************************/

#include "BACnetReadProperty.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetReadProperty_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetReadPropertyConfigFB, g_nStringIdBACnetReadProperty)

const char* const CBacnetReadPropertyConfigFB::scmError = "Failed";
const char* const CBacnetReadPropertyConfigFB::scmOK = "Initialized";

const CStringDictionary::TStringId CBacnetReadPropertyConfigFB::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdObserverName, g_nStringIdDeviceID, g_nStringIdObjectType, g_nStringIdObjectID, g_nStringIdObjectProperty, g_nStringIdArrayIndex};

const CStringDictionary::TStringId CBacnetReadPropertyConfigFB::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT};

const CStringDictionary::TStringId CBacnetReadPropertyConfigFB::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId CBacnetReadPropertyConfigFB::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TDataIOID CBacnetReadPropertyConfigFB::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 CBacnetReadPropertyConfigFB::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId CBacnetReadPropertyConfigFB::scm_anEventOutputNames[] = {g_nStringIdIND};

const SAdapterInstanceDef CBacnetReadPropertyConfigFB::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true },
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterIn, false }};

const SFBInterfaceSpec CBacnetReadPropertyConfigFB::scm_stFBInterfaceSpec = {
  0,  0,  0,  0,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  7,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  2,scm_astAdapterInstances};


void CBacnetReadPropertyConfigFB::executeEvent(int pa_nEIID){
  if(BACnetAdapterIn().INIT() == pa_nEIID) {
    DEVLOG_DEBUG("[BACnetReadPropertyConfigFB] init event\n");
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
uint32_t getObjectType(CIEC_WSTRING paObjectType){
  if(paObjectType == "ANALOG_OUTPUT"){
    return OBJECT_ANALOG_OUTPUT;
  }
}

uint32_t getObjectProperty(CIEC_WSTRING paObjectProperty){
  if(paObjectProperty == "PRESENT_VALUE"){
    return PROP_PRESENT_VALUE;
  }
}

const char* CBacnetReadPropertyConfigFB::init(){

  m_nIndex = BACnetAdapterIn().Index();
  forte::core::io::IOConfigFBMultiMaster *master = forte::core::io::IOConfigFBMultiMaster::getMasterById(BACnetAdapterIn().MasterId());
  CBacnetClientController *clictr = static_cast<CBacnetClientController *>(master->getDeviceController());

  m_stServiceConfig.mDeviceId = DeviceID();
  m_stServiceConfig.mObjectType = getObjectType(ObjectType());
  m_stServiceConfig.mObjectId = ObjectID();
  m_stServiceConfig.mObjectProperty = getObjectProperty(ObjectProperty());
  m_stServiceConfig.mArrayIndex = BACNET_ARRAY_ALL;
  m_stServiceConfig.dummy_value = 567;

  clictr->addAddrListEntry(m_stServiceConfig.mDeviceId);

  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, m_nIndex, SERVICE_CONFIRMED_READ_PROPERTY, this);
     
  clictr->addHandle(desc); 

  return 0;
}






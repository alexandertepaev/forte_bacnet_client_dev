/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetSubscribeUnconfirmedCOV
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2020-01-18/root -  - 
 *************************************************************************/

#include "BACnetSubscribeUnconfirmedCOV.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetSubscribeUnconfirmedCOV_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetSubscribeUnconfirmedCOVConfigFB, g_nStringIdBACnetSubscribeUnconfirmedCOV)

const char* const CBacnetSubscribeUnconfirmedCOVConfigFB::scmError = "Failed";
const char* const CBacnetSubscribeUnconfirmedCOVConfigFB::scmOK = "Initialized";

const CStringDictionary::TStringId CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdObserverName, g_nStringIdDeviceID, g_nStringIdObjectType, g_nStringIdObjectID};

const CStringDictionary::TStringId CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT};

const CStringDictionary::TStringId CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TDataIOID CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId CBacnetSubscribeUnconfirmedCOVConfigFB::scm_anEventOutputNames[] = {g_nStringIdIND};

const SAdapterInstanceDef CBacnetSubscribeUnconfirmedCOVConfigFB::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true },
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterIn, false }};

const SFBInterfaceSpec CBacnetSubscribeUnconfirmedCOVConfigFB::scm_stFBInterfaceSpec = {
  0,  0,  0,  0,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  5,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  2,scm_astAdapterInstances};


void CBacnetSubscribeUnconfirmedCOVConfigFB::executeEvent(int pa_nEIID){
  if(BACnetAdapterIn().INIT() == pa_nEIID) {
    DEVLOG_DEBUG("[BacnetSubscribeUnconfirmedCOVConfigFB] init event\n");
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

const char* CBacnetSubscribeUnconfirmedCOVConfigFB::init(){

  m_nIndex = BACnetAdapterIn().Index();
  forte::core::io::IOConfigFBMultiMaster *master = forte::core::io::IOConfigFBMultiMaster::getMasterById(BACnetAdapterIn().MasterId());
  CBacnetClientController *clictr = static_cast<CBacnetClientController *>(master->getDeviceController());

  m_stServiceConfig = new ServiceConfig(DeviceID(), getObjectType(ObjectType()), ObjectID());

  // TODO if not analog input/value/object and not binary input/value/object and not present value - return -1 --- for now only allow these type of operations

  clictr->addAddrListEntry(m_stServiceConfig->mDeviceId);

  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, m_nIndex, SERVICE_CONFIRMED_SUBSCRIBE_COV, this);
     
  clictr->addHandle(desc); 

  return 0;
}





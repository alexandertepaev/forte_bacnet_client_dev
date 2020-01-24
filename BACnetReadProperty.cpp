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

const char* const CBacnetReadPropertyConfigFB::scmError = "Initialization failed";
const char* const CBacnetReadPropertyConfigFB::scmAddrFetchFailed = "Address fetch failed";
// "Subscription failed"
const char* const CBacnetReadPropertyConfigFB::scmOK = "Initialized";
const char* const CBacnetReadPropertyConfigFB::scmHandleInitFailed = "Handle initialization failed";

//static const char* const scmCOVSubscriptionFailed;

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
    
    // todo error message if init() return -1
    const char* const error = init();

    if(error){
      QO() = false;
      STATUS() = error;
    }

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
  } else if(cg_nExternalEventID == pa_nEIID){
    switch(mNotificationType){
      case e_Success:
        QO() = true;
        STATUS() = scmOK;
        break;
      case e_AddrFetchFailed:
        QO() = false;
        STATUS() = scmAddrFetchFailed;
        break;
      case e_COVSubscriptionFailed:
        break;
      default:
        break;
    }
  }
}


// /*

// *** TODO: move this somewhere?
//     functions that return bacnet libs encodings
// */
// uint32_t getObjectType(CIEC_WSTRING paObjectType){
//   if(paObjectType == "ANALOG_OUTPUT"){
//     return OBJECT_ANALOG_OUTPUT;
//   }
// }

// uint32_t getObjectProperty(CIEC_WSTRING paObjectProperty){
//   if(paObjectProperty == "PRESENT_VALUE"){
//     return PROP_PRESENT_VALUE;
//   }
// }

const char* CBacnetReadPropertyConfigFB::init(){

  setEventChainExecutor(m_poInvokingExecEnv);

  m_nIndex = BACnetAdapterIn().Index();

  CBacnetClientConfigFB *master = CBacnetClientConfigFB::getClientConfigFB(); // TODO check if master is not NULL
  CBacnetClientController *clictr = static_cast<CBacnetClientController *>(master->getDeviceController());

  m_stServiceConfig = new ServiceConfig(DeviceID(), getObjectType(ObjectType()), ObjectID(), getObjectProperty(ObjectProperty()), BACNET_ARRAY_ALL); // TODO BACNET_ARRAY_ALL?

  //TODO if not analog input/value/object and not binary input/value/object and not present value - return -1 --- for now only allow these type of operations
  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, SERVICE_CONFIRMED_READ_PROPERTY, this);
  
  // ask client controller to register new handle to the IOMapper
  clictr->addHandle(desc); 

  // if the registered handle is 0 (register failed), return error
  if(mServiceHandle == 0) {
    return scmHandleInitFailed;
  } 

  // otherwise update controller's service funcblocks list
  clictr->updateSCFBsList(this);

  return 0;
}






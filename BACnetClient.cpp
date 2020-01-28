/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetClient
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2019-10-24/root - null - 
 *************************************************************************/

#include "BACnetClient.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetClient_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetClientConfigFB, g_nStringIdBACnetClient)

CBacnetClientConfigFB *CBacnetClientConfigFB::mBacnetClientConfigFB = NULL;

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdPort, g_nStringIdDeviceObjectID, g_nStringIdDeviceObjectName, g_nStringIdPathToAddrFile};

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdUINT, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdWSTRING};

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TForteInt16 CBacnetClientConfigFB::scm_anEIWithIndexes[] = {0};
const TDataIOID CBacnetClientConfigFB::scm_anEIWith[] = {0, 1, 2, 3, 4, 255};
const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anEventInputNames[] = {g_nStringIdINIT};

const TDataIOID CBacnetClientConfigFB::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 CBacnetClientConfigFB::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anEventOutputNames[] = {g_nStringIdINITO};

const SAdapterInstanceDef CBacnetClientConfigFB::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true }};

const SFBInterfaceSpec CBacnetClientConfigFB::scm_stFBInterfaceSpec = {
  1,  scm_anEventInputNames,  scm_anEIWith,  scm_anEIWithIndexes,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  5,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  1,scm_astAdapterInstances};


int CBacnetClientConfigFB::sm_nControllerInstances = 0;

void CBacnetClientConfigFB::executeEvent(int pa_nEIID){
  IOConfigFBController::executeEvent(pa_nEIID);

  if (BACnetAdapterOut().INITO() == pa_nEIID) {
    IOConfigFBController::onStartup();
    static_cast<CBacnetClientController *>(getDeviceController())->daisyChainInitDone();
  }
    
}

forte::core::io::IODeviceController* CBacnetClientConfigFB::createDeviceController(CDeviceExecution& paDeviceExecution){
  m_nId = sm_nControllerInstances++;
  DEVLOG_DEBUG("[CBacnetClientConfigFB] createDeviceController(): Creating client controller id=%d\n", m_nId);
  return new CBacnetClientController(paDeviceExecution, m_nId);
  // return new BacnetClientController(paDeviceExecution);
  //return 0;
}

void CBacnetClientConfigFB::setConfig(){
  DEVLOG_DEBUG("[CBacnetClientConfigFB] setConfig(): Setting client controller's configuration parameters\n");
  CBacnetClientController::SBacnetClientControllerConfig config;
  config.nPortNumber = Port();
  config.nDeviceObjID = DeviceObjectID();
  config.sDeviceObjName = DeviceObjectName().getValue();
  config.sPathToAddrFile = PathToAddrFile().getValue(); // TODO - maybe not needed? We want to use Who-is/I-Am
  getDeviceController()->setConfig(&config);
}


void CBacnetClientConfigFB::onStartup(){
  DEVLOG_DEBUG("[CBacnetClientConfigFB] onStartup()\n");
  //check if we have adapter peers 
  if (BACnetAdapterOut().getPeer() == 0) {
    static_cast<CBacnetClientController *>(getDeviceController())->daisyChainInitDone();
    return IOConfigFBController::onStartup();
  }

  CBacnetClientConfigFB::mBacnetClientConfigFB = this;
  // pass BACnetAdatper.INIT event
  BACnetAdapterOut().MasterId() = m_nId;
  BACnetAdapterOut().Index() = 1;
  sendAdapterEvent(scm_nBACnetAdapterOutAdpNum, BACnetAdapter::scm_nEventINITID);
}

CBacnetClientConfigFB* CBacnetClientConfigFB::getClientConfigFB() {
  return CBacnetClientConfigFB::mBacnetClientConfigFB;
}


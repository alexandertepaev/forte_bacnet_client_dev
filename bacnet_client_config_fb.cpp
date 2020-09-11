/*******************************************************************************
 * Copyright (c) 2017 - 2020 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev - initial implementation and documentation
 *******************************************************************************/

#include "bacnet_client_config_fb.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "bacnet_client_config_fb_gen.cpp"
#endif
#include "bacnet_client_controller.h"

#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetClient_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetClientConfigFB, g_nStringIdBACnetClient)

CBacnetClientConfigFB *CBacnetClientConfigFB::mBacnetClientConfigFB = NULL;

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdPort};

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdUINT};

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TForteInt16 CBacnetClientConfigFB::scm_anEIWithIndexes[] = {0};
const TDataIOID CBacnetClientConfigFB::scm_anEIWith[] = {0, 1, 255};
const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anEventInputNames[] = {g_nStringIdINIT};

const TDataIOID CBacnetClientConfigFB::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 CBacnetClientConfigFB::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId CBacnetClientConfigFB::scm_anEventOutputNames[] = {g_nStringIdINITO};

const SAdapterInstanceDef CBacnetClientConfigFB::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true }};

const SFBInterfaceSpec CBacnetClientConfigFB::scm_stFBInterfaceSpec = {
  1,  scm_anEventInputNames,  scm_anEIWith,  scm_anEIWithIndexes,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  2,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  1,scm_astAdapterInstances};


CBacnetClientConfigFB::CBacnetClientConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
forte::core::io::IOConfigFBController( pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {

}

CBacnetClientConfigFB::~CBacnetClientConfigFB(){

}

void CBacnetClientConfigFB::executeEvent(int pa_nEIID){

  // call executeEvent of the base class, which initializes and starts the client controller
  IOConfigFBController::executeEvent(pa_nEIID);

  // in case BACnetAdapterOut().INITO is received, change client controller's state to address discovery
  // and call onStartup() of the base class, which results in issuing INITO output event
  if (BACnetAdapterOut().INITO() == pa_nEIID) {
    static_cast<CBacnetClientController *>(getDeviceController())->m_eClientControllerState = CBacnetClientController::e_AddressDiscovery;
    IOConfigFBController::onStartup();
  }    
}

inline forte::core::io::IODeviceController* CBacnetClientConfigFB::createDeviceController(CDeviceExecution& paDeviceExecution){
  return new CBacnetClientController(paDeviceExecution);
}

void CBacnetClientConfigFB::setConfig(){
  CBacnetClientController::SBacnetClientControllerConfig stConfig;
  stConfig.nPort = htons(Port()); // store in network byte order
  getDeviceController()->setConfig(&stConfig);
}


void CBacnetClientConfigFB::onStartup(){

  CBacnetClientConfigFB::mBacnetClientConfigFB = this;

  if(BACnetAdapterOut().getPeer() != 0) {
    // pass the BACnetAdapterOut.INIT event to the next configuration FB 
    sendAdapterEvent(scm_nBACnetAdapterOutAdpNum, BACnetAdapter::scm_nEventINITID);
  } else {
    // in case there is no other configuration FBs in the daisy-chain, change client controller's state to address discovery
    // and call onStartup() of the base class, which results in issuing INITO output event
    static_cast<CBacnetClientController *>(getDeviceController())->m_eClientControllerState = CBacnetClientController::e_AddressDiscovery;
    IOConfigFBController::onStartup();
  }
}

CBacnetClientConfigFB* CBacnetClientConfigFB::getClientConfigFB() {
  return CBacnetClientConfigFB::mBacnetClientConfigFB;
}


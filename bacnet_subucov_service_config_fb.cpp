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


#include "bacnet_subucov_service_config_fb.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "bacnet_subucov_service_config_fb_gen.cpp"
#endif

#include "bacnet_client_controller.h"


DEFINE_FIRMWARE_FB(CBacnetSubscribeUnconfirmedCOVConfigFB, g_nStringIdBACnetSubscribeUnconfirmedCOV)
CBacnetSubscribeUnconfirmedCOVConfigFB::CBacnetSubscribeUnconfirmedCOVConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
CBacnetServiceConfigFB(pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {};

CBacnetSubscribeUnconfirmedCOVConfigFB::~CBacnetSubscribeUnconfirmedCOVConfigFB(){};

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


bool CBacnetSubscribeUnconfirmedCOVConfigFB::setConfig() {
  BACNET_OBJECT_TYPE objType = stringToBacnetObjectType(ObjectType()); // set to MAX_BACNET_OBJECT_TYPE if not supported

  if(objType == MAX_BACNET_OBJECT_TYPE || 
      DeviceID() > BACNET_MAX_INSTANCE || 
      ObjectID() > BACNET_MAX_INSTANCE)
      return false;
  
  m_stServiceConfig = new SServiceConfig(DeviceID(), objType, ObjectID());
  return true;
}

bool CBacnetSubscribeUnconfirmedCOVConfigFB::initHandle(CBacnetClientController *paController) {
  // HandleDescriptor(observer name string, direction, service type, IEC datatype of the communicated data, pointer to this config fb)
  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, SERVICE_CONFIRMED_SUBSCRIBE_COV, objectPropertyAndTypeToIECDataType(stringToBacnetObjectType(ObjectType()), PROP_PRESENT_VALUE), this);
  paController->addHandle(desc);
  return true;
}







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

#include "bacnet_writeproperty_service_config_fb.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "bacnet_writeproperty_service_config_fb_gen.cpp"
#endif

#include "bacnet_client_controller.h"


DEFINE_FIRMWARE_FB(CBacnetWritePropertyConfigFB, g_nStringIdBACnetWriteProperty)

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


CBacnetWritePropertyConfigFB::CBacnetWritePropertyConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
CBacnetServiceConfigFB(pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {
};

CBacnetWritePropertyConfigFB::~CBacnetWritePropertyConfigFB(){
}

bool CBacnetWritePropertyConfigFB::setConfig() {
  BACNET_OBJECT_TYPE objType = stringToBacnetObjectType(ObjectType()); // set to MAX_BACNET_OBJECT_TYPE if not supported
  BACNET_PROPERTY_ID objProp = stringToBacnetObjectProperty(ObjectProperty()); // set to MAX_BACNET_OBJECT_TYPE if not supported

  if(objType == MAX_BACNET_OBJECT_TYPE || 
      objProp == MAX_BACNET_PROPERTY_ID || 
      DeviceID() > BACNET_MAX_INSTANCE || 
      ObjectID() > BACNET_MAX_INSTANCE ||
      Priority() < BACNET_NO_PRIORITY ||
      Priority() > BACNET_MAX_PRIORITY)
      return false;

  m_stServiceConfig = new SServiceConfig(DeviceID(), objType, (TForteUInt16) ObjectID(), objProp, BACNET_ARRAY_ALL, BACNET_WRITEPROP_LOWEST_PRIO); // BACNET_ARRAY_ALL hardcoded, BACNET_WRITEPROP_LOWEST_PRIO hardcoded
  return true;
}

bool CBacnetWritePropertyConfigFB::initHandle(CBacnetClientController *paController) {
   // HandleDescriptor(observer name string, direction, service type, IEC datatype of the communicated data, pointer to this config fb)
  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::Out, SERVICE_CONFIRMED_WRITE_PROPERTY,  objectPropertyAndTypeToIECDataType(stringToBacnetObjectType(ObjectType()), stringToBacnetObjectProperty(ObjectProperty())), this);
  paController->addHandle(desc);
  return true;
}



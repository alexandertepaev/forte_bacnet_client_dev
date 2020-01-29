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
#include "bacnet_client_controller.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetWriteProperty_gen.cpp"
#endif

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
CBacnetServiceConfigFB(e_WriteProperty, pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {
};

CBacnetWritePropertyConfigFB::~CBacnetWritePropertyConfigFB(){
}

bool CBacnetWritePropertyConfigFB::setConfig() {
  BACNET_OBJECT_TYPE objType = getObjectType(ObjectType());
  BACNET_PROPERTY_ID objProp = getObjectProperty(ObjectProperty());

  if(objType == BACNET_OBJECT_TYPE::MAX_BACNET_OBJECT_TYPE || 
      objProp == BACNET_PROPERTY_ID::MAX_BACNET_PROPERTY_ID || 
      DeviceID() > BACNET_MAX_INSTANCE || 
      ObjectID() > BACNET_MAX_INSTANCE ||
      Priority() < BACNET_NO_PRIORITY ||
      Priority() > BACNET_MAX_PRIORITY)
      return false;

  m_stServiceConfig = new ServiceConfig(DeviceID(), objType, ObjectID(), objProp, BACNET_ARRAY_ALL, Priority()); // TODO BACNET_ARRAY_ALL?
  return true;
}

bool CBacnetWritePropertyConfigFB::initHandle(CBacnetClientController *paController) {
  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::Out, BACNET_CONFIRMED_SERVICE::SERVICE_CONFIRMED_WRITE_PROPERTY, this);

  paController->addHandle(desc);

  if(mServiceHandle == 0)
    return false;

  return true;
}



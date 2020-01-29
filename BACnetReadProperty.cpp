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
#include "bacnet_client_controller.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetReadProperty_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetReadPropertyConfigFB, g_nStringIdBACnetReadProperty)


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


CBacnetReadPropertyConfigFB::CBacnetReadPropertyConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
CBacnetServiceConfigFB(e_ReadProperty, pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData) {
}

CBacnetReadPropertyConfigFB::~CBacnetReadPropertyConfigFB() {
}


bool CBacnetReadPropertyConfigFB::setConfig() {
  BACNET_OBJECT_TYPE objType = getObjectType(ObjectType());
  BACNET_PROPERTY_ID objProp = getObjectProperty(ObjectProperty());

  if(objType == BACNET_OBJECT_TYPE::MAX_BACNET_OBJECT_TYPE || 
      objProp == BACNET_PROPERTY_ID::MAX_BACNET_PROPERTY_ID || 
      DeviceID() > BACNET_MAX_INSTANCE || 
      ObjectID() > BACNET_MAX_INSTANCE)
      return false;

  m_stServiceConfig = new ServiceConfig(DeviceID(), objType, ObjectID(), objProp, BACNET_ARRAY_ALL);
  return true;
}

bool CBacnetReadPropertyConfigFB::initHandle(CBacnetClientController *paController) {
  CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, BACNET_CONFIRMED_SERVICE::SERVICE_CONFIRMED_READ_PROPERTY, this);

  paController->addHandle(desc);

  if(mServiceHandle == 0)
    return false;

  return true;
}





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
#include "bacnet_client_controller.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetSubscribeUnconfirmedCOV_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(CBacnetSubscribeUnconfirmedCOVConfigFB, g_nStringIdBACnetSubscribeUnconfirmedCOV)
CBacnetSubscribeUnconfirmedCOVConfigFB::CBacnetSubscribeUnconfirmedCOVConfigFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) : \
CBacnetServiceConfigFB(e_UnconfirmedCOVSub, pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId, m_anFBConnData, m_anFBVarsData), mSubscriptionInvokeId(0), mSubscriptionAcknowledged(false) {};

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
  BACNET_OBJECT_TYPE objType = getObjectType(ObjectType());

  if(objType == MAX_BACNET_OBJECT_TYPE || 
      DeviceID() > BACNET_MAX_INSTANCE || 
      ObjectID() > BACNET_MAX_INSTANCE)
      return false;
  
  m_stServiceConfig = new ServiceConfig(DeviceID(), objType, ObjectID());
  return true;
}

bool CBacnetSubscribeUnconfirmedCOVConfigFB::initHandle(CBacnetClientController *paController) {
   CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, SERVICE_CONFIRMED_SUBSCRIBE_COV, getIECDataType(m_stServiceConfig->mObjectType), this);
  // CBacnetClientController::HandleDescriptor *desc = new CBacnetClientController::HandleDescriptor(ObserverName(), forte::core::io::IOMapper::In, BACNET_CONFIRMED_SERVICE::SERVICE_CONFIRMED_SUBSCRIBE_COV, this);

  paController->addHandle(desc);

  if(mServiceHandle == 0)
    return false;

  return true;
}







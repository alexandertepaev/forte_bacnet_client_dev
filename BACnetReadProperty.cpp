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

DEFINE_FIRMWARE_FB(FORTE_BACnetReadProperty, g_nStringIdBACnetReadProperty)

const CStringDictionary::TStringId FORTE_BACnetReadProperty::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdObserverName, g_nStringIdDeviceID, g_nStringIdObjectType, g_nStringIdObjectID, g_nStringIdObjectProperty, g_nStringIdArrayIndex};

const CStringDictionary::TStringId FORTE_BACnetReadProperty::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT, g_nStringIdWSTRING, g_nStringIdUINT};

const CStringDictionary::TStringId FORTE_BACnetReadProperty::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId FORTE_BACnetReadProperty::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TDataIOID FORTE_BACnetReadProperty::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 FORTE_BACnetReadProperty::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId FORTE_BACnetReadProperty::scm_anEventOutputNames[] = {g_nStringIdIND};

const SAdapterInstanceDef FORTE_BACnetReadProperty::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true },
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterIn, false }};

const SFBInterfaceSpec FORTE_BACnetReadProperty::scm_stFBInterfaceSpec = {
  0,  0,  0,  0,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  7,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  2,scm_astAdapterInstances};


void FORTE_BACnetReadProperty::executeEvent(int pa_nEIID){
  if(BACnetAdapterIn().INIT() == pa_nEIID) {
    DEVLOG_DEBUG("[BACnetReadPropertyConfigFB] \n");
  }
}




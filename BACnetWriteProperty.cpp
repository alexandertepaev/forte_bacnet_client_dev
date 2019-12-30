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
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetWriteProperty_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(FORTE_BACnetWriteProperty, g_nStringIdBACnetWriteProperty)

const CStringDictionary::TStringId FORTE_BACnetWriteProperty::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdObserverName, g_nStringIdDeviceID, g_nStringIdObjectType, g_nStringIdObjectID, g_nStringIdObjectProperty, g_nStringIdPriority, g_nStringIdArrayIndex};

const CStringDictionary::TStringId FORTE_BACnetWriteProperty::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdSTRING, g_nStringIdUINT, g_nStringIdSTRING, g_nStringIdUINT, g_nStringIdSTRING, g_nStringIdUINT, g_nStringIdUINT};

const CStringDictionary::TStringId FORTE_BACnetWriteProperty::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId FORTE_BACnetWriteProperty::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TDataIOID FORTE_BACnetWriteProperty::scm_anEOWith[] = {0, 1, 255};
const TForteInt16 FORTE_BACnetWriteProperty::scm_anEOWithIndexes[] = {0, -1};
const CStringDictionary::TStringId FORTE_BACnetWriteProperty::scm_anEventOutputNames[] = {g_nStringIdIND};

const SAdapterInstanceDef FORTE_BACnetWriteProperty::scm_astAdapterInstances[] = {
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterOut, true },
{g_nStringIdBACnetAdapter, g_nStringIdBACnetAdapterIn, false }};

const SFBInterfaceSpec FORTE_BACnetWriteProperty::scm_stFBInterfaceSpec = {
  0,  0,  0,  0,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  8,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  2,scm_astAdapterInstances};


void FORTE_BACnetWriteProperty::executeEvent(int pa_nEIID){
  switch(pa_nEIID){
  }
}




// /*************************************************************************
//  *** FORTE Library Element
//  ***
//  *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
//  ***
//  *** Name: BACnetAdapter
//  *** Description: Adapter Interface
//  *** Version: 
//  *************************************************************************/

// #include "BACnetAdapter.h"
// #ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
// #include "BACnetAdapter_gen.cpp"
// #endif

// DEFINE_ADAPTER_TYPE(BACnetAdapter, g_nStringIdBACnetAdapter)

// const CStringDictionary::TStringId BACnetAdapter::scm_anDataInputNames[] = {g_nStringIdQO};

// const CStringDictionary::TStringId BACnetAdapter::scm_anDataInputTypeIds[] = {g_nStringIdBOOL};

// const CStringDictionary::TStringId BACnetAdapter::scm_anDataOutputNames[] = {g_nStringIdQI, g_nStringIdMasterId, g_nStringIdIndex};

// const CStringDictionary::TStringId BACnetAdapter::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdUINT, g_nStringIdUINT};

// const TDataIOID BACnetAdapter::scm_anEIWith[] = {0, 255};
// const TForteInt16 BACnetAdapter::scm_anEIWithIndexes[] = {0, -1};
// const CStringDictionary::TStringId BACnetAdapter::scm_anEventInputNames[] = {g_nStringIdINITO};

// const TDataIOID BACnetAdapter::scm_anEOWith[] = {0, 1, 2, 255};
// const TForteInt16 BACnetAdapter::scm_anEOWithIndexes[] = {0, -1};
// const CStringDictionary::TStringId BACnetAdapter::scm_anEventOutputNames[] = {g_nStringIdINIT};

// const SFBInterfaceSpec BACnetAdapter::scm_stFBInterfaceSpecSocket = {
//   1,  scm_anEventInputNames,  scm_anEIWith,  scm_anEIWithIndexes,
//   1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  1,  scm_anDataInputNames, scm_anDataInputTypeIds,
//   3,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
//   0, 0
// };

// const SFBInterfaceSpec BACnetAdapter::scm_stFBInterfaceSpecPlug = {
//   1,  scm_anEventOutputNames,  scm_anEOWith,  scm_anEOWithIndexes,
//   1,  scm_anEventInputNames,  scm_anEIWith, scm_anEIWithIndexes,  3,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
//   1,  scm_anDataInputNames, scm_anDataInputTypeIds,
//   0, 0
// };



/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: BACnetAdapter
 *** Description: Adapter Interface
 *** Version: 
 *************************************************************************/

#include "BACnetAdapter.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetAdapter_gen.cpp"
#endif

DEFINE_ADAPTER_TYPE(BACnetAdapter, g_nStringIdBACnetAdapter)

const TForteInt16 BACnetAdapter::scm_anEIWithIndexes[] = {-1, -1};
const CStringDictionary::TStringId BACnetAdapter::scm_anEventInputNames[] = {g_nStringIdINITO};

const TForteInt16 BACnetAdapter::scm_anEOWithIndexes[] = {-1, -1};
const CStringDictionary::TStringId BACnetAdapter::scm_anEventOutputNames[] = {g_nStringIdINIT};

const SFBInterfaceSpec BACnetAdapter::scm_stFBInterfaceSpecSocket = {
  1,  scm_anEventInputNames,  0,  scm_anEIWithIndexes,
  1,  scm_anEventOutputNames,  0, 0,  0,  0, 0, 
  0,  0, 0,
  0, 0
};

const SFBInterfaceSpec BACnetAdapter::scm_stFBInterfaceSpecPlug = {
  1,  scm_anEventOutputNames,  0,  scm_anEOWithIndexes,
  1,  scm_anEventInputNames,  0, 0,  0,  0, 0, 
  0,  0, 0,
  0, 0
};




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

#include "BACnetAdapter.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BACnetAdapter_gen.cpp"
#endif

DEFINE_ADAPTER_TYPE(BACnetAdapter, g_nStringIdBACnetAdapter)

const CStringDictionary::TStringId BACnetAdapter::scm_anDataOutputNames[] = {g_nStringIdControllerID};

const CStringDictionary::TStringId BACnetAdapter::scm_anDataOutputTypeIds[] = {g_nStringIdUINT};

const TForteInt16 BACnetAdapter::scm_anEIWithIndexes[] = {-1, -1};
const CStringDictionary::TStringId BACnetAdapter::scm_anEventInputNames[] = {g_nStringIdINITO};

const TDataIOID BACnetAdapter::scm_anEOWith[] = {0, 255};
const TForteInt16 BACnetAdapter::scm_anEOWithIndexes[] = {-1, -1};
const CStringDictionary::TStringId BACnetAdapter::scm_anEventOutputNames[] = {g_nStringIdINIT};

const SFBInterfaceSpec BACnetAdapter::scm_stFBInterfaceSpecSocket = {
  1,  scm_anEventInputNames,  0,  scm_anEIWithIndexes,
  1,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  0,  0, 0, 
  1,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  0, 0
};

const SFBInterfaceSpec BACnetAdapter::scm_stFBInterfaceSpecPlug = {
  1,  scm_anEventOutputNames,  scm_anEOWith,  scm_anEOWithIndexes,
  1,  scm_anEventInputNames,  0, 0,  1,  scm_anDataOutputNames, scm_anDataOutputTypeIds, 
  0,  0, 0,
  0, 0
};




/*******************************************************************************
 * Copyright (c) 2014 - 2015 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alois Zoitl, Waldemar Eisenmenger - initial API and implementation and/or initial documentation
 *******************************************************************************/
#include "BacnetQX.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BacnetQX_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(FORTE_BacnetQX, g_nStringIdBacnetQX)

const CStringDictionary::TStringId FORTE_BacnetQX::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdPARAMS, g_nStringIdOUT};

const CStringDictionary::TStringId FORTE_BacnetQX::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdSTRING, g_nStringIdBOOL};

const CStringDictionary::TStringId FORTE_BacnetQX::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId FORTE_BacnetQX::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdSTRING};

const TForteInt16 FORTE_BacnetQX::scm_anEIWithIndexes[] = {0, 3};
const TDataIOID FORTE_BacnetQX::scm_anEIWith[] = {0, 1, 255, 0, 2, 255};
const CStringDictionary::TStringId FORTE_BacnetQX::scm_anEventInputNames[] = {g_nStringIdINIT, g_nStringIdREQ};

const TDataIOID FORTE_BacnetQX::scm_anEOWith[] = {0, 1, 255, 0, 1, 255};
const TForteInt16 FORTE_BacnetQX::scm_anEOWithIndexes[] = {0, 3, -1};
const CStringDictionary::TStringId FORTE_BacnetQX::scm_anEventOutputNames[] = {g_nStringIdINITO, g_nStringIdCNF};

const SFBInterfaceSpec FORTE_BacnetQX::scm_stFBInterfaceSpec = {
  2,  scm_anEventInputNames,  scm_anEIWith,  scm_anEIWithIndexes,
  2,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  3,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  0, 0
};

void FORTE_BacnetQX::executeEvent(int pa_nEIID){
  QO() = QI();
  switch(pa_nEIID){
    case cg_nExternalEventID:
      
      QO() = CProcessInterface::writePin();
      sendOutputEvent(scm_nEventCNFID);
      break;

    case scm_nEventINITID:
      if(true == QI()){
        QO() = CProcessInterface::initialise(false); //initialise as output
      }
      else{
        QO() = CProcessInterface::deinitialise();
      }
      sendOutputEvent(scm_nEventINITOID);
      break;
    case scm_nEventREQID:
      // if(true == QI()){
      //   QO() = CProcessInterface::writePin();
      // }
      // sendOutputEvent(scm_nEventCNFID);
      CProcessInterface::writePin();
      break;
  }
}


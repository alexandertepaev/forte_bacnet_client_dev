#/*******************************************************************************
# * Copyright (c) 2020 Alexander Tepaev github.com/alexandertepaev
# * This program and the accompanying materials are made available under the
# * terms of the Eclipse Public License 2.0 which is available at
# * http://www.eclipse.org/legal/epl-2.0.
# *
# * SPDX-License-Identifier: EPL-2.0
# *
# * Contributors:
# *   Alexander Tepaev
# *******************************************************************************/
forte_add_module(BACnetClient "BACnetClient")

if(FORTE_MODULE_BACnetClient)
  SET(BACNET_STACK_DIR "" CACHE PATH "Root directory of BACnet Stack library")

  if(EXISTS ${BACNET_STACK_DIR})
    forte_add_include_directories(${BACNET_STACK_DIR}/src)
    forte_add_include_directories(${BACNET_STACK_DIR}/src/bacnet)
    forte_add_include_directories(${BACNET_STACK_DIR}/src/bacnet/datalink)
    
    SET(BACNET_STACK_BUILD_DIR "" CACHE PATH "Build directory of BACnet Stack library")
    if(EXISTS ${BACNET_STACK_BUILD_DIR})
      forte_add_link_directories(${BACNET_STACK_BUILD_DIR})
      forte_add_link_library(libbacnet-stack.a)
    else()
      message(FATAL_ERROR "Provide correct path to BACnet Stack library build directory")
    endif()
  else()
    message(FATAL_ERROR "Provide correct path to BACnet Stack library root directory")
  endif()

  ### Config FBs+Adapter
  forte_add_sourcefile_hcpp(bacnet_client_config_fb)
  forte_add_sourcefile_hcpp(bacnet_client_controller)
  forte_add_sourcefile_hcpp(bacnet_service_config_fb)
  forte_add_sourcefile_hcpp(bacnet_readproperty_service_config_fb)
  forte_add_sourcefile_hcpp(bacnet_writeproperty_service_config_fb)
  forte_add_sourcefile_hcpp(bacnet_subucov_service_config_fb)
  forte_add_sourcefile_hcpp(bacnet_service_handle)
  forte_add_sourcefile_hcpp(bacnet_readproperty_handle)
  forte_add_sourcefile_hcpp(bacnet_writeproperty_handle)
  forte_add_sourcefile_hcpp(bacnet_unconfirmed_cov_handle)
  forte_add_sourcefile_hcpp(BACnetAdapter)

  ### BACnet IO FBs
  forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetIX)
  forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetQX)
  forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetID)
  forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetQD)

  
endif()
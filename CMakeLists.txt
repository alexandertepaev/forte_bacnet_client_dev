
forte_add_module(BACnet_Client_1.11dev "BACnet_Client_1.11dev")
SET(BACNET_LIB_DIR "" CACHE PATH "Path to Bacnet library directory ")

#forte_add_sourcefile_hcpp(BACnetClient)
forte_add_sourcefile_hcpp(bacnet_client_config_fb)
forte_add_sourcefile_hcpp(BACnetAdapter)
forte_add_sourcefile_hcpp(bacnet_client_controller)
forte_add_sourcefile_hcpp(bacnet_service_config_fb)
forte_add_sourcefile_hcpp(BACnetReadProperty)
forte_add_sourcefile_hcpp(BACnetWriteProperty)
forte_add_sourcefile_hcpp(BACnetSubscribeUnconfirmedCOV)
forte_add_sourcefile_hcpp(bacnet_service_handle)
forte_add_sourcefile_hcpp(bacnet_readproperty_handle)
forte_add_sourcefile_hcpp(bacnet_writeproperty_handle)
forte_add_sourcefile_hcpp(bacnet_unconfirmed_cov_handle)

forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetIX)
forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetQX)
forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetID)
forte_add_sourcefile_hcpp(bacnet_io_fbs/BacnetQD)


forte_add_include_directories( /home/adm1n/Desktop/bachelors_thesis/fordiac/ext_modules/bacnet_client_1.11_dev/bacnet-stack-0.8.6/include/ )
forte_add_include_directories( /home/adm1n/Desktop/bachelors_thesis/fordiac/ext_modules/bacnet_client_1.11_dev/bacnet-stack-0.8.6/ports/linux/ )

forte_add_link_directories(${BACNET_LIB_DIR})
forte_add_link_library( libbacnet.a )
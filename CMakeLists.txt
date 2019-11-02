
forte_add_module(BACnet_Client_1.11dev "BACnet_Client_1.11dev")
SET(BACNET_LIB_DIR "" CACHE PATH "Path to Bacnet library directory ")

forte_add_sourcefile_hcpp(BACnetClient)
forte_add_sourcefile_hcpp(BACnetAdapter)
forte_add_sourcefile_hcpp(bacnet_client_controller)
forte_add_sourcefile_hcpp(bacnet_object)
forte_add_sourcefile_hcpp(bacnet_device_object)
forte_add_sourcefile_hcpp(BACnetReadProperty)
forte_add_sourcefile_hcpp(bacnet_service_config_fb)
forte_add_sourcefile_hcpp(bacnet_service_handle)
forte_add_sourcefile_hcpp(bacnet_readproperty_handle)

forte_add_include_directories( /home/adm1n/Desktop/bachelors_thesis/fordiac/ext_modules/bacnet_client_1.11_dev/bacnet-stack-0.8.6/include/ )
forte_add_include_directories( /home/adm1n/Desktop/bachelors_thesis/fordiac/ext_modules/bacnet_client_1.11_dev/bacnet-stack-0.8.6/ports/linux/ )
#forte_add_include_directories( /home/adm1n/Desktop/bachelors_thesis/fordiac/ext_modules/bacnet_client_1.11_dev/bacnet-stack-0.8.6/demo/object/ )

forte_add_link_directories(${BACNET_LIB_DIR})
forte_add_link_library( libbacnet.a )
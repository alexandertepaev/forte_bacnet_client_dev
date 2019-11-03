#ifndef _BACNET_DEVICE_OBJECT_H_
#define _BACNET_DEVICE_OBJECT_H_


#include "bacnet_object.h"

class CBacnetDeviceObject : public CBacnetObject
{
  public:
    CBacnetDeviceObject(uint32_t pa_nObjectId, char *pa_sObjectName);
    ~CBacnetDeviceObject();

  protected:
    virtual void init();

    // readProperty 
    
    /*
    Object_Identifier    BACnetObjectIdentifier
    Object_Name    CharacterString
    Object_Type    BACnetObjectType
    System_Status    BACnetDeviceStatus
    Vendor_Name    CharacterString
    Vendor_Identifier    Unsigned16
    Model_Name    CharacterString
    Firmware_Revision    CharacterString
    Application_Software_Version    CharacterString
    Protocol_Version Unsigned
    Protocol_Revision    Unsigned
    Protocol_Services_Supported    BACnetServicesSupported
    Protocol_Object_Types_Supported    BACnetObjectTypesSupported
    Object_List    BACnetARRAY[N] of BACnetObjectIdentifier
    Max_APDU_Length_Accepted Unsigned
    Segmentation_Supported BACnetSegmentation
    APDU_Timeout Unsigned
    Number_Of_APDU_Retries Unsigned
    Device_Address_Binding BACnetLIST of BACnetAddressBinding // device-identifier (BACnetObjectIdentifier) device-address (5-octets)
    Database_Revision Unsigned
    Property_List BACnetARRAY[N] of BACnetPropertyIdentifier
    */

  private:
};

#endif
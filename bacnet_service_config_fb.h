#ifndef _BACNET_SERVICE_CONFIG_FB_H_
#define _BACNET_SERVICE_CONFIG_FB_H_

#include"../../forte-incubation_1.11.0/src/core/io/configFB/io_master_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/device/io_controller_multi.h"
#include "../../forte-incubation_1.11.0/src/core/io/mapper/io_mapper.h"

class CBacnetServiceConfigFB
{
private:
  /* data */
public:
  CBacnetServiceConfigFB(/* args */);
  ~CBacnetServiceConfigFB();

  // rework
  struct ServiceConfig {
    uint32_t mDeviceId;
    uint32_t mObjectType;
    uint32_t mObjectId;
    uint32_t mObjectProperty;
    uint32_t mArrayIndex;
  };

};

#endif
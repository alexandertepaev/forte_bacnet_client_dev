#include "bacnet_object.h"


CBacnetObject::CBacnetObject(uint32_t pa_nObjectId, char *pa_sObjectName) : m_sObjectName(pa_sObjectName)
{
 m_nObjectId = (OBJECT_DEVICE << 22) | (pa_nObjectId & 0x3FFFFF); 
}

CBacnetObject::~CBacnetObject()
{
}
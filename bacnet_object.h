#include "include/bacnet.h"
#include <cstdint>

class CBacnetObject
{
  public:
    CBacnetObject(uint32_t pa_nObjectId, char *pa_sObjectName);
    ~CBacnetObject();

    int m_nObjectId;
    char *m_sObjectName;

  protected:
    virtual void init() = 0;

  private:
};
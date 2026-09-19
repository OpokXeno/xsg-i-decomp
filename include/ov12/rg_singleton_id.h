#ifndef INCLUDE_OV12_RG_SINGLETON_ID_H
#define INCLUDE_OV12_RG_SINGLETON_ID_H

#include "shared.h"

void *RgSingletonIDGet(unsigned int singleton_id);

extern void RgSingletonIDEntry(
    int singleton_id,
    RgSimpleDB *database,
    void (*destructor)(RgSimpleDB *database));

#endif /* INCLUDE_OV12_RG_SINGLETON_ID_H */

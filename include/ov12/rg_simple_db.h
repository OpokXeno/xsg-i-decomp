#ifndef INCLUDE_OV12_RG_SIMPLE_DB_H
#define INCLUDE_OV12_RG_SIMPLE_DB_H

#include "shared.h"

extern void DisposeRgSimpleDB(RgSimpleDB *database);

/* _InitDB stores capacity at +4 and entry_size at +8 of the descriptor. */
extern RgSimpleDB *CreateRgSimpleDB(int capacity, int entry_size);

#endif /* INCLUDE_OV12_RG_SIMPLE_DB_H */

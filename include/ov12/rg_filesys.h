#ifndef INCLUDE_OV12_RG_FILESYS_H
#define INCLUDE_OV12_RG_FILESYS_H

#include "shared.h"

/* The address of the file record's inline name buffer (+0x1C). */
char *RgFileSysDataGetName(RgFileSysData *pFile);

#endif /* INCLUDE_OV12_RG_FILESYS_H */

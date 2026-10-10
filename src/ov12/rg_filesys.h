/*
 * TU-local declarations of ov12/tu065 (src/ov12/rg_filesys.c).
 */

#ifndef SRC_OV12_RG_FILESYS_H
#define SRC_OV12_RG_FILESYS_H

#include "shared.h"

/* Shared RgFileSysData declarations are provided by include/shared.h.
 * struct RgFileSys itself remains local to rg_filesys.c. */

void RgFileSysPrepareFile(RgFileSys *pSys, const char *pszName,
                          const char *pszRoot);

extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);

/* The address of the file record's inline name buffer (+0x1C). */
char *RgFileSysDataGetName(RgFileSysData *pFile);

RgFileSys *InstanceOfRgFileSys(void);

RgFileSysData *RgFileSysDup(RgFileSys *pSys, const char *pszName,
                            const char *pszRoot);

RgFileSysData *RgFileSysOnMemory(RgFileSys *pSys, const char *pszName,
                                 void *pBuf, unsigned int nSize);

extern const char pSys_not_nil[];

extern const char rg_filesys_source_file[];

extern const char prepare_count_check[];

#endif /* SRC_OV12_RG_FILESYS_H */

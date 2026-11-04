#ifndef INCLUDE_OV12_RG_FILESYS_H
#define INCLUDE_OV12_RG_FILESYS_H

#include "shared.h"

/* Functions and data used by other translation units. */
RgFileSys *InstanceOfRgFileSys(void);
RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                             const char *pszRoot);
RgFileSysData *RgFileSysDup(RgFileSys *pSys, const char *pszName,
                            const char *pszRoot);
RgFileSysData *RgFileSysOnMemory(RgFileSys *pSys, const char *pszName,
                                 void *pBuf, unsigned int nSize);
void RgFileSysPrepareFile(RgFileSys *pSys, const char *pszName,
                          const char *pszRoot);
void RgFileSysDisposePrepares(RgFileSys *pSys);
void RgFileSysClear(RgFileSys *pSys);
void RgFileSysDump(RgFileSys *pSys);
void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *source_file,
                              int line);
char *RgFileSysDataGetName(RgFileSysData *pFile);

#endif /* INCLUDE_OV12_RG_FILESYS_H */

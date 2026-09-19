/*
 * TU-local declarations of ov12/tu065 (src/ov12/rg_filesys.c).
 */

#ifndef SRC_OV12_RG_FILESYS_H
#define SRC_OV12_RG_FILESYS_H

#include "shared.h"

/*
 * RgFileSys, RgFileSysData and struct RgFileSysData are this TU's types in
 * the accepted spelling of its original record (io ov12-00a34518). More than
 * one TU reads them, so tools/header_harvest.py publishes them in
 * include/shared.h; neither this header nor any other TU restates them.
 * struct RgFileSys itself stays in rg_filesys.c.
 */

void RgFileSysPrepareFile(RgFileSys *pSys, const char *pszName,
                          const char *pszRoot);

extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);

/* The address of the file record's inline name buffer (+0x1C). */
char *RgFileSysDataGetName(RgFileSysData *pFile);

extern const char pSys_not_nil[];

extern const char rg_filesys_source_file[];

extern const char prepare_count_check[];

#endif /* SRC_OV12_RG_FILESYS_H */

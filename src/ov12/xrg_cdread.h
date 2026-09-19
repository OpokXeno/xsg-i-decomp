/*
 * TU-local declarations of ov12/tu082 (src/ov12/xrg_cdread.c).
 */

#ifndef SRC_OV12_XRG_CDREAD_H
#define SRC_OV12_XRG_CDREAD_H

#include "shared.h"

extern int xglCdGetFileSize(const char *name);

extern unsigned int strlen(const char *string);

int XrgCdFileSize(const char *pszName);

int XrgCdFileRead(const char *pszName, void *pBuf);

int XrgCdFileAlignmentSize(int uSize);

extern const char root_name_name_check[];

extern const char root_name_source_file[];

extern const char root_name_result_check[];

char *_GetLocalPath(void);

#endif /* SRC_OV12_XRG_CDREAD_H */

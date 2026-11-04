#ifndef INCLUDE_OV12_RG_SIMPLE_DB_H
#define INCLUDE_OV12_RG_SIMPLE_DB_H

#include "shared.h"

/* Functions and data used by other translation units. */
RgSimpleDB *CreateRgSimpleDB(int capacity, int entry_size);
void DisposeRgSimpleDB(RgSimpleDB *pDB);
int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
int RgSimpleDBSize(RgSimpleDB *pDB);
void *RgSimpleDBGet(RgSimpleDB *pDB, int nDataID);
char *RgSimpleDBGetName(RgSimpleDB *pDB, int nDataID);
void RgSimpleDBEntry(RgSimpleDB *pDB, void *pDat, const char *pszName);
void RgSimpleDBClear(RgSimpleDB *pDB);
void RgSimpleDBDump(RgSimpleDB *pDB);

#endif /* INCLUDE_OV12_RG_SIMPLE_DB_H */

#ifndef INCLUDE_OV12_XRG_CDREAD_H
#define INCLUDE_OV12_XRG_CDREAD_H

/* Sectors on the disc are 2048 bytes; file buffers round up to whole sectors. */
#define XRG_CD_SECTOR_SIZE 0x800

/* Functions and data used by other translation units. */
int XrgCdFileSize(const char *pszName);
int XrgCdFileRead(const char *pszName, void *pBuf);
int XrgCdFileAlignmentSize(int uSize);
char *_GetLocalPath(void);

#endif /* INCLUDE_OV12_XRG_CDREAD_H */

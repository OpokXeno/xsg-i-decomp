#ifndef INCLUDE_OV12_XRG_FILESYS_MODE_H
#define INCLUDE_OV12_XRG_FILESYS_MODE_H

/* Where XrgFileSysAlloc takes file buffers from. */
#define XRG_FILESYS_MODE_DATA_HEAP 0
#define XRG_FILESYS_MODE_CODE_HEAP 1

/* Functions and data used by other translation units. */
void XrgFileSysPushMode(int eMode);
void XrgFileSysPopMode(void);
int XrgFileSysModeGet(void);
void *XrgFileSysAlloc(unsigned int uSize, const char *pszFile, int iLine);
void XrgFileSysFree(void *pBuf, const char *pszFile, int iLine);

#endif /* INCLUDE_OV12_XRG_FILESYS_MODE_H */

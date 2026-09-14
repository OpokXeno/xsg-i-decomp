/*
 * TU-local declarations of ov12/tu065 (src/ov12/rg_filesys.c).
 */

#ifndef SRC_OV12_RG_FILESYS_H
#define SRC_OV12_RG_FILESYS_H

typedef struct RgFileSys RgFileSys;

typedef struct RgFileSysData RgFileSysData;

struct RgFileSysData {
    void *data;
    unsigned int size;
    unsigned int mode;
    RgFileSys *owner;
    unsigned int ref_count;
};

void RgFileSysPrepareFile(RgFileSys *pSys, const char *pszName,
                          const char *pszRoot);

extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);

extern const char pSys_not_nil[];

extern const char rg_filesys_source_file[];

extern const char prepare_count_check[];

#endif /* SRC_OV12_RG_FILESYS_H */

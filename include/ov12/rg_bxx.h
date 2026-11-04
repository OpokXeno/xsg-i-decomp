#ifndef INCLUDE_OV12_RG_BXX_H
#define INCLUDE_OV12_RG_BXX_H

struct RgLinkData;
struct RgFileSysData;

/*
 * The texture archive handle, defined by src/ov12/rg_bxx.c. Other
 * translation units only store and forward the pointer.
 */
typedef struct RgBxx RgBxx;
typedef struct RgBxxPic RgBxxPic;

/*
 * CreateRgBxx_sub allocates exactly sizeof(RgBxx) == 0x2C bytes; _InitBxx
 * clears m_pLink/m_pLoad, copies the archive name into m_szName and stores
 * the caller's mode in m_nMode. The field names come from the original
 * assertion text ("pBxx->m_pLink != NIL", "pBxx->m_pLoad == NIL").
 */
struct RgBxx {
    struct RgLinkData *m_pLink;     /* +0x00 */
    struct RgFileSysData *m_pLoad;  /* +0x04 */
    char m_szName[0x20];            /* +0x08 */
    int m_nMode;                    /* +0x28 */
};

/*
 * The archive's decoded header, returned by _GetHeader as
 * RgLinkDataGetIndex(pBxx->m_pLink, 0). The picture table starts right
 * after it, at +0x30 (the original asserts sizeof(RgBxxHeader) == 48).
 */
typedef struct RgBxxHeader {
    unsigned char unmodeled_000[4];
    int numTex; /* +0x004 */
    unsigned char unmodeled_008[8];
    int width4;  /* +0x010: texture width in the 4 bpp layout */
    int width8;  /* +0x014 */
    int width16; /* +0x018 */
    int width24; /* +0x01C: also used for 32 bpp */
    unsigned int height; /* +0x020 */
    unsigned char unmodeled_024[0xC];
} RgBxxHeader;

/*
 * One picture record of the archive, 0x60 bytes each (the original asserts
 * sizeof(RgBxxPic) == 96). Names start at byte zero (_FindPicByName
 * compares them with strncmp); RgBxxSetData stores the owning archive at
 * +0x58.
 */
struct RgBxxPic {
    char name[0x20];                /* +0x00 */
    unsigned char unmodeled_020[4];
    int bpp;                        /* +0x24: bits per pixel, read by XrgBxxPs2Tex0 */
    unsigned char unmodeled_028[4];
    int x;                          /* +0x2C */
    int y;                          /* +0x30 */
    int width;                      /* +0x34 */
    int height;                     /* +0x38 */
    unsigned int paletteBase;       /* +0x3C */
    unsigned char unmodeled_040[0x58 - 0x40];
    RgBxx *pOwner;                  /* +0x58 */
    unsigned char unmodeled_05C[0x60 - 0x5C];
};

/* Functions and data used by other translation units. */
RgBxxHeader *RgBxxGetHeader(RgBxx *pBxx);
void *RgBxxGetXtx(RgBxx *pBxx);
RgBxx *CreateRgBxx_sub(const char *pszName, int nMode);
RgBxx *LoadRgBxx_sub(const char *pszName, const char *pszArchiveName,
                     int nMode);
void DisposeRgBxx_sub(RgBxx *pBxx, const char *pszFile, int nLine);
void RgBxxSetData(RgBxx *pBxx, void *pData);
int RgBxxGetTexNum(RgBxx *pBxx);
RgBxxPic *RgBxxGetPic(RgBxx *pBxx, const char *pszName);
RgBxxPic *RgBxxGetPicID(RgBxx *pBxx, unsigned int id);
int RgBxxGetFindPic(RgBxx *pBxx, const char *pszName);

#endif /* INCLUDE_OV12_RG_BXX_H */

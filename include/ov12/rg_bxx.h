#ifndef INCLUDE_OV12_RG_BXX_H
#define INCLUDE_OV12_RG_BXX_H

/*
 * Opaque texture-archive link-data handle owned by ov12/tu063
 * (src/ov12/rg_linkdata.c). This allocation only stores the pointer and
 * forwards it to InitRgLinkData/RgLinkDataNumOfData/RgLinkDataGetIndex (all
 * still INCLUDE_ASM there); it never reads a member itself, so an incomplete
 * type is enough here.
 */
typedef struct RgLinkData RgLinkData;

/*
 * The texture archive handle. CreateRgBxx_sub (ov12:0x00a3ebb0) allocates
 * exactly sizeof(RgBxx) == 0x2C bytes; _InitBxx (ov12:0x00a3ea10) clears
 * m_pLink/m_pLoad, copies the archive name into m_szName and stores the
 * caller's mode in m_nMode. The field names come from the original
 * assertion text ("pBxx->m_pLink != NIL", "pBxx->m_pLoad == NIL").
 */
struct RgBxx {
    RgLinkData *m_pLink;            /* +0x00 */
    struct RgFileSysData *m_pLoad;  /* +0x04 */
    char m_szName[0x20];            /* +0x08 */
    int m_nMode;                    /* +0x28 */
};

#endif /* INCLUDE_OV12_RG_BXX_H */

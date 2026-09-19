/*
 * TU-local declarations of ov12/tu073 (src/ov12/rg_bxx.c).
 */

#ifndef SRC_OV12_RG_BXX_H
#define SRC_OV12_RG_BXX_H

#include "shared.h"

/*
 * Opaque texture-archive link-data handle owned by ov12/tu063
 * (src/ov12/rg_linkdata.c). This allocation only stores the pointer and
 * forwards it to InitRgLinkData/RgLinkDataNumOfData/RgLinkDataGetIndex (all
 * still INCLUDE_ASM there); it never reads a member itself, so an incomplete
 * type is enough here.
 */
typedef struct RgLinkData RgLinkData;

/*
 * Several other TUs (rg_shot_effect.c, rg_piclist.h, rg_disp_wpn1p.h) forward
 * declare this tag locally and only ever hold it as an opaque pointer; this
 * TU is the owner and completes the body below.
 */
typedef struct RgBxx RgBxx;

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

/*
 * The texture archive's decoded header, returned by _GetHeader
 * (ov12:0x00a3eea8) as RgLinkDataGetIndex(pBxx->m_pLink, 0). numTex at +0x04
 * is the only field any function of this allocation reads (RgBxxGetTexNum,
 * RgBxxSetData, RgBxxGetPicID); the picture table _GetPicTop
 * (ov12:0x00a3ef10) returns starts exactly at +0x30 (sizeof(RgBxxHeader)),
 * so the struct ends there and nothing past it is claimed here.
 */
typedef struct RgBxxHeader {
    unsigned char unmodeled_000[4];
    int numTex; /* +0x004 */
    unsigned char unmodeled_008[0x28];
} RgBxxHeader;

/*
 * One picture record of the archive, 0x60 (96) bytes each (RgBxxGetPicID's
 * id * 0x60 and _FindPicByName's pointer step by 96). rg_help.h defines
 * this tag with a partial view (`height` at +0x38); RgBxxSetData
 * (ov12:0x00a3ed58) additionally evidences an owning-archive back-pointer
 * at +0x58, absent from that view, so the type stays incomplete here.
 */
typedef struct RgBxxPic RgBxxPic;

#endif /* SRC_OV12_RG_BXX_H */

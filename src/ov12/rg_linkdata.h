/*
 * TU-local declarations of ov12/tu063 (src/ov12/rg_linkdata.c).
 */

#ifndef SRC_OV12_RG_LINKDATA_H
#define SRC_OV12_RG_LINKDATA_H

/*
 * The field this allocation evidences: InitRgLinkData stores the caller's
 * buffer pointer at +0x00, and RgLinkDataMatchDataName reads it back to
 * compare the first four bytes of the buffer against a name. No other
 * function of this allocation touches this object, and the remaining
 * RgLinkData functions of this TU (RgLinkDataVersion, RgLinkDataTime,
 * RgLinkDataNumOfData, RgLinkDataGetName, RgLinkDataGetSize, RgLinkDataGet,
 * RgLinkDataFindExt, RgLinkDataGetIndex) are still INCLUDE_ASM, so nothing
 * past +0x00 is claimed here.
 */
typedef struct RgLinkData {
    void *buf; /* +0x00: InitRgLinkData, RgLinkDataMatchDataName */
} RgLinkData;

/*
 * The original literals at ov12:0x00a556c0 ("pAna != NIL"), 0x00a556d0
 * ("../rg_linkdata.euc.c") and 0x00a556e8 ("pBuf != NIL") are scaffold-owned
 * (config/tu-build.json data_ownership: this .rodata window is still owner
 * "asm") and have no entry in config/symbols/ov12.txt, so they keep their
 * splat names (docs/naming.md, "Scaffold-owned data keeps its splat name").
 */
extern const char D_00A556C0[];
extern const char D_00A556D0[];
extern const char D_00A556E8[];

#endif /* SRC_OV12_RG_LINKDATA_H */

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

/*
 * The archive header pAna->buf points at, read through _get_uint's
 * unaligned load (the same helper RgLinkDataMatchDataName's own raw buf
 * access already parallels for +0x00). RgLinkDataVersion, RgLinkDataTime and
 * RgLinkDataNumOfData decode version/numOfData/time; RgLinkDataGetName reads
 * nameTableOffset; RgLinkDataGetSize, RgLinkDataGet, RgLinkDataFindExt and
 * RgLinkDataGetIndex read offsetTableOffset. Nothing in this allocation
 * reads +0x0C.
 */
typedef struct RgLinkDataHeader {
    unsigned char unmodeled_000[4];
    unsigned char nameTableOffset[4];   /* +0x04: RgLinkDataGetName */
    unsigned char offsetTableOffset[4]; /* +0x08: RgLinkDataGetSize,
                                          * RgLinkDataGet, RgLinkDataFindExt,
                                          * RgLinkDataGetIndex */
    unsigned char unmodeled_00C[4];
    unsigned char version[4];   /* +0x10: RgLinkDataVersion */
    unsigned char numOfData[4]; /* +0x14: RgLinkDataNumOfData */
    unsigned char time[4];      /* +0x18: RgLinkDataTime */
} RgLinkDataHeader;

/*
 * One name-table record: RgLinkDataGetName steps by sizeof(RgLinkDataName)
 * (0x20 bytes) per index from the header's nameTableOffset.
 */
typedef char RgLinkDataName[0x20];

/*
 * One offset-table record: RgLinkDataGetSize, RgLinkDataGet and
 * RgLinkDataFindExt walk this table by sizeof(RgLinkDataOffset) (4 bytes)
 * from the header's offsetTableOffset, and RgLinkDataGetIndex indexes it
 * directly by argument. Each decoded value is a byte offset from the
 * header; RgLinkDataGetSize subtracts one entry from the next entry to
 * get a payload's size.
 */
typedef unsigned char RgLinkDataOffset[4];

/*
 * The literals at ov12:0x00a55708 ("RgLinkDataVersion(pAna) >= 2"),
 * 0x00a556f8 ("uNum > nIndex") and 0x00a55728 ("uNum > uIndex") are
 * scaffold-owned splat data in the same window as D_00A556C0 above.
 */
extern const char D_00A55708[];
extern const char D_00A556F8[];
extern const char D_00A55728[];

#endif /* SRC_OV12_RG_LINKDATA_H */
